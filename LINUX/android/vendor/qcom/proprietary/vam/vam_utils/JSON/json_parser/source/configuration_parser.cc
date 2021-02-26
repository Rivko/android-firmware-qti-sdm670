/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "configuration_parser.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "json_configuration_types.h"
#include <string.h>

using namespace std;

JSONConfigurationParser::JSONConfigurationParser()
{
    configuration_.cnt_atomic_rules = 0;
    configuration_.cnt_rules = 0;
    configuration_.cnt_zones = 0;
#if (VA_CONFIG_VERSION==0)
    for (int i = 0; i < EXTENSION_MAX; i++){
        configuration_.extension[i].data = 0;
    }
    configuration_.cnt_extension = 0;
    configuration_.cnt_lines = 0;
#endif
#if (VA_CONFIG_VERSION==1)
    for (int i = 0; i < 5; i++){
        configuration_.reserve[i] = 0;
        configuration_.reserve_str[i][0] = 0;
    }
#endif
}
JSONConfigurationParser::~JSONConfigurationParser()
{
#if (VA_CONFIG_VERSION==0)
    if (configuration_content_.extension.size() > 0){
        for (int i = 0; i < configuration_content_.extension.size(); i++){
            delete []configuration_content_.extension[i].data;
        }
        configuration_content_.extension.clear();
    }
    if (configuration_.cnt_extension > 0){
        for (int i = 0; i < configuration_.cnt_extension; i++){
            delete []configuration_.extension[i].data;
        }
    }
    configuration_content_.lines.clear();
#endif
    configuration_content_.atomic_rules.clear();
    configuration_content_.rules.clear();
    configuration_content_.zones.clear();
}
JSONVAStatus JSONConfigurationParser::ClearConfigurationContent(JSONConfigurationContent
    &configuration_content)
{
#if (VA_CONFIG_VERSION==0)
    configuration_content.lines.clear();
    configuration_content.extension.clear();
#endif
#if (VA_CONFIG_VERSION==1)
    for (int i = 0; i < 5; i++){
        configuration_content.reserve[i] = 0;
        configuration_content.reserve_str[i][0] = 0;
    }
#endif
    configuration_content.zones.clear();
    configuration_content.atomic_rules.clear();
    configuration_content.rules.clear();
    return JSON_VA_OK;
}
JSONVAStatus JSONConfigurationParser::LoadFileToString(const char *filename, char **
    doc_str)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in){
        in.seekg(0, std::ios::end);
        doc_str_.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&doc_str_[0], doc_str_.size());
        in.close();
        *doc_str = (char*)doc_str_.c_str();
        return JSON_VA_OK;
    }
    else{
        *doc_str = 0;
        return JSON_VA_INVALID;
    }

}
JSONVAStatus JSONConfigurationParser::ParseDoc(const char* doc, JSONConfigurationContent
    & configuration_content)
{
#if (VA_CONFIG_VERSION==0)
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    ClearConfigurationContent(configuration_content);
    bool parsingSuccessful = reader.parse(doc, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse configuration\n"
            << reader.getFormattedErrorMessages();
        return JSON_VA_INVALID;
    }
    if (root.isMember("general_configuration")){
        const Json::Value general_configuration = root["general_configuration"];
        ParseGeneralConfiguration(general_configuration, configuration_content);
    }
    else
        return JSON_VA_INVALID;
    if (root.isMember("scene_calibration")){
        const Json::Value scene_calibration = root["scene_calibration"];
        ParseSceneCalibration(scene_calibration, configuration_content);
    }
    else{
        configuration_content.scene_calibration.cnt_objects = 0;
    }

    if (root.isMember("rule_configuration")){
        const Json::Value rule_configuration = root["rule_configuration"];
        if (rule_configuration.isMember("zones")){
            const Json::Value zones = rule_configuration["zones"];
            ParseZones(zones, configuration_content);
        }
        if (rule_configuration.isMember("lines")){
            const Json::Value lines = rule_configuration["lines"];
            ParseLines(lines, configuration_content);
        }
        if (rule_configuration.isMember("atomic_rules")){
            const Json::Value atomic_rules = rule_configuration["atomic_rules"];
            ParseAtomicRules(atomic_rules, configuration_content);
        }
        else
            return JSON_VA_INVALID;
        if (rule_configuration.isMember("rules")){
            const Json::Value rules = rule_configuration["rules"];
            ParseRules(rules, configuration_content);
        }
    }
    else
        return JSON_VA_INVALID;
    if (root.isMember("extension")){
        const Json::Value extensions = root["extension"];
        ParseExtension(extensions, configuration_content);
    }
    return JSON_VA_OK;
#endif
#if (VA_CONFIG_VERSION==1)
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    JSONVAStatus ret = JSON_VA_INVALID;
    ClearConfigurationContent(configuration_content);
    bool parsingSuccessful = reader.parse(doc, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse configuration\n"
            << reader.getFormattedErrorMessages();
        return JSON_VA_INVALID;
    }
    if (root.isMember("version")){
        const Json::Value version = root["version"];
        strcpy(configuration_content.version, version.asString().c_str());
        if (strncmp(configuration_content.version, "1.0", 3) != 0)
        {
            printf("configuration version is not matched with current parser\n");
        }
    }
    else{
        printf("version number of configuration is not found\n");
        //return JSON_VA_INVALID;
    }
    if (root.isMember("id")){
        const Json::Value id = root["id"];
        strcpy(configuration_content.id, id.asString().c_str());
    }
    else{
        printf("ID of configuration is not found\n");
        configuration_content.id[0] = 0;
    }
    if (root.isMember("time_stamp")){
        const Json::Value time_stamp = root["time_stamp"];
        configuration_content.time_stamp = time_stamp.asUInt64();
    }
    else{
        configuration_content.time_stamp = 0;
    }
    for (int k = 0; k < 5; k++){
        configuration_content.reserve_str[k][0] = 0;
        configuration_content.reserve[k] = 0;
    }
    if (root.isMember("reserve")){
        Json::Value reserve = root["reserve"];
        Json::Value::iterator it_reserve;
        int k = 0;
        for (it_reserve = reserve.begin(); it_reserve != reserve.end(); ++it_reserve){
#if defined(JSON_HAS_INT64)
            configuration_content.reserve[k] = (*it_reserve).asInt64();
#else
            configuration_content.reserve[k] = (*it_reserve).asInt();
#endif
            k++;
            if (k == 5)
                break;
        }
    }
    if (root.isMember("reserve_str")){
        Json::Value reserve_str = root["reserve_str"];
        Json::Value::iterator it_reserveStr;
        int k = 0;
        for (it_reserveStr = reserve_str.begin(); it_reserveStr != reserve_str.end(); ++it_reserveStr){
            strcpy(configuration_content.reserve_str[k], (*it_reserveStr).asString().c_str());
            k++;
            if (k == 5)
                break;
        }
    }
    if (root.isMember("scene_calibration")){
        const Json::Value scene_calibration = root["scene_calibration"];
        ret = ParseSceneCalibration(scene_calibration, configuration_content);
        if (ret != JSON_VA_OK){
            printf("Failed in ParseSceneCalibration\n");
            return ret;
        }
    }
    else{
        configuration_content.scene_calibration.cnt_objects = 0;
    }
    if (root.isMember("zones")){
        const Json::Value zones = root["zones"];
        ret = ParseZones(zones, configuration_content);
        if (ret != JSON_VA_OK){
            printf("Failed in ParseZones\n");
            return ret;
        }
    }
    if (root.isMember("atomic_rules")){
        const Json::Value atomic_rules = root["atomic_rules"];
        ret = ParseAtomicRules(atomic_rules, configuration_content);
        if (ret != JSON_VA_OK){
            printf("Failed in ParseAtomicRules\n");
            return ret;
        }
    }
    else{
        printf("No zone is defined in the configuration\n");
        return JSON_VA_INVALID;
    }
    if (root.isMember("rules")){
        const Json::Value rules = root["rules"];
        ret = ParseRules(rules, configuration_content);
        if (ret != JSON_VA_OK){
            printf("Failed in ParseRules\n");
            return ret;
        }
    }
    return JSON_VA_OK;
#endif
}
JSONVAStatus JSONConfigurationParser::ParseDoc(
    const char* doc, JSONConfiguration ** pp_configuration)
{
#if (VA_CONFIG_VERSION==0)
    JSONConfigurationContent &configuration_content = configuration_content_;
    *pp_configuration = &configuration_;
    JSONConfiguration &configuration = configuration_;
    for (int i = 0; i < configuration.cnt_extension; i++){
        if (configuration.extension[i].data){
            delete[]configuration.extension[i].data;
            configuration.extension[i].data = 0;
        }
    }
    if (doc==0||strcmp(doc, "")==0)
        return JSON_VA_INVALID;
    ParseDoc(doc, configuration_content);
    configuration.cnt_extension = configuration_content.extension.size();
    configuration.cnt_lines = configuration_content.lines.size();
    configuration.cnt_zones = configuration_content.zones.size();
    configuration.cnt_atomic_rules = configuration_content.atomic_rules.size();
    configuration.cnt_rules = configuration_content.rules.size();
    //general_configuration
    strcpy(configuration.general_configuration.configuration_id.uuid,
        configuration_content.general_configuration.configuration_id.uuid);
    configuration.general_configuration.scene_type =
        configuration_content.general_configuration.scene_type;
    configuration.general_configuration.time_stamp =
        configuration_content.general_configuration.time_stamp;
    strcpy(configuration.general_configuration.version,
        configuration_content.general_configuration.version);
    //scene_calibration
    configuration.scene_calibration.cnt_objects =
        configuration_content.scene_calibration.cnt_objects;
    memcpy(configuration.scene_calibration.objects,
        configuration_content.scene_calibration.objects,
        sizeof(CalibObject)*configuration_content.scene_calibration.cnt_objects);
    configuration.scene_calibration.length_unit =
        configuration_content.scene_calibration.length_unit;
    //zones
    for (int i = 0; i < configuration.cnt_zones; i++){
        strcpy(configuration.zones[i].zone_id.uuid,
            configuration_content.zones[i].zone_id.uuid);
        configuration.zones[i].cnt_points =
            configuration_content.zones[i].cnt_points;
        memcpy(configuration.zones[i].points,
            configuration_content.zones[i].points,
            sizeof(VAPoint)*configuration_content.zones[i].cnt_points);
        configuration.zones[i].zone_type =
            configuration_content.zones[i].zone_type;
    }
    //lines
    for (int i = 0; i < configuration.cnt_lines; i++){
        strcpy(configuration.lines[i].line_id.uuid,
            configuration_content.lines[i].line_id.uuid);
        configuration.lines[i].cnt_points =
            configuration_content.lines[i].cnt_points;
        memcpy(configuration.lines[i].points,
            configuration_content.lines[i].points,
            sizeof(VAPoint)*configuration_content.lines[i].cnt_points);
    }
    //atomic rules
    for (int i = 0; i < configuration.cnt_atomic_rules; i++){
        memcpy(&configuration.atomic_rules[i].attributes,
            &configuration_content.atomic_rules[i].attributes,
            sizeof(union AttributesOfAtomicRule));
        configuration.atomic_rules[i].event_type =
            configuration_content.atomic_rules[i].event_type;
        strcpy(configuration.atomic_rules[i].line_id.uuid,
            configuration_content.atomic_rules[i].line_id.uuid);
        configuration.atomic_rules[i].cnt_zones =
            configuration_content.atomic_rules[i].cnt_zones;
        //for (int k = 0; k<configuration.atomic_rules[i].cnt_zones;k++)
        strcpy(configuration.atomic_rules[i].zone_id.uuid,
            configuration_content.atomic_rules[i].zone_id.uuid);
        strcpy(configuration.atomic_rules[i].rule_id.uuid,
            configuration_content.atomic_rules[i].rule_id.uuid);
        strcpy(configuration.atomic_rules[i].rule_name,
            configuration_content.atomic_rules[i].rule_name);
        configuration.atomic_rules[i].status =
            configuration_content.atomic_rules[i].status;
        configuration.atomic_rules[i].detector_type = configuration_content.atomic_rules[i].detector_type;
        configuration.atomic_rules[i].cnt_zones = configuration_content.atomic_rules[i].cnt_zones;
        for (int k = 0; k < configuration.atomic_rules[i].cnt_zones - 1; k++){
            strcpy(configuration.atomic_rules[i].exclude_zone_ids[k].uuid, configuration_content.atomic_rules[i].exclude_zone_ids[k].uuid);
        }
    }
    //rules
    for (int i = 0; i < configuration.cnt_rules; i++){
        strcpy(configuration.rules[i].operation,
            configuration_content.rules[i].operation);
        strcpy(configuration.rules[i].rule_id.uuid,
            configuration_content.rules[i].rule_id.uuid);
        strcpy(configuration.rules[i].rule_name,
            configuration_content.rules[i].rule_name);
        strcpy(configuration.rules[i].sub_rule_id1.uuid,
            configuration_content.rules[i].sub_rule_id1.uuid);
        configuration.rules[i].relationship =
            configuration_content.rules[i].relationship;
        configuration.rules[i].status = configuration_content.rules[i].status;
        if (strcmp(configuration.rules[i].operation, "NOT")){
            strcpy(configuration.rules[i].sub_rule_id2.uuid,
                configuration_content.rules[i].sub_rule_id2.uuid);
        }
    }
    //extensions
    for (int i = 0; i < configuration.cnt_extension; i++){
        configuration.extension[i].size_data =
            configuration_content.extension[i].size_data;
        configuration.extension[i].data =
            new char[configuration.extension[i].size_data+1];
        memcpy(configuration.extension[i].data,
            configuration_content.extension[i].data,
            configuration.extension[i].size_data);
        configuration.extension[i].data[configuration.extension[i].size_data]
            = 0;
        strcpy(configuration.extension[i].engine_id,
            configuration_content.extension[i].engine_id);
    }

    return JSON_VA_OK;
#endif
#if (VA_CONFIG_VERSION==1)
    JSONConfigurationContent &configuration_content = configuration_content_;
    *pp_configuration = &configuration_;
    if (strcmp(doc, "")==0)
        return JSON_VA_INVALID;
    JSONConfiguration &configuration = configuration_;
    JSONVAStatus res = ParseDoc(doc, configuration_content);
    if (res != JSON_VA_OK){
        printf("parseDoc failed\n");
        return res;
    }
    configuration.cnt_zones = configuration_content.zones.size();
    configuration.cnt_atomic_rules = configuration_content.atomic_rules.size();
    configuration.cnt_rules = configuration_content.rules.size();
    //general_configuration
    strcpy(configuration.id,
        configuration_content.id);
    configuration.time_stamp =
        configuration_content.time_stamp;
    strcpy(configuration.version,
        configuration_content.version);
    memcpy(configuration.reserve,
        configuration_content.reserve,
        sizeof(int64_t) * 5);
    for (int k = 0; k < 5; k++){
        strcpy(configuration.reserve_str[k], configuration_content.reserve_str[k]);
    }
    //scene_calibration
    configuration.scene_calibration.cnt_objects =
        configuration_content.scene_calibration.cnt_objects;
    memcpy(configuration.scene_calibration.objects,
        configuration_content.scene_calibration.objects,
        sizeof(CalibObject)*configuration_content.scene_calibration.cnt_objects);
    configuration.scene_calibration.length_unit =
        configuration_content.scene_calibration.length_unit;
    //zones
    for (uint32_t i = 0; i < configuration.cnt_zones; i++){
        strcpy(configuration.zones[i].id,
            configuration_content.zones[i].id);
        configuration.zones[i].cnt_points =
            configuration_content.zones[i].cnt_points;
        memcpy(configuration.zones[i].points,
            configuration_content.zones[i].points,
            sizeof(VAPoint)*configuration_content.zones[i].cnt_points);
        configuration.zones[i].type =
            configuration_content.zones[i].type;
    }
    //atomic rules
    for (uint32_t i = 0; i < configuration.cnt_atomic_rules; i++){
        memcpy(configuration.atomic_rules[i].reserve,
            configuration_content.atomic_rules[i].reserve,
            sizeof(int64_t) * 5);
        for (int k = 0; k < 5; k++){
            strcpy(configuration.atomic_rules[i].reserve_str[k], configuration_content.atomic_rules[i].reserve_str[k]);
        }
        configuration.atomic_rules[i].event_type =
            configuration_content.atomic_rules[i].event_type;
        configuration.atomic_rules[i].cnt_zones =
            configuration_content.atomic_rules[i].cnt_zones;
        //for (uint32_t k = 0; k<configuration.atomic_rules[i].cnt_zones;k++)
        strcpy(configuration.atomic_rules[i].id,
            configuration_content.atomic_rules[i].id);
        strcpy(configuration.atomic_rules[i].name,
            configuration_content.atomic_rules[i].name);
        configuration.atomic_rules[i].status =
            configuration_content.atomic_rules[i].status;
        configuration.atomic_rules[i].cnt_zones = configuration_content.atomic_rules[i].cnt_zones;
        configuration.atomic_rules[i].sensitivity = configuration_content.atomic_rules[i].sensitivity;
        configuration.atomic_rules[i].min_size = configuration_content.atomic_rules[i].min_size;
        for (uint32_t k = 0; k < configuration.atomic_rules[i].cnt_zones; k++){
            strcpy(configuration.atomic_rules[i].zone_ids[k], configuration_content.atomic_rules[i].zone_ids[k]);
        }
        memcpy(configuration.atomic_rules[i].reserve,
            configuration_content.atomic_rules[i].reserve,
            sizeof(int64_t) * 5);
        for (int k = 0; k < 5; k++){
            strcpy(configuration.atomic_rules[i].reserve_str[k], configuration_content.atomic_rules[i].reserve_str[k]);
        }
        if (configuration.atomic_rules[i].cnt_zones == 0){
            if (configuration.atomic_rules[i].event_type != 3 && configuration.atomic_rules[i].event_type != 5){
                strcpy(configuration.atomic_rules[i].zone_ids[0], "00000000-0000-0000-0000-000000000001");
                configuration.atomic_rules[i].cnt_zones = 1;
            }
            else{
                printf("There is no line assigned to counting or line crosssing\n");
                return JSON_VA_INVALID;
            }
        }
        else{
            int flag = 0;
            for (uint32_t k = 0; k < configuration.atomic_rules[i].cnt_zones; k++){
                int found = 0;
                for (uint32_t m = 0; m < configuration.cnt_zones; m++){
                    if (strcmp(configuration.atomic_rules[i].zone_ids[k], configuration.zones[m].id) == 0){
                        found = 1;
                        if (configuration.zones[m].type != ZONE_TYPE_EXCLUDE){
                            flag++;
                            switch (configuration.atomic_rules[i].event_type){
                            case    METADATA_EVENT_CAMERA_TAMPER_DETECTED:
                            case    METADATA_EVENT_MOTION_DETECTED:
                            case    METADATA_EVENT_INTRUSION_DETECTED:
                            case    METADATA_EVENT_LOITERING_DETECTED:
                            case    METADATA_EVENT_FACE_DETECTED:
                            case    METADATA_EVENT_FACE_RECOGNIZED:
                            case    METADATA_EVENT_OBJECT_CLASSIFIED:
                            case    METADATA_EVENT_OBJECT_DETECTED:
                            case    METADATA_EVENT_COMPOUND_EVENT:
                            //case    METADATA_EVENT_HEATMAP_GENERATED:
                                if (configuration.zones[m].type != ZONE_TYPE_INCLUDE){
                                    printf("event_type and zone_type are not compatible\n");
                                    return JSON_VA_INVALID;
                                }
                                break;
                            case    METADATA_EVENT_LINECROSSED:
                            case    METADATA_EVENT_OBJECT_COUNTED:
                                if (configuration.zones[m].type != LINE_TYPE_BOTH_DIR &&
                                    configuration.zones[m].type != LINE_TYPE_DIR){
                                    printf("event_type and zone_type are not compatible\n");
                                    return JSON_VA_INVALID;
                                    }
                                break;
                            default:
                                break;

                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    //rules
    for (uint32_t i = 0; i < configuration.cnt_rules; i++){
        configuration.rules[i].operation =
            configuration_content.rules[i].operation;
        strcpy(configuration.rules[i].id,
            configuration_content.rules[i].id);
        strcpy(configuration.rules[i].name,
            configuration_content.rules[i].name);
        strcpy(configuration.rules[i].sub_rule_id1,
            configuration_content.rules[i].sub_rule_id1);
        configuration.rules[i].relationship =
            configuration_content.rules[i].relationship;
        configuration.rules[i].status = configuration_content.rules[i].status;
        if (configuration.rules[i].operation!=2){
            strcpy(configuration.rules[i].sub_rule_id2,
                configuration_content.rules[i].sub_rule_id2);
        }
        memcpy(configuration.rules[i].reserve,
            configuration_content.rules[i].reserve,
            sizeof(int64_t) * 5);
        for (int k = 0; k < 5; k++){
            strcpy(configuration.rules[i].reserve_str[k], configuration_content.rules[i].reserve_str[k]);
        }
    }

    return JSON_VA_OK;
#endif
}
#if (VA_CONFIG_VERSION==0)
JSONVAStatus JSONConfigurationParser::ParseGeneralConfiguration(const Json::Value
    general_configuration, JSONConfigurationContent & configuration_content)
{

    if (general_configuration.isMember("configuration_id")){
        strcpy(configuration_content.general_configuration.configuration_id.uuid,
            general_configuration["configuration_id"].asString().c_str());
    }
    else
        return JSON_VA_INVALID;
    if (general_configuration.isMember("scene_type")){
        configuration_content.general_configuration.scene_type = (SceneType)
            general_configuration["scene_type"].asInt();
    }
    else
        return JSON_VA_INVALID;
    if (general_configuration.isMember("time_stamp")){
        configuration_content.general_configuration.time_stamp = (uint64_t)
            general_configuration["time_stamp"].asUInt64();
    }
    else
        return JSON_VA_INVALID;
    if (general_configuration.isMember("version")){
        strcpy(configuration_content.general_configuration.version,
            general_configuration["version"].asString().c_str());
    }
    else
        return JSON_VA_INVALID;
    return JSON_VA_OK;
}
JSONVAStatus JSONConfigurationParser::ParseLines(const Json::Value lines,
    JSONConfigurationContent & configuration_content)
{
    Json::Value::iterator it;
    for (it = lines.begin(); it != lines.end(); ++it){
        JSONVALine line;
        strcpy(line.line_id.uuid, (*it)["line_id"].asString().c_str());
        Json::Value::iterator it_points;
        Json::Value points = (*it)["points"];
        line.cnt_points = 0;
        for (it_points = points.begin(); it_points != points.end(); ++it_points){
            VAPoint point;
            point.x = (*it_points)["x"].asInt();
            point.y = (*it_points)["y"].asInt();
            line.points[line.cnt_points]=point;
            line.cnt_points++;
        }
        configuration_content.lines.push_back(line);
    }
    return JSON_VA_OK;
}
#endif
JSONVAStatus JSONConfigurationParser::ParseSceneCalibration(const Json::Value
    scene_calibration, JSONConfigurationContent & configuration_content)
{
    configuration_content.scene_calibration.cnt_objects=0;
    if (scene_calibration.isMember("length_unit"))
        configuration_content.scene_calibration.length_unit = (VALengthUnit)
        scene_calibration["length_unit"].asInt();
    else
        configuration_content.scene_calibration.length_unit = (VALengthUnit)0;
    if (scene_calibration.isMember("objects"))
    {
        for (Json::Value::iterator it = scene_calibration["objects"].begin();
            it != scene_calibration["objects"].end(); ++it){
            CalibObject object;
            object.x = (*it)["x"].asInt();
            object.y = (*it)["y"].asInt();
            object.width = (*it)["width"].asInt();
            object.height = (*it)["height"].asInt();
            object.physical_height = (*it)["physical_height"].asFloat();
            configuration_content.scene_calibration.objects[configuration_content.scene_calibration.cnt_objects]=object;
            configuration_content.scene_calibration.cnt_objects++;
        }
    }
    return JSON_VA_OK;
}

JSONVAStatus JSONConfigurationParser::ParseZones(const Json::Value zones,
    JSONConfigurationContent & configuration_content)
{
    JSONVAStatus ret = JSON_VA_INVALID;
    Json::Value::iterator it;
    for (it = zones.begin(); it != zones.end(); ++it){
        JSONVAZone zone;
#if (VA_CONFIG_VERSION==0)
        strcpy(zone.zone_id.uuid, (*it)["zone_id"].asString().c_str());
        zone.zone_type = (ZoneType)(*it)["zone_type"].asInt();
#endif
#if (VA_CONFIG_VERSION==1)
        if ((*it).isMember("id")){
            strcpy(zone.id, (*it)["id"].asString().c_str());
        }
        else{
            printf("Failure: zone has no id\n");
            return ret;
        }
        if ((*it).isMember("type")){
            zone.type = (ZoneType)(*it)["type"].asInt();
        }
        else{
            printf("Failure: zone has no type\n");
            return ret;
        }

#endif
        Json::Value::iterator it_points;
        Json::Value points = (*it)["points"];
        zone.cnt_points = 0;
        for (it_points = points.begin(); it_points != points.end(); ++it_points){
            VAPoint point;
            point.x = (*it_points)["x"].asInt();
            point.y = (*it_points)["y"].asInt();
            zone.points[zone.cnt_points]=point;
            zone.cnt_points++;
        }
        configuration_content.zones.push_back(zone);
    }
    return JSON_VA_OK;
}
JSONVAStatus JSONConfigurationParser::ParseAtomicRules(const Json::Value atomic_rules,
    JSONConfigurationContent & configuration_content)
{
#if (VA_CONFIG_VERSION==0)
    string default_atomic_rulenames[12] ={
        "Undefined",
        "Camera tamper detected",
        "Motion detected",
        "Intrustion detected",
        "Line crossed",
        "Object counted",
        "Loitering detected",
        "Face detected",
        "Face Recognized",
        "Object classified",
        "Object detected",
        "Heatmap generated"
    };
    Json::Value::iterator it;
    for (it = atomic_rules.begin(); it != atomic_rules.end(); ++it){
        JSONVAAtomicRule atomic_rule;
        atomic_rule.cnt_zones = 0;
        atomic_rule.detector_type = VA_DETECTORTYPE_ZONE;
        if ((*it).isMember("event_type")){
            atomic_rule.event_type = (MetadataEventType)(*it)["event_type"].asInt();
            if (atomic_rule.event_type<0 || atomic_rule.event_type >12)
                return JSON_VA_INVALID;
        }
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("exclude_zone_ids")){
            Json::Value exclude_zone_ids = (*it)["exclude_zone_ids"];
            Json::Value::iterator it_exclude;
            int k = 0;
            for (it_exclude = exclude_zone_ids.begin(); it_exclude != exclude_zone_ids.end(); ++it_exclude){
                strcpy(atomic_rule.exclude_zone_ids[k].uuid, (*it_exclude).asString().c_str());
                atomic_rule.cnt_zones++;
                if (atomic_rule.cnt_zones > EXCLUDE_ZONE_MAX)
                    atomic_rule.cnt_zones = EXCLUDE_ZONE_MAX;
                k++;
                if (k == EXCLUDE_ZONE_MAX)
                    break;
            }
        }
        if ((*it).isMember("rule_id"))
            strcpy(atomic_rule.rule_id.uuid, (*it)["rule_id"].asString().c_str());
        else
            return JSON_VA_INVALID;
        atomic_rule.cnt_zones++;
        if ((*it).isMember("status")){
            atomic_rule.status = (VARuleStatus)(*it)["status"].asInt();
            if (atomic_rule.status != 0 && atomic_rule.status != 1)
                return JSON_VA_INVALID;
        }
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("rule_name"))
            strcpy(atomic_rule.rule_name, (*it)["rule_name"].asString().c_str());
        else
            strcpy(atomic_rule.rule_name, default_atomic_rulenames[
                (int)atomic_rule.event_type].c_str());
        switch (atomic_rule.event_type){
        case METADATA_EVENT_CAMERA_TAMPER_DETECTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity")){
                    atomic_rule.attributes.camera_tamper_detected.sensitivity =
                        attributes["sensitivity"].asInt();
                    if (atomic_rule.attributes.camera_tamper_detected.
                        sensitivity<1 || atomic_rule.attributes.
                        camera_tamper_detected.sensitivity>100)
                        return JSON_VA_INVALID;
                }
                else
                    atomic_rule.attributes.camera_tamper_detected.sensitivity = 50;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid,
                    (*it)["zone_id"].asString().c_str());
                //atomic_rule.cnt_zones++;
            }
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_MOTION_DETECTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity")){
                    atomic_rule.attributes.motion_detected.sensitivity =
                        attributes["sensitivity"].asInt();
                    if (atomic_rule.attributes.motion_detected.sensitivity<1 ||
                        atomic_rule.attributes.motion_detected.sensitivity>100)
                        return JSON_VA_INVALID;
                }
                else
                    atomic_rule.attributes.motion_detected.sensitivity = 50;
                if (attributes.isMember("minimum_size")){
                    atomic_rule.attributes.motion_detected.minimum_size =
                        attributes["minimum_size"].asInt();
                    if (atomic_rule.attributes.motion_detected.minimum_size<1 ||
                        atomic_rule.attributes.motion_detected.minimum_size>100)
                        return JSON_VA_INVALID;
                }
                else
                    atomic_rule.attributes.motion_detected.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
                if (attributes.isMember("event_on_delay"))
                    atomic_rule.attributes.motion_detected.event_on_delay =
                    attributes["event_on_delay"].asInt();
                else
                    atomic_rule.attributes.motion_detected.event_on_delay = 300;
                if (attributes.isMember("event_off_delay"))
                    atomic_rule.attributes.motion_detected.event_off_delay =
                    attributes["event_off_delay"].asInt();
                else
                    atomic_rule.attributes.motion_detected.event_off_delay = 1000;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid,
                    (*it)["zone_id"].asString().c_str());
               // atomic_rule.cnt_zones++;
            }
            else
                return JSON_VA_INVALID;
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_OBJECT_DETECTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity")){
                    atomic_rule.attributes.object_detected.sensitivity =
                        attributes["sensitivity"].asInt();
                    if (atomic_rule.attributes.object_detected.sensitivity<1 ||
                        atomic_rule.attributes.object_detected.sensitivity>100)
                        return JSON_VA_INVALID;
                }
                else
                    atomic_rule.attributes.object_detected.sensitivity = 50;
                if (attributes.isMember("minimum_size")){
                    atomic_rule.attributes.object_detected.minimum_size =
                        attributes["minimum_size"].asInt();
                    if (atomic_rule.attributes.object_detected.minimum_size<1 ||
                        atomic_rule.attributes.object_detected.minimum_size>100)
                        return JSON_VA_INVALID;
                }
                else
                    atomic_rule.attributes.object_detected.minimum_size = 50;

                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid,
                    (*it)["zone_id"].asString().c_str());
               // atomic_rule.cnt_zones++;
            }
            else
                return JSON_VA_INVALID;
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_INTRUSION_DETECTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.intrusion_detected.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.intrusion_detected.sensitivity = 50;
                if (attributes.isMember("minimum_size"))
                    atomic_rule.attributes.intrusion_detected.minimum_size =
                    attributes["minimum_size"].asInt();
                else
                    atomic_rule.attributes.intrusion_detected.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid,
                    (*it)["zone_id"].asString().c_str());
            }
            else
                return JSON_VA_INVALID;
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_LINECROSSED:{
            atomic_rule.detector_type = VA_DETECTORTYPE_LINE;
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.line_crossed.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.line_crossed.sensitivity = 50;
                if (attributes.isMember("minimum_size"))
                    atomic_rule.attributes.line_crossed.minimum_size =
                    attributes["minimum_size"].asInt();
                else
                    atomic_rule.attributes.line_crossed.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
                if (attributes.isMember("direction"))
                    atomic_rule.attributes.line_crossed.direction =
                    (VADir)attributes["direction"].asInt();
                else
                    atomic_rule.attributes.line_crossed.direction = VA_DIR_ANY;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid,
                    (*it)["zone_id"].asString().c_str());
            }
            if ((*it).isMember("line_id")){
                strcpy(atomic_rule.line_id.uuid, (*it)["line_id"].asString().c_str());
            }
            else{
                atomic_rule.line_id.uuid[0] = 0;
                return JSON_VA_INVALID;
            }
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_OBJECT_COUNTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                atomic_rule.detector_type = VA_DETECTORTYPE_ZONE;
                atomic_rule.attributes.object_counted.detector_type = -1;

                if ((*it).isMember("zone_id")){
                    strcpy(atomic_rule.zone_id.uuid, (*it)["zone_id"].asString().c_str());
                    atomic_rule.attributes.object_counted.detector_type = 0;
                    atomic_rule.detector_type = VA_DETECTORTYPE_ZONE;
                }
                else

                    atomic_rule.zone_id.uuid[0] = 0;
                if ((*it).isMember("line_id")){
                    strcpy(atomic_rule.line_id.uuid, (*it)["line_id"].asString().c_str());
                    atomic_rule.attributes.object_counted.detector_type = 1;
                    atomic_rule.detector_type = VA_DETECTORTYPE_LINE;
                }
                else
                    atomic_rule.line_id.uuid[0] = 0;
                if (atomic_rule.attributes.object_counted.detector_type == -1)
                    return JSON_VA_INVALID;
                //line
                if (atomic_rule.attributes.object_counted.detector_type == 1){
                    if (attributes.isMember("direction"))
                        atomic_rule.attributes.object_counted.direction =
                        (VADir)attributes["direction"].asInt();
                    else
                        atomic_rule.attributes.object_counted.direction =
                        VA_DIR_ANY;
                }
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.object_counted.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.object_counted.sensitivity = 50;
                if (attributes.isMember("minimum_size"))
                    atomic_rule.attributes.object_counted.minimum_size =
                    attributes["minimum_size"].asInt();
                else
                    atomic_rule.attributes.object_counted.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
                if (attributes.isMember("report_time_interval"))
                    atomic_rule.attributes.object_counted.report_time_interval =
                    attributes["report_time_interval"].asInt();
                else
                    atomic_rule.attributes.object_counted.report_time_interval = 0;
                if (attributes.isMember("reset_time_interval"))
                    atomic_rule.attributes.object_counted.reset_time_interval =
                    attributes["reset_time_interval"].asInt();
                else
                    atomic_rule.attributes.object_counted.reset_time_interval = 0;
            }
            else
                return JSON_VA_INVALID;
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_LOITERING_DETECTED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.loitering_detected.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.loitering_detected.sensitivity = 50;
                if (attributes.isMember("minimum_size"))
                    atomic_rule.attributes.loitering_detected.minimum_size =
                    attributes["minimum_size"].asInt();
                else
                    atomic_rule.attributes.loitering_detected.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
                if (attributes.isMember("dwell_time"))
                    atomic_rule.attributes.loitering_detected.dwell_time =
                    attributes["dwell_time"].asInt();
                else
                    return JSON_VA_INVALID;
            }
            else
                return JSON_VA_INVALID;
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid, (*it)["zone_id"].asString().c_str());
            }
            else{
                atomic_rule.zone_id.uuid[0] = 0;
                return JSON_VA_INVALID;
            }
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_FACE_RECOGNIZED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.face_recognized.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.face_recognized.sensitivity = 50;
                if (attributes.isMember("minimum_size"))
                    atomic_rule.attributes.face_recognized.minimum_size =
                    attributes["minimum_size"].asInt();
                else
                    atomic_rule.attributes.face_recognized.minimum_size = 50;
                if (attributes.isMember("scene_type"))
                    atomic_rule.attributes.motion_detected.scene_type =
                    attributes["scene_type"].asString().c_str();
                else
                    atomic_rule.attributes.motion_detected.scene_type = 0;
                if (attributes.isMember("confidence"))
                    atomic_rule.attributes.face_recognized.confidence =
                    attributes["confidence"].asInt();
                else
                    atomic_rule.attributes.face_recognized.confidence = 80;
                atomic_rule.attributes.face_recognized.cnt_groups=0;
                if (attributes.isMember("groups")){
                    Json::Value groups = attributes["groups"];
                    Json::Value::iterator it_groups;
                    for (it_groups = groups.begin(); it_groups != groups.end();
                        ++it_groups){
                        VAUUID group;
                        group.uuid = (*it_groups).asString();
                        strcpy(atomic_rule.attributes.face_recognized.groups
                            [atomic_rule.attributes.face_recognized.cnt_groups].
                            uuid,group.uuid.c_str());
                        atomic_rule.attributes.face_recognized.cnt_groups++;
                    }
                }
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid, (*it)["zone_id"].asString().c_str());
            }
            else{
                atomic_rule.zone_id.uuid[0] = 0;
                return JSON_VA_INVALID;
            }
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        case METADATA_EVENT_OBJECT_CLASSIFIED:{
            if ((*it).isMember("attributes")){
                Json::Value attributes = (*it)["attributes"];
                if (attributes.isMember("sensitivity"))
                    atomic_rule.attributes.object_classified.sensitivity =
                    attributes["sensitivity"].asInt();
                else
                    atomic_rule.attributes.object_classified.sensitivity = 50;
                if (attributes.isMember("object_type")){
                    Json::Value object_type = attributes["object_type"];
                    Json::Value::iterator it_type;
                    atomic_rule.attributes.object_classified.cnt_object_types = 0;
                    for (it_type = object_type.begin();
                        it_type != object_type.end(); ++it_type){
                        atomic_rule.attributes.object_classified.
                            object_type[atomic_rule.attributes.
                            object_classified.cnt_object_types] =
                            (VACategory)(*it_type).asInt();
                        atomic_rule.attributes.
                            object_classified.cnt_object_types++;
                    }

                }
                else
                    return JSON_VA_INVALID;
            }
            if ((*it).isMember("zone_id")){
                strcpy(atomic_rule.zone_id.uuid, (*it)["zone_id"].asString().c_str());
            }
            else{
                atomic_rule.zone_id.uuid[0] = 0;
                return JSON_VA_INVALID;
            }
            configuration_content.atomic_rules.push_back(atomic_rule);
        }
            break;
        default:
            return JSON_VA_INVALID;

        }
    }
    return JSON_VA_OK;
