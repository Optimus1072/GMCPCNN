//
// Created by wrede on 24.06.16.
//

#include <boost/graph/named_function_params.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include "KShortestPaths4.h"
#include <iomanip>
#define DEBUG

namespace algo
{
    KShortestPaths4::KShortestPaths4(DirectedGraph graph, Vertex source, Vertex sink,
                                     size_t max_paths_count)
    {
        graph_ = graph;
        source_ = source;
        sink_ = sink;

//        vertex_labels_ = VertexDistanceMap();
//        vertex_distances_ = VertexDistanceMap();
//        vertex_weights_ = VertexDistanceMap();
//        vertex_predecessors_ = VertexVertexMap();
//        vertex_candidates_ = std::vector<Vertex>();
//
//        i_shortest_paths_ = std::vector<VertexVertexMap>();

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
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;

        // Clear previous data
        vertex_labels_.clear();
        vertex_distances_.clear();
        vertex_candidates_.clear();
        interlacing_predecessors_.clear();
        interlacing_predecessors_positive_.clear();
        source_neighbors_.clear();
        sink_neighbors_.clear();
        path_predecessors_.clear();

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

        //TODO check if the path is node simple

        // Store the vertex of the path that is the predecessor of the sink
        sink_neighbors_.push_back(path_pred_map[sink_]);

        // Add the path
        for (Vertex u = sink_neighbors_[0], v = path_pred_map[u]; u != source_; u = v, v = path_pred_map[v])
        {
            path_predecessors_[u] = v;
        }

        // Store the vertex of the path that is the successor of the source
        source_neighbors_.push_back(FindPathSuccessor(path_predecessors_, source_, sink_neighbors_[0], source_));

        total_paths_count_ = 1;
        total_paths_distance_ = path_total_distance;

        // If only one path is needed the algorithm can terminate successfully
        if (max_paths_count_ <= 1)
        {
            FeasibleTermination();
            return;
        }

        // Set the distance labels for all vertices
        for (boost::tie(vi, vi_end) = boost::vertices(graph_); vi != vi_end; ++vi)
        {
            double distance = path_dist_map[*vi];

            // If the vertex is not part of the spanning tree, or the distance is greater then the total distance,
            // set the label to unlabeled (max double value)
            if (distance > path_total_distance || (path_pred_map[*vi] == *vi && *vi != source_))
            {
                distance = std::numeric_limits<double>::max();
            }

            vertex_labels_[*vi] = distance;
        }

//#ifdef DEBUG
//        std::cout << "Graph edges:\n";
//        for (boost::tie(ei, ei_end) = boost::edges(graph_); ei != ei_end; ++ei)
//        {
//            Vertex e_s = boost::source(*ei, graph_);
//            Vertex e_t = boost::target(*ei, graph_);
//            std::cout << "from " << std::setw(2) << e_s << " to " << std::setw(2) << e_t
//                      << " with weight " << std::setw(2) << boost::get(boost::edge_weight, graph_, *ei) << std::endl;
//        }
//#endif
//
//        // Transform the graph into its canonic equivalent
//        EdgeWeightMap edge_weights = boost::get(boost::edge_weight, graph_);
//        for (boost::tie(ei, ei_end) = boost::edges(graph_); ei != ei_end; ++ei)
//        {
//            Vertex e_s = boost::source(*ei, graph_);
//            Vertex e_t = boost::target(*ei, graph_);
//            double l_s = std::min(vertex_labels_[e_s], path_total_distance);
//            double l_t = std::min(vertex_labels_[e_t], path_total_distance);
//            edge_weights[*ei] = l_s + edge_weights[*ei] - l_t;
//        }
//
//#ifdef DEBUG
//        std::cout << "Graph edges transformed:\n";
//        for (boost::tie(ei, ei_end) = boost::edges(graph_); ei != ei_end; ++ei)
//        {
//            Vertex e_s = boost::source(*ei, graph_);
//            Vertex e_t = boost::target(*ei, graph_);
//            std::cout << "from " << std::setw(2) << e_s << " to " << std::setw(2) << e_t
//                      << " with weight " << std::setw(2) << boost::get(boost::edge_weight, graph_, *ei) << std::endl;
//        }
//#endif

        // Define the candidate list
        SetCandidates();

        InterlacingConstruction();
    }

