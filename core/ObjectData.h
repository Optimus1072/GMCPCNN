//
// Created by wrede on 19.04.16.
//

#ifndef GBMOT_NODEDATA_H
#define GBMOT_NODEDATA_H


#include <string>
#include <unordered_map>
#include <iostream>

namespace core
{
    //TODO ObjectDataBase / ObjectBase / DataBase / AObject ...
    // Base class for all detected objects.
    // Stores the corresponding frame index.
    class ObjectData
    {
    private:
        bool is_virtual_;
    protected:
        // The frame the object was detected in
        std::size_t frame_index_;
        // Print function, used in << operator
        virtual void Print(std::ostream& os) const;
    public:
        // Creates a new empty ObjectData (e.g. for virtual objects)
        ObjectData();

        // Creates a new ObjectData with the given frame index
        ObjectData(std::size_t frame_index);

        // Gets the corresponding frame index
        std::size_t GetFrameIndex() const;

        bool IsVirtual() const;

        //TODO ObjectDataComparable / IComparable ...
        //TODO find a better name
        virtual double CompareTo(ObjectData *obj);

        friend std::ostream& operator<<(std::ostream& os, const ObjectData& obj);
    };
}


#endif //GBMOT_NODEDATA_H
