/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "metadata_generator.h"
#include <vector>
#include <string.h>
#include "metadata_types.h"
#include "json_common_structs.h"
using namespace std;

MetadataGenerator::MetadataGenerator()
    : fp_json_(0),
      fp_idx_(0),
      doc_str_(0),
      frame_cnt(0),
      start_time_stamp_(0),
      last_time_stamp_(0),
      time_interval_(33)
{
    AllocateMetadataContent(&metadata_content_);
    metadata_content_.cnt_atomic_events = 0;
    metadata_content_.cnt_events = 0;
    metadata_content_.cnt_extensions = 0;
    metadata_content_.cnt_heat_maps = 0;
    metadata_content_.cnt_objects = 0;
    metadata_content_.cnt_object_trees = 0;
}

MetadataGenerator::~MetadataGenerator()
{
    ReleaseMetadataContent(&metadata_content_);
    if (doc_str_)
        free(doc_str_);
}

JSONVAStatus MetadataGenerator::Clear()
{
    objects_.clear();
    object_trees_.clear();
    events_.clear();
    if (heat_map_.size() > 0)
        for (int i = 0; i < (int)heat_map_.size(); i++){
            delete[]heat_map_[i].data;
            heat_map_[i].data = NULL;;
        }
    heat_map_.clear();
    metadata_content_.cnt_atomic_events = 0;
    metadata_content_.cnt_events = 0;
    metadata_content_.cnt_extensions = 0;
    metadata_content_.cnt_heat_maps = 0;
    metadata_content_.cnt_objects = 0;
    metadata_content_.cnt_object_trees = 0;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::SetTimeStamp(uint64_t time_stamp)
{
    time_stamp_ = time_stamp;
    metadata_content_.time_stamp = time_stamp;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::GetTimeStamp(uint64_t &time_stamp)
{
    time_stamp = time_stamp_;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::SetTimeInterval(uint64_t interval){
    time_interval_ = interval;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::SetCameraID(VAUUID camera_id)
{
    camera_id_.uuid = camera_id.uuid;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::GetCameraID(VAUUID &camera_id)
{
    camera_id.uuid = camera_id_.uuid;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddObject(VAObject object)
{
    objects_.push_back(object);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddObjectTree(VAObjectTree object_tree)
{
    object_trees_.push_back(object_tree);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddAtomicEvent(VAAtomicEvent atomic_event)
{
    atomic_events_.push_back(atomic_event);
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AddEvent(VAEvent event)
{
    events_.push_back(event);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddHeatMap(VAHeatMap heat_map)
{
    heat_map_.push_back(heat_map);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddExtension(VAExtension extension)
{

    extensions_.push_back(extension);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::SetCameraID(struct metadata_uuid_t camera_id)
{
    strcpy(metadata_content_.camera_id.uuid, camera_id.uuid);
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::GetCameraID(struct metadata_uuid_t *camera_id)
{
    strcpy((*camera_id).uuid, metadata_content_.camera_id.uuid);
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AddObject(metadata_object_t object)
{
    if (metadata_content_.cnt_objects > MD_OBJECT_MAX)
        return JSON_VA_INVALID;
    metadata_content_.objects[metadata_content_.cnt_objects].object_id =
        object.object_id;
    strcpy(metadata_content_.objects[metadata_content_.cnt_objects].
        engine_type, object.engine_type);
    metadata_content_.objects[metadata_content_.cnt_objects].
        appearance_descriptor = object.appearance_descriptor;
    metadata_content_.objects[metadata_content_.cnt_objects].on_event.status =
        object.on_event.status;
    metadata_content_.objects[metadata_content_.cnt_objects].
        on_event.num_events = object.on_event.num_events;
    for (int k = 0; k < 5; k++){
        metadata_content_.objects[metadata_content_.cnt_objects].reserve[k] = object.reserve[k];
        strcpy(metadata_content_.objects[metadata_content_.cnt_objects].reserve_str[k], object.reserve_str[k]);
    }
    if (object.on_event.status){
        for (uint32_t i = 0; i < object.on_event.num_events; i++)
            strcpy(metadata_content_.objects[metadata_content_.cnt_objects].
            on_event.event_ids[i].uuid, object.on_event.event_ids[i].uuid);
    }
    metadata_content_.cnt_objects++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddObjectTree(metadata_object_tree_t object_tree)
{
    if (metadata_content_.cnt_object_trees >= MD_OBJECT_TREE_MAX)
        return JSON_VA_INVALID;
    metadata_content_.object_trees[metadata_content_.cnt_object_trees].action = object_tree.action;
    metadata_content_.object_trees[metadata_content_.cnt_object_trees].num_from = object_tree.num_from;
    metadata_content_.object_trees[metadata_content_.cnt_object_trees].num_to = object_tree.num_to;
    for (uint32_t i = 0; i < object_tree.num_from; i++)
        metadata_content_.object_trees[metadata_content_.cnt_object_trees].from[i] = object_tree.from[i];
    for (uint32_t i = 0; i < object_tree.num_to; i++)
        metadata_content_.object_trees[metadata_content_.cnt_object_trees].to[i] = object_tree.to[i];
    metadata_content_.cnt_object_trees++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddAtomicEvent(metadata_atomic_event_t atomic_event)
{
    if (metadata_content_.cnt_atomic_events >= MD_ATOMIC_EVENT_MAX)
        return JSON_VA_INVALID;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].
        start_time = atomic_event.start_time;
    strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].
        rule_name, atomic_event.rule_name);
    strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].
        rule_id.uuid, atomic_event.rule_id.uuid);
    strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].
        event_id.uuid, atomic_event.event_id.uuid);
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_type = atomic_event.event_type;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_camera_tamper_detected = atomic_event.event_details.details_camera_tamper_detected;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_detected = atomic_event.event_details.details_face_detected;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_motion_detected = atomic_event.event_details.details_motion_detected;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_object_counted = atomic_event.event_details.details_object_counted;
    metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_object_trackor = atomic_event.event_details.details_object_trackor;
    if (atomic_event.event_type == METADATA_EVENT_FACE_RECOGNIZED){
        strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_recognized.display_name, atomic_event.event_details.details_face_recognized.display_name);
        strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_recognized.group_name, atomic_event.event_details.details_face_recognized.group_name);
        strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_recognized.group_id.uuid, atomic_event.event_details.details_face_recognized.group_id.uuid);
        strcpy(metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_recognized.person_id.uuid, atomic_event.event_details.details_face_recognized.person_id.uuid);
        metadata_content_.atomic_events[metadata_content_.cnt_atomic_events].event_details.details_face_recognized.object_id = atomic_event.event_details.details_face_recognized.object_id;
    }
    metadata_content_.cnt_atomic_events++;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AddEvent(metadata_event_t event)
{
    if (metadata_content_.cnt_events >= MD_EVENT_MAX)
        return JSON_VA_INVALID;
    metadata_content_.events[metadata_content_.cnt_events].start_time = event.start_time;
    strcpy(metadata_content_.events[metadata_content_.cnt_events].event_id.uuid, event.event_id.uuid);
    strcpy(metadata_content_.events[metadata_content_.cnt_events].rule_id.uuid, event.rule_id.uuid);
    strcpy(metadata_content_.events[metadata_content_.cnt_events].rule_name, event.rule_name);
    metadata_content_.events[metadata_content_.cnt_events].composite_event.num_sub_events = event.composite_event.num_sub_events;
    strcpy(metadata_content_.events[metadata_content_.cnt_events].composite_event.sub_event_id1.uuid, event.composite_event.sub_event_id1.uuid);
    if (event.composite_event.num_sub_events==2)
        strcpy(metadata_content_.events[metadata_content_.cnt_events].composite_event.sub_event_id2.uuid, event.composite_event.sub_event_id2.uuid);
    metadata_content_.cnt_events++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddHeatMap(metadata_heat_map_t heat_map)
{
    if (metadata_content_.cnt_heat_maps >= MD_HEAT_MAP_MAX)
        return JSON_VA_INVALID;
    metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].start_time = heat_map.start_time;
    metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].end_time = heat_map.end_time;
    if (metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].cells_per_row != heat_map.cells_per_row || metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].rows_per_column != heat_map.rows_per_column){
        if(metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].data)
            free(metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].data);
        metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].data = (uint16_t *)malloc(heat_map.cells_per_row*heat_map.rows_per_column*sizeof(uint16_t));
    }
    metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].cells_per_row = heat_map.cells_per_row;
    metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].rows_per_column = heat_map.rows_per_column;
    memcpy(metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].data, heat_map.data, heat_map.cells_per_row *heat_map.rows_per_column*sizeof(uint16_t));
    metadata_content_.cnt_heat_maps++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddExtension(metadata_extension_t extension)
{
    if (metadata_content_.cnt_extensions >= MD_EXTENSION_MAX)
        return JSON_VA_INVALID;
    if (metadata_content_.extensions[metadata_content_.cnt_extensions].size_data != extension.size_data){
        if (metadata_content_.extensions[metadata_content_.cnt_extensions].data)
            free(metadata_content_.extensions[metadata_content_.cnt_extensions].data);
        metadata_content_.extensions[metadata_content_.cnt_extensions].data =
            new char[extension.size_data];
    }
    metadata_content_.extensions[metadata_content_.cnt_extensions].size_data = extension.size_data;
    memcpy(metadata_content_.extensions[metadata_content_.cnt_extensions].data, extension.data, extension.size_data);
    strcpy(metadata_content_.extensions[metadata_content_.cnt_extensions].engine_id, extension.engine_id);
    metadata_content_.cnt_extensions++;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AddObject(
    struct metadata_content_t *metadata_content,
    metadata_object_t object)
{
    if (metadata_content->cnt_objects > MD_OBJECT_MAX)
        return JSON_VA_INVALID;
    metadata_content->objects[metadata_content->cnt_objects].object_id =
        object.object_id;
    strcpy(metadata_content->objects[metadata_content->cnt_objects].
        engine_type, object.engine_type);
    metadata_content->objects[metadata_content->cnt_objects].
        appearance_descriptor = object.appearance_descriptor;
    metadata_content->objects[metadata_content->cnt_objects].on_event.status =
        object.on_event.status;
    metadata_content->objects[metadata_content->cnt_objects].
        on_event.num_events = object.on_event.num_events;
    if (object.on_event.status){
        for (uint32_t i = 0; i < object.on_event.num_events; i++)
            strcpy(metadata_content->objects[metadata_content->cnt_objects].
            on_event.event_ids[i].uuid, object.on_event.event_ids[i].uuid);
    }
    metadata_content->cnt_objects++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddObjectTree(
    struct metadata_content_t *metadata_content,
    metadata_object_tree_t object_tree)
{
    if (metadata_content->cnt_object_trees >= MD_OBJECT_TREE_MAX)
        return JSON_VA_INVALID;
    metadata_content->object_trees[metadata_content->cnt_object_trees].action = object_tree.action;
    metadata_content->object_trees[metadata_content->cnt_object_trees].num_from = object_tree.num_from;
    metadata_content->object_trees[metadata_content->cnt_object_trees].num_to = object_tree.num_to;
    for (uint32_t i = 0; i < object_tree.num_from; i++)
        metadata_content->object_trees[metadata_content->cnt_object_trees].from[i] = object_tree.from[i];
    for (uint32_t i = 0; i < object_tree.num_to; i++)
        metadata_content->object_trees[metadata_content->cnt_object_trees].to[i] = object_tree.to[i];
    metadata_content->cnt_object_trees++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddAtomicEvent(
    struct metadata_content_t *metadata_content,
    metadata_atomic_event_t atomic_event)
{
    if (metadata_content->cnt_atomic_events >= MD_ATOMIC_EVENT_MAX)
        return JSON_VA_INVALID;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].
        start_time = atomic_event.start_time;
    strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].
        rule_name, atomic_event.rule_name);
    strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].
        rule_id.uuid, atomic_event.rule_id.uuid);
    strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].
        event_id.uuid, atomic_event.event_id.uuid);
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_type = atomic_event.event_type;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_camera_tamper_detected = atomic_event.event_details.details_camera_tamper_detected;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_detected = atomic_event.event_details.details_face_detected;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_motion_detected = atomic_event.event_details.details_motion_detected;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_object_counted = atomic_event.event_details.details_object_counted;
    metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_object_trackor = atomic_event.event_details.details_object_trackor;
    if (atomic_event.event_type == METADATA_EVENT_FACE_RECOGNIZED){
        strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_recognized.display_name, atomic_event.event_details.details_face_recognized.display_name);
        strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_recognized.group_name, atomic_event.event_details.details_face_recognized.group_name);
        strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_recognized.group_id.uuid, atomic_event.event_details.details_face_recognized.group_id.uuid);
        strcpy(metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_recognized.person_id.uuid, atomic_event.event_details.details_face_recognized.person_id.uuid);
        metadata_content->atomic_events[metadata_content->cnt_atomic_events].event_details.details_face_recognized.object_id = atomic_event.event_details.details_face_recognized.object_id;
    }
    metadata_content->cnt_atomic_events++;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AddEvent(
    struct metadata_content_t *metadata_content,
    metadata_event_t event)
{
    if (metadata_content->cnt_events >= MD_EVENT_MAX)
        return JSON_VA_INVALID;
    metadata_content->events[metadata_content->cnt_events].start_time = event.start_time;
    strcpy(metadata_content->events[metadata_content->cnt_events].event_id.uuid, event.event_id.uuid);
    strcpy(metadata_content->events[metadata_content->cnt_events].rule_id.uuid, event.rule_id.uuid);
    strcpy(metadata_content->events[metadata_content->cnt_events].rule_name, event.rule_name);
    metadata_content->events[metadata_content->cnt_events].composite_event.num_sub_events = event.composite_event.num_sub_events;
    strcpy(metadata_content->events[metadata_content->cnt_events].composite_event.sub_event_id1.uuid, event.composite_event.sub_event_id1.uuid);
    if (event.composite_event.num_sub_events == 2)
        strcpy(metadata_content->events[metadata_content->cnt_events].composite_event.sub_event_id2.uuid, event.composite_event.sub_event_id2.uuid);
    metadata_content->cnt_events++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddHeatMap(
    struct metadata_content_t *metadata_content,
    metadata_heat_map_t heat_map)
{
    if (metadata_content->cnt_heat_maps >= MD_HEAT_MAP_MAX)
        return JSON_VA_INVALID;
    metadata_content->heat_maps[metadata_content->cnt_heat_maps].start_time = heat_map.start_time;
    metadata_content->heat_maps[metadata_content->cnt_heat_maps].end_time = heat_map.end_time;
    if (metadata_content->heat_maps[metadata_content->cnt_heat_maps].cells_per_row != heat_map.cells_per_row || metadata_content_.heat_maps[metadata_content_.cnt_heat_maps].rows_per_column != heat_map.rows_per_column){
        if (metadata_content->heat_maps[metadata_content->cnt_heat_maps].data)
            free(metadata_content->heat_maps[metadata_content->cnt_heat_maps].data);
        metadata_content->heat_maps[metadata_content->cnt_heat_maps].data = (uint16_t *)malloc(heat_map.cells_per_row*heat_map.rows_per_column*sizeof(uint16_t));
    }
    metadata_content->heat_maps[metadata_content->cnt_heat_maps].cells_per_row = heat_map.cells_per_row;
    metadata_content->heat_maps[metadata_content->cnt_heat_maps].rows_per_column = heat_map.rows_per_column;
    memcpy(metadata_content->heat_maps[metadata_content->cnt_heat_maps].data, heat_map.data, heat_map.cells_per_row *heat_map.rows_per_column*sizeof(uint16_t));
    metadata_content->cnt_heat_maps++;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::AddExtension(
    struct metadata_content_t *metadata_content,
    metadata_extension_t extension)
{
    if (metadata_content->cnt_extensions >= MD_EXTENSION_MAX)
        return JSON_VA_INVALID;
    if (metadata_content->extensions[metadata_content->cnt_extensions].size_data != extension.size_data){
        if (metadata_content->extensions[metadata_content->cnt_extensions].data)
            free(metadata_content->extensions[metadata_content->cnt_extensions].data);
        metadata_content->extensions[metadata_content->cnt_extensions].data =
            new char[extension.size_data];
    }
    metadata_content->extensions[metadata_content->cnt_extensions].size_data = extension.size_data;
    memcpy(metadata_content->extensions[metadata_content->cnt_extensions].data, extension.data, extension.size_data);
    strcpy(metadata_content->extensions[metadata_content->cnt_extensions].engine_id, extension.engine_id);
    metadata_content->cnt_extensions++;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::CollectMetadata(
    VAUUID camera_id,
    uint64_t time_stamp,
    std::vector<VAObject> objects,
    std::vector<VAObjectTree> object_trees,
    std::vector<VAAtomicEvent> atomic_events,
    std::vector<VAEvent> events,
    std::vector<VAHeatMap> heat_map,
    vector<VAExtension> extensions)
{
    SetCameraID(camera_id);
    SetTimeStamp(time_stamp);
    objects_ = objects;
    atomic_events_ = atomic_events;
    events_ = events;
    object_trees_ = object_trees;
    heat_map_ = heat_map;
    extensions_ =  extensions;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::ObjectsToJSON(Json::Value &objects_json)
{

    Json::Value objects(Json::arrayValue);
    for (unsigned int i = 0; i < objects_.size(); i++){
        Json::Value object(Json::objectValue);
        object["object_id"] = objects_[i].object_id;
        object["engine_type"] = objects_[i].engine_type;
        Json::Value appearance(Json::objectValue);
        Json::Value location(Json::objectValue);
        location["x"] = objects_[i].appearance_descriptor.location.x;
        location["y"] = objects_[i].appearance_descriptor.location.y;
        location["width"] = objects_[i].appearance_descriptor.location.width;
        location["height"] = objects_[i].appearance_descriptor.location.height;
        appearance["location"] = location;
        appearance["object_type"] =
            (int)objects_[i].appearance_descriptor.object_type;
        appearance["object_type_confidence"] =
            (int)objects_[i].appearance_descriptor.object_type_confidence;
        if (objects_[i].engine_type== "object_tracker"){
            appearance["physical_width"] = objects_[i].appearance_descriptor.
                object_tracker_appearance.physical_width;
            appearance["physical_height"] = objects_[i].appearance_descriptor.
                object_tracker_appearance.physical_height;
            appearance["moving_dir"] = objects_[i].appearance_descriptor.
                object_tracker_appearance.moving_dir;
            appearance["moving_speed"] = objects_[i].appearance_descriptor.
                object_tracker_appearance.moving_speed;
        }
        appearance["object_type"] =
            (int)objects_[i].appearance_descriptor.object_type;
        object["appearance"] = appearance;
        if (objects_[i].on_event.status){
            Json::Value on_event(Json::objectValue);
            on_event["status"] = objects_[i].on_event.status;
            Json::Value events(Json::arrayValue);
            for (unsigned int k = 0; k < objects_[i].on_event.event_ids.size();
                k++){
                Json::Value event(Json::objectValue);
                event["event"] = objects_[i].on_event.event_ids[k].uuid;
                on_event["events"].append(event);
            }
            object["on_event"] = on_event;
        }

        objects.append(object);
    }
    objects_json = objects;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::ObjectTreesToJSON(Json::Value &object_trees_json)
{
    Json::Value object_trees(Json::arrayValue);
    for (unsigned int i = 0; i < object_trees_.size(); i++){
        Json::Value object_tree(Json::objectValue);
        switch (object_trees_[i].action){
        case OBJECT_TREE_MERGED:{
            Json::Value merged(Json::objectValue);
            for (unsigned int k = 0; k < object_trees_[i].from.size(); k++){
                merged["from"].append(object_trees_[i].from[k]);
            }
            merged["to"] = object_trees_[i].to[0];
            object_tree["merged"] = merged;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_SPLIT:{
            Json::Value split(Json::objectValue);
            split["from"] = object_trees_[i].from[0];
            for (unsigned int k = 0; k < object_trees_[i].to.size(); k++){
                split["to"].append(object_trees_[i].to[k]);
            }
            object_tree["split"] = split;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_CREATED:{
            Json::Value created(Json::objectValue);
            created["to"] = object_trees_[i].to[0];
            object_tree["created"] = created;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_DELETED:{
            Json::Value deleted(Json::objectValue);
            deleted["from"] = object_trees_[i].from[0];
            object_tree["deleted"] = deleted;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_RENAMED:{
            Json::Value renamed(Json::objectValue);
            renamed["from"] = object_trees_[i].from[0];
            renamed["to"] = object_trees_[i].to[0];
            object_tree["renamed"] = renamed;
            object_trees.append(object_tree);
            break;
        }
        default:
            break;
        }
    }
    object_trees_json = object_trees;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AtomicEventsToJSON(Json::Value &atomic_events_json)
{
    if (atomic_events_.size() == 0)
        return JSON_VA_INVALID;
    Json::Value atomic_events(Json::arrayValue);
    for (unsigned int i = 0; i < atomic_events_.size(); i++){
        Json::Value atomic_event(Json::objectValue);
        atomic_event["rule_id"] = atomic_events_[i].rule_id.uuid;
        atomic_event["event_id"] = atomic_events_[i].event_id.uuid;
        ////atomic_event["rule_name"] = atomic_events_[i].rule_name;
        atomic_event["event_type"] = atomic_events_[i].event_type;
        if (time_stamp_ != atomic_events_[i].start_time)
            atomic_event["start_time"] = atomic_events_[i].start_time;
        switch (atomic_events_[i].event_type){
        case METADATA_EVENT_CAMERA_TAMPER_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["tamper_type"] = atomic_events_[i].event_details.
                details_camera_tamper_detected.tamper_type;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_MOTION_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["motion_activity"] = atomic_events_[i].event_details.
                details_motion_detected.motion_activity;
            event_details["object_id"] = atomic_events_[i].event_details.
                details_motion_detected.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_INTRUSION_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_LINECROSSED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_COUNTED:{
            Json::Value event_details(Json::objectValue);
            event_details["count"] = atomic_events_[i].event_details.
                details_object_counted.count;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_LOITERING_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_FACE_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_face_detected.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_FACE_RECOGNIZED:{
            Json::Value event_details(Json::objectValue);
            event_details["person_id"] = atomic_events_[i].event_details.
                details_face_recognized.person_id.uuid;
            event_details["group_id"] = atomic_events_[i].event_details.
                details_face_recognized.group_id.uuid;
            event_details["object_id"] = atomic_events_[i].event_details.
                details_face_recognized.object_id;
            event_details["display_name"] = atomic_events_[i].event_details.
                details_face_recognized.display_name;;
            event_details["group_name"] = atomic_events_[i].event_details.
                details_face_recognized.group_name;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_CLASSIFIED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        default:
            break;
        }

    }
    atomic_events_json = atomic_events;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::EventsToJSON(Json::Value &events_json)
{
    if (events_.size() > 0){
        Json::Value events(Json::arrayValue);
        for (unsigned int i = 0; i < events_.size(); i++){
            Json::Value event(Json::objectValue);
            if (time_stamp_ != events_[i].start_time)
                event["start_time"] = events_[i].start_time;
            event["event_id"] = events_[i].event_id.uuid;
            event["rule_id"] = events_[i].rule_id.uuid;
            event["rule_name"] = events_[i].rule_name;
            Json::Value composite_event(Json::objectValue);
            composite_event["sub_event_id1"] =
                events_[i].composite_event.sub_event_id1.uuid;
            if (events_[i].composite_event.num_sub_events == 2)
                composite_event["sub_event_id2"] =
                events_[i].composite_event.sub_event_id2.uuid;
            event["composite_event"] = composite_event;
            events.append(event);
        }
        events_json = events;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::HeatMapToJSON(Json::Value &heat_map_json)
{

    Json::Value heat_map(Json::objectValue);
    if (heat_map_.size()>0){
        heat_map["cells_per_row"] = heat_map_[0].cells_per_row;
        heat_map["rows_per_column"] = heat_map_[0].rows_per_column;
        heat_map["start_time"] = heat_map_[0].start_time;
        heat_map["end_time"] = heat_map_[0].end_time;
        Json::Value data(Json::arrayValue);
        for (unsigned int i = 0; i < heat_map_[0].cells_per_row*heat_map_[0].
            rows_per_column; i++)
            data.append(heat_map_[0].data[i]);
        heat_map["data"] = data;
        heat_map_json = heat_map;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::ExtensionsToJSON(Json::Value &extensions_json)
{
    Json::Value extensions(Json::arrayValue);
    if (extensions_.size()>0){
        for (int i = 0; i < (int)extensions_.size(); i++)
        {
            Json::Value extension(Json::objectValue);
            extension["engine_id"] = extensions_[i].engine_id;
            extension["data"] = extensions_[i].data;
            extensions.append(extension);
        }
        extensions_json = extensions;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::ToJSON(Json::Value &metadata_json)
{
    Json::Value root(Json::objectValue);
    Json::Value objects_json;
    Json::Value object_trees_json;
    Json::Value atomic_events_json;
    Json::Value extensions_json;
    Json::Value events_json;
    Json::Value heat_map_json;
    Json::Value scene_json(Json::objectValue);
    root["camera_id"] = camera_id_.uuid;
    root["time_stamp"] = time_stamp_;
    ObjectsToJSON(objects_json);
    ObjectTreesToJSON(object_trees_json);
    AtomicEventsToJSON(atomic_events_json);
    EventsToJSON(events_json);
    HeatMapToJSON(heat_map_json);
    ExtensionsToJSON(extensions_json);
    root["objects"] = objects_json;
    root["object_tree"] = object_trees_json;
    root["atomic_events"] = atomic_events_json;
    root["events"] = events_json;
    scene_json["heat_map"] = heat_map_json;
    root["scene"] = scene_json;
    root["extension"] = extensions_json;
    metadata_json = root;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::ObjectsToJSON(struct metadata_content_t metadata_content, Json::Value &objects_json)
{

    Json::Value objects(Json::arrayValue);
    for (unsigned int i = 0; i < metadata_content.cnt_objects; i++){
        Json::Value object(Json::objectValue);
        object["object_id"] = metadata_content.objects[i].object_id;
        object["engine_type"] = string(metadata_content.objects[i].engine_type);
        Json::Value appearance(Json::objectValue);
        Json::Value location(Json::objectValue);
        location["x"] = metadata_content.objects[i].appearance_descriptor.location.x;
        location["y"] = metadata_content.objects[i].appearance_descriptor.location.y;
        location["width"] = metadata_content.objects[i].appearance_descriptor.location.width;
        location["height"] = metadata_content.objects[i].appearance_descriptor.location.height;
        appearance["location"] = location;
        appearance["object_type"] =
            (int)metadata_content.objects[i].appearance_descriptor.object_type;
        appearance["object_type_confidence"] =
            (int)metadata_content.objects[i].appearance_descriptor.object_type_confidence;
        if (strcmp(metadata_content.objects[i].engine_type, "object_tracker")==0){
            appearance["physical_width"] = metadata_content.objects[i].appearance_descriptor.
                object_tracker_appearance.physical_width;
            appearance["physical_height"] = metadata_content.objects[i].appearance_descriptor.
                object_tracker_appearance.physical_height;
            appearance["moving_dir"] = metadata_content.objects[i].appearance_descriptor.
                object_tracker_appearance.moving_dir;
            appearance["moving_speed"] = metadata_content.objects[i].appearance_descriptor.
                object_tracker_appearance.moving_speed;
        }
        appearance["object_type"] =
            (int)metadata_content.objects[i].appearance_descriptor.object_type;
        object["appearance"] = appearance;
        if (metadata_content.objects[i].on_event.status){
            Json::Value on_event(Json::objectValue);
            on_event["status"] = metadata_content.objects[i].on_event.status;
            Json::Value events(Json::arrayValue);
            for (unsigned int k = 0; k < metadata_content.objects[i].on_event.num_events;
                k++){
                Json::Value event(Json::objectValue);
                event["event"] = metadata_content.objects[i].on_event.event_ids[k].uuid;
                on_event["events"].append(event);
            }
            object["on_event"] = on_event;
        }
        int flag_reserve = 0;
        int flag_reserveStr = 0;
        for (int k = 0; k < 5; k++){
            if (metadata_content.objects[i].reserve[k] != 0)
                flag_reserve++;
            if (metadata_content.objects[i].reserve_str[k][0] != 0)
                flag_reserveStr++;
        }
        if (flag_reserve){
            Json::Value reserve(Json::arrayValue);
            for (int k = 0; k < 5; k++){
                reserve.append(metadata_content.objects[i].reserve[k]);
            }
            object["reserve"] = reserve;
        }
        if (flag_reserveStr){
            Json::Value reserve_str(Json::arrayValue);
            for (int k = 0; k < 5; k++){
                reserve_str.append(metadata_content.objects[i].reserve_str[k]);
            }
            object["reserve_str"] = reserve_str;
        }
        objects.append(object);
    }
    objects_json = objects;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::ObjectTreesToJSON(struct metadata_content_t metadata_content, Json::Value &object_trees_json)
{
    Json::Value object_trees(Json::arrayValue);
    for (unsigned int i = 0; i < metadata_content.cnt_object_trees; i++){
        Json::Value object_tree(Json::objectValue);
        switch (metadata_content.object_trees[i].action){
        case OBJECT_TREE_MERGED:{
            Json::Value merged(Json::objectValue);
            for (unsigned int k = 0; k < metadata_content.object_trees[i].num_from; k++){
                merged["from"].append(metadata_content.object_trees[i].from[k]);
            }
            merged["to"] = metadata_content.object_trees[i].to[0];
            object_tree["merged"] = merged;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_SPLIT:{
            Json::Value split(Json::objectValue);
            split["from"] = metadata_content.object_trees[i].from[0];
            for (unsigned int k = 0; k < metadata_content.object_trees[i].num_to; k++){
                split["to"].append(metadata_content.object_trees[i].to[k]);
            }
            object_tree["split"] = split;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_CREATED:{
            Json::Value created(Json::objectValue);
            created["to"] = metadata_content.object_trees[i].to[0];
            object_tree["created"] = created;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_DELETED:{
            Json::Value deleted(Json::objectValue);
            deleted["from"] = metadata_content.object_trees[i].from[0];
            object_tree["deleted"] = deleted;
            object_trees.append(object_tree);
            break;
        }
        case OBJECT_TREE_RENAMED:{
            Json::Value renamed(Json::objectValue);
            renamed["from"] = metadata_content.object_trees[i].from[0];
            renamed["to"] = metadata_content.object_trees[i].to[0];
            object_tree["renamed"] = renamed;
            object_trees.append(object_tree);
            break;
        }
        default:
            break;
        }
    }
    object_trees_json = object_trees;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::AtomicEventsToJSON(struct metadata_content_t metadata_content, Json::Value &atomic_events_json)
{
    if (metadata_content.cnt_atomic_events == 0)
        return JSON_VA_INVALID;
    Json::Value atomic_events(Json::arrayValue);
    for (unsigned int i = 0; i < metadata_content.cnt_atomic_events; i++){
        Json::Value atomic_event(Json::objectValue);
        atomic_event["rule_id"] = string(metadata_content.atomic_events[i].rule_id.uuid);
        atomic_event["event_id"] = string(metadata_content.atomic_events[i].event_id.uuid);
        ////atomic_event["rule_name"] = atomic_events_[i].rule_name;
        atomic_event["event_type"] = metadata_content.atomic_events[i].event_type;
        if (metadata_content.time_stamp != metadata_content.atomic_events[i].start_time)
            atomic_event["start_time"] = metadata_content.atomic_events[i].start_time;
        int flag_reserve = 0;
        int flag_reserveStr = 0;
        for (int k = 0; k < 5; k++){
            if (metadata_content.atomic_events[i].reserve[k] != 0)
                flag_reserve++;
            if (metadata_content.atomic_events[i].reserve_str[k][0] != 0)
                flag_reserveStr++;
        }
        if (flag_reserve){
            Json::Value reserve(Json::arrayValue);
            for (int k = 0; k < 5; k++){
                reserve.append(metadata_content.atomic_events[i].reserve[k]);
            }
            atomic_event["reserve"] = reserve;
        }
        if (flag_reserveStr){
            Json::Value reserve_str(Json::arrayValue);
            for (int k = 0; k < 5; k++){
                reserve_str.append(metadata_content.atomic_events[i].reserve_str[k]);
            }
            atomic_event["reserve_str"] = reserve_str;
        }
        switch (metadata_content.atomic_events[i].event_type){
        case METADATA_EVENT_CAMERA_TAMPER_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["tamper_type"] = metadata_content.atomic_events[i].event_details.
                details_camera_tamper_detected.tamper_type;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_MOTION_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["motion_activity"] = metadata_content.atomic_events[i].event_details.
                details_motion_detected.motion_activity;
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_motion_detected.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_INTRUSION_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_LINECROSSED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_COUNTED:{
            Json::Value event_details(Json::objectValue);
            event_details["count"] = metadata_content.atomic_events[i].event_details.
                details_object_counted.count;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_LOITERING_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_FACE_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_face_detected.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_FACE_RECOGNIZED:{
            Json::Value event_details(Json::objectValue);
            event_details["person_id"] = string(metadata_content.atomic_events[i].event_details.
                details_face_recognized.person_id.uuid);
            event_details["group_id"] = string(metadata_content.atomic_events[i].event_details.
                details_face_recognized.group_id.uuid);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_face_recognized.object_id;
            event_details["display_name"] = string(metadata_content.atomic_events[i].event_details.
                details_face_recognized.display_name);
            event_details["group_name"] = string(metadata_content.atomic_events[i].event_details.
                details_face_recognized.group_name);
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_CLASSIFIED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = metadata_content.atomic_events[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        case METADATA_EVENT_OBJECT_DETECTED:{
            Json::Value event_details(Json::objectValue);
            event_details["object_id"] = atomic_events_[i].event_details.
                details_object_trackor.object_id;
            atomic_event["event_details"] = event_details;
            atomic_events.append(atomic_event);
            break;
        }
        default:
            break;
        }

    }
    atomic_events_json = atomic_events;
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::EventsToJSON(struct metadata_content_t metadata_content, Json::Value &events_json)
{
    if (metadata_content.cnt_events > 0){
        Json::Value events(Json::arrayValue);
        for (unsigned int i = 0; i < metadata_content.cnt_events; i++){
            Json::Value event(Json::objectValue);
            if (metadata_content.time_stamp != metadata_content.events[i].start_time)
                event["start_time"] = metadata_content.events[i].start_time;
            event["event_id"] = string(metadata_content.events[i].event_id.uuid);
            event["rule_id"] = string(metadata_content.events[i].rule_id.uuid);
            event["rule_name"] = string(metadata_content.events[i].rule_name);
            Json::Value composite_event(Json::objectValue);
            composite_event["sub_event_id1"] =
                string(metadata_content.events[i].composite_event.
                    sub_event_id1.uuid);
            if (metadata_content.events[i].composite_event.
                num_sub_events == 2)
                composite_event["sub_event_id2"] =
                    string(metadata_content.events[i].composite_event.
                    sub_event_id2.uuid);
            event["composite_event"] = composite_event;
            int flag_reserve = 0;
            int flag_reserveStr = 0;
            for (int k = 0; k < 5; k++){
                if (metadata_content.events[i].reserve[k] != 0)
                    flag_reserve++;
                if (metadata_content.events[i].reserve_str[k][0] != 0)
                    flag_reserveStr++;
            }
            if (flag_reserve){
                Json::Value reserve(Json::arrayValue);
                for (int k = 0; k < 5; k++){
                    reserve.append(metadata_content.events[i].reserve[k]);
                }
                event["reserve"] = reserve;
            }
            if (flag_reserveStr){
                Json::Value reserve_str(Json::arrayValue);
                for (int k = 0; k < 5; k++){
                    reserve_str.append(metadata_content.events[i].reserve_str[k]);
                }
                event["reserve_str"] = reserve_str;
            }
            events.append(event);
        }
        events_json = events;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::HeatMapToJSON(struct metadata_content_t metadata_content, Json::Value &heat_map_json)
{

    Json::Value heat_map(Json::objectValue);
    if (metadata_content.cnt_heat_maps>0){
        heat_map["cells_per_row"] = metadata_content.heat_maps[0].cells_per_row;
        heat_map["rows_per_column"] =
            metadata_content.heat_maps[0].rows_per_column;
        heat_map["start_time"] = metadata_content.heat_maps[0].start_time;
        heat_map["end_time"] = metadata_content.heat_maps[0].end_time;
        Json::Value data(Json::arrayValue);
        for (unsigned int i = 0;
            i < metadata_content.heat_maps[0].cells_per_row*metadata_content.
            heat_maps[0].rows_per_column; i++)
            data.append(metadata_content.heat_maps[0].data[i]);
        heat_map["data"] = data;
        heat_map_json = heat_map;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::ExtensionsToJSON(struct metadata_content_t metadata_content, Json::Value &extensions_json)
{
    Json::Value extensions(Json::arrayValue);
    if (metadata_content.cnt_extensions>0){
        for (uint32_t i = 0; i < metadata_content.cnt_extensions; i++)
        {
            Json::Value extension(Json::objectValue);
            extension["engine_id"] = string(metadata_content.extensions[i].engine_id);
            extension["data"] = string(metadata_content.extensions[i].data);
            extensions.append(extension);
        }
        extensions_json = extensions;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
JSONVAStatus MetadataGenerator::ToJSON(struct metadata_content_t metadata_content, Json::Value &metadata_json)
{
    Json::Value root(Json::objectValue);
    Json::Value objects_json;
    Json::Value object_trees_json;
    Json::Value atomic_events_json;
    Json::Value extensions_json;
    Json::Value events_json;
    Json::Value heat_map_json;
    Json::Value scene_json(Json::objectValue);
    last_time_stamp_ = time_stamp_;
    time_stamp_ = metadata_content.time_stamp;
    time_interval_ = time_stamp_ - last_time_stamp_;
    root["camera_id"] = string(metadata_content.camera_id.uuid);
    root["time_stamp"] = metadata_content.time_stamp;
    if (metadata_content.cnt_objects > 0){
        ObjectsToJSON(metadata_content, objects_json);
        root["objects"] = objects_json;
    }
    if (metadata_content.cnt_object_trees > 0){
        ObjectTreesToJSON(metadata_content, object_trees_json);
        root["object_tree"] = object_trees_json;
    }
    if (metadata_content.cnt_atomic_events > 0){
        AtomicEventsToJSON(metadata_content, atomic_events_json);
        root["atomic_events"] = atomic_events_json;
    }
    if (metadata_content.cnt_events > 0){
        EventsToJSON(metadata_content, events_json);
        root["events"] = events_json;
    }
    if (metadata_content.cnt_heat_maps > 0){
        HeatMapToJSON(metadata_content, heat_map_json);
        scene_json["heat_map"] = heat_map_json;
        root["scene"] = scene_json;
    }
    if (metadata_content.cnt_extensions > 0){
        ExtensionsToJSON(metadata_content, extensions_json);
        root["extension"] = extensions_json;
    }
    metadata_json = root;
    return JSON_VA_OK;
}

JSONVAStatus MetadataGenerator::Serialize(
    Json::Value metadata_json,
    std::string  &output)
{
    Json::FastWriter writer;
    //Json::StyledWriter writer;
    output = writer.write(metadata_json);    // metadata_json.toStyledString();
    return JSON_VA_OK;
}


JSONVAStatus MetadataGenerator::AllocateMetadataContent(
struct metadata_content_t *metadata_content)
{
    metadata_content->atomic_events = (struct metadata_atomic_event_t*)
        malloc(sizeof(struct metadata_atomic_event_t)*MD_ATOMIC_EVENT_MAX);
    metadata_content->events = (struct metadata_event_t*)
        malloc(sizeof(struct metadata_event_t)*MD_EVENT_MAX);
    metadata_content->extensions = (struct metadata_extension_t*)
        malloc(sizeof(struct metadata_extension_t)*MD_EXTENSION_MAX);
    metadata_content->heat_maps = (struct metadata_heat_map_t*)
        malloc(sizeof(struct metadata_heat_map_t)*MD_HEAT_MAP_MAX);
    metadata_content->objects = (struct metadata_object_t*)
        malloc(sizeof(struct metadata_object_t)*MD_OBJECT_MAX);
    metadata_content->object_trees = (struct metadata_object_tree_t*)
        malloc(sizeof(struct metadata_object_tree_t)*MD_OBJECT_TREE_MAX);
    memset(metadata_content->atomic_events, 0, sizeof(struct metadata_atomic_event_t)*MD_ATOMIC_EVENT_MAX);
    memset(metadata_content->events, 0, sizeof(struct metadata_event_t)*MD_EVENT_MAX);
    memset(metadata_content->extensions, 0, sizeof(struct metadata_extension_t)*MD_EXTENSION_MAX);
    memset(metadata_content->heat_maps, 0, sizeof(struct metadata_heat_map_t)*MD_HEAT_MAP_MAX);
    memset(metadata_content->objects, 0, sizeof(struct metadata_object_t)*MD_OBJECT_MAX);
    memset(metadata_content->object_trees, 0, sizeof(struct metadata_object_tree_t)*MD_OBJECT_TREE_MAX);
    for (int k = 0; k < 5; k++){
        metadata_content->reserve[k] = 0;
        metadata_content->reserve_str[k][0] = 0;
    }
    for (int i = 0; i < MD_HEAT_MAP_MAX; i++)
    {
        metadata_content->heat_maps[i].data = 0;
        metadata_content->heat_maps[i].cells_per_row = 0;
        metadata_content->heat_maps[i].rows_per_column = 0;
    }
    for (int i = 0; i < MD_EXTENSION_MAX; i++)
    {
        metadata_content->extensions[i].size_data = 0;
        metadata_content->extensions[i].data = 0;
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::CopyMetadataContent(
    const struct metadata_content_t metadata_content_src,
    struct metadata_content_t *metadata_content_dst)
{

    return JSON_VA_OK;
}
JSONVAStatus MetadataGenerator::ReleaseMetadataContent(
    struct metadata_content_t *metadata_content)
{

    for (int i = 0; i < MD_HEAT_MAP_MAX; i++){
        if (metadata_content->heat_maps[i].data){
            free(metadata_content->heat_maps[i].data);
            metadata_content->heat_maps[i].data = 0;
        }
        metadata_content->heat_maps[i].cells_per_row = 0;
        metadata_content->heat_maps[i].rows_per_column = 0;
    }
    for (int i = 0; i < MD_EXTENSION_MAX; i++){
        if (metadata_content->extensions[i].data){
            free(metadata_content->extensions[i].data);
            metadata_content->extensions[i].data = 0;
        }
        metadata_content->extensions[i].size_data = 0;
    }
    free(metadata_content->atomic_events);
    free(metadata_content->events);
    free(metadata_content->extensions);
    free(metadata_content->heat_maps);
    free(metadata_content->objects);
    free(metadata_content->object_trees);
    return JSON_VA_OK;
}
JSONVAStatus metadata_generator_set_time_stamp(void * handle, uint64_t time_stamp)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->SetTimeStamp(time_stamp);
    return status;
}
JSONVAStatus metadata_generator_get_time_stamp(void * handle, uint64_t &time_stamp)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->GetTimeStamp(time_stamp);
    return status;
}
JSONVAStatus metadata_generator_set_camera_id(void * handle, struct metadata_uuid_t camera_id)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->SetCameraID(camera_id);
    return status;
}
JSONVAStatus metadata_generator_get_camera_id(void * handle,
struct metadata_uuid_t *camera_id)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->GetCameraID(camera_id);
    return status;
}
JSONVAStatus metadata_generator_add_object(void * handle,
    VAObject object)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddObject(object);
    return status;
}
JSONVAStatus metadata_generator_add_object_tree(void * handle,
    VAObjectTree object_tree)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddObjectTree(object_tree);
    return status;
}
JSONVAStatus metadata_generator_add_atomic_event(void * handle,
    VAAtomicEvent atomic_event)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddAtomicEvent(atomic_event);
    return status;
}
JSONVAStatus metadata_generator_add_event(void * handle, VAEvent event)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddEvent(event);
    return status;
}
JSONVAStatus metadata_generator_add_heat_map(void * handle,
    VAHeatMap heat_map)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddHeatMap(heat_map);
    return status;
}
JSONVAStatus metadata_generator_add_extension(void * handle, VAExtension extension)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddExtension(extension);
    return status;
}

JSONVAStatus MetadataGenerateToJSON(void * handle, Json::Value &metadata_json)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->ToJSON(metadata_json);
    return status;
}
JSONVAStatus MetadataGenerateSerialize(void * handle, Json::Value metadata_json,
    std::string  &output)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->Serialize(metadata_json, output);
    return status;
}
