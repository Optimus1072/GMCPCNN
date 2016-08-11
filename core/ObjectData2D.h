//
// Created by wrede on 02.06.16.
//

#ifndef GBMOT_OBJECTDATA2D_H
#define GBMOT_OBJECTDATA2D_H

#include "ObjectData.h"

namespace core
{
    class ObjectData2D;
    typedef std::shared_ptr<ObjectData2D> ObjectData2DPtr;

    /**
     * Class for storing a detection in two dimensional space.
     */
    class ObjectData2D : public ObjectData
    {
    private:
        static const std::string CONSTRAINT_DISTANCE_EUCLID;

        /**
         * The position in the two dimensional space
         */
        const cv::Point2d position_;

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
        ObjectData2D(size_t frame_index, cv::Point2d position);

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
         * Gets the position in two dimensional space.
         * @return The position
         */
        cv::Point2d GetPosition() const;

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
        virtual bool IsWithinConstraints(ObjectDataPtr obj,
                                         std::unordered_map<std::string, double> & constraints)
                const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
        virtual std::string ToString(char delimiter) const override;
    };
}


#endif //GBMOT_OBJECTDATA2D_H
