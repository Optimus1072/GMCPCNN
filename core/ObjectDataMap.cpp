//
// Created by wrede on 22.04.16.
//

#include "ObjectDataMap.h"
#include "../util/MyMath.h"

namespace core
{
    void ObjectDataMap::Print(std::ostream& os) const
    {
        os << "ObjectDataMap{frame:" << GetFrameIndex();

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

    void ObjectDataMap::Put(std::string key, double value,
                            double weight)
    {
        value_weight_map_[key] = std::make_pair(value, weight);
    }

    void ObjectDataMap::Put(std::string key,
                            std::pair<double, double> value_weight)
    {
        value_weight_map_[key] = value_weight;
    }

    double ObjectDataMap::CompareTo(ObjectDataPtr obj) const
    {
        ObjectDataMapPtr obj_map = std::static_pointer_cast<ObjectDataMap>(obj);

        double diff = 0.0;

        for (auto it = value_weight_map_.begin(); it != value_weight_map_.end(); ++it)
        {
            // |other_value - this_value| * this_weight;
            diff += fabs(obj_map->value_weight_map_[it->first].first - it->second.first)
                    * it->second.second;
        }

        return diff;
    }

    ObjectDataPtr ObjectDataMap::Interpolate(ObjectDataPtr obj,
                                          double fraction) const
    {
        ObjectDataMapPtr obj_map = std::static_pointer_cast<ObjectDataMap>(obj);

        fraction = util::MyMath::Clamp(0.0, 1.0, fraction);

        double frame_index = util::MyMath::Lerp(GetFrameIndex(),
                                              obj_map->GetFrameIndex(),
                                              fraction);

        ObjectDataMapPtr obj_out(new ObjectDataMap(static_cast<size_t>(fabs(frame_index))));

        // Interpolate all values but leave the weights untouched
        for (auto iter = value_weight_map_.begin();
             iter != value_weight_map_.end();
             ++iter)
        {
            obj_out->Put(
                    iter->first,
                    util::MyMath::Lerp(iter->second.first,
                                     obj_map->value_weight_map_[iter->first].first,
                                     fraction),
                    iter->second.second
            );
        }

        return obj_out;
    }

    void ObjectDataMap::Visualize(cv::Mat& image, cv::Scalar& color) const
    {
        ObjectData::Visualize(image, color);
    }
}

