/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "event_parser.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "json_event.h"
using namespace std;
JSONEventParser::JSONEventParser(const char * stream_locator,
                                 uint64_t start_time,
                                 uint64_t end_time, uint32_t period)
      : fp_idx_(0),
        fp_event_(0),
        cur_record_id_(0),
        events_(0),
        start_time_stamp_(start_time),
        end_time_stamp_(end_time),
        period_(period),
        file_pos_(0)
{
    strcpy(stream_locator_, stream_locator);
}
JSONEventParser::~JSONEventParser()
{
    for (uint32_t i = 0; i < events_vec_.size(); i++){
        switch (events_vec_[i].base.event_type){
        //case JSON_EVENT_HEATMAP_GENERATED:
        //    if (events_vec_[i].body.heat_map.data){
        //        delete[]events_vec_[i].body.heat_map.data;
        //    }
        //    break;
        case JSON_EVENT_EXTENSION_RESERVED:
            if (events_vec_[i].body.extension.payload){
                delete[]events_vec_[i].body.extension.payload;
            }
            break;
        default:
            break;
        }
    }
    events_vec_.clear();
    if (events_)
        delete events_;
}

int JSONEventParser::LoadIdxFile(const char *filename)
{
    fp_idx_ = fopen(filename, "r+t");
    if (fp_idx_ == NULL)
        return 2; //end of file
    while (!feof(fp_idx_))
    {
        EventIdxFileRecord record;
        fscanf(fp_idx_, "%llu %d %d", &record.time_stamp, &record.frame_id, &record.frame_size);
        idx_records_.push_back(record);
    }
    fclose(fp_idx_);
    fp_idx_ = 0;
    cur_record_id_ = 0;
    return 0;
}
int JSONEventParser::LoadFileToString(const char *filename,
    std::string &contents)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in){
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return 0;
    }
    else
        return 1;
}

