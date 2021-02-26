/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_PARSER_APIS_H_
#define METADATA_PARSER_APIS_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
enum MetadataParserStatus{
    MP_OK = 0,///< the call is successful
    MP_INVALID,///< the call is failed
    MP_END_OF_SEQUENCE///<end of sequence is detected
};
/**
* @brief
* Initialize MetadataParser. This function
* should be called before any Metadata Parser calls.
* @param stream_locator prefix of a metadata file or rtsp url
* @param type the input type: VAM_FILE for file storage and
* VAM_RTSP for rtsp streaming
* @param start_time_stamp the time stamp of the first metadata frame
* @param start_frame_id the first metadata frame with the frame_id to be parsed
* @param  handle an opaque handle to the MetadataParser
**/
enum MetadataParserStatus metadata_parser_init(
    const char * stream_locator,
    enum VAMMediaType type,
    const uint64_t start_time_stamp,
    const int start_frame_id,
    void ** handle);

/**
* @brief
* Deinitialize MetadataParser and free all the memories used by it. This
* function should be called after all other MetadataParser calls.
* @param handle the opaque handle of MetadataParser
**/
enum MetadataParserStatus metadata_parser_deinit(void * handle);
/**
* @brief
* Load a JSON metadata file to a string to be parsed
* @param handle the opaque handle of MetadataParser
* @param filename the filename of the JSON file
* @param doc_str the JSON metadata content as a string
**/
enum MetadataParserStatus metadata_parser_load_file_to_memory(
    void * handle,
    const char *filename,
    char **doc_str);

/**
* @brief
* Parse a JSON string to metadata_content_t structure
* @param handle the opaque handle of MetadataParser
* @param metadata_str the JSON metadata content as a string
* @param metadata the place holder for parsed metadata content.
**/
enum MetadataParserStatus metadata_parser_parse_metadata_frame(void * handle,
    const char* metadata_str,
    struct metadata_content_t ** metadata);

/**
* @brief
* load next metadata frame to the memory
* @param handle the opaque handle of MetadataParser
* @param metadata_str the JSON metadata content as a string
**/
enum MetadataParserStatus metadata_parser_get_next_frame(
    void * handle,
    char **metadata_str);
/**
* @brief
* parse next metadata frame to metadata_content_t structure
* @param handle the opaque handle of MetadataParser
* @param metadata the place holder for parsed metadata content
**/
enum MetadataParserStatus metadata_parser_parse_next_frame(
    void * handle,
    struct metadata_content_t ** metadata);
/**
* @brief
* seek metadata frame with the given time stamp
* @param handle the opaque handle of MetadataParser
* @param time_stamp the time stamp of the metadata to be seeked
**/
enum MetadataParserStatus metadata_parser_seek_metadata_frame(
    void * handle,
    uint64_t time_stamp);
#ifdef __cplusplus
}
#endif
#endif
