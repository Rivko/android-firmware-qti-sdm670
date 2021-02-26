/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_GENERATOR_APIS_H_
#define METADATA_GENERATOR_APIS_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include "json_metadata.h"
#include "json_common_types.h"
/**
* @brief initialize the metadata generator. Return the transparent handle
* of metadata generator
* @param locator the prefix of filename for file storage
* @param type VAM_FILE for file storage and VAM_RTSP for rtsp
* streaming
* @param frame_count_per_chunk the number of frames to be loaded for processing
* each time
* @param chunk_count_per_file the number of chunks in each file
* @param handle the transparent handle of metadata generator
**/
enum JSONVAStatus metadata_generator_init(
    const char * locator,
    enum VAMMediaType type,
    const int frame_count_per_chunk,
    const int chunk_count_per_file,
    void ** handle);
/**
* @brief deinitialize the metadata generator. This must be called after the
* metadata generator finishes its work.
* @param handle the transparent handle of metadata generator
**/
enum JSONVAStatus metadata_generator_deinit(void * handle);
/**
* @brief initialize a metadata_content_t structure. metadata_content_t
* should be initialized before it can be used
* @param handle the transparent handle of metadata generator
* @param metadata_content the metadata content its internal memory will be allocated
**/
enum JSONVAStatus metadata_generator_initialize_metadata_content(void * handle, struct metadata_content_t *metadata_content);

/**
* @brief release a metadata_content_t structure. metadata_content_t
* should be released when it will not be used
* @param handle the transparent handle of metadata generator
* @param metadata_content the metadata content to be released
**/
enum JSONVAStatus metadata_generator_release_metadata_content(void * handle,
    struct metadata_content_t *metadata_content);

/**
* @brief format the metadata to a string from json internal representation
* @param handle the transparent handle of metadata generator
* @param metadata_content the metadata content to be serialized
* @param output the metadata in a string of JSON format
**/
enum JSONVAStatus metadata_generator_serialize(void * handle,
    struct metadata_content_t *metadata_content, char **output);

/**
* @brief output the metadata in VAM JSON format for storage or transmission
* @param handle the transparent handle of metadata generator
* @param metadata_content the metadata content to be sent out
**/
enum JSONVAStatus metadata_generator_output_json(void * handle,
struct metadata_content_t *metadata_content);
/**
* @brief clear the metadata_content_t structure.
* @param handle the transparent handle of metadata generator
* @param metadata_content the metadata content to be cleared
**/
enum JSONVAStatus metadata_generator_clear_metadata_content(void * handle,
struct metadata_content_t *metadata_content);
/**
* @brief set the time interval between the metadata frames
* @param handle the transparent handle of metadata generator
* @param interval time interval between the metadata frames
**/
enum JSONVAStatus metadata_generator_set_frame_interval(void * handle,
    uint64_t interval);
#ifdef __cplusplus
}
#endif
#endif