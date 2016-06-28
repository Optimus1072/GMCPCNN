//
// Created by wrede on 19.05.16.
//

#include <math.h>
#include "ObjectDataAngular.h"
#include "../util/MyMath.h"

namespace core
{
    ObjectDataAngular::ObjectDataAngular(size_t frame_index,
                                         const cv::Point2d& position,
                                         double angle)
            : ObjectData2D(frame_index, position), angle_(angle), angular_weight_(1.0)
    {
    }

    ObjectDataAngular::ObjectDataAngular(size_t frame_index,
                                         const cv::Point2d& position,
                                         double angle,
                                         double temporal_weight,
                                         double spatial_weight,
                                         double angular_weight)
            : ObjectData2D(frame_index, position)
    {
        angle_ = angle;
        angular_weight_ = angular_weight;

        SetTemporalWeight(temporal_weight);
        SetSpatialWeight(spatial_weight);
    }

    void ObjectDataAngular::SetAngularWeight(double weight)
    {
        angular_weight_ = weight;
    }

    double ObjectDataAngular::GetAngle() const
    {
        return angle_;
    }

    double ObjectDataAngular::GetAngularWeight() const
    {
        return angular_weight_;
    }

    double ObjectDataAngular::CompareTo(ObjectDataPtr obj) const
    {
        ObjectDataAngularPtr obj_ang =
                std::static_pointer_cast<ObjectDataAngular>(obj);

        double d_ang = std::abs(obj_ang->angle_ - angle_);

        return ObjectData2D::CompareTo(obj) + d_ang * angular_weight_;
    }

    ObjectDataPtr ObjectDataAngular::Interpolate(ObjectDataPtr obj,
                                                 double fraction) const
    {
        ObjectData2DPtr obj_in =
                std::static_pointer_cast<ObjectData2D>(
                        ObjectData2D::Interpolate(obj, fraction));

        ObjectDataAngularPtr obj_ang =
                std::static_pointer_cast<ObjectDataAngular>(obj);

        double angle = util::MyMath::Lerp(angle_, obj_ang->angle_, fraction);

        ObjectDataAngularPtr obj_out(
                new ObjectDataAngular(
                        obj_in->GetFrameIndex(), obj_in->GetPosition(), angle));

        return obj_out;
    }

    void ObjectDataAngular::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        double x = GetPosition().x * image.cols;
        double y = GetPosition().y * image.rows;
        int r = (int) (0.005 * (image.rows + image.cols) * 0.5);
        double a_x = x + cos(angle_) * r * 6.0;
        double a_y = y + sin(angle_) * r * 6.0;

        cv::circle(image, cv::Point2d(x, y), r, color);
        cv::line(image, cv::Point2d(x, y), cv::Point2d(a_x, a_y), color);
    }

    void ObjectDataAngular::Print(std::ostream& os) const
    {
        os << "ObjectDataAngular{"
        << "f:" << GetFrameIndex() << ", "
        << "x:" << GetPosition().x << ", "
        << "y:" << GetPosition().y << ", "
        << "a:" << GetAngle() << "}";
    }
}
