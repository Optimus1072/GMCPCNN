//
// Created by wrede on 02.06.16.
//

#ifndef GBMOT_BERCLAZ_H
#define GBMOT_BERCLAZ_H

#include "../graph/Definitions.h"
#include "../core/DetectionSequence.h"
#include "../core/Tracklet.h"
#include "../util/Grid.h"

namespace algo
{
    /**
     * Algorithm for tracking multiple object in a frame sequence.
     * The algorithm needs the location of detections.
     */
    class Berclaz
    {
    private:
        const double VIRTUAL_EDGE_WEIGHT = 0.0;
        const double MAX_SCORE_VALUE = 0.999999;
        const double MIN_SCORE_VALUE = 0.000001;

        /**
         * Horizontal grid resolution
         */
        int h_res_;

        /**
         * Vertical grid resolution
         */
        int v_res_;

        /**
         * The number of cells a detection can move within one frame
         */
        int vicinity_size_;

        /**
         * Creates a graph from the given sequence.
         *
         * @param graph The graph to write into
         * @param source A reference to the source vertex
         * @param sink A reference to the sink vertex
         * @param grid The detection values as a grid
         */
        void CreateGraph(DirectedGraph& graph, Vertex& source, Vertex& sink,
                         util::Grid& grid);

        /**
         * Extracts the final tracks from the given graph and predecessor map.
         *
         * @param graph The graph to read the values from
         * @param map The predecessor map to read the paths from
         * @param origin The vertex to start the reverse path traversal from
         * @param tracks The vector to fill with the extracted tracks
         */
        void ExtractTracks(DirectedGraph& graph,
                           MultiPredecessorMap& map, Vertex origin,
                           std::vector<core::TrackletPtr> & tracks);

        //TODO comment
        void ConnectTracks(std::vector<core::TrackletPtr> & tracks);
    public:
        /**
         * Instantiate with the given parameters.
         *
         * @param h_res The horizontal grid resolution
         * @param v_res The vertical grid resolution
         * @param vicinity_size The maximum number of cells a detection can skip
         *                      within one frame
         */
        Berclaz(int h_res, int v_res, int vicinity_size);

        /**
         * Runs the algorithm on the given sequence. Splits the sequence into
         * batches to allow faster processing.
         *
         * @param sequence The detection to use
         * @param batch_size The number of frames one batch will have at maximum
         * @param max_track_count The maximum number of tracks to extract
         * @param tracks The vector to store the found tracks into
         * @param filter The filter used to convolve the detections in the grid
         */
        void Run(core::DetectionSequence & sequence, size_t batch_size,
                 size_t max_track_count, std::vector<core::TrackletPtr> & tracks,
                 util::Filter2D & filter);
    };
}


#endif //GBMOT_BERCLAZ_H
