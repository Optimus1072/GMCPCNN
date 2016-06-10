//
// Created by wrede on 19.05.16.
//

#ifndef GBMOT_OBJECTDATAANGULAR_H
#define GBMOT_OBJECTDATAANGULAR_H

#include "ObjectData2D.h"

namespace core
{
    class ObjectDataAngular;
    typedef std::shared_ptr<ObjectDataAngular> ObjectDataAngularPtr;

    /**
     * Class for storing a detection in two dimensional space with an rotation
     * angle in radians.
     */
    class ObjectDataAngular : public ObjectData2D
    {
    private:
        /**
         * The rotation angle in radians
         */
        double angle_;

        /**
         * The weight of the angular difference for the comparison
         */
        double angular_weight_;

        virtual void Print(std::ostream& os) const override;
    public:
        /**
         * Creates a new object in the given frame, with the given position and
         * the given angle.
         * @param frame_index The index of the frame
         * @param position The position in two dimensional space
         * @param angle The rotation angle in radians
         */
        ObjectDataAngular(size_t frame_index, const cv::Point2d& position, double angle);

        /**
         * Creates a new object in the given frame, with the given position and
         * the given angle. The weights are used in the comparison calculation.
         * @param frame_index The index of the frame
         * @param position The position in two dimensional space
         * @param angle The rotation angle in radians
         * @param temporal_weight The temporal weight
         * @param spatial_weight The spatial weight
         * @param angular_weight The angular weight
         */
        ObjectDataAngular(size_t frame_index, const cv::Point2d& position,
                          double angle, double temporal_weight,
                          double spatial_weight, double angular_weight);

        /**
         * Sets the angular weight.
         * @param weight The angular weight
         */
        void SetAngularWeight(double weight);

        /**
         * Gets the rotation angle in radians.
         * @return The rotation angle in radians
         */
        double GetAngle() const;

        /**
         * Gets the angular weight.
         * @return The angular weight
         */
        double GetAngularWeight() const;

        virtual double CompareTo(ObjectDataPtr obj) const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
    };
}


#endif //GBMOT_OBJECTDATAANGULAR_H
