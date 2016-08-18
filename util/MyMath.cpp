//
// Created by wrede on 02.05.16.
//

#include "MyMath.h"

namespace util
{
    const double MyMath::PI = 3.14159;

    double MyMath::Clamp(double min, double max, double value)
    {
        if (value < min)
        {
            return min;
        }
        else if (value > max)
        {
            return max;
        }
        else
        {
            return value;
        }
    }

    double MyMath::Lerp(double a, double b, double value)
    {
        if (a == b) return a;
        return (b - a) * value + a;
    }

    double MyMath::InverseLerp(double a, double b, double value)
    {
        if (a == b) return a;
        return (value - a) / (b - a);
    }

    double MyMath::EuclideanDistance(cv::Point3d a, cv::Point3d b)
    {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double dz = b.z - a.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    double MyMath::EuclideanDistance(cv::Point2d a, cv::Point2d b)
    {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    double MyMath::Radian(double degree)
    {
        return (degree * PI) / 180.0;
    }
}


