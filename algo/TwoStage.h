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
    /**
     * Implementation if the two-staged graph-based multi-object tracker.
     */
    class TwoStage
    {
    private:
        /**
         * Maximum edge length to link object
         */
        size_t max_frame_skip_;
        /**
         * Edge value to link to source and sink
         */
        double penalty_value_;
        /*
         * Maximum dijkstra iterations / number of tracklets to create
         */
        size_t max_tracklet_count_;
        /*
         * The last created object graph
         */
        DirectedGraph obj_graph_;
        /*
         * The last created tracklet graph
         */
        DirectedGraph tlt_graph_;
        /**
         * The frame count of the last read sequence
         */
        size_t frame_count_;
    public:
        /**
         * Initializes the algorithm wih the given values.
         * @param max_frame_skip The maximum edge length to link objects
         * @param penalty_value The Edge value to link to source and sink
         * @param max_tracklet_count The maximum number of tracklets to create
         */
        TwoStage(size_t max_frame_skip, double penalty_value,
                 size_t max_tracklet_count);

        /**
         * Creates a graph with vertices for every detected object
         * @param detections The objects to use for the graph
         * @return The created graph containing the object data
         */
        DirectedGraph CreateObjectGraph(core::DetectionSequence detections);

        /**
         * Reduces the object graph into linked tracklets.
         * The last created graph is used, as well as his frame count.
         *
         */
        DirectedGraph CreateTrackletGraph();

        /**
         * Reduces the object graph into linked tracklets.
         * @param obj_graph The object graph to reduce
         * @param frame_count The frame count of the object graph
         * @return The created graph containing the tracklet data
         */
        DirectedGraph CreateTrackletGraph(DirectedGraph obj_graph, size_t frame_count);

        /**
         * Extracts the finished tracks from the last created tracklet graph.
         * @return The vector of finished object tracks
         */
        std::vector<core::Tracklet> ExtractTracks();

        /**
         * Extracts the finished tracks from the given tracklet graph.
         * @param tlt_graph The tracklet graph to extract from
         * @return The vector of finished object tracks
         */
        std::vector<core::Tracklet> ExtractTracks(DirectedGraph tlt_graph);
    };
}


#endif //GBMOT_KOERNERTRACKING_H
