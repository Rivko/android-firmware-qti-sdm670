/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"

/**
 * @brief A set of sensor attributes
 *
 * Each sensor may have an instance of this class to store the
 * attributes received from the SSC.
 *
 * Provides helper functions for easy retrieval of the
 * attributes
 *
 */
class sensor_attributes
{
public:

    /**
     * @brief save an entry in attribute map
     * @param attr_id  attribute ID
     * @param attr_value  protobuf-decoded attribute value object
     */
    void set(int32_t attr_id, const sns_std_attr_value& attr_value)
    {
        _attr_map[attr_id] = attr_value;
    }

    /**
     * @brief get protobuf decoded attribute value object
     * @param attr_id attribute ID
     * @return const sns_std_attr_value&
     */
    const sns_std_attr_value& get(int32_t attr_id) const
    {
        return _attr_map.at(attr_id);
    }

    /**
     * @brief checks if given attribute is available
     * @param attr_id attribute ID
     * @return bool true if attribute is present, else false
     */
    bool is_present(int32_t attr_id) const
    {
        return (_attr_map.find(attr_id) != _attr_map.end());
    }

    /**
     * @brief get string value of an attribute
     * @param attr_id attribute ID
     * @return const std::string&
     */
    const std::string& get_string(int32_t attr_id) const;

    /**
     * @brief get value as vector of floats
     * @param attr_id attribute ID
     * @return vector<float> value of the attribute
     */
    std::vector<float> get_floats(int32_t attr_id) const;

    /**
     * @brief get value as vector of ints
     * @param attr_id attribute ID
     * @return vector<int64_t> value of the attribute
     */
    std::vector<int64_t> get_ints(int32_t attr_id) const;

    /**
     * @brief get value as vector of bools
     * @param attr_id attribute ID
     * @return vector<bool> value of the attribute
     */
    std::vector<bool> get_bools(int32_t attr_id) const;

    /**
     * @brief get ranges attribute
     * @return std::vector<std::pair<float,float>> vector of pairs
     *         containing {min, max} operating ranges
     */
    std::vector<std::pair<float, float>> get_ranges(
            int32_t attr_id = SNS_STD_SENSOR_ATTRID_RANGES) const;

    /**
     * @brief get stream_type attribute
     * @return int64_t stream_type
     */
    int64_t get_stream_type() const;

private:
    /* hashmap for storing attribute id-value pairs */
    std::unordered_map<int32_t, sns_std_attr_value> _attr_map;
};