#endif
#if (VA_CONFIG_VERSION==1)

    Json::Value::iterator it;
    for (it = atomic_rules.begin(); it != atomic_rules.end(); ++it){
        JSONVAAtomicRule atomic_rule;
        memset(atomic_rule.reserve, 0, sizeof(int64_t) * 5);
        for (int i = 0; i < 5; i++){
            atomic_rule.reserve_str[i][0] = 0;
        }
        atomic_rule.cnt_zones = 0;
        if ((*it).isMember("event_type")){
            atomic_rule.event_type = (JSONEventType)(*it)["event_type"].asInt();
            if (atomic_rule.event_type<0 || atomic_rule.event_type >16){
                printf("Failure: event_type is not valid for the atomic rule\n");
                return JSON_VA_INVALID;
            }
        }
        else{
            printf("Failure: event_type is not defined for the atomic rule\n");
            return JSON_VA_INVALID;
        }
        if ((*it).isMember("id"))
            strcpy(atomic_rule.id, (*it)["id"].asString().c_str());
        else{
            printf("id is not defined for the atomic rule\n");
            return JSON_VA_INVALID;
        }
        for (int k = 0; k < 5; k++){
            atomic_rule.reserve_str[k][0] = 0;
            atomic_rule.reserve[k] = 0;
        }
        if ((*it).isMember("reserve")){
            Json::Value reserve = (*it)["reserve"];
            Json::Value::iterator it_reserve;
            int k = 0;
            for (it_reserve = reserve.begin(); it_reserve != reserve.end(); ++it_reserve){
#if defined(JSON_HAS_INT64)
                atomic_rule.reserve[k] = (*it_reserve).asInt64();
#else
                atomic_rule.reserve[k] = (*it_reserve).asInt();
#endif
                k++;
                if (k == 5)
                    break;
            }
        }
        if ((*it).isMember("reserve_str")){
            Json::Value reserve_str = (*it)["reserve_str"];
            Json::Value::iterator it_reserveStr;
            int k = 0;
            for (it_reserveStr = reserve_str.begin(); it_reserveStr != reserve_str.end(); ++it_reserveStr){
                strcpy(atomic_rule.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
                if (k == 5)
                    break;
            }
        }
        if ((*it).isMember("zones")){
            Json::Value zones = (*it)["zones"];
            Json::Value::iterator it_zones;
            int k = 0;
            for (it_zones = zones.begin(); it_zones != zones.end(); ++it_zones){
                strcpy(atomic_rule.zone_ids[k], (*it_zones).asString().c_str());
                atomic_rule.cnt_zones++;
                if (atomic_rule.cnt_zones > 5)
                    atomic_rule.cnt_zones = 5;
                k++;
                if (k == 5)
                    break;
            }
        }
        else if (atomic_rule.event_type != 3 && atomic_rule.event_type != 5){
            strcpy(atomic_rule.zone_ids[0], "00000000-0000-0000-0000-000000000001");
            atomic_rule.cnt_zones++;
        }
        else{ //return invalid if the event_type is counting or line_crossing and there is no zone defined
            printf("There is no zone defined for counting or line_crossing\n");
            return JSON_VA_INVALID;
        }
       // atomic_rule.cnt_zones++;
        if ((*it).isMember("status")){
            atomic_rule.status = (VARuleStatus)(*it)["status"].asInt();
            if (atomic_rule.status != 0 && atomic_rule.status != 1)
                atomic_rule.status = 1;
        }
        else
            atomic_rule.status=1;
        if ((*it).isMember("name"))
            strcpy(atomic_rule.name, (*it)["name"].asString().c_str());
        else
            strcpy(atomic_rule.name, "");
        if ((*it).isMember("sensitivity"))
            atomic_rule.sensitivity = (*it)["sensitivity"].asInt();
        else
            atomic_rule.sensitivity = 50;
        if ((*it).isMember("min_size"))
            atomic_rule.min_size = (*it)["min_size"].asInt();
        else
            atomic_rule.min_size = 10;
        for (int k = 0; k < 5; k++){
            atomic_rule.reserve_str[k][0] = 0;
            atomic_rule.reserve[k] = 0;
        }
        if ((*it).isMember("reserve")){
            Json::Value reserve = (*it)["reserve"];
            Json::Value::iterator it_reserve;
            int k = 0;
            for (it_reserve = reserve.begin(); it_reserve != reserve.end(); ++it_reserve){
#if defined(JSON_HAS_INT64)
                atomic_rule.reserve[k] = (*it_reserve).asInt64();
#else
                atomic_rule.reserve[k] = (*it_reserve).asInt();
#endif
                k++;
                if (k == 5)
                    break;
            }
        }
        if ((*it).isMember("reserve_str")){
            Json::Value reserve_str = (*it)["reserve_str"];
            Json::Value::iterator it_reserveStr;
            int k = 0;
            for (it_reserveStr = reserve_str.begin(); it_reserveStr != reserve_str.end(); ++it_reserveStr){
                strcpy(atomic_rule.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
                if (k == 5)
                    break;
            }
        }
        configuration_content.atomic_rules.push_back(atomic_rule);
    }
    return JSON_VA_OK;
#endif
}
JSONVAStatus JSONConfigurationParser::ParseRules(const Json::Value rules,
    JSONConfigurationContent & configuration_content)
{
#if (VA_CONFIG_VERSION==0)
    Json::Value::iterator it;
    for (it = rules.begin(); it != rules.end(); ++it){
        JSONVARule rule;
        if ((*it).isMember("operator"))
            strcpy(rule.operation, (*it)["operator"].asString().c_str());
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("rule_id"))
            strcpy(rule.rule_id.uuid, (*it)["rule_id"].asString().c_str());
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("rule_name"))
            strcpy(rule.rule_name, (*it)["rule_name"].asString().c_str());
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("status"))
            rule.status = (VARuleStatus)(*it)["status"].asInt();
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("sub_rule_id1"))
            strcpy(rule.sub_rule_id1.uuid, (*it)["sub_rule_id1"].asString().c_str());
        else
            return JSON_VA_INVALID;
        if (strcmp(rule.operation, "AND")==0 || strcmp(rule.operation, "OR") == 0){
            if ((*it).isMember("sub_rule_id2"))
                strcpy(rule.sub_rule_id2.uuid, (*it)["sub_rule_id2"].asString().c_str());
            else
                return JSON_VA_INVALID;

            if ((*it).isMember("relationship")){
                Json::Value relationship = (*it)["relationship"];
                rule.relationship.relative_start_time =
                    relationship["relative_start_time"].asFloat();
                rule.relationship.relative_end_time =
                    relationship["relative_end_time"].asFloat();
            }
            else{
                rule.relationship.relative_start_time = 0;
                rule.relationship.relative_end_time = 0;
            }

        }
        configuration_content.rules.push_back(rule);
    }
    return JSON_VA_OK;
