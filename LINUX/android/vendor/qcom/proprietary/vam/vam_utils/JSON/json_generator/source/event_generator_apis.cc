/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C"
{
#include "event_generator_apis.h"
}
#include "event_generator.h"
#include <iostream>
#include <string.h>
//
extern "C"  enum EventGeneratorStatus json_event_generator_init(
    const char * locator,
    enum JSONEventMediaType type,
    const uint64_t start_time,
    const uint32_t period,
    void ** handle)
{
    JsonEventGenerator *event_generator = new JsonEventGenerator(start_time, period);

    if (type == JSON_EVENT_FILE){
        event_generator->file_id_ = start_time - (start_time%(period*1000));
        strcpy(event_generator->file_prefix_, locator);
    }
    *handle = (void *) event_generator;
    return EG_OK;
}
enum EventGeneratorStatus json_event_generator_serialize(
    void * handle,
    uint64_t time_stamp,
    uint32_t frame_id,
    struct json_event *events,
    uint32_t cnt_events,
    char ** output_buffer)
{
    JsonEventGenerator *event_generator = (JsonEventGenerator *)handle;
    event_generator->Clear();
    for (uint32_t i = 0; i < cnt_events; i++){
        event_generator->AddEvent(events[i]);
    }
    event_generator->EventsToJSON();
    *output_buffer  = event_generator->doc_str_;
    if (((time_stamp - event_generator->file_id_ >= event_generator->period_ * 1000))){
        event_generator->file_id_ = time_stamp - (time_stamp % (event_generator->period_ * 1000));
        event_generator->start_time_ = event_generator->file_id_;
    }
    return EG_OK;
}
extern "C"  enum EventGeneratorStatus json_event_generator_output_json(
    void * handle,
    uint64_t time_stamp,
    uint32_t frame_id,
    struct json_event *events,
    uint32_t cnt_events)
{
    JsonEventGenerator *event_generator = (JsonEventGenerator *)handle;
    char *output;
    event_generator->Clear();
    for (uint32_t i = 0; i < cnt_events; i++){
        event_generator->AddEvent(events[i]);
    }
    event_generator->EventsToJSON();
    output = (char*)(event_generator->events_buf_).c_str();
    int size = strlen(output);
    if (event_generator->fp_json_==0||
        ((time_stamp - event_generator->file_id_ >= event_generator->period_ * 1000))){
        event_generator->file_id_ = time_stamp - (time_stamp%(event_generator->period_*1000));
        event_generator->start_time_ = event_generator->file_id_;
        if (event_generator->fp_json_)
            fclose(event_generator->fp_json_);
        if (event_generator->fp_idx_)
            fclose(event_generator->fp_idx_);
        sprintf(event_generator->filename_json_, "%s_%llu.jsonx",
            event_generator->file_prefix_, event_generator->file_id_);
        sprintf(event_generator->filename_idx_, "%s_%llu.idx",
            event_generator->file_prefix_, event_generator->file_id_);

        event_generator->fp_json_ = fopen(event_generator->filename_json_, "w+t");
        if (event_generator->fp_json_ == NULL)
            return EG_INVALID;
        event_generator->fp_idx_ = fopen(event_generator->filename_idx_, "w+t");
        if (event_generator->fp_idx_ == NULL)
            return EG_INVALID;
    }
    fprintf(event_generator->fp_idx_, "%llu\t%d\t%d\n", time_stamp, frame_id, size);
    fwrite(output, 1, size, event_generator->fp_json_);

    return EG_OK;
}

extern "C"  enum EventGeneratorStatus json_event_generator_deinit(void * handle)
{
    JsonEventGenerator *event_generator = (JsonEventGenerator *)handle;
    if (event_generator->fp_json_)
        fclose(event_generator->fp_json_);
    if (event_generator->fp_idx_)
        fclose(event_generator->fp_idx_);
    delete event_generator;
    return EG_OK;
}
