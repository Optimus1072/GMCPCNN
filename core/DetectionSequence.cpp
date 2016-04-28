//
// Created by wrede on 19.04.16.
//

#include "DetectionSequence.h"

namespace core
{
    DetectionSequence::DetectionSequence(std::string name)
    {
        name_ = name;
        objects_ = std::vector<std::vector<ObjectData>>();
    }

    void DetectionSequence::AddObject(ObjectData object_data)
    {
        if (object_data.GetFrameIndex() >= objects_.size())
        {
            objects_.push_back(std::vector<ObjectData>());
        }
        objects_[object_data.GetFrameIndex()].push_back(object_data);
    }

    void DetectionSequence::Clear()
    {
        objects_.clear();
    }

    std::string DetectionSequence::GetName() const
    {
        return name_;
    }

    ObjectData DetectionSequence::GetObject(
            size_t frame_index, size_t object_index)
    {
        return objects_[frame_index][object_index];
    }

    size_t DetectionSequence::GetFrameCount() const
    {
        return objects_.size();
    }

    size_t DetectionSequence::GetObjectCount(size_t frame_index) const
    {
        return objects_[frame_index].size();
    }

    void DetectionSequence::Print(std::ostream &os) const
    {
        for (size_t frame_i = 0; frame_i < objects_.size(); ++frame_i)
        {
            os << "Frame: " << frame_i << std::endl;

            for (size_t object_i = 0;
                 object_i < objects_[frame_i].size();
                 ++object_i)
            {
                os << objects_[frame_i][object_i] << std::endl;
            }
        }

        os << std::endl;

    }

    std::ostream& operator<<(std::ostream &os, const DetectionSequence &obj)
    {
        obj.Print(os);
        return os;
    }
}











