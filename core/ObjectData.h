//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_OBJECTDATA_H
#define GBMOT_OBJECTDATA_H


#include <string>
#include <unordered_map>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <memory>

namespace core
{
    class ObjectData;
    typedef std::shared_ptr<ObjectData> ObjectDataPtr;

    /**
     * Base class for all detected objects.
     * Stores the corresponding frame index.
     */
    class ObjectData
    {
    private:
        static const std::string CONSTRAINT_FRAME_DIFFERENCE;
        static const std::string CONSTRAINT_SCORE_DIFFERENCE;

        /**
         * If this node is considered virtual
         */
        const bool is_virtual_;

        /**
         * The frame the object was detected in
         */
        std::size_t frame_index_;

        /**
         * The detection score.
         * Defines the quality of this detection.
         */
        double detection_score_;

        /**
         * Used in the << operator
         * @param os The stream to write to
         */
        virtual void Print(std::ostream& os) const;
    public:
        /**
         * Creates a new empty virtual ObjectData
         */
        ObjectData();

        /**
         * Creates a new ObjectData with the given frame index
         * @param frame_index the index in which the object was detected
         */
        ObjectData(std::size_t frame_index);

        /**
         * Gets the frame index
         * @return The frame index
         */
        std::size_t GetFrameIndex() const;

        /**
         * Sets the frame index
         * @param index The new frame index
         */
        void SetFrameIndex(size_t index);

        /**
         * Sets the detection score
         * @param score The detection score
         */
        void SetDetectionScore(double score);

        /**
         * Gets the detection score
         * @return The detection score
         */
        double GetDetectionScore() const;

        /**
         * Is this node considered a virtual node
         * @return True, if this node is virtual
         */
        bool IsVirtual() const;

        /**
         * Compares this object with the given object.
         * @param obj A pointer to the object to compare this object to
         * @return A double value indicating the comparison result
         */
        virtual double CompareTo(ObjectDataPtr obj) const;

        /**
         * Checks if the difference between this object and the specified object is within the
         * constraints specified. The difference is calculated for each constraint separately.
         *
         * @param obj The object to get the difference to
         * @param constraints The constraints to assure
         */
        virtual bool IsWithinConstraints(ObjectDataPtr obj,
                                         std::unordered_map<std::string, double> & constraints)
                const;

        /**
         * Linearly interpolates between this and the given object.
         * Creates a new object to fit between the two objects.
         * @param obj A pointer to the target object
         * @param fraction Describes where the interpolation should
         *                 be done. A fraction of zero is a clone of this object,
         *                 a fraction of one is a clone of the target object.
         * @return The interpolated object
         */
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const;

        /**
         * Visualizes the object in the given image with the given color.
         * This method does nothing, it needs to be overwritten to visualize
         * something.
         * @param image The image to write into
         * @param color The color to use
         * @param alpha The alpha channel value in the range [0.0, 1.0]
         */
        virtual void Visualize(cv::Mat& image, cv::Scalar& color, double alpha) const;

        /**
         * Overrides the << operator for custom output.
         * Calls the print method.
         * @param os The stream to write to
         * @param obj The object to write into the stream
         * @return The stream written to
         */
        friend std::ostream& operator<<(std::ostream& os, const ObjectData& obj);

        /**
         * Returns a string representing the values of this object data.
         *
         * @param delimiter The delimiter used to separate values
         * @return The string containing the values
         */
        virtual std::string ToString(char delimiter) const;
    };
}


#endif //GBMOT_OBJECTDATA_H
