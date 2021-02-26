/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C"{
#include "metadata_generator_apis.h"
}
#include "metadata_generator.h"
#include <string.h>

extern "C" JSONVAStatus metadata_generator_init(
    const char * stream_locator,
    enum VAMMediaType type,
    const int frame_count_per_chunk,
    const int chunk_count_per_file,
    void ** handle)
{
    MetadataGenerator *metadata_generator = new MetadataGenerator;
    if (type == VAM_FILE){
        metadata_generator->file_id_ = 0;
        metadata_generator->frame_count_per_chunk_ = frame_count_per_chunk;
        metadata_generator->chunk_count_per_file_ = chunk_count_per_file;
        strcpy(metadata_generator->file_prefix_, stream_locator);
    }
    *handle = (void*)metadata_generator;
    return JSON_VA_OK;
}
extern "C" JSONVAStatus metadata_generator_deinit(void * handle)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    if (metadata_generator->fp_json_)
        fclose(metadata_generator->fp_json_);
    if (metadata_generator->fp_idx_)
        fclose(metadata_generator->fp_idx_);
    delete metadata_generator;
    return JSON_VA_OK;
}

extern "C" JSONVAStatus metadata_generator_serialize(void * handle, struct metadata_content_t *metadata_content, char **output)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    Json::Value root;
    metadata_generator->ToJSON(*metadata_content, root);
    std::string doc;
    metadata_generator->Serialize(root, doc);
    metadata_generator->doc_str_ = (char*)realloc(metadata_generator->doc_str_, doc.length()+10);
    strcpy(metadata_generator->doc_str_, doc.c_str());
    *output = metadata_generator->doc_str_;
    return JSON_VA_OK;
}

extern "C" JSONVAStatus metadata_generator_output_json(void * handle, struct metadata_content_t *metadata_content)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    char *output;
    metadata_generator_serialize(handle, metadata_content, &output);

    int size = strlen(output) ;
    if (metadata_generator->frame_cnt %
        (metadata_generator->frame_count_per_chunk_*
        metadata_generator->chunk_count_per_file_) == 0){
    //if ((metadata_generator->time_stamp_ -
    //	metadata_generator->start_time_stamp_) >
    //	(metadata_generator->chunk_count_per_file_*
    //	metadata_generator->frame_count_per_chunk_*
    //	metadata_generator->time_interval_))
        if (metadata_generator->fp_json_)
            fclose(metadata_generator->fp_json_);
        if (metadata_generator->fp_idx_)
            fclose(metadata_generator->fp_idx_);
        metadata_generator->file_id_ = metadata_generator->frame_cnt / (metadata_generator->frame_count_per_chunk_*metadata_generator->chunk_count_per_file_);
        if (metadata_generator->frame_cnt==0){
            sprintf(metadata_generator->filename_json_, "%s_0_%d.jsonx", metadata_generator->file_prefix_, metadata_generator->frame_cnt);
            sprintf(metadata_generator->filename_idx_, "%s_0_%d.idx", metadata_generator->file_prefix_, metadata_generator->frame_cnt);
        }
        else{
            sprintf(metadata_generator->filename_json_, "%s_%llu_%d.jsonx", metadata_generator->file_prefix_, metadata_content->time_stamp, metadata_generator->frame_cnt);
            sprintf(metadata_generator->filename_idx_, "%s_%llu_%d.idx", metadata_generator->file_prefix_, metadata_content->time_stamp, metadata_generator->frame_cnt);
        }
        //printf("%s\n", metadata_generator->filename_json_);
        //printf("%s\n", metadata_generator->filename_idx_);
        metadata_generator->fp_json_ = fopen(metadata_generator->filename_json_, "w+t");
        if (metadata_generator->fp_json_ == NULL)
            return JSON_VA_INVALID;
        metadata_generator->fp_idx_ = fopen(metadata_generator->filename_idx_, "w+t");
        if (metadata_generator->fp_idx_ == NULL)
            return JSON_VA_INVALID;
    }
    fprintf(metadata_generator->fp_idx_, "%llu\t%d\t%d\n", metadata_content->time_stamp, metadata_generator->frame_cnt, size);
    fwrite(output, 1, size, metadata_generator->fp_json_);
    metadata_generator->frame_cnt++;
    return JSON_VA_OK;
}

