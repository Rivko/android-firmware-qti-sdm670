/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EVENT_GENERATOR_H_
#define EVENT_GENERATOR_H_

#include <vector>
#include "json_event.h"
#include "json/json.h"

class JsonEventGenerator
{
public:
    JsonEventGenerator(uint64_t start_time, uint32_t period);
    ~JsonEventGenerator();
    int Clear();
    int AddEvent(struct json_event event);
    int GenerateEvents();
    int EventToJSON(struct json_event event, Json::Value &events_json);
    int EventsToJSON();
    uint64_t start_time_;
    uint32_t period_;
    std::vector<struct json_event> events_;
    std::string events_buf_;
    char * doc_str_;
    uint64_t file_id_;
    char file_prefix_[256];
    char filename_json_[1000];
    char filename_idx_[1000];
    FILE * fp_json_;
    FILE * fp_idx_;
};
#endif
