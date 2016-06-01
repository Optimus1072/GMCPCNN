//
// Created by wrede on 25.05.16.
//

#include "KShortestPaths.h"
#include "../util/Logger.h"
#include "../util/FileIO.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/copy.hpp>

namespace algo
{
    KShortestPaths::KShortestPaths(DirectedGraph graph, Vertex source, Vertex sink)
            : original_graph_(graph), source_(source), sink_(sink)
    {
    }

    MultiPredecessorMap KShortestPaths::Run(size_t max_path_count)
    {
        util::Logger::LogInfo("Running k-shortest-paths");

        // Clear all data from previous runs
        i_shortest_paths_.clear();

        util::Logger::LogDebug("find shortest path #1");

        FindAndAugmentPath(1, true);

        // Find the k shortest paths
        for (size_t i = 1; i < max_path_count; ++i)
        {
            util::Logger::LogDebug("find shortest path #" + std::to_string(i + 1));

            if (i != 1)
            {
                //If the costs are increasing, no further shortest paths will be found
                if (OverallCosts(i) > OverallCosts(i - 1))
                {
                    return i_shortest_paths_[i];
                }
            }

            // Create a copy of the original graph which is used
            // for the graph and edge transformations
            CopyOriginalGraph();

            // Extend the graph (has negative and positive edge weights)
            ExtendGraph(i);

            //TODO (optional) Transform the edge weights (positive edge weights) and use dijkstra

            // Finds the next path and adds it to the found paths
            FindAndAugmentPath(i + 1, false);

            // Removes the duplicate edges to make the paths node-disjoint
            Interlace(i + 1);
        }

        // All k shortest paths have been found
        return i_shortest_paths_[max_path_count];
    }

    void KShortestPaths::ExtendGraph(size_t i)
    {
        boost::graph_traits<DirectedGraph>::out_edge_iterator oei, oei_end;
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        VertexValueMap graph_values = boost::get(boost::vertex_name, copied_graph_);

        // Split all vertices within the paths
        for (boost::tie(vi, vi_end) = boost::vertices(original_graph_); vi != vi_end; ++vi)
        {
            Vertex v_original = (*vi);

            // Ignore vertices off the paths, the source and the sink
            if (i_shortest_paths_[i].count(v_original) == 0 || v_original == source_ || v_original == sink_)
            {
                continue;
            }

            Vertex v_in = original_to_copy_[v_original];
            Vertex v_out = boost::add_vertex(graph_values[v_in], copied_graph_);
            copy_to_original_[v_out] = v_original;

            // Copy outgoing edges to v_out
            for (boost::tie(oei, oei_end) = boost::out_edges(v_in, copied_graph_); oei != oei_end; ++oei)
            {
                QueueAddEdge(v_out, boost::target(*oei, copied_graph_),
                             boost::get(boost::edge_weight, copied_graph_, *oei));
            }

            // Remove outgoing edges from vertex
            for (boost::tie(oei, oei_end) = boost::out_edges(v_in, copied_graph_); oei != oei_end; ++oei)
            {
                QueueRemoveEdge(v_in, boost::target(*oei, copied_graph_));
            }

            // Create auxiliary edge
            QueueAddEdge(v_in, v_out, 0.0);
        }

        UpdateEdges();

        // Iterate all vertices within the copied graph
        for (boost::tie(vi, vi_end) = boost::vertices(copied_graph_); vi != vi_end; ++vi)
        {
            Vertex v_copy = (*vi);
            Vertex v_original = copy_to_original_[v_copy];

            // Ignore vertices off the paths
            if (i_shortest_paths_[i].count(v_original) == 0)
            {
                continue;
            }

            // Iterate all outgoing edges at the current vertex
            for (boost::tie(oei, oei_end) = boost::out_edges(v_copy, copied_graph_); oei != oei_end; ++oei)
            {
                Vertex t_copy = boost::target(*oei, copied_graph_);
                Vertex t_original = copy_to_original_[t_copy];

                // Ignore edges off the paths
                if (i_shortest_paths_[i].count(t_original) == 0)
                {
                    continue;
                }

                // Invert the edge direction and weight
                double weight = boost::get(boost::edge_weight, copied_graph_,
                                           *oei);
                QueueAddEdge(t_copy, v_copy, -weight);
                QueueRemoveEdge(v_copy, t_copy);
            }
        }

        UpdateEdges();
    }

    double KShortestPaths::OverallCosts(size_t i)
    {
        EdgeWeightMap weights = boost::get(boost::edge_weight, original_graph_);
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        double value = 0.0;

        for (boost::tie(ei, ei_end) = boost::edges(original_graph_);
             ei != ei_end; ++ei)
        {
            Vertex source = boost::source(*ei, original_graph_);
            Vertex target = boost::target(*ei, original_graph_);

            // Is edge within paths?
            if (i_shortest_paths_[i].count(target) > 0 && i_shortest_paths_[i][target].count(source) > 0)
            {
                value += weights[*ei];
            }
        }

        return value;
    }

