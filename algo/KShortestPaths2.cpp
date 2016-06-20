//
// Created by wrede on 15.06.16.
//

#include <boost/graph/copy.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "KShortestPaths2.h"
#include "../util/Logger.h"
#include "../util/FileIO.h"

namespace algo
{
    void KShortestPaths2::CreateCopy()
    {
        util::Logger::LogDebug("create copy");

        // Clear all previous data
        graph_copy_.clear();
        orig_to_copy_.clear();
        copy_to_orig_.clear();
        in_to_out_.clear();
        out_to_in_.clear();

        // Copy the graph and store the vertex map temporarily
        std::vector<Vertex> original_to_copy_vector(boost::num_vertices(graph_orig_));
        VertexVertexMap temp_map(original_to_copy_vector.begin());
        boost::copy_graph(graph_orig_, graph_copy_, boost::orig_to_copy(temp_map));

        // Copy the vertex map into the two persistent maps, one for each
        // mapping direction
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph_orig_);
             vi != vi_end; ++vi)
        {
            Vertex v_original = (*vi);
            Vertex v_copy = temp_map[v_original];

            orig_to_copy_[v_original] = v_copy;
            copy_to_orig_[v_copy] = v_original;
        }

        // Split every vertex in the copied graph
        for (boost::tie(vi, vi_end) = boost::vertices(graph_orig_);
             vi != vi_end; ++vi)
        {
            Vertex v_orig = *vi;
            Vertex v_copy_in = orig_to_copy_[v_orig];
            Vertex v_copy_out = boost::add_vertex(graph_copy_);

            in_to_out_[v_copy_in] = v_copy_out;
            out_to_in_[v_copy_out] = v_copy_in;
            copy_to_orig_[v_copy_out] = v_orig;
        }
    }

    void KShortestPaths2::UpdateEdges()
    {
        // Remove the old edges, needs to be done without any iterator since
        // the removal invalidates any iterators
        for (auto edge : edges_to_remove_)
        {
            boost::remove_edge(edge.first, edge.second, graph_copy_);
        }
        edges_to_remove_.clear();

        // Add the new edges, needs to be done without any iterator since
        // the addition invalidates any iterators
        for (auto edge : edges_to_add_)
        {
            boost::add_edge(edge.first.first, edge.first.second,
                            edge.second, graph_copy_);
        }
        edges_to_add_.clear();
    }

    void KShortestPaths2::QueueAddEdge(Vertex source, Vertex target, double weight)
    {
        edges_to_add_.push_back(
                std::pair<std::pair<Vertex, Vertex>, double>(
                        std::pair<Vertex, Vertex>(source, target), weight));
    }

    void KShortestPaths2::QueueRemoveEdge(Vertex source, Vertex target)
    {
        edges_to_remove_.push_back(std::pair<Vertex, Vertex>(source, target));
    }

    void KShortestPaths2::QueueCopyEdges()
    {
        EdgeWeightMap weights = boost::get(boost::edge_weight, graph_orig_);
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph_orig_);
             ei != ei_end; ++ei)
        {
            Vertex s_orig = boost::source(*ei, graph_orig_);
            Vertex t_orig = boost::target(*ei, graph_orig_);
            Vertex s_copy = orig_to_copy_[s_orig];
            Vertex t_copy = orig_to_copy_[t_orig];
            double weight = weights[*ei];
            QueueAddEdge(s_copy, t_copy, weight);
        }
    }

    void KShortestPaths2::QueueRemoveAll()
    {
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph_copy_);
             ei != ei_end; ++ei)
        {
            Vertex s_copy = boost::source(*ei, graph_copy_);
            Vertex t_copy = boost::target(*ei, graph_copy_);
            QueueRemoveEdge(s_copy, t_copy);
        }
    }

    MultiPredecessorMap KShortestPaths2::Run(DirectedGraph& graph_orig,
                                             Vertex source, Vertex sink,
                                             size_t iterations)
    {
        // Clear all previous data
        edges_to_add_.clear();
        edges_to_remove_.clear();
        i_distances_.clear();
        i_paths_.clear();

        graph_orig_ = graph_orig;
        source_ = source;
        sink_ = sink;

        CreateCopy();
        FindAndAugment(0);
        for (size_t i = 0; i < iterations; ++i)
        {
            util::Logger::LogDebug("ksp iteration: " + std::to_string(i));

            if (i > 0)
            {
                if (PathCosts(i) >= PathCosts(i - 1))
                {
                    return i_paths_[i];
                }
            }

            ExtendGraph(i);
            i > 0 ? TransformEdges(i - 1) : TransformEdges(0);
            FindAndAugment(i + 1);
            Interlace(i + 1);
        }

        return i_paths_[iterations];
    }

    void KShortestPaths2::ExtendGraph(size_t iteration)
    {
        util::Logger::LogDebug("extend graph iteration: " + std::to_string(iteration));

        // Clear all previous working data
        util::Logger::LogDebug("remove all");
        QueueRemoveAll();
        util::Logger::LogDebug("copy all");
        QueueCopyEdges();
        util::Logger::LogDebug("update edges");
        UpdateEdges();

        util::Logger::LogDebug("iterate vertices");
        MultiPredecessorMap& paths = i_paths_[iteration];
        boost::graph_traits<DirectedGraph>::out_edge_iterator oei, oei_end;
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        EdgeWeightMap weights = boost::get(boost::edge_weight, graph_copy_);

        // Iterate all vertices within the original graph
        for (boost::tie(vi, vi_end) = boost::vertices(graph_orig_);
             vi != vi_end; ++vi)
        {
            Vertex v_orig = *vi;

            // Ignore vertices off the paths, the source and the sink
            if (paths.count(v_orig) == 0 || v_orig == source_ || v_orig == sink_)
            {
                continue;
            }

            Vertex v_copy_in = orig_to_copy_[v_orig];
            Vertex v_copy_out = in_to_out_[v_copy_in];

            // Iterate all input vertices
            for (boost::tie(oei, oei_end) = boost::out_edges(v_copy_in, graph_copy_);
                 oei != oei_end; ++oei)
            {
                // Copy the output edges to the output vertices
                QueueAddEdge(v_copy_out,
                             boost::target(*oei, graph_copy_),
                             weights[*oei]);

                // Remove the old output edge
                QueueRemoveEdge(v_copy_in, boost::target(*oei, graph_copy_));
            }

            // Create auxiliary edge
            // Is already inverted, because it will not be iterated later
            QueueAddEdge(v_copy_out, v_copy_in, 0.0);
        }

        util::Logger::LogDebug("update edges");
        UpdateEdges();

        util::Logger::LogDebug("iterate edges");
        // Iterate all edges within the copied graph
        weights = boost::get(boost::edge_weight, graph_copy_);
        for (boost::tie(ei, ei_end) = boost::edges(graph_copy_);
             ei != ei_end; ++ei)
        {
            Vertex s_copy = boost::source(*ei, graph_copy_);
            Vertex t_copy = boost::target(*ei, graph_copy_);
            Vertex s_orig = copy_to_orig_[s_copy];
            Vertex t_orig = copy_to_orig_[t_copy];
            double weight = weights[*ei];

            // If the edge is part of the paths
            if (paths.count(t_orig) > 0 && paths[t_orig].count(s_orig) > 0)
            {
                // Add the edge with direction and weight inverted
                QueueAddEdge(t_copy, s_copy, -weight);

                // Remove the old edge
                QueueRemoveEdge(s_copy, t_copy);
            }
        }

        util::Logger::LogDebug("update edges");
        UpdateEdges();
    }

    void KShortestPaths2::TransformEdges(size_t iteration)
    {
        util::Logger::LogDebug("transform edges iteration: " + std::to_string(iteration));

        std::unordered_map<Vertex, double>& distances = i_distances_[iteration];

        EdgeWeightMap weights = boost::get(boost::edge_weight, graph_copy_);
        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph_copy_);
             ei != ei_end; ++ei)
        {
            Vertex s_copy = boost::source(*ei, graph_copy_);
            Vertex t_copy = boost::target(*ei, graph_copy_);

            // Transform the edge weights with the distances calculated in
            // the specified path finding iteration
            if (distances.count(s_copy) > 0 && distances.count(t_copy) > 0)
            {
                //TODO set all edges to source to zero (experimental)
                if (t_copy == orig_to_copy_[source_]) {
                    weights[*ei] = 0.0;
                    continue;
                }

                // check if the vertex was found in previous iterations
                if (distances[s_copy] == std::numeric_limits<double>::max() ||
                        distances[t_copy] == std::numeric_limits<double>::max())
                {
                    weights[*ei] = std::numeric_limits<double>::infinity();
                }
                else
                {
                    weights[*ei] += distances[s_copy] - distances[t_copy];
                }
            }

            // Correct rounding errors
            if (weights[*ei] >= -0.0000001 && weights[*ei] <= 0.0000001)
            {
                weights[*ei] = 0.0;
            }

            //TODO debug output (experimental)
            if (weights[*ei] < 0.0)
            {
                util::Logger::LogDebug(std::to_string(s_copy)
                                       + " -> " + std::to_string(weights[*ei])
                                       + " -> " + std::to_string(t_copy));
            }
        }

