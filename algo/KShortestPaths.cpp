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

        util::Logger::LogDebug("find shortest path #0");

        util::Logger::LogDebug("find");
        FindAndAugmentPath(0, true);

        // Find the k shortest paths
        for (size_t i = 0; i < max_path_count - 1; ++i)
        {
            if (i > 0)
            {
                // If the costs are increasing, no further shortest paths will
                // be found
                if (OverallCosts(i) > OverallCosts(i - 1))
                {
                    return i_shortest_paths_[i];
                }
            }

            util::Logger::LogDebug("find shortest path #" + std::to_string(i + 1));

            util::Logger::LogDebug("copy");
            // Create a copy of the original graph which is used
            // for the graph and edge transformations
            CopyOriginalGraph();

            util::FileIO::WriteCSVMatlab(copied_graph_,
                                         "/home/wrede/Dokumente/graph_" +
                                         std::to_string(i) + "_c.csv");

            util::Logger::LogDebug("extend");
            // Extend the graph (has negative and positive edge weights)
            ExtendGraph(i);

            util::FileIO::WriteCSVMatlab(copied_graph_,
                                         "/home/wrede/Dokumente/graph_" +
                                         std::to_string(i) + "_e.csv");

            util::Logger::LogDebug("transform");
            // Transforms the edge costs (has only positive edge weights)
            if (i > 0)
            {
                TransformEdgeCosts(i - 1, false);
            }
            else
            {
                TransformEdgeCosts(0, true);
            }

            util::FileIO::WriteCSVMatlab(copied_graph_,
                                         "/home/wrede/Dokumente/graph_" +
                                         std::to_string(i) + "_t.csv");

            util::Logger::LogDebug("find");
            // Finds the next path and adds it to the found paths
            FindAndAugmentPath(i + 1, false);

            util::Logger::LogDebug("interlace");
            // Removes the duplicate edges to make the paths node-disjoint
            Interlace(i + 1);
        }

        // All k shortest paths have been found
        return i_shortest_paths_[max_path_count - 1];
    }

    void KShortestPaths::ExtendGraph(size_t i)
    {
        boost::graph_traits<DirectedGraph>::out_edge_iterator oei, oei_end;
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        VertexValueMap graph_values = boost::get(boost::vertex_name, copied_graph_);

        // Split all vertices within the paths
        for (boost::tie(vi, vi_end) = boost::vertices(original_graph_);
             vi != vi_end; ++vi)
        {
            // Create a copy of each vertex for easy index matching with
            // preceding and succeeding iterations
            Vertex v_original = (*vi);
            Vertex v_in = original_to_copy_[v_original];
            Vertex v_out = boost::add_vertex(graph_values[v_in], copied_graph_);

            // Ignore vertices off the paths, the source and the sink
            if (i_shortest_paths_[i].count(v_original) == 0 ||
                    v_original == source_ || v_original == sink_)
            {
                continue;
            }

            copy_to_original_[v_out] = v_original;

            // Copy outgoing edges to v_out
            for (boost::tie(oei, oei_end) = boost::out_edges(v_in, copied_graph_);
                 oei != oei_end; ++oei)
            {
                QueueAddEdge(v_out, boost::target(*oei, copied_graph_),
                             boost::get(boost::edge_weight, copied_graph_, *oei));
            }

            // Remove outgoing edges from vertex
            for (boost::tie(oei, oei_end) = boost::out_edges(v_in, copied_graph_);
                 oei != oei_end; ++oei)
            {
                QueueRemoveEdge(v_in, boost::target(*oei, copied_graph_));
            }

            // Create auxiliary edge (inverted -> won't be iterated in the next step)
            QueueAddEdge(v_out, v_in, 0.0);
        }

        UpdateEdges();

        // Iterate all edges within the copied graph
        EdgeWeightMap weights = boost::get(boost::edge_weight, copied_graph_);
        for (boost::tie(ei, ei_end) = boost::edges(copied_graph_);
             ei != ei_end; ++ei)
        {
            Vertex s_copy = boost::source(*ei, copied_graph_);
            Vertex t_copy = boost::target(*ei, copied_graph_);
            Vertex s_orig = copy_to_original_[s_copy];
            Vertex t_orig = copy_to_original_[t_copy];
            double weight = weights[*ei];

            // If the edge is part of the paths
            if (i_shortest_paths_[i].count(t_orig) > 0 &&
                    i_shortest_paths_[i][t_orig].count(s_orig) > 0)
            {
                // Add the edge with direction and weight inverted
                QueueAddEdge(t_copy, s_copy, -weight);

                // Remove the old edge
                QueueRemoveEdge(s_copy, t_copy);
            }
        }

        UpdateEdges();

        util::Logger::LogDebug("num vertices original " + std::to_string(boost::num_vertices(original_graph_)));
        util::Logger::LogDebug("num vertices copy " + std::to_string(boost::num_vertices(copied_graph_)));
    }

    double KShortestPaths::OverallCosts(size_t i)
    {
        double value = 0.0;

        EdgeWeightMap weights = boost::get(boost::edge_weight, original_graph_);

        // Iterate all edges
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(original_graph_);
             ei != ei_end; ++ei)
        {
            Vertex source = boost::source(*ei, original_graph_);
            Vertex target = boost::target(*ei, original_graph_);

            if (i_shortest_paths_[i].count(target) > 0 &&
                    i_shortest_paths_[i][target].count(source) > 0)
            {
                value += weights[*ei];
            }
        }

