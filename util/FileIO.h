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
#include "../core/Tracklet.h"

namespace util
{
    typedef std::vector<std::vector<std::vector<double>>> Vector3d;
    typedef std::vector<std::vector<double>> Vector2d;
    typedef std::unordered_map<std::string, double> ValueMap;
    typedef std::vector<ValueMap> ValueMapVector;

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
         *
         * @param values The 3D array of values to store the read values in
         * @param file_name The name of the file to read
         * @param delimiter The value delimiter of the file
         */
        static void ReadCSV(Vector3d& values, const std::string& file_name, char delimiter);

        /**
         * Reads a CSV file and stores the values in a 2D array.
         * The first dimension is the row and the second the value in that row.
         *
         * @param values The 2D array of values to store the read values in
         * @param file_name The name of the file to read
         * @param delimiter The value delimiter of the file
         */
        static void ReadCSV(Vector2d& values, const std::string& file_name, char delimiter);

        /**
         * Lists all file names in the given folder.
         *
         * @param folder The folder to look into
         * @param file_names The name of the files in the folder
         * @param sort True, if the files should be sorted alphabetically
         */
        static void ListFiles(const std::string& folder, std::vector<std::string>& file_names,
                              bool sort = true);

        /**
         * Writes the specified graph into a CSV file with an format readable by
         * Matlab.
         *
         * @param graph The graph to write
         * @param file_name The name of the file to write
         */
        static void WriteCSVMatlab(DirectedGraph& graph, const std::string& file_name);

        /**
         * Writes the specified multi predecessor map into a CSV format
         * readable by Matlab to display all paths in the corresponding graph.
         *
         * @param map The multi predecessor map to extract the paths from
         * @param source The sink, this is the vertex where all paths begin
         * @param sink The sink, this is the vertex where all paths end
         * @param file_name The name of the file to write
         */
        static void  WriteCSVMatlab(MultiPredecessorMap& map, Vertex& source, Vertex& sink,
                                    const std::string& file_name);

        /**
         * Reads a CSV file.
         * The first line of the CSV file is a header specifying the keys.
         * The values are stored with their specified key into one map per line.
         *
         * @param values A vector of maps to store the key-value pairs into
         * @param file_name The name of the file to read
         * @param delimiter The value delimiter of the file
         */
        static void ReadCSV(ValueMapVector& values, const std::string& file_name, char delimiter);

        /**
         * Reads a CSV file.
         * The header specifies the keys.
         * The values are stored with their specified key into one map per line.
         *
         * @param values A vector of maps to store the key-value pairs into
         * @param header A string containing the keys separated by the delimiter
         * @param file_name The name of the file to read
         * @param delimiter The value delimiter of the file
         */
        static void ReadCSV(ValueMapVector& values, const std::string& header,
                            const std::string& file_name, char delimiter);

        //TODO comment
        static void WriteCSV(std::vector<core::TrackletPtr>& tracks, const std::string& file_name,
                             char delimiter);
    };
}


#endif //GBMOT_FILEIO_H
