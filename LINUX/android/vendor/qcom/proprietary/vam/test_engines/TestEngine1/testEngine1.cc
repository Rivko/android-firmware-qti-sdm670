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

#define VAM_INFO(fmt, args...) ALOGD(fmt, ##args)
#define VAM_ERROR(fmt, args...) ALOGE(fmt, ##args)

extern "C" {
    int32_t test1_init(const struct vaapi_source_info *sourceInfo,
                       void **handle);
    int32_t test1_deinit(void *handle);

    int32_t test1_process(void *handle, struct vaapi_frame_info *pFrameInfo);

    int32_t test1_add_rule(void *handle, vaapi_rule *rule);
    int32_t test1_del_rule(void *handle, const char *config_id);

    int32_t test1_enroll_obj(void *handle,
                             struct vaapi_enrollment_info *enroll_info);
    int32_t test1_disenroll_obj(void *handle, const char *obj_id);

    int32_t test1_get_event_count(void *handle, uint32_t *event_count);
    int32_t test1_get_events(void *handle,
                             struct vaapi_event *events,
                             uint32_t num_events);
    int32_t test1_get_object_count(void *handle, uint32_t *object_count);
    int32_t test1_get_objects(void *handle,
                              vaapi_object *objects,
                              uint32_t obj_count);
    int32_t test1_get_target_FPS(void *handle, uint8_t *target_fps);

    int32_t test1_is_event_supported(void *handle,
                                     vaapi_event_type type,
                                     uint8_t *is_supported);
    int32_t test1_is_depends_on_other_event(void *handle,
                                            vaapi_event_type type,
                                            uint8_t *is_depended);
}

vaapi_module_methods_t va_methods =
{
    .init                 = test1_init,
    .deinit               = test1_deinit,
    .process              = test1_process,
    .add_rule             = test1_add_rule,
    .del_rule             = test1_del_rule,
    .enroll_obj           = test1_enroll_obj,
    .disenroll_obj        = test1_disenroll_obj,
    .get_event_count      = test1_get_event_count,
    .get_events           = test1_get_events,
    .get_object_count     = test1_get_object_count,
    .get_objects          = test1_get_objects,
    .get_target_FPS       = test1_get_target_FPS,
    .is_event_supported   = test1_is_event_supported,
    .is_depends_on_other_event = test1_is_depends_on_other_event
};

vaapi_alg_module_t VA_ALG_INFO_SYM =
{
    .module_api_version      = "1.0",
    .name                    = "Test Engine 1",
    .author                  = "Qualcomm",
    .methods                 = &va_methods,
    .reserved                = {NULL, NULL, NULL}
};

static struct vaapi_source_info sourceInfo;
static uint64_t ptsNow = 0;

int32_t test1_init(const struct vaapi_source_info *pSourceInfo, void **handle) {
    if (pSourceInfo == nullptr) {
        return VAM_NULLPTR;
    }

    memcpy(&sourceInfo, pSourceInfo, sizeof(vaapi_source_info));
    switch (sourceInfo.img_format) {
    case vaapi_format_YUVJ420P:
        VAM_INFO("test2 init: img format: vaapi_format_YUVJ420P");
    case vaapi_format_yv12:
        VAM_INFO("test2 init: img format: vaapi_format_yv12");
        break;
    default:
        VAM_ERROR("test2 init error: img format %d not supported. \n:",
                  sourceInfo.img_format);
        return VAM_OK;
    }

    return VAM_OK;
}

int32_t test1_deinit(void *handle) {
    return VAM_OK;
}

int32_t test1_process(void *handle, struct vaapi_frame_info *pFrameInfo) {
    if (pFrameInfo == nullptr || pFrameInfo->frame_l_data[0] == nullptr) {
        VAM_ERROR("test1 process error: found NULL pointer");
        return VAM_NULLPTR;
    }

    VAM_ERROR("test1_process: process 30 ms. pFrameInfo->obj_count=%d\n",
              pFrameInfo->obj_count);
    ptsNow = pFrameInfo->time_stamp;

    return VAM_OK;
}

int32_t test1_add_rule(void *handle, vaapi_rule *rule) {
    if (rule == nullptr) {
        VAM_ERROR("test1_add_rule error: found NULL pointer");
        return VAM_NULLPTR;
    }

    switch (rule->type) {
    case vaapi_event_object_detected:
        VAM_INFO("test1_add_rule: successful.");
        break;
    default:
        VAM_ERROR("test1_add_rule error: found unsupported event:%d.\n",
                rule->type);
        break;
    }

    return VAM_OK;
}

int32_t test1_del_rule(void *handle, const char *config_id) {
    VAM_INFO("test1_del_rule");
    return VAM_OK;
}

int32_t test1_enroll_obj(void *handle,
                         struct vaapi_enrollment_info *enroll_info) {
    if (enroll_info == nullptr) {
        return VAM_NULLPTR;
    }

    return VAM_OK;
}

int32_t test1_disenroll_obj(void *handle, const char *obj_id) {
    return VAM_OK;
}

int32_t test1_get_event_count(void *handle, uint32_t *event_count) {
    if (event_count == nullptr) {
        return VAM_NULLPTR;
    }

    static int idx = 0;
    if (idx++ % 20 == 0) {
        *event_count = 1;
    }

    return VAM_OK;
}

int32_t test1_get_events(void *handle,
                         struct vaapi_event *events,
                         uint32_t num_events) {
    if (events == nullptr) {
        return VAM_NULLPTR;
    }

    static int idx = 0;
    if (idx++ % 20 == 0) {
        VAM_INFO("testEngine1 test1_get_events: event generated");
        snprintf(events[0].id, VAAPI_UUID_LEN, "event-%d", idx);
        events[0].type = vaapi_event_object_detected;
        events[0].pts = ptsNow;
        vaapi_object &o = events[0].obj;
        o.id = 1;
        snprintf(o.display_name, VAAPI_NAME_LEN, "testEngine1 obj1");
        o.pos.x = 100;
        o.pos.y = 100;
        o.pos.width = 384;
        o.pos.height = 480;
    }

    return VAM_OK;
}

int32_t test1_get_object_count(void *handle, uint32_t *object_count) {
    if (object_count == nullptr) {
        return VAM_NULLPTR;
    }

    *object_count = 1;
    VAM_INFO("test1_get_object_count set to 1.");

    return VAM_OK;
}

int32_t test1_get_objects(void *handle,
                          vaapi_object *objects,
                          uint32_t obj_count) {
    if (objects == nullptr) {
        return VAM_NULLPTR;
    }

    vaapi_object &o = objects[0];

    o.id = 1;
    snprintf(o.display_name, VAAPI_NAME_LEN, "testEngine1 obj1");
    o.pos.x = 100;
    o.pos.y = 100;
    o.pos.width = 384;
    o.pos.height = 480;

    return VAM_OK;
}

int32_t test1_get_target_FPS(void *handle, uint8_t *target_fps) {
    if (target_fps == nullptr) {
        return VAM_NULLPTR;
    }

    *target_fps = 60;
    return VAM_OK;
}

int32_t test1_is_event_supported(void *handle,
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

int32_t test1_is_depends_on_other_event(void *handle,
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

