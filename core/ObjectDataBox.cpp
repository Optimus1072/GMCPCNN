//
// Created by wrede on 09.06.16.
//

#include <opencv2/imgproc.hpp>
#include "ObjectDataBox.h"
#include <math.h>
#include "../util/MyMath.h"

namespace core
{
    ObjectDataBox::ObjectDataBox(size_t frame_index, cv::Point2d anchor,
                                 cv::Point2d size)
            : ObjectData2D(frame_index, anchor),
              size_(size)
    {
    }

    void ObjectDataBox::Print(std::ostream& os) const
    {
        os << "ObjectDataBox{"
           << "frame: " << GetFrameIndex() << ","
           << "x: " << GetPosition().x << ","
           << "x: " << GetPosition().y << ","
           << "width: " << size_.x << ","
           << "height: " << size_.y << "}";
    }

    double ObjectDataBox::CompareTo(ObjectDataPtr obj) const
    {
        ObjectDataBoxPtr other = std::static_pointer_cast<ObjectDataBox>(obj);

        cv::Point2d this_center = GetPosition() + size_ * 0.5;
        cv::Point2d other_center = other->GetPosition() + other->size_ * 0.5;

        double d_temp = other->GetFrameIndex() - GetFrameIndex();
        double d_spat = util::MyMath::EuclideanDistance(this_center,
                                                        other_center);

        return d_temp * GetTemporalWeight() + d_spat * GetSpatialWeight();
    }

    ObjectDataPtr ObjectDataBox::Interpolate(ObjectDataPtr obj,
                                             double fraction) const
    {
        ObjectDataBoxPtr other = std::static_pointer_cast<ObjectDataBox>(obj);

        size_t frame = (size_t) fabs(util::MyMath::Lerp(GetFrameIndex(),
                                                        other->GetFrameIndex(),
                                                        fraction));
        double x = util::MyMath::Lerp(GetPosition().x, other->GetPosition().x,
                                      fraction);
        double y = util::MyMath::Lerp(GetPosition().y, other->GetPosition().y,
                                      fraction);
        double w = util::MyMath::Lerp(size_.x, other->size_.x, fraction);
        double h = util::MyMath::Lerp(size_.y, other->size_.y, fraction);

        ObjectDataBoxPtr result(
                new ObjectDataBox(frame, cv::Point2d(x, y), cv::Point2d(w, h)));

        return result;
    }

    void ObjectDataBox::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        cv::Point2d position(GetPosition().x * image.cols,
                             GetPosition().y * image.rows);
        cv::Point2d size(size_.x * image.cols, size_.y * image.rows);

        cv::rectangle(image, position, position + size, color);
    }

    cv::Point2d ObjectDataBox::GetSize() const
    {
        return size_;
    }
}