//
// Created by wrede on 04.05.16.
//

#ifndef GBMOT_OBJECTDATA3D_H
#define GBMOT_OBJECTDATA3D_H

#include "ObjectData.h"
#include <opencv2/opencv.hpp>

namespace core
{
    class ObjectData3D;
    typedef std::shared_ptr<ObjectData3D> ObjectData3DPtr;

    /**
     * Class for storing a detection in three dimensional space.
     */
    class ObjectData3D : public ObjectData
    {
    private:
        /**
         * The position in the three dimensional space
         */
        cv::Point3d position_;

        /**
         * The weight of the temporal distance for the comparison.
         * The temporal distance is the frame difference.
         */
        double temporal_weight_;

        /**
         * The weight of the spatial distance for the comparison.
         * The spatial distance is the euclidean distance of the positions.
         */
        double spatial_weight_;

        virtual void Print(std::ostream& os) const override;
    public:
        /**
         * Creates a new detection with the given index and position.
         * @param frame_index The frame index
         * @param position The position in three dimensional space
         */
        ObjectData3D(size_t frame_index, cv::Point3d position);

        /**
         * Sets the temporal weight.
         * @param weight The temporal weight
         */
        void SetTemporalWeight(double weight);

        /**
         * Sets the spatial weight
         * @param weight The spatial weight
         */
        void SetSpatialWeight(double weight);

        /**
         * Gets the position in three dimensional space.
         * @return The position
         */
        cv::Point3d GetPosition() const;

        /**
         * Gets the temporal weight.
         * @return The temporal weight
         */
        double GetTemporalWeight() const;

        /**
         * Gets the spatial weight
         * @return The spatial weight
         */
        double GetSpatialWeight() const;

        virtual double CompareTo(ObjectDataPtr obj) const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj,
                                          double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
    };
}


#endif //GBMOT_OBJECTDATA3D_H
