//
// Created by wrede on 09.06.16.
//

#include "ObjectDataBox.h"
#include <math.h>
#include "../util/MyMath.h"

namespace core
{
    const std::string ObjectDataBox::CONSTRAINT_WIDTH_DIFFERENCE = "width_difference";
    const std::string ObjectDataBox::CONSTRAINT_HEIGHT_DIFFERENCE = "height_difference";

    ObjectDataBox::ObjectDataBox(size_t frame_index, cv::Point2d center,
                                 cv::Point2d size)
            : ObjectData2D(frame_index, center),
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

//        cv::Point2d this_center = GetPosition() + size_ * 0.5;
//        cv::Point2d other_center = other->GetPosition() + other->size_ * 0.5;

        double d_temp = other->GetFrameIndex() - GetFrameIndex() - 1;
        double d_spat = util::MyMath::EuclideanDistance(GetPosition(), other->GetPosition());

        double weight = d_temp * GetTemporalWeight() + d_spat * GetSpatialWeight();

//        std::cout << "box weight " << weight << ", t:" << d_temp << ", s:" << d_spat << std::endl;

        return weight;
    }

    bool ObjectDataBox::IsWithinConstraints(ObjectDataPtr obj,
                                            std::unordered_map<std::string, double> & constraints)
    const
    {
        if (!ObjectData2D::IsWithinConstraints(obj, constraints))
            return false;

        ObjectDataBoxPtr obj_box = std::static_pointer_cast<ObjectDataBox>(obj);

        if (constraints.count(CONSTRAINT_WIDTH_DIFFERENCE) > 0) {
            double width_difference = fabs(size_.x - obj_box->size_.x);

            if (width_difference > constraints[CONSTRAINT_WIDTH_DIFFERENCE])
                return false;
        }

        if (constraints.count(CONSTRAINT_HEIGHT_DIFFERENCE) > 0) {
            double height_difference = fabs(size_.y - obj_box->size_.y);

            if (height_difference > constraints[CONSTRAINT_HEIGHT_DIFFERENCE])
                return false;
        }

        return true;
    }

    ObjectDataPtr ObjectDataBox::Interpolate(ObjectDataPtr obj,
                                             double fraction) const
    {
        ObjectDataBoxPtr other = std::static_pointer_cast<ObjectDataBox>(obj);

        size_t frame = (size_t) fabs(util::MyMath::Lerp(GetFrameIndex(),
                                                        other->GetFrameIndex(),
                                                        fraction));

        double x = util::MyMath::Lerp(GetPosition().x, other->GetPosition().x, fraction);
        double y = util::MyMath::Lerp(GetPosition().y, other->GetPosition().y, fraction);
        double w = util::MyMath::Lerp(size_.x, other->size_.x, fraction);
        double h = util::MyMath::Lerp(size_.y, other->size_.y, fraction);

        ObjectDataBoxPtr result(
                new ObjectDataBox(frame, cv::Point2d(x, y), cv::Point2d(w, h)));

        return result;
    }

    void ObjectDataBox::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        cv::Point2d center(GetPosition().x * image.cols, GetPosition().y * image.rows);
        cv::Point2d size(size_.x * image.cols, size_.y * image.rows);
        cv::Point2d top_left = center - size * 0.5;

        cv::rectangle(image, top_left, top_left + size, color);
    }

    cv::Point2d ObjectDataBox::GetSize() const
    {
        return size_;
    }

    std::string ObjectDataBox::ToString(char delimiter) const
    {
        return ObjectData2D::ToString(delimiter) + delimiter +
                std::to_string(size_.x) + delimiter + std::to_string(size_.y);
    }
}