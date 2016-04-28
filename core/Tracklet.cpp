//
// Created by wrede on 25.04.16.
//

#include "Tracklet.h"

namespace core
{
    Tracklet::Tracklet() : ObjectData(0)
    {
        path_objects_ = std::vector<ObjectData>();
        last_frame_index_ = 0;
    }

    Tracklet::Tracklet(ObjectData firstObject)
            : ObjectData(firstObject.GetFrameIndex())
    {
        path_objects_ = std::vector<ObjectData>();
        path_objects_.push_back(firstObject);

        last_frame_index_ = firstObject.GetFrameIndex();
    }

    Tracklet::Tracklet(std::vector<ObjectData> path_objects,
                       size_t first_frame_index, size_t last_frame_index)
            : ObjectData(first_frame_index)
    {
        path_objects_ = path_objects;
        last_frame_index_ = last_frame_index;
    }

    size_t Tracklet::GetFirstFrameIndex()
    {
        return frame_index_;
    }

    size_t Tracklet::GetLastFrameIndex()
    {
        return last_frame_index_;
    }

    ObjectData Tracklet::GetPathObject(size_t i)
    {
        return path_objects_[i];
    }

    void Tracklet::AddPathObjectFirst(ObjectData obj)
    {
        if (!obj.IsVirtual())
        {
            path_objects_.insert(path_objects_.begin(), obj);

            if (obj.GetFrameIndex() < frame_index_)
            {
                frame_index_ = obj.GetFrameIndex();
            }
            else if (obj.GetFrameIndex() > last_frame_index_)
            {
                last_frame_index_ = obj.GetFrameIndex();
            }
        }
    }

    void Tracklet::AddPathObjectLast(ObjectData obj)
    {
        if (!obj.IsVirtual())
        {
            path_objects_.push_back(obj);

            if (obj.GetFrameIndex() > last_frame_index_)
            {
                last_frame_index_ = obj.GetFrameIndex();
            }
            else if (obj.GetFrameIndex() < frame_index_)
            {
                frame_index_ = obj.GetFrameIndex();
            }
        }
    }

    double Tracklet::CompareTo(ObjectData *obj)
    {
        return 0.0;
    }
}



