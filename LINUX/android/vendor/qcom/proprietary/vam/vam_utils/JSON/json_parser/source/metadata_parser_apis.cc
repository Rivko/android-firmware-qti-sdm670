/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C"{
#include "json_metadata.h"
#include "metadata_parser_apis.h"
}
#include "metadata_parser.h"
#include <string.h>

extern "C" enum MetadataParserStatus metadata_parser_init(
    const char * stream_locator,
    enum VAMMediaType type,
    const uint64_t start_time_stamp,
    const int start_frame_id,
    void ** handle)
{
    MetadataParser *metadata_parser = new MetadataParser;
    metadata_parser->file_id_ = 0;
    strcpy(metadata_parser->file_prefix_, stream_locator);
    metadata_parser->start_frame_id_ = start_frame_id;
    metadata_parser->start_time_stamp_ = start_time_stamp;
    metadata_parser->time_stamp_ = start_time_stamp;
    strcpy(metadata_parser->file_prefix_, stream_locator);
    *handle = (void*)metadata_parser;
    return MP_OK;
}
extern "C" enum MetadataParserStatus metadata_parser_deinit(void * handle)
{
    MetadataParser *metadata_parser = (MetadataParser *)handle;
    delete metadata_parser;
    return MP_OK;
}
extern "C" enum MetadataParserStatus metadata_parser_get_next_frame(
    void * handle,
    char **doc)
{
    MetadataParser *metadata_parser = (MetadataParser *)handle;
//    int pos_start = metadata_parser->file_pos_;
    JSONVAStatus res = metadata_parser->GetNextMetadataFrame(metadata_parser->str_chunk_);
    if (res == JSON_VA_INVALID)
        return MP_INVALID;
    else if (res == JSON_VA_END_OF_FILE)
        return MP_END_OF_SEQUENCE;
    *doc = (char *)(metadata_parser->str_chunk_.c_str());
    return MP_OK;
}

extern "C" enum MetadataParserStatus metadata_parser_parse_metadata_frame(void * handle,
    const char* metadata_str,
struct metadata_content_t **metadata)
{
    JSONVAStatus res;
    MetadataParser *metadata_parser =
        (MetadataParser *)handle;
    res = metadata_parser->ParseDoc(metadata_str, metadata_parser->metadata_frame_);
    if (res != JSON_VA_OK){
        printf("Failed to parse metadata frame in metadata_aprser_apis\n");
        return MP_INVALID;
    }
	metadata_parser->ConvertMetadataFrameToMetadataContent(metadata_parser->metadata_frame_, metadata);
    metadata_parser->last_time_stamp_ = metadata_parser->time_stamp_;
    metadata_parser->time_stamp_ = (*metadata)->time_stamp;
    metadata_parser->time_interval_ = metadata_parser->time_stamp_ - metadata_parser->last_time_stamp_;
//	*metadata = metadata1;
    return MP_OK;
}

enum MetadataParserStatus metadata_parser_parse_doc(
    void * handle,
    const char* doc,
    struct metadata_content_t ** metadata_frames,
    int * cnt_metadata_frames)
{
    MetadataParser *metadata_parser =
        (MetadataParser *)handle;
    std::vector<MetadataFrame>  metadata;
    metadata_parser->ParseDoc(doc, metadata);
    *cnt_metadata_frames = metadata.size();
    *metadata_frames = (metadata_content_t *)malloc(
        sizeof(metadata_content_t)*metadata.size());
    for (int i = 0; i < *cnt_metadata_frames; i++)
    {
        metadata_parser->ConvertMetadataFrameToMetadataContent(
            metadata[i], &metadata_frames[i]);
    }
    return MP_OK;
}
enum MetadataParserStatus metadata_parser_parse_doc_chunk(
    void * handle,
    const char* doc,
    struct metadata_content_t ** metadata,
    int * cnt_metadata)
{
    MetadataParser *metadata_parser =
        (MetadataParser *)handle;
    MetadataFrame metadata_frame;
    metadata_parser->ParseDoc(doc, metadata_frame);
    metadata_parser->ConvertMetadataFrameToMetadataContent(metadata_frame, metadata);
    return MP_OK;
}

extern "C" enum MetadataParserStatus metadata_parser_load_file_to_memory(
    void * handle,
    const char *filename,
    char **doc_str)
{
    std::string str;
    MetadataParser *metadata_parser = (MetadataParser *)handle;
    metadata_parser->LoadFileToString(filename, metadata_parser->file_string_);
    *doc_str = (char*)metadata_parser->file_string_.c_str();
    return MP_OK;
}

extern "C" enum MetadataParserStatus metadata_parser_parse_next_frame(
    void * handle,
    struct metadata_content_t ** metadata)
{
    char *str;
    enum MetadataParserStatus res = metadata_parser_get_next_frame(handle, &str);
    if (res != MP_OK)
        return res;
    res = metadata_parser_parse_metadata_frame(handle, str, metadata);
    return res;
}

extern "C" 	enum MetadataParserStatus metadata_parser_seek_metadata_frame(
        void * handle,
        uint64_t time_stamp)
{
    MetadataParser *metadata_parser = (MetadataParser *)handle;
    JSONVAStatus res = metadata_parser->SeekMetadataPos(time_stamp);
    return (enum MetadataParserStatus)res;
}
