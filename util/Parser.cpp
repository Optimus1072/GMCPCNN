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
    const std::string Parser::KEY_WIDTH = "width";
    const std::string Parser::KEY_HEIGHT = "height";
    const std::string Parser::KEY_ANGLE = "angle";

    void Parser::ParseObjectData2D(ValueMapVector& values,
                                   core::DetectionSequence& sequence,
                                   double image_width, double image_height,
                                   double temporal_weight,
                                   double spatial_weight)
    {
        util::Logger::LogInfo("Parsing ObjectData2D detections");

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
            double score = values[line_index][KEY_SCORE];

            cv::Point2d point(x, y);

            core::ObjectData2DPtr object(
                    new core::ObjectData2D(frame, point));

            object->SetTemporalWeight(temporal_weight);
            object->SetSpatialWeight(spatial_weight);
            object->SetDetectionScore(
                    util::MyMath::InverseLerp(min_score, max_score, score));

            sequence.AddObject(object);

            obj_count++;
        }

        util::Logger::LogDebug("objects parsed " + std::to_string(obj_count));
        util::Logger::LogDebug("frame count " + std::to_string(sequence.GetFrameCount()));
    }

    void Parser::ParseObjectDataAngular(ValueMapVector& values,
                                        core::DetectionSequence& sequence,
                                        double image_width,
                                        double image_height,
                                        double temporal_weight,
                                        double spatial_weight,
                                        double angular_weight)
    {
        util::Logger::LogInfo("Parsing ObjectDataAngular detections");

        // Calculate max and min score to normalize the score
        double max_score = std::numeric_limits<double>::min();
        double min_score = std::numeric_limits<double>::max();
        for (size_t line_index = 0; line_index < values.size(); ++line_index)
        {
            for (size_t object_i = 0; object_i < values[line_index].size();
                 ++object_i)
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
            double angle = MyMath::Radian(values[line_index][KEY_ANGLE]);
            double score = values[line_index][KEY_SCORE];

            //TODO detection score interpolation (0.5 <-> 1.0) (experimental)
            score = util::MyMath::InverseLerp(min_score, max_score, score);
            score = score * 0.5 + 0.5;

            cv::Point2d point(x, y);

            core::ObjectDataAngularPtr object(
                    new core::ObjectDataAngular(frame, point, angle));

            object->SetTemporalWeight(temporal_weight);
            object->SetSpatialWeight(spatial_weight);
            object->SetAngularWeight(angular_weight);
            object->SetDetectionScore(score);

            sequence.AddObject(object);

            obj_count++;
        }

        util::Logger::LogDebug("objects parsed " + std::to_string(obj_count));
        util::Logger::LogDebug("frame count " + std::to_string(sequence.GetFrameCount()));
    }

    void Parser::ParseObjectDataBox(ValueMapVector& values,
                                    core::DetectionSequence& sequence,
                                    double image_width,
                                    double image_height,
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

            core::ObjectDataBoxPtr object(
                    new core::ObjectDataBox(frame, point, size));

            object->SetTemporalWeight(temporal_weight);
            object->SetSpatialWeight(spatial_weight);
            object->SetDetectionScore(
                    util::MyMath::InverseLerp(min_score, max_score, score));

            sequence.AddObject(object);

            obj_count++;
        }

        util::Logger::LogDebug("objects parsed " + std::to_string(obj_count));
        util::Logger::LogDebug("frame count " + std::to_string(sequence.GetFrameCount()));
    }

    Grid Parser::ParseGrid(core::DetectionSequence& sequence,
                           size_t start, size_t stop,
                           double min_x, double max_x, int res_x,
                           double min_y, double max_y, int res_y)
    {
        stop = std::min(stop, sequence.GetFrameCount());
        int res_z = (int) (stop - start);
        double width = max_x - min_x;
        double height = max_y - min_y;
        double depth = (double) (stop - start);
        Grid grid(res_x, res_y, res_z, width, height, depth);

        // Fill with elements with detection score of zero
        for (int z = 0; z < grid.GetDepthCount(); ++z)
        {
            for (int y = 0; y < grid.GetHeightCount(); ++y)
            {
                for (int x = 0; x < grid.GetWidthCount(); ++x)
                {
                    core::ObjectDataPtr value(
                            new core::ObjectData((size_t)(z + start)));

                    grid.SetValue(value, x, y, z);
                }
            }
        }

        // Add the detections
        for (size_t f = start; f < stop; ++f)
        {
            for (size_t i = 0; i < sequence.GetObjectCount(f); ++i)
            {
                core::ObjectDataPtr original_value = sequence.GetObject(f, i);
                core::ObjectData2DPtr value =
                        std::static_pointer_cast<core::ObjectData2D>(original_value);

                double x = value->GetPosition().x;
                double y = value->GetPosition().y;
                double stored_score = grid.GetValue(x, y, f - start)->GetDetectionScore();

                // Only overwrite if the new detection score is at least as good
                // as the detection score of the already stored value
                if (stored_score <= original_value->GetDetectionScore())
                {
                    grid.SetValue(original_value, x, y, f - start);
                }
            }
        }

        return grid;
    }
}


