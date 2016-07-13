//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_DETECTIONSEQUENCE_H
#define GBMOT_DETECTIONSEQUENCE_H


#include <string>
#include "ObjectData.h"

namespace core
{
    /**
     * Class for storing a full sequence of frame, each with multiple
     * detected objects.
     */
    class DetectionSequence
    {
    private:
        /**
         * Display name
         */
        std::string name_;

        /**
         * Two dimensional vector of pointers to all detected objects.
         * The first dimension is the frame.
         * The second dimension is the object in that frame.
         */
        std::unordered_map<size_t, std::vector<ObjectDataPtr>> objects_;
        //TODO ORIGINAL
//        std::vector<std::vector<ObjectDataPtr>> objects_;

        /**
         * The frame offset for the first detection
         */
        size_t frame_offset_;

        /**
         * The frame count
         */
        size_t frame_count_;
    public:
        /**
         * Creates a detection sequence with the given name.
         *
         * @param name The name of this sequence
         */
        DetectionSequence(std::string const & name = "DetectionSequence");

        /**
         * Adds a new object, creates a new frame vector if the given objects
         * frame index is greater than the current frame vector size.
         *
         * @param object_data The object to add
         */
        void AddObject(ObjectDataPtr object_data);

        /**
         * Removes all objects.
         */
        void Clear();

        /**
         * Gets the name of this sequence.
         *
         * @return The name
         */
        std::string GetName() const;

        /**
         * Gets a pointer to the object in the given frame with the given index.
         *
         * @param frame_index The frame to get the object from
         * @param object_index The objects index in the corresponding frame
         * @return A pointer to the stored object data
         */
        ObjectDataPtr GetObject(size_t frame_index, size_t object_index);

        /**
         * Gets the frame count.
         *
         * @return The frame count
         */
        size_t GetFrameCount() const;

        /**
         * Gets the frame offset.
         *
         * @return The Frame offset
         */
        size_t GetFrameOffset() const;

        /**
         * Gets the object count in the given frame.
         *
         * @param frame_index The frame to get the object count of
         * @return The number of objects in this frame
         */
        size_t GetObjectCount(size_t frame_index);

        /**
         * Overrides the << operator for easy output.
         *
         * @param os The stream to write to
         * @param obj The object to write into the stream
         * @return The stream written to
         */
        friend std::ostream & operator<<(std::ostream & os, DetectionSequence const & obj);
    };
}


#endif //GBMOT_DETECTIONSEQUENCE_H
