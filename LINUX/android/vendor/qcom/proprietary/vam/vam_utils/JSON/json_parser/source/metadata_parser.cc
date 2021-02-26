/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "metadata_parser.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
extern "C"{
#include "json_metadata.h"
}
using namespace std;


MetadataParser::MetadataParser() :
        filename_loaded(string("")),
        file_pos_(0),
        frames_loaded_(0),
        time_stamp_(0),
        fp_json_(0),
        fp_idx_(0),
        frame_cnt_(0),
        cur_record_id_(0),
        time_interval_(33)
{
    AllocateMetadataContent(&metadata_content_);
    metadata_content_.cnt_atomic_events = 0;
    metadata_content_.cnt_events = 0;
    metadata_content_.cnt_extensions = 0;
    metadata_content_.cnt_heat_maps = 0;
    metadata_content_.cnt_objects = 0;
    metadata_content_.cnt_object_trees = 0;
    metadata_frame_.camera_id.uuid = string(" ");

}
MetadataParser::~MetadataParser()
{
    ReleaseMetadataContent(&metadata_content_);
	if (metadata_frame_.heat_maps.size() > 0)
	{
		for (uint32_t i = 0; i < metadata_frame_.heat_maps.size(); i++)
            if (metadata_frame_.heat_maps[i].data)
                delete[]metadata_frame_.heat_maps[i].data;
	}
    if (fp_json_)
        fclose(fp_json_);
    if (fp_idx_)
        fclose(fp_idx_);

}
JSONVAStatus MetadataParser::LoadFileToString(const char *filename,
    std::string &contents)
{

    frames_loaded_ = 0;
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
JSONVAStatus MetadataParser::LoadIdxFile(const char *filename)
{
    int ret;
    fp_idx_ = fopen(filename, "r+t");
    if (fp_idx_ == NULL)
        return JSON_VA_END_OF_FILE;
//    int idx_cnt = 0;
    while (!feof(fp_idx_))
    {
        IdxFileRecord record;
        ret = fscanf(fp_idx_, "%llu %d %d", &record.time_stamp, &record.frame_id, &record.frame_size);
        if (ret != 3)
            break;
        idx_records.push_back(record);
    }
    fclose(fp_idx_);
    fp_idx_ = 0;
    cur_record_id_ = 0;
    return JSON_VA_OK;
}

JSONVAStatus MetadataParser::GetNextMetadataFrame(std::string & str)
{
    if (cur_record_id_ == 0 || cur_record_id_ == idx_records.size()-1){
        if (fp_json_)
            fclose(fp_json_);
        if (fp_idx_)
            fclose(fp_idx_);
        if (cur_record_id_ != 0)
            start_time_stamp_ = time_stamp_+time_interval_;
        cur_record_id_ = 0;
        //time_stamp_ = frame_cnt_;
        sprintf(filename_json_, "%s_%lld_%d.jsonx", file_prefix_, start_time_stamp_, frame_cnt_);
        sprintf(filename_idx_, "%s_%lld_%d.idx", file_prefix_, start_time_stamp_, frame_cnt_);
        fp_json_ = fopen(filename_json_, "r+t");
        if (fp_json_ == NULL)
            return JSON_VA_END_OF_FILE;
        fp_idx_ = fopen(filename_idx_, "r+t");
        if (fp_idx_ == NULL)
            return JSON_VA_END_OF_FILE;
        fclose(fp_idx_);
        fp_idx_ = 0;
        LoadIdxFile(filename_idx_);
    }
    if (idx_records.size() == 0 || idx_records.size() == cur_record_id_)
        return JSON_VA_END_OF_FILE;
    int size = idx_records[cur_record_id_].frame_size;
    char * buf = new char[size+1];
    int size2 = fread(buf, 1, size, fp_json_);
    buf[size2] = 0;
    str = string(buf);
    //printf("%s\n", buf);
    delete []buf;
    cur_record_id_++;
    frame_cnt_++;
    if (size2 != size)
        return JSON_VA_INVALID;

    return JSON_VA_OK;
}
template<typename T>
bool MetadataParser::NotInValidRange(T value, T minValue, T maxValue)
{
    if (value<minValue || value>maxValue)
        return true;
    else
        return false;
}
JSONVAStatus MetadataParser::ParseScene(const Json::Value scene,
    MetadataFrame & metadata)
{
    if (scene.isMember("heat_map")){
        Json::Value heat_map_node = scene["heat_map"];
        VAHeatMap heat_map;
        heat_map.cells_per_row = heat_map_node["cells_per_row"].asInt();
        heat_map.rows_per_column = heat_map_node["rows_per_column"].asInt();
        heat_map.start_time = heat_map_node["start_time"].asUInt64();
        heat_map.end_time = heat_map_node["end_time"].asUInt64();
        if (NotInValidRange<uint32_t>(heat_map.cells_per_row, 0, 100) ||
            NotInValidRange<uint32_t>(heat_map.rows_per_column, (uint32_t)0,
            (uint32_t)100))
            return JSON_VA_INVALID;
        Json::Value heat_map_data = heat_map_node["data"];
        heat_map.data =
            new uint16_t[heat_map.cells_per_row*heat_map.rows_per_column];
        Json::Value::iterator it;
        int i;
        for (it= heat_map_data.begin(),  i = 0; it != heat_map_data.end();
            ++it, i++){
            heat_map.data[i] = (*it).asInt();
        }
#ifdef DEBUG_
        for (i = 0; i<heat_map.cells_per_row*heat_map.rows_per_column; i++){
            cout<<heat_map.data[i] <<",  ";
        }
#endif
        metadata.heat_maps.push_back(heat_map);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ClearMetadata(MetadataFrame & metadata)
{
    metadata.camera_id.uuid = string(" ");
    metadata.atomic_events.clear();
    metadata.events.clear();
    metadata.extensions.clear();
    for (unsigned int i = 0; i < metadata.heat_maps.size(); i++){
        if (metadata.heat_maps[i].data){
            delete[]metadata.heat_maps[i].data;
            metadata.heat_maps[i].data = NULL;
        }
    }
    metadata.heat_maps.clear();
    metadata.objects.clear();
    metadata.object_trees.clear();
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseObjects(const Json::Value objects,
    MetadataFrame & metadata)
{
    Json::Value::iterator it;
    for (it = objects.begin(); it != objects.end(); ++it){
        VAObject object;
        object.engine_type = (*it)["engine_type"].asString();
        object.object_id = (*it)["object_id"].asInt();
        if ((*it).isMember("appearance"))
        {
            Json::Value appearance = (*it)["appearance"];
            object.appearance_descriptor.object_type =
                (MetadataCategory)appearance["object_type"].asInt();
            object.appearance_descriptor.object_type_confidence =
                appearance["object_type_confidence"].asInt();
            if (appearance.isMember("location"))
            {
                Json::Value location = appearance["location"];
                object.appearance_descriptor.location.x = location["x"].asInt();
                object.appearance_descriptor.location.y = location["y"].asInt();
                object.appearance_descriptor.location.width =
                    location["width"].asInt();
                object.appearance_descriptor.location.height =
                    location["height"].asInt();
            }
            if (object.engine_type == "object_tracker")
            {
                object.appearance_descriptor.object_tracker_appearance.
                    physical_width = appearance["physical_width"].asInt();
                object.appearance_descriptor.object_tracker_appearance.
                    physical_height = appearance["physical_height"].asInt();
                object.appearance_descriptor.object_tracker_appearance.
                    moving_dir = (MetadataDir)appearance["moving_dir"].asInt();
                object.appearance_descriptor.object_tracker_appearance.
                    moving_speed = appearance["moving_speed"].asFloat();
            }
        }
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("on_event"))
        {
            Json::Value on_event = (*it)["on_event"];
            object.on_event.status = on_event["status"].asInt();
            Json::Value::iterator it_events;
            for (it_events = on_event["events"].begin(); it_events !=
                on_event["events"].end(); it_events++)
            {
                struct metadata_uuid_t event_id;
                strcpy(event_id.uuid, (*it_events)["event"].asString().c_str());
                object.on_event.event_ids.push_back(event_id);
            }
        }
        else
            object.on_event.status = 0;
        for (int k = 0; k < 5; k++){
            object.reserve[k] = 0;
            object.reserve_str[k][0] = 0;
        }
        if ((*it).isMember("reserve"))
        {
            Json::Value::iterator it_reserve;
            Json::Value reserve = (*it)["reserve"];
            int k = 0;
            for (it_reserve = (*it)["reserve"].begin(); it_reserve !=
                (*it)["reserve"].end(); it_reserve++)
            {
                if (k >= 5)
                    break;
                object.reserve[k] = (*it_reserve).asInt64();
                k++;
            }
        }
        if ((*it).isMember("reserve_str"))
        {
            Json::Value::iterator it_reserveStr;
            Json::Value reserve_str = (*it)["reserve_str"];
            int k = 0;
            for (it_reserveStr = (*it)["reserve_str"].begin(); it_reserveStr !=
                (*it)["reserve_str"].end(); it_reserveStr++)
            {
                if (k >= 5)
                    break;
                strcpy(object.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
            }
        }
        metadata.objects.push_back(object);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseObjectTrees(const Json::Value object_trees,
    MetadataFrame & metadata)
{

    Json::Value::iterator it;
    for (it = object_trees.begin(); it != object_trees.end(); ++it)
    {
        VAObjectTree object_tree;
        if( (*it).isMember("merged"))
        {
            Json::Value merged = (*it)["merged"];
            object_tree.action = METADATA_OBJECT_TREE_MERGED;
            object_tree.to.push_back(merged["to"].asInt());
            Json::Value::iterator it_from;
            for (it_from = merged["from"].begin(); it_from !=
                merged["from"].end(); it_from++)
            {
                object_tree.from.push_back((*it_from).asInt());
            }
        }
        else if ((*it).isMember("split"))
        {
            Json::Value split = (*it)["split"];
            object_tree.action = METADATA_OBJECT_TREE_SPLIT;
            object_tree.from.push_back(split["from"].asInt());
            Json::Value::iterator it_to;
            for (it_to = split["to"].begin(); it_to != split["to"].end();
                it_to++)
            {
                object_tree.to.push_back((*it_to).asInt());
            }
        }
        else if ((*it).isMember("created"))
        {
            Json::Value created = (*it)["created"];
            object_tree.action = METADATA_OBJECT_TREE_CREATED;
            object_tree.to.push_back(created["to"].asInt());
        }
        else if ((*it).isMember("deleted"))
        {
            Json::Value deleted = (*it)["deleted"];
            object_tree.action = METADATA_OBJECT_TREE_DELETED;
            object_tree.from.push_back(deleted["from"].asInt());
        }
        else if ((*it).isMember("renamed"))
        {
            Json::Value renamed = (*it)["renamed"];
            object_tree.action = METADATA_OBJECT_TREE_RENAMED;
            object_tree.to.push_back(renamed["to"].asInt());
            object_tree.from.push_back(renamed["from"].asInt());
        }
        else
            return JSON_VA_INVALID;
        metadata.object_trees.push_back(object_tree);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseAtomicEvents(const Json::Value atomic_events,
    MetadataFrame & metadata)
{
    Json::Value::iterator it;
    for (it = atomic_events.begin(); it != atomic_events.end(); ++it){
        VAAtomicEvent atomic_event;
        atomic_event.event_id.uuid = (*it)["event_id"].asString();
        atomic_event.rule_id.uuid = (*it)["rule_id"].asString();
        atomic_event.event_type = (MetadataEventType)(*it)["event_type"].asInt();
        atomic_event.rule_name = (*it)["rule_name"].asString();
        if ((*it).isMember("start_time"))
            atomic_event.start_time = (*it)["start_time"].asUInt64();
        else
            atomic_event.start_time = metadata.time_stamp;
        if ((*it).isMember("event_details")){
            Json::Value event_details = (*it)["event_details"];
            switch (atomic_event.event_type){
            case METADATA_EVENT_CAMERA_TAMPER_DETECTED:
                atomic_event.event_details.details_camera_tamper_detected.
                    tamper_type =
                    (CameraTamperType)event_details["tamper_type"].asInt();
                break;
            case METADATA_EVENT_MOTION_DETECTED:
                atomic_event.event_details.details_motion_detected.
                    motion_activity =
                    (CameraTamperType)event_details["motion_activity"].asInt();
                atomic_event.event_details.details_motion_detected.object_id =
                    (CameraTamperType)event_details["object_id"].asInt();
                break;
            case METADATA_EVENT_INTRUSION_DETECTED:
            case METADATA_EVENT_LINECROSSED:
            case METADATA_EVENT_LOITERING_DETECTED:
            case METADATA_EVENT_OBJECT_DETECTED:
                atomic_event.event_details.details_object_trackor.object_id =
                    event_details["object_id"].asInt();
                break;
            case METADATA_EVENT_OBJECT_COUNTED:
                atomic_event.event_details.details_object_counted.count =
                    event_details["count"].asInt();
                break;
            case METADATA_EVENT_FACE_DETECTED:
                atomic_event.event_details.details_face_detected.object_id =
                    event_details["object_id"].asInt();
                break;
            case METADATA_EVENT_FACE_RECOGNIZED:
                atomic_event.event_details.details_face_recognized.group_name =
                    event_details["group_name"].asString();
                atomic_event.event_details.details_face_recognized.display_name =
                    event_details["display_name"].asString();
                atomic_event.event_details.details_face_recognized.object_id =
                    event_details["object_id"].asInt();
                atomic_event.event_details.details_face_recognized.person_id.uuid =
                    event_details["person_id"].asString();
                atomic_event.event_details.details_face_recognized.group_id.uuid =
                    event_details["group_id"].asString();
                break;
            case METADATA_EVENT_OBJECT_CLASSIFIED:
                atomic_event.event_details.details_object_trackor.object_id =
                    event_details["object_id"].asInt();
                break;
            default:
              break;
            }
        }
        for (int k = 0; k < 5; k++){
            atomic_event.reserve[k] = 0;
            atomic_event.reserve_str[k][0] = 0;
        }
        if ((*it).isMember("reserve"))
        {
            Json::Value::iterator it_reserve;
            Json::Value reserve = (*it)["reserve"];
            int k = 0;
            for (it_reserve = (*it)["reserve"].begin(); it_reserve !=
                (*it)["reserve"].end(); it_reserve++)
            {
                if (k >= 5)
                    break;
                atomic_event.reserve[k] = (*it_reserve).asInt64();
                k++;
            }
        }
        if ((*it).isMember("reserve_str"))
        {
            Json::Value::iterator it_reserveStr;
            Json::Value reserve_str = (*it)["reserve_str"];
            int k = 0;
            for (it_reserveStr = (*it)["reserve_str"].begin(); it_reserveStr !=
                (*it)["reserve_str"].end(); it_reserveStr++)
            {
                if (k >= 5)
                    break;
                strcpy(atomic_event.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
            }
        }
        metadata.atomic_events.push_back(atomic_event);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseEvents(const Json::Value events,
    MetadataFrame & metadata)
{
    Json::Value::iterator it;
    for (it = events.begin(); it != events.end(); ++it){
        VAEvent event;
        event.event_id.uuid = (*it)["event_id"].asString();
        event.rule_id.uuid = (*it)["rule_id"].asString();
        event.rule_name = (*it)["rule_name"].asString();
        if ((*it).isMember("start_time"))
            event.start_time = (*it)["start_time"].asUInt64();
        else
            event.start_time = metadata.time_stamp;
        Json::Value composite_event = (*it)["composite_event"];
        event.composite_event.sub_event_id1.uuid =
            composite_event["sub_event_id1"].asString();
        event.composite_event.num_sub_events = 1;
        if (composite_event.isMember("sub_event_id2")){
            event.composite_event.sub_event_id2.uuid =
                composite_event["sub_event_id2"].asString();
            event.composite_event.num_sub_events++;
        }
        for (int k = 0; k < 5; k++){
            event.reserve[k] = 0;
            event.reserve_str[k][0] = 0;
        }
        if ((*it).isMember("reserve"))
        {
            Json::Value::iterator it_reserve;
            Json::Value reserve = (*it)["reserve"];
            int k = 0;
            for (it_reserve = (*it)["reserve"].begin(); it_reserve !=
                (*it)["reserve"].end(); it_reserve++)
            {
                if (k >= 5)
                    break;
                event.reserve[k] = (*it_reserve).asInt64();
                k++;
            }
        }
        if ((*it).isMember("reserve_str"))
        {
            Json::Value::iterator it_reserveStr;
            Json::Value reserve_str = (*it)["reserve_str"];
            int k = 0;
            for (it_reserveStr = (*it)["reserve_str"].begin(); it_reserveStr !=
                (*it)["reserve_str"].end(); it_reserveStr++)
            {
                if (k >= 5)
                    break;
                strcpy(event.reserve_str[k], (*it_reserveStr).asString().c_str());
                k++;
            }
        }
        metadata.events.push_back(event);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseExtension(const Json::Value extensions,
    MetadataFrame & metadata)
{

    Json::Value::iterator it;
    for (it = extensions.begin(); it != extensions.end(); ++it){
        VAExtension extension;
        extension.engine_id = (*it)["engine_id"].asString();
        extension.data = (*it)["data"].asString();
        metadata.extensions.push_back(extension);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseDoc(const char* doc, MetadataFrame & metadata)
{
    JSONVAStatus res;
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    ClearMetadata(metadata);
    bool parsingSuccessful = reader.parse(doc, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse metadata\n"
            << reader.getFormattedErrorMessages();
        return JSON_VA_INVALID;
    }
    if (root.isMember("camera_id")){
        //printf("camera_id: %p\n", root["camera_id"].asString().c_str());
        metadata.camera_id.uuid = root["camera_id"].asString();
    }
    else
        metadata.camera_id.uuid = " ";
    if (root.isMember("time_stamp"))
        metadata.time_stamp = root["time_stamp"].asUInt64();
    else
        return JSON_VA_INVALID;
    if (root.isMember("scene")){
        const Json::Value scene = root["scene"];
        ParseScene(scene, metadata);
    }
    if (root.isMember("objects")){
        const Json::Value objects = root["objects"];
        res = ParseObjects(objects, metadata);
        if (res != JSON_VA_OK){
            printf("failed to parse objects in metadata parser\n");
            return res;
        }
    }
    if (root.isMember("object_tree")){
        const Json::Value object_trees = root["object_tree"];
        res = ParseObjectTrees(object_trees, metadata);
        if (res != JSON_VA_OK){
            printf("failed to parse object_tree in metadata parser\n");
            return res;
        }
    }
    if (root.isMember("atomic_events")){
        const Json::Value atomic_events = root["atomic_events"];
        res = ParseAtomicEvents(atomic_events, metadata);
        if (res != JSON_VA_OK){
            printf("failed to parse atomic_events in metadata parser\n");
            return res;
        }
    }
    if (root.isMember("events")){
        const Json::Value events = root["events"];
        res = ParseEvents(events, metadata);
        if (res != JSON_VA_OK){
            printf("failed to parse events in metadata parser\n");
            return res;
        }
    }
    if (root.isMember("extension")){
        const Json::Value extension = root["extension"];
        ParseExtension(extension, metadata);
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ParseDoc(const char* doc, vector<MetadataFrame> & metadata)
{
    Json::Value root;   // will contains the root value after parsing.
    Json::Value metadata_content;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(doc, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse metadata\n"
            << reader.getFormattedErrorMessages();
        return JSON_VA_INVALID;
    }
    Json::Value::iterator it;
    metadata_content = root["metadata_content"];
    for (it = metadata_content.begin(); it != metadata_content.end(); ++it){
        MetadataFrame metadata_frame;
        ClearMetadata(metadata_frame);
        if ((*it).isMember("camera_id"))
            metadata_frame.camera_id.uuid = (string)(*it)["camera_id"].asString();
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("time_stamp"))
            metadata_frame.time_stamp = (*it)["time_stamp"].asUInt64();
        else
            return JSON_VA_INVALID;
        if ((*it).isMember("scene")){
            const Json::Value scene = (*it)["scene"];
            ParseScene(scene, metadata_frame);
        }
        if ((*it).isMember("objects")){
            const Json::Value objects = (*it)["objects"];
            ParseObjects(objects, metadata_frame);
        }
        if ((*it).isMember("object_tree")){
            const Json::Value object_trees = (*it)["object_tree"];
            ParseObjectTrees(object_trees, metadata_frame);
        }
        if ((*it).isMember("atomic_events")){
            const Json::Value atomic_events = (*it)["atomic_events"];
            ParseAtomicEvents(atomic_events, metadata_frame);
        }
        if ((*it).isMember("events")){
            const Json::Value events = (*it)["events"];
            ParseEvents(events, metadata_frame);
        }
        if ((*it).isMember("extension")){
            const Json::Value extension = (*it)["extension"];
            ParseExtension(extension, metadata_frame);
        }
        metadata.push_back(metadata_frame);
    }
    return JSON_VA_OK;
}

JSONVAStatus MetadataParser::ParseMetadataFrame(const char* metadata_str, MetadataFrame & metadata_frame)
{
    Json::Value root;   // will contains the root value after parsing.
    Json::Value metadata_content;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(metadata_str, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse metadata\n"
            << reader.getFormattedErrorMessages();
        return JSON_VA_INVALID;
    }
    ClearMetadata(metadata_frame);
    if (root.isMember("camera_id"))
        metadata_frame.camera_id.uuid = (string)root["camera_id"].asString();
    else
        return JSON_VA_INVALID;
    if (root.isMember("time_stamp"))
        metadata_frame.time_stamp = root["time_stamp"].asUInt64();
    else
        return JSON_VA_INVALID;
    if (root.isMember("scene")){
        const Json::Value scene = root["scene"];
        ParseScene(scene, metadata_frame);
    }
    if (root.isMember("objects")){
        const Json::Value objects = root["objects"];
        ParseObjects(objects, metadata_frame);
    }
    if (root.isMember("object_tree")){
        const Json::Value object_trees = root["object_tree"];
        ParseObjectTrees(object_trees, metadata_frame);
    }
    if (root.isMember("atomic_events")){
        const Json::Value atomic_events = root["atomic_events"];
        ParseAtomicEvents(atomic_events, metadata_frame);
    }
    if (root.isMember("events")){
        const Json::Value events = root["events"];
        ParseEvents(events, metadata_frame);
    }
    if (root.isMember("extension")){
        const Json::Value extension = root["extension"];
        ParseExtension(extension, metadata_frame);
    }
    return JSON_VA_OK;
}

JSONVAStatus MetadataParser::ConvertMetadataFrameToMetadataContent(
    const MetadataFrame  metadata,
    struct metadata_content_t **metadata_content)
{
    struct metadata_content_t *metadata_content_new;
    metadata_content_new = &metadata_content_;
    *metadata_content = metadata_content_new;
    if (metadata_content_new->objects == 0)
    {
//        int flag = 1;
    }
	for (int i = 0; i < MD_EXTENSION_MAX; i++)
	{
		if (metadata_content_new->extensions[i].data){
			free(metadata_content_new->extensions[i].data);
			metadata_content_new->extensions[i].data = 0;
		}
	}
	for (int i = 0; i < MD_HEAT_MAP_MAX; i++)
	{
		if (metadata_content_new->heat_maps[i].data){
			free(metadata_content_new->heat_maps[i].data);
			metadata_content_new->heat_maps[i].data = 0;
		}
	}
    metadata_content_new->cnt_atomic_events = metadata.atomic_events.size();
    metadata_content_new->cnt_events = metadata.events.size();
    metadata_content_new->cnt_extensions = metadata.extensions.size();
    metadata_content_new->cnt_heat_maps = metadata.heat_maps.size();
    metadata_content_new->cnt_objects = metadata.objects.size();
    metadata_content_new->cnt_object_trees = metadata.object_trees.size();
    strcpy(metadata_content_new->camera_id.uuid, metadata.camera_id.uuid.c_str());
    metadata_content_new->time_stamp = metadata.time_stamp;
    for (int k = 0; k < 5; k++){
        metadata_content_new->reserve[k] = metadata.reserve[k];
        strcpy(metadata_content_new->reserve_str[k], metadata.reserve_str[k]);
    }
    if (metadata.atomic_events.size() > 0){
        for (uint32_t i = 0; i < metadata.atomic_events.size(); i++){
            metadata_content_new->atomic_events[i].start_time =
                metadata.atomic_events[i].start_time;
            strcpy(metadata_content_new->atomic_events[i].rule_id.uuid,
                metadata.atomic_events[i].rule_id.uuid.c_str());
            strcpy(metadata_content_new->atomic_events[i].event_id.uuid,
                metadata.atomic_events[i].event_id.uuid.c_str());
            metadata_content_new->atomic_events[i].event_type =
                (MetadataEventType)metadata.atomic_events[i].event_type;
            strcpy(metadata_content_new->atomic_events[i].rule_name,
                metadata.atomic_events[i].rule_name.c_str());
            for (int k = 0; k < 5; k++){
                metadata_content_new->atomic_events[i].reserve[k] = metadata.atomic_events[i].reserve[k];
                strcpy(metadata_content_new->atomic_events[i].reserve_str[k], metadata.atomic_events[i].reserve_str[k]);
            }

            switch (metadata.atomic_events[i].event_type){
            case METADATA_EVENT_CAMERA_TAMPER_DETECTED:
                metadata_content_new->atomic_events[i].event_details.
                    details_camera_tamper_detected.tamper_type =
                    (enum MetadataCameraTamperType)metadata.atomic_events[i].
                    event_details.details_camera_tamper_detected.tamper_type;
                break;
            case METADATA_EVENT_MOTION_DETECTED:
                metadata_content_new->atomic_events[i].event_details.
                    details_motion_detected.motion_activity =
                    metadata.atomic_events[i].event_details.
                    details_motion_detected.motion_activity;
                metadata_content_new->atomic_events[i].event_details.
                    details_motion_detected.object_id =
                    metadata.atomic_events[i].event_details.
                    details_motion_detected.object_id;
                break;
            case METADATA_EVENT_INTRUSION_DETECTED:
            case METADATA_EVENT_LINECROSSED:
            case METADATA_EVENT_LOITERING_DETECTED:
            case METADATA_EVENT_OBJECT_DETECTED:
                metadata_content_new->atomic_events[i].event_details.
                    details_object_trackor.object_id =
                    metadata.atomic_events[i].event_details.
                    details_object_trackor.object_id;
                break;
            case METADATA_EVENT_OBJECT_COUNTED:
                metadata_content_new->atomic_events[i].event_details.
                    details_object_counted.count =
                    metadata.atomic_events[i].event_details.
                    details_object_counted.count;
                break;
            case METADATA_EVENT_FACE_DETECTED:
                metadata_content_new->atomic_events[i].event_details.
                    details_face_detected.object_id =
                    metadata.atomic_events[i].event_details.
                    details_face_detected.object_id;
                break;
            case METADATA_EVENT_FACE_RECOGNIZED:
                strcpy(metadata_content_new->atomic_events[i].event_details.
                    details_face_recognized.group_name, metadata.
                    atomic_events[i].event_details.details_face_recognized.
                    group_name.c_str());
                strcpy(metadata_content_new->atomic_events[i].event_details.
                    details_face_recognized.display_name, metadata.
                    atomic_events[i].event_details.details_face_recognized.
                    display_name.c_str());
                metadata_content_new->atomic_events[i].event_details.
                    details_face_recognized.object_id =
                    metadata.atomic_events[i].event_details.
                    details_face_recognized.object_id;
                strcpy(metadata_content_new->atomic_events[i].event_details.
                    details_face_recognized.person_id.uuid,
                    metadata.atomic_events[i].event_details.
                    details_face_recognized.person_id.uuid.c_str());
                strcpy(metadata_content_new->atomic_events[i].event_details.
                    details_face_recognized.group_id.uuid, metadata.
                    atomic_events[i].event_details.	details_face_recognized.
                    group_id.uuid.c_str());
                break;
            case METADATA_EVENT_OBJECT_CLASSIFIED:
                metadata_content_new->atomic_events[i].event_details.
                    details_object_trackor.object_id = metadata.
                    atomic_events[i].event_details.details_object_trackor.
                    object_id;
                break;
            default:
                break;
            }
        }
    }
    if (metadata.events.size() > 0){
        for (uint32_t i = 0; i < metadata.events.size(); i++){
            metadata_content_new->events[i].start_time =
                metadata.events[i].start_time;
            metadata_content_new->events[i].composite_event.num_sub_events =
                metadata.events[i].composite_event.num_sub_events;
            strcpy(metadata_content_new->events[i].composite_event.
                sub_event_id1.uuid, metadata.events[i].composite_event.
                sub_event_id1.uuid.c_str());
            if (metadata.events[i].composite_event.num_sub_events > 1){
                strcpy(metadata_content_new->events[i].composite_event.
                    sub_event_id2.uuid, metadata.events[i].composite_event.
                    sub_event_id2.uuid.c_str());
            }
            strcpy(metadata_content_new->events[i].rule_name,
                metadata.events[i].rule_name.c_str());
            strcpy(metadata_content_new->events[i].event_id.uuid,
                metadata.events[i].event_id.uuid.c_str());
            strcpy(metadata_content_new->events[i].rule_id.uuid,
                metadata.events[i].rule_id.uuid.c_str());
            for (int k = 0; k < 5; k++){
                metadata_content_new->events[i].reserve[k] = metadata.events[i].reserve[k];
                strcpy(metadata_content_new->events[i].reserve_str[k], metadata.events[i].reserve_str[k]);
            }
        }
    }
    if (metadata.objects.size() > 0){
        for (uint32_t i = 0; i < metadata.objects.size(); i++){
            if (metadata_content_new->objects == 0)
            {
//                int flag = 1;
            }
            metadata_content_new->objects[i].object_id =
                metadata.objects[i].object_id;
            strcpy(metadata_content_new->objects[i].engine_type,
                metadata.objects[i].engine_type.c_str());
            metadata_content_new->objects[i].appearance_descriptor =
                metadata.objects[i].appearance_descriptor;
            metadata_content_new->objects[i].on_event.status =
                metadata.objects[i].on_event.status;
            metadata_content_new->objects[i].on_event.num_events =
                metadata.objects[i].on_event.event_ids.size();
            for (int k = 0; k < 5; k++){
                metadata_content_new->objects[i].reserve[k] = metadata.objects[i].reserve[k];
                strcpy(metadata_content_new->objects[i].reserve_str[k], metadata.objects[i].reserve_str[k]);
            }
            if (metadata_content_new->objects[i].on_event.num_events > 0){
                for (uint32_t k = 0;
                    k < metadata_content_new->objects[i].on_event.num_events;
                    k++){
                    strcpy(metadata_content_new->objects[i].on_event.event_ids[k].uuid,
                        metadata.objects[i].on_event.event_ids[k].uuid);
                }
            }
        }
    }
    if (metadata.object_trees.size() > 0){
        for (uint32_t i = 0; i < metadata.object_trees.size(); i++){
            metadata_content_new->object_trees[i].num_from =
                metadata.object_trees[i].from.size();
            metadata_content_new->object_trees[i].num_to =
                metadata.object_trees[i].to.size();
            metadata_content_new->object_trees[i].action =
                metadata.object_trees[i].action;
            for (uint32_t k = 0; k < metadata_content_new->object_trees[i].num_from;
                k++){
                metadata_content_new->object_trees[i].from[k] =
                    metadata.object_trees[i].from[k];
            }
            for (uint32_t k = 0; k < metadata_content_new->object_trees[i].num_to;
                k++){
                metadata_content_new->object_trees[i].to[k] =
                    metadata.object_trees[i].to[k];
            }

        }
    }
    if (metadata.heat_maps.size() > 0){
        for (uint32_t i = 0; i < metadata.heat_maps.size(); i++){
            metadata_content_new->heat_maps[i].cells_per_row =
                metadata.heat_maps[i].cells_per_row;
            metadata_content_new->heat_maps[i].rows_per_column =
                metadata.heat_maps[i].rows_per_column;
            metadata_content_new->heat_maps[i].start_time =
                metadata.heat_maps[i].start_time;
            metadata_content_new->heat_maps[i].end_time =
                metadata.heat_maps[i].end_time;
            if (metadata_content_new->heat_maps[i].data==0)
                metadata_content_new->heat_maps[i].data =
                (uint16_t *)malloc(metadata.heat_maps[i].cells_per_row *
                metadata.heat_maps[i].rows_per_column * sizeof(uint16_t));
            memcpy(metadata_content_new->heat_maps[i].data,
                metadata.heat_maps[i].data,
                metadata.heat_maps[i].cells_per_row *metadata.heat_maps[i].
                rows_per_column * sizeof(uint16_t));
        }
    }
    if (metadata.extensions.size() > 0){
        for (uint32_t i = 0; i < metadata.extensions.size(); i++){
            metadata_content_new->extensions[i].size_data =
                metadata.extensions[i].data.length()+1;
            strcpy(metadata_content_new->extensions[i].engine_id,
                metadata.extensions[i].engine_id.c_str());
            if (metadata_content_new->extensions[i].data){
                free(metadata_content_new->extensions[i].data);
            }
            metadata_content_new->extensions[i].data =
                (char*)malloc(metadata_content_new->extensions[i].size_data);

            strcpy(metadata_content_new->extensions[i].data,
                metadata.extensions[i].data.c_str());
        }
    }
    return JSON_VA_OK;
}

JSONVAStatus MetadataParser::ConvertMetadataContentToMetadataFrame(
    const struct metadata_content_t *metadata_content,
    MetadataFrame &metadata)
{
    metadata.camera_id.uuid = string(metadata_content->camera_id.uuid);
    metadata.time_stamp = metadata_content->time_stamp;
    if (metadata_content->cnt_atomic_events > 0){
        for (uint32_t i = 0; i <metadata_content->cnt_atomic_events; i++){
            VAAtomicEvent atomic_event;
            atomic_event.start_time =
                metadata_content->atomic_events[i].start_time;
            atomic_event.rule_id.uuid =
                string(metadata_content->atomic_events[i].rule_id.uuid);
            atomic_event.event_id.uuid =
                string(metadata_content->atomic_events[i].event_id.uuid);

            atomic_event.event_type =
                (MetadataEventType)metadata_content->atomic_events[i].event_type;
            atomic_event.rule_name =
                string(metadata_content->atomic_events[i].rule_name);

            switch (metadata_content->atomic_events[i].event_type){
            case METADATA_EVENT_CAMERA_TAMPER_DETECTED:
                atomic_event.event_details.details_camera_tamper_detected.
                    tamper_type =
                    (enum CameraTamperType)metadata_content->atomic_events[i].
                    event_details.details_camera_tamper_detected.tamper_type;
                break;
            case METADATA_EVENT_MOTION_DETECTED:
                atomic_event.event_details.details_motion_detected.
                    motion_activity =
                    metadata_content->atomic_events[i].event_details.
                    details_motion_detected.motion_activity;
                atomic_event.event_details.details_motion_detected.
                    object_id=
                    metadata_content->atomic_events[i].event_details.
                    details_motion_detected.object_id;
                break;
            case METADATA_EVENT_INTRUSION_DETECTED:
            case METADATA_EVENT_LINECROSSED:
            case METADATA_EVENT_LOITERING_DETECTED:
            case METADATA_EVENT_OBJECT_DETECTED:
                atomic_event.event_details.
                    details_object_trackor.object_id =
                metadata_content->atomic_events[i].event_details.
                    details_object_trackor.object_id;
                break;
            case METADATA_EVENT_OBJECT_COUNTED:
                atomic_event.event_details.details_object_counted.count =
                    metadata_content->atomic_events[i].event_details.
                    details_object_counted.count;
                break;
            case METADATA_EVENT_FACE_DETECTED:
                atomic_event.event_details.details_face_detected.object_id =
                    metadata_content->atomic_events[i].event_details.
                    details_face_detected.object_id;
                break;
            case METADATA_EVENT_FACE_RECOGNIZED:
                atomic_event.event_details.details_face_recognized.group_name =
                    string(metadata_content->atomic_events[i].event_details.
                    details_face_recognized.group_name);

                atomic_event.event_details.details_face_recognized.
                    display_name = string(metadata_content->atomic_events[i].
                    event_details.details_face_recognized.display_name);
                atomic_event.event_details.details_face_recognized.object_id =
                    metadata_content->atomic_events[i].event_details.
                    details_face_recognized.object_id;
                atomic_event.event_details.details_face_recognized.
                    person_id.uuid =
                    string(metadata_content->atomic_events[i].event_details.
                    details_face_recognized.person_id.uuid);
                atomic_event.event_details.details_face_recognized.
                    group_id.uuid = string(metadata_content->atomic_events[i].
                    event_details.details_face_recognized.group_id.uuid) ;
                break;
            case METADATA_EVENT_OBJECT_CLASSIFIED:
                atomic_event.event_details.details_object_trackor.object_id =
                    metadata_content->atomic_events[i].event_details.
                    details_object_trackor.object_id ;
                break;
            default:
                break;
            }
            metadata.atomic_events.push_back(atomic_event);
        }
    }
    if (metadata_content->cnt_events > 0){
        VAEvent event;
        for (uint32_t i = 0; i <metadata_content->cnt_events; i++){
            event.start_time =
                metadata_content->events[i].start_time;
            event.composite_event.num_sub_events =
                metadata_content->events[i].composite_event.num_sub_events;
            event.composite_event.sub_event_id1.uuid =
                string(metadata_content->events[i].composite_event.
                sub_event_id1.uuid);
            if (metadata_content->events[i].composite_event.num_sub_events >1){
                event.composite_event.sub_event_id2.uuid =
                string(metadata_content->events[i].composite_event.
                sub_event_id2.uuid);
            }
            event.rule_name = string(metadata_content->events[i].rule_name);
            event.event_id.uuid =
                string(metadata_content->events[i].event_id.uuid);
            event.rule_id.uuid =
                string(metadata_content->events[i].rule_id.uuid);
            metadata.events.push_back(event);
        }
    }
    if (metadata_content->cnt_objects > 0){
        for (uint32_t i = 0; i < metadata_content->cnt_objects; i++){
            VAObject object;
            object.object_id = metadata_content->objects[i].object_id;
            object.engine_type =
                string(metadata_content->objects[i].engine_type);
            object.appearance_descriptor =
                metadata_content->objects[i].appearance_descriptor;
            object.on_event.status =
                metadata_content->objects[i].on_event.status;
            if (metadata_content->objects[i].on_event.num_events > 0){

                for (uint32_t k = 0; k < metadata_content->objects[i].on_event.num_events; k++){
                    object.on_event.event_ids.push_back(metadata_content->objects[i].on_event.event_ids[k]);
                }
            }
            metadata.objects.push_back(object);
        }
    }
    if (metadata_content->cnt_object_trees > 0){
        for (uint32_t i = 0; i < metadata_content->cnt_object_trees; i++){
            VAObjectTree object_tree;
            object_tree.action = metadata_content->object_trees[i].action;
            for (uint32_t k = 0; k < metadata_content->object_trees[i].num_from; k++)
            {
                object_tree.from.push_back( metadata_content->object_trees[i].from[k]);
            }
            for (uint32_t k = 0; k < metadata_content->object_trees[i].num_to; k++){
                object_tree.to.push_back(metadata_content->object_trees[i].to[k]);
            }
            metadata.object_trees.push_back(object_tree);
        }
    }
    if (metadata_content->cnt_heat_maps > 0){
        for (uint32_t i = 0; i < metadata_content->cnt_heat_maps; i++){
            VAHeatMap heat_map;
            heat_map.cells_per_row =
                metadata_content->heat_maps[i].cells_per_row;
            heat_map.rows_per_column =
                metadata_content->heat_maps[i].rows_per_column;
            heat_map.data = (uint16_t *)malloc(heat_map.cells_per_row *
                heat_map.rows_per_column * sizeof(uint16_t));
            heat_map.start_time = metadata_content->heat_maps[i].start_time;
            heat_map.end_time = metadata_content->heat_maps[i].end_time;
            memcpy(heat_map.data, metadata_content->heat_maps[i].data,
                heat_map.cells_per_row * heat_map.rows_per_column *
                sizeof(uint16_t));
            metadata.heat_maps.push_back(heat_map);
        }
    }
    if (metadata_content->cnt_extensions > 0){
        for (uint32_t i = 0; i < metadata_content->cnt_extensions; i++){
            VAExtension extension;
            extension.engine_id = string(metadata_content->extensions[i].engine_id);
            extension.data = string(metadata_content->extensions[i].data);
            metadata.extensions.push_back(extension);
        }
    }
    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::AllocateMetadataContent(
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
JSONVAStatus MetadataParser::CopyMetadataContent(
    const struct metadata_content_t metadata_content_src,
    struct metadata_content_t *metadata_content_dst)
{

    return JSON_VA_OK;
}
JSONVAStatus MetadataParser::ReleaseMetadataContent(
struct metadata_content_t *metadata_content)
{
    for (int i = 0; i < MD_HEAT_MAP_MAX; i++){
        if (metadata_content->heat_maps[i].data)
            free(metadata_content->heat_maps[i].data);
    }
    for (int i = 0; i < MD_EXTENSION_MAX; i++){
        if (metadata_content->extensions[i].data)
            free(metadata_content->extensions[i].data);
    }
    free(metadata_content->atomic_events);
    free(metadata_content->events);
    free(metadata_content->extensions);
    free(metadata_content->heat_maps);
    free(metadata_content->objects);
    free(metadata_content->object_trees);
    return JSON_VA_OK;
}

JSONVAStatus MetadataParser::SeekMetadataPos(uint64_t time_stamp)
{
    bool found = false;
    int idx = -1;
    for (uint32_t i = 0; i < idx_records.size(); i++)
    {
        if (time_stamp == idx_records[i].time_stamp){
            found = true;
            idx = i;
            break;
        }
        else if (time_stamp < idx_records[i].time_stamp){
            return JSON_VA_INVALID;
        }
    }
    if (found){
        long size = 0;
        for (int i = 0; i < idx; i++){
            size += idx_records[i].frame_size;
        }
        cur_record_id_ = idx;
        if(fseek(fp_json_, size, SEEK_CUR))
            return JSON_VA_INVALID;
        return JSON_VA_OK;
    }
    else
        return JSON_VA_INVALID;
}
