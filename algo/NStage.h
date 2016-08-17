//
// Created by wrede on 25.04.16.
//

#ifndef GBMOT_NSTAGE_H
#define GBMOT_NSTAGE_H

#include "../core/DetectionSequence.h"
#include "../core/Tracklet.h"
#include "../graph/Definitions.h"

namespace algo
{
    /**
     * Implementation of the two-staged graph-based multi-object tracker.
     * Extended to allow N stages.
     */
    class NStage
    {
    private:
        /**
         * Maximum edge length to link object
         */
        std::vector<size_t> max_frame_skips_;

        /**
         * Edge value to link to source and sink
         */
        std::vector<double> penalty_values_;

        /**
         * Maximum dijkstra iterations / number of tracklets to create
         */
        std::vector<size_t> max_tracklet_counts_;

        /**
         * Number of iterations
         */
        size_t iterations_;

        /**
         * The maximum edge weights the edge can have to be created in the initial graph building
         */
        double edge_weight_threshold_;

        /**
         * The constraints to ensure when creating edges
         */
        std::vector<std::unordered_map<std::string, double>> constraints_;

        /**
         * Creates a graph with vertices for every detected object
         *
         * @param graph The graph to write into
         * @param detections The objects to use for the graph
         * @param constraints The constraints to assure when creating edges
         */
        void CreateObjectGraph(DirectedGraph & graph, core::DetectionSequence & detections);

        /**
         * Reduces the object graph into linked tracklets.
         *
         * @param obj_graph The object graph to reduce
         * @param tlt_graph The graph to write the tracklets in
         * @param frame_count The frame count of the object graph
         * @param iteration The current iteration
         */
        void CreateTrackletGraph(DirectedGraph & obj_graph, DirectedGraph & tlt_graph,
                                 size_t frame_count, size_t iteration);

        /**
         * Extracts the finished tracks from the given tracklet graph.
         *
         * @param tlt_graph The tracklet graph to extract from
         * @param depth The depth to flatten the tracklets to
         * @param tracks The vector to write the extracted tracks in
         */
        void ExtractTracks(DirectedGraph& tlt_graph, size_t depth,
                           std::vector<core::TrackletPtr> & tracks);
    public:
        /**
         * Initializes the algorithm wih the given values.
         * The number of stages is determined by the size of the given
         * vectors.
         *
         * @param max_frame_skip The maximum edge length to link objects
         * @param penalty_value The edge value to link to source and sink
         * @param max_tracklet_count The maximum number of tracklets to create
         * @param edge_weight_threshold The maximum weight an edge can have in the initial graph,
         *                              edges with higher weights are discarded
         * @param constraints The constraints to ensure when creating edges
         */
        NStage(std::vector<size_t> max_frame_skip,
               std::vector<double> penalty_value,
               std::vector<size_t> max_tracklet_count,
               double edge_weight_threshold,
               std::vector<std::unordered_map<std::string, double>> constraints);

        /**
         * Runs the algorithm on the specified sequence and stores the found tracks into the
         * specified tracks vector.
         *
         * @param sequence The detection values to use
         * @param tracks A vector to store the found tracks in
         */
        void Run(core::DetectionSequence & sequence,
                 std::vector<core::TrackletPtr> & tracks);
    };
}


#endif //GBMOT_NSTAGE_H
