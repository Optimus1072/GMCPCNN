//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_FILEIO_H
#define GBMOT_FILEIO_H

#include <string>
#include <fstream>
#include <vector>
#include "../core/ObjectData.h"
#include "../graph/Definitions.h"

namespace util
{
    typedef std::vector<std::vector<std::vector<double>>> Vector3d;
    typedef std::vector<std::vector<double>> Vector2d;

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
         * @param values The 3D array of values to store the read values in
         * @param filename The filename to read from
         * @param delimiter The delimiter used to separate the values in the file
         */
        static void ReadCSV(Vector3d& values,
                            const std::string& filename,
                            char delimiter = ';');

        /**
         * Reads a CSV file and stores the values in a 2D array.
         * The first dimension is the row and the second the value in that row.
         * @param values The 2D array of values to store the read values in
         * @param filename The filename to read from
         * @param delimiter The delimiter used to separate the values in the file
         */
        static void ReadCSV(Vector2d& values,
                            const std::string& filename,
                            char delimiter = ';');

        /**
         * Lists all file names in the given folder.
         * @param folder The folder to look into
         * @param file_names The vector to store the file names into
         * @param sort True, if the files should be sorted alphabetically
         */
        static void ListFiles(const std::string& folder,
                              std::vector<std::string>& file_names,
                              bool sort = true);

        /**
         * Writes the given graph into a CSV file with an format readable by
         * Matlab.
         * @param graph The graph to write
         * @param file_name The name of the file to write into
         * @param delimiter The delimiter to use
         */
        static void WriteCSVMatlab(DirectedGraph& graph,
                                   const std::string& file_name,
                                   char delimiter = ';');
    };
}


#endif //GBMOT_FILEIO_H
