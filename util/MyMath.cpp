//
// Created by wrede on 02.05.16.
//

#include "MyMath.h"

namespace util
{
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
        return (b - a) * value + a;
    }

    double MyMath::InverseLerp(double a, double b, double value)
    {
        return (value - a) / (b - a);
    }

    double MyMath::EuclideanDistance(cv::Point3d a, cv::Point3d b)
    {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double dz = b.z - a.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
}


