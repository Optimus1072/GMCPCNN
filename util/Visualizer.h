//
// Created by wrede on 04.05.16.
//

#ifndef GBMOT_VISUALIZER_H
#define GBMOT_VISUALIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "../core/DetectionSequence.h"
#include "../core/Tracklet.h"
#include "dirent.h"
#include <chrono>
#include <random>

namespace util
{
    /**
     * Utility class for visualizing detection sequences or tracks.
     */
    class Visualizer
    {
    private:
        /**
         * Gets the current time in milliseconds.
         * @return the current time in ms
         */
        int GetTime();
    public:
        /**
         * Displays the given tracks in an window.
         * Use D for next frame, A for previous frame, F to toggle auto play and
         * ESC to exit.
         * If a grid size greater zero is specified a grid will be drawn.
         *
         * @param tracks The tracks to display
         * @param frame_offset The offset of the first frame
         * @param image_folder The images to use
         * @param output If the frames with the visualized tracks should be stored
         * @param output_path The path to store the images into (will need an images folder)
         * @param title The window title
         * @param first_frame The frame to start at
         * @param play_fps The FPS to use when auto play is activated
         * @param If a grid should be shown
         * @param grid_width The number of cells in a row
         * @param grid_height The number of cells in a column
         */
        void Display(std::vector<core::TrackletPtr> & tracks, size_t frame_offset,
                     std::string image_folder, bool output,
                     std::string output_path, std::string title = "Visualizer",
                     size_t first_frame = 0, int play_fps = 24,
                     bool show_grid = false, int grid_width = 0, int grid_height = 0);
    };
}


#endif //GBMOT_VISUALIZER_H
