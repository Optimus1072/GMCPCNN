//
// Created by wrede on 22.04.16.
//

#include "ObjectDataMap.h"

namespace core
{
    void ObjectDataMap::Print(std::ostream& os) const
    {
        os << "ObjectDataMap{" << GetFrameIndex();

        for (auto it = value_weight_map_.begin(); it != value_weight_map_.end(); ++it)
        {
            os << "," << it->first
            << ":" << it->second.first
            << "*" << it->second.second;
        }

        os << "}";
    }

    ObjectDataMap::ObjectDataMap(size_t frame_index)
            : ObjectData(frame_index)
    {
        value_weight_map_ = std::unordered_map<std::string, std::pair<double, double>>();
    }

    ObjectDataMap::ObjectDataMap(size_t frame_index,
                                 std::vector<std::string> keys,
                                 std::vector<double> value_list)
            : ObjectDataMap(frame_index)
    {
        double weight = 1.0 / value_list.size();

        for (size_t i = 0; i < value_list.size() && i < keys.size(); ++i)
        {
            value_weight_map_[keys[i]] = std::make_pair(value_list[i], weight);
        }
    }

    ObjectDataMap::ObjectDataMap(size_t frame_index,
                                 std::vector<std::string> keys,
                                 std::vector<double> value_list,
                                 std::vector<double> weight_list)
            : ObjectDataMap(frame_index)
    {
        for (size_t i = 0;
             i < value_list.size() && i < keys.size() && i < weight_list.size();
             ++i)
        {
            value_weight_map_[keys[i]] = std::make_pair(value_list[i], weight_list[i]);
        }
    }

    ObjectDataMap::ObjectDataMap(size_t frame_index,
                                 std::vector<std::string> keys,
                                 std::vector<std::pair<double, double>> value_weight_list)
            : ObjectDataMap(frame_index)
    {
        for (size_t i = 0; i < value_weight_list.size() && i < keys.size(); ++i)
        {
            value_weight_map_[keys[i]] = value_weight_list[i];
        }
    }

    double ObjectDataMap::GetValue(std::string key)
    {
        return value_weight_map_[key].first;
    }

    double ObjectDataMap::GetWeight(std::string key)
    {
        return value_weight_map_[key].second;
    }

    void ObjectDataMap::PutValueWeight(std::string key, double value,
                                       double weight)
    {
        value_weight_map_[key] = std::make_pair(value, weight);
    }

    void ObjectDataMap::PutValueWeight(std::string key,
                                       std::pair<double, double> value_weight)
    {
        value_weight_map_[key] = value_weight;
    }

    double ObjectDataMap::CompareTo(ObjectData *obj)
    {
        ObjectDataMap* obj_dm = dynamic_cast<ObjectDataMap*>(obj);

        if (obj_dm)
        {
            return CompareTo(obj_dm);
        }
        else
        {
            return 0.0;
        }
    }

    double ObjectDataMap::CompareTo(ObjectDataMap *obj)
    {
        double diff = 0.0;

        for (auto it = value_weight_map_.begin(); it != value_weight_map_.end(); ++it)
        {
            // |other_value - this_value| * this_weight;
            diff += fabs(obj->value_weight_map_[it->first].first - it->second.first)
                    * it->second.second;
        }

        return diff;
    }
}

