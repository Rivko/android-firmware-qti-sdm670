/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_GENERATOR_H_
#define METADATA_GENERATOR_H_
#include <vector>
#include "metadata_types.h"
#include "json_common_structs.h"
#include <json/json.h>
extern "C"{
#include "json_common_types.h"
#include "json_metadata.h"
#include "metadata_generator_apis.h"
}

class MetadataGenerator
{
public:
    MetadataGenerator();
    ~MetadataGenerator();
    JSONVAStatus Clear();
    JSONVAStatus SetTimeStamp(uint64_t time_stamp);
    JSONVAStatus GetTimeStamp(uint64_t &time_stamp);
    JSONVAStatus SetCameraID(VAUUID camera_id);
    JSONVAStatus GetCameraID(VAUUID &camera_id);
    JSONVAStatus AddObject(VAObject object);
    JSONVAStatus AddObjectTree(VAObjectTree object_tree);
    JSONVAStatus AddAtomicEvent(VAAtomicEvent atomic_event);
    JSONVAStatus AddEvent(VAEvent event);
    JSONVAStatus AddHeatMap(VAHeatMap heat_map);
    JSONVAStatus AddExtension(VAExtension extension);
    JSONVAStatus SetCameraID(struct metadata_uuid_t  camera_id);
    JSONVAStatus GetCameraID(struct metadata_uuid_t *camera_id);
    JSONVAStatus SetTimeInterval(uint64_t interval);
    JSONVAStatus AddObject(metadata_object_t object);
    JSONVAStatus AddObjectTree(metadata_object_tree_t object_tree);
    JSONVAStatus AddAtomicEvent(metadata_atomic_event_t atomic_event);
    JSONVAStatus AddEvent(metadata_event_t event);
    JSONVAStatus AddHeatMap(metadata_heat_map_t heat_map);
    JSONVAStatus AddExtension(metadata_extension_t extension);
    JSONVAStatus AddObject(struct metadata_content_t *metadata_content, metadata_object_t object);
    JSONVAStatus AddObjectTree(struct metadata_content_t *metadata_content, metadata_object_tree_t object_tree);
    JSONVAStatus AddAtomicEvent(struct metadata_content_t *metadata_content, metadata_atomic_event_t atomic_event);
    JSONVAStatus AddEvent(struct metadata_content_t *metadata_content, metadata_event_t event);
    JSONVAStatus AddHeatMap(struct metadata_content_t *metadata_content, metadata_heat_map_t heat_map);
    JSONVAStatus AddExtension(struct metadata_content_t *metadata_content, metadata_extension_t extension);
    JSONVAStatus ObjectsToJSON(struct metadata_content_t metadata_content,
        Json::Value &objects_json);
    JSONVAStatus ObjectTreesToJSON(struct metadata_content_t metadata_content,
        Json::Value &object_trees_json);
    JSONVAStatus AtomicEventsToJSON(struct metadata_content_t metadata_content,
        Json::Value &atomic_events_json);
    JSONVAStatus EventsToJSON(struct metadata_content_t metadata_content,
        Json::Value &events_json);
    JSONVAStatus HeatMapToJSON(struct metadata_content_t metadata_content,
        Json::Value &heat_map_json);
    JSONVAStatus ExtensionsToJSON(struct metadata_content_t metadata_content,
        Json::Value &extensions_json);
    JSONVAStatus ObjectsToJSON(Json::Value &objects_json);
    JSONVAStatus ObjectTreesToJSON(Json::Value &object_trees_json);
    JSONVAStatus AtomicEventsToJSON(Json::Value &atomic_events_json);
    JSONVAStatus EventsToJSON(Json::Value &events_json);
    JSONVAStatus HeatMapToJSON(Json::Value &heat_map_json);
    JSONVAStatus ExtensionsToJSON(Json::Value &extensions_json);
    JSONVAStatus ToJSON(struct metadata_content_t metadata_content,
        Json::Value &metadata_json);
    JSONVAStatus ToJSON(Json::Value &metadata_json);
    JSONVAStatus FromJSON(Json::Value metadata_json);
    JSONVAStatus Serialize(Json::Value metadata_json, std::string  &output);
    JSONVAStatus CollectMetadata(VAUUID camera_id, uint64_t time_stamp,
        std::vector<VAObject> objects, std::vector<VAObjectTree> object_trees,
        std::vector<VAAtomicEvent> atomic_events, std::vector<VAEvent> events,
        std::vector<VAHeatMap> heat_map, std::vector<VAExtension> extensions);
    JSONVAStatus AllocateMetadataContent(
        struct metadata_content_t *metadata_content);
    JSONVAStatus CopyMetadataContent(
        const struct metadata_content_t metadata_content_src,
        struct metadata_content_t *metadata_content_dst);
    JSONVAStatus ReleaseMetadataContent(struct metadata_content_t *metadata_content);
    FILE * fp_json_;
    FILE * fp_idx_;
    char file_prefix_[256];
    char filename_json_[1000];
    char filename_idx_[1000];
    uint32_t file_id_;
    struct metadata_content_t metadata_content_;
    int frame_count_per_chunk_;
    int chunk_count_per_file_;
    char * doc_str_;
    int frame_cnt;
    uint64_t time_stamp_;//Linux time stamp
    uint64_t start_time_stamp_;//time stamp of the first frame in a file/stream
    uint64_t last_time_stamp_;// time stamp of last generated frame
    uint64_t time_interval_; //the time interval between the current frame and the last frame
private:
    VAUUID	 camera_id_;	//the VAUUID of the camera
    std::vector<VAObject> objects_;
    std::vector<VAObjectTree> object_trees_;
    std::vector<VAAtomicEvent> atomic_events_;
    std::vector<VAEvent> events_;
    std::vector<VAHeatMap> heat_map_;
    std::vector<VAExtension> extensions_;
    Json::Value metadata_json_;
};
/**
* clear the buffer of the metadata generator. This must be called before generating a new metadata frame
* handle: the transparent handle of metadata generator
**/
JSONVAStatus metadata_generator_clear(void * handle);
/**
* set the time stamp of the metadata
* handle: the transparent handle of metadata generator
* time_stamp: the time stamp of the metadata frame
**/
JSONVAStatus metadata_generator_set_time_stamp(void * handle, uint64_t time_stamp);
/**
* get the time stamp of the metadata
* handle: the transparent handle of metadata generator
* time_stamp: the time stamp of the metadata frame
**/
JSONVAStatus metadata_generator_get_time_stamp(void * handle, uint64_t &time_stamp);
/**
* set the camera identifier of the metadata
* handle: the transparent handle of metadata generator
* camera_id: the camera identifier
**/
JSONVAStatus metadata_generator_set_camera_id(void * handle, struct metadata_uuid_t camera_id);
/**
* get the time stamp of the metadata
* handle: the transparent handle of metadata generator
* camera_id: the camera identifier
**/
JSONVAStatus metadata_generator_get_camera_id(void * handle, struct metadata_uuid_t &camera_id);
JSONVAStatus metadata_generator_add_object(void * handle, VAObject object);
/**
* add an entry of object_tree to the metadata
* handle: the transparent handle of metadata generator
* object_tree: an entry of the object_tree
**/
JSONVAStatus metadata_generator_add_object_tree(void * handle,
    VAObjectTree object_tree);
