//
// Created by wrede on 22.04.16.
//

#ifndef GBMOT_OBJECTDATAMAP_H
#define GBMOT_OBJECTDATAMAP_H

#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "ObjectData.h"

namespace core
{
    // Stores an map of key-value pairs for fast access and unified use.
    class ObjectDataMap : public ObjectData
    {
    private:
        // The stored value-weight pairs
        std::unordered_map<std::string, std::pair<double, double>> value_weight_map_;
    protected:
        // Print function, used in << operator
        virtual void Print(std::ostream& os) const;
    public:
        // Creates a new empty ObjectDataMap
        ObjectDataMap(size_t frame_index);

        // Creates a new ObjectDataMap with the given keys and values
        // stored as triple with an equal weight for every value
        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<double> value_list);

        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<double> value_list,
                std::vector<double> weight_list);

        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<std::pair<double, double>> value_weight_list);

        // Gets the value
        double GetValue(std::string key);

        // Gets the weight
        double GetWeight(std::string key);

        //TODO find a better name
        void PutValueWeight(std::string key, double value, double weight);
        void PutValueWeight(std::string key, std::pair<double, double> value_weight);

        virtual double CompareTo(ObjectData *obj);
        virtual double CompareTo(ObjectDataMap *obj);
    };
}


#endif //GBMOT_OBJECTDATAMAP_H