extern "C" JSONVAStatus metadata_generator_initialize_metadata_content(void * handle, struct metadata_content_t *metadata_content)
{

    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = metadata_generator->AllocateMetadataContent(metadata_content);
    if (status == JSON_VA_INVALID)
        return JSON_VA_INVALID;
    metadata_content->cnt_atomic_events = 0;
    metadata_content->cnt_events = 0;
    metadata_content->cnt_extensions = 0;
    metadata_content->cnt_heat_maps = 0;
    metadata_content->cnt_objects = 0;
    metadata_content->cnt_object_trees = 0;
    return JSON_VA_OK;
}
extern "C" JSONVAStatus metadata_generator_clear_metadata_content(void * handle, struct metadata_content_t *metadata_content)
{

    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    for (int i = 0; i < MD_HEAT_MAP_MAX; i++){
        if (metadata_content->heat_maps[i].data){
            free(metadata_content->heat_maps[i].data);
            metadata_content->heat_maps[i].data = 0;
        }
        metadata_content->heat_maps[i].cells_per_row = 0;
        metadata_content->heat_maps[i].rows_per_column = 0;
    }
    for (int i = 0; i < MD_EXTENSION_MAX; i++){
        if (metadata_content->extensions[i].data){
            free(metadata_content->extensions[i].data);
            metadata_content->extensions[i].data = 0;
        }
        metadata_content->extensions[i].size_data = 0;
    }
    metadata_content->cnt_atomic_events = 0;
    metadata_content->cnt_events = 0;
    metadata_content->cnt_extensions = 0;
    metadata_content->cnt_heat_maps = 0;
    metadata_content->cnt_objects = 0;
    metadata_content->cnt_object_trees = 0;
    metadata_generator->Clear();
    return JSON_VA_OK;
}
extern "C" JSONVAStatus metadata_generator_release_metadata_content(void * handle, struct metadata_content_t *metadata_content)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    metadata_generator->ReleaseMetadataContent(metadata_content);
    return JSON_VA_OK;
}
extern "C" enum JSONVAStatus metadata_generator_set_frame_interval(void * handle,
    uint64_t interval)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    metadata_generator->SetTimeInterval(interval);
    return JSON_VA_OK;
}
 JSONVAStatus metadata_generator_add_object(void * handle,
    struct metadata_content_t *metadata_content,
    struct metadata_object_t object)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddObject(metadata_content, object);
    return status;
}
 JSONVAStatus metadata_generator_add_object_tree(void * handle,
    struct metadata_content_t *metadata_content,
    struct metadata_object_tree_t object_tree)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddObjectTree(metadata_content, object_tree);
    return status;
}

 JSONVAStatus metadata_generator_add_atomic_event(void * handle,
    struct metadata_content_t *metadata_content,
    struct metadata_atomic_event_t  atomic_event)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddAtomicEvent(metadata_content, atomic_event);
    return status;
}
 JSONVAStatus metadata_generator_add_event(void * handle,
    struct metadata_content_t *metadata_content, struct metadata_event_t event)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddEvent(metadata_content, event);
    return status;
}
JSONVAStatus metadata_generator_add_heat_map(void * handle,
    struct metadata_content_t *metadata_content,
    struct metadata_heat_map_t heat_map)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddHeatMap(metadata_content, heat_map);
    return status;
}
 JSONVAStatus metadata_generator_add_extension(void * handle,
    struct metadata_content_t *metadata_content,
    struct metadata_extension_t extension)
{
    MetadataGenerator *metadata_generator = (MetadataGenerator *)handle;
    JSONVAStatus status = (JSONVAStatus)metadata_generator->AddExtension(metadata_content, extension);
    return status;
}
