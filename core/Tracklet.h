//
// Created by wrede on 25.04.16.
//

#ifndef GBMOT_TRACKLET_H
#define GBMOT_TRACKLET_H

#include <cstdlib>
#include <vector>
#include "ObjectData.h"

namespace core
{
    class Tracklet : public ObjectData
    {
    private:
        std::vector<ObjectData> path_objects_;
        size_t last_frame_index_;
    public:
        Tracklet();

        Tracklet(ObjectData firstObject);

        Tracklet(std::vector<ObjectData> path_objects,
                 size_t first_frame_index, size_t last_frame_index);

        void AddPathObjectFirst(ObjectData obj);
        void AddPathObjectLast(ObjectData obj);
        size_t GetFirstFrameIndex();
        size_t GetLastFrameIndex();
        ObjectData GetPathObject(size_t i);

        //TODO point interpolation -> derived class
        virtual double CompareTo(ObjectData *obj);
        //TODO implement CompareTo -> derived class
    };
}


#endif //GBMOT_TRACKLET_H
