//
// Created by wrede on 09.06.16.
//

#ifndef GBMOT_OBJECTDATABOX_H
#define GBMOT_OBJECTDATABOX_H

#include "ObjectData2D.h"

namespace core
{
    //TODO comment
    class ObjectDataBox;
    typedef std::shared_ptr<ObjectDataBox> ObjectDataBoxPtr;

    class ObjectDataBox : public ObjectData2D
    {
    private:
        const cv::Point2d size_;

        virtual void Print(std::ostream& os) const override;
    public:
        ObjectDataBox(size_t frame_index, cv::Point2d anchor, cv::Point2d size);

        cv::Point2d GetSize() const;

        virtual double CompareTo(ObjectDataPtr obj) const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj,
                                          double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
    };
}


#endif //GBMOT_OBJECTDATABOX_H
