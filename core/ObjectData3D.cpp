//
// Created by wrede on 04.05.16.
//

#include "ObjectData3D.h"
#include "../util/MyMath.h"

namespace core
{
    ObjectData3D::ObjectData3D(size_t frame_index, cv::Point3d position)
            : ObjectData(frame_index),
              position_(position),
              temporal_weight_(0.5),
              spatial_weight_(0.5)
    {
    }

    void ObjectData3D::SetTemporalWeight(double weight)
    {
        temporal_weight_ = weight;
    }

    void ObjectData3D::SetSpatialWeight(double weight)
    {
        spatial_weight_ = weight;
    }

    cv::Point3d ObjectData3D::GetPosition() const
    {
        return position_;
    }

    double ObjectData3D::GetTemporalWeight() const
    {
        return temporal_weight_;
    }

    double ObjectData3D::GetSpatialWeight() const
    {
        return spatial_weight_;
    }

    double ObjectData3D::CompareTo(ObjectDataPtr obj) const
    {
        ObjectData3DPtr obj_3d = std::static_pointer_cast<ObjectData3D>(obj);

        double d_temp = obj_3d->GetFrameIndex() - GetFrameIndex();
        double d_spat = util::MyMath::EuclideanDistance(position_, obj_3d->position_);

        return d_temp * temporal_weight_ + d_spat * spatial_weight_;
    }

    ObjectDataPtr ObjectData3D::Interpolate(ObjectDataPtr obj,
                                            double fraction) const
    {
        ObjectDataPtr obj_in = ObjectData::Interpolate(obj, fraction);

        ObjectData3DPtr obj_3d = std::static_pointer_cast<ObjectData3D>(obj);

        double x = util::MyMath::Lerp(position_.x, obj_3d->position_.x, fraction);
        double y = util::MyMath::Lerp(position_.y, obj_3d->position_.y, fraction);
        double z = util::MyMath::Lerp(position_.z, obj_3d->position_.z, fraction);

        ObjectData3DPtr obj_out(
                new ObjectData3D(obj_in->GetFrameIndex(), cv::Point3d(x, y, z)));

        return obj_out;
    }

    void ObjectData3D::Print(std::ostream& os) const
    {
        os << "ObjectData3D{"
           << "frame: " << GetFrameIndex() << ","
           << "x: " << position_.x << ","
           << "y: " << position_.y << ","
           << "z: " << position_.z << "}";
    }

    void ObjectData3D::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        double x = position_.x * image.cols;
        double y = position_.y * image.rows;
        int r = (int) (0.005 * (image.rows + image.cols) * 0.5);

        cv::circle(image, cv::Point2d(x, y), r, color);
    }
}


