/*
 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAAPI_H_
#define _VAAPI_H_


/*****************************************
 VA API
 release version: 1607182238
 *****************************************/

#include "vaapi_common.h"
#include "vaapi_errors.h"
#include "vaapi_config.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t vaapi_init(const vaapi_source_info *info, const char *dyn_lib_path);
int32_t vaapi_deinit();

int32_t vaapi_set_config(struct vaapi_configuration *va_config);
int32_t vaapi_del_config(struct vaapi_configuration *va_config);
int32_t vaapi_enroll_obj(vaapi_event_type type,
                         vaapi_enrollment_info *enroll_info);
int32_t vaapi_disenroll_obj(vaapi_event_type type, const char *id);

// start/stop engines
int32_t vaapi_run();
int32_t vaapi_stop();

// looping for feeding frames
int32_t vaapi_process(struct vaapi_frame_info *frame_info);

// callback function types
typedef int32_t (*vaapi_event_cb_func)(struct vaapi_event *event,
                                       void* usr_data);
typedef int32_t (*vaapi_metadata_cb_func)(struct vaapi_metadata_frame *frame,
                                          void* usrData);
typedef int32_t (*vaapi_snapshot_cb_func)(struct vaapi_snapshot_info *info,
                                          void *usr_data);
typedef int32_t (*vaapi_frame_processed_cb_func)(uint64_t time_stamp,
                                                 void *usr_data);

// callback function registrations
int32_t vaapi_register_event_cb(vaapi_event_cb_func func, void *usrData);
int32_t vaapi_register_metadata_cb(vaapi_metadata_cb_func func, void* usrData);
int32_t vaapi_register_snapshot_cb(vaapi_snapshot_cb_func func, void* usrData);
int32_t vaapi_register_frame_processed_cb(vaapi_frame_processed_cb_func func,
                                          void* usrData);

int32_t vaapi_is_event_type_supported(vaapi_event_type type,
                                      uint8_t *is_supported);

int32_t vaapi_convert_metadata_to_json(const struct vaapi_metadata_frame *frame,
                                       char *json_str,
                                       uint32_t json_str_size);
int32_t vaapi_convert_event_to_json(const struct vaapi_event *event,
                                    char *json_str,
                                    uint32_t json_str_size);
int32_t vaapi_convert_rule_to_json(const struct vaapi_rule *rule,
                                   char *json_str,
                                   uint32_t json_str_size);
int32_t vaapi_convert_enroll_info_to_json(const struct vaapi_enrollment_info *enroll,
                                          char *json_str,
                                          uint32_t json_str_size);

#ifdef __cplusplus
}
#endif

#endif  // #define _VAAPI_H_