//        for (size_t cost_i = 0; cost_i <= i; ++cost_i)
//        {
//            value += i_distances_[cost_i][sink_];
//        }

        util::Logger::LogDebug("cost in " + std::to_string(i) + " : " + std::to_string(value));

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
            if (i_shortest_paths_[i].count(target) > 0 &&
                    i_shortest_paths_[i][target].count(source) > 0)
            {
                // Is edge duplicate?
                if (i_shortest_paths_[i].count(source) > 0 &&
                        i_shortest_paths_[i][source].count(target) > 0)
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

        // Add new distance maps until the needed iteration is reached
        while (i_distances_.size() < (i + 1))
        {
            i_distances_.push_back(std::unordered_map<Vertex, double>());
        }

        // Only copy old paths if old paths exist
        if (i > 0)
        {
            // Copy the old paths
            for (auto it = i_shortest_paths_[i - 1].begin();
                 it != i_shortest_paths_[i - 1].end(); ++it)
            {
                i_shortest_paths_[i][it->first] = it->second;
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
            EdgeWeightMap weight_map = boost::get(boost::edge_weight,
                                                  original_graph_);
            PredecessorMap pred_map(&pred_list[0], graph_indices);
            DistanceMap dist_map(&dist_list[0], graph_indices);

            // Find the shortest path
            boost::bellman_ford_shortest_paths(original_graph_,
                                               graph_size,
                                               boost::root_vertex(source_)
                                                       .weight_map(weight_map)
                                                       .predecessor_map(pred_map)
                                                       .distance_map(dist_map));

            // Add the new distances
            boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
            for (boost::tie(vi, vi_end) = boost::vertices(original_graph_);
                 vi != vi_end; ++vi)
            {
                i_distances_[i][*vi] = dist_map[*vi];
            }

            // Add the new path
            size_t path_length = 0;
            for (Vertex u = sink_, v = pred_map[u];
                 u != v; u = v, v = pred_map[v])
            {
                i_shortest_paths_[i][u].insert(v);

                ++path_length;
            }

            util::Logger::LogDebug("path length " + std::to_string(path_length));
        }
        else
        {
            // Prepare variables for path finding
            size_t graph_size = boost::num_vertices(copied_graph_);
            std::vector<Vertex> pred_list(graph_size);
            std::vector<double> dist_list(graph_size);
            VertexIndexMap graph_indices = boost::get(boost::vertex_index,
                                                      copied_graph_);
            PredecessorMap pred_map(&pred_list[0], graph_indices);
            DistanceMap dist_map(&dist_list[0], graph_indices);

            // Find the shortest path
            boost::dijkstra_shortest_paths(copied_graph_,
                                           original_to_copy_[source_],
                                           boost::predecessor_map(pred_map)
                                                   .distance_map(dist_map));

            util::Logger::LogDebug("add the path");

            // Add the new distances
            boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
            for (boost::tie(vi, vi_end) = boost::vertices(copied_graph_);
                 vi != vi_end; ++vi)
            {
                Vertex v_copy = *vi;

                i_distances_[i][v_copy] = dist_map[v_copy];
            }

            // Add the new path (the given path is in the copied graph, so the
            // vertices need to be mapped to the original graph)
            size_t path_length = 0;
            Vertex sink_copy = original_to_copy_[sink_];
            std::cout << sink_;
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

                std::cout << "->" << v_original;
                i_shortest_paths_[i][u_original].insert(v_original);

                ++path_length;
            }
            std::cout << std::endl;

            util::Logger::LogDebug("path length " + std::to_string(path_length));
        }

        // Add source
        i_shortest_paths_[i][source_].insert(source_);
    }

    void KShortestPaths::TransformEdgeCosts(size_t i, bool original)
    {
        EdgeWeightMap weights = boost::get(boost::edge_weight, copied_graph_);
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;

        if (original)
        {
            for (boost::tie(ei, ei_end) = boost::edges(copied_graph_);
                 ei != ei_end; ++ei)
            {
                Vertex s_copy = boost::source(*ei, copied_graph_);
                Vertex t_copy = boost::target(*ei, copied_graph_);
                Vertex s_orig = copy_to_original_[s_copy];
                Vertex t_orig = copy_to_original_[t_copy];

                if (i_distances_[i].count(s_orig) > 0 &&
                        i_distances_[i].count(t_orig) > 0)
                {
                    weights[*ei] +=
                            i_distances_[i][s_orig] - i_distances_[i][t_orig];
                    util::Logger::LogDebug(std::to_string(weights[*ei]));
                }
            }
        }
        else
        {
            for (boost::tie(ei, ei_end) = boost::edges(copied_graph_);
                 ei != ei_end; ++ei)
            {
                Vertex s_copy = boost::source(*ei, copied_graph_);
                Vertex t_copy = boost::target(*ei, copied_graph_);

                if (i_distances_[i].count(s_copy) > 0 &&
                        i_distances_[i].count(t_copy) > 0)
                {
                    weights[*ei] +=
                            i_distances_[i][s_copy] - i_distances_[i][t_copy];
                    util::Logger::LogDebug(std::to_string(weights[*ei]));
                }
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

