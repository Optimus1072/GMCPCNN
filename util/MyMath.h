//
// Created by wrede on 02.05.16.
//

#ifndef GBMOT_UTILITY_H
#define GBMOT_UTILITY_H

#include <cstdlib>
#include <cmath>
#include <opencv2/core/core.hpp>

namespace util
{
    /**
     * Utility class for mathematical operations.
     */
    class MyMath
    {
    public:
        /**
         * Clamps the value between min and max, both inclusive.
         * @param min The minimum value
         * @param max The maximum value
         * @param value The value to clamp
         * @return The clamped value
         */
        static double Clamp(double min, double max, double value);

        /**
         * Linearly interpolates between a and b at value.
         * @param a The first value
         * @param b The second value
         * @param value The interpolation value
         * @return The interpolated value
         */
        static double Lerp(double a, double b, double value);

        /**
         * Inverse linearly interpolates between a and b at value.
         * @param a The first value
         * @param b The second value
         * @param value The value to get the interpolation of
         * @return The interpolation value
         */
        static double InverseLerp(double a, double b, double value);

        /**
         * Calculates the euclidean distance of the given points.
         * @param a The first point in 3D space
         * @param b The second point in 3D space
         * @return The euclidean distance
         */
        static double EuclideanDistance(cv::Point3d a, cv::Point3d b);
    };
}


#endif //GBMOT_UTILITY_H
