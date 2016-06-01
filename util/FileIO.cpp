//
// Created by wrede on 19.04.16.
//

#include <algorithm>
#include "FileIO.h"
#include "Logger.h"
#include "dirent.h"

namespace util
{
    void FileIO::ReadCSV(Vector3d& values,
                         const std::string& filename,
                         char delimiter)
    {
        Logger::LogInfo("Reading CSV file");

        std::ifstream in(filename, std::ifstream::in);
        std::string line;

        // Read lines while the reader is in good condition
        while (in.good() && !in.eof())
        {
            getline(in, line);

            // Ignore empty lines
            if (line.size() == 0) continue;

            // Get frame index
            size_t dIndex = line.find(delimiter);
            size_t frameIndex = std::stoul(line.substr(0, dIndex).c_str());

            // Extract point values
            std::vector<double> pointValues;
            while (dIndex != std::string::npos)
            {
                line = line.substr(dIndex + 1);
                dIndex = line.find(delimiter);
                pointValues.push_back(std::stof(line.substr(0, dIndex).c_str()));
            }

            // Add point data to detection data
            while (frameIndex >= values.size())
            {
                values.push_back(std::vector<std::vector<double>>());
            }

            values[frameIndex].push_back(pointValues);
        }

        in.close();

        Logger::LogDebug("frame count " + std::to_string(values.size()));
    }

    void FileIO::ReadCSV(Vector2d& values,
                         const std::string& filename,
                         char delimiter)
    {
        Logger::LogInfo("Reading CSV file");

        std::ifstream in(filename, std::ifstream::in);
        std::string line;

        // Read lines while the reader is in good condition and the
        // end of file is not reached
        while (in.good() && !in.eof())
        {
            getline(in, line);

            // Ignore empty lines
            if (line.size() == 0) continue;

            // Extract point values
            size_t dIndex;
            std::vector<double> pointValues;
            do
            {
                dIndex = line.find(delimiter);

                pointValues.push_back(
                        std::stof(line.substr(0, dIndex).c_str()));

                line = line.substr(dIndex + 1);
            }
            while (dIndex != std::string::npos);

            // Add point data to detection data
            values.push_back(pointValues);
        }

        in.close();

        Logger::LogDebug("line count " + std::to_string(values.size()));
    }

    void FileIO::ListFiles(const std::string& folder,
                           std::vector<std::string>& file_names,
                           bool sort)
    {
        Logger::LogInfo("Listing files in folder");

        DIR* dir;
        struct dirent *ent;
        if ((dir = opendir(folder.c_str())) != NULL)
        {
            int offset = 2;
            while ((ent = readdir(dir)) != NULL)
            {
                if (offset <= 0)
                {
                    file_names.push_back(ent->d_name);
                }
                offset--;
            }
            closedir(dir);

            if (sort)
            {
                std::sort(file_names.begin(), file_names.end());
            }

            Logger::LogDebug("file count " + std::to_string(file_names.size()));
        }
        else
        {
            Logger::LogError("Could not open folder");
        }
    }

    void FileIO::WriteCSVMatlab(DirectedGraph& graph,
                                const std::string& file_name,
                                char delimiter)
    {
        std::ofstream out(file_name, std::ofstream::out);

        // Iterate all outgoing edges of every vertex
        EdgeWeightMap weights = boost::get(boost::edge_weight, graph);
        VertexIndexMap indices = boost::get(boost::vertex_index, graph);
        boost::graph_traits<DirectedGraph>::vertex_iterator vi, vi_end;
        boost::graph_traits<DirectedGraph>::out_edge_iterator oei, oei_end;
        for (boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi)
        {
            for (boost::tie(oei, oei_end) = boost::out_edges(*vi, graph);
                 oei != oei_end; ++oei)
            {
                // Write the edge to file
                out << indices[boost::source(*oei, graph)] << delimiter
                    << indices[boost::target(*oei, graph)] << delimiter
                    << weights[*oei] << std::endl;
            }
        }

        out.close();
    }
}

