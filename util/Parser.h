//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_PARSE_H
#define GBMOT_PARSE_H

#include "../core/Definitions.h"
#include "../core/DetectionSequence.h"
#include "../core/ObjectData.h"
#include "../core/ObjectDataMap.h"
#include "../core/ObjectData3D.h"
#include <opencv2/core/core.hpp>

namespace util
{
    /**
     * Utility class for parsing diverse objects.
     */
    class Parser
    {
    public:
        //TODO define csv value order
        /**
         * Parses the keys and values into a DetectionSequence of ObjectDataMap
         * objects.
         * The keys are used for the third dimension in the values list.
         * @param keys A 1D vector of keys
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectDataMap(
                const std::vector<std::string>& keys,
                const core::Vector3d& values,
                core::DetectionSequence& sequence);

        /**
         * Parses the values into a DetectionSequence of ObjectData3D
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectData3D(
                const core::Vector3d& values,
                core::DetectionSequence& sequence);

        /**
         * Parses the values into a DetectionSequence of ObjectDataAngular objects.
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectDataAngular(
                const core::Vector3d& values,
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
        static void ParseObjectDataAngular(const core::Vector3d& values,
                                           core::DetectionSequence& sequence,
                                           double temporal_weight,
                                           double spatial_weight,
                                           double angular_weight);
    };
}


#endif //GBMOT_PARSE_H