//        util::FileIO::WriteCSVMatlab(graph_copy_, "/home/wrede/Dokumente/graph_t.csv");
    }

    void KShortestPaths2::FindAndAugment(size_t iteration)
    {
        util::Logger::LogDebug("find and augment iteration: " + std::to_string(iteration));

        // Add new path maps until the needed iteration is reached
        while (i_paths_.size() < (iteration + 1))
        {
            i_paths_.push_back(MultiPredecessorMap());
        }

        // Add new distance maps until the needed iteration is reached
        while (i_distances_.size() < (iteration + 1))
        {
            i_distances_.push_back(std::unordered_map<Vertex, double>());
        }

        // Only copy old paths if old paths exist
        if (iteration > 0)
        {
            // Copy the old paths
            for (auto it = i_paths_[iteration - 1].begin();
                 it != i_paths_[iteration - 1].end(); ++it)
            {
                i_paths_[iteration][it->first] = it->second;
            }
        }

        // Prepare variables for path finding
        size_t graph_size = boost::num_vertices(graph_copy_);
        Vertex root_vertex = orig_to_copy_[source_];
        std::vector<Vertex> pred_list(graph_size);
        std::vector<double> dist_list(graph_size);
        VertexIndexMap graph_indices = boost::get(boost::vertex_index,
                                                  graph_copy_);
        EdgeWeightMap weight_map = boost::get(boost::edge_weight,
                                              graph_copy_);
        PredecessorMap pred_map(&pred_list[0], graph_indices);
        DistanceMap dist_map(&dist_list[0], graph_indices);

        // Find the shortest path
        if (iteration == 0)
        {
            boost::bellman_ford_shortest_paths(graph_copy_,
                                               graph_size,
                                               boost::root_vertex(root_vertex)
                                                       .weight_map(weight_map)
                                                       .predecessor_map(pred_map)
                                                       .distance_map(dist_map));
        }
        else
        {
            boost::dijkstra_shortest_paths(graph_copy_,
                                           root_vertex,
                                           boost::predecessor_map(pred_map)
                                                   .distance_map(dist_map));
        }

        // Add the new distances
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph_copy_);
             vi != vi_end; ++vi)
        {
            i_distances_[iteration][*vi] = dist_map[*vi];
        }

        // Add the new path (the given path is in the copied graph, so the
        // vertices need to be mapped to the original graph)
        Vertex sink_copy = orig_to_copy_[sink_];
        for (Vertex u_copy = sink_copy, v_copy = pred_map[u_copy];
             u_copy != v_copy; u_copy = v_copy, v_copy = pred_map[v_copy])
        {
            Vertex u_orig = copy_to_orig_[u_copy];
            Vertex v_orig = copy_to_orig_[v_copy];

            // Ignore loops
            // Loops are caused by mapping the input and output vertices in the
            // copied graph to the same vertex in the original graph
            if (u_orig == v_orig)
            {
                continue;
            }

            i_paths_[iteration][u_orig].insert(v_orig);
        }
    }

    void KShortestPaths2::Interlace(size_t iteration)
    {
        util::Logger::LogDebug("interlace iteration: " + std::to_string(iteration));

        MultiPredecessorMap& paths = i_paths_[iteration];

        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph_orig_);
             ei != ei_end; ++ei)
        {
            Vertex s_orig = boost::source(*ei, graph_orig_);
            Vertex t_orig = boost::target(*ei, graph_orig_);

            // Ignore source and sink
            if (s_orig == source_ || t_orig == sink_)
            {
                continue;
            }

            // Is edge within paths?
            if (paths.count(t_orig) > 0 && paths.count(s_orig) > 0)
            {
                // Is edge duplicate?
                if (paths.count(s_orig) > 0 && paths[s_orig].count(t_orig) > 0)
                {
                    paths[t_orig].erase(s_orig);
                    paths[s_orig].erase(t_orig);
                }
            }
        }
    }

    double KShortestPaths2::PathCosts(size_t iteration)
    {
        util::Logger::LogDebug("path costs iteration: " + std::to_string(iteration));

        MultiPredecessorMap& paths = i_paths_[iteration];
        double value = 0.0;

//        EdgeWeightMap weights = boost::get(boost::edge_weight, graph_orig_);
//        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
//        for (boost::tie(ei, ei_end) = boost::edges(graph_orig_);
//             ei != ei_end; ++ei)
//        {
//            Vertex s_orig = boost::source(*ei, graph_orig_);
//            Vertex t_orig = boost::target(*ei, graph_orig_);
//
//            // If the edge is on the path, add the edge weight to the overall
//            // path costs
//            if (paths.count(t_orig) > 0 && paths[t_orig].count(s_orig) > 0)
//            {
//                value += weights[*ei];
//            }
//        }

        for (int i = 0; i < iteration; ++i)
        {
            value += i_distances_[i][orig_to_copy_[sink_]];
        }

        util::Logger::LogDebug("calculated costs: " + std::to_string(value));

        return value;
    }

    KShortestPaths2::KShortestPaths2()
    {
        /* EMPTY */
    }

    KShortestPaths2::~KShortestPaths2()
    {
        /* EMPTY */
    }
}