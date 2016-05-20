//
// Created by wrede on 19.04.16.
//

#include <cmath>
#include "ObjectData.h"
#include "../util/MyMath.h"

namespace core
{
    ObjectData::ObjectData()
    {
        frame_index_ = 0;
        is_virtual_ = true;
        detection_score_ = 0.0;
    }

    ObjectData::ObjectData(std::size_t frame_index)
    {
        frame_index_ = frame_index;
        is_virtual_ = false;
        detection_score_ = 0.0;
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
        os << "Object in frame " << frame_index_;
    }

    double ObjectData::CompareTo(ObjectDataPtr obj) const
    {
        return 0.0;
    }

    ObjectDataPtr ObjectData::Interpolate(ObjectDataPtr obj,
                                       double fraction) const
    {
        size_t index = static_cast<size_t>(
                util::MyMath::Lerp(
                        GetFrameIndex(), obj->GetFrameIndex(), fraction));

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

    double ObjectData::GetDetectionScore()
    {
        return detection_score_;
    }
}







