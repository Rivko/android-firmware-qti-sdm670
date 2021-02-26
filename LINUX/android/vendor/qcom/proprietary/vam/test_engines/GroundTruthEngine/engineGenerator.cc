/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <VAM/vam_engine_api.h>
#include <VAM/vaapi_common.h>
#include <VAM/vaapi_config.h>
#include <VAM/vaapi_errors.h>
#include <VAM/VAMUtilities.h>
#include <unistd.h>
#include <utils/Log.h>

#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>

#include "json_metadata.h"
#include "metadata_parser_apis.h"

#define VAM_INFO(fmt, args...) ALOGD(fmt, ##args)
#define VAM_ERROR(fmt, args...) ALOGE(fmt, ##args)

extern "C" {
    int32_t GTRenderer_init(const struct vaapi_source_info *sourceInfo,
                            void **handle);
    int32_t GTRenderer_deinit(void *handle);

    int32_t GTRenderer_process(void *handle,
                               struct vaapi_frame_info *pFrameInfo);

    int32_t GTRenderer_add_rule(void *handle, vaapi_rule *rule);
    int32_t GTRenderer_del_rule(void *handle, const char *config_id);

    int32_t GTRenderer_enroll_obj(void *handle,
                                  struct vaapi_enrollment_info *enroll_info);
    int32_t GTRenderer_disenroll_obj(void *handle, const char *obj_id);

    int32_t GTRenderer_get_event_count(void *handle, uint32_t *event_count);
    int32_t GTRenderer_get_events(void *handle,
                                  struct vaapi_event *events,
                                  uint32_t num_events);
    int32_t GTRenderer_get_object_count(void *handle, uint32_t *object_count);
    int32_t GTRenderer_get_objects(void *handle,
                                   vaapi_object *objects,
                                   uint32_t obj_count);
    int32_t GTRenderer_get_target_FPS(void *handle, uint8_t *target_fps);

    int32_t GTRenderer_is_event_supported(void *handle,
                                          vaapi_event_type type,
                                          uint8_t *is_supported);
    int32_t GTRenderer_is_depends_on_other_event(void *handle,
                                                 vaapi_event_type type,
                                                 uint8_t *is_depended);
}

vaapi_module_methods_t va_methods =
{
    .init                 = GTRenderer_init,
    .deinit               = GTRenderer_deinit,
    .process              = GTRenderer_process,
    .add_rule             = GTRenderer_add_rule,
    .del_rule             = GTRenderer_del_rule,
    .enroll_obj           = GTRenderer_enroll_obj,
    .disenroll_obj        = GTRenderer_disenroll_obj,
    .get_event_count      = GTRenderer_get_event_count,
    .get_events           = GTRenderer_get_events,
    .get_object_count     = GTRenderer_get_object_count,
    .get_objects          = GTRenderer_get_objects,
    .get_target_FPS       = GTRenderer_get_target_FPS,
    .is_event_supported   = GTRenderer_is_event_supported,
    .is_depends_on_other_event = GTRenderer_is_depends_on_other_event
};

vaapi_alg_module_t VA_ALG_INFO_SYM =
{
    .module_api_version      = "1.0",
    .name                    = "Test Ground Truth Engine",
    .author                  = "Qualcomm Technologies Inc",
    .methods                 = &va_methods,
    .reserved                = {NULL, NULL, NULL}
};

static struct vaapi_source_info sourceInfo;

void *jsonHandle = nullptr;
struct metadata_content_t *json_metadata = nullptr;
int32_t GTRenderer_init(const struct vaapi_source_info *pSourceInfo,
                        void **handle) {
    if (pSourceInfo == nullptr) {
        return VAM_NULLPTR;
    }

    memcpy(&sourceInfo, pSourceInfo, sizeof(vaapi_source_info));

    std::ifstream inputFile;

    std::string inputFileName(pSourceInfo->data_folder);
    inputFileName += "/GTRenderer.data";

    inputFile.open(inputFileName.c_str());
    if (inputFile.is_open() == false) {
        VAM_ERROR("GTRenderer_init error: failed to open file %s\n",
                    inputFileName.c_str());
        return VAM_FAIL;
    }
    std::string GTFilePrefix;
    inputFile >> GTFilePrefix;

    MetadataParserStatus ret = metadata_parser_init(GTFilePrefix.c_str(),
                                                    VAM_FILE,
                                                    0, 0, &jsonHandle);
    if (ret != MP_OK) {
        VAM_ERROR("GTRenderer_init error(%d): metadata_parser_init failed\n",
                    (int)ret);
    }

    inputFile.close();

    return VAM_OK;
}

