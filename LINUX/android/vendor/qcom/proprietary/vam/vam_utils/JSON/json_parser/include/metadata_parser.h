/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_PARSER_H_
#define METADATA_PARSER_H_
#include "metadata_types.h"
#include <json/json.h>
#include "json_common_structs.h"
extern "C"{
#include "metadata_parser_apis.h"
#include "json_common_types.h"
#include "json_metadata.h"
}
struct IdxFileRecord{
    uint64_t time_stamp;
    uint32_t frame_id;
    uint32_t frame_size;
};

class MetadataParser
{
public:
    MetadataParser();
    ~MetadataParser();
    JSONVAStatus ClearMetadata(MetadataFrame & metadata);
    JSONVAStatus LoadIdxFile(const char *filename);
    JSONVAStatus LoadNextJsonFrame(std::string &doc_str);
    JSONVAStatus LoadFileToString(const char *filename, std::string &doc_str);
    JSONVAStatus GetNextMetadataFrame(std::string & str);
    JSONVAStatus ParseMetadataFrame(const char* metadata_frame,
        MetadataFrame & metadata);
    JSONVAStatus ParseDoc(const char* doc, MetadataFrame & metadata);
    JSONVAStatus ParseDoc(const char* doc,
        std::vector<MetadataFrame> & metadata);
    JSONVAStatus ParseScene(const Json::Value scene,
        MetadataFrame & metadata);
    JSONVAStatus ParseObjects(const Json::Value objects,
        MetadataFrame & metadata);
    JSONVAStatus ParseObjectTrees(const Json::Value object_trees,
        MetadataFrame & metadata);
    JSONVAStatus ParseAtomicEvents(const Json::Value atomic_events,
        MetadataFrame & metadata);
    JSONVAStatus ParseEvents(const Json::Value events,
        MetadataFrame & metadata);
    JSONVAStatus ParseExtension(const Json::Value extensions,
        MetadataFrame & metadata);
    JSONVAStatus ConvertMetadataFrameToMetadataContent(
        const MetadataFrame metadata,
        struct metadata_content_t **metadata_content);
    JSONVAStatus ConvertMetadataContentToMetadataFrame(
        const struct metadata_content_t *metadata_content,
        MetadataFrame &metadata);
    JSONVAStatus AllocateMetadataContent(
        struct metadata_content_t *metadata_content);
    JSONVAStatus CopyMetadataContent(
        const struct metadata_content_t metadata_content_src,
        struct metadata_content_t *metadata_content_dst);
    JSONVAStatus ReleaseMetadataContent(
        struct metadata_content_t *metadata_content_src);
    JSONVAStatus SeekMetadataPos(uint64_t time_stamp);
    template<typename T> bool NotInValidRange(
        T value,
        T minValue,
        T maxValue);
    std::string file_string_;
    std::string str_chunk_;
    std::string filename_loaded;
    uint32_t file_pos_;
    uint32_t frames_loaded_;
    uint64_t time_stamp_;
    uint64_t last_time_stamp_;
    struct metadata_content_t metadata_content_;
	struct MetadataFrame metadata_frame_;
    FILE * fp_json_;
    FILE * fp_idx_;
    char file_prefix_[256];
    char filename_json_[1000];
    char filename_idx_[1000];
    uint32_t file_id_;
    char * doc_str_;
    int frame_cnt_;
    uint32_t cur_record_id_;
    uint64_t start_time_stamp_;
    int start_frame_id_;
    uint64_t time_interval_;
private:
    Json::Value metadata_json_;
    std::vector<IdxFileRecord> idx_records;
};
/**
* @brief
* Load a chunk of metadata frames from a file to a string to be parsed
* @param handle the opaque handle of MetadataParser
* @param filename the filename of the JSON file, if the
* filename is 0, the previous opened file will be used to load data
* @param doc_str the JSON metadata content as a string
* @return the number of metadata frames loaded
**/
uint32_t metadata_parser_load_file_chunk_to_memory(
    void * handle,
    const char *filename,
    char **doc_str);
/**
* @brief
* Parse a JSON string to metadata_content_t structure
* @param handle the opaque handle of MetadataParser
* @param doc the JSON metadata content as a string
* @param metadata the place holder for parsed metadata content
* @param cnt_metadata the count of metadata frames
**/
enum MetadataParserStatus metadata_parser_parse_doc(
    void * handle,
    const char* doc,
    struct metadata_content_t ** metadata,
    int * cnt_metadata);
/**
* @brief
* Parse a JSON string to a vector of MetadataFrame structure
* @param handle the opaque handle of MetadataParser
* @param doc the JSON metadata content as a string
* @param metadata the place holder for parsed metadata content
**/
enum MetadataParserStatus metadata_parser_parse_doc_chunk(void * handle,
    const char* doc,
struct metadata_content_t ** metadata_frame,
    int * cnt_metadata_frames);
#endif //METADATA_PARSER_H_