/**
* add an atomic event to the metadata
* handle: the transparent handle of metadata generator
* atomic_event: the atomic event to be added to the metadata
**/
JSONVAStatus metadata_generator_add_atomic_event(void * handle,
    VAAtomicEvent atomic_event);
/**
* add an event to the metadata
* handle: the transparent handle of metadata generator
* event: the event to be added to the metadata
**/
JSONVAStatus metadata_generator_add_event(void * handle, VAEvent event);
/**
* add a heat map of motion to the metadata
* handle: the transparent handle of metadata generator
* heat_map: the heat map to be added to the metadata
**/
JSONVAStatus metadata_generator_add_heat_map(void * handle, VAHeatMap heat_map);
/**
* add an extension to the metadata. The extention of metadata is a
* piece of metadata that is engine proprietary and VA manager will not
* consume
* handle: the transparent handle of metadata generator
* extension: the extension to be added to the metadata.
**/
JSONVAStatus metadata_generator_add_extension(void * handle, VAExtension extension);

/**
* add an object to the metadata
* handle: the transparent handle of metadata generator
* object: the object to be added to the metadata
**/
JSONVAStatus metadata_generator_add_object(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_object_t object);
/**
* add an entry of object_tree to the metadata
* handle: the transparent handle of metadata generator
* metadata_content: the metadata frame to store the metadata content
* object_tree: an entry of the object_tree
**/
JSONVAStatus metadata_generator_add_object_tree(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_object_tree_t object_tree);
/**
* add an atomic event to the metadata
* handle: the transparent handle of metadata generator
* metadata_content: the metadata frame to store the metadata content
* atomic_event: the atomic event to be added to the metadata
**/
JSONVAStatus metadata_generator_add_atomic_event(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_atomic_event_t atomic_event);
/**
* add an event to the metadata
* handle: the transparent handle of metadata generator
* metadata_content: the metadata frame to store the metadata content
* event: the event to be added to the metadata
**/
JSONVAStatus metadata_generator_add_event(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_event_t event);
/**
* add a heat map of motion to the metadata
* handle: the transparent handle of metadata generator
* metadata_content: the metadata frame to store the metadata content
* heat_map: the heat map to be added to the metadata
**/
JSONVAStatus metadata_generator_add_heat_map(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_heat_map_t heat_map);
/**
* add an extension to the metadata. The extention of metadata is a
* piece of metadata that is engine proprietary and VA manager will not
* consume
* handle: the transparent handle of metadata generator
* metadata_content: the metadata frame to store the metadata content
* extension: the extension to be added to the metadata.
**/
JSONVAStatus metadata_generator_add_extension(void * handle,
struct metadata_content_t *metadata_content,
struct metadata_extension_t extension);
JSONVAStatus MetadataGenerateToJSON(void * handle, Json::Value &metadata_json);
JSONVAStatus MetadataGenerateSerialize(void * handle, Json::Value metadata_json,
    std::string  &output);
#endif
