/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_PARSER_H_
#define CONFIGURATION_PARSER_H_
extern "C"{
#include "json_common_types.h"
}
#include "json_common_structs.h"
#include "json_configuration_types.h"
//#include "simple_scene_calibration.h"
#include <json/json.h>


class JSONConfigurationParser
{
public:
    JSONConfigurationParser();
    ~JSONConfigurationParser();
    JSONVAStatus ClearConfigurationContent(JSONConfigurationContent &
        configuration_content);
    JSONVAStatus LoadFileToString(const char *filename, char ** doc_str);
    JSONVAStatus ParseDoc(const char* doc, JSONConfigurationContent &
        configuration_content);
    JSONVAStatus ParseDoc(const char* doc, JSONConfiguration ** configuration);
    JSONVAStatus ParseGeneralConfiguration(const Json::Value
        general_configuration, JSONConfigurationContent & configuration_content);
    JSONVAStatus ParseSceneCalibration(const Json::Value scene_calibration,
        JSONConfigurationContent & configuration_content);
    JSONVAStatus ParseZones(const Json::Value zones, JSONConfigurationContent &
        configuration_content);
    JSONVAStatus ParseLines(const Json::Value lines, JSONConfigurationContent &
        configuration_content);
    JSONVAStatus ParseAtomicRules(const Json::Value atomic_rules,
        JSONConfigurationContent & configuration_content);
    JSONVAStatus ParseRules(const Json::Value rules, JSONConfigurationContent &
        configuration_content);
    JSONVAStatus ParseExtension(const Json::Value extensions,
        JSONConfigurationContent & configuration_content);
    struct JSONConfiguration configuration_;
    JSONConfigurationContent configuration_content_;
    std::string doc_str_;
private:
    Json::Value configuration_json_;
};

#endif //CONFIGURATION_PARSER_H_