int32_t GTRenderer_deinit(void *handle) {
    VAM_INFO("GTRenderer_deinit");
    if (jsonHandle != nullptr) {
        metadata_parser_deinit(jsonHandle);
    }

    return VAM_OK;
}

int32_t GTRenderer_process(void *handle, struct vaapi_frame_info *pFrameInfo) {
    if (pFrameInfo == nullptr || pFrameInfo->frame_l_data[0] == nullptr) {
        VAM_ERROR("GTRenderer process error: found NULL pointer");
        return VAM_NULLPTR;
    }

    char *metadata_str = nullptr;
    MetadataParserStatus ret = metadata_parser_get_next_frame(jsonHandle,
                                                              &metadata_str);
    if (metadata_str == nullptr) {
        VAM_ERROR("GTRenderer_init error(%d): metadata_parser_get_next_frame "
                    "failed\n", (int)ret);
    } else {
        ret = metadata_parser_parse_metadata_frame(jsonHandle,
                                                   metadata_str,
                                                   &json_metadata);
    }

    return VAM_OK;
}

int32_t GTRenderer_add_rule(void *handle, vaapi_rule *rule) {
    if (rule == nullptr) {
        VAM_ERROR("GTRenderer_add_rule error: found NULL pointer");
        return VAM_NULLPTR;
    }

    switch (rule->type) {
    case vaapi_event_object_detected:
        VAM_INFO("GTRenderer_add_rule: successful.");
        break;
    default:
        VAM_INFO("GTRenderer_add_rule error: found unsupported event:%d.\n",
                rule->type);
        break;
    }

    return VAM_OK;
}

int32_t GTRenderer_del_rule(void *handle, const char *config_id) {
    VAM_INFO("GTRenderer_del_rule");
    return VAM_OK;
}

int32_t GTRenderer_enroll_obj(void *handle,
                              struct vaapi_enrollment_info *enroll_info) {
    if (enroll_info == nullptr) {
        return VAM_NULLPTR;
    }

    return VAM_OK;
}

int32_t GTRenderer_disenroll_obj(void *handle, const char *obj_id) {
    return VAM_OK;
}

int32_t GTRenderer_get_event_count(void *handle, uint32_t *event_count) {
    return VAM_OK;
}

int32_t GTRenderer_get_events(void *handle,
                              struct vaapi_event *events,
                              uint32_t num_events) {
    return VAM_OK;
}

int32_t GTRenderer_get_object_count(void *handle, uint32_t *object_count) {
    if (object_count == nullptr) {
        return VAM_NULLPTR;
    }

    *object_count = json_metadata->cnt_objects;

    return VAM_OK;
}

int32_t GTRenderer_get_objects(void *handle,
                               vaapi_object *objects,
                               uint32_t obj_count) {
    if (objects == nullptr) {
        return VAM_NULLPTR;
    }

    VAM_INFO("GTRenderer_get_objects:");
    for(uint32_t i = 0; i < json_metadata->cnt_objects; i++)
    {
        vaapi_object &o = objects[i];
        struct metadata_object_t &objSrc = json_metadata->objects[i];

        o.id = objSrc.object_id;
        snprintf(o.display_name, VAAPI_NAME_LEN, "GT obj-%d", o.id);

        metadata_rect_t &posSrc = objSrc.appearance_descriptor.location;
        o.pos.x = posSrc.x;
        o.pos.y = posSrc.y;
        o.pos.width = posSrc.width;
        o.pos.height = posSrc.height;
        o.confidence = objSrc.appearance_descriptor.object_type_confidence;

        VAM_INFO("\tobj %d (id %d): (%d, %d, %d, %d)\n",
                i, o.id, o.pos.x, o.pos.y, o.pos.width, o.pos.height);
    }

    return VAM_OK;
}

int32_t GTRenderer_get_target_FPS(void *handle, uint8_t *target_fps) {
    if (target_fps == nullptr) {
        return VAM_NULLPTR;
    }

    *target_fps = 60;
    return VAM_OK;
}

int32_t GTRenderer_is_event_supported(void *handle,
                                      vaapi_event_type type,
                                      uint8_t *is_supported) {
    if (is_supported == nullptr) {
        return VAM_NULLPTR;
    }

    switch (type) {
    case vaapi_event_object_detected:
        *is_supported = 1;
        break;
    default:
        *is_supported = 0;
        break;
    }

    return VAM_OK;
}

int32_t GTRenderer_is_depends_on_other_event(void *handle,
                                             vaapi_event_type type,
                                             uint8_t *is_depended) {
    if (is_depended == nullptr) {
        return VAM_NULLPTR;
    }

    switch (type) {
    default:
        *is_depended = 0;
    }

    return VAM_OK;
}

