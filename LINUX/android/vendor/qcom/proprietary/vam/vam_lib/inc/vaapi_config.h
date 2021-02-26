/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAAPI_CONFIG_H_
#define _VAAPI_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "vaapi_common.h"

struct vaapi_scene_calibration_object {
    vaapi_position pos;
    uint32_t physical_height;
    vaapi_object_type type;
};

struct vaapi_scene_calibration {
    vaapi_calibration_unit length_unit;
    uint32_t object_size;
    vaapi_scene_calibration_object *objects;
};

struct vaapi_face_group {
    char id[VAAPI_UUID_LEN];
    char name[VAAPI_NAME_LEN];

    uint32_t id_list_size;
    char *id_list[VAAPI_UUID_LEN];
};

struct vaapi_person_info {
    char id[VAAPI_UUID_LEN];
    char name[VAAPI_NAME_LEN];
    char display_name[VAAPI_NAME_LEN];

    uint32_t face_image_size;
    char face_image_name[VAAPI_NAME_LEN];
};

struct vaapi_rule_compound_event {
    char sub_rule_id1[VAAPI_UUID_LEN];
    char sub_rule_id2[VAAPI_UUID_LEN];

    vaapi_logic_operation op;
    int32_t relative_start_time;  // in second
    int32_t relative_end_time;  // in second
};

struct vaapi_configuration {
    char id[VAAPI_UUID_LEN];
    char version[VAAPI_NAME_LEN];
    uint32_t config_size;

    vaapi_scene_type scene_type;
    vaapi_scene_calibration calibration;

    uint32_t rule_size;
    vaapi_rule rules[VAAPI_RULE_MAX];
};

struct vaapi_snapshot_info {
    uint64_t pts;
    uint8_t *img_data[3];
    uint32_t data_size[3];
    char id[VAAPI_UUID_LEN];
    vaapi_event *events;
    uint32_t events_count;
    vaapi_object *objects;
    uint32_t objects_count;
    vaapi_source_info source_info;
    char file_name[VAAPI_NAME_LEN];
};

struct vaapi_metadata_frame {
    uint64_t pts;
    uint32_t object_num;
    vaapi_object *objects;
};


#ifdef __cplusplus
}
#endif


#endif  // #define _VAAPI_CONFIG_H_
