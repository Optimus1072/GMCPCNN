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
    class Tracklet;
    typedef std::shared_ptr<Tracklet> TrackletPtr;

    /**
     * A class for storing multiple object data objects.
     * The object data objects are handled as a path.
     * All objects are stored sorted ascending by their frame index.
     */
    class Tracklet : public ObjectData
    {
    private:
        /**
         * The path objects.
         * Sorted ascending by their frame index.
         */
        std::vector<ObjectDataPtr> path_objects_;

        /**
         * The highest frame index of all objects in the path.
         */
        size_t last_frame_index_;

        virtual void Print(std::ostream& os) const;
    public:
        /**
         * Creates a empty tracklet to store path object in.
         * This is NOT a virtual object.
         */
        Tracklet();

        /**
         * Adds the path object sorted into the tracklet.
         * @param obj The path object to add
         * @param overwrite If true and an object in the same frame as the given
         *                  object already exists, the old one will be replaced
         *                  by the new one
         */
        void AddPathObject(ObjectDataPtr obj, bool overwrite = false);

        /**
         * Gets the lowest frame index of all path objects.
         * @return The lowest frame index
         */
        size_t GetFirstFrameIndex() const;

        /**
         * Gets the highest frame index of all path objects.
         * @return The highest frame index
         */
        size_t GetLastFrameIndex() const;

        /**
         * Gets the path object at the given index.
         * The index is NOT the frame index
         * @return A pointer to the path object
         */
        ObjectDataPtr GetPathObject(size_t i);

        /**
         * Gets the count of all path objects.
         * @return The path object count
         */
        size_t GetPathObjectCount() const;

        /**
         * Interpolates between the current path objects until every missing
         * frame has an object. Only frames between the first frame index and
         * the last frame index are interpolated.
         */
        void InterpolateMissingFrames();

        /**
         * Visualizes the tracklet by visualizing the path object in the given
         * frame and the number of path objects in the given range before and
         * after the given frame.
         * @param image The image to write into
         * @param color The color to use
         * @param frame The frame index to visualize the path objects from
         * @param predecessor_count The number of path objects to visualize
         *                          before the given frame
         * @param successor_count The number of path objects to visualize after
         *                        the given frame
         * @param True, if only points should be drawn
         */
        void Visualize(cv::Mat& image, cv::Scalar& color, size_t frame,
                       size_t predecessor_count, size_t successor_count, bool points) const;

        /**
         * Flattens the current tracklet one level.
         * That means, that if this tracklet contains other tracklets as path
         * objects, their path objects are all extracted and used as the new
         * path objects of this tracklet. The old tracklet path objects are
         * removed.
         */
        void Flatten();

        /**
         * Copies all detections from the specified tracklet to this tracklet
         *
         * @param other The tracklet to copy the detections from
         */
        void Combine(TrackletPtr other);

        /**
         * Gets the detected object at the given frame index or a nullptr if there is not detection.
         *
         * @param frame_index The index of the frame to take the detection from
         * @return A pointer to the detection in the given frame
         */
        ObjectDataPtr GetFrameObject(size_t frame_index);

        virtual double CompareTo(ObjectDataPtr obj) const override;
        virtual bool IsWithinConstraints(ObjectDataPtr obj,
                                         std::unordered_map<std::string, double> & constraints) const override;
        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const override;
        virtual void Visualize(cv::Mat& image, cv::Scalar& color, double alpha) const override;
    };
}


#endif //GBMOT_TRACKLET_H
