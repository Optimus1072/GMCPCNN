//
// Created by wrede on 25.05.16.
//

#ifndef GBMOT_KSHORTESTPATHS_H
#define GBMOT_KSHORTESTPATHS_H

#include "../graph/Definitions.h"
#include "../core/ObjectData.h"
#include "../core/Tracklet.h"

namespace algo
{
    /**
     * Class providing a k-shortest-paths algorithm implementation.
     */
    class KShortestPaths
    {
    private:
        /**
         * The original graph, will not be altered.
         */
        DirectedGraph original_graph_;

        /**
         * The current copy of the original graph, will be transformed.
         */
        DirectedGraph copied_graph_;

        /**
         * The starting vertex for the next algorithm run.
         * Corresponds to the original graph.
         */
        const Vertex source_;

        /**
         * The target vertex for the next algorithm run.
         * Corresponds to the original graph.
         */
        const Vertex sink_;

        /**
         * Maps the vertices in the original graph to the vertices in the copied
         * graph.
         */
        std::unordered_map<Vertex, Vertex> original_to_copy_;

        /**
         * Maps the vertices in the copied graph to the vertices in the original
         * graph.
         */
        std::unordered_map<Vertex, Vertex> copy_to_original_;

        /**
         * All found paths for every iteration of the current run.
         * Corresponds to the original graph.
         */
        std::vector<MultiPredecessorMap> i_shortest_paths_;

        /**
         * Creates a copy of the original graph and updates all corresponding
         * maps.
         */
        void CopyOriginalGraph();

        /**
         * Extends the current copied graph.
         * Splits the vertices along all paths at the given iteration into two
         * vertices, one for incoming edges and one for outgoing edges. Connects
         * the two vertices with an auxiliary edge.
         * @param i The targeted shortest paths iteration
         */
        void ExtendGraph(size_t i);

        /**
         * Calculates the path costs of every path at the iteration given.
         * @param i The targeted shortest paths iteration
         */
        double OverallCosts(size_t i);

        /**
         * Interlaces all paths in the given iterations.
         * Removes duplicate edges and produces node-disjoint paths.
         * @param i The targeted shortest paths iteration
         */
        void Interlace(size_t i);

        /**
         * Finds the shortest path in either the original or the copied graph.
         * Adds the path to the given iteration of the i_shortest_paths map.
         * @param i The targeted shortest paths iteration
         * @param original If true, the original graph will be used, else the
         *                 current copied graph will be used.
         */
        void FindAndAugmentPath(size_t i, bool original);

        /**
         * Utility variable. Simulates a queue for the edges to add in the next
         * edge update call. All edges and vertices correspond to the copied
         * graph.
         */
        std::vector<std::pair<std::pair<Vertex, Vertex>, double>> edges_to_add_;

        /**
         * Utility variable. Simulates a queue for the edges to remove in the
         * next edge update call. All edges and vertices correspond to the
         * copied graph.
         */
        std::vector<std::pair<Vertex, Vertex>> edges_to_remove_;

        /**
         * Queues the edge between the given two vertices.
         * In the next update edges call the edge will be removed.
         * @param source The source vertex
         * @param target The target vertex
         */
        void QueueRemoveEdge(Vertex source, Vertex target);

        /**
         * Queues the edge between the given two vertices.
         * In the next update edges call the edge will be added with the given
         * weight.
         * @param source The source vertex
         * @param target The target vertex
         * @param weight The weight of the edge
         */
        void QueueAddEdge(Vertex source, Vertex target, double weight);

        /**
         * Updates the copied graph with all queued edges.
         * First removes the queued edges then adds the queued edges.
         */
        void UpdateEdges();
    public:
        /**
         * Initializes the k-shortest-paths algorithm for the given graph.
         * Uses the source vertex as starting and the sink vertex as target
         * vertex.
         * @param graph The graph to work with
         * @param source The starting vertex
         * @param sink The target vertex
         */
        KShortestPaths(DirectedGraph graph, Vertex source, Vertex sink);

        /**
         * Runs the algorithm to find the given number of shortest paths.
         * The actual number of paths found is the number of entries in the
         * MultiPredecessorMap with the sink vertex as the key.
         * @param max_path_count The maximum number of paths to find
         */
        MultiPredecessorMap Run(size_t max_path_count);
    };
}


#endif //GBMOT_KSHORTESTPATHS_H
