//
// Created by wrede on 24.06.16.
//

#ifndef GBMOT_KSHORTESTPATHS4_H
#define GBMOT_KSHORTESTPATHS4_H

#include "../graph/Definitions.h"
#include "../core/Tracklet.h"

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
        std::vector<Vertex> vertex_candidates_;

        VertexPredecessorMap interlacing_predecessors_;
        std::unordered_map<Vertex, bool> interlacing_predecessors_positive_;

        std::vector<Vertex> source_neighbors_;
        std::vector<Vertex> sink_neighbors_;
        VertexPredecessorMap path_predecessors_;

        size_t max_paths_count_;

        size_t total_paths_count_;
        double total_paths_distance_;

        EdgeWeightMap original_weights_;

        void Initialization();
        void InterlacingConstruction();
        void NeighborDistanceTest(Vertex vertex_r);
        void NegativeInterlacing(Vertex vertex_i);
        void NextPathDefinition();
        void NewInitialConditions();
        void FeasibleTermination();
        void NonFeasibleTermination();
        void SetCandidates();

        Vertex FindPathDestination(VertexPredecessorMap& map, Vertex origin,
                                   std::vector<Vertex>& possible_destination, Vertex element);
        Vertex FindPathSuccessor(VertexPredecessorMap& map, Vertex origin, Vertex destination, Vertex element);
        bool Remove(std::vector<Vertex>& vector, Vertex element);
        bool Contains(VertexPredecessorMap& map, Vertex origin, Vertex destination, Vertex element);
        bool Contains(std::vector<Vertex>& vector, Vertex element);
    public:
        KShortestPaths4(DirectedGraph graph, Vertex source, Vertex sink, size_t max_paths_count);
        void Run();

        std::vector<std::vector<Vertex>> GetPaths();
        void GetTracks(std::vector<core::TrackletPtr>& tracks);
        double GetTotalPathsLength();
    };
}

#endif //GBMOT_KSHORTESTPATHS4_H
