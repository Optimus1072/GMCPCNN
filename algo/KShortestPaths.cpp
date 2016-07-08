//
// Created by wrede on 28.06.16.
//

#include <boost/graph/named_function_params.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <iomanip>
#include "KShortestPaths.h"
#include "../util/Logger.h"

namespace algo
{

    KShortestPaths::KShortestPaths(DirectedGraph input_graph, Vertex source, Vertex sink)
    {
        orig_graph_ = input_graph;
        source_ = source;
        sink_ = sink;
    }

    void KShortestPaths::Run(size_t max_path_count)
    {
        paths_.clear();
        sink_neighbors_.clear();
        switch (max_path_count)
        {
            case 0:
                break;
            case 1:
                FindPath(orig_graph_, source_, sink_, paths_);
                sink_neighbors_.push_back(paths_[sink_]);
                break;
            case 2:
                FindPathPair();
                break;
            default:
                FindPaths(max_path_count);
                break;
        }

        util::Logger::LogDebug(std::to_string(sink_neighbors_.size()) + " paths have been found");
    }

    bool KShortestPaths::FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                                   VertexPredecessorMap& predecessors, VertexDistanceMap& distances)
    {
        // The predecessors and the distances
        std::vector<Vertex> p(boost::num_vertices(graph));
        std::vector<Weight> d(boost::num_vertices(graph));

        util::Logger::LogDebug("scan the graph for negative edge weights");

        // Scan the graph for negative edge weights to use the proper algorithm
        bool negative_edges = false;
        EdgeIter ei, ei_end;
        for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei)
        {
            if (boost::get(boost::edge_weight, graph, *ei) < 0)
            {
                negative_edges = true;
                break;
            }
        }

        if (negative_edges)
            util::Logger::LogDebug("the graph contains negative edges");
        else
            util::Logger::LogDebug("the graph contains only positive edges");

        util::Logger::LogDebug("run a single-source shortest paths algorithm");

        if (negative_edges)
        {
            // Run bellman ford to find the single-source shortest paths
            boost::bellman_ford_shortest_paths(
                    graph,
                    boost::num_vertices(graph),
                    boost::root_vertex(source)
                            .predecessor_map(
                                    boost::make_iterator_property_map(
                                            p.begin(), boost::get(boost::vertex_index, graph)))
                            .distance_map(
                                    boost::make_iterator_property_map(
                                            d.begin(), boost::get(boost::vertex_index, graph))));
        }
        else
        {
            // Run dijkstra to find the single-source shortest paths
            boost::dijkstra_shortest_paths(
                    graph,
                    source,
                    boost::predecessor_map(
                            boost::make_iterator_property_map(
                                    p.begin(),
                                    boost::get(boost::vertex_index, graph)))
                            .distance_map(
                                    boost::make_iterator_property_map(
                                            d.begin(),
                                            boost::get(boost::vertex_index, graph))));
        }

        util::Logger::LogDebug("prepare a map of visited vertices to detect negative cycles");

        // Record the vertices already visited to detect negative cycles (and store the distances)
        std::unordered_map<Vertex, bool> visited_vertices;
        VertexIter vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi)
        {
            visited_vertices[*vi] = false;

            distances[*vi] = d[*vi];
        }

        util::Logger::LogDebug("insert the path into the output map (and detect negative cycles)");

        // Insert the path from the specified source to target into the specified map
        for (auto u = sink, v = p[u]; u != source; u = v, v = p[u])
        {
            if (visited_vertices[u])
            {
                util::Logger::LogError("negative cycle at vertex " + std::to_string(u));
                return false;
            }

            predecessors[u] = v;
            visited_vertices[u] = true;
        }

        return true;
    }

    bool KShortestPaths::FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                                   VertexPredecessorMap& predecessors)
    {
        VertexDistanceMap d;
        return FindPath(graph, source, sink, predecessors, d);
    }

    void KShortestPaths::FindPathPair()
    {
        VertexIter vi, vi_end;
        EdgeIter ei, ei_end;

        util::Logger::LogDebug("find the first path (in the original graph)");

        // Find the first path
        VertexPredecessorMap orig_first_path;
        FindPath(orig_graph_, source_, sink_, orig_first_path);

        // Transform the graph, invert the direction and weight of every edge in the first path
        // found, then add concomitant vertices for every vertex on the path, eventually check that
        // every edge pointing on the path should end in the newly created vertex in the splitting
        // process

        util::Logger::LogDebug("copy the original graph");

        // Create the graph to transform and create the map to map from vertices in the transformed
        // graph to vertices in the original graph, at first every vertex is mapped to itself
        DirectedGraph trans_graph;
        for (boost::tie(vi, vi_end) = boost::vertices(orig_graph_); vi != vi_end; ++vi)
        {
            boost::add_vertex(trans_graph);
        }

        // Transform the first path by inverting edges (and weights) and splitting nodes along the
        // path

        util::Logger::LogDebug("reverse the first path");

        // Reverse the first path
        VertexPredecessorMap trans_first_path;
        for (auto u = sink_, v = orig_first_path[u]; u != source_; u = v, v = orig_first_path[u])
        {
            trans_first_path[v] = u;
        }

        util::Logger::LogDebug("invert the edges along the first path");

        // Invert edges
        for (auto u = sink_, v = orig_first_path[u]; u != source_; u = v, v = orig_first_path[u])
        {
            Edge edge;
            bool e_found;
            boost::tie(edge, e_found) = boost::edge(v, u, orig_graph_);
            if (e_found)
            {
                Weight w = boost::get(boost::edge_weight, orig_graph_, edge);

                boost::add_edge(u, v, -w, trans_graph);
            }
            else
            {
                util::Logger::LogError("edge not found " + std::to_string(v) + " -> " +
                                       std::to_string(u));
            }
        }

        util::Logger::LogDebug("split the nodes along the first path");

        // Split nodes
        VertexPredecessorMap old_to_new;
        VertexPredecessorMap new_to_old;
        for (auto u = orig_first_path[sink_], v = orig_first_path[u];
             v != source_;
             u = v, v = orig_first_path[u])
        {
            // Create the concomitant vertex
            Vertex new_u = boost::add_vertex(trans_graph);
            old_to_new[u] = new_u;
            new_to_old[new_u] = u;

            // Retrieve the weight from the original path in the original graph
            Weight w = 0.0;
            Edge edge;
            bool e_found;
            boost::tie(edge, e_found) = boost::edge(v, u, orig_graph_);
            if (e_found)
            {
                w = boost::get(boost::edge_weight, orig_graph_, edge);
            }
            else
            {
                util::Logger::LogError("edge not found " + std::to_string(v) + " -> " +
                                       std::to_string(u));
            }

            // Create the edge from the concomitant vertex to the path predecessor
            boost::add_edge(new_u, v, -w, trans_graph);
        }

        util::Logger::LogDebug("extend the copied graph with the remaining edges");

        // Add all remaining edges
        for (boost::tie(ei, ei_end) = boost::edges(orig_graph_); ei != ei_end; ++ei)
        {
            Vertex source = boost::source(*ei, orig_graph_);
            Vertex target = boost::target(*ei, orig_graph_);
            Weight weight = boost::get(boost::edge_weight, orig_graph_, *ei);

            // Ignore vertices on the path (they are already added)
            if (orig_first_path.count(target) > 0 && orig_first_path[target] == source)
                continue;

            // If the edge points to source or sink add the edge unchanged
            if (target == source_ || target == sink_)
            {
                boost::add_edge(source, target, weight, trans_graph);
                continue;
            }

            // If the edge points to split vertices (vertices on the path except source and sink),
            // point the edge towards the concomitant vertex
            if (trans_first_path.count(target) > 0 && old_to_new.count(target) > 0)
            {
                boost::add_edge(source, old_to_new[target], weight, trans_graph);
                continue;
            }

            // Add every other edge unchanged
            boost::add_edge(source, target, weight, trans_graph);
        }

        util::Logger::LogDebug("find the second path (in the copied and transformed graph)");

        // Find the second path in the transformed graph
        VertexPredecessorMap trans_second_path;
        FindPath(trans_graph, source_, sink_, trans_second_path);

        util::Logger::LogDebug("map the second path into the original graph");

        // Map the second path from the transformed graph into the original graph
        VertexPredecessorMap orig_second_path;
        for (auto u = sink_, v = trans_second_path[u];
             u != source_;
             u = v, v = trans_second_path[u])
        {
            Vertex orig_u = new_to_old.count(u) > 0 ? new_to_old[u] : u;
            Vertex orig_v = new_to_old.count(v) > 0 ? new_to_old[v] : v;
            orig_second_path[orig_u] = orig_v;
        }

        util::Logger::LogDebug("check if the two paths are already vertex disjoint");

        // Check if the two paths have vertices (except source and sink) in common
        bool vertex_disjoint = true;
        for (auto u = orig_first_path[sink_]; u != source_; u = orig_first_path[u])
        {
            if (orig_second_path.count(u) > 0)
            {
                vertex_disjoint = false;
                break;
            }
        }

        // If the paths are not vertex disjoint, we need to remove the edges common to both paths
        if (!vertex_disjoint)
        {
            util::Logger::LogDebug("remove edges used by both paths to guarantee vertex disjointness");

            for (auto u = sink_, v = orig_first_path[u];
                 u != source_;
                 u = v, v = orig_first_path[u])
            {
                if (orig_second_path.count(v) > 0 && orig_second_path[v] == u)
                {
                    orig_first_path.erase(u);
                    orig_second_path.erase(v);
                }
            }
        }

        util::Logger::LogDebug("add the first and second path to the map of all paths");

        // Store the paths
        AddPath(orig_first_path);
        AddPath(orig_second_path);
    }

    void KShortestPaths::AddPath(VertexPredecessorMap& path)
    {
        for (auto edge : path)
        {
            if (edge.first == edge.second)
                continue;

            if (edge.first == sink_)
                sink_neighbors_.push_back(edge.second);
            else
                paths_[edge.first] = edge.second;
        }
    }

    void KShortestPaths::AddPath(VertexPredecessorMap& in, MultiPredecessorMap& out)
    {
        for (auto edge : in)
        {
            if (edge.first != edge.second)
                out[edge.first].insert(edge.second);
        }
    }

    void KShortestPaths::AddPaths(MultiPredecessorMap& paths)
    {
        for (auto pair : paths)
        {
            Vertex t = pair.first;
            for (auto s : pair.second)
            {
                if (t == sink_)
                    sink_neighbors_.push_back(s);
                else
                    paths_[t] = s;
            }
        }
    }

    void KShortestPaths::FindPaths(size_t count)
    {
        VertexIter vi, vi_end;
        EdgeIter ei, ei_end;

        // The edge weights of the original graph
        EdgeWeightMap orig_weights = boost::get(boost::edge_weight, orig_graph_);

        // All found paths
        MultiPredecessorMap k_orig_paths;

        // Find the first path (only path found in the original graph)
        VertexDistanceMap orig_distances;
        VertexPredecessorMap orig_first_path;
        if (!FindPath(orig_graph_, source_, sink_, orig_first_path, orig_distances))
        {
            util::Logger::LogInfo("Not even a single path could have been found!");
            return;
        }
        AddPath(orig_first_path, k_orig_paths);

        // Transform the original edge weights
        for (boost::tie(ei, ei_end) = boost::edges(orig_graph_); ei != ei_end; ++ei)
        {
            Vertex source = boost::source(*ei, orig_graph_);
            Vertex target = boost::target(*ei, orig_graph_);

            orig_weights[*ei] = orig_distances[source] + orig_weights[*ei] - orig_distances[target];
        }

        // Find the specified amount of paths iteratively
        for (size_t i = 1; i < count; ++i)
        {
            util::Logger::LogDebug("copy the original graph");

            // Create a graph used for transformations and start by copying the vertices
            DirectedGraph trans_graph;
            for (boost::tie(vi, vi_end) = boost::vertices(orig_graph_); vi != vi_end; ++vi)
            {
                boost::add_vertex(trans_graph);
            }

            util::Logger::LogDebug("invert the edges along all previous found paths");

            // Invert edges for all previous paths (also invert their weight)
            std::vector<EdgeIter> edge_queue;
            for (boost::tie(ei, ei_end) = boost::edges(orig_graph_); ei != ei_end; ++ei)
            {
                Vertex source = boost::source(*ei, orig_graph_);
                Vertex target = boost::target(*ei, orig_graph_);

                if (k_orig_paths.count(target) > 0 && k_orig_paths[target].count(source) > 0)
                {
                    Weight weight = orig_weights[*ei];

                    boost::add_edge(target, source, -weight, trans_graph);
                }
                else
                {
                    edge_queue.push_back(ei);
                }
            }

            util::Logger::LogDebug("split the nodes along all previous found path");

            // Split nodes
            VertexPredecessorMap old_to_new;
            VertexPredecessorMap new_to_old;
            for (auto orig_edge : k_orig_paths)
            {
                Vertex source = *orig_edge.second.begin();
                Vertex target = orig_edge.first;

                // Create the concomitant vertex
                Vertex new_target = boost::add_vertex(trans_graph);
                old_to_new[target] = new_target;
                new_to_old[new_target] = target;

                // Retrieve the weight from the original path in the original graph
                Weight weight = 0.0;
                Edge edge;
                bool e_found;
                boost::tie(edge, e_found) = boost::edge(source, target, orig_graph_);
                if (e_found)
                {
                    weight = orig_weights[edge];
                }
                else
                {
                    util::Logger::LogError("error: edge not found " + std::to_string(source) +
                                           " -> " + std::to_string(target));
                }

                // Create the edge from the concomitant vertex to the path predecessor
                boost::add_edge(new_target, source, -weight, trans_graph);
            }

            util::Logger::LogDebug("extend the copied graph with the remaining edges");

            // Add all remaining edges
            for (auto e : edge_queue)
            {
                Vertex source = boost::source(*e, orig_graph_);
                Vertex target = boost::target(*e, orig_graph_);
                Weight weight = orig_weights[*e];

                // If the edge points to source or sink add the edge unchanged
                if (target == source_ || target == sink_)
                {
                    boost::add_edge(source, target, weight, trans_graph);
                    continue;
                }

                // If the edge points to split vertices (vertices on the path except source and
                // sink), point the edge towards the concomitant vertex
                if (k_orig_paths.count(target) > 0 && old_to_new.count(target) > 0)
                {
                    boost::add_edge(source, old_to_new[target], weight, trans_graph);
                    continue;
                }

                // Add every other edge unchanged
                boost::add_edge(source, target, weight, trans_graph);
            }

            util::Logger::LogDebug("find the second path (in the copied and transformed graph)");

            // Find the next path in the transformed graph
            VertexPredecessorMap trans_next_path;
            if (!FindPath(trans_graph, source_, sink_, trans_next_path))
            {
                util::Logger::LogInfo("No more paths may be found!");
                return;
            }

            util::Logger::LogDebug("map the second path into the original graph");

            // Map the second path from the transformed graph into the original graph
            VertexPredecessorMap orig_next_path;
            for (auto u = sink_, v = trans_next_path[u]; u != source_; u = v, v = trans_next_path[u])
            {
                Vertex orig_u = new_to_old.count(u) > 0 ? new_to_old[u] : u;
                Vertex orig_v = new_to_old.count(v) > 0 ? new_to_old[v] : v;

                orig_next_path[orig_u] = orig_v;
            }

            AddPath(orig_next_path, k_orig_paths);

            util::Logger::LogDebug("remove edges used by multiple paths to guarantee vertex disjointness");

            // Remove edges used by multiple paths
            for (auto pair : k_orig_paths)
            {
                Vertex target = pair.first;
                for (auto source : pair.second)
                {
                    if (k_orig_paths.count(source) > 0 && k_orig_paths[source].count(target) > 0)
                    {
                        k_orig_paths[source].erase(target);
                        k_orig_paths[target].erase(source);
                    }
                }
            }

            util::Logger::LogDebug("remove vertices with no edges from the path");

            // Remove empty vertices from the paths
            for (auto iter = k_orig_paths.begin(); iter != k_orig_paths.end(); )
            {
                auto pair = *iter;
                if (pair.second.empty())
                {
                    iter = k_orig_paths.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        // Store the paths
        AddPaths(k_orig_paths);
    }

    void KShortestPaths::GetPaths(std::vector<std::vector<Vertex>>& paths)
    {
        for (auto v : sink_neighbors_)
        {
            std::vector<Vertex> path;
            path.push_back(sink_);
            for (auto u = v; u != source_; u = paths_[u])
            {
                path.insert(path.begin(), u);
            }
            path.insert(path.begin(), source_);
            paths.push_back(path);
        }
    }
}



