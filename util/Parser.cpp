//
// Created by wrede on 22.04.16.
//

#include "Parser.h"
#include "MyMath.h"
#include "Logger.h"
#include "../core/ObjectDataAngular.h"

namespace util
{
    void Parser::ParseObjectDataMap(
            const std::vector<std::string>& keys,
            const core::Vector3d& values,
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

    void Parser::ParseObjectData3D(const core::Vector3d& values,
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

    void Parser::ParseObjectDataAngular(const core::Vector3d& values,
                                        core::DetectionSequence& sequence)
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

        // Create objects
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double angle = (values[frame_i][object_i][0] * 3.14159) / 180.0;
                double score = values[frame_i][object_i][1];
                double x = values[frame_i][object_i][2];
                double y = values[frame_i][object_i][3];
                double z = 0.0;
                cv::Point3d point(x, y, z);

                core::ObjectDataAngularPtr object(new core::ObjectDataAngular(frame_i, point, angle));

                object->SetDetectionScore(util::MyMath::InverseLerp(min_score, max_score, score));

                sequence.AddObject(object);
            }
        }
    }

    void Parser::ParseObjectDataAngular(const core::Vector3d& values,
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

        // Create objects
        for (size_t frame_i = 0; frame_i < values.size(); ++frame_i)
        {
            for (size_t object_i = 0; object_i < values[frame_i].size();
                 ++object_i)
            {
                double angle = (values[frame_i][object_i][0] * 3.14159) / 180.0;
                double score = values[frame_i][object_i][1];
                double x = values[frame_i][object_i][2];
                double y = values[frame_i][object_i][3];
                double z = 0.0;
                cv::Point3d point(x, y, z);

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
}


