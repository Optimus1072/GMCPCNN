//
// Created by wrede on 28.06.16.
//

#include <boost/graph/named_function_params.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include "KShortestPaths5.h"
#define DEBUG

namespace algo
{

    KShortestPaths5::KShortestPaths5(DirectedGraph input_graph, Vertex source, Vertex sink)
    {
        orig_graph_ = input_graph;
        source_ = source;
        sink_ = sink;
    }

    void KShortestPaths5::Run(size_t max_path_count)
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
        }
    }

    void KShortestPaths5::FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                                   VertexPredecessorMap& predecessors)
    {
        // The predecessors and the distances
        std::vector<Vertex> p(boost::num_vertices(graph));
        std::vector<Weight> d(boost::num_vertices(graph));

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

#ifdef DEBUG
        std::cout << "the specified graph has "
                  << (negative_edges ? "negative" : "only positive")
                  << " edge weights" << std::endl;
#endif

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

#ifdef DEBUG
        {
            std::cout << "distances and parents:" << std::endl;
            VertexIter vi, vend;
            for (boost::tie(vi, vend) = vertices(graph); vi != vend; ++vi)
            {
                std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
                std::cout << "parent(" << *vi << ") = " << p[*vi] << std::endl;
            }
            std::cout << std::endl;
        }
#endif

#ifdef DEBUG
        std::cout << "path: ";
        std::cout << sink;
#endif
        // Record the vertices already visited to detect negative cycles
        std::unordered_map<Vertex, bool> visited_vertices;
        VertexIter vi, vi_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi)
        {
            visited_vertices[*vi] = false;
        }

        // Insert the path from the specified source to target into the specified map
        for (auto u = sink, v = p[u]; u != source; u = v, v = p[u])
        {
            if (visited_vertices[u])
            {
                std::cerr << "error: negative cycles are not allowed" << std::endl;
                break;
            }

            predecessors[u] = v;
            visited_vertices[u] = true;
#ifdef DEBUG
            std::cout << "<" << v;
#endif
        }

#ifdef DEBUG
        std::cout << std::endl;
#endif
    }

    void KShortestPaths5::FindPathPair()
    {
        VertexIter vi, vi_end;
        EdgeIter ei, ei_end;

        // Find the first path
        VertexPredecessorMap orig_first_path;
        FindPath(orig_graph_, source_, sink_, orig_first_path);

        // Transform the graph, invert the direction and weight of every edge in the first path
        // found, then add concomitant vertices for every vertex on the path, eventually check that
        // every edge pointing on the path should end in the newly created vertex in the splitting
        // process

        // Create the graph to transform and create the map to map from vertices in the transformed
        // graph to vertices in the original graph, at first every vertex is mapped to itself
        DirectedGraph trans_graph;
        for (boost::tie(vi, vi_end) = boost::vertices(orig_graph_); vi != vi_end; ++vi)
        {
            boost::add_vertex(trans_graph);
        }

        // Transform the first path by inverting edges (and weights) and splitting nodes along the
        // path

        // Reverse the first path
        VertexPredecessorMap trans_first_path;
        for (auto u = sink_, v = orig_first_path[u]; u != source_; u = v, v = orig_first_path[u])
        {
            trans_first_path[v] = u;
        }

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
                std::cerr << "error: edge not found " << v << " -> " << u << std::endl;
            }
        }

#ifdef DEBUG
        std::cout << "inverted edges: " << std::endl;
        for (boost::tie(ei, ei_end) = boost::edges(trans_graph); ei != ei_end; ++ei)
        {
            std::cout << boost::source(*ei, trans_graph)
                      << " > " << boost::target(*ei, trans_graph)
                      << " | " << boost::get(boost::edge_weight, trans_graph, *ei)
                      << std::endl;
        }
        std::cout << std::endl;
#endif

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
                w = boost::get(boost::edge_weight, trans_graph, edge);
            }
            else
            {
                std::cerr << "error: edge not found " << v << " -> " << u << std::endl;
            }

            // Create the edge from the concomitant vertex to the path predecessor
            boost::add_edge(new_u, v, -w, trans_graph);
        }

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

#ifdef DEBUG
        std::cout << "transformed graph: " << std::endl;
        for (boost::tie(ei, ei_end) = boost::edges(trans_graph); ei != ei_end; ++ei)
        {
            std::cout << boost::source(*ei, trans_graph)
                      << " > " << boost::target(*ei, trans_graph)
                      << " | " << boost::get(boost::edge_weight, trans_graph, *ei)
                      << std::endl;
        }
        std::cout << std::endl;
#endif

        // Find the second path in the transformed graph
        VertexPredecessorMap trans_second_path;
        FindPath(trans_graph, source_, sink_, trans_second_path);

        // Check if the two paths have vertices (except source and sink) in common
        bool vertex_disjoint = true;
        for (auto u = trans_first_path[sink_]; u != source_; u = trans_first_path[u])
        {
            if (trans_second_path.count(u) > 0)
            {
                vertex_disjoint = false;
                break;
            }
        }

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

        // If the paths are not vertex disjoint, we need to remove the edges common to both paths
        if (!vertex_disjoint)
        {
            for (auto u = sink_, v = orig_first_path[u];
                 u != source_;
                 u = v, v = orig_first_path[u])
            {
                if (orig_second_path.count(v) > 0 && orig_second_path[v] == u)
                {
                    orig_second_path.erase(v);
                }
            }
        }

        // Store the paths
        AddPath(orig_first_path);
        AddPath(orig_second_path);
    }

    void KShortestPaths5::AddPath(VertexPredecessorMap& path)
    {
        for (auto u = path[sink_], v = path[u]; u != source_; u = v, v = path[u])
        {
            paths_[u] = v;
        }

        sink_neighbors_.push_back(path[sink_]);
    }

    void KShortestPaths5::FindPaths(size_t count)
    {
        //TODO
    }

    void KShortestPaths5::GetPaths(std::vector<std::vector<Vertex>>& paths)
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



