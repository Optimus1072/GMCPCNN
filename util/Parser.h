//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_PARSE_H
#define GBMOT_PARSE_H

#include "../core/Definitions.h"
#include "../core/DetectionSequence.h"

namespace util
{
    namespace parser
    {
        // Parses the given values with the given keys into the given sequence
        void ParseObjectDataMap(
                const std::vector<std::string> &keys,
                const core::Vector3d &values,
                core::DetectionSequence &sequence);
    };
}


#endif //GBMOT_PARSE_H
