/* sound_trigger_hw.h
 *
 * This library contains the API to load sound models with
 * DSP and start/stop detection of associated key phrases.
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#ifndef ST_HW_SESSION_H
#define ST_HW_SESSION_H

#include <hardware/sound_trigger.h>
#include <tinyalsa/asoundlib.h>
#include "sound_trigger_platform.h"
#include "st_common_defs.h"

enum sound_trigger_states {
    SES_CREATED = 0x00,
    SES_STARTED = 0x01,
    SES_EVENT_RECEIVED = 0x02,
    SES_BUFFERING = 0x04,
    SES_INVALIDATED = 0x08,
};

typedef enum st_hw_sess_event_id {
    ST_HW_SESS_EVENT_DETECTED,
    ST_HW_SESS_EVENT_MAX
}  st_hw_sess_event_id_t;

typedef struct st_hw_sess_detected_ev {
    uint64_t timestamp;
    int detect_status;
    void* detect_payload;
    size_t payload_size;
} st_hw_sess_detected_ev_t;

typedef struct st_hw_sess_event {
    st_hw_sess_event_id_t event_id;
    union  {
        st_hw_sess_detected_ev_t detected;
    } payload;
} st_hw_sess_event_t;

typedef void (*hw_ses_event_callback_t)(st_hw_sess_event_t *event, void *cookie);

struct st_hw_session {

    struct st_session_fptrs *fptrs;

    unsigned int use_case_idx;

    struct pcm_config  config;
    struct st_vendor_info *vendor_uuid_info;

    unsigned int num_conf_levels;
    unsigned char *conf_levels;

    hw_ses_event_callback_t callback_to_st_session;
    void *cookie;

    st_exec_mode_t exec_mode;

    enum sound_trigger_states state;
    sound_model_handle_t sm_handle; /* used when logging debug info */
    struct sound_trigger_device *stdev;

    st_device_t st_device;
    char *st_device_name;
};

typedef struct st_hw_session st_hw_session_t;

/* Function pointers to routing layers */
typedef void (*sound_trigger_init_session_t)(st_hw_session_t *);
typedef int (*sound_trigger_reg_sm_t)(st_hw_session_t *,
    void*,  sound_trigger_sound_model_type_t sm_type);
typedef int (*sound_trigger_reg_sm_params_t)(st_hw_session_t *,
    unsigned int recognition_mode, bool capture_requested,
    struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void * sm_data);

typedef int (*sound_trigger_dereg_sm_t)(st_hw_session_t *,
    bool capture_requested);
typedef int (*sound_trigger_dereg_sm_params_t)(st_hw_session_t *);
typedef int (*sound_trigger_start_t)(st_hw_session_t *);
typedef int (*sound_trigger_restart_t)(st_hw_session_t *, unsigned int, bool,
   struct sound_trigger_recognition_config *,
   sound_trigger_sound_model_type_t, void *);
typedef int (*sound_trigger_stop_t)(st_hw_session_t *);
typedef int (*sound_trigger_stop_buffering_t)(st_hw_session_t *,
    bool capture_requested);
typedef int (*sound_trigger_set_device_t)(st_hw_session_t *, bool);
typedef int (*sound_trigger_read_pcm_t)(st_hw_session_t *,
                              unsigned char *, unsigned int );
typedef void (*sound_trigger_lab_capture_t)(st_hw_session_t *);
typedef int (*sound_trigger_send_custom_chmix_coeff_t)(st_hw_session_t *, char *);
typedef int (*sound_trigger_disable_device_t)(st_hw_session_t *, bool);
typedef int (*sound_trigger_enable_device_t)(st_hw_session_t *, bool);
typedef int (*sound_trigger_get_param_data_t)(st_hw_session_t *, const char *,
    void *, size_t, size_t *);

struct st_session_fptrs {
    sound_trigger_reg_sm_t reg_sm;
    sound_trigger_reg_sm_params_t reg_sm_params;
    sound_trigger_dereg_sm_t dereg_sm;
    sound_trigger_dereg_sm_params_t dereg_sm_params;
    sound_trigger_start_t start;
    sound_trigger_restart_t restart; /* If sessionn already started used to
        quickly restart the session, in case of lsm this is a no-op in-case of
        gcs it will do a stop/start */
    sound_trigger_stop_t stop;
    sound_trigger_stop_buffering_t stop_buffering; /* directs underlying
    driver to stop sending PCM buffers, this will cause the pcm_read to
    fail eventually, also signals the lab_capture thread in-case it is waiting
    for data to be read from the big buffer */
    sound_trigger_set_device_t set_device;
    sound_trigger_read_pcm_t read_pcm;
    sound_trigger_lab_capture_t process_lab_capture; /* goes into a loop that
        read a chunk of data from PCM device and writes to large buffer that
        is part of the session */
    sound_trigger_send_custom_chmix_coeff_t send_custom_chmix_coeff;
    sound_trigger_disable_device_t disable_device;
    sound_trigger_enable_device_t enable_device;
    sound_trigger_get_param_data_t get_param_data;
};

#endif
