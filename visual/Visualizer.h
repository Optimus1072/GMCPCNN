//
// Created by wrede on 04.05.16.
//

#ifndef GBMOT_VISUALIZER_H
#define GBMOT_VISUALIZER_H

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include "../core/DetectionSequence.h"
#include "../core/ObjectData3D.h"
#include "../core/Tracklet.h"
#include "dirent.h"
#include <chrono>
#include <random>

namespace visual
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
         * Displays the given sequence in an window.
         * Use D for next frame, A for previous frame, F to toggle auto play and
         * ESC to exit.
         * @param sequence The sequence of detection data to display
         * @param image_folder The images to use
         * @param title The window title
         * @param first_frame The frame to start at
         * @param play_fps The FPS to use when auto play is activated.
         */
        void Display(core::DetectionSequence& sequence,
                     std::string image_folder,
                     std::string title = "Visualizer", size_t first_frame = 0,
                     int play_fps = 24);

        /**
         * Displays the given tracks in an window.
         * Use D for next frame, A for previous frame, F to toggle auto play and
         * ESC to exit.
         * @param tracks The tracks to display
         * @param image_folder The images to use
         * @param title The window title
         * @param first_frame The frame to start at
         * @param play_fps The FPS to use when auto play is activated.
         */
        void Display(std::vector<core::TrackletPtr>& tracks,
                     std::string image_folder,
                     std::string title = "Visualizer", size_t first_frame = 0,
                     int play_fps = 24);
    };
}


#endif //GBMOT_VISUALIZER_H
