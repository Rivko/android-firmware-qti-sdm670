/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "sensor_attributes.h"

using namespace std;

const string& sensor_attributes::get_string(int32_t attr_id) const
{
    const sns_std_attr_value& attr_value =  get(attr_id);
    if (attr_value.values_size() > 0 && attr_value.values(0).has_str()) {
        return attr_value.values(0).str();
    }
    throw runtime_error("attribute value is not a string");
}

vector<float> sensor_attributes::get_floats(int32_t attr_id) const
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
        return vector<float>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<float> floats(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (!attr_value.values(i).has_flt()) {
            throw runtime_error("attribute " + to_string(attr_id) +
                                " element " + to_string(i) + " is not a float");
        }
        floats[i] = attr_value.values(i).flt();
    }
    return floats;
}

vector<int64_t> sensor_attributes::get_ints(int32_t attr_id) const
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
        return vector<int64_t>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<int64_t> ints(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (!attr_value.values(i).has_sint()) {
            throw runtime_error("attribute " + to_string(attr_id) +
                                " element " + to_string(i) + " is not an int");
        }
        ints[i] = attr_value.values(i).sint();
    }
    return ints;
}

vector<bool> sensor_attributes::get_bools(int32_t attr_id) const
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
        return vector<bool>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<bool> bools(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (!attr_value.values(i).has_boolean()) {
            throw runtime_error("attribute " + to_string(attr_id) +
                                " element " + to_string(i) + " is not a boolean");
        }
        bools[i] = attr_value.values(i).boolean();
    }
    return bools;
}

vector<pair<float, float>> sensor_attributes::get_ranges(int32_t attr_id) const
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
        return vector<pair<float, float>>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<pair<float, float>> ranges(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (attr_value.values(i).has_subtype() &&
            attr_value.values(i).subtype().values_size() > 1 &&
            attr_value.values(i).subtype().values(0).has_flt() &&
            attr_value.values(i).subtype().values(1).has_flt())
        {
            ranges[i].first = attr_value.values(i).subtype().values(0).flt();
            ranges[i].second = attr_value.values(i).subtype().values(1).flt();
        } else {
            throw runtime_error(
                "malformed value for SNS_STD_SENSOR_ATTRID_RANGES");
        }
    }
    return ranges;
}

int64_t sensor_attributes::get_stream_type() const
{
    auto ints = get_ints(SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
    if (ints.size() == 0) {
        throw runtime_error("stream_type attribute not available");
    }
    return ints[0];
}
