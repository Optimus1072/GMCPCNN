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
     * Implementation of the two-staged graph-based multi-object tracker.
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

        /**
         * Maximum dijkstra iterations / number of tracklets to create
         */
        size_t max_tracklet_count_;
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
         * @param graph The graph to write into
         * @param detections The objects to use for the graph
         */
        void CreateObjectGraph(DirectedGraph& graph,
                               core::DetectionSequence& detections);

        /**
         * Reduces the object graph into linked tracklets.
         * @param obj_graph The object graph to reduce
         * @param tlt_graph The graph to write the tracklets in
         * @param frame_count The frame count of the object graph
         */
        void CreateTrackletGraph(DirectedGraph& obj_graph,
                                 DirectedGraph& tlt_graph, size_t frame_count);

        /**
         * Extracts the finished tracks from the given tracklet graph.
         * @param tlt_graph The tracklet graph to extract from
         * @param depth The depth to flatten the tracklets to
         * @param tracks The vector to write the extracted tracks in
         */
        void ExtractTracks(DirectedGraph& tlt_graph, size_t depth,
                           std::vector<core::TrackletPtr>& tracks);
    };
}


#endif //GBMOT_KOERNERTRACKING_H
