/* st_hw_session_gcs.h
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#ifndef ST_HW_SESSION_GCS
#define ST_HW_SESSION_GCS

#include <pthread.h>
#include "sound_trigger_platform.h"
#include "st_hw_session.h"
#include "st_common_defs.h"

#define ST_GRAPHITE_LAB_PERIOD_COUNT (4)
#define ST_BYTES_PER_SAMPLE (2)
#define MAX_DETECTION_PAYLOAD_SZ 256

#define ST_GRAPHITE_LAB_PERIOD_SIZE_IN_SAMPLES \
    CALCULATE_PERIOD_SIZE(ST_GRAPHITE_LAB_BUF_DURATION_MS, \
     SOUND_TRIGGER_SAMPLING_RATE_16000, ST_GRAPHITE_LAB_PERIOD_COUNT, 32)

#define ST_GCS_READ_BUF_SIZE (ST_GRAPHITE_LAB_PERIOD_SIZE_IN_SAMPLES * \
                             ST_BYTES_PER_SAMPLE)

struct sound_trigger_device;

typedef struct st_hw_session_gcs {
    st_hw_session_t common;
    struct st_gcs_params* gcs_usecase;
    uint32_t graph_handle;
    uint32_t loaded_sm_handle;
    void* nonpersistent_cal;
    size_t nonpersistent_cal_size;
    struct gcs_det_engine_start_custom_config *start_engine_cal;
    size_t start_engine_cal_size;

    uint8_t detect_payload[MAX_DETECTION_PAYLOAD_SZ];
    size_t detect_payload_size;

    bool detection_signaled;
    bool exit_buffering;
    bool exit_detection;
    bool lab_processing_active;

    pthread_t callback_thread;
    pthread_cond_t callback_thread_cond;
    pthread_mutex_t callback_thread_lock;

    pthread_mutex_t circ_buff_lock;
    pthread_cond_t circ_buff_cond;
    uint8_t *circ_buff;
    uint8_t *circ_buff_tail;
    uint32_t circ_buff_sz;
    uint8_t *rd_ptr; /* points to location that was read last */
    uint8_t *wr_ptr; /* point to next empty location where we will write */
    uint32_t read_rsp_cnt; /* keeps track of how many read responses were
        received so we know how many read_req to send when sender thread runs */
    uint32_t unread_bytes;

    uint8_t *mulaw_op_buf;
} st_hw_session_gcs_t;

int st_hw_sess_gcs_init(st_hw_session_t *const p_ses,
   hw_ses_event_callback_t cb, void *cookie, st_exec_mode_t exec_mode,
   struct st_vendor_info *v_info, sound_model_handle_t sm_handle,
   struct sound_trigger_device *stdev);

void st_hw_sess_gcs_deinit(st_hw_session_t *const p_ses);

int st_hw_gcs_init(void);
void st_hw_gcs_deinit(void);
#endif /* ST_HW_SESSION_GCS */
