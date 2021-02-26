/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EVENT_PARSER_H_
#define EVENT_PARSER_H_

#include <stdint.h>
#include <vector>
#include "json/json.h"
#include "json_event.h"
struct EventIdxFileRecord{
    uint64_t time_stamp;
    uint32_t frame_id;
    uint32_t frame_size;
};


class JSONEventParser{
public:
    JSONEventParser(const char * stream_locator, uint64_t start_time, uint64_t end_time, uint32_t period);
    ~JSONEventParser();

    int LoadIdxFile(const char *filename);
    int LoadNextJsonEvents(std::string &events_str);
    int ParseEvents(const char* events_str);
    int LoadFileToString(const char *filename, std::string &contents);
    FILE *fp_idx_;
    FILE *fp_event_;
    std::string file_string_;
    std::string str_chunk_;
    char stream_locator_[256];
    uint32_t cur_record_id_;
    struct json_event *events_;
    int event_cnt_;
    std::vector<struct json_event> events_vec_;
    std::vector<EventIdxFileRecord> idx_records_;
    uint64_t start_time_stamp_;
    uint64_t end_time_stamp_;
    uint64_t file_id_;
    uint32_t frame_id_;
    uint32_t period_;
    uint32_t file_pos_;
};

#endif //EVENT_PARSER_H_
