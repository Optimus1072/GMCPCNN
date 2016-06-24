//
// Created by wrede on 24.06.16.
//

#ifndef GBMOT_KSHORTESTPATHS4_H
#define GBMOT_KSHORTESTPATHS4_H

#include "../graph/Definitions.h"

namespace algo
{
    class KShortestPaths4
    {
    private:
        DirectedGraph graph_;
        Vertex source_;
        Vertex sink_;
        VertexDistanceMap vertex_labels_;
        VertexDistanceMap vertex_distances_;
        VertexVertexMap vertex_predecessors_;
        std::vector<Vertex> vertex_candidates_;

        std::vector<std::list<Vertex>> i_shortest_paths_;

        size_t max_paths_count_;

        size_t total_paths_count_;
        double total_paths_distance_;

        void Initialization();
        void InterlacingConstruction();
        void NeighborDistanceTest(Vertex r);
        void NegativeInterlacing(Vertex input);
        void FeasibleTermination();
        void NonFeasibleTermination();

        bool Contains(std::vector<Vertex>& vector, Vertex& element);
        bool Contains(std::list<Vertex>& list, Vertex& element);
    public:
        KShortestPaths4(DirectedGraph graph, Vertex source, Vertex sink, size_t max_paths_count);
        void Run();
    };
}

#endif //GBMOT_KSHORTESTPATHS4_H
