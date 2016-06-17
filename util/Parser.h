//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_PARSE_H
#define GBMOT_PARSE_H

#include "../core/DetectionSequence.h"
#include "../core/ObjectData.h"
#include "Grid.h"
#include "FileIO.h"
#include <opencv2/core/core.hpp>

namespace util
{
    typedef std::vector<std::vector<std::vector<double>>> Vector3d;
    typedef std::vector<std::vector<double>> Vector2d;

    /**
     * Utility class for parsing diverse objects.
     */
    class Parser
    {
    public:
        static const std::string KEY_FRAME;
        static const std::string KEY_ID;
        static const std::string KEY_SCORE;
        static const std::string KEY_X;
        static const std::string KEY_Y;
        static const std::string KEY_Z;
        static const std::string KEY_WIDTH;
        static const std::string KEY_HEIGHT;
        static const std::string KEY_DEPTH;
        static const std::string KEY_ANGLE;

        /**
         * Parses the specified values into the specified sequence.
         * The used format is ObjectData2D.
         *
         * @param values The input values
         * @param sequence The output sequence containing the parsed values
         * @param image_width The width of the image used for normalized
         *                    coordinates
         * @param image_height The height of the image used for normalized
         *                     coordinates
         * @param temporal_weight The temporal weight
         * @param spatial_weight The spatial weight
         */
        static void ParseObjectData2D(ValueMapVector& values,
                                       core::DetectionSequence& sequence,
                                       double image_width,
                                       double image_height,
                                       double temporal_weight,
                                       double spatial_weight);

        /**
         * Parses the specified values into the specified sequence.
         * The used format is ObjectDataBox.
         *
         * @param values The input values
         * @param sequence The output sequence containing the parsed values
         * @param image_width The width of the image used for normalized
         *                    coordinates
         * @param image_height The height of the image used for normalized
         *                     coordinates
         * @param temporal_weight The temporal weight
         * @param spatial_weight The spatial weight
         */
        static void ParseObjectDataBox(ValueMapVector& values,
                                       core::DetectionSequence& sequence,
                                       double image_width,
                                       double image_height,
                                       double temporal_weight,
                                       double spatial_weight);

        /**
         * Parses the specified values into the specified sequence.
         * The used format is ObjectDataAngular.
         *
         * @param values The input values
         * @param sequence The sequence to store the created objects in
         * @param image_width The width of the image used for normalized
         *                    coordinates
         * @param image_height The height of the image used for normalized
         *                     coordinates
         * @param temporal_weight The temporal weight
         * @param spatial_weight The spatial weight
         * @param angular_weight The angular weight
         */
        static void ParseObjectDataAngular(ValueMapVector& values,
                                           core::DetectionSequence& sequence,
                                           double image_width,
                                           double image_height,
                                           double temporal_weight,
                                           double spatial_weight,
                                           double angular_weight);

        /**
         * Parses the given sequence into a grid.
         * The sequence data need to be a ObjectData2D.
         * The frame index is the depth of the grid.
         *
         * @param sequence The detection sequence to parse
         * @param start The first frame to use
         * @param stop The first frame not to use
         * @param min_x The minimal x value
         * @param max_x The maximal x value
         * @param res_x The number of cells on the x axis
         * @param min_y The minimal y value
         * @param max_y The maximal y value
         * @param res_y The number of cells on the y axis
         * @return The grid with the detection values
         */
        static Grid ParseGrid(
                core::DetectionSequence& sequence,
                size_t start, size_t stop,
                double min_x, double max_x, int res_x,
                double min_y, double max_y, int res_y);
    };
}


#endif //GBMOT_PARSE_H
