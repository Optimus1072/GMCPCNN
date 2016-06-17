//
// Created by wrede on 15.06.16.
//

#ifndef GBMOT_KSHORTESTPATHS2_H
#define GBMOT_KSHORTESTPATHS2_H

#include "../graph/Definitions.h"

namespace algo
{
    class KShortestPaths2
    {
    private:
        DirectedGraph graph_orig_;
        DirectedGraph graph_copy_;
        Vertex source_;
        Vertex sink_;
        std::unordered_map<Vertex, Vertex> orig_to_copy_;
        std::unordered_map<Vertex, Vertex> copy_to_orig_;
        std::unordered_map<Vertex, Vertex> in_to_out_;
        std::unordered_map<Vertex, Vertex> out_to_in_;
        std::vector<std::pair<std::pair<Vertex, Vertex>, double>> edges_to_add_;
        std::vector<std::pair<Vertex, Vertex>> edges_to_remove_;
        std::vector<std::unordered_map<Vertex, double>> i_distances_;
        std::vector<MultiPredecessorMap> i_paths_;

        void CreateCopy();
        void ExtendGraph(size_t iteration);
        void TransformEdges(size_t iteration);
        void FindAndAugment(size_t iteration);
        void Interlace(size_t iteration);
        double PathCosts(size_t iteration);

        void QueueCopyEdges();
        void QueueRemoveAll();
        void QueueRemoveEdge(Vertex source, Vertex target);
        void QueueAddEdge(Vertex source, Vertex target, double weight);
        void UpdateEdges();
    public:
        KShortestPaths2();
        ~KShortestPaths2();
        MultiPredecessorMap Run(DirectedGraph& graph_orig,
                                Vertex source, Vertex sink,
                                size_t iterations);
    };
}


#endif //GBMOT_KSHORTESTPATHS2_H
