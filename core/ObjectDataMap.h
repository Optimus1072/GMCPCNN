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
    class ObjectDataMap;
    typedef std::shared_ptr<ObjectDataMap> ObjectDataMapPtr;

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

        //TODO RENAME
        /**
         * Stores the given value-weight pair with the given key.
         * If the key is already stored it will be overridden with the new pair.
         * @param key The key to store the value-weight pair at
         * @param value The value of the value-weight pair
         * @param weight The weight of the value-weight pair
         */
        void Put(std::string key, double value, double weight);

        /**
         * Stores the given value-weight pair with the given key.
         * If the key is already stored it will be overridden with the new pair.
         * @param key The key to store the value-weight pair at
         * @param value_weight The value-weight pair
         */
        void Put(std::string key, std::pair<double, double> value_weight);

        virtual double CompareTo(ObjectDataPtr obj) const override;

        virtual ObjectDataPtr Interpolate(ObjectDataPtr obj, double fraction) const override;

        virtual void Visualize(cv::Mat& image, cv::Scalar& color) const override;
    };
}


#endif //GBMOT_OBJECTDATAMAP_H
