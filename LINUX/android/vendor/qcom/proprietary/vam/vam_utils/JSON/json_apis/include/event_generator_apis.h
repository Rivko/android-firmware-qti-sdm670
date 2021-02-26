/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EVENT_GENERATOR_APIS_H_
#define EVENT_GENERATOR_APIS_H_

#include "json_event.h"

enum EventGeneratorStatus{
    EG_OK = 0,///< the call is successful
    EG_INVALID,///< the call is failed
    EG_END_OF_SEQUENCE///<end of sequence is detected
};

/**
* @brief initialize the event generator. Return the transparent handle
* of event generator
* @param locator the prefix of filename for file storage
* @param type JSON_EVENT_FILE for file storage and JSON_EVENT_RTSP for rtsp
* streaming
* @param start_time the start time stamp of the event file
* @param period the period of event file in seconds
* @param handle the transparent handle of event generator
**/
enum EventGeneratorStatus json_event_generator_init(
    const char * locator,
    enum JSONEventMediaType type,
    const uint64_t start_time,
    const uint32_t period,
    void ** handle);
/**
* @brief generate events in JSON format and write to file/rtsp stream directly
*
* @param  handle the transparent handle of event generator
* @param time_stamp the time stamp of the current frame
* @param frame_id the frame_id of the input video frame
* @param events the array of json_event structure that holds the events data
* @param cnt_events the count of events in the events array
**/
enum EventGeneratorStatus json_event_generator_output_json(
    void * handle,
    uint64_t time_stamp,
    uint32_t frame_id,
    struct json_event *events,
        uint32_t cnt_events);
/**
* @brief generate events in JSON format and output it in output_buffer
*
* @param  handle the transparent handle of event generator
* @param time_stamp the time stamp of the current frame
* @param frame_id the frame_id of the input video frame
* @param events the array of json_event structure that holds the events data
* @param cnt_events the count of events in the events array
* @param output_buf the ouptut buffer for the json events
**/
enum EventGeneratorStatus json_event_generator_serialize(
    void * handle,
    uint64_t time_stamp,
    uint32_t frame_id,
    struct json_event *events,
    uint32_t cnt_events,
    char **output_buffer);
/**
* @brief deinitialize the handle of event generator and release internal memory.
* this function has to be called after event generator fishish its work.
* @param  handle the transparent handle of event generator
**/
enum EventGeneratorStatus json_event_generator_deinit(void * handle);
#endif