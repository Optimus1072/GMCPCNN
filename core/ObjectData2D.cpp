//
// Created by wrede on 02.06.16.
//

#include "ObjectData2D.h"
#include "../util/MyMath.h"

namespace core
{
    ObjectData2D::ObjectData2D(size_t frame_index, cv::Point2d position)
            : ObjectData(frame_index),
              position_(position),
              temporal_weight_(1.0),
              spatial_weight_(1.0)
    {
    }

    void ObjectData2D::SetTemporalWeight(double weight)
    {
        temporal_weight_ = weight;
    }

    void ObjectData2D::SetSpatialWeight(double weight)
    {
        spatial_weight_ = weight;
    }

    cv::Point2d ObjectData2D::GetPosition() const
    {
        return position_;
    }

    double ObjectData2D::GetTemporalWeight() const
    {
        return temporal_weight_;
    }

    double ObjectData2D::GetSpatialWeight() const
    {
        return spatial_weight_;
    }

    double ObjectData2D::CompareTo(ObjectDataPtr obj) const
    {
        ObjectData2DPtr obj_2d = std::static_pointer_cast<ObjectData2D>(obj);

        double d_temp = obj_2d->GetFrameIndex() - GetFrameIndex();
        double d_spat = util::MyMath::EuclideanDistance(position_, obj_2d->position_);

        return d_temp * temporal_weight_ + d_spat * spatial_weight_;
    }

    ObjectDataPtr ObjectData2D::Interpolate(ObjectDataPtr obj,
                                            double fraction) const
    {
        ObjectDataPtr obj_in = ObjectData::Interpolate(obj, fraction);

        ObjectData2DPtr obj_2d = std::static_pointer_cast<ObjectData2D>(obj);

        double x = util::MyMath::Lerp(position_.x, obj_2d->position_.x, fraction);
        double y = util::MyMath::Lerp(position_.y, obj_2d->position_.y, fraction);

        ObjectData2DPtr obj_out(
                new ObjectData2D(obj_in->GetFrameIndex(), cv::Point2d(x, y)));

        return obj_out;
    }

    void ObjectData2D::Print(std::ostream& os) const
    {
        os << "ObjectData2D{"
           << "frame: " << GetFrameIndex() << ","
           << "x: " << position_.x << ","
           << "y: " << position_.y << "}";
    }

    void ObjectData2D::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        double x = position_.x;
        double y = position_.y;
        int r = (int) (0.005 * (image.rows + image.cols) * 0.5);

        cv::circle(image, cv::Point2d(x, y), r, color);
    }

    std::string ObjectData2D::ToString(char delimiter) const
    {
        return ObjectData::ToString(delimiter) + delimiter +
                std::to_string(position_.x) + delimiter + std::to_string(position_.y);
    }
}