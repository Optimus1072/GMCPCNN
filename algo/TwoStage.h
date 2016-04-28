//
// Created by wrede on 25.04.16.
//

#ifndef GBMOT_KOERNERTRACKING_H
#define GBMOT_KOERNERTRACKING_H

#include "Definitions.h"
#include "../core/DetectionSequence.h"
#include "../core/Tracklet.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace algo
{
    class TwoStage
    {
    private:
        // Maximum edge length to link frames
        size_t max_frame_skip_;
        // Edge value to link to source and sink
        double penalty_value_;
        // Maximum dijkstra iterations / number of tracklets to create
        size_t max_tracklet_count_;
        // The last created object graph
        DirectedGraph obj_graph_;
        // The last created tracklet graph
        DirectedGraph tlt_graph_;
        // The frame count of the last read sequence
        size_t frame_count_;
    public:
        TwoStage(size_t max_frame_skip, double penalty_value,
                 size_t max_tracklet_count);

        // Creates a graph with vertices for every detected object
        DirectedGraph CreateObjectGraph(core::DetectionSequence detections);

        // Reduces the object graph into linked tracklets,
        // if no graph is given, the last created is used
        DirectedGraph CreateTrackletGraph();
        DirectedGraph CreateTrackletGraph(DirectedGraph obj_graph, size_t frameCount);

        std::vector<core::Tracklet> ExtractTracks();
        std::vector<core::Tracklet> ExtractTracks(DirectedGraph tlt_graph);
    };
}


#endif //GBMOT_KOERNERTRACKING_H
