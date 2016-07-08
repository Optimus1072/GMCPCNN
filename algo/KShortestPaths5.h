//
// Created by wrede on 28.06.16.
//

#ifndef GBMOT_KSHORTESTPATHS5_H
#define GBMOT_KSHORTESTPATHS5_H
#include "../graph/Definitions.h"

namespace algo
{
    class KShortestPaths5
    {
    private:
        DirectedGraph orig_graph_;
        Vertex source_;
        Vertex sink_;

        VertexPredecessorMap paths_;
        std::vector<Vertex> sink_neighbors_;

        bool FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                      VertexPredecessorMap& predecessors, VertexDistanceMap& distances);
        bool FindPath(DirectedGraph& graph, Vertex& source, Vertex& sink,
                      VertexPredecessorMap& predecessors);
        void FindPathPair();
        void FindPaths(size_t count);

        void AddPath(VertexPredecessorMap& path);
        void AddPath(VertexPredecessorMap& in, MultiPredecessorMap& out);
        void AddPaths(MultiPredecessorMap& paths);
    public:
        KShortestPaths5(DirectedGraph input_graph, Vertex source, Vertex sink);
        void Run(size_t max_path_count);
        void GetPaths(std::vector<std::vector<Vertex>>& paths);
    };
}


#endif //GBMOT_KSHORTESTPATHS5_H
