/*
 * Copyright (c) 2016-2018, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <dlfcn.h>
#include <vaapi.h>
#include <VAManager.h>
#include <VAMUtilities.h>
#include <iostream>
#include <string>

static VAManager VAMInstance;
extern "C" int32_t vaapi_init(const vaapi_source_info *info,
                              const char *dyn_lib_path) {
    return VAMInstance.init(info, dyn_lib_path);
}

extern "C" int32_t vaapi_deinit() {
    return VAMInstance.destroy();
}

extern "C" int32_t vaapi_set_config(vaapi_configuration *va_config) {
    return VAMInstance.setConfig(va_config);
}

extern "C" int32_t vaapi_del_config(vaapi_configuration *va_config) {
    return VAMInstance.delConfig(va_config);
}

extern "C" int32_t vaapi_enroll_obj(vaapi_event_type type,
                         vaapi_enrollment_info *enroll_info) {
    return VAMInstance.enrollObj(type, enroll_info);
}

extern "C" int32_t vaapi_disenroll_obj(vaapi_event_type type,
                                       const char *id) {
    return VAMInstance.disenrollObj(type, id);
}

extern "C" int32_t vaapi_run() {
    return VAMInstance.run();
}

extern "C" int32_t vaapi_stop() {
    return VAMInstance.stop();
}

extern "C" int32_t vaapi_process(vaapi_frame_info *frame_info) {
    return VAMInstance.putFrame(frame_info);
}

extern "C" int32_t vaapi_register_event_cb(vaapi_event_cb_func func,
                                           void *usrData) {
    VAMInstance.registerEventCB(func, usrData);
    return VAM_OK;
}

extern "C" int32_t vaapi_register_metadata_cb(vaapi_metadata_cb_func func,
                                              void *usrData) {
    VAMInstance.registerMetadataFrameCB(func, usrData);
    return VAM_OK;
}

extern "C" int32_t vaapi_register_snapshot_cb(vaapi_snapshot_cb_func func,
                                              void *usrData) {
    VAMInstance.registerSnapshotCB(func, usrData);
    return VAM_OK;
}

extern "C" int32_t vaapi_register_frame_processed_cb(
                                          vaapi_frame_processed_cb_func func,
                                          void *usrData) {
    VAMInstance.registerFrameProcessedCB(func, usrData);
    return VAM_OK;
}

extern "C" int32_t vaapi_is_event_type_supported(vaapi_event_type type,
                                                 uint8_t *is_supported) {
    if (is_supported == nullptr) {
        return VAM_NULLPTR;
    }

    *is_supported = VAMInstance.isEventSupported(type);

    return VAM_OK;
}

extern "C" int32_t vaapi_convert_metadata_to_json(
                                       const struct vaapi_metadata_frame *frame,
                                       char *json_str,
                                       uint32_t json_str_size) {
    std::string jsonStr;
    int ret = VAMInstance.convertMetadataToJSON(frame, &jsonStr);

    if (ret != VAM_OK) {
        return ret;
    }

    if (jsonStr.size() >= json_str_size) {
        return VAM_STR_OVERSIZE;
    }

    snprintf(json_str, json_str_size, "%s", jsonStr.c_str());
    return VAM_OK;
}

extern "C" int32_t vaapi_convert_event_to_json(const struct vaapi_event *event,
                                               char *json_str,
                                               uint32_t json_str_size) {
    std::string jsonStr;
    int ret = VAMInstance.convertEventToJSON(event, &jsonStr);
    if (ret != VAM_OK) {
        return ret;
    }

    if (jsonStr.size() >= json_str_size) {
        return VAM_STR_OVERSIZE;
    }

    snprintf(json_str, json_str_size, "%s", jsonStr.c_str());
    return VAM_OK;
}

extern "C" int32_t vaapi_convert_rule_to_json(const struct vaapi_rule *rule,
                                              char *json_str,
                                              uint32_t json_str_size) {
    std::string jsonStr;
    int ret = VAMInstance.convertRuleToJSON(rule, &jsonStr);
    if (ret != VAM_OK) {
        return ret;
    }

    if (jsonStr.size() >= json_str_size) {
        return VAM_STR_OVERSIZE;
    }

    snprintf(json_str, json_str_size, "%s", jsonStr.c_str());
    return VAM_OK;
}

extern "C" int32_t vaapi_convert_enroll_info_to_json(const struct vaapi_enrollment_info *enroll,
                                                     char *json_str,
                                                     uint32_t json_str_size) {
    std::string jsonStr;
    int ret = VAMInstance.convertEnrollInfoToJSON(enroll, &jsonStr);
    if (ret != VAM_OK) {
        return ret;
    }

    if (jsonStr.size() >= json_str_size) {
        return VAM_STR_OVERSIZE;
    }

    snprintf(json_str, json_str_size, "%s", jsonStr.c_str());
    return VAM_OK;
}
