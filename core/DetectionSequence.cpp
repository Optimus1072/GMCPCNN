//
// Created by wrede on 19.04.16.
//

#include "DetectionSequence.h"

namespace core
{
    DetectionSequence::DetectionSequence(const std::string& name)
    {
        name_ = name;
        objects_ = std::vector<std::vector<ObjectDataPtr>>();
    }

    void DetectionSequence::AddObject(ObjectDataPtr object_data)
    {
        while (object_data->GetFrameIndex() >= objects_.size())
        {
            objects_.push_back(std::vector<ObjectDataPtr>());
        }
        objects_[object_data->GetFrameIndex()].push_back(object_data);
    }

    void DetectionSequence::Clear()
    {
        objects_.clear();
    }

    std::string DetectionSequence::GetName() const
    {
        return name_;
    }

    ObjectDataPtr DetectionSequence::GetObject
            (size_t frame_index, size_t object_index) const
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

    std::ostream& operator<<(std::ostream& os, const DetectionSequence& obj)
    {
        for (size_t frame = 0; frame < obj.objects_.size(); ++frame)
        {
            os << "Frame: " << frame << std::endl;

            for (auto obj_in_frame : obj.objects_[frame])
            {
                os << *obj_in_frame << std::endl;
            }
        }

        return os;
    }
}