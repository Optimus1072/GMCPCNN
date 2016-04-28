//
// Created by wrede on 19.04.16.
//

#include "IO.h"

namespace util
{
    namespace io
    {
        void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector3d &values)
        {
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
                size_t frameIndex = std::stoul(line.substr(0, dIndex).c_str()) - 1;

                // Extract point values
                std::vector<double> pointValues;
                while (dIndex != std::string::npos)
                {
                    line = line.substr(dIndex + 1);
                    dIndex = line.find(delimiter);
                    pointValues.push_back(std::stof(line.substr(0, dIndex).c_str()));
                }

                // Add point data to detection data
                if (frameIndex >= values.size())
                {
                    values.push_back(std::vector<std::vector<double>>());
                }

                values[frameIndex].push_back(pointValues);
            }

            in.close();
        }

        void ReadCSV(
                const std::string &filename,
                const char &delimiter,
                core::Vector2d &values)
        {
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
        }

    }
}

