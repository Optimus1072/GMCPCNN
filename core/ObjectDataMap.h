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
    //TODO RENAME
    /**
     * Stores a map of key-value-weight pairs.
     * The weight is used to compare this object with other objects.
     */
    class ObjectDataMap : public ObjectData
    {
    private:
        /**
         * The stored value-weight-pairs.
         */
        std::unordered_map<std::string, std::pair<double, double>> value_weight_map_;
    protected:
        /**
         * Used in the << operator
         * @param os The stream to write to
         */
        virtual void Print(std::ostream& os) const;
    public:
        /**
         * Creates a new empty object data map.
         * @param frame_index The index of the frame this object was detected in
         */
        ObjectDataMap(size_t frame_index);

        /**
         * Creates a object data map with the given keys and values and an
         * equal weight for every value.
         * @param frame_index The index of the frame this object was detected in
         * @param keys The keys for the values to store
         * @param value_list The values to store with the given keys
         */
        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<double> value_list);
        /**
         * Creates a object data map with the given keys and values and an
         * given weight for the corresponding key-value pair.
         * @param frame_index The index of the frame this object was detected in
         * @param keys The keys for the values to store
         * @param value_list The values to store with the given keys
         * @param weight_list The weights to store with the given key-value pairs
         */
        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<double> value_list,
                std::vector<double> weight_list);

        /**
         * Creates a object data map with the given keys and value-weight pairs.
         * @param frame_index The index of the frame this object was detected in
         * @param keys The keys for the values to store
         * @param value_weight_list The value-weight-pairs to store with the keys
         */
        ObjectDataMap(
                size_t frame_index,
                std::vector<std::string> keys,
                std::vector<std::pair<double, double>> value_weight_list);

        /**
         * Gets the value of the given key.
         * @param key The key for the value
         * @return The value
         */
        double GetValue(std::string key);

        /**
         * Gets the weight of the given key.
         * @param key The key for the value
         * @return The weight
         */
        double GetWeight(std::string key);

        //TODO find a better name
        /**
         * Stores the given value-weight pair with the given key.
         * If the key is already stored it will be overridden with the new pair.
         * @param key The key to store the value-weight pair at
         * @param value The value of the value-weight pair
         * @param weight The weight of the value-weight pair
         */
        void PutValueWeight(std::string key, double value, double weight);

        /**
         * Stores the given value-weight pair with the given key.
         * If the key is already stored it will be overridden with the new pair.
         * @param key The key to store the value-weight pair at
         * @param value_weight The value-weight pair
         */
        void PutValueWeight(std::string key, std::pair<double, double> value_weight);

        /**
         * Compares this object with the given object by calculating the difference
         * in every value and applies the corresponding weight to that difference.
         * Than all weighted differences are summed up.
         * @param obj A pointer to the object to compare this object to
         * @return The summed up weighted differences
         */
        virtual double CompareTo(ObjectData *obj);

        /**
         * Compares this object with the given object by calculating the difference
         * in every value and applies the corresponding weight to that difference.
         * Than all weighted differences are summed up.
         * @param obj A pointer to the object to compare this object to
         * @return The summed up weighted differences
         */
        virtual double CompareTo(ObjectDataMap *obj);
    };
}


#endif //GBMOT_OBJECTDATAMAP_H
