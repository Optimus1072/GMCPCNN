//
// Created by wrede on 28.06.16.
//

#ifndef GBMOT_KSHORTESTPATHS_H
#define GBMOT_KSHORTESTPATHS_H
#include "../graph/Definitions.h"

namespace algo
{
    /**
     * Finds the k-shortest paths in a specified graph, from the source vertex to the sink vertex.
     * The algorithm provides a global optimal path, thus reducing the overall sum of all path costs
     * in trade off for higher cost of one single path.
     */
    class KShortestPaths
    {
    private:
        /**
         * The original/input graph (will not be altered, but can't be const because of boost)
         */
        DirectedGraph orig_graph_;

        /**
         * The source vertex. This is where all paths start from
         */
        Vertex source_;

        /**
         * The sink vertex. This is where all paths end
         */
        Vertex sink_;

        /**
         * A map containing all predecessors for every vertex in every path found so far
         */
        VertexPredecessorMap paths_;

        /**
         * The predecessor of the sink vertex on evey path. Useful to iterate all paths
         */
        std::vector<Vertex> sink_neighbors_;

        /**
         * Finds the shortest path in the specified graph from source to sink.
         * Stores the predecessor vertices and the distances of the vertices.
         *
         * @param graph The graph to work in
         * @param source The vertex to start the path search from
         * @param sink The vertex to end the path at
         * @param predecessors A map containing the predecessors for every vertex
         * @param distances A map containing the distances for every vertex
         */
        bool FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                      VertexPredecessorMap& predecessors, VertexDistanceMap& distances);
        bool FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                      VertexPredecessorMap& predecessors);

        /**
         * Finds a global optimal pair of shortest paths from source to sink in the original graph.
         */
        void FindPathPair();

        /**
         * Finds 'count' global optimal shortest paths from source to sink in the original graph.
         *
         * @param count The number of paths to finds
         */
        void FindPaths(size_t count);

        /**
         * Stores the path to the paths member map. Also updates the sink_neighbors.
         *
         * @param path The path to store
         */
        void AddPath(VertexPredecessorMap& path);

        /**
         * Stores the path to the specified multi predecessor map.
         *
         * @param in The path to store
         * @param out The map to store the path in
         */
        void AddPath(VertexPredecessorMap& in, MultiPredecessorMap& out);

        /**
         * Stores all paths in the given multi predecessor map into the paths member map.
         * Also updates the sink_neighbors.
         *
         * @param paths A map containing all paths to store
         */
        void AddPaths(MultiPredecessorMap& paths);
    public:
        /**
         * Initializes the algorithm for the specified graph with the specified source and sink
         * vertex to find a path between.
         *
         * @param input_graph The graph to work with (will not be altered)
         * @param source The vertex to start all path searches from
         * @param sink The vertex to end all path searches at
         */
        KShortestPaths(DirectedGraph input_graph, Vertex source, Vertex sink);

        /**
         * Runs the algorithm to store the specified number of paths.
         * These paths can later be retrieved by the GetPaths method.
         *
         * @param max_path_count The number of paths to find
         */
        void Run(size_t max_path_count);

        /**
         * Gets the last found paths.
         *
         * @param paths The vector to store all found paths in
         */
        void GetPaths(std::vector<std::vector<Vertex>>& paths);
    };
}


#endif //GBMOT_KSHORTESTPATHS_H