    void KShortestPaths::CopyOriginalGraph()
    {
        // Clear all previous data
        copied_graph_.clear();
        original_to_copy_.clear();
        copy_to_original_.clear();

        // Copy the graph and store the vertex map temporarily
        std::vector<Vertex> original_to_copy_vector(boost::num_vertices(original_graph_));
        VertexVertexMap temp_map(original_to_copy_vector.begin());
        boost::copy_graph(original_graph_,
                          copied_graph_,
                          boost::orig_to_copy(temp_map));

        // Copy the vertex map into the two persistent maps, one for each
        // mapping direction
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(original_graph_);
             vi != vi_end; ++vi)
        {
            Vertex v_original = (*vi);
            Vertex v_copy = temp_map[v_original];

            original_to_copy_[v_original] = v_copy;
            copy_to_original_[v_copy] = v_original;
        }
    }

    void KShortestPaths::Interlace(size_t i)
    {
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(original_graph_);
             ei != ei_end; ++ei)
        {
            Vertex source = boost::source(*ei, original_graph_);
            Vertex target = boost::target(*ei, original_graph_);

            // Ignore source and sink
            if (source == source_ || target == sink_)
            {
                continue;
            }

            // Is edge within paths?
            if (i_shortest_paths_[i].count(target) > 0 && i_shortest_paths_[i][target].count(source) > 0)
            {
                // Is edge duplicate?
                if (i_shortest_paths_[i].count(source) > 0 && i_shortest_paths_[i][source].count(target) > 0)
                {
                    i_shortest_paths_[i][target].erase(source);
                    i_shortest_paths_[i][source].erase(target);
                }
            }
        }
    }

    void KShortestPaths::FindAndAugmentPath(size_t i, bool original)
    {
        // Add new path maps until the needed iteration is reached
        while (i_shortest_paths_.size() < (i + 1))
        {
            i_shortest_paths_.push_back(MultiPredecessorMap());
        }

        // Only copy old paths if old paths exist
        if (i > 0)
        {
            // Copy the old paths
            for (auto it = i_shortest_paths_[i - 1].begin();
                 it != i_shortest_paths_[i - 1].end(); ++it)
            {
                i_shortest_paths_[i][(*it).first] = (*it).second;
            }
        }

        if (original)
        {
            // Prepare variables for path finding
            size_t graph_size = boost::num_vertices(original_graph_);
            std::vector<Vertex> pred_list(graph_size);
            std::vector<double> dist_list(graph_size);
            VertexIndexMap graph_indices = boost::get(boost::vertex_index,
                                                      original_graph_);
            PredecessorMap pred_map(&pred_list[0], graph_indices);
            DistanceMap dist_map(&dist_list[0], graph_indices);

            // Find the shortest path
            boost::dijkstra_shortest_paths(original_graph_,
                                           source_,
                                           boost::predecessor_map(pred_map)
                                                   .distance_map(dist_map));

            // Add the new path
            for (Vertex u = sink_, v = pred_map[u];
                 u != v; u = v, v = pred_map[v])
            {
                i_shortest_paths_[i][u].insert(v);
            }
        }
        else
        {
            // Prepare variables for path finding
            size_t graph_size = boost::num_vertices(copied_graph_);
            Vertex root_vertex = original_to_copy_[source_];
            std::vector<Vertex> pred_list(graph_size);
            std::vector<double> dist_list(graph_size);
            VertexIndexMap graph_indices = boost::get(boost::vertex_index,
                                                      copied_graph_);
            EdgeWeightMap weight_map = boost::get(boost::edge_weight,
                                                     copied_graph_);
            PredecessorMap pred_map(&pred_list[0], graph_indices);
            DistanceMap dist_map(&dist_list[0], graph_indices);

            // Find the shortest path
            boost::bellman_ford_shortest_paths(copied_graph_,
                                               graph_size,
                                               boost::root_vertex(root_vertex)
                                                       .weight_map(weight_map)
                                                       .predecessor_map(pred_map)
                                                       .distance_map(dist_map));

            // Add the new path (the given path is in the copied graph, so the
            // vertices need to be mapped to the original graph)
            Vertex sink_copy = original_to_copy_[sink_];
            for (Vertex u_copy = sink_copy, v_copy = pred_map[u_copy];
                 u_copy != v_copy; u_copy = v_copy, v_copy = pred_map[v_copy])
            {
                Vertex u_original = copy_to_original_[u_copy];
                Vertex v_original = copy_to_original_[v_copy];

                // Ignore loops
                if (u_original == v_original)
                {
                    continue;
                }

                i_shortest_paths_[i][u_original].insert(v_original);
            }
        }
    }

    void KShortestPaths::UpdateEdges()
    {
        // Remove the old edges, needs to be done without any iterator since
        // the removal invalidates any iterators
        for (auto edge : edges_to_remove_)
        {
            boost::remove_edge(edge.first, edge.second, copied_graph_);
        }
        edges_to_remove_.clear();

        // Add the new edges, needs to be done without any iterator since
        // the addition invalidates any iterators
        for (auto edge : edges_to_add_)
        {
            boost::add_edge(edge.first.first, edge.first.second,
                            edge.second, copied_graph_);
        }
        edges_to_add_.clear();
    }

    void KShortestPaths::QueueAddEdge(Vertex source, Vertex target, double weight)
    {
        edges_to_add_.push_back(
                std::pair<std::pair<Vertex, Vertex>, double>(
                        std::pair<Vertex, Vertex>(source, target), weight));
    }

    void KShortestPaths::QueueRemoveEdge(Vertex source, Vertex target)
    {
        edges_to_remove_.push_back(std::pair<Vertex, Vertex>(source, target));
    }
}

