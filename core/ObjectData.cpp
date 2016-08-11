//
// Created by wrede on 19.04.16.
//

#include <cmath>
#include "ObjectData.h"
#include "../util/MyMath.h"

namespace core
{
    const std::string ObjectData::CONSTRAINT_FRAME_DIFFERENCE = "frame_difference";
    const std::string ObjectData::CONSTRAINT_SCORE_DIFFERENCE = "score_difference";

    ObjectData::ObjectData()
            : frame_index_(0), is_virtual_(true), detection_score_(0.0)
    {
        /* EMPTY */
    }

    ObjectData::ObjectData(std::size_t frame_index)
            : frame_index_(frame_index), is_virtual_(false), detection_score_(0.0)
    {
        /* EMPTY */
    }

    std::size_t ObjectData::GetFrameIndex() const
    {
        return frame_index_;
    }

    bool ObjectData::IsVirtual() const
    {
        return is_virtual_;
    }

    void ObjectData::Print(std::ostream &os) const
    {
        if (is_virtual_)
        {
            os << "Object{-}";
        }
        else
        {
            os << "Object{" << frame_index_ << "}";
        }
    }

    double ObjectData::CompareTo(ObjectDataPtr obj) const
    {
        /* EMPTY */
        return 0.0;
    }

    bool ObjectData::IsWithinConstraints(ObjectDataPtr obj,
                                         std::unordered_map<std::string, double> & constraints)
    const
    {
        if (constraints.count(CONSTRAINT_FRAME_DIFFERENCE) > 0)
        {
            size_t frame_difference;

            // check the size to prevent negative values for unsigned type
            if (frame_index_ > obj->GetFrameIndex()) {
                frame_difference = frame_index_ - obj->GetFrameIndex();
            } else {
                frame_difference = obj->GetFrameIndex() - frame_index_;
            }

            if (frame_difference > constraints[CONSTRAINT_FRAME_DIFFERENCE]) {
                return false;
            }
        }

        if (constraints.count(CONSTRAINT_SCORE_DIFFERENCE) > 0) {
            double score_difference = fabs(detection_score_ - obj->GetDetectionScore());

            if (score_difference > constraints[CONSTRAINT_SCORE_DIFFERENCE]) {
                return false;
            }
        }

        return true;
    }

    ObjectDataPtr ObjectData::Interpolate(ObjectDataPtr obj, double fraction) const
    {
        size_t index = static_cast<size_t>(
                util::MyMath::Lerp(GetFrameIndex(), obj->GetFrameIndex(), fraction));

        return ObjectDataPtr(new ObjectData(index));
    }

    std::ostream& operator<<(std::ostream &os, const ObjectData &obj)
    {
        obj.Print(os);
        return os;
    }

    void ObjectData::SetFrameIndex(size_t index)
    {
        frame_index_ = index;
    }

    void ObjectData::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        /* EMPTY */
    }

    void ObjectData::SetDetectionScore(double score)
    {
        detection_score_ = score;
    }

    double ObjectData::GetDetectionScore() const
    {
        return detection_score_;
    }

    std::string ObjectData::ToString(char delimiter) const
    {
        return std::to_string(frame_index_);
    }
}







