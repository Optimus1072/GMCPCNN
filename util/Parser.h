//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_PARSE_H
#define GBMOT_PARSE_H

#include "../core/DetectionSequence.h"
#include "../core/ObjectData.h"
#include "../core/ObjectDataMap.h"
#include "../core/ObjectData3D.h"
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

        //TODO rework old parsers
        /**
         * Parses the keys and values into a DetectionSequence of ObjectDataMap
         * objects.
         * The keys are used for the third dimension in the values list.
         * @param keys A 1D vector of keys
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectDataMap(const std::vector<std::string>& keys,
                                       const Vector3d& values,
                                       core::DetectionSequence& sequence);

        /**
         * Parses the values into a DetectionSequence of ObjectData3D
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectData3D(const Vector3d& values,
                                      core::DetectionSequence& sequence);

        /**
         * Parses the values into a DetectionSequence of ObjectDataAngular objects.
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectDataAngular(const Vector3d& values,
                                           core::DetectionSequence& sequence);

        /**
         * Parses the values into a DetectionSequence of ObjectDataAngular objects.
         * The keys are used for the third dimension in the values list.
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         * @param temporal_weight The temporal weight
         * @param spatial_weight The spatial weight
         * @param angular_weight The angular weight
         */
        static void ParseObjectDataAngular(const Vector3d& values,
                                           core::DetectionSequence& sequence,
                                           double temporal_weight,
                                           double spatial_weight,
                                           double angular_weight);
        /**
         * Parses the given sequence into a grid.
         * The sequence data need to be a ObjectData2D.
         * The frame index is the depth of the grid.
         * @param sequence The detection sequence to parse
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
                double min_x, double max_x, int res_x,
                double min_y, double max_y, int res_y);

        //TODO comment
        static void ParseObjectDataBox(ValueMapVector& values,
                                       core::DetectionSequence& sequence,
                                       double image_width = 1.0,
                                       double image_height = 1.0,
                                       double temporal_weight = 1.0,
                                       double spatial_weight = 1.0);
    };
}


#endif //GBMOT_PARSE_H
