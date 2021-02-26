/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _VAENGINEAPI_H_
#define _VAENGINEAPI_H_

/*****************************************
 VAM engine API
 release version: 1607182238
 *****************************************/

#include "vaapi_common.h"

typedef struct vaapi_alg_module_t {
    /**
    * The API version of the implemented engine (algorithm module). The engine owner is
    * responsible for updating the version when a module interface has changed.
    */
    const char * module_api_version;

    /** Name of this module */
    const char *name;

    /** Author/owner/implementor of the module */
    const char *author;

    /** Modules methods */
    struct vaapi_module_methods_t* methods;

    /** reserved field */
    void *reserved[3];
} vaapi_alg_module_t;

typedef struct vaapi_module_methods_t {
    /* init */
    int32_t (*init)(const struct vaapi_source_info *source_info, void **handle);

    /* stop */
    int32_t (*deinit)( void *handle);

    /* process a frame */
    int32_t (*process)(void *handle, struct vaapi_frame_info *frame_info);

    /* add/update a new va rule */
    int32_t (*add_rule)(void *handle, vaapi_rule *rule);

    /* delete a va rule */
    int32_t (*del_rule)(void *handle, const char *rule_id);

    /* enroll an object */
    int32_t (*enroll_obj)(void *handle,
                          struct vaapi_enrollment_info *enroll_info);

    /* dis-enroll an object */
    int32_t (*disenroll_obj)(void *handle, const char *obj_id);

    /* get total event count for current frame */
    int32_t (*get_event_count)(void *handle, uint32_t *event_count);

    /* get events for current frame */
    int32_t (*get_events)(void *handle,
                          struct vaapi_event *events,
                          uint32_t num_events);

    /* get total object count for current frame */
    int32_t (*get_object_count)(void *handle, uint32_t *obj_count);

    /* get objects for current frame */
    int32_t (*get_objects)(void *handle,
                           vaapi_object *objects,
                           uint32_t obj_count);

    /* desired frame rate for this va engine */
    int32_t (*get_target_FPS)(void *handle, uint8_t *desired_fps);

    /* is event type supported? *is_supported value: 0 1 */
    int32_t (*is_event_supported)(void *handle,
                                  vaapi_event_type type,
                                  uint8_t *is_supported);

    /* is process has evented object dependecy? *is_depended value: 0 1 */
    int32_t (*is_depends_on_other_event)(void *handle,
                                         vaapi_event_type type,
                                         uint8_t *is_depended);
} vaapi_module_methods_t;


#endif  // #ifndef _VAENGINEAPI_H_
