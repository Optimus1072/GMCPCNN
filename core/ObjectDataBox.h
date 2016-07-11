//
// Created by wrede on 09.06.16.
//

#ifndef GBMOT_OBJECTDATABOX_H
#define GBMOT_OBJECTDATABOX_H

#include "ObjectData2D.h"

namespace core
{
    class ObjectDataBox;
    typedef std::shared_ptr<ObjectDataBox> ObjectDataBoxPtr;

    /**
     * Class for storing an object detection with an bounding box.
     */
    class ObjectDataBox : public ObjectData2D
    {
    private:
        /**
         * The size of the bounding box
         */
        const cv::Point2d size_;

        virtual void Print(std::ostream& os) const override;
    public:
        /**
         * Creates a new detection with an bounding box of the given size.
         *
         * @param frame_index The frame index of the detection
         * @param center The center of the bounding box
         * @param size The size of the bounding box
         */
        ObjectDataBox(size_t frame_index, cv::Point2d center, cv::Point2d size);

        /**
         * Gets the size of the bounding box.
         *
         * @return The size of the bounding box
         */
        cv::Point2d GetSize() const;

        virtual double CompareTo(ObjectDataPtr obj) const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
    };
}


#endif //GBMOT_OBJECTDATABOX_H
