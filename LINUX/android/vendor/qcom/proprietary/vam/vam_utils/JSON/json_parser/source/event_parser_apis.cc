/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "event_parser_apis.h"
#include "event_parser.h"

enum EventParserStatus json_event_parser_init(
    const char * locator,
    enum JSONEventMediaType type,
    const uint64_t start_time,
    const uint64_t end_time,
    const int period,
    void ** handle)
{
    JSONEventParser * parser = new JSONEventParser(locator, start_time, end_time, period);
    *handle = (void*)parser;
    return EP_OK;
}

    enum EventParserStatus json_event_parser_deinit(void * handle)
{
    JSONEventParser * parser = (JSONEventParser *)handle;
    delete parser;
    return EP_OK;
}

enum EventParserStatus json_event_parser_load_file_to_memory(
    void * handle,
    const char *filename,
    char **doc_str)
{
    std::string str;
    JSONEventParser * parser = (JSONEventParser *)handle;
    parser->LoadFileToString(filename, parser->file_string_);
    *doc_str = (char*)parser->file_string_.c_str();
    return EP_OK;
}

enum EventParserStatus json_event_parser_parse_events(void * handle,
    const char* events_str, struct json_event ** events,
    int * cnt_events)
{
    JSONEventParser * parser = (JSONEventParser *)handle;
    parser->ParseEvents(events_str);
    *events = parser->events_;
    (*events)->base.frame_id = parser->frame_id_;
    *cnt_events = parser->event_cnt_;
    return EP_OK;
}

enum EventParserStatus json_event_parser_get_next_packet(
    void * handle,
    char **events_str)
{
    JSONEventParser * parser = (JSONEventParser *)handle;
    //int pos_start = parser->file_pos_;
    int res = parser->LoadNextJsonEvents(parser->str_chunk_);
    if (res == 1)
        return EP_INVALID;
    else if (res == 2)
        return EP_END_OF_SEQUENCE;
    *events_str = (char *)(parser->str_chunk_.c_str());
    return EP_OK;
}
enum EventParserStatus json_event_parser_parse_next_packet(
    void * handle, struct json_event ** events,
    int * cnt_events)
{
    char *str;
    enum EventParserStatus res = json_event_parser_get_next_packet(handle, &str);
    if (res != EP_OK)
        return res;
    res = json_event_parser_parse_events(handle, str, events, cnt_events);
    return res;
}
