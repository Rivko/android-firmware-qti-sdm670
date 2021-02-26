/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef JSON_EVENT_H_
#define JSON_EVENT_H_
#include <stdint.h>

#ifndef JSON_UUID_LENGTH
#define JSON_UUID_LENGTH (37)
#endif

#ifndef RULE_NAME_LENGTH
#define RULE_NAME_LENGTH (128)
#endif

#define VERTEX_CNT_MAX (16)
#define NAME_LENGTH (128)
#define VAEVENT_NAME_LEN (128)
enum JSONEventMediaType{
    JSON_EVENT_FILE = 0,
    JSON_EVENT_RTSP
};
enum JSONEventType{
    JSON_EVENT_UNINITIALIZED = 0,/**< uninitialized event*/
    JSON_EVENT_CAMERA_TAMPER_DETECTED = 1,	/**< camera tamper detected*/
    JSON_EVENT_MOTION_DETECTED = 2,		/**< camera tamper detected*/
    JSON_EVENT_LINECROSSED = 3,			/**< line crossed */
    JSON_EVENT_INTRUSION_DETECTED = 4,    /**< intrusion detected */
    JSON_EVENT_OBJECT_COUNTED = 5,		/**< object counted*/
    JSON_EVENT_LOITERING_DETECTED = 6,	/**< loitering detected*/
    JSON_EVENT_FACE_DETECTED = 7,			/**< face detected*/
    JSON_EVENT_FACE_RECOGNIZED = 8,		/**< face recognized*/
    JSON_EVENT_OBJECT_CLASSIFIED = 9,	/**< object classified*/
    JSON_EVENT_OBJECT_DETECTED = 10,    /**< object detected*/
    JSON_EVENT_COMPOUND_EVENT = 11,     /**<A client-defined compound event*/
    JSON_EVENT_RESERVED_1 = 12,
    JSON_EVENT_RESERVED_2 = 13,
    JSON_EVENT_RESERVED_3 = 14,
    JSON_EVENT_RESERVED_4 = 15,
    JSON_EVENT_RESERVED_5 = 16,
    //JSON_EVENT_ABANDONED_OBJECT_DETECTED = 12,  /**<heat map generated*/
    //JSON_EVENT_HEATMAP_GENERATED = 13,  /**<heat map generated*/
    JSON_EVENT_EXTENSION_RESERVED = 999  /**<extension reserved*/

};

struct json_event_point{
    uint32_t x;
    uint32_t y;
};

struct json_event_base{
    uint64_t event_time;
    uint32_t frame_id;
    enum JSONEventType event_type;
    uint32_t priority;
    char event_id[JSON_UUID_LENGTH];
    char rule_id[JSON_UUID_LENGTH];
    char rule_name[RULE_NAME_LENGTH];
};
struct json_event_tamper{
    uint32_t tamper_type;
    uint32_t duration;
    uint32_t confidence;
};
struct json_event_line_crossed{
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    uint32_t dir;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_object_counted{
    uint32_t count;
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    uint32_t dir;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_intrusion_detected{
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_loitering_detected{
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t duration;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_face_detected{
    uint32_t object_id;
    uint32_t confidence;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_face_recognized{
    uint32_t object_id;
    uint32_t confidence;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    uint32_t duration;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
    char person_id[JSON_UUID_LENGTH];
    char group_id[JSON_UUID_LENGTH];
    char person_name[NAME_LENGTH];
};
struct json_event_object_classified{
    uint32_t object_id;
    uint32_t type;
    uint32_t confidence;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_object_detected{
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_abandoned_object_detected{
    uint32_t object_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t duration;
    uint32_t cnt_zone_vertices;
    struct json_event_point points[VERTEX_CNT_MAX];
    char zone_id[JSON_UUID_LENGTH];
};
struct json_event_heat_map{
    uint64_t start_time;
    uint64_t end_time;
    uint32_t cells_per_row;
    uint32_t rows_per_column;
    uint16_t * data;
};
struct json_event_compound_event{
    char sub_event_id1[JSON_UUID_LENGTH];
    char sub_event_id2[JSON_UUID_LENGTH];
    uint32_t operation;                      //0:AND, 1:OR, 2:NOT
};
struct json_event_extension_reserved{
    uint32_t size;
    char *payload;
};
union json_event_body{
    struct json_event_tamper tamper;
    struct json_event_line_crossed line_crossed;
    struct json_event_object_counted object_counted;
    struct json_event_intrusion_detected intrusion_detected;
    struct json_event_loitering_detected loitering_detected;
    struct json_event_face_detected face_detected;
    struct json_event_face_recognized face_recognized;
    struct json_event_object_classified object_classified;
    struct json_event_object_detected object_detected;
    struct json_event_abandoned_object_detected abandoned_object_detected;
    struct json_event_heat_map heat_map;
    struct json_event_compound_event compound_event;
    struct json_event_extension_reserved extension;
};
struct json_event{
    struct json_event_base base;
    union json_event_body body;
    int64_t reserve[5];
    char reserve_str[5][VAEVENT_NAME_LEN];
};

#endif