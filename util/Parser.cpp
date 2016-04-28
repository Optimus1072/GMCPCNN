//
// Created by wrede on 22.04.16.
//

#include "Parser.h"

namespace util
{
    namespace parser
    {
        void ParseObjectDataMap(
                const std::vector<std::string> &keys,
                const core::Vector3d &values,
                core::DetectionSequence &sequence)
        {
            for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
            {
                for (size_t object_i = 0; object_i < values[frame_i].size();
                     ++object_i)
                {
                    core::ObjectDataMap object(frame_i, keys, values[frame_i][object_i]);

                    sequence.AddObject(object);
                }
            }
        }
    }
}


