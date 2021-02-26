/* st_session.h
 *
 * This library contains a sound trigger user session abstraction. This
 * abstraction represents a single st session from the application/framework
 * point of view. Internally it can contain up to two HW sessions one for CPE
 * and one for ADSP.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef ST_SESSION_H
#define ST_SESSION_H

#include <stdint.h>
#include <pthread.h>
#include "st_hw_session.h"
#include "sound_trigger_platform.h"
#include "st_common_defs.h"

#define MAX_STATE_NAME_LEN 50

/* Below are the states that can be requested from the client */
enum client_states_t {
    ST_STATE_IDLE,
    ST_STATE_LOADED,
    ST_STATE_ACTIVE
};

typedef enum st_session_event_id {
    ST_SES_EV_LOAD_SM,
    ST_SES_EV_UNLOAD_SM,
    ST_SES_EV_START,
    ST_SES_EV_RESTART,
    ST_SES_EV_STOP,
    ST_SES_EV_DETECTED,
    ST_SES_EV_READ_PCM,
    ST_SES_EV_END_BUFFERING,
    ST_SES_EV_SET_EXEC_MODE,
    ST_SES_EV_SSR_OFFLINE,
    ST_SES_EV_SSR_ONLINE,
    ST_SES_EV_PAUSE,
    ST_SES_EV_RESUME,
    ST_SES_EV_SEND_CHMIX_COEFF,
    ST_SES_EV_SET_DEVICE,
    ST_SES_EV_LOCAL_STOP,
    ST_SES_EV_GET_PARAM_DATA,
} st_session_event_id_t;

struct sound_trigger_device;
struct st_session_ev;
typedef struct st_session_ev st_session_ev_t;

typedef struct st_session st_session_t;
typedef int (*st_session_state_fn_t)(st_session_t*, st_session_ev_t *ev);

struct st_session {
    /* TODO: decouple device below from session */
    struct listnode list_node;
    struct listnode transit_list_node;

    struct sound_trigger_device *stdev;
    struct st_vendor_info *vendor_uuid_info;

    pthread_mutex_t lock;
    st_exec_mode_t exec_mode;
    st_exec_mode_t ssr_transit_exec_mode;
    bool enable_trans;
    struct sound_trigger_phrase_sound_model *sm_data;
    struct sound_trigger_recognition_config *rc_config;

    sound_trigger_sound_model_type_t sm_type;

    sound_model_handle_t sm_handle;
    recognition_callback_t callback;
    void *cookie;
    audio_io_handle_t capture_handle;

    bool capture_requested;

    unsigned int num_phrases;
    unsigned int num_users;
    unsigned int recognition_mode;

    st_hw_session_t *hw_ses_cpe; /* cpe hw session */
    st_hw_session_t *hw_ses_adsp; /* adsp hw session */
    st_hw_session_t *hw_ses_arm; /* arm hw session */
    st_hw_session_t *hw_ses_current; /* current hw session, this is set every
        time there is an exec_mode change and points to one of the above
        hw sessions */
    bool start_received;
    bool restart_received;
    bool paused;
    /* flag gets set if user restarts
        session right after detection before we have a chance to stop the
        session */

    st_session_state_fn_t current_state;
    enum client_states_t client_req_state; /* holds the state that was requested by
        user this is used for recovering from SSR */
    bool device_disabled;
    bool local_stop;
};

/*
 * Initialzies a sound trigger session. Must be called before
 * any other opertaions.
 * Parameters:
 * use_gcs TRUE indicates that GCS should be used for CPE HW
 * session (WCD9340 and beyond) otherwise use
 * LSM(WCD9335 and earlier)
 * exec_mode Indicates initial execution mode for the st
 *  session, whether it is in CPE or ADSP
 */
int st_session_init(st_session_t *st_ses, struct sound_trigger_device *stdev,
    st_exec_mode_t exec_mode, sound_model_handle_t sm_handle);
int st_session_deinit(st_session_t *);

int st_session_load_sm(st_session_t *st_ses);
int st_session_start(st_session_t *st_ses);
int st_session_unload_sm(st_session_t *st_ses);
int st_session_stop(st_session_t *st_ses);
int st_session_read_pcm(st_session_t *st_ses, uint8_t *buff,
    size_t buff_size, /*out*/ size_t *read_size);
int st_session_stop_lab(st_session_t *st_ses);

int st_session_ssr_offline(st_session_t *st_ses,
   enum ssr_event_status ssr_type);
int st_session_ssr_online(st_session_t *st_ses,
   enum ssr_event_status ssr_type);
int st_session_pause(st_session_t *st_ses);
int st_session_resume(st_session_t *st_ses);
void st_session_query_state(st_session_t *st_ses, char *state_name, size_t len);
int st_session_restart(st_session_t *st_ses);
int st_session_send_custom_chmix_coeff(st_session_t *st_ses, char *str);
int st_session_get_config(st_session_t *st_ses, struct pcm_config *config);
int st_session_enable_device(st_session_t *st_ses);
int st_session_disable_device(st_session_t *st_ses);
bool st_session_is_detected(st_session_t *st_ses);
bool st_session_is_active(st_session_t *st_ses);
bool st_session_is_buffering(st_session_t *st_ses);
bool st_session_is_ssr_state(st_session_t *st_ses);
int st_session_set_exec_mode(st_session_t *st_ses, st_exec_mode_t exec);
int st_session_get_param_data(st_session_t *st_ses, const char *param,
    void *payload, size_t payload_size, size_t *param_data_size);

#endif /* ST_SESSION_H */
