//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_FILEIO_H
#define GBMOT_FILEIO_H

#include "../core/Definitions.h"
#include <string>
#include <fstream>

namespace util
{
    // Utility class for file in- and output.
    namespace io
    {
        // Reads the CSV file and stores the values in a 3D array:
        // [frame_index][object_in_frame_index][value_in_object_index]
        void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector3d &values);
        // Reads the CSV file and stores the values in a 2D array:
        // [row_index][value_index]
        void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector2d &values);
    };
}


#endif //GBMOT_FILEIO_H