#endif
#if (VA_CONFIG_VERSION==1)
    Json::Value::iterator it;
    for (it = rules.begin(); it != rules.end(); ++it){
        JSONVARule rule;
        memset(rule.reserve, 0, sizeof(int64_t) * 5);
        for (int i = 0; i < 5; i++){
            rule.reserve_str[i][0] = 0;
        }
        if ((*it).isMember("operator"))
            rule.operation =  (*it)["operator"].asInt();
        else
            rule.operation = 0;
        if ((*it).isMember("id"))
            strcpy(rule.id, (*it)["id"].asString().c_str());
        else{
            printf("id is not defined for the compound rule\n");
            return JSON_VA_INVALID;
        }
        if ((*it).isMember("name"))
            strcpy(rule.name, (*it)["name"].asString().c_str());
        else{
            strcpy(rule.name, "");
        }
        if ((*it).isMember("status")){
            rule.status = (VARuleStatus)(*it)["status"].asInt();
            if (rule.status != 0 && rule.status != 1)
                rule.status = 1;
        }
        else
            rule.status = 1;
        if ((*it).isMember("sub_rule_id1"))
            strcpy(rule.sub_rule_id1, (*it)["sub_rule_id1"].asString().c_str());
        else{
            printf("sub_rule_id1 is not defined for the compound rule\n");
            return JSON_VA_INVALID;
        }
        strcpy(rule.sub_rule_id2, "");
        if (rule.operation < 2){
            if ((*it).isMember("sub_rule_id2"))
                strcpy(rule.sub_rule_id2, (*it)["sub_rule_id2"].asString().c_str());
            else{
                printf("sub_rule_id2 is not defined for the compound rule\n");
                return JSON_VA_INVALID;
            }
        }
        if ((*it).isMember("relationship")){
            Json::Value relationship = (*it)["relationship"];
            rule.relationship.relative_start_time =
                relationship["relative_start_time"].asFloat();
            rule.relationship.relative_end_time =
                relationship["relative_end_time"].asFloat();
        }
        else{
            rule.relationship.relative_start_time = 0;
            rule.relationship.relative_end_time = 0;
        }

        for (int k = 0; k < 5; k++){
            rule.reserve_str[k][0] = 0;
            rule.reserve[k] = 0;
        }
        if ((*it).isMember("reserve")){
            Json::Value reserve = (*it)["reserve"];
            Json::Value::iterator it_reserve;
            int k = 0;
            for (it_reserve = reserve.begin(); it_reserve != reserve.end(); ++it_reserve,++k){
#if defined(JSON_HAS_INT64)
                rule.reserve[k] = (*it_reserve).asInt64();
#else
                rule.reserve[k] = (*it_reserve).asInt();
#endif
                if (k == 5)
                    break;
            }
        }
        if ((*it).isMember("reserve_str")){
            Json::Value reserve_str = (*it)["reserve_str"];
            Json::Value::iterator it_reserveStr;
            int k = 0;
            for (it_reserveStr = reserve_str.begin(); it_reserveStr != reserve_str.end(); ++it_reserveStr){
                strcpy(rule.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
                if (k == 5)
                    break;
            }
        }
        configuration_content.rules.push_back(rule);
    }
    return JSON_VA_OK;
#endif
}

#if (VA_CONFIG_VERSION==0)
JSONVAStatus JSONConfigurationParser::ParseExtension(const Json::Value extensions,
    JSONConfigurationContent & configuration_content)
{

    Json::Value::iterator it;
    for (it = extensions.begin(); it != extensions.end(); ++it){
        JSONVAConfigExtension extension;
        if ((*it).isMember("engine_id"))
            strcpy(extension.engine_id, (*it)["engine_id"].asString().c_str());
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("data")){
            extension.data = new char[(*it)["data"].asString().length()+1];
            strcpy(extension.data, (*it)["data"].asString().c_str());
            extension.size_data = strlen(extension.data);
        }
        else
            return JSON_VA_INVALID;
        configuration_content.extension.push_back(extension);
    }
    return JSON_VA_OK;
}
#endif
