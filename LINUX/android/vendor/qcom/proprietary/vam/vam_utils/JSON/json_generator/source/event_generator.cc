/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "event_generator.h"
JsonEventGenerator::JsonEventGenerator(uint64_t start_time, uint32_t period) :
start_time_(start_time), period_(period), fp_json_(0), fp_idx_(0)
{
    events_.clear();
    doc_str_ = 0;
}
JsonEventGenerator::~JsonEventGenerator()
{
   // events_.clear();
}
int JsonEventGenerator::AddEvent(struct json_event event)
{
    struct json_event event2 = event;
    events_.push_back(event2);
    return 0;
}
int JsonEventGenerator::EventToJSON(struct json_event event, Json::Value &event_json)
{
    Json::Value json_event(Json::objectValue);
    Json::Value event_base(Json::objectValue);
    Json::Value event_body(Json::objectValue);
    Json::Value position(Json::objectValue);
    Json::Value line(Json::objectValue);
    Json::Value zone(Json::objectValue);
    Json::Value point(Json::objectValue);
    Json::Value vertices(Json::arrayValue);
    Json::Value data(Json::arrayValue);
    event_base["event_id"] = event.base.event_id;
    event_base["event_type"] = (int)event.base.event_type;
    event_base["event_time"] = event.base.event_time;
    event_base["priority"] = event.base.priority;
    event_base["rule_id"] = event.base.rule_id;
    event_base["rule_name"] = event.base.rule_name;
    switch (event.base.event_type)
    {
    case JSON_EVENT_CAMERA_TAMPER_DETECTED:
        event_body["confidence"] = event.body.tamper.confidence;
        event_body["tamper_type"] = event.body.tamper.tamper_type;
        event_body["duration"] = event.body.tamper.duration;
        break;
    case JSON_EVENT_MOTION_DETECTED:
        break;
    case JSON_EVENT_LINECROSSED:
        event_body["object_id"] = event.body.line_crossed.object_id;
        position["x"] = event.body.line_crossed.x;
        position["y"] = event.body.line_crossed.y;
        position["width"] = event.body.line_crossed.width;
        position["height"] = event.body.line_crossed.height;
        event_body["position"] = position;
        if (event.body.line_crossed.cnt_zone_vertices > 1){
            line["line_id"] = event.body.line_crossed.zone_id;
            line["direction"] = event.body.line_crossed.dir;
            for (uint32_t i = 0; i < event.body.line_crossed.cnt_zone_vertices; i++){
                point["x"] = event.body.line_crossed.points[i].x;
                point["y"] = event.body.line_crossed.points[i].y;
                vertices.append(point);
            }
            line["vertices"] = vertices;
            event_body["line"] = line;
        }
        break;
    case JSON_EVENT_INTRUSION_DETECTED:
        event_body["object_id"] = event.body.intrusion_detected.object_id;
        position["x"] = event.body.intrusion_detected.x;
        position["y"] = event.body.intrusion_detected.y;
        position["width"] = event.body.intrusion_detected.width;
        position["height"] = event.body.intrusion_detected.height;
        event_body["position"] = position;
        if (event.body.intrusion_detected.cnt_zone_vertices > 2){
            zone["zone_id"] = event.body.intrusion_detected.zone_id;
            for (uint32_t i = 0; i < event.body.intrusion_detected.cnt_zone_vertices; i++){
                point["x"] = event.body.intrusion_detected.points[i].x;
                point["y"] = event.body.intrusion_detected.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_OBJECT_COUNTED:
        event_body["count"] = event.body.object_counted.count;
        event_body["object_id"] = event.body.object_counted.object_id;
        position["x"] = event.body.object_counted.x;
        position["y"] = event.body.object_counted.y;
        position["width"] = event.body.object_counted.width;
        position["height"] = event.body.object_counted.height;
        event_body["position"] = position;
        if (event.body.object_counted.cnt_zone_vertices > 1){
            line["line_id"] = event.body.object_counted.zone_id;
            line["direction"] = event.body.object_counted.dir;
            for (uint32_t i = 0; i < event.body.object_counted.cnt_zone_vertices; i++){
                point["x"] = event.body.object_counted.points[i].x;
                point["y"] = event.body.object_counted.points[i].y;
                vertices.append(point);
            }
            line["vertices"] = vertices;
            event_body["line"] = line;
        }
        break;
    case JSON_EVENT_LOITERING_DETECTED:
        event_body["object_id"] = event.body.loitering_detected.object_id;
        event_body["duration"] = event.body.loitering_detected.duration;
        position["x"] = event.body.loitering_detected.x;
        position["y"] = event.body.loitering_detected.y;
        position["width"] = event.body.loitering_detected.width;
        position["height"] = event.body.loitering_detected.height;
        event_body["position"] = position;
        if (event.body.loitering_detected.cnt_zone_vertices > 2){
            zone["zone_id"] = event.body.loitering_detected.zone_id;
            for (uint32_t i = 0; i < event.body.loitering_detected.cnt_zone_vertices; i++){
                point["x"] = event.body.loitering_detected.points[i].x;
                point["y"] = event.body.loitering_detected.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_FACE_DETECTED:
        event_body["object_id"] = event.body.face_detected.object_id;
        event_body["confidence"] = event.body.face_detected.confidence;
        position["x"] = event.body.face_detected.x;
        position["y"] = event.body.face_detected.y;
        position["width"] = event.body.face_detected.width;
        position["height"] = event.body.face_detected.height;
        event_body["position"] = position;
        if (event.body.face_detected.cnt_zone_vertices > 2){
            zone["zone_id"] = event.body.face_detected.zone_id;
            for (uint32_t i = 0; i < event.body.face_detected.cnt_zone_vertices; i++){
                point["x"] = event.body.face_detected.points[i].x;
                point["y"] = event.body.face_detected.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_FACE_RECOGNIZED:
        event_body["object_id"] = event.body.face_recognized.object_id;
        event_body["confidence"] = event.body.face_recognized.confidence;
        event_body["group_id"] = event.body.face_recognized.group_id;
        event_body["person_id"] = event.body.face_recognized.person_id;
        event_body["person_name"] = event.body.face_recognized.person_name;
        event_body["duration"] = event.body.face_recognized.duration;
        position["x"] = event.body.face_recognized.x;
        position["y"] = event.body.face_recognized.y;
        position["width"] = event.body.face_recognized.width;
        position["height"] = event.body.face_recognized.height;
        event_body["position"] = position;
        if (event.body.face_recognized.cnt_zone_vertices > 2){
            zone["zone_id"] = event.body.face_recognized.zone_id;
            for (uint32_t i = 0; i < event.body.face_recognized.cnt_zone_vertices; i++){
                point["x"] = event.body.face_recognized.points[i].x;
                point["y"] = event.body.face_recognized.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_OBJECT_CLASSIFIED:
        event_body["object_id"] = event.body.object_classified.object_id;
        event_body["confidence"] = event.body.object_classified.confidence;
        event_body["type"] = event.body.object_classified.type;
        position["x"] = event.body.object_classified.x;
        position["y"] = event.body.object_classified.y;
        position["width"] = event.body.object_classified.width;
        position["height"] = event.body.object_classified.height;
        event_body["position"] = position;
        zone["zone_id"] = event.body.object_classified.zone_id;
        if (event.body.object_classified.cnt_zone_vertices > 2){
            for (uint32_t i = 0; i < event.body.object_classified.cnt_zone_vertices; i++){
                point["x"] = event.body.object_classified.points[i].x;
                point["y"] = event.body.object_classified.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_OBJECT_DETECTED:
        event_body["object_id"] = event.body.object_detected.object_id;
        position["x"] = event.body.object_detected.x;
        position["y"] = event.body.object_detected.y;
        position["width"] = event.body.object_detected.width;
        position["height"] = event.body.object_detected.height;
        event_body["position"] = position;
        if (event.body.object_detected.cnt_zone_vertices > 2){
            zone["zone_id"] = event.body.object_detected.zone_id;
            for (uint32_t i = 0; i < event.body.object_detected.cnt_zone_vertices; i++){
                point["x"] = event.body.object_detected.points[i].x;
                point["y"] = event.body.object_detected.points[i].y;
                vertices.append(point);
            }
            zone["vertices"] = vertices;
            event_body["zone"] = zone;
        }
        break;
    case JSON_EVENT_COMPOUND_EVENT:
        event_body["sub_event_id1"] = event.body.compound_event.sub_event_id1;
        event_body["sub_event_id2"] = event.body.compound_event.sub_event_id1;
        event_body["operator"] = event.body.compound_event.operation;
        break;
    //case JSON_EVENT_ABANDONED_OBJECT_DETECTED:
    //    event_body["object_id"] = event.body.abandoned_object_detected.object_id;
    //    event_body["duration"] = event.body.abandoned_object_detected.duration;
    //    position["x"] = event.body.abandoned_object_detected.x;
    //    position["y"] = event.body.abandoned_object_detected.y;
    //    position["width"] = event.body.abandoned_object_detected.width;
    //    position["height"] = event.body.abandoned_object_detected.height;
    //    event_body["position"] = position;
    //    if (event.body.abandoned_object_detected.cnt_zone_vertices > 2){
    //        zone["zone_id"] = event.body.abandoned_object_detected.zone_id;
    //        for (uint32_t i = 0; i < event.body.abandoned_object_detected.cnt_zone_vertices; i++){
    //            point["x"] = event.body.abandoned_object_detected.points[i].x;
    //            point["y"] = event.body.abandoned_object_detected.points[i].y;
    //            vertices.append(point);
    //        }
    //        zone["vertices"] = vertices;
    //        event_body["zone"] = zone;
    //    }
    //    break;
    //case JSON_EVENT_HEATMAP_GENERATED:
    //    event_body["start_time"] = event.body.heat_map.start_time;
    //    event_body["end_time"] = event.body.heat_map.end_time;
    //    event_body["cells_per_row"] = event.body.heat_map.cells_per_row;
    //    event_body["rows_per_column"] = event.body.heat_map.rows_per_column;
    //    for (uint32_t i = 0; i < event.body.heat_map.cells_per_row*event.body.heat_map.rows_per_column; i++){
    //        data.append(event.body.heat_map.data[i]);
    //    }
    //    event_body["data"] = data;
    //    break;
    case JSON_EVENT_EXTENSION_RESERVED:
        event_body["size"] = event.body.extension.size;
        event_body["payload"] = event.body.extension.payload;
        break;
    default:
        break;
    }
    json_event["event_base"] = event_base;
    json_event["event_body"] = event_body;
    int flag_reserve = 0;
    int flag_reserveStr = 0;
    for (int k = 0; k < 5; k++){
        if (event.reserve[k] != 0)
            flag_reserve++;
        if (event.reserve_str[k][0] != 0)
            flag_reserveStr++;
    }
    if (flag_reserve){
        Json::Value reserve(Json::arrayValue);
        for (int k = 0; k < 5; k++){
            reserve.append(event.reserve[k]);
        }
        json_event["reserve"] = reserve;
    }
    if (flag_reserveStr){
        Json::Value reserve_str(Json::arrayValue);
        for (int k = 0; k < 5; k++){
            reserve_str.append(event.reserve_str[k]);
        }
        json_event["reserve_str"] = reserve_str;
    }
    event_json = json_event;
    return 0;
}
int JsonEventGenerator::EventsToJSON()
{
    Json::Value events_json(Json::objectValue);
    Json::Value events_json_internal(Json::arrayValue);
    Json::Value event;
    for (size_t i = 0; i < events_.size(); i++){
        EventToJSON(events_[i], event);
        events_json_internal.append(event);
    }
    events_json["events"] = events_json_internal;

    Json::FastWriter writer;
    //Json::StyledWriter writer;
    events_buf_ = writer.write(events_json);
    doc_str_ = (char *)events_buf_.c_str();
    Clear();
    return 0;
}
int JsonEventGenerator::Clear()
{
    events_.clear();
    return 0;
}
