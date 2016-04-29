//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_PARSE_H
#define GBMOT_PARSE_H

#include "../core/Definitions.h"
#include "../core/DetectionSequence.h"

namespace util
{
    /**
     * Utility class for parsing diverse objects.
     */
    class Parser
    {
    public:
        /**
         * Parses the keys and values into a DetectionSequence of ObjectDataMap
         * objects.
         * The keys are used for the third dimension in the values list.
         * @see DetectionSequence
         * @see ObjectDataMap
         * @param keys A 1D vector of keys
         * @param values A 3D vector of values
         * @param sequence The sequence to store the created objects in
         */
        static void ParseObjectDataMap(
                const std::vector<std::string> &keys,
                const core::Vector3d &values,
                core::DetectionSequence &sequence);
    };
}


#endif //GBMOT_PARSE_H
