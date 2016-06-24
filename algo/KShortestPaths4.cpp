//
// Created by wrede on 24.06.16.
//

#include <boost/graph/named_function_params.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include "KShortestPaths4.h"

namespace algo
{
    KShortestPaths4::KShortestPaths4(DirectedGraph graph, Vertex source, Vertex sink,
                                     size_t max_paths_count)
    {
        graph_ = graph;
        source_ = source;
        sink_ = sink;

        vertex_labels_ = VertexDistanceMap();
        vertex_distances_ = VertexDistanceMap();
        vertex_predecessors_ = VertexVertexMap();
        vertex_candidates_ = std::vector<Vertex>();

        i_shortest_paths_ = std::vector<VertexVertexMap>();

        max_paths_count_ = max_paths_count;

        total_paths_count_ = 0;
        total_paths_distance_ = 0.0;
    }

    void KShortestPaths4::Run()
    {
        Initialization();
    }

    void KShortestPaths4::Initialization()
    {
        // Find the first path with predecessors and distances
        size_t graph_size = boost::num_vertices(graph_);
        std::vector<Vertex> pred_list(graph_size);
        std::vector<double> dist_list(graph_size);
        VertexIndexMap graph_indices = boost::get(boost::vertex_index, graph_);
        EdgeWeightMap weight_map = boost::get(boost::edge_weight, graph_);
        PredecessorMap path_pred_map(&pred_list[0], graph_indices);
        DistanceMap path_dist_map(&dist_list[0], graph_indices);
        boost::bellman_ford_shortest_paths(graph_, graph_size,
                                           boost::root_vertex(source_)
                                                   .weight_map(weight_map)
                                                   .predecessor_map(path_pred_map)
                                                   .distance_map(path_dist_map));
        double path_total_distance = path_dist_map[sink_];

        //TODO add the path to the i_shortest_paths_;

        // If only one path is needed the algorithm can terminate successful
        if (max_paths_count_ == 1) return; //TODO return with the found path

        // Store the vertices of the path that are the predecessor of the source vertex
        // and the successor of the sink vertex
        Vertex source_predecessor = source_;
        Vertex sink_successor = sink_;
        for (Vertex u = path_pred_map[sink_], v = path_pred_map[u]; u != v; u = v, v = path_pred_map[v])
        {
            if (v == sink_)
            {
                source_predecessor = u;
                break;
            }
        }
        sink_successor = path_pred_map[sink_];

        // Set the distance label for all vertices
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph_); vi != vi_end; ++vi)
        {
            double distance = path_dist_map[*vi];

            // If the vertex is not part of the spanning tree, set the distance to the total
            // distance of the path from source to sink
            if (path_pred_map[*vi] == *vi && vi != source_)
            {
                distance = path_total_distance;
            }

            vertex_labels_[*vi] = distance;
        }

        // Define the candidate list
        for (boost::tie(vi, vi_end) = boost::vertices(graph_); vi != vi_end; ++vi)
        {
            if (*vi == source_ || *vi == source_predecessor) continue;

            vertex_candidates_.push_back(*vi);
        }

        // For each candidate define the distance and predecessors
        EdgeWeightMap edge_weights = boost::get(boost::edge_weight, graph_);
        boost::graph_traits<DirectedGraph>::edge_descriptor ed;
        bool e_found;
        for (Vertex v : vertex_candidates_)
        {
            if (path_pred_map[v] == source_)
            {
                boost::tie(ed, e_found) = boost::edge(source_, v, graph_);
                vertex_distances_[v] = boost::get(edge_weights, ed) - vertex_labels_[v];
                vertex_predecessors_[v] = source_;
            }
            else
            {
                vertex_distances_[v] = std::numeric_limits<double>::max();
                vertex_predecessors_[v] = std::numeric_limits<size_t>::max();
            }
        }

        total_paths_count_ = 1;
        total_paths_distance_ = path_total_distance;

        InterlacingConstruction();
    }

    void KShortestPaths4::InterlacingConstruction()
    {
        // Find the vertex in the candidate list with the minimum distance
        Vertex min_distance_vertex = 0;
        size_t min_distance_index = 0;
        double min_distance = std::numeric_limits<double>::max();
        for (size_t i = 0; i < vertex_candidates_.size(); ++i)
        {
            Vertex v = vertex_candidates_[i];
            double distance = vertex_distances_[v];
            if (distance < min_distance)
            {
                min_distance_vertex = v;
                min_distance_index = i;
                min_distance = distance;
            }
        }

        if (min_distance == std::numeric_limits<double>::max())
        {
            NonFeasibleTermination();
            return;
        }

        // Add the distance to the vertex label and remove the vertex from the candidates
        vertex_labels_[min_distance_vertex] += min_distance;
        vertex_candidates_.erase(vertex_candidates_.begin() + min_distance_index);

        if (min_distance_vertex == sink_)
        {
            //TODO goto P_M+1 Definition
        }
        else
        {
            for (Vertex v : i_shortest_paths_[total_paths_count_ - 1])
            {
                if (min_distance_vertex == v)
                {
                    NegativeInterlacing(min_distance_vertex);
                    return;
                }
            }

            NeighborDistanceTest(min_distance_vertex);
            InterlacingConstruction();
        }
    }

    void KShortestPaths4::NeighborDistanceTest(Vertex r)
    {
        // Compute the distance to all neighbors and find the best fitting neighbor,
        // than save that neighbor and the new calculated distance
        EdgeWeightMap edge_weights = boost::get(boost::edge_weight, graph_);
        boost::graph_traits<DirectedGraph>::edge_iterator oei, oei_end;
        for (boost::tie(oei, oei_end) = boost::out_edges(r, graph_); oei != oei_end; ++oei)
        {
            Vertex j = boost::target(*oei, graph_);

            // Check if the vertex is a candidate
            if (Contains(vertex_candidates_, j))
            {
                // Calculate possible new edge weight for the candidate
                double delta = vertex_labels_[r] + edge_weights[*oei] - vertex_labels_[j];

                if (vertex_distances_[j] > delta)
                {
                    vertex_distances_[j] = delta;
                    vertex_predecessors_[j] = r;
                }
            }
        }
    }

    void KShortestPaths4::NegativeInterlacing(Vertex input)
    {
        // Find the path containing the specified vertex
        std::list<Vertex> path;
        for (auto p : i_shortest_paths_)
        {
            if (Contains(p, input) > 0)
            {
                path = p;
                break;
            }
        }
        // Iterate the path in reverse vertex order //TODO check for correct order
        Vertex j;
        bool found_input = false;
        bool is_last = false;
        for (auto vertex = path.rbegin(); vertex != path.rend(); ++vertex)
        {
            // Find the first vertex which is not a candidate
            if (!found_input)
            {
                if (*vertex == input)
                {
                    found_input = true;
                }
            }
            else
            {
                j = *vertex;
                break;
            }
        }

        //TODO implement
    }

    void KShortestPaths4::FeasibleTermination()
    {
        //TODO implement
    }

    void KShortestPaths4::NonFeasibleTermination()
    {
        //TODO implement
    }

    bool KShortestPaths4::Contains(std::vector<Vertex>& vector, Vertex& element)
    {
        for (Vertex v : vector)
        {
            if (v == element)
            {
                return true;
            }
        }

        return false;
    }

    bool KShortestPaths4::Contains(std::list<Vertex>& list, Vertex& element)
    {
        for (Vertex v : list)
        {
            if (v == element)
            {
                return true;
            }
        }

        return false;
    }
}





