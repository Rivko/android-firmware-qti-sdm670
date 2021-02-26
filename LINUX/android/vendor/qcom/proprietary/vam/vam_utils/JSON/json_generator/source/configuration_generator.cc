/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cstddef>
#include "configuration_generator.h"
extern "C"{
#include "json_configuration.h"
}
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

using namespace std;


ConfigurationGenerator::ConfigurationGenerator()
{
#if (VA_CONFIG_VERSION==0)
    for (int i = 0; i < EXTENSION_MAX; i++){
        configuration_.extension[i].data = 0;
    }
    configuration_.cnt_extension = 0;
    configuration_.cnt_lines = 0;
#endif
#if (VA_CONFIG_VERSION==1)
    strcpy(configuration_.version, "1.0");
    configuration_.time_stamp = 0;
    memset(configuration_.reserve, 0, sizeof(int64_t) * 5);
    for (int i = 0; i < 5; i++){
        configuration_.reserve_str[i][0] = 0;
    }
    time_stamp_ = 0;
    memset(reserve_, 0, sizeof(int64_t) * 5);
    for (int i = 0; i < 5; i++){
        reserveStr_[i][0] = 0;
    }
#endif
    configuration_json_ = Json::Value(Json::objectValue);
    configuration_.cnt_atomic_rules = 0;
    configuration_.cnt_rules = 0;
    configuration_.cnt_zones = 0;
    configuration_.scene_calibration.cnt_objects = 0;
    configuration_.scene_calibration.length_unit = VA_UNIT_MM;
}
ConfigurationGenerator::~ConfigurationGenerator()
{
#if (VA_CONFIG_VERSION==0)
    for (int i = 0; i < extensions_.size(); i++){
        if (extensions_[i].data)
            delete []extensions_[i].data;
    }
    for (int i = 0; i < EXTENSION_MAX; i++){
        if (configuration_.extension[i].data&&configuration_.extension[i].data!=(char*)0xcccccccccccccccc){
            delete []configuration_.extension[i].data;
            configuration_.extension[i].data = 0;
        }
    }
#endif
}
JSONVAStatus ConfigurationGenerator::LoadFileToString(const char *filename,
    std::string &contents)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in){
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}

JSONVAStatus ConfigurationGenerator::Clear()
{
    zones_.clear();
    atomic_rules_.clear();
    rules_.clear();
    scene_calibration_.cnt_objects=0;
#if (VA_CONFIG_VERSION==0)
    lines_.clear();
     for (int i = 0; i < extensions_.size(); i++){
        if (extensions_[i].data){
            delete[]extensions_[i].data;
            extensions_[i].data = 0;
        }
    }
    extensions_.clear();
#endif
    return JSON_VA_OK;
}

