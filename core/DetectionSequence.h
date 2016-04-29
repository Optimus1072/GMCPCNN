//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_DETECTIONSEQUENCE_H
#define GBMOT_DETECTIONSEQUENCE_H


#include "Definitions.h"
#include <string>
#include "ObjectData.h"
#include "ObjectDataMap.h"

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
         * Two dimensional vector of all detected objects.
         * The first dimension is the frame.
         * The second dimension is the object in that frame.
         */
        std::vector<std::vector<ObjectData>> objects_;
    protected:
        /**
         * Used in the << operator
         * @param os The stream to write to
         */
        virtual void Print(std::ostream& os) const;
    public:
        /**
         * Creates a detection sequence with the given name.
         * @param name The name of this sequence
         */
        DetectionSequence(std::string name);

        /**
         * Adds a new object, creates a new frame vector if the given objects
         * frame index is greater than the current frame vector size.
         * @param object_data The object to add
         */
        void AddObject(ObjectData object_data);

        /**
         * Removes all objects.
         */
        void Clear();

        /**
         * Gets the name of this sequence.
         * @return The name
         */
        std::string GetName() const;

        /**
         * Gets the object in the given frame with the given index.
         * @param frame_index The frame to get the object from
         * @param object_index The objects index in the corresponding frame
         */
        ObjectData GetObject(size_t frame_index, size_t object_index);

        /**
         * Gets the frame count.
         * @return The frame count
         */
        size_t GetFrameCount() const;

        /**
         * Gets the object count in the given frame.
         * @param frame_index The frame to get the object count of
         * @return The number of object in this frame
         */
        size_t GetObjectCount(size_t frame_index) const;

        /**
         * Overrides the << operator for easy output.
         * Calls the print method.
         * @param os The stream to write to
         * @param obj The object to write into the stream
         * @return The stream written to
         */
        friend std::ostream& operator<<(std::ostream& os, const DetectionSequence& obj);
    };
}


#endif //GBMOT_DETECTIONSEQUENCE_H
