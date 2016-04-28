//
// Created by wrede on 19.04.16.
//

#include "ObjectData.h"

namespace core
{
    ObjectData::ObjectData()
    {
        frame_index_ = 0;
        is_virtual_ = true;
    }

    ObjectData::ObjectData(std::size_t frame_index)
    {
        frame_index_ = frame_index;
        is_virtual_ = false;
    }

    std::size_t ObjectData::GetFrameIndex() const
    {
        return frame_index_;
    }

    bool ObjectData::IsVirtual() const
    {
        return is_virtual_;
    }

    void ObjectData::Print(std::ostream &os) const
    {
        os << "Object in frame " << frame_index_;
    }

    double ObjectData::CompareTo(ObjectData *obj)
    {
        return 0.0;
    }

    std::ostream& operator<<(std::ostream &os, const ObjectData &obj)
    {
        obj.Print(os);
        return os;
    }
}







