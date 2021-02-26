/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef EVENT_PARSER_APIS_H_
#define EVENT_PARSER_APIS_H_
#include <stdint.h>
#include "json_event.h"
#ifdef __cplusplus
extern "C"
{
#endif
enum EventParserStatus{
    EP_OK = 0,///< the call is successful
    EP_INVALID,///< the call is failed
    EP_END_OF_SEQUENCE///<end of sequence is detected
};
/**
* @brief
* Initialize EventParser. This function should be called before any Event
* Parser calls.
* @param locator prefix of a event file or rtsp url
* @param type JSON_EVENT_FILE for file storage and JSON_EVENT_RTSP for rtsp
* streaming
* @param start_time the start time of the event file
* @param period the period of event file in seconds
* @param handle the transparent handle of event generator
**/
enum EventParserStatus json_event_parser_init(
    const char * locator,
    enum JSONEventMediaType type,
    const uint64_t start_time,
    const uint64_t end_time,
    const int period,
    void ** handle);

/**
* @brief
* Deinitialize EventParser and free all the memories used by it. This
* function should be called after all other EventParser calls.
* @param handle the opaque handle of EventParser
**/
    enum EventParserStatus json_event_parser_deinit(void * handle);

/**
* @brief
* Load a JSON event file to a string to be parsed
* @param handle the opaque handle of EventParser
* @param filename the filename of the JSON file
* @param doc_str the JSON event content as a string
**/
enum EventParserStatus json_event_parser_load_file_to_memory(
    void * handle,
    const char *filename,
    char **doc_str);

/**
* @brief
* Parse a JSON string to metadata_content_t structure
* @param handle the opaque handle of MetadataParser
* @param events_str the JSON events packet as a string
* @param cnt_events the count of the evnets.
**/

enum EventParserStatus json_event_parser_parse_events(void * handle,
    const char* events_str, struct json_event ** events,
    int * cnt_events);

/**
* @brief
* load next events packet to the memory
* @param handle the opaque handle of MetadataParser
* @param metadata_str the JSON metadata content as a string
**/
enum EventParserStatus json_event_parser_get_next_packet(
    void * handle,
    char **events_str);
/**
* @brief
* parse next packet of events
* @param handle the opaque handle of MetadataParser
* @param events the place holder for parsed events
* @param cnt_events the count of events in the packet
**/
    enum EventParserStatus json_event_parser_parse_next_packet(
    void * handle, struct json_event ** events,
    int * cnt_events);

#ifdef __cplusplus
}
#endif
#endif //EVENT_PARSER_APIS_H_
