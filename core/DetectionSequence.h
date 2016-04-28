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
    // Class for storing a full sequence of frames,
    // each with multiple detected objects.
    class DetectionSequence
    {
    private:
        // Name for display or identification purposes
        std::string name_;
        // Two-dimensional vector of smart pointers to all stored objects
        std::vector<std::vector<ObjectData>> objects_;
    protected:
        // Print function, used in << operator
        virtual void Print(std::ostream& os) const;
    public:
        // Creates a new DetectionSequence with the given name
        // Initializes the storage for detected objects
        DetectionSequence(std::string name);

        // Adds a new object, creates a new frame vector if the given objects
        // frame index is greater than the current frame vector size.
        void AddObject(ObjectData object_data);

        // Removes all objects.
        void Clear();

        // Gets the name
        std::string GetName() const;

        // Gets the object in the given frame with the index in that frame.
        ObjectData GetObject(size_t frame_index, size_t object_index);

        // Gets the frame count
        size_t GetFrameCount() const;

        // Gets the object count in the given frame
        size_t GetObjectCount(size_t frame_index) const;

        friend std::ostream& operator<<(std::ostream& os, const DetectionSequence& obj);
    };
}


#endif //GBMOT_DETECTIONSEQUENCE_H
