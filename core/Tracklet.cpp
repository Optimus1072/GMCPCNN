//
// Created by wrede on 25.04.16.
//

#include "Tracklet.h"
#include "../util/Logger.h"

namespace core
{
    void Tracklet::Print(std::ostream& os) const
    {
        os << "Tracklet{\n";
        for (auto obj : path_objects_)
        {
            os << *obj << std::endl;
        }
        os << "}";
    }

    Tracklet::Tracklet()
            : ObjectData(0)
    {
        path_objects_ = std::vector<ObjectDataPtr>();
        last_frame_index_ = 0;
    }

    size_t Tracklet::GetFirstFrameIndex() const
    {
        return GetFrameIndex();
    }

    size_t Tracklet::GetLastFrameIndex() const
    {
        return last_frame_index_;
    }

    void Tracklet::AddPathObject(ObjectDataPtr obj, bool overwrite)
    {
        // Prevent virtual objects to be added, they should be interpolated later from
        // the real detections
        if (!obj->IsVirtual())
        {
            bool inserted = false;

            if (!path_objects_.empty())
            {
                for (auto iter = path_objects_.begin();
                     iter != path_objects_.end() && !inserted;
                     ++iter)
                {
                    // If the frame index is the same, either overwrite the value or do nothing
                    // If the frame index is smaller than the stored, the new object should be
                    // inserted here to accomplish a frame index order
                    if ((*iter)->GetFrameIndex() == obj->GetFrameIndex())
                    {
                        if (overwrite)
                        {
                            iter = path_objects_.erase(iter);
                            iter = path_objects_.insert(iter, obj);
                        }
                        inserted = true;
                    }
                    else if ((*iter)->GetFrameIndex() > obj->GetFrameIndex())
                    {
                        iter = path_objects_.insert(iter, obj);
                        inserted = true;
                    }
                }
            }

            // If the frame index is higher than all already stored object, the new object is
            // simple added at the end to preserve the ascending frame index order
            if (!inserted)
            {
                path_objects_.push_back(obj);
            }

            // Update the lowest and highest frame index fields
            SetFrameIndex(path_objects_.front()->GetFrameIndex());
            last_frame_index_ = path_objects_.back()->GetFrameIndex();
        }
    }

    ObjectDataPtr Tracklet::GetPathObject(size_t i)
    {
        return path_objects_[i];
    }

    double Tracklet::CompareTo(ObjectDataPtr obj) const
    {
        TrackletPtr tlt = std::static_pointer_cast<Tracklet>(obj);
        ObjectDataPtr this_obj = path_objects_[path_objects_.size() - 1];
        ObjectDataPtr that_obj = tlt->path_objects_[0];

        return path_objects_[path_objects_.size() - 1]->CompareTo(tlt->path_objects_[0]);
    }

    bool Tracklet::IsWithinConstraints(ObjectDataPtr obj,
                                       std::unordered_map<std::string, double> & constraints) const
    {
        TrackletPtr tlt = std::static_pointer_cast<Tracklet>(obj);
        return path_objects_[path_objects_.size() - 1]->
                IsWithinConstraints(tlt->path_objects_[0], constraints);
    }

    ObjectDataPtr Tracklet::Interpolate(ObjectDataPtr obj, double fraction) const
    {
        TrackletPtr tlt = std::static_pointer_cast<Tracklet>(obj);

        return path_objects_[path_objects_.size() - 1]->Interpolate(tlt->path_objects_[0], fraction);
    }

    void Tracklet::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        for (auto obj : path_objects_)
        {
            obj->Visualize(image, color);
        }
    }

    void Tracklet::Visualize(cv::Mat& image, cv::Scalar& color, size_t frame,
                             size_t predecessor_count, size_t successor_count) const
    {
        // Prevent negative values because frame is unsigned
        predecessor_count = std::min(predecessor_count, frame);

        size_t start = (frame - predecessor_count > GetFirstFrameIndex()) ?
                       frame - predecessor_count : GetFirstFrameIndex();
        size_t end = (frame + successor_count < GetLastFrameIndex()) ?
                     frame + successor_count : GetLastFrameIndex();

        for (auto obj : path_objects_)
        {
            if (obj->GetFrameIndex() >= start && obj->GetFrameIndex() <= end)
            {
                obj->Visualize(image, color);
            }
        }
    }

    //TODO find a better method than linear interpolation
    void Tracklet::InterpolateMissingFrames()
    {
        for (size_t i = 1; i < path_objects_.size(); ++i)
        {
            size_t gap = path_objects_[i]->GetFrameIndex() - path_objects_[i - 1]->GetFrameIndex();
            if (gap > 1)
            {
                path_objects_.insert(path_objects_.begin() + i,
                                     path_objects_[i - 1]->Interpolate(path_objects_[i], 0.5));
                --i;
            }
        }
    }

    size_t Tracklet::GetPathObjectCount() const
    {
        return path_objects_.size();
    }

    void Tracklet::Flatten()
    {
        std::vector<ObjectDataPtr> new_path_objects;

        for (auto obj : path_objects_)
        {
            core::TrackletPtr tlt = std::static_pointer_cast<core::Tracklet>(obj);

            for (auto intern_obj : tlt->path_objects_)
            {
                new_path_objects.push_back(intern_obj);
            }
        }

        path_objects_ = new_path_objects;

        SetFrameIndex(path_objects_.front()->GetFrameIndex());
        last_frame_index_ = path_objects_.back()->GetFrameIndex();
    }

    void Tracklet::Combine(TrackletPtr other)
    {
        for (auto obj : other->path_objects_)
        {
            AddPathObject(obj);
        }
    }

    ObjectDataPtr Tracklet::GetFrameObject(size_t frame_index)
    {
        for (auto obj : path_objects_)
        {
            if (obj->GetFrameIndex() == frame_index)
            {
                return obj;
            }
        }

        return nullptr;
    }
}



