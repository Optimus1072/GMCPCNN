//
// Created by wrede on 19.04.16.
//

#include "DetectionSequence.h"

namespace core
{
    DetectionSequence::DetectionSequence(const std::string& name)
    {
        name_ = name;
        //TODO ORIGINAL
//        objects_ = std::vector<std::vector<ObjectDataPtr>>();
        frame_offset_ = std::numeric_limits<size_t>::max();
        frame_count_ = 0;
    }

    void DetectionSequence::AddObject(ObjectDataPtr object_data)
    {
        size_t frame = object_data->GetFrameIndex();

        objects_[frame].push_back(object_data);

        if (!object_data->IsVirtual())
        {
            if (frame < frame_offset_)
                frame_offset_ = frame;

            if (frame > frame_count_)
                frame_count_ = frame;
        }

        //TODO ORIGINAL
//        while (object_data->GetFrameIndex() >= objects_.size())
//        {
//            objects_.push_back(std::vector<ObjectDataPtr>());
//        }
//        objects_[object_data->GetFrameIndex()].push_back(object_data);
    }

    void DetectionSequence::Clear()
    {
        objects_.clear();
    }

    std::string DetectionSequence::GetName() const
    {
        return name_;
    }

    ObjectDataPtr DetectionSequence::GetObject(size_t frame_index, size_t object_index)
    {
        return objects_[frame_index][object_index];
    }

    size_t DetectionSequence::GetFrameCount() const
    {
        return frame_count_;
    }

    size_t DetectionSequence::GetObjectCount(size_t frame_index)
    {
        return objects_[frame_index].size();
    }

    std::ostream & operator<<(std::ostream & os, DetectionSequence const & obj)
    {
        for (auto frame : obj.objects_)
        {
            os << "Frame: " << frame.first << std::endl;

            for (auto obj_in_frame : frame.second)
            {
                os << *obj_in_frame << std::endl;
            }
        }

        return os;
    }

    size_t DetectionSequence::GetFrameOffset() const
    {
        return frame_offset_;
    }
}