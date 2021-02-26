/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAAPI_COMMON_H_
#define _VAAPI_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define VAAPI_UUID_LEN 37
#define VAAPI_NAME_LEN 128
#define VAAPI_PATH_LEN 512
#define VAAPI_RESERVE_ITEM 5
#define VAAPI_RULE_MAX 16
#define VAAPI_ZONE_MAX 16
#define VAAPI_POINTS_MAX 26

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

enum vaapi_event_type {
    // default type, should always be replaced
    vaapi_event_type_none               = 0,
    // The camera is covered, blurry, or moved
    vaapi_event_camera_tamper_detected  = 1,
    // Motion is detected in the zone
    vaapi_event_motion_detected         = 2,
    // An object crossed a line
    vaapi_event_line_crossed            = 3,
    // An object intrude  an include zone
    vaapi_event_intrusion_detected      = 4,
    // An object is counted when in a zone or cross a line
    vaapi_event_object_counted          = 5,
    // An object stays in a zone for a certain period
    vaapi_event_loitering_detected      = 6,
    // A face is detected
    vaapi_event_face_detected           = 7,
    // A face is recognized
    vaapi_event_face_recognized         = 8,
    // An object is classified
    vaapi_event_object_classified       = 9,
    // An object is detected
    vaapi_event_object_detected         = 10,
    // A client-defined compound event
    vaapi_event_compound_event          = 11,
    // Reserved for extension 1
    vaapi_event_extension_reserved_1    = 12,
    // Reserved for extension 2
    vaapi_event_extension_reserved_2    = 13,
    // Reserved for extension 3
    vaapi_event_extension_reserved_3    = 14,
    // Reserved for extension 4
    vaapi_event_extension_reserved_4    = 15,
    // Reserved for extension 5
    vaapi_event_extension_reserved_5    = 16,
    vaapi_event_size                    = 17
};

enum vaapi_calibration_unit {
    vaapi_calibration_unit_mm      = 0,  // Measured in millimeters
    vaapi_calibration_unit_meter   = 1,  // Measured in meters
    vaapi_calibration_unit_inch    = 2,  // Measured in inches
    vaapi_calibration_unit_foot    = 3   // Measured in feet
};

enum vaapi_scene_type {
    vaapi_scene_unknown = 0,
    vaapi_scene_indoor,
    vaapi_scene_outdoor
};

enum vaapi_zone_type {
    vaapi_include_zone = 0,
    vaapi_exclude_zone = 1,
    vaapi_line = 2,
    vaapi_line_dir = 3,
    vaapi_line_in = 4,
    vaapi_line_out = 5,
    vaapi_include_invalid = 99
};

enum vaapi_img_format {
    vaapi_format_invalid = 0,
    vaapi_format_yv12,
    vaapi_format_nv12,
    vaapi_format_nv21,
    vaapi_format_YUVJ420P,
    vaapi_format_YUVJ422P,
    vaapi_format_YUVJ444P,
    vaapi_format_GRAY8,
    vaapi_format_RGB24,
    vaapi_format_JPEG,
};

enum vaapi_priority {
    vaapi_priority_low,
    vaapi_priority_normal,
    vaapi_priority_high,
    vaapi_priority_critical
};

enum vaapi_object_type {
    vaapi_object_type_unknown = 0,   // default type, should alwyas be replaced
    vaapi_object_type_people = 1,
    vaapi_object_type_vehicle = 2,
    vaapi_object_type_face = 3,
};

struct vaapi_point {
    uint32_t x;
    uint32_t y;
};

struct vaapi_zone {
    char id[VAAPI_UUID_LEN];
    uint32_t point_size;
    vaapi_point points[VAAPI_POINTS_MAX];
    vaapi_zone_type zone_type;
};

struct vaapi_position {
    uint32_t x;
    uint32_t y;
    uint32_t width;   // 0 <= (X + Width) <= 10000
    uint32_t height;  // 0 <= (Y + Height) <= 10000
};

/******************************
TODO
 ******************************/

struct vaapi_rule {
    char id[VAAPI_UUID_LEN];
    char name[VAAPI_NAME_LEN];
    vaapi_event_type type;

    uint8_t sensitivity;
    uint8_t minimum_size;
    vaapi_scene_type scene_type;

    uint32_t zone_size;
    vaapi_zone zones[VAAPI_ZONE_MAX];

    int64_t reserve[VAAPI_RESERVE_ITEM];
    char reserve_str[VAAPI_RESERVE_ITEM][VAAPI_NAME_LEN];
};

enum vaapi_logic_operation {
    vaapi_logic_op_none,
    vaapi_logic_op_or,
    vaapi_logic_op_and,
    vaapi_logic_op_not
};

struct vaapi_compound_rule {
    uint8_t enable;
    vaapi_logic_operation op;
    char rule_id_1[VAAPI_UUID_LEN];
    char rule_id_2[VAAPI_UUID_LEN];

    uint32_t time_interval_1;  // relative_start_time
    uint32_t time_interval_2;  // relative_end_time
};

struct vaapi_source_info {
    char data_folder[VAAPI_PATH_LEN];

    vaapi_img_format img_format;

    uint8_t frame_l_enable;
    uint32_t frame_l_width[3];
    uint32_t frame_l_pitch[3];
    uint32_t frame_l_height[3];
    uint32_t frame_l_scanline[3];

    uint8_t frame_s_enable;
    uint32_t frame_s_width[3];
    uint32_t frame_s_pitch[3];
    uint32_t frame_s_height[3];
    uint32_t frame_s_scanline[3];

    char is_test_mode;
};

/******************************
  vaapi_object_type:
    vaapi_object_type_face
      - reserveStr[0]: recognized person ID
      - reserve[0]: confidence
 ******************************/
struct vaapi_object {
    uint32_t id;
    char display_name[VAAPI_NAME_LEN];
    vaapi_position pos;
    vaapi_object_type type;
    uint8_t confidence;

    int64_t reserve[VAAPI_RESERVE_ITEM];
    char reserve_str[VAAPI_RESERVE_ITEM][VAAPI_NAME_LEN];
};

struct vaapi_frame_info {
    uint64_t time_stamp;
    int32_t fd;

    uint8_t *frame_l_data[3];
    uint8_t *frame_s_data[3];

    uint32_t obj_count;
    vaapi_object *objects;
};

struct vaapi_enrollment_info {
    char id[VAAPI_UUID_LEN];  // FR: personID
    char display_name[VAAPI_NAME_LEN];
    vaapi_object_type type;

    char img_id[VAAPI_UUID_LEN];  // FR: image id
    vaapi_img_format img_format;
    uint8_t *img_data[3];
    uint32_t img_width[3];
    uint32_t img_pitch[3];
    uint32_t img_height[3];
};

/******************************
TODO
 ******************************/

struct vaapi_event {
    char id[VAAPI_UUID_LEN];
    vaapi_event_type type;
    uint64_t pts;
    char parent_id[VAAPI_UUID_LEN];
    vaapi_priority priority;
    char rule_id[VAAPI_UUID_LEN];

    vaapi_object obj;

    int64_t reserve[VAAPI_RESERVE_ITEM];
    char reserve_str[VAAPI_RESERVE_ITEM][VAAPI_NAME_LEN];
};

#ifdef __cplusplus
}
#endif


#endif  // #ifndef _VAAPI_COMMON_H_