    void KShortestPaths4::InterlacingConstruction()
    {
        while (true)
        {
#ifdef DEBUG
            for (auto const &v : vertex_candidates_)
            {
                double distance = vertex_distances_[v];
                if (distance != std::numeric_limits<double>::max())
                {
                    std::cout << "(" << v << "," << vertex_distances_[v] << ","
                              << (interlacing_predecessors_positive_[v] ? "" : "-")
                              << interlacing_predecessors_[v] << ")";
                }
            }
            std::cout << std::endl;
#endif

            // Find the vertex in the candidate list with the minimum distance
            Vertex min_distance_vertex = 0;
            double min_distance = std::numeric_limits<double>::max();
            for (size_t i = 0; i < vertex_candidates_.size(); ++i)
            {
                Vertex v = vertex_candidates_[i];
                double distance = vertex_distances_[v];
                if (distance < min_distance)
                {
                    min_distance_vertex = v;
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
            Remove(vertex_candidates_, min_distance_vertex);

#ifdef DEBUG
            std::cout << min_distance_vertex << " " << vertex_labels_[min_distance_vertex]
                      << (interlacing_predecessors_positive_[min_distance_vertex] ? " " : " -")
                      << interlacing_predecessors_[min_distance_vertex] << std::endl;
#endif

            if (min_distance_vertex == sink_)
            {
                NextPathDefinition();
                return;
            }
            else
            {
                if (path_predecessors_.count(min_distance_vertex) > 0 || Contains(sink_neighbors_, min_distance_vertex))
                {
                    NegativeInterlacing(min_distance_vertex);
                    return;
                }
                else
                {
                    NeighborDistanceTest(min_distance_vertex);
                }
            }
        }
    }

    void KShortestPaths4::NeighborDistanceTest(Vertex r)
    {
        // Compute the distance to all neighbors and find the best fitting neighbor,
        // than save that neighbor and the new calculated distance
        EdgeWeightMap edge_weights = boost::get(boost::edge_weight, graph_);
        boost::graph_traits<DirectedGraph>::out_edge_iterator oei, oei_end;
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
                    interlacing_predecessors_[j] = r;
                }
            }
        }
    }

    void KShortestPaths4::NegativeInterlacing(Vertex vertex_i)
    {
        boost::graph_traits<DirectedGraph>::edge_descriptor ed;
        bool e_found;

        // Find the path containing the specified vertex
        Vertex path_destination = FindPathDestination(path_predecessors_, source_, sink_neighbors_, vertex_i);

        // Find the first vertex after I, in reverse vertex order, which is either weighted or not a candidate
        Vertex vertex_j = source_;
        for (auto u = vertex_i, v = path_predecessors_[u]; v != source_; u = v, v = path_predecessors_[v])
        {
            boost::tie(ed, e_found) = boost::edge(v, u, graph_);
            double weight = vertex_labels_[v] + boost::get(boost::edge_weight, graph_, ed) - vertex_labels_[u];;
            if (weight < 0 || !Contains(vertex_candidates_, v))
            {
                vertex_j = v;
                break;
            }
        }

        // Set Q to the next vertex after J in vertex order
        Vertex vertex_q = FindPathSuccessor(path_predecessors_, source_, path_destination, vertex_j);

        // Set T to the node in P were the interlacing from origin to I last enters P
        Vertex vertex_t = vertex_i;
        for (auto u = vertex_i, v = path_predecessors_[u]; u != source_; u = v, v = path_predecessors_[v])
        {
            if (interlacing_predecessors_.count(u) > 0 && interlacing_predecessors_[u] != v)
            {
                vertex_t = u;
                break;
            }
        }

//#ifdef DEBUG
//        std::cout << "I = " << std::setw(2) << vertex_i << " "
//                  << "J = " << std::setw(2) << vertex_j << " "
//                  << "Q = " << std::setw(2) << vertex_q << " "
//                  << "T = " << std::setw(2) << vertex_t << std::endl;
//#endif

        // For each node on path between J and I (and different from)
        for (auto v = path_predecessors_[vertex_i]; v != vertex_j; v = path_predecessors_[v])
        {
            // Remove the node from candidates
            Remove(vertex_candidates_, v);
        }

        // For each node on path between J and I (and different from)
        for (auto v = path_predecessors_[vertex_i]; v != vertex_j; v = path_predecessors_[v])
        {
            // Increase the label by the distance to I
            vertex_labels_[v] += vertex_distances_[vertex_i];

            // Set the predecessor to -T
            interlacing_predecessors_[v] = vertex_t;
            interlacing_predecessors_positive_[v] = false;

            NeighborDistanceTest(v);
        }

        // Save L_J and compute L_J' to perform a neighbor distance test with L_J' and then restore L_J
        double label_j = vertex_labels_[vertex_j];
        boost::tie(ed, e_found) = boost::edge(vertex_j, vertex_q, graph_);
        double label_j_mark = vertex_labels_[vertex_q] - boost::get(boost::edge_weight, graph_, ed);
        vertex_labels_[vertex_j] = label_j_mark;
        NeighborDistanceTest(vertex_j);
        vertex_labels_[vertex_j] = label_j;

        // If and only if J is a candidate
        if (Contains(vertex_candidates_, vertex_j))
        {
            double delta = label_j_mark - label_j;
            if (vertex_distances_[vertex_j] > delta)
            {
                vertex_distances_[vertex_j] = delta;

                // Set the predecessor to -T
                interlacing_predecessors_[vertex_j] = vertex_t;
                interlacing_predecessors_positive_[vertex_j] = false;
            }
        }

        InterlacingConstruction();
    }

    void KShortestPaths4::NextPathDefinition()
    {
        // Start with the sink vertex
        Vertex vertex_i = sink_;

        // Follow the interlacing predecessors backwards and revise the path to define P_{M+1}
        while (true)
        {
            // Set J to the vertex in the shortest path where the interlacing last leaves the path prior to I
            Vertex vertex_j = source_;
            for (auto u = vertex_i, v = interlacing_predecessors_[vertex_i]; v != source_;
                 u = v, v = interlacing_predecessors_[v])
            {
                if (path_predecessors_.count(u) == 0 && path_predecessors_.count(v) > 0)
                {
                    vertex_j = v;
                    break;
                }
            }

            // Set Q to the first vertex in the interlacing following J
            Vertex vertex_q = FindPathSuccessor(interlacing_predecessors_, source_, vertex_i, vertex_j);

            // For each node j from Q to I (inclusive) on S, set t_j = p_j
            for (auto u = vertex_i, v = interlacing_predecessors_[u]; u != vertex_q;
                 u = v, v = interlacing_predecessors_[v])
            {
                path_predecessors_[u] = v;
            }
            path_predecessors_[vertex_q] = interlacing_predecessors_[vertex_q]; //TODO (experimental)

            // Check if the interlacing reached the source vertex, if so, the interlacing is completed
            if (vertex_j != source_)
            {
                // Find T = node following J on P_M
                Vertex vertex_t = 0;
                for (auto& v : sink_neighbors_)
                {
                    Vertex successor = FindPathSuccessor(path_predecessors_, source_, v, vertex_j);
                    if (successor != vertex_j)
                    {
                        vertex_t = successor;
                        break;
                    }
                }

                //TODO verify the meaning of (I = node in P_M where S last enters P_M prior to J)
                if (interlacing_predecessors_positive_[vertex_t])
                {
                    vertex_i = vertex_t;
                }
                else
                {
                    vertex_i = interlacing_predecessors_[vertex_t];
                }
            }
            else
            {
                // Add Q to the source neighbors
                source_neighbors_.push_back(vertex_q);

                // Add interlacing predecessor of the sink to the sink neighbors
                sink_neighbors_.push_back(interlacing_predecessors_[sink_]);

                // Add the paths distance to the total distance of all paths
                total_paths_distance_ += vertex_labels_[sink_];

                // A new path was found
                ++total_paths_count_;

                // Check if the necessary number of paths was found
                if (total_paths_count_ < max_paths_count_)
                    NewInitialConditions();
                else
                    FeasibleTermination();

                return;
            }
        }
    }

    void KShortestPaths4::NewInitialConditions()
    {
        // Add the total path to each vertex label
        for (auto const& v : vertex_candidates_)
        {
            vertex_labels_[v] += vertex_distances_[sink_];
        }

        // Transform the graph
//        EdgeWeightMap edge_weights = boost::get(boost::edge_weight, graph_);
//        boost::graph_traits<DirectedGraph>::edge_iterator ei, ei_end;
//        for (boost::tie(ei, ei_end) = boost::edges(graph_); ei != ei_end; ++ei)
//        {
//            Vertex e_s = boost::source(*ei, graph_);
//            Vertex e_t = boost::target(*ei, graph_);
//            double l_s = vertex_labels_[e_s];
//            double l_t = vertex_labels_[e_t];
//            edge_weights[*ei] = l_s + edge_weights[*ei] - l_t;
//        }

        // Redefine candidate list
        SetCandidates();

        InterlacingConstruction();
    }

    void KShortestPaths4::SetCandidates()
    {
        // Clear all corresponding values
        vertex_candidates_.clear();
        vertex_distances_.clear();
        interlacing_predecessors_.clear();
        interlacing_predecessors_positive_.clear();

        // Add all vertices in the graph except the source neighbors and the source itself
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph_); vi != vi_end; ++vi)
        {
            if (*vi == source_) continue;
            if (Contains(source_neighbors_, *vi)) continue;

            vertex_candidates_.push_back(*vi);
        }

        // For each candidate define the distance and predecessors
        boost::graph_traits<DirectedGraph>::edge_descriptor ed;
        bool e_found;
        for (auto const& v : vertex_candidates_)
        {
            boost::tie(ed, e_found) = boost::edge(source_, v, graph_);
            if (e_found)
            {
                vertex_distances_[v] = boost::get(boost::edge_weight, graph_, ed) - vertex_labels_[v];
                interlacing_predecessors_[v] = source_;
                interlacing_predecessors_positive_[v] = true;
            }
            else
            {
                vertex_distances_[v] = std::numeric_limits<double>::max();

                //TODO check if necessary (if not, remove the lines completely)
                interlacing_predecessors_[v] = std::numeric_limits<Vertex>::max();
                interlacing_predecessors_positive_[v] = true;
            }
        }
    }

    void KShortestPaths4::FeasibleTermination()
    {
        //TODO implement
#ifdef DEBUG
        std::cout << "Feasible termination!\n";
#endif
    }

    void KShortestPaths4::NonFeasibleTermination()
    {
        //TODO implement
#ifdef DEBUG
        std::cout << "Non feasible termination!\n";
#endif
    }

    Vertex KShortestPaths4::FindPathDestination(VertexPredecessorMap& map, Vertex origin,
                                                std::vector<Vertex>& possible_destination, Vertex element)
    {
        if (element == origin) return element;

        for (auto d : possible_destination)
        {
            for (auto& v = d; v != origin; v = map[v])
            {
                if (v == element)
                {
                    return d;
                }
            }
        }

        return element;
    }

    Vertex KShortestPaths4::FindPathSuccessor(VertexPredecessorMap& map, Vertex origin, Vertex destination,
                                              Vertex element)
    {
        for (auto u = destination, v = map[u]; u != origin; u = v, v = map[v])
        {
            if (v == element)
            {
                return u;
            }
        }

        return element;
    }

    bool KShortestPaths4::Remove(std::vector<Vertex>& vector, Vertex element)
    {
        for (size_t i = 0; i < vector.size(); ++i)
        {
            if (vector[i] == element)
            {
                vector.erase(vector.begin() + i);
                return true;
            }
        }

        return false;
    }

    bool KShortestPaths4::Contains(VertexPredecessorMap& map, Vertex origin, Vertex destination, Vertex element)
    {
        if (element == origin || element == destination) return true;

        for (auto& v = map[destination]; v != origin; v = map[v])
        {
            if (v == element)
            {
                return true;
            }
        }

        return false;
    }

    bool KShortestPaths4::Contains(std::vector<Vertex>& vector, Vertex element)
    {
        for (auto const& v : vector)
        {
            if (v == element)
            {
                return true;
            }
        }

        return false;
    }

    std::vector<std::vector<Vertex>> KShortestPaths4::GetPaths()
    {
        std::vector<std::vector<Vertex>> paths;
        for (auto u : sink_neighbors_)
        {
            std::vector<Vertex> path;

            path.push_back(sink_);
            for (auto v = u; v != source_; v = path_predecessors_[v])
            {
                path.insert(path.begin(), v);
            }
            path.insert(path.begin(), source_);

            paths.push_back(path);
        }
        return paths;
    }

    double KShortestPaths4::GetTotalPathsLength() {
        return total_paths_distance_;
    }
}





