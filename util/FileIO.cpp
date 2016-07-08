//
// Created by wrede on 19.04.16.
//

#include <algorithm>
#include "FileIO.h"
#include "Logger.h"
#include "dirent.h"
#include "../core/ObjectData2D.h"

namespace util
{
    void FileIO::ReadCSV(Vector3d& values, const std::string& file_name, char delimiter)
    {
        Logger::LogInfo("Read CSV file");

        std::ifstream in(file_name, std::ifstream::in);
        std::string line;

        // Read lines while the reader is in good condition
        while (in.good() && !in.eof())
        {
            getline(in, line);

            // Ignore empty lines
            if (line.size() == 0) continue;

            // Get frame index
            size_t dIndex = line.find(delimiter);
            std::string part = line.substr(0, dIndex);
            size_t frameIndex = std::stoul(part.c_str());

            // Extract point values
            std::vector<double> pointValues;
            while (dIndex != std::string::npos)
            {
                line = line.substr(dIndex + 1);
                dIndex = line.find(delimiter);
                part = line.substr(0, dIndex);

                if (part.size() > 0)
                {
                    try
                    {
                        pointValues.push_back(std::stof(part.c_str()));
                    }
                    catch (const std::exception& e)
                    {
                        /* EMPTY */
                        // Possible cause: "\n"
                    }
                }
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

    void FileIO::ReadCSV(Vector2d& values, const std::string& file_name, char delimiter)
    {
        Logger::LogInfo("Read CSV file");

        std::ifstream in(file_name, std::ifstream::in);
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
            std::string part;
            std::vector<double> pointValues;
            do
            {
                dIndex = line.find(delimiter);

                part = line.substr(0, dIndex);

                if (part.size() > 0)
                {
                    pointValues.push_back(std::stof(part.c_str()));
                }

                line = line.substr(dIndex + 1);
            }
            while (dIndex != std::string::npos);

            // Add point data to detection data
            values.push_back(pointValues);
        }

        in.close();

        Logger::LogDebug("line count " + std::to_string(values.size()));
    }

    void FileIO::ListFiles(const std::string& folder, std::vector<std::string>& file_names,
                           bool sort)
    {
        Logger::LogInfo("List files in folder");

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

    void FileIO::WriteCSVMatlab(DirectedGraph& graph, const std::string& file_name)
    {
        char delimiter = ',';

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
                unsigned long si = indices[boost::source(*oei, graph)];
                unsigned long ti = indices[boost::target(*oei, graph)];
                // Write the edge to file
                out << (si + 1) << delimiter
                    << (ti + 1) << delimiter
                    << weights[*oei] << std::endl;
            }
        }

        out.close();
    }

    void FileIO::WriteCSVMatlab(MultiPredecessorMap& map, Vertex& source, Vertex& sink,
                                const std::string& file_name)
    {
        char delimiter = ',';

        std::ofstream out(file_name, std::ofstream::out);
        for (Vertex first : map[sink])
        {
            out << (sink + 1) << delimiter << (first + 1);

            for (Vertex u = first, v = (*map[u].begin()); u != v; u = v, v = (*map[v].begin()))
            {
                out << delimiter << (v + 1);

                if (v == source)
                {
                    break;
                }
            }

            out << std::endl;
        }
        out.close();
    }

    void FileIO::ReadCSV(ValueMapVector& values, const std::string& file_name, char delimiter)
    {
        // Read the file
        std::ifstream in(file_name, std::ifstream::in);

        // Only proceed if the file could be opened
        if (!in.is_open())
        {
            throw "Unable to open file: " + file_name;
        }

        std::string line;

        // Get the first line that is not empty
        while (in.good() && !in.eof())
        {
            getline(in, line);

            if (line.size() > 0) break;
        }

        in.close();

        ReadCSV(values, line, file_name, delimiter);
    }

    void FileIO::ReadCSV(ValueMapVector& values, const std::string& header,
                         const std::string& file_name, char delimiter)
    {
        Logger::LogInfo("Read CSV file");

        // Read the file
        std::ifstream in(file_name, std::ifstream::in);

        // Only proceed if the file could be opened
        if (!in.is_open())
        {
            throw "Unable to open file: " + file_name;
        }

        std::string line, part;
        size_t d_index;

        // Split the header into separate keys
        std::vector<std::string> key_vector;
        line = header;
        do
        {
            d_index = line.find(delimiter);
            part = line.substr(0, d_index);

            key_vector.push_back(part);

            line = line.substr(d_index + 1);
        }
        while (d_index != std::string::npos);

        util::Logger::LogDebug("parsed keys:");
        for (std::string str : key_vector)
        {
            util::Logger::LogDebug(str);
        }

        // Read lines while the reader is in good condition and the
        // end of file is not reached
        while (in.good() && !in.eof())
        {
            getline(in, line);

            // Ignore empty lines
            if (line.size() == 0) continue;

            // Extract detection values
            size_t key_index = 0;
            ValueMap detection_values;
            do
            {
                d_index = line.find(delimiter);
                part = line.substr(0, d_index);

                // Try to parse the value
                double value;
                try
                {
                    value = std::stof(part.c_str());
                }
                catch (std::exception& e)
                {
                    util::Logger::LogError(e.what());
                    value = 0.0;
                }

                // Store the value
                detection_values[key_vector[key_index]] = value;
                ++key_index;

                line = line.substr(d_index + 1);
            }
            while (d_index != std::string::npos &&
                   key_index < key_vector.size());

            // Add point data to detection data
            values.push_back(detection_values);
        }

        util::Logger::LogDebug("parsed values in line 2:");
        for (std::string str : key_vector)
        {
            util::Logger::LogDebug(str + "=" + std::to_string(values[1][str]));
        }

        in.close();
    }

    void FileIO::WriteTracks(std::vector<core::TrackletPtr>& tracks, const std::string& file_name,
                             char delimiter)
    {
        // Get the frame range
        size_t first_frame = tracks[0]->GetFirstFrameIndex();
        size_t last_frame = tracks[0]->GetLastFrameIndex();
        for (auto track : tracks)
        {
            if (track->GetFirstFrameIndex() < first_frame)
                first_frame = track->GetFirstFrameIndex();
            if (track->GetLastFrameIndex() > last_frame)
                last_frame = track->GetLastFrameIndex();
        }

        std::ofstream out(file_name, std::ios::out);

        if (!out.is_open())
        {
            util::Logger::LogError("Unable to open file: " + file_name);
            return;
        }

        for (size_t frame = first_frame; frame <= last_frame; ++frame)
        {
            for (size_t i = 0; i < tracks.size(); ++i)
            {
                core::ObjectData2DPtr obj =
                        std::static_pointer_cast<core::ObjectData2D>(tracks[i]->GetFrameObject(frame));

                if (obj != nullptr)
                {
                    out << obj->GetPosition().x << delimiter;
                    out << obj->GetPosition().y;
                }
                else
                {
                    out << delimiter;
                }

                if (i < (tracks.size() - 1))
                {
                    out << delimiter;
                }
            }

            out << std::endl;
        }

        out.close();
    }

    void FileIO::ReadTracks(std::vector<core::TrackletPtr>& tracks, const std::string& file_name,
                            char delimiter)
    {
        std::ifstream in(file_name, std::ios::in);

        if (!in.is_open())
        {
            util::Logger::LogError("Unable to open file: " + file_name);
            return;
        }

        std::string line;
        size_t line_index = 0;
        while (in.good() && !in.eof())
        {
            getline(in, line);

            if (line.size() == 0) continue;

            std::vector<std::string> parts = split(line, delimiter);

            while (tracks.size() < (parts.size() / 2))
            {
                tracks.push_back(core::TrackletPtr(new core::Tracklet()));
            }

            for (size_t i = 1; i < parts.size(); i += 2)
            {
                if (!parts[i - 1].empty() && !parts[i].empty())
                {
                    cv::Point2d point(std::stod(parts[i - 1]), std::stod(parts[i]));

                    tracks[(i - 1) / 2]->AddPathObject(
                            core::ObjectData2DPtr(new core::ObjectData2D(line_index, point)));
                }
            }

            line_index++;
        }

        in.close();
    }

    std::vector<std::string> FileIO::split(const std::string& input, char delimiter)
    {
        std::vector<std::string> output;
        std::stringstream ss(input);
        std::string part;
        while (getline(ss, part, delimiter))
        {
            output.push_back(part);
        }
        return output;
    }
}