JSONVAStatus ConfigurationGenerator::SetSceneCalibration(JSONVASceneCalibration * scene_calibration)
{
    scene_calibration_.cnt_objects = scene_calibration->cnt_objects;
    scene_calibration_.length_unit = scene_calibration->length_unit;
    memcpy(scene_calibration_.objects, scene_calibration->objects,
        sizeof(CalibObject)*scene_calibration->cnt_objects);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::GetSceneCalibration(JSONVASceneCalibration * scene_calibration)
{
    scene_calibration->cnt_objects = scene_calibration_.cnt_objects;
    memcpy(scene_calibration->objects, scene_calibration_.objects, sizeof(CalibObject)*scene_calibration->cnt_objects);
    scene_calibration->length_unit = scene_calibration_.length_unit;
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::AddZone(JSONVAZone zone)
{
    zones_.push_back(zone);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::AddAtomicRule(JSONVAAtomicRule atomic_rule)
{
    atomic_rules_.push_back(atomic_rule);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::AddRule(JSONVARule rule)
{
    rules_.push_back(rule);
    return JSON_VA_OK;
}

#if (VA_CONFIG_VERSION==0)
JSONVAStatus ConfigurationGenerator::SetGeneralConfiguration(JSONGeneralConfiguration
    general_configuration)
{
    strcpy(general_configuration_.configuration_id.uuid, general_configuration.configuration_id.uuid);
    general_configuration_.scene_type = general_configuration.scene_type;
    general_configuration_.time_stamp = general_configuration.time_stamp;
    strcpy(general_configuration_.version, general_configuration.version);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::GetGeneralConfiguration(JSONGeneralConfiguration
    &general_configuration)
{
    strcpy(general_configuration.configuration_id.uuid, general_configuration_.configuration_id.uuid);
    general_configuration.scene_type = general_configuration_.scene_type;
    general_configuration.time_stamp = general_configuration_.time_stamp;
    strcpy(general_configuration.version, general_configuration_.version);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::AddLine(JSONVALine line)
{
    lines_.push_back(line);
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::AddExtension(JSONVAConfigExtension extension)
{
    JSONVAConfigExtension extension0;
    extension0.size_data = extension.size_data;
    extension0.data = new char[extension.size_data+1];
    memcpy(extension0.data, extension.data, extension.size_data);
    extension0.data[extension0.size_data] = 0;
    strcpy(extension0.engine_id, extension.engine_id);
    extensions_.push_back(extension0);
    return JSON_VA_OK;
}

#endif
JSONVAStatus ConfigurationGenerator::GetConfiguration(struct JSONConfiguration **configuration)
{
#if (VA_CONFIG_VERSION==1)
    *configuration = &configuration_;
    (*configuration)->time_stamp = time_stamp_;
    strcpy((*configuration)->version, version_);
    for (int i = 0; i < 5; i++)
    {
        (*configuration)->reserve[i] = reserve_[i];
        strcpy((*configuration)->reserve_str[i], reserveStr_[i]);
    }
    GetSceneCalibration(&((*configuration)->scene_calibration));
    (*configuration)->cnt_zones = zones_.size();
    (*configuration)->cnt_rules = rules_.size();
    (*configuration)->cnt_atomic_rules = atomic_rules_.size();
    for (uint32_t i = 0; i < (*configuration)->cnt_zones; i++){
        strcpy((*configuration)->zones[i].id,
            zones_[i].id);
        (*configuration)->zones[i].cnt_points = zones_[i].cnt_points;
        memcpy((*configuration)->zones[i].points, zones_[i].points,
            sizeof(VAPoint)*zones_[i].cnt_points);
        (*configuration)->zones[i].type = zones_[i].type;
    }

    for (uint32_t i = 0; i < (*configuration)->cnt_atomic_rules; i++){
        memcpy((*configuration)->atomic_rules[i].reserve, atomic_rules_[i].reserve, sizeof(int64_t)*5);
        for (int k = 0; k < 5; k++){
            strcpy((*configuration)->atomic_rules[i].reserve_str[k], atomic_rules_[i].reserve_str[k]);
        }
        (*configuration)->atomic_rules[i].event_type = atomic_rules_[i].event_type;
        (*configuration)->atomic_rules[i].cnt_zones = atomic_rules_[i].cnt_zones;
        for (uint32_t k = 0; k < atomic_rules_[i].cnt_zones; k++)
        {
            strcpy((*configuration)->atomic_rules[i].zone_ids[k], atomic_rules_[i].zone_ids[k]);
        }
        strcpy((*configuration)->atomic_rules[i].id, atomic_rules_[i].id);
        strcpy((*configuration)->atomic_rules[i].name, atomic_rules_[i].name);
        (*configuration)->atomic_rules[i].status = atomic_rules_[i].status;
    }

    for (uint32_t i = 0; i < (*configuration)->cnt_rules; i++){
        (*configuration)->rules[i].operation= rules_[i].operation;
        strcpy((*configuration)->rules[i].id, rules_[i].id);
        strcpy((*configuration)->rules[i].name, rules_[i].name);
        strcpy((*configuration)->rules[i].sub_rule_id1, rules_[i].sub_rule_id1);
        (*configuration)->rules[i].relationship = rules_[i].relationship;
        (*configuration)->rules[i].status = rules_[i].status;
        if ((*configuration)->rules[i].operation!=2){
            strcpy((*configuration)->rules[i].sub_rule_id2, rules_[i].sub_rule_id2);
        }
    }
#endif
#if (VA_CONFIG_VERSION==0)
for (int i = 0; i < EXTENSION_MAX; i++){
        if (configuration_.extension[i].data){
            delete[]configuration_.extension[i].data;
            configuration_.extension[i].data = 0;
        }
    }

    *configuration = &configuration_;
    GetGeneralConfiguration((*configuration)->general_configuration);
    GetSceneCalibration(&((*configuration)->scene_calibration));
    (*configuration)->cnt_zones = zones_.size();
    (*configuration)->cnt_lines = lines_.size();
    (*configuration)->cnt_rules = rules_.size();
    (*configuration)->cnt_atomic_rules = atomic_rules_.size();
    (*configuration)->cnt_extension = extensions_.size();
    for (uint32_t i = 0; i < (*configuration)->cnt_zones; i++){
        strcpy((*configuration)->zones[i].zone_id.uuid,
            zones_[i].zone_id.uuid);
        (*configuration)->zones[i].cnt_points = zones_[i].cnt_points;
        memcpy((*configuration)->zones[i].points, zones_[i].points,
            sizeof(VAPoint)*zones_[i].cnt_points);
        (*configuration)->zones[i].zone_type = zones_[i].zone_type;
    }
    for (uint32_t i = 0; i < (*configuration)->cnt_lines; i++){
        strcpy((*configuration)->lines[i].line_id.uuid,
            lines_[i].line_id.uuid);
        (*configuration)->lines[i].cnt_points = lines_[i].cnt_points;
        memcpy((*configuration)->lines[i].points, lines_[i].points,
            sizeof(VAPoint)*lines_[i].cnt_points);
    }

    for (uint32_t i = 0; i < (*configuration)->cnt_atomic_rules; i++){
        memcpy(&(*configuration)->atomic_rules[i].attributes,&atomic_rules_[i].attributes,sizeof(union AttributesOfAtomicRule));
        (*configuration)->atomic_rules[i].event_type = atomic_rules_[i].event_type;
        strcpy((*configuration)->atomic_rules[i].line_id.uuid, atomic_rules_[i].line_id.uuid);
        strcpy((*configuration)->atomic_rules[i].zone_id.uuid, atomic_rules_[i].zone_id.uuid);
        strcpy((*configuration)->atomic_rules[i].rule_id.uuid, atomic_rules_[i].rule_id.uuid);
        strcpy((*configuration)->atomic_rules[i].rule_name, atomic_rules_[i].rule_name);
        (*configuration)->atomic_rules[i].status = atomic_rules_[i].status;
    }

    for (uint32_t i = 0; i < (*configuration)->cnt_rules; i++){
        strcpy((*configuration)->rules[i].operation, rules_[i].operation);
        strcpy((*configuration)->rules[i].rule_id.uuid, rules_[i].rule_id.uuid);
        strcpy((*configuration)->rules[i].rule_name, rules_[i].rule_name);
        strcpy((*configuration)->rules[i].sub_rule_id1.uuid, rules_[i].sub_rule_id1.uuid);
        (*configuration)->rules[i].relationship = rules_[i].relationship;
        (*configuration)->rules[i].status = rules_[i].status;
        if (strcmp((*configuration)->rules[i].operation, "NOT")){
            strcpy((*configuration)->rules[i].sub_rule_id2.uuid, rules_[i].sub_rule_id2.uuid);
        }
    }
    for (uint32_t i = 0; i < (*configuration)->cnt_extension; i++){
        (*configuration)->extension[i].size_data = extensions_[i].size_data;
        (*configuration)->extension[i].data = new char[extensions_[i].size_data+1];
        memcpy((*configuration)->extension[i].data, extensions_[i].data, extensions_[i].size_data);
        (*configuration)->extension[i].data[extensions_[i].size_data] = 0;
        strcpy((*configuration)->extension[i].engine_id, extensions_[i].engine_id);
    }
#endif
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::ToJSON(Json::Value &node)
{
    ToJSON();
    node = configuration_json_;
    return JSON_VA_OK;
}

JSONVAStatus ConfigurationGenerator::ToJSON()
{
#if (VA_CONFIG_VERSION==0)
    Json::Value root(Json::objectValue);
    Json::Value general_configuration(Json::objectValue);
    Json::Value scene_calibration(Json::objectValue);
    Json::Value rule_configuration(Json::objectValue);
    Json::Value zones(Json::arrayValue);
    Json::Value lines(Json::arrayValue);
    Json::Value atomic_rules(Json::arrayValue);
    Json::Value rules(Json::arrayValue);
    //general_configuration
    general_configuration["version"] = general_configuration_.version;
    general_configuration["configuration_id"] = general_configuration_.
        configuration_id.uuid;
    general_configuration["scene_type"] = general_configuration_.scene_type;
    general_configuration["time_stamp"] = general_configuration_.time_stamp;
    root["general_configuration"] = general_configuration;
//scene_calibration
    int flag_rules = 0;
    if (scene_calibration_.cnt_objects > 0){
        flag_rules++;
        Json::Value calibration_json(Json::objectValue);
        Json::Value objects_json(Json::arrayValue);

        for (uint32_t i = 0; i < scene_calibration_.cnt_objects; i++) {
            Json::Value value(Json::objectValue);
            value["x"] = scene_calibration_.objects[i].x;
            value["y"] = scene_calibration_.objects[i].y;
            value["width"] = scene_calibration_.objects[i].width;
            value["height"] = scene_calibration_.objects[i].height;
            value["physical_height"] = scene_calibration_.objects[i].physical_height;
            objects_json.append(value);
        }
        scene_calibration["length_unit"] = scene_calibration_.length_unit;
        scene_calibration["objects"] = objects_json;
        root["scene_calibration"] = scene_calibration;
    }
//rule_configuration
    if (zones_.size() > 0){
        flag_rules++;
        for (int i = 0; i < (int)zones_.size(); i++){
            Json::Value zone(Json::objectValue);
            zone["zone_id"] = zones_[i].zone_id.uuid;
            zone["zone_type"] = (int)zones_[i].zone_type;
            for (int k = 0; k < (int)zones_[i].cnt_points; k++){
                Json::Value point(Json::objectValue);
                point["x"] = zones_[i].points[k].x;
                point["y"] = zones_[i].points[k].y;
                zone["points"].append(point);
            }
            zones.append(zone);
        }
        rule_configuration["zones"] = zones;
    }
    if (lines_.size() > 0){
        flag_rules++;
        for (int i = 0; i < (int)lines_.size(); i++){
            Json::Value line(Json::objectValue);
            line["line_id"] = lines_[i].line_id.uuid;
            for (int k = 0; k < (int)lines_[i].cnt_points; k++){
                Json::Value point(Json::objectValue);
                point["x"] = lines_[i].points[k].x;
                point["y"] = lines_[i].points[k].y;
                line["points"].append(point);
            }
            lines.append(line);
        }
        rule_configuration["lines"] = lines;
    }
    if (atomic_rules_.size()>0){
        flag_rules++;
        for (int i = 0; i < (int)atomic_rules_.size(); i++){
            Json::Value atomic_rule(Json::objectValue);
            atomic_rule["rule_id"] = atomic_rules_[i].rule_id.uuid;
            atomic_rule["rule_name"] = atomic_rules_[i].rule_name;
            atomic_rule["event_type"] = atomic_rules_[i].event_type;
            atomic_rule["status"] = (int)atomic_rules_[i].status;
            Json::Value attributes(Json::objectValue);
            switch (atomic_rules_[i].event_type){
            case METADATA_EVENT_CAMERA_TAMPER_DETECTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    camera_tamper_detected.sensitivity;
                break;
            case METADATA_EVENT_MOTION_DETECTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    motion_detected.sensitivity;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    motion_detected.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                attributes["event_on_delay"] = atomic_rules_[i].attributes.
                    motion_detected.event_on_delay;
                attributes["event_off_delay"] = atomic_rules_[i].attributes.
                    motion_detected.event_off_delay;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                break;
            case METADATA_EVENT_INTRUSION_DETECTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    intrusion_detected.sensitivity;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    intrusion_detected.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                break;
            case METADATA_EVENT_LINECROSSED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    line_crossed.sensitivity;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    line_crossed.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                attributes["direction"] = (int)atomic_rules_[i].attributes.
                    line_crossed.direction;
                atomic_rule["line_id"] = atomic_rules_[i].line_id.uuid;
                break;
            case METADATA_EVENT_OBJECT_COUNTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    object_counted.sensitivity;
                attributes["minimum_size"] = (int)atomic_rules_[i].attributes.
                    object_counted.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                attributes["report_time_interval"] = (int)atomic_rules_[i].
                    attributes.object_counted.report_time_interval;
                attributes["reset_time_interval"] = (int)atomic_rules_[i].
                    attributes.object_counted.reset_time_interval;
                //zone
                if (atomic_rules_[i].attributes.object_counted.
                    detector_type == 0){
                    atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                }
                //line
                else if (atomic_rules_[i].attributes.object_counted.
                    detector_type == 1){
                    atomic_rule["line_id"] = atomic_rules_[i].line_id.uuid;
                    attributes["direction"] = (int)atomic_rules_[i].attributes.
                        object_counted.direction;
                }
                else
                    return JSON_VA_INVALID;
                break;
            case METADATA_EVENT_LOITERING_DETECTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    loitering_detected.sensitivity;
                attributes["dwell_time"] = atomic_rules_[i].attributes.
                    loitering_detected.dwell_time;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    loitering_detected.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                break;
            case METADATA_EVENT_FACE_RECOGNIZED:
                attributes["confidence"] = atomic_rules_[i].attributes.
                    face_recognized.confidence;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    face_recognized.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    face_recognized.sensitivity;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                if (atomic_rules_[i].attributes.face_recognized.cnt_groups > 0){

                    Json::Value groups(Json::arrayValue);
                    for (int k = 0; k < (int)atomic_rules_[i].attributes.
                        face_recognized.cnt_groups; k++){
                        groups.append(atomic_rules_[i].attributes.
                            face_recognized.groups[k].uuid);
                    }
                    attributes["groups"] = groups;
                }
                break;
            case METADATA_EVENT_OBJECT_CLASSIFIED:{
                Json::Value object_type(Json::arrayValue);
                for (int k = 0; k < (int)atomic_rules_[i].attributes.
                    object_classified.cnt_object_types; k++){
                    object_type.append((int)atomic_rules_[i].attributes.
                        object_classified.object_type[k]);
                }
                attributes["object_type"] = object_type;
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    object_classified.sensitivity;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
            }
                break;
            case METADATA_EVENT_OBJECT_DETECTED:
                attributes["sensitivity"] = atomic_rules_[i].attributes.
                    object_detected.sensitivity;
                attributes["minimum_size"] = atomic_rules_[i].attributes.
                    object_detected.minimum_size;
                attributes["scene_type"] = atomic_rules_[i].attributes.
                    motion_detected.scene_type;
                atomic_rule["zone_id"] = atomic_rules_[i].zone_id.uuid;
                break;
            default:
                break;
            }
            if (atomic_rules_[i].cnt_zones > 1){
                Json::Value exclude_zone_ids(Json::arrayValue);
                for (int k = 0; k < atomic_rules_[i].cnt_zones - 1; k++){
                    exclude_zone_ids.append(atomic_rules_[i].exclude_zone_ids[k].uuid);
                }
                atomic_rule["exclude_zone_ids"] = exclude_zone_ids;
            }
            atomic_rule["attributes"] = attributes;
            atomic_rules.append(atomic_rule);
        }
        rule_configuration["atomic_rules"] = atomic_rules;
    } // (atomic_rules_.size()>0)

    if (rules_.size()>0){
        flag_rules++;
        for (int i = 0; i < (int)rules_.size(); i++){
            Json::Value rule(Json::objectValue);
            rule["rule_id"] = rules_[i].rule_id.uuid;
            rule["rule_name"] = rules_[i].rule_name;
            rule["status"] = (int)rules_[i].status;
            rule["operator"] = rules_[i].operation;
            rule["sub_rule_id1"] = rules_[i].sub_rule_id1.uuid;
            if (strcmp(rules_[i].operation , "AND" )==0|| strcmp(rules_[i].operation, "OR")==0){
                rule["sub_rule_id2"] = rules_[i].sub_rule_id2.uuid;
                if (rules_[i].relationship.relative_start_time != 0 ||
                    rules_[i].relationship.relative_end_time != 0){
                    Json::Value relationship(Json::objectValue);
                    relationship["relative_start_time"] =
                        rules_[i].relationship.relative_start_time;
                    relationship["relative_end_time"] =
                        rules_[i].relationship.relative_end_time;
                    rule["relationship"] = relationship;
                }
            }
            rules.append(rule);
        }
        rule_configuration["rules"] = rules;
    }
    if (flag_rules > 0){
        root["rule_configuration"] = rule_configuration;
    }
    if (extensions_.size()>0){
        Json::Value extensions(Json::arrayValue);
        for (int i = 0; i < (int)extensions_.size(); i++){
            Json::Value extension(Json::objectValue);
            extension["engine_id"] = extensions_[i].engine_id;
            extension["data"] = extensions_[i].data;
            extensions.append(extension);
        }
        root["extension"] = extensions;
    }
    configuration_json_ = root;
    return JSON_VA_OK;
#endif
#if (VA_CONFIG_VERSION==1)
    Json::Value root(Json::objectValue);
    //Json::Value general_configuration(Json::objectValue);
    Json::Value scene_calibration(Json::objectValue);
    Json::Value rule_configuration(Json::objectValue);
    Json::Value zones(Json::arrayValue);
    Json::Value lines(Json::arrayValue);
    Json::Value atomic_rules(Json::arrayValue);
    Json::Value rules(Json::arrayValue);
    root["version"] = version_;
    root["id"] = id_;
    root["time_stamp"] = time_stamp_;
    Json::Value reserve_json(Json::arrayValue);
    Json::Value reserveStr_json(Json::arrayValue);
    for (int i = 0; i < 5; i++) {
        Json::Value value(Json::objectValue);
        value = reserve_[i];
        reserve_json.append(value);
    }
    root["reserve"] = reserve_json;
    for (int i = 0; i < 5; i++) {
        Json::Value value(Json::objectValue);
        value = reserveStr_[i];
        reserveStr_json.append(value);
    }
    root["reserve_str"] = reserveStr_json;
//scene_calibration
    int flag_rules = 0;
    if (scene_calibration_.cnt_objects > 0){
        flag_rules++;
        Json::Value calibration_json(Json::objectValue);
        Json::Value objects_json(Json::arrayValue);
        for (uint32_t i = 0; i < scene_calibration_.cnt_objects; i++) {
            Json::Value value(Json::objectValue);
            value["x"] = scene_calibration_.objects[i].x;
            value["y"] = scene_calibration_.objects[i].y;
            value["width"] = scene_calibration_.objects[i].width;
            value["height"] = scene_calibration_.objects[i].height;
            value["physical_height"] = scene_calibration_.objects[i].physical_height;
            objects_json.append(value);
        }
        scene_calibration["length_unit"] = scene_calibration_.length_unit;
        scene_calibration["objects"] = objects_json;
        root["scene_calibration"] = scene_calibration;
    }
//rule_configuration
    if (zones_.size() > 0){
        flag_rules++;
        for (int i = 0; i < (int)zones_.size(); i++){
            Json::Value zone(Json::objectValue);
            zone["id"] = zones_[i].id;
            zone["type"] = (int)zones_[i].type;
            for (uint32_t k = 0; k < zones_[i].cnt_points; k++){
                Json::Value point(Json::objectValue);
                point["x"] = zones_[i].points[k].x;
                point["y"] = zones_[i].points[k].y;
                zone["points"].append(point);
            }
            zones.append(zone);
        }
        root["zones"] = zones;
    }
    if (atomic_rules_.size()>0){
        flag_rules++;
        for (int i = 0; i < (int)atomic_rules_.size(); i++){
            Json::Value atomic_rule(Json::objectValue);
            atomic_rule["id"] = atomic_rules_[i].id;
            atomic_rule["name"] = atomic_rules_[i].name;
            atomic_rule["event_type"] = atomic_rules_[i].event_type;
            atomic_rule["status"] = (int)atomic_rules_[i].status;
            atomic_rule["sensitivity"] = (int)atomic_rules_[i].sensitivity;
            atomic_rule["min_size"] = (int)atomic_rules_[i].min_size;
            Json::Value reserve_json(Json::arrayValue);
            Json::Value reserveStr_json(Json::arrayValue);
            for (int k = 0; k < 5; k++) {
                Json::Value value(Json::objectValue);
                value = atomic_rules_[i].reserve[k];
                reserve_json.append(value);
            }
            atomic_rule["reserve"] = reserve_json;
            for (int k = 0; k < 5; k++) {
                Json::Value value(Json::objectValue);
                value = atomic_rules_[i].reserve_str[k];
                reserveStr_json.append(value);
            }
            atomic_rule["reserve_str"] = reserveStr_json;
            if (atomic_rules_[i].cnt_zones > 0){
                Json::Value zone_ids(Json::arrayValue);
                for (uint32_t k = 0; k < atomic_rules_[i].cnt_zones; k++){
                    zone_ids.append(atomic_rules_[i].zone_ids[k]);
                }
                atomic_rule["zones"] = zone_ids;
            }
            atomic_rules.append(atomic_rule);
        }
        root["atomic_rules"] = atomic_rules;
    } // (atomic_rules_.size()>0)
    if (rules_.size()>0){
        flag_rules++;
        for (int i = 0; i < (int)rules_.size(); i++){
            Json::Value rule(Json::objectValue);
            rule["id"] = rules_[i].id;
            rule["name"] = rules_[i].name;
            rule["status"] = (int)rules_[i].status;
            Json::Value reserve_json(Json::arrayValue);
            Json::Value reserveStr_json(Json::arrayValue);
            for (int k = 0; k < 5; k++) {
                Json::Value value(Json::objectValue);
                value = rules_[i].reserve[k];
                reserve_json.append(value);
            }
            rule["reserve"] = reserve_json;
            for (int k = 0; k < 5; k++) {
                Json::Value value(Json::objectValue);
                value = rules_[i].reserve_str[k];
                reserveStr_json.append(value);
            }
            rule["reserve_str"] = reserveStr_json;
            rule["operator"] = rules_[i].operation;
            rule["sub_rule_id1"] = rules_[i].sub_rule_id1;
            if (rules_[i].operation ==0|| rules_[i].operation==1){
                rule["sub_rule_id2"] = rules_[i].sub_rule_id2;
                if (rules_[i].relationship.relative_start_time != 0 ||
                    rules_[i].relationship.relative_end_time != 0){
                    Json::Value relationship(Json::objectValue);
                    relationship["relative_start_time"] =
                        rules_[i].relationship.relative_start_time;
                    relationship["relative_end_time"] =
                        rules_[i].relationship.relative_end_time;
                    rule["relationship"] = relationship;
                }
            }
            rules.append(rule);
        }
        root["rules"] = rules;
    }
    configuration_json_ = root;
    return JSON_VA_OK;
#endif
}

JSONVAStatus ConfigurationGenerator::Serialize(std::string &doc)
{
    ToJSON();
    doc_ = configuration_json_.toStyledString();
    doc = doc_;
    //Json::FastWriter writer;
    //doc = writer.write(configuration_json_);
//	doc = configuration_json_.toStyledString();
    return JSON_VA_OK;
}
JSONVAStatus ConfigurationGenerator::Serialize(Json::Value node, std::string &doc)
{
    doc = node.toStyledString();
    return JSON_VA_OK;
}
