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
    //TODO sorted insert
    /**
     * A class for storing multiple object data objects.
     * The object data objects are handled as a path.
     */
    class Tracklet : public ObjectData
    {
    private:
        /**
         * The path objects.
         */
        std::vector<ObjectData> path_objects_;
        /**
         * The highest frame index of all objects in the path.
         */
        size_t last_frame_index_;
    public:
        /**
         * Creates a empty tracklet to store path object in.
         * This is NOT a virtual object.
         */
        Tracklet();

        /**
         * Creates a tracklet with the given initial object.
         * @see ObjectData
         * @param first_object The first object to store in the path
         */
        Tracklet(ObjectData first_object);

        /**
         * Adds the object in the first place of the path.
         * @param obj The object to add
         */
        void AddPathObjectFirst(ObjectData obj);

        /**
         * Adds the object in the last place of the path.
         * @param obj The object to add
         */
        void AddPathObjectLast(ObjectData obj);

        /**
         * Gets the lowest frame index of all path objects.
         * @return The lowest frame index
         */
        size_t GetFirstFrameIndex();

        /**
         * Gets the highest frame index of all path objects.
         * @return The highest frame index
         */
        size_t GetLastFrameIndex();

        /**
         * Gets the path object at the given index.
         */
        ObjectData GetPathObject(size_t i);

        /**
         * Compares this object with the given object.
         * @param obj A pointer to the object to compare this object to
         * @return A double value indicating the comparison result
         */
        virtual double CompareTo(ObjectData *obj);

        //TODO point interpolation -> object data (last object of this with first of other)
        //TODO implement CompareTo (last object of this with first of other)
    };
}


#endif //GBMOT_TRACKLET_H
