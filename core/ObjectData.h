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
    //TODO RENAME
    //TODO ObjectDataBase / ObjectBase / DataBase / AObject ...
    /**
     * Base class for all detected objects.
     * Stores the corresponding frame index.
     */
    class ObjectData
    {
    private:
        /**
         * If this node is considered virtual
         */
        bool is_virtual_;
    protected:
        /**
         * The frame the object was detected in
         */
        std::size_t frame_index_;
        /**
         * Used in the << operator
         * @param os The stream to write to
         */
        virtual void Print(std::ostream& os) const;
    public:
        /**
         * Creates a new empty ObjectData (e.g. for virtual objects)
         */
        ObjectData();

        /**
         * Creates a new ObjectData with the given frame index
         * @param frame_index the index in which the object was detected
         */
        ObjectData(std::size_t frame_index);

        /**
         * Getter for the frame index
         * @return The frame index
         */
        std::size_t GetFrameIndex() const;

        /**
         * Is this node considered a virtual node
         * @return Whether this node is virtual
         */
        bool IsVirtual() const;

        //TODO RENAME
        //TODO ObjectDataComparable / IComparable ...
        /**
         * Compares this object with the given object.
         * @param obj A pointer to the object to compare this object to
         * @return A double value indicating the comparison result
         */
        virtual double CompareTo(ObjectData *obj);

        /**
         * Overrides the << operator for easy output.
         * Calls the print method.
         * @param os The stream to write to
         * @param obj The object to write into the stream
         * @return The stream written to
         */
        friend std::ostream& operator<<(std::ostream& os, const ObjectData& obj);
    };
}


#endif //GBMOT_NODEDATA_H
