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
    /**
     * Utility class for file in- and output.
     */
    class FileIO
    {
    public:
        /**
         * Reads a CSV file and stores the values in a 3D array.
         * The first dimension is the first value of each row, used as a
         * index to bundle multiple rows with the same first value into a
         * single vector.
         * The second dimension is the row in the row bundle.
         * The third dimension is the value in that row.
         * @param filename The filename to read from
         * @param delimiter The delimiter used to separate the values in the file
         * @param values The 3D array of values to store the read values in
         */
        static void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector3d &values);
        /**
         * Reads a CSV file and stores the values in a 2D array.
         * The first dimension is the row and the second the value in that row.
         * @param filename The filename to read from
         * @param delimiter The delimiter used to separate the values in the file
         * @param values The 2D array of values to store the read values in
         */
        static void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector2d &values);
    };
}


#endif //GBMOT_FILEIO_H
