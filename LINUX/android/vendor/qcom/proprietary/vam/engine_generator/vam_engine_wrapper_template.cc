/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <math.h>
#include <VAM/vam_engine_api.h>
#include <VAM/vaapi_errors.h>

#include <iostream>
#include <cstdio>

extern "C" {
    int32_t va_engine_init(const struct vaapi_source_info *sourceInfo,
                           void **handle);
    int32_t va_engine_deinit(void *handle);

    int32_t va_engine_process(void *handle,
                              struct vaapi_frame_info *pFrameInfo);

    int32_t va_engine_add_rule(void *handle, vaapi_rule *rule);
    int32_t va_engine_del_rule(void *handle, const char *config_id);

    int32_t va_engine_enroll_obj(void *handle,
                                 struct vaapi_enrollment_info *enroll_info);
    int32_t va_engine_disenroll_obj(void *handle, const char *obj_id);

    int32_t va_engine_get_event_count(void *handle, uint32_t *event_count);
    int32_t va_engine_get_events(void *handle,
                                 struct vaapi_event *events,
                                 uint32_t num_events);
    int32_t va_engine_get_object_count(void *handle, uint32_t *object_count);
    int32_t va_engine_get_objects(void *handle,
                                  vaapi_object *objects,
                                  uint32_t obj_count);
    int32_t va_engine_get_target_FPS(void *handle, uint8_t *desired_fps);

    int32_t va_engine_is_event_supported(void *handle,
                                         vaapi_event_type type,
                                         uint8_t *is_supported);
    int32_t va_engine_is_depends_on_other_event(void *handle,
                                                vaapi_event_type type,
                                                uint8_t *is_depended);
}

vaapi_module_methods_t va_methods =
{
    .init                      = va_engine_init,
    .deinit                    = va_engine_deinit,
    .process                   = va_engine_process,
    .add_rule                  = va_engine_add_rule,
    .del_rule                  = va_engine_del_rule,
    .enroll_obj                = va_engine_enroll_obj,
    .disenroll_obj             = va_engine_disenroll_obj,
    .get_event_count           = va_engine_get_event_count,
    .get_events                = va_engine_get_events,
    .get_object_count          = va_engine_get_object_count,
    .get_objects               = va_engine_get_objects,
    .get_target_FPS            = va_engine_get_target_FPS,
    .is_event_supported        = va_engine_is_event_supported,
    .is_depends_on_other_event = va_engine_is_depends_on_other_event
};

vaapi_alg_module_t VA_ALG_INFO_SYM =
{
    .module_api_version      = "1.0",
    .name                    = "Custom Engine Template",
    .author                  = "Qualcomm",
    .methods                 = &va_methods,
};
int32_t va_engine_init(const struct vaapi_source_info *pSourceInfo,
                       void **handle) {
    printf("va_engine_init\n");
    return VAM_OK;
}

int32_t va_engine_deinit(void *handle) {
    printf("va_engine_deinit\n");
    return VAM_OK;
}

int32_t va_engine_process(void *handle, struct vaapi_frame_info *pFrameInfo) {
    static int frameIDX = 0;
    printf("va_engine_process()..%d\n", frameIDX++);
    return VAM_OK;
}

int32_t va_engine_add_rule(void *handle, vaapi_rule *rule) {
    printf("va_engine_add_rule\n");
    return VAM_OK;
}

int32_t va_engine_del_rule(void *handle, const char *config_id) {
    printf("va_engine_del_rule\n");
    return VAM_OK;
}

int32_t va_engine_enroll_obj(void *handle,
                             struct vaapi_enrollment_info *enroll_info) {
    printf("va_engine_enroll_obj\n");
    return VAM_OK;
}

int32_t va_engine_disenroll_obj(void *handle, const char *obj_id) {
    printf("va_engine_disenroll_obj\n");
    return VAM_OK;
}

int32_t va_engine_get_event_count(void *handle, uint32_t *event_count) {
    printf("va_engine_get_events_count\n");
    if (event_count == nullptr) {
        return VAM_NULLPTR;
    }

    *event_count = 0;
    return VAM_OK;
}

int32_t va_engine_get_events(void *handle,
                             struct vaapi_event *events,
                             uint32_t num_events) {
    printf("va_engine_get_events\n");
    return VAM_OK;
}

int32_t va_engine_get_object_count(void *handle, uint32_t *object_count) {
    printf("va_engine_get_meta_size\n");
    if (object_count == nullptr) {
        return VAM_NULLPTR;
    }

    *object_count = 2;
    return VAM_OK;
}

int32_t va_engine_get_objects(void *handle,
                              vaapi_object *objects,
                              uint32_t obj_count) {
    printf("va_engine_get_meta\n");
    if (obj_count != 2) {
        return VAM_FAIL;
    }

    static float t = 0.0f;

    objects[0].type = vaapi_object_type_unknown;
    objects[0].id = 1;
    objects[0].confidence = 100;
    snprintf(objects[0].display_name, VAAPI_NAME_LEN, "hi1");
    objects[0].pos.x = static_cast<int>(50.0 * cos(t)) + 300;
    objects[0].pos.y = static_cast<int>(50.0 * sin(t)) + 300;
    objects[0].pos.width = 200;
    objects[0].pos.height = 200;

    objects[1].type = vaapi_object_type_unknown;
    objects[1].id = 10;
    objects[1].confidence = 100;
    snprintf(objects[1].display_name, VAAPI_NAME_LEN, "hi2");
    objects[1].pos.x = static_cast<int>(50.0 * cos(-t)) + 400;
    objects[1].pos.y = static_cast<int>(50.0 * sin(-t)) + 400;
    objects[1].pos.width = 200;
    objects[1].pos.height = 200;
    t += 0.2f;
    return VAM_OK;
}

int32_t va_engine_get_target_FPS(void *handle, uint8_t *desired_fps) {
    if (desired_fps == nullptr) {
        return VAM_NULLPTR;
    }

    printf("va_engine_get_target_FPS\n");
    *desired_fps = 10;

    return VAM_OK;
}

int32_t va_engine_is_event_supported(void *handle,
                                     vaapi_event_type type,
                                     uint8_t *is_supported) {
    printf("va_engine_is_event_supported\n");
    if (is_supported == nullptr) {
        return VAM_NULLPTR;
    }

    switch (type) {
    case vaapi_event_object_detected:
        *is_supported = 1;
        break;
    default:
        *is_supported = 0;
    }

    return VAM_OK;
}

int32_t va_engine_is_depends_on_other_event(void *handle,
                                            vaapi_event_type type,
                                            uint8_t *is_depended) {
    printf("va_engine_is_depends_on_other_event\n");
    if (is_depended == nullptr) {
        return VAM_NULLPTR;
    }

    switch (type) {
    // case vaapi_event_object_detected:
    //    *is_supported = 1;
    //    break;
    default:
        *is_depended = 0;
    }

    return VAM_OK;
}
