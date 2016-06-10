//
// Created by wrede on 22.04.16.
//

#include "Parser.h"
#include "MyMath.h"
#include "Logger.h"
#include "../core/ObjectDataAngular.h"
#include "../core/ObjectDataBox.h"

namespace util
{
    const std::string Parser::KEY_FRAME = "frame";
    const std::string Parser::KEY_ID = "id";
    const std::string Parser::KEY_SCORE = "score";
    const std::string Parser::KEY_X = "x";
    const std::string Parser::KEY_Y = "y";
    const std::string Parser::KEY_Z = "z";
    const std::string Parser::KEY_WIDTH = "width";
    const std::string Parser::KEY_HEIGHT = "height";
    const std::string Parser::KEY_DEPTH = "depth";
    const std::string Parser::KEY_ANGLE = "angle";

    void Parser::ParseObjectDataMap(const std::vector<std::string>& keys,
                                    const Vector3d& values,
                                    core::DetectionSequence& sequence)
    {
        util::Logger::LogInfo("Parsing ObjectDataMap");

        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                core::ObjectDataMapPtr object(
                        new core::ObjectDataMap(frame_i, keys,
                                                values[frame_i][object_i]));

                sequence.AddObject(object);
            }
        }
    }

    void Parser::ParseObjectData3D(const Vector3d& values,
                                   core::DetectionSequence& sequence)
    {
        util::Logger::LogInfo("Parsing ObjectData3D");

        // Calculate max and min score to normalize the score
        double max_score = std::numeric_limits<double>::min();
        double min_score = std::numeric_limits<double>::max();
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double score = values[frame_i][object_i][1];
                if (score > max_score)
                {
                    max_score = score;
                }
                if (score < min_score)
                {
                    min_score = score;
                }
            }
        }

        // Create objects
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double score = values[frame_i][object_i][1];
                double x = values[frame_i][object_i][2];
                double y = values[frame_i][object_i][3];
                double z = 0.0;
                cv::Point3d point(x, y, z);

                core::ObjectData3DPtr object(new core::ObjectData3D(frame_i, point));

                object->SetDetectionScore(util::MyMath::InverseLerp(min_score, max_score, score));

                sequence.AddObject(object);
            }
        }
    }

    void Parser::ParseObjectDataAngular(const Vector3d& values,
                                        core::DetectionSequence& sequence)
    {
        ParseObjectDataAngular(values, sequence, 1.0, 1.0, 1.0);
    }

    void Parser::ParseObjectDataAngular(const Vector3d& values,
                                        core::DetectionSequence& sequence,
                                        double temporal_weight,
                                        double spatial_weight,
                                        double angular_weight)
    {
        util::Logger::LogInfo("Parsing ObjectDataAngular");

        // Calculate max and min score to normalize the score
        double max_score = std::numeric_limits<double>::min();
        double min_score = std::numeric_limits<double>::max();
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double score = values[frame_i][object_i][1];
                if (score > max_score)
                {
                    max_score = score;
                }
                if (score < min_score)
                {
                    min_score = score;
                }
            }
        }

        util::Logger::LogDebug("min score " + std::to_string(min_score));
        util::Logger::LogDebug("max score " + std::to_string(max_score));

        // Create objects
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double angle = MyMath::Radian(values[frame_i][object_i][0]);
                double score = values[frame_i][object_i][1];
                double x = values[frame_i][object_i][2];
                double y = values[frame_i][object_i][3];
                cv::Point2d point(x, y);

                //util::Logger::LogDebug("score " + std::to_string(score));

                core::ObjectDataAngularPtr object(
                        new core::ObjectDataAngular(frame_i, point, angle,
                                                    temporal_weight,
                                                    spatial_weight,
                                                    angular_weight));

                object->SetDetectionScore(util::MyMath::InverseLerp(min_score, max_score, score));

                sequence.AddObject(object);
            }
        }
    }

    Grid Parser::ParseGrid(core::DetectionSequence& sequence,
                           double min_x, double max_x, int res_x,
                           double min_y, double max_y, int res_y)
    {
        int res_z = (int) sequence.GetFrameCount();
        double width = max_x - min_x;
        double height = max_y - min_y;
        double depth = sequence.GetFrameCount();
        Grid grid(res_x, res_y, res_z, width, height, depth);

        // Fill with elements with detection score of zero
        for (int z = 0; z < grid.GetDepthCount(); ++z)
        {
            for (int y = 0; y < grid.GetHeightCount(); ++y)
            {
                for (int x = 0; x < grid.GetWidthCount(); ++x)
                {
                    core::ObjectDataPtr value(new core::ObjectData((size_t)z));
                    grid.SetValue(value, x, y, z);
                }
            }
        }

        // Add the detections
        for (size_t f = 0; f < sequence.GetFrameCount(); ++f)
        {
            for (size_t i = 0; i < sequence.GetObjectCount(f); ++i)
            {
                core::ObjectDataPtr original_value = sequence.GetObject(f, i);
                core::ObjectData2DPtr value =
                        std::static_pointer_cast<core::ObjectData2D>(original_value);

                grid.SetValue(original_value,
                              value->GetPosition().x,
                              value->GetPosition().y,
                              f);
            }
        }

        return grid;
    }

    void Parser::ParseObjectDataBox(ValueMapVector& values,
                                    core::DetectionSequence& sequence,
                                    double image_width, double image_height,
                                    double temporal_weight,
                                    double spatial_weight)
    {
        util::Logger::LogInfo("Parsing ObjectDataBox detections");

        // Calculate max and min score to normalize the score
        double max_score = std::numeric_limits<double>::min();
        double min_score = std::numeric_limits<double>::max();
        for (size_t line_index = 0; line_index < values.size(); ++line_index)
        {
            double score = values[line_index][KEY_SCORE];
            if (score > max_score)
            {
                max_score = score;
            }
            if (score < min_score)
            {
                min_score = score;
            }
        }

        util::Logger::LogDebug("min score " + std::to_string(min_score));
        util::Logger::LogDebug("max score " + std::to_string(max_score));

        // Create objects
        size_t obj_count = 0;
        for (size_t line_index = 0; line_index < values.size(); ++line_index)
        {
            size_t frame = (size_t) fabs(values[line_index][KEY_FRAME]);
            double x = values[line_index][KEY_X] / image_width;
            double y = values[line_index][KEY_Y] / image_height;
            double width = values[line_index][KEY_WIDTH] / image_width;
            double height = values[line_index][KEY_HEIGHT] / image_height;
            double score = values[line_index][KEY_SCORE];

            cv::Point2d point(x, y);
            cv::Point2d size(width, height);

            core::ObjectDataBoxPtr object(new core::ObjectDataBox(frame,
                                                                  point,
                                                                  size));

            object->SetTemporalWeight(temporal_weight);
            object->SetSpatialWeight(spatial_weight);
            object->SetDetectionScore(util::MyMath::InverseLerp(min_score,
                                                                max_score,
                                                                score));

            sequence.AddObject(object);

            obj_count++;
        }

        util::Logger::LogDebug("objects parsed " + std::to_string(obj_count));
        util::Logger::LogDebug("frame count " + std::to_string(sequence.GetFrameCount()));
    }
}


