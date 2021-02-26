/*
 * This file defines all the APIs for interacting with Graphite service and
 *  modules
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#include "graphite_common.h"

#define GCS_DETECTION_ENGINE_CONFIG_KW_DET_ENABLE 0x1
#define GCS_DETECTION_ENGINE_CONFIG_USER_VER_ENABLE 0x2
#define GCS_DETECTION_ENGINE_CONFIG_FAILURE_DET_ENABLE 0x4

#define GCS_DETECTION_ENGINE_EVENT_DETECTED 0
#define GCS_DETECTION_ENGINE_EVENT_FAILED 1

#define GCS_READ_CMDRSP_STATUS_SUCCESS 0

struct gcs_det_engine_config_param {
    struct graphite_cal_header cal_hdr;
    uint16_t mode;
    uint16_t custom_payload_sz;
} __packed;

struct gcs_det_engine_custom_config_param {
    struct graphite_cal_header cal_hdr;
};

struct gcs_det_engine_event {
    uint16_t status;
    uint16_t custom_payload_sz;
    /* followed by uint8_t*custom_payload_sz for conf levels */
};

struct gcs_cmd_read_payload_t {
    uint32_t size_in_bytes;
};

struct gcs_cmd_readrsp_payload_t {
    uint32_t status;
    uint32_t flags;
    uint32_t timestamp_lsw;
    uint32_t timestapm_msw;
};

struct gcs_data_cmd_t {
    struct graphite_data_cmd_hdr hdr;
    union {
        struct gcs_cmd_read_payload_t read;
    } payload;
} __packed;

/* custom params defined by STHAL to VA engine */

/*
 * start and stop individual engines in single
 * module
 */
struct gcs_det_engine_start_custom_config {
    struct graphite_cal_header cal_hdr;
    uint16_t minor_version; /* Param version. Currently fixed value 1 */
    uint16_t enable; /* 1: start engine 0: stop engine */
    uint32_t token; /* unique token per engine */
} __packed;

struct gcs_det_engine_start_params {
    uint16_t minor_version; /* Param version. Currently fixed value 1 */
    uint32_t token; /* unique token per engine sent in
                       @gcs_det_engine_start_custom_config*/
} __packed;

/*
 * Expected format of event when used with
 * @gcs_det_engine_start_custom_config
 */
struct gcs_det_engine_extended_event {
    struct gcs_det_engine_event ev_params;
    struct gcs_det_engine_start_params start_params;
    /* followed by VA engine specific custom payload */
} __packed;