int JSONEventParser::LoadNextJsonEvents(std::string &events_str)
{
    while (cur_record_id_ == 0 || cur_record_id_ == idx_records_.size() ){
        if (fp_event_)
            fclose(fp_event_);
        if (fp_idx_)
            fclose(fp_idx_);
        cur_record_id_ = 0;
        //time_stamp_ = frame_cnt_;
        char filename_json[256];
        char filename_idx[256];
        file_id_ = start_time_stamp_ - (start_time_stamp_%(period_*1000));
        sprintf(filename_json, "%s_%llu.jsonx", stream_locator_, file_id_);
        sprintf(filename_idx, "%s_%llu.idx", stream_locator_, file_id_);
         if (start_time_stamp_ > end_time_stamp_ + period_*1000){
            return 2;//end of events files
        }
        start_time_stamp_ += period_*1000;
        fp_event_ = fopen(filename_json, "r+t");
        if (fp_event_ == NULL)
            continue;//no file
        fp_idx_ = fopen(filename_idx, "r+t");
        if (fp_idx_ == NULL){
            fclose(fp_event_);
            continue;//no file
        }

        fclose(fp_idx_);
        fp_idx_ = 0;
        idx_records_.clear();
        LoadIdxFile(filename_idx);
        cur_record_id_ = 0;
        int size = idx_records_[cur_record_id_].frame_size;
        frame_id_ = idx_records_[cur_record_id_].frame_id;
        char * buf = new char[size + 1];
        int size2 = fread(buf, 1, size, fp_event_);
        buf[size2] = 0;
        events_str = string(buf);
       // printf("%d,%d\n%s\n", size,size2, buf);
        delete[]buf;
        if (size2 != size)
            return 1; //error happened
        else{
            cur_record_id_++;
            return 0;
        }
    }
    int size = idx_records_[cur_record_id_].frame_size;
    frame_id_ = idx_records_[cur_record_id_].frame_id;
    char * buf = new char[size + 1];
    int size2 = fread(buf, 1, size, fp_event_);
    buf[size2] = 0;
    events_str = string(buf);
    delete[]buf;
    cur_record_id_++;
    if (size2 != size)
        return 1;

    return 0;
}
int JSONEventParser::ParseEvents(const char* events_str)
{
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    //clear events_vec_ and events_ before parsing new events string
    for (uint32_t i = 0; i < events_vec_.size(); i++){
        switch (events_vec_[i].base.event_type){
 /*       case JSON_EVENT_HEATMAP_GENERATED:
            if (events_vec_[i].body.heat_map.data){
                delete[]events_vec_[i].body.heat_map.data;
            }
            break;*/
        case JSON_EVENT_EXTENSION_RESERVED:
            if (events_vec_[i].body.extension.payload){
                delete[]events_vec_[i].body.extension.payload;
            }
            break;
        default:
            break;
        }
    }
    events_vec_.clear();
    if (events_)
        delete events_;
    event_cnt_ = 0;

    bool parsingSuccessful = reader.parse(events_str, root);
    if (!parsingSuccessful){
        // report to the user the failure and their locations in the document.
        std::cout << "Failed to parse events\n"
            << reader.getFormattedErrorMessages();
        return 1;
    }
    if (root.isMember("events")){
        const Json::Value events = root["events"];

        Json::Value::iterator it;
        for (it = events.begin(); it != events.end(); ++it){
            struct json_event event;
            memset(&event, 0, sizeof(struct json_event));
            if ((*it).isMember("event_base")){
                const Json::Value event_base = (*it)["event_base"];
                strcpy(event.base.event_id, event_base["event_id"].asString().c_str());
                strcpy(event.base.rule_id, event_base["rule_id"].asString().c_str());
                strcpy(event.base.rule_name, event_base["rule_name"].asString().c_str());
                event.base.event_time = event_base["event_time"].asUInt64();
                event.base.event_type = (JSONEventType)event_base["event_type"].asInt();
                event.base.priority = event_base["priority"].asInt();
            }
            else
                return 1;
            if ((*it).isMember("event_body")){
                const Json::Value event_body = (*it)["event_body"];
                switch (event.base.event_type){
                case JSON_EVENT_CAMERA_TAMPER_DETECTED:
                    event.body.tamper.tamper_type = event_body["tamper_type"].asInt();
                    event.body.tamper.duration = event_body["duration"].asInt();
                    event.body.tamper.confidence = event_body["confidence"].asInt();
                    break;
                case JSON_EVENT_LINECROSSED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.line_crossed.x = position["x"].asInt();
                        event.body.line_crossed.y = position["y"].asInt();
                        event.body.line_crossed.width = position["width"].asInt();
                        event.body.line_crossed.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    if (event_body.isMember("line")){
                        const Json::Value line = event_body["line"];
                        event.body.line_crossed.dir = line["direction"].asInt();
                        strcpy(event.body.line_crossed.zone_id, line["line_id"].asString().c_str());
                        if (line.isMember("vertices")){
                            const Json::Value vertices = line["vertices"];
                            Json::Value::iterator it_vtx;
                            event.body.line_crossed.cnt_zone_vertices = 0;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.line_crossed.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.line_crossed.points[event.body.line_crossed.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.line_crossed.points[event.body.line_crossed.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.line_crossed.dir = event_body["direction"].asInt();
                    event.body.line_crossed.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_OBJECT_COUNTED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.object_counted.x = position["x"].asInt();
                        event.body.object_counted.y = position["y"].asInt();
                        event.body.object_counted.width = position["width"].asInt();
                        event.body.object_counted.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    if (event_body.isMember("line")){
                        const Json::Value line = event_body["line"];
                        event.body.object_counted.dir = line["direction"].asInt();
                        strcpy(event.body.object_counted.zone_id, line["line_id"].asString().c_str());
                        if (line.isMember("vertices")){
                            const Json::Value vertices = line["vertices"];
                            Json::Value::iterator it_vtx;
                            event.body.object_counted.cnt_zone_vertices = 0;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.object_counted.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.object_counted.points[event.body.object_counted.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.object_counted.points[event.body.object_counted.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.object_counted.count = event_body["count"].asInt();
                    event.body.object_counted.dir = event_body["direction"].asInt();
                    event.body.object_counted.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_INTRUSION_DETECTED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.intrusion_detected.x = position["x"].asInt();
                        event.body.intrusion_detected.y = position["y"].asInt();
                        event.body.intrusion_detected.width = position["width"].asInt();
                        event.body.intrusion_detected.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.intrusion_detected.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            event.body.intrusion_detected.cnt_zone_vertices = 0;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.intrusion_detected.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.intrusion_detected.points[event.body.intrusion_detected.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.intrusion_detected.points[event.body.intrusion_detected.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.intrusion_detected.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_LOITERING_DETECTED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.loitering_detected.x = position["x"].asInt();
                        event.body.loitering_detected.y = position["y"].asInt();
                        event.body.loitering_detected.width = position["width"].asInt();
                        event.body.loitering_detected.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.loitering_detected.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            event.body.loitering_detected.cnt_zone_vertices = 0;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.loitering_detected.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.loitering_detected.points[event.body.loitering_detected.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.loitering_detected.points[event.body.loitering_detected.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.loitering_detected.duration = event_body["duration"].asInt();
                    event.body.loitering_detected.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_FACE_DETECTED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.face_detected.x = position["x"].asInt();
                        event.body.face_detected.y = position["y"].asInt();
                        event.body.face_detected.width = position["width"].asInt();
                        event.body.face_detected.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    event.body.face_detected.cnt_zone_vertices = 0;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.face_detected.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.face_detected.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.face_detected.points[event.body.face_detected.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.face_detected.points[event.body.face_detected.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.face_detected.confidence = event_body["confidence"].asInt();
                    event.body.face_detected.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_FACE_RECOGNIZED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.face_recognized.x = position["x"].asInt();
                        event.body.face_recognized.y = position["y"].asInt();
                        event.body.face_recognized.width = position["width"].asInt();
                        event.body.face_recognized.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    event.body.face_recognized.cnt_zone_vertices = 0;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.face_recognized.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.face_recognized.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.face_recognized.points[event.body.face_recognized.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.face_recognized.points[event.body.face_recognized.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.face_recognized.confidence = event_body["confidence"].asInt();
                    event.body.face_recognized.duration = event_body["duration"].asInt();
                    event.body.face_recognized.object_id = event_body["object_id"].asInt();
                    strcpy(event.body.face_recognized.group_id, event_body["group_id"].asString().c_str());
                    strcpy(event.body.face_recognized.person_id, event_body["person_id"].asString().c_str());
                    strcpy(event.body.face_recognized.person_name, event_body["person_name"].asString().c_str());
                    break;
                case  JSON_EVENT_OBJECT_CLASSIFIED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.object_classified.x = position["x"].asInt();
                        event.body.object_classified.y = position["y"].asInt();
                        event.body.object_classified.width = position["width"].asInt();
                        event.body.object_classified.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    event.body.object_classified.cnt_zone_vertices = 0;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.object_classified.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.object_classified.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.object_classified.points[event.body.object_classified.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.object_classified.points[event.body.object_classified.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.object_classified.type = event_body["type"].asInt();
                    event.body.object_classified.confidence = event_body["confidence"].asInt();
                    event.body.object_classified.object_id = event_body["object_id"].asInt();
                    break;
                case  JSON_EVENT_OBJECT_DETECTED:
                    if (event_body.isMember("position")){
                        const Json::Value position = event_body["position"];
                        event.body.object_detected.x = position["x"].asInt();
                        event.body.object_detected.y = position["y"].asInt();
                        event.body.object_detected.width = position["width"].asInt();
                        event.body.object_detected.height = position["height"].asInt();
                    }
                    else
                        return 1;
                    event.body.object_detected.cnt_zone_vertices = 0;
                    if (event_body.isMember("zone")){
                        const Json::Value zone = event_body["zone"];
                        strcpy(event.body.object_detected.zone_id, zone["zone_id"].asString().c_str());
                        if (zone.isMember("vertices")){
                            const Json::Value vertices = zone["vertices"];
                            Json::Value::iterator it_vtx;
                            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                                if (event.body.object_detected.
                                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                                    break;
                                event.body.object_detected.points[event.body.object_detected.
                                    cnt_zone_vertices].x = (*it_vtx)["x"].asInt();
                                event.body.object_detected.points[event.body.object_detected.
                                    cnt_zone_vertices++].y = (*it_vtx)["y"].asInt();
                            }
                        }
                        else
                            return 1;
                    }
                    else
                        return 1;
                    event.body.object_detected.object_id = event_body["object_id"].asInt();
                    break;
                //case  JSON_EVENT_ABANDONED_OBJECT_DETECTED:
                //    if (event_body.isMember("position")){
                //        const Json::Value position = event_body["position"];
                //        event.body.abandoned_object_detected.x = position["x"].asInt();
                //        event.body.abandoned_object_detected.y = position["y"].asInt();
                //        event.body.abandoned_object_detected.width = position["width"].asInt();
                //        event.body.abandoned_object_detected.height = position["height"].asInt();
                //    }
                //    else
                //        return 1;
                //    if (event_body.isMember("zone")){
                //        const Json::Value zone = event_body["zone"];
                //        strcpy(event.body.abandoned_object_detected.zone_id,
                //            zone["zone_id"].asString().c_str());
                //        if (zone.isMember("vertices")){
                //            const Json::Value vertices = zone["vertices"];
                //            Json::Value::iterator it_vtx;
                //            event.body.abandoned_object_detected.cnt_zone_vertices = 0;
                //            for (it_vtx = vertices.begin(); it_vtx != vertices.end(); it_vtx++){
                //                if (event.body.abandoned_object_detected.
                //                    cnt_zone_vertices >= VERTEX_CNT_MAX)
                //                    break;
                //                event.body.abandoned_object_detected.points[event.body.
                //                    abandoned_object_detected.cnt_zone_vertices].x =
                //                    (*it_vtx)["x"].asInt();
                //                event.body.abandoned_object_detected.points[event.body.
                //                    abandoned_object_detected.cnt_zone_vertices++].y =
                //                    (*it_vtx)["y"].asInt();
                //            }
                //        }
                //        else
                //            return 1;
                //    }
                //    else
                //        return 1;
                //    event.body.abandoned_object_detected.duration =
                //        event_body["duration"].asInt();
                //    event.body.abandoned_object_detected.object_id =
                //        event_body["object_id"].asInt();
                //    break;
                case  JSON_EVENT_COMPOUND_EVENT:
                    event.body.compound_event.operation =
                        event_body["operator"].asInt();
                    strcpy(event.body.compound_event.sub_event_id1,
                        event_body["sub_event_id1"].asString().c_str());
                    strcpy(event.body.compound_event.sub_event_id2,
                        event_body["sub_event_id2"].asString().c_str());
                    break;
 /*               case  JSON_EVENT_HEATMAP_GENERATED:
                    {
                        event.body.heat_map.start_time =
                            event_body["start_time"].asUInt64();
                        event.body.heat_map.end_time =
                            event_body["end_time"].asUInt64();
                        event.body.heat_map.cells_per_row =
                            event_body["cells_per_row"].asInt();
                        event.body.heat_map.rows_per_column =
                            event_body["cells_per_row"].asInt();
                        event.body.heat_map.data =
                            new uint16_t[event.body.heat_map.cells_per_row*
                            event.body.heat_map.rows_per_column];
                        Json::Value::iterator it_data;
                        const Json::Value data = event_body["data"];

                            int i = 0;
                            for (it_data = data.begin(); it_data != data.end(); it_data++){
                                event.body.heat_map.data[i++] = (*it_data).asInt();
                            }
                        }
                    break;*/
                case  JSON_EVENT_EXTENSION_RESERVED:
                {
                    event.body.extension.size = event_body["size"].asInt();
                    event.body.extension.payload = new char[event.body.extension.size + 1];
                    strcpy(event.body.extension.payload, event_body["payload"].asString().c_str());
                }
                    break;
                default:
                    break;
                }
            }
            else
                return 1;
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
            events_vec_.push_back(event);
        }
    }
    else
        return 1;
    if (events_vec_.size() > 0){
        event_cnt_ = events_vec_.size();
        events_ = new struct json_event[event_cnt_];
        for (int i = 0; i < event_cnt_; i++)
        {
            events_[i] = events_vec_[i];
        }
    }
    return 0;
}
