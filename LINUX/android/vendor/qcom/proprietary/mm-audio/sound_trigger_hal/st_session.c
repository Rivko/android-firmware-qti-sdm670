/*
 * This library contains the state machine for a single sound trigger
 * user session. This state machine implements logic for handling all user
 * interactions, detectinos, SSR and Audio Concurencies.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "sound_trigger_hw"
#define ATRACE_TAG (ATRACE_TAG_HAL)
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <cutils/trace.h>
#include "st_session.h"
#include "st_hw_session.h"
#include "st_hw_session_lsm.h"
#include "st_hw_session_gcs.h"
#include "sound_trigger_hw.h"
#include "st_hw_session_pcm.h"
#include "st_hw_extn.h"

/* below enum used in cleanup in error scenarios */
enum hw_session_err_mask {
    HW_SES_ERR_MASK_DEVICE_SET = 0x1,
    HW_SES_ERR_MASK_REG_SM = 0x2,
    HW_SES_ERR_MASK_REG_SM_PARAM = 0x4,
    HW_SES_ERR_MASK_STARTED = 0x8,
    HW_SES_ERR_MASK_BUFFERING = 0x10,
};

#define STATE_TRANSITION(st_session, new_state_fn)\
do {\
        st_session->current_state = new_state_fn;\
        ALOGD("session[%d]: %s ---> %s %s", st_session->sm_handle, __func__, \
            #new_state_fn, st_session->paused ? "(paused)" : "");\
} while(0)

#define DISPATCH_EVENT(ST_SESSION, EVENT, STATUS)\
do {\
    STATUS = ST_SESSION->current_state(ST_SESSION, &EVENT);\
} while (0)

static inline int process_detection_event
(
    st_session_t *st_ses, uint64_t timestamp, int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_recognition_event **event
);

static int idle_state_fn(st_session_t *st_ses, st_session_ev_t *ev);
static int loaded_state_fn(st_session_t *st_ses, st_session_ev_t *ev);
static int active_state_fn(st_session_t *st_ses, st_session_ev_t *ev);
static int detected_state_fn(st_session_t *st_ses, st_session_ev_t *ev);
static int buffering_state_fn(st_session_t *st_ses, st_session_ev_t *ev);
static int ssr_state_fn(st_session_t *st_ses, st_session_ev_t *ev);

typedef struct st_session_loadsm_payload {
    struct sound_trigger_phrase_sound_model *sm_data;
} st_session_loadsm_payload_t;

typedef struct st_session_start_payload {
    void *config;
    size_t config_size;
    recognition_callback_t callback;
    void *cookie;
} st_session_start_payload_t;

typedef struct st_session_read_pcm_payload {
    void *out_buff;
    size_t out_buff_size;
    size_t *actual_read_size;
} st_session_readpcm_payload_t;

typedef struct st_session_get_param_payload {
    const char *param;
    void *payload;
    size_t payload_size;
    size_t *param_data_size;
} st_session_getparam_payload_t;

struct st_session_ev {
    st_session_event_id_t ev_id;
    union {
        st_session_loadsm_payload_t loadsm;
        st_session_start_payload_t start;
        st_hw_sess_detected_ev_t detected;
        st_exec_mode_t exec_mode;
        st_session_readpcm_payload_t readpcm;
        enum ssr_event_status ssr;
        char *chmix_coeff_str;
        bool enable;
        st_session_getparam_payload_t getparam;
    } payload;
};

ST_DBG_DECLARE(FILE *lab_fp = NULL; static int file_cnt = 0);

void hw_sess_cb(st_hw_sess_event_t *hw_event, void *cookie)
{
    st_session_t *st_ses = (st_session_t *)cookie;
    int status = 0;
    int lock_status = 0;

    if (!hw_event || !cookie) {
        ALOGE("%s: received NULL params", __func__);
        return;
    }

    switch (hw_event->event_id) {
    case ST_HW_SESS_EVENT_DETECTED:
        {
            st_session_ev_t ev;
            ev.ev_id = ST_SES_EV_DETECTED;
            ev.payload.detected = hw_event->payload.detected;

            do {
                lock_status = pthread_mutex_trylock(&st_ses->lock);
            } while (lock_status && !st_ses->device_disabled &&
                     (st_ses->exec_mode != ST_EXEC_MODE_NONE) &&
                     (st_ses->current_state != ssr_state_fn));

            if (st_ses->device_disabled) {
                ALOGV("%s:[%d] device switch in progress, ignore event",
                      __func__, st_ses->sm_handle);
            } else if (st_ses->exec_mode == ST_EXEC_MODE_NONE) {
                ALOGV("%s:[%d] transition in progress, ignore event",
                      __func__, st_ses->sm_handle);
            } else if (st_ses->current_state == ssr_state_fn) {
                ALOGV("%s:[%d] SSR handling in progress, ignore event",
                      __func__, st_ses->sm_handle);
            } else if (!lock_status) {
                DISPATCH_EVENT(st_ses, ev, status);
            }

            if (!lock_status)
                pthread_mutex_unlock(&st_ses->lock);
            break;
        }

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;
    };

}

static void do_hw_sess_cleanup(st_session_t *st_ses, st_hw_session_t *hw_ses,
    enum hw_session_err_mask err)
{
    if (err & HW_SES_ERR_MASK_BUFFERING)
        hw_ses->fptrs->stop_buffering(hw_ses, st_ses->capture_requested);

    if (err & HW_SES_ERR_MASK_STARTED)
        hw_ses->fptrs->stop(hw_ses);

    if (err & HW_SES_ERR_MASK_REG_SM_PARAM)
        hw_ses->fptrs->dereg_sm_params(hw_ses);

    if (err & HW_SES_ERR_MASK_DEVICE_SET)
        hw_ses->fptrs->set_device(hw_ses, false);

    if (err & HW_SES_ERR_MASK_REG_SM)
        hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
}

static void reg_hal_event_session(st_session_t *p_ses, st_hw_session_t *hw_ses)
{
    struct sound_trigger_event_info event_info;
    /* Pass the pcm information to audio hal for capturing LAB */
    if (p_ses->capture_requested && p_ses->stdev->audio_hal_cb) {
        ALOGD("%s: ST_EVENT_SESSION_REGISTER capture_handle %d p_ses %p",
              __func__, p_ses->capture_handle, (void *)p_ses);
        event_info.st_ses.p_ses = (void *)p_ses;
        event_info.st_ses.config = hw_ses->config;
        event_info.st_ses.capture_handle = p_ses->capture_handle;
        /*
         * set pcm to NULL as this version of st_hal doesn't pass pcm to
         * audio HAL
         */
        event_info.st_ses.pcm = NULL;
        p_ses->stdev->audio_hal_cb(ST_EVENT_SESSION_REGISTER, &event_info);
    }
}

static void dereg_hal_event_session(st_session_t *p_ses)
{
    struct sound_trigger_event_info event_info;
    /* Indicate to audio hal that buffering is stopped to stop reading LAB data */
    if (p_ses->capture_requested && p_ses->stdev->audio_hal_cb) {
        ALOGD("%s: ST_EVENT_SESSION_DEREGISTER capture_handle %d p_ses %p",
           __func__, p_ses->capture_handle, p_ses);
        event_info.st_ses.p_ses = (void *)p_ses;
        event_info.st_ses.capture_handle = p_ses->capture_handle;
        event_info.st_ses.pcm = NULL;
        p_ses->stdev->audio_hal_cb(ST_EVENT_SESSION_DEREGISTER, &event_info);
    }
}

static int start_hw_session(st_session_t *st_ses, st_hw_session_t *hw_ses, bool load_sm)
{
    int status = 0, err = 0;

    if (load_sm) {
        status = hw_ses->fptrs->reg_sm(hw_ses, st_ses->sm_data,
            st_ses->sm_type);
        if (status) {
            ALOGE("%s:[%d] failed to reg_sm err %d", __func__,
                st_ses->sm_handle, status);
            goto cleanup;
        }
        err |= HW_SES_ERR_MASK_REG_SM;
    }

    status = hw_ses->fptrs->set_device(hw_ses, true);
    if (status) {
        ALOGE("%s:[%d] failed to set_device err %d", __func__,
            st_ses->sm_handle, status);
        goto cleanup;
    }
    err |= HW_SES_ERR_MASK_DEVICE_SET;

    status = hw_ses->fptrs->reg_sm_params(hw_ses,
        st_ses->recognition_mode, st_ses->capture_requested,
        st_ses->rc_config, st_ses->sm_type, st_ses->sm_data);
    if (status) {
        ALOGE("%s:[%d] failed to reg_sm_params err %d", __func__,
            st_ses->sm_handle, status);
        goto cleanup;
    }
    err |= HW_SES_ERR_MASK_REG_SM_PARAM;

    status = hw_ses->fptrs->start(hw_ses);
    if (status) {
        ALOGE("%s:[%d] failed to start err %d", __func__,
            st_ses->sm_handle, status);
        goto cleanup;
    }
    err |= HW_SES_ERR_MASK_STARTED;

    return status;

cleanup:
    do_hw_sess_cleanup(st_ses, hw_ses, err);
    return status;
}

static int stop_hw_session(st_session_t *st_ses, st_hw_session_t *hw_ses, bool unload_sm)
{
    int status = 0;
    int rc = 0;

    status = hw_ses->fptrs->stop(hw_ses);
    if (status) {
        ALOGE("%s:[%d] failed to stop err %d", __func__,
            st_ses->sm_handle, status);
        rc = status;
    }

    status = hw_ses->fptrs->dereg_sm_params(hw_ses);
    if (status) {
        ALOGE("%s:[%d] failed to dereg_sm_params err %d", __func__,
            st_ses->sm_handle, status);
        rc = status;
    }

    status = hw_ses->fptrs->set_device(hw_ses, false);
    if (status) {
        ALOGE("%s:[%d] failed to set_device err %d", __func__,
            st_ses->sm_handle, status);
        rc = status;
    }
    if (unload_sm) {
        status = hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        if (status) {
            ALOGE("%s:[%d] failed to dereg_sm, err %d", __func__,
                st_ses->sm_handle, status);
            rc = status;
        }
    }

    return rc;
}

static int start_session(st_session_t *st_ses, st_hw_session_t *hw_ses, bool load_sm)
{
    int status = 0;

    /*
     * The reg_hal_event_session call must be after start_hw_session. This is
     * important for when load_sm is true, because reg_sm sets the correct pcm
     * config for the current hw session. That pcm config is then sent to audio hal.
     */
    status = start_hw_session(st_ses, hw_ses, load_sm);
    reg_hal_event_session(st_ses, hw_ses);
    return status;
}

static int stop_session(st_session_t *st_ses, st_hw_session_t *hw_ses, bool unload_sm)
{
    dereg_hal_event_session(st_ses);
    return stop_hw_session(st_ses, hw_ses, unload_sm);
}

static int idle_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_hw_session_t *hw_ses = st_ses->hw_ses_current;

    /* skip parameter check as this is an internal funciton */
    ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle, ev->ev_id);

    switch (ev->ev_id) {
    case ST_SES_EV_LOAD_SM:
        if (!st_ses->sm_data) {
            ALOGE("%s: sound model data is not initialzed", __func__);
            status = -EINVAL;
            break;
        }

        status = hw_ses->fptrs->reg_sm(hw_ses, st_ses->sm_data,
            st_ses->sm_type);
        if (status) {
            if (st_ses->stdev->ssr_offline_received) {
                st_ses->client_req_state = ST_STATE_LOADED;
                STATE_TRANSITION(st_ses, ssr_state_fn);
                /* Send success to client because the failure is recovered
                 * internally from SSR.
                 */
                status = 0;
            } else {
                ALOGE("%s:[%d] failed to reg sm, err %d", __func__,
                    st_ses->sm_handle, status);
            }
            break;
        }

        STATE_TRANSITION(st_ses, loaded_state_fn);
        break;

    case ST_SES_EV_SET_EXEC_MODE:
        st_ses->exec_mode = ev->payload.exec_mode;
        if (ST_EXEC_MODE_CPE == st_ses->exec_mode)
            st_ses->hw_ses_current = st_ses->hw_ses_cpe;
        else if (ST_EXEC_MODE_ADSP == st_ses->exec_mode)
            st_ses->hw_ses_current = st_ses->hw_ses_adsp;
        /* remain in current state */
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        break;

    case ST_SES_EV_RESUME:
        st_ses->paused = false;
        break;

    case ST_SES_EV_SSR_OFFLINE:
        STATE_TRANSITION(st_ses, ssr_state_fn);
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = -EIO;
        break;

    case ST_SES_EV_GET_PARAM_DATA:
        status = -EIO;
        break;

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;
    };

    return status;
}

static int loaded_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_hw_session_t *hw_ses = st_ses->hw_ses_current;
    st_hw_session_t *new_hw_ses = NULL;
    st_exec_mode_t new_exec_mode;

    /* skip parameter check as this is an internal funciton */
    ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle, ev->ev_id);

    switch (ev->ev_id) {
    case ST_SES_EV_RESUME:
        if (!st_ses->paused)
            break;
        st_ses->paused = false;
        if (st_ses->client_req_state != ST_STATE_ACTIVE)
            break;
        /* If the session is paused and client_req_state is active, fall through
         * and handle similarly to start/restart.
         */
    case ST_SES_EV_START:
    case ST_SES_EV_RESTART:
        st_ses->client_req_state = ST_STATE_ACTIVE;
        if (!st_ses->paused) {
            status = start_session(st_ses, hw_ses, false);
            if (status) {
                if (st_ses->stdev->ssr_offline_received) {
                    hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
                    STATE_TRANSITION(st_ses, ssr_state_fn);
                    /* Send success to client because the failure is recovered
                     * internally from SSR.
                     */
                    status = 0;
                } else {
                    ALOGE("%s:[%d] failed to start session, err %d", __func__,
                        st_ses->sm_handle, status);
                }
                break;
            }
            STATE_TRANSITION(st_ses, active_state_fn);
        }
        break;

    case ST_SES_EV_UNLOAD_SM:
        status = hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        if (status) {
            /* since this is a teardown scenario dont fail here */
            ALOGE("%s:[%d] dereg_sm failed with err %d", __func__,
                st_ses->sm_handle, status);
            status = 0;
        }
        STATE_TRANSITION(st_ses, idle_state_fn);
        break;

    case ST_SES_EV_SSR_OFFLINE:
        /* exec mode can be none if ssr occurs during a transition */
        if (st_ses->exec_mode != ST_EXEC_MODE_NONE)
            hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        st_ses->client_req_state = ST_STATE_LOADED;
        STATE_TRANSITION(st_ses, ssr_state_fn);
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        break;

    case ST_SES_EV_STOP:
        st_ses->client_req_state = ST_STATE_LOADED;
        break;

    case ST_SES_EV_SET_EXEC_MODE:
        new_exec_mode = ev->payload.exec_mode;

        if ((st_ses->exec_mode != new_exec_mode) &&
            st_ses->enable_trans) {

            if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
                st_ses->exec_mode = ST_EXEC_MODE_NONE;
                /* unload sm for current hw session */
                status = hw_ses->fptrs->dereg_sm(hw_ses,
                            st_ses->capture_requested);
                if (status) {
                    ALOGE("%s:[%d] dereg_sm failed with err %d", __func__,
                        st_ses->sm_handle, status);
                    break;
                }
            }

            if (new_exec_mode == ST_EXEC_MODE_NONE)
                break;

            /* load sm to new hw_ses */
            if (ST_EXEC_MODE_CPE == new_exec_mode)
                new_hw_ses = st_ses->hw_ses_cpe;
            else if (ST_EXEC_MODE_ADSP == new_exec_mode)
                new_hw_ses = st_ses->hw_ses_adsp;
            else {
                ALOGE("%s: unknown execution mode %d", __func__,
                    new_exec_mode);
                status = -EINVAL;
                break;
            }

            status = new_hw_ses->fptrs->reg_sm(new_hw_ses,
                        st_ses->sm_data, st_ses->sm_type);
            if (status) {
                ALOGE("%s:[%d] reg_sm failed with err %d", __func__,
                       st_ses->sm_handle, status);
                break;
            }
            /* switch hw sessions only if successful*/
            st_ses->exec_mode = new_exec_mode;
            st_ses->hw_ses_current = new_hw_ses;
            /* remain in current state */
        }
        break;

    case ST_SES_EV_SET_DEVICE:
        /*
         * This event handling is needed for certain graphs which
         * have multiple buffering modules with a single voice wakeup
         * module in each usecase.
         */
        if (!ev->payload.enable)
            status = hw_ses->fptrs->disable_device(hw_ses, false);
        else
            status = hw_ses->fptrs->enable_device(hw_ses, false);

        break;

    case ST_SES_EV_READ_PCM:
        /*
         * set status to failure this will tell AHAL to
         * provide zero buffers to client
         */
        status = -EIO;
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = -EIO;
        break;

    case ST_SES_EV_GET_PARAM_DATA:
        status = hw_ses->fptrs->get_param_data(hw_ses,
                      ev->payload.getparam.param, ev->payload.getparam.payload,
                      ev->payload.getparam.payload_size,
                      ev->payload.getparam.param_data_size);
        break;

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;

    };

    return status;
}

static int active_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_hw_session_t *hw_ses = st_ses->hw_ses_current;
    st_session_ev_t stop_ev =  { .ev_id = ST_SES_EV_LOCAL_STOP };
    st_hw_session_t *new_hw_ses = NULL;
    st_exec_mode_t new_exec_mode;

    /* skip parameter check as this is an internal funciton */
    ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle, ev->ev_id);

    switch (ev->ev_id) {
    case ST_SES_EV_SET_EXEC_MODE:
        new_exec_mode = ev->payload.exec_mode;

        /* if no change in mode or dynamic transition not enabled then noop */
        if ((new_exec_mode == st_ses->exec_mode) || !st_ses->enable_trans)
            break;

        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            ALOGV("%s: disable current session", __func__);
            st_ses->exec_mode = ST_EXEC_MODE_NONE;
            status = stop_session(st_ses, hw_ses, true);
            if (status)
                break;
        }

        if (new_exec_mode == ST_EXEC_MODE_NONE)
            break;

        if (ST_EXEC_MODE_CPE == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_cpe;
        else if (ST_EXEC_MODE_ADSP == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_adsp;
        else {
            ALOGE("%s: unknown execution mode %d", __func__,
                new_exec_mode);
            status = -EINVAL;
            break;
        }

        ALOGV("%s: enable current session", __func__);
        status = start_session(st_ses, new_hw_ses, true);
        if (status)
            break;

        /* set new exec mode and current session */
        st_ses->exec_mode = new_exec_mode;
        st_ses->hw_ses_current = new_hw_ses;
        ALOGV("%s: end transition", __func__);
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        /* Fall through to handle pause events similarly to stop events. */
    case ST_SES_EV_STOP:
        if (st_ses->paused)
            st_ses->client_req_state = ST_STATE_ACTIVE;
        else
            st_ses->client_req_state = ST_STATE_LOADED;
        status = stop_session(st_ses, hw_ses, false);
        if (status) {
            if (st_ses->stdev->ssr_offline_received) {
                STATE_TRANSITION(st_ses, ssr_state_fn);
                hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
                /* Send success to client because the failure is recovered
                 * internally from SSR.
                 */
                status = 0;
            } else {
                ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                    st_ses->sm_handle, status);
            }
            break;
        }

        STATE_TRANSITION(st_ses, loaded_state_fn);
        break;

    case ST_SES_EV_DETECTED:
        {
            size_t payload_size = ev->payload.detected.payload_size;
            struct sound_trigger_recognition_event *event = NULL;
            recognition_callback_t callback;
            bool capture_requested;

            status = process_detection_event(st_ses,
                ev->payload.detected.timestamp,
                ev->payload.detected.detect_status,
                ev->payload.detected.detect_payload,
                payload_size, &event);
            if (status || !event) {
                ALOGE("%s:[%d] process_detection_event failed err %d", __func__,
                    st_ses->sm_handle, status);
                /* Stop buffering if this is not a successful detection and
                    LAB is triggered in hw automatically */
                hw_ses->fptrs->stop_buffering(hw_ses,
                    st_ses->capture_requested);

                if (event)
                    free(event);
                break;
            }

            /*
             * change to new state before invoking user callback, this will
             * ensure that if user calls start_recognition immediately from the
             * callback it will be handled by one of the two states below
             */
            if (!status && st_ses->capture_requested) {
                ST_DBG_FILE_OPEN_WR(lab_fp, ST_DEBUG_DUMP_LOCATION, "lab_capture",
                    "bin", file_cnt++);
                STATE_TRANSITION(st_ses, buffering_state_fn);
            } else {
                STATE_TRANSITION(st_ses, detected_state_fn);
            }

            if (!st_ses->callback) {
                ALOGE("%s:[%d] received detection event but no callback",
                    __func__, st_ses->sm_handle);
                status = -EINVAL;
                if (event)
                    free(event);
                break;
            }
            callback = st_ses->callback;

            /*
             * store the current capture requested in-case a new start comes
             * once we exist the critical-section.
             * In this case we continue to operate based on previous capture requested
             * setting untill the new session start is processed and resets the state
             */
            capture_requested = st_ses->capture_requested;
            pthread_mutex_unlock(&st_ses->lock);

            /*
             * callback to user, assumption is that client does not
             * block in the callback waiting for data otherwise will be a deadlock
             */
            ALOGD("%s:[%d] invoking the client callback",
                __func__, st_ses->sm_handle);
            ATRACE_ASYNC_END("sthal: event_callback", hw_ses->sm_handle);
            callback(event, st_ses->cookie);

            if (capture_requested && (ev->payload.detected.detect_status ==
                RECOGNITION_STATUS_SUCCESS)) {
                /* Cache lab data to internal buffers (blocking call) */
                hw_ses->fptrs->process_lab_capture(hw_ses);
            }

            /*
             * It is possible that the client may start/stop/unload the session
             * with the same lock held, before we aqcuire lock here.
             * We need further processing only if client starts in detected state,
             * else return gracefully.
             */
             do {
                status = pthread_mutex_trylock(&st_ses->lock);
             } while (status && (st_ses->current_state == detected_state_fn));

            if (st_ses->current_state != detected_state_fn) {
                ALOGV("%s:[%d] client stopped after callback, lock status %d",
                      __func__, st_ses->sm_handle, status);
                if (!status)
                    pthread_mutex_unlock(&st_ses->lock);
                free(event);
                status = 0;
                break;
            }

            /*
             * Stop detection locally after every detection as per
             * sound trigger API. Control here means we are in detected state
             * without buffering. If buffering is enabled stop will happen
             * in stop_lab context from client.
             */
            DISPATCH_EVENT(st_ses, stop_ev, status);

            free(event);
            break;
        }

    case ST_SES_EV_SSR_OFFLINE:
        STATE_TRANSITION(st_ses, ssr_state_fn);
        /* exec mode can be none if ssr occurs during a transition */
        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            hw_ses->fptrs->stop(hw_ses);
            hw_ses->fptrs->dereg_sm_params(hw_ses);
            hw_ses->fptrs->set_device(hw_ses, false);
            hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        }

        st_ses->client_req_state = ST_STATE_ACTIVE;
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = hw_ses->fptrs->send_custom_chmix_coeff(hw_ses,
                                  ev->payload.chmix_coeff_str);
        break;

    case ST_SES_EV_SET_DEVICE:
        if (!ev->payload.enable)
            status = hw_ses->fptrs->disable_device(hw_ses, true);
        else
            status = hw_ses->fptrs->enable_device(hw_ses, true);

        break;

    case ST_SES_EV_READ_PCM:
        /*
         * buffering could have been stopped internally
         * and switched to active state ex: transitions.
         * set status to failure this will tell AHAL to
         * provide zero buffers to client
         */
        status = -EIO;
        break;

    case ST_SES_EV_GET_PARAM_DATA:
        status = hw_ses->fptrs->get_param_data(hw_ses,
                      ev->payload.getparam.param, ev->payload.getparam.payload,
                      ev->payload.getparam.payload_size,
                      ev->payload.getparam.param_data_size);
        break;

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;

    };

    return status;
}

static int detected_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_exec_mode_t new_exec_mode;
    st_hw_session_t *hw_ses = st_ses->hw_ses_current;
    st_hw_session_t *new_hw_ses = NULL;

    /* skip parameter check as this is an internal funciton */
    ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle, ev->ev_id);

    switch (ev->ev_id) {
    case ST_SES_EV_LOCAL_STOP:
        /* This event is issued internally after sending event to client.
         * Control can be here with below scenarios
         * 1. Client may or may not have issued start again
         * 2. A device switch may or may not have happened in between.
         * 3. SSR may or may not have happened
         */

        /* If device switch is in progress delay the processing till
         * switching is completed */
        if (st_ses->device_disabled)
            break;

        /* If transition is in progress, session is already stopped */
        if (ST_EXEC_MODE_NONE == st_ses->exec_mode) {
            st_ses->local_stop = true;
            break;
        }

        if (st_ses->restart_received) {
            /* session already restarted without any config changes */
            st_ses->restart_received = false;
            hw_ses->fptrs->restart(hw_ses, st_ses->recognition_mode,
                st_ses->capture_requested, st_ses->rc_config, st_ses->sm_type,
                st_ses->sm_data);
            STATE_TRANSITION(st_ses, active_state_fn);
        } else if (st_ses->start_received) {
            /* session already started but config has changed stop and restart */
            st_ses->start_received = false;
            status = stop_session(st_ses, hw_ses, false);
            if (status)
                break;
            status = start_session(st_ses, hw_ses, false);
            if (status)
                break;
            STATE_TRANSITION(st_ses, active_state_fn);
        } else {
            /* session not started by client */
            status = stop_session(st_ses, hw_ses, false);
            if (status) {
                if (st_ses->stdev->ssr_offline_received) {
                    STATE_TRANSITION(st_ses, ssr_state_fn);
                    hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
                    st_ses->client_req_state = ST_STATE_LOADED;
                    /* Send success to client because the failure is recovered
                     * internally from SSR.
                     */
                    status = 0;
                } else {
                    ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                        st_ses->sm_handle, status);
                }
                break;
            }

            STATE_TRANSITION(st_ses, loaded_state_fn);
        }
        break;

    case ST_SES_EV_STOP:
        /*
         * It is possible that the client can issue stop after detection
         * callback
         */
        status = stop_session(st_ses, hw_ses, false);
        if (status) {
            if (st_ses->stdev->ssr_offline_received) {
                STATE_TRANSITION(st_ses, ssr_state_fn);
                hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
                st_ses->client_req_state = ST_STATE_LOADED;
                /* Send success to client because the failure is recovered
                 * internally from SSR.
                 */
                status = 0;
            } else {
                ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                    st_ses->sm_handle, status);
            }
            break;
        }

        STATE_TRANSITION(st_ses, loaded_state_fn);
        break;

    case ST_SES_EV_START:
        st_ses->start_received = true;
        break;

    case ST_SES_EV_RESTART:
        st_ses->restart_received = true;
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        break;

    case ST_SES_EV_SSR_OFFLINE:
        /*
         * Ignore return status during SSR handling
         * as the ADSP or CPE might be down so these
         * calls would fail. Exec mode can be none if
         * ssr occurs during a transition.
         */
        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            hw_ses->fptrs->stop(hw_ses);
            hw_ses->fptrs->dereg_sm_params(hw_ses);
            hw_ses->fptrs->set_device(hw_ses, false);
            hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        }

        st_ses->client_req_state = ST_STATE_ACTIVE;
        STATE_TRANSITION(st_ses, ssr_state_fn);
        break;

    case ST_SES_EV_SET_EXEC_MODE:
        new_exec_mode = ev->payload.exec_mode;

        /* if no change in mode or dynamic transition not enabled then noop */
        if ((new_exec_mode == st_ses->exec_mode) || !st_ses->enable_trans)
            break;

        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            st_ses->exec_mode = ST_EXEC_MODE_NONE;
            status = stop_session(st_ses, hw_ses, true);
            if (status)
                break;
        }

        if (new_exec_mode == ST_EXEC_MODE_NONE)
            break;

        /* switch to new hw session */
        if (ST_EXEC_MODE_CPE == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_cpe;
        else if (ST_EXEC_MODE_ADSP == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_adsp;
        else {
            ALOGE("%s: unknown execution mode %d", __func__,
                new_exec_mode);
            status = -EINVAL;
            break;
        }

        /* start/restart received while transition in progress */
        if (st_ses->restart_received || st_ses->start_received) {
            st_ses->start_received = st_ses->restart_received = false;
            /* make false to handle case where local stop event was also received */
            st_ses->local_stop = false;
            status = start_session(st_ses, new_hw_ses, true);
            if (status)
                break;

            st_ses->exec_mode = new_exec_mode;
            st_ses->hw_ses_current = new_hw_ses;
            STATE_TRANSITION(st_ses, active_state_fn);
            break;
        }

        /* check if local stop was received in between transition */
        if (st_ses->local_stop) {
            st_ses->local_stop = false;
            status = new_hw_ses->fptrs->reg_sm(new_hw_ses, st_ses->sm_data,
                                               st_ses->sm_type);
            if (status) {
                ALOGE("%s:[%d] failed to reg_sm err %d", __func__,
                    st_ses->sm_handle, status);
                break;
            }
            /* set new exec mode, current session and change to loaded state */
            st_ses->exec_mode = new_exec_mode;
            st_ses->hw_ses_current = new_hw_ses;
            STATE_TRANSITION(st_ses, loaded_state_fn);
            break;
        }

        /*
         * start new hw session and stay in detected state as
         * client restart and stop concurrency scenarios are handled
         * in this state
         */
        status = start_session(st_ses, new_hw_ses, true);
        if (status)
            break;

        st_ses->exec_mode = new_exec_mode;
        st_ses->hw_ses_current = new_hw_ses;
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = -EINVAL;
        break;

    case ST_SES_EV_SET_DEVICE:
        if (!ev->payload.enable) {
            status = stop_session(st_ses, hw_ses, false);
            if (status)
                break;
            st_ses->device_disabled = true;
        } else {
            /* Handle delayed ST_SES_EV_LOCAL_STOP event process
             * due to device switch
             */
            if (st_ses->restart_received || st_ses->start_received) {
                /* Since session is already stoped, start again */
                st_ses->start_received = st_ses->restart_received = false;
                status = start_session(st_ses, hw_ses, false);
                if (status)
                    break;
                STATE_TRANSITION(st_ses, active_state_fn);
            } else {
                /* Session not started by client after detection. Since it is
                 * already stopped, move to loaded state */
                STATE_TRANSITION(st_ses, loaded_state_fn);
            }
            st_ses->device_disabled = false;
        }
        break;

    case ST_SES_EV_GET_PARAM_DATA:
        status = hw_ses->fptrs->get_param_data(hw_ses,
                      ev->payload.getparam.param, ev->payload.getparam.payload,
                      ev->payload.getparam.payload_size,
                      ev->payload.getparam.param_data_size);
        break;

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;

    };

    return status;
}

static int buffering_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_hw_session_t *hw_ses = st_ses->hw_ses_current;
    st_exec_mode_t new_exec_mode;
    st_hw_session_t *new_hw_ses = NULL;

    /* skip parameter check as this is an internal funciton */

    switch (ev->ev_id) {
    case ST_SES_EV_READ_PCM:
        ALOGVV("%s:[%d] handle event id %d", __func__, st_ses->sm_handle,
            ev->ev_id);

        /* Note: this function may block if there is no PCM data ready*/
        hw_ses->fptrs->read_pcm(hw_ses, ev->payload.readpcm.out_buff,
            ev->payload.readpcm.out_buff_size);
        ST_DBG_FILE_WRITE(lab_fp, ev->payload.readpcm.out_buff,
            ev->payload.readpcm.out_buff_size);
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        /* Fall through to handle pause events similarly to stop events. */
    case ST_SES_EV_END_BUFFERING:
    case ST_SES_EV_STOP:
        ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle,
            ev->ev_id);
        /*
         * These events are related to a tear down sequence, so transition to
         * loaded state even if there is a failure.
         */
        status = hw_ses->fptrs->stop_buffering(hw_ses,
            st_ses->capture_requested);
        if (status)
            ALOGE("%s:[%d] failed to stop_buffering err %d", __func__,
                st_ses->sm_handle, status);

        if (st_ses->paused)
            st_ses->client_req_state = ST_STATE_ACTIVE;
        else
            st_ses->client_req_state = ST_STATE_LOADED;
        status = stop_session(st_ses, hw_ses, false);
        if (status)
            ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                st_ses->sm_handle, status);

        ST_DBG_FILE_CLOSE(lab_fp);
        STATE_TRANSITION(st_ses, loaded_state_fn);
        break;

    case ST_SES_EV_START:
    case ST_SES_EV_RESTART:
        ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle,
            ev->ev_id);

        /*
         * client erroneously starts detection again, in this
         * case stop buffering and restart the session so that any new
         * parameters take effect. NOTE: this should not happen normally
         * but we are handling it in this way for robustness.
         */
        status = hw_ses->fptrs->stop_buffering(hw_ses,
            st_ses->capture_requested);
        if (status && !st_ses->stdev->ssr_offline_received) {
            ALOGE("%s:[%d] failed to stop_buffering err %d", __func__,
                st_ses->sm_handle, status);
            break;
        }

        status = stop_session(st_ses, hw_ses, false);
        if (status && !st_ses->stdev->ssr_offline_received) {
            ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                st_ses->sm_handle, status);
            break;
        }

        status = start_session(st_ses, hw_ses, false);
        if (status) {
            if (st_ses->stdev->ssr_offline_received) {
                hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
                st_ses->client_req_state = ST_STATE_ACTIVE;
                STATE_TRANSITION(st_ses, ssr_state_fn);
                /* Send success to client because the failure is recovered
                 * internally from SSR.
                 */
                status = 0;
            } else {
                ALOGE("%s:[%d] failed to start session, err %d", __func__,
                    st_ses->sm_handle, status);
            }
            break;
        }

        STATE_TRANSITION(st_ses, active_state_fn);
        break;

    case ST_SES_EV_SSR_OFFLINE:
        ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle,
            ev->ev_id);

        /*
         *  Ignore return status during SSR handling
         *  as the ADSP or CPE might be down so these
         *  calls would fail. Exec mode can be none if
         *  ssr occurs during a transition.
         */
        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            hw_ses->fptrs->stop_buffering(hw_ses,
                st_ses->capture_requested);
            hw_ses->fptrs->stop(hw_ses);
            hw_ses->fptrs->dereg_sm_params(hw_ses);
            hw_ses->fptrs->set_device(hw_ses, false);
            hw_ses->fptrs->dereg_sm(hw_ses, st_ses->capture_requested);
        }

        /* transition state */
        if (st_ses->paused)
            st_ses->client_req_state = ST_STATE_ACTIVE;
        else
            st_ses->client_req_state = ST_STATE_LOADED;
        STATE_TRANSITION(st_ses, ssr_state_fn);
        break;

    case ST_SES_EV_SET_EXEC_MODE:
        ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle,
            ev->ev_id);

        new_exec_mode = ev->payload.exec_mode;

        /* if no change in mode or dynamic transition not enabled then noop */
        if ((new_exec_mode == st_ses->exec_mode) || !st_ses->enable_trans)
            break;

        if (st_ses->exec_mode != ST_EXEC_MODE_NONE) {
            st_ses->exec_mode = ST_EXEC_MODE_NONE;
            status = hw_ses->fptrs->stop_buffering(hw_ses,
                              st_ses->capture_requested);
            if (status) {
                ALOGE("%s:[%d] failed to stop_buffering err %d", __func__,
                    st_ses->sm_handle, status);
                break;
            }

            status = stop_session(st_ses, hw_ses, true);
            if (status) {
                ALOGE("%s:[%d] failed to stop session, err %d", __func__,
                    st_ses->sm_handle, status);
                break;
            }
        }

        if (new_exec_mode == ST_EXEC_MODE_NONE)
            break;

        /* switch to new hw session */
        if (ST_EXEC_MODE_CPE == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_cpe;
        else if (ST_EXEC_MODE_ADSP == new_exec_mode)
            new_hw_ses = st_ses->hw_ses_adsp;
        else {
            ALOGE("%s: unknown execution mode %d", __func__,
                new_exec_mode);
            status = -EINVAL;
            break;
        }

        status = start_session(st_ses, new_hw_ses, true);
        if (status) {
            ALOGE("%s:[%d] failed to start hw ses, err %d", __func__,
                st_ses->sm_handle, status);
            break;
        }
        st_ses->exec_mode = new_exec_mode;
        st_ses->hw_ses_current = new_hw_ses;
        STATE_TRANSITION(st_ses, active_state_fn);
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = hw_ses->fptrs->send_custom_chmix_coeff(hw_ses,
                                  ev->payload.chmix_coeff_str);
        break;

    case ST_SES_EV_GET_PARAM_DATA:
        status = hw_ses->fptrs->get_param_data(hw_ses,
                      ev->payload.getparam.param, ev->payload.getparam.payload,
                      ev->payload.getparam.payload_size,
                      ev->payload.getparam.param_data_size);
        break;

    default:
        ALOGD("%s:[%d] unhandled event, id %d", __func__, st_ses->sm_handle,
            ev->ev_id);
        break;

    };

    return status;
}

static int ssr_state_fn(st_session_t *st_ses, st_session_ev_t *ev)
{
    int status = 0;
    st_session_ev_t load_ev =  { .ev_id = ST_SES_EV_LOAD_SM };
    st_session_ev_t start_ev =  { .ev_id = ST_SES_EV_START };
    st_session_ev_t exec_mode_ev =  { .ev_id = ST_SES_EV_SET_EXEC_MODE };

    /* skip parameter check as this is an internal funciton */
    ALOGD("%s:[%d] handle event id %d", __func__, st_ses->sm_handle, ev->ev_id);

    switch (ev->ev_id) {
    case ST_SES_EV_SSR_ONLINE:
        ALOGV("%s:[%d] SSR ONLINE received", __func__, st_ses->sm_handle);

        STATE_TRANSITION(st_ses, idle_state_fn);

        if ((st_ses->ssr_transit_exec_mode == ST_EXEC_MODE_CPE) ||
            (st_ses->ssr_transit_exec_mode == ST_EXEC_MODE_ADSP)) {
            exec_mode_ev.payload.exec_mode = st_ses->ssr_transit_exec_mode;
            DISPATCH_EVENT(st_ses, exec_mode_ev, status);
            st_ses->ssr_transit_exec_mode = ST_EXEC_MODE_NONE;
        }

        if ((ST_STATE_ACTIVE == st_ses->client_req_state) ||
            (ST_STATE_LOADED == st_ses->client_req_state)) {
            DISPATCH_EVENT(st_ses, load_ev, status);
            if (status)
                break;
        }

        if ((ST_STATE_ACTIVE == st_ses->client_req_state) &&
            !st_ses->paused) {
            DISPATCH_EVENT(st_ses, start_ev, status);
            if (status)
                break;
        }

        break;

    case ST_SES_EV_LOAD_SM:
        if (ST_STATE_IDLE == st_ses->client_req_state) {
            st_ses->client_req_state = ST_STATE_LOADED;
        } else {
            ALOGE("%s: received unexpected event, client_req_state = %d",
                __func__, st_ses->client_req_state);
        }
        break;

    case ST_SES_EV_UNLOAD_SM:
        if (ST_STATE_LOADED == st_ses->client_req_state) {
            st_ses->client_req_state = ST_STATE_IDLE;
        } else {
            ALOGE("%s: received unexpected event, client_req_state = %d",
                __func__, st_ses->client_req_state);
        }
        break;

    case ST_SES_EV_START:
    case ST_SES_EV_RESTART:
        if (ST_STATE_LOADED == st_ses->client_req_state) {
            st_ses->client_req_state = ST_STATE_ACTIVE;
        } else {
            ALOGE("%s: received unexpected event, client_req_state = %d",
                __func__, st_ses->client_req_state);
        }
        break;

    case ST_SES_EV_STOP:
        if (ST_STATE_ACTIVE == st_ses->client_req_state) {
            st_ses->client_req_state = ST_STATE_LOADED;
        } else {
            ALOGE("%s: received unexpected event, client_req_state = %d",
                __func__, st_ses->client_req_state);
        }
        break;

    case ST_SES_EV_PAUSE:
        st_ses->paused = true;
        break;

    case ST_SES_EV_RESUME:
        st_ses->paused = false;
        break;

    case ST_SES_EV_READ_PCM:
        status = -EIO;
        break;

    case ST_SES_EV_SEND_CHMIX_COEFF:
        status = -EIO;

    case ST_SES_EV_GET_PARAM_DATA:
        status = -EIO;

    case ST_SES_EV_SET_EXEC_MODE:
        st_ses->exec_mode = ev->payload.exec_mode;
        if (ST_EXEC_MODE_CPE == st_ses->exec_mode)
            st_ses->hw_ses_current = st_ses->hw_ses_cpe;
        else if (ST_EXEC_MODE_ADSP == st_ses->exec_mode)
            st_ses->hw_ses_current = st_ses->hw_ses_adsp;
        /* remain in current state */
        break;

    default:
        ALOGD("%s:[%d] unhandled event", __func__, st_ses->sm_handle);
        break;
    };

    return status;
}

static int process_detection_event_keyphrase(
    st_session_t *st_ses, int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_phrase_recognition_event **event)
{
    st_hw_session_t *st_hw_ses = st_ses->hw_ses_current;
    unsigned int i, j;
    int status = 0;
    struct sound_trigger_phrase_recognition_event *local_event = NULL;

    if (st_ses->vendor_uuid_info &&
        st_ses->vendor_uuid_info->smlib_handle) {
        /* if smlib is present, get the event from it else send the
           DSP recieved payload as it is to App */
        /* TODO: checking is_gcs should be avoided here */
        if (st_ses->stdev->is_gcs && (ST_EXEC_MODE_CPE == st_ses->exec_mode)) {
            ALOGD("%s: about to call generate_st_phrase_recognition_event_v2",
                  __func__);
            status = st_ses->vendor_uuid_info->
                generate_st_phrase_recognition_event_v2(st_ses->sm_data,
                st_ses->rc_config, payload, payload_size, &local_event);
        } else {
            status = st_ses->vendor_uuid_info->
                generate_st_phrase_recognition_event(st_ses->sm_data,
                st_ses->rc_config, payload, payload_size, &local_event);
        }

        if (status) {
            ALOGW("%s: smlib fill recognition event failed, status %d",
                __func__, status);
            goto exit;
        }
    } else if (!st_ses->vendor_uuid_info &&
               st_ses->stdev->smlib_handle) {
        /* This is SVA non topology solution */
        /* TODO: checking is_gcs should be avoided here */
        if (st_ses->stdev->is_gcs) {
            status = st_ses->stdev->generate_st_phrase_recognition_event_v2(
                st_ses->sm_data, st_ses->rc_config, payload, payload_size,
                &local_event);
        } else {
            status = st_ses->stdev->generate_st_phrase_recognition_event(
                st_ses->sm_data, st_ses->rc_config, payload,  payload_size,
                &local_event);
        }

        if (status) {
            ALOGW("%s: SVA smlib fill recognition event failed, status\
                %d", __func__, status);
            goto exit;
        }
    } else {
        local_event = calloc(1, sizeof(*local_event) + payload_size);
        if (!local_event) {
            ALOGE("%s: event allocation failed, size %zd", __func__,
                payload_size);
            status = -ENOMEM;
            goto exit;
        }
        memcpy(local_event->phrase_extras,
            st_ses->rc_config->phrases, st_ses->rc_config->num_phrases *
            sizeof(struct sound_trigger_phrase_recognition_extra));
        local_event->num_phrases = st_ses->rc_config->num_phrases;
        local_event->common.data_offset = sizeof(*local_event);
        local_event->common.data_size = payload_size;
        memcpy((char *)local_event + local_event->common.data_offset, payload,
            payload_size);
    }

    /* fill the remaining recognition event parameters not specific
       to soundmodel lib */
    local_event->common.status = detect_status;
    local_event->common.type = st_ses->sm_data->common.type;
    local_event->common.model = st_ses->sm_handle;
    local_event->common.capture_available = st_ses->capture_requested;
    local_event->common.capture_delay_ms = 0;
    local_event->common.capture_preamble_ms = 0;
    local_event->common.audio_config.sample_rate =
        SOUND_TRIGGER_SAMPLING_RATE_16000;
    local_event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;
    local_event->common.audio_config.channel_mask =
        audio_channel_in_mask_from_count(st_hw_ses->config.channels);

    for (i = 0; i < local_event->num_phrases; ++i) {
        ALOGV("%s: [%d] kw_id %d level %d", __func__, i,
            local_event->phrase_extras[i].id,
            local_event->phrase_extras[i].confidence_level);
        for (j = 0; j < local_event->phrase_extras[i].num_levels; ++j) {
            ALOGV("%s: [%d] user_id %d level %d ", __func__, i,
                local_event->phrase_extras[i].levels[j].user_id,
                local_event->phrase_extras[i].levels[j].level);
        }
    }

    ALOGI("%s:[%d]", __func__, st_ses->sm_handle);

    ALOGV("%s:[%d] status=%d, type=%d, model=%d, capture_avaiable=%d, "
        "num_phrases=%d id=%d", __func__,
        st_ses->sm_handle, local_event->common.status, local_event->common.type,
        local_event->common.model, local_event->common.capture_available,
        local_event->num_phrases, local_event->phrase_extras[0].id);

    *event = local_event;
exit:
    return status;
}

static int process_detection_event_generic(st_session_t *st_ses,
    int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_recognition_event **event)
{
    st_hw_session_t *st_hw_ses = st_ses->hw_ses_current;
    struct st_vendor_info *v_info = st_ses->vendor_uuid_info;
    int status = 0;
    struct sound_trigger_recognition_event *local_event = NULL;

    local_event = calloc(1, sizeof(*local_event) + payload_size);
    if (!local_event) {
        ALOGE("%s: event allocation failed, size %zd", __func__,
            payload_size);
        status = -ENOMEM;
        goto exit;
    }

    local_event->status = detect_status;
    local_event->type = st_ses->sm_type;
    local_event->model = st_ses->sm_handle;
    local_event->capture_available = st_ses->capture_requested;
    local_event->capture_delay_ms = 0;
    local_event->capture_preamble_ms = 0;
    local_event->audio_config.sample_rate = v_info ?
        v_info->sample_rate : SOUND_TRIGGER_SAMPLING_RATE_16000;
    local_event->audio_config.format = AUDIO_FORMAT_PCM_16_BIT;
    local_event->audio_config.channel_mask =
        audio_channel_in_mask_from_count(st_hw_ses->config.channels);

    local_event->data_offset = sizeof(*local_event);
    local_event->data_size = payload_size;
    memcpy((char *)local_event + local_event->data_offset,
        payload, payload_size);

    ALOGI("%s:[%d]", __func__, st_ses->sm_handle);
    ALOGV("%s:[%d] status=%d, type=%d, model=%d, capture_avaiable=%d",
        __func__, st_ses->sm_handle, local_event->status,
        local_event->type, local_event->model,
        local_event->capture_available);

    *event = local_event;

exit:
    return status;
}

static inline int process_detection_event(st_session_t *st_ses,
    uint64_t timestamp,
    int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_recognition_event **event)
{
    int ret;
    struct sound_trigger_phrase_recognition_event *phrase_event = NULL;

    *event = NULL;
    if (st_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        if (sthw_extn_check_process_det_ev_support())
            ret = sthw_extn_process_detection_event_keyphrase(st_ses,
                                      timestamp, detect_status,
                                      payload, payload_size, &phrase_event);
        else
            ret = process_detection_event_keyphrase(st_ses, detect_status,
                                                    payload, payload_size,
                                                    &phrase_event);
        if (phrase_event)
            *event = &phrase_event->common;
    } else {
        ret = process_detection_event_generic(st_ses, detect_status, payload,
                                            payload_size, event);
    }
    return ret;
}

int st_session_load_sm(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;

    /* SM is stored in the session by st_device, hence set NULL below */
    st_session_loadsm_payload_t payload = { NULL };
    st_session_ev_t ev = { .ev_id = ST_SES_EV_LOAD_SM,
        .payload.loadsm = payload };

    /*
     * no need to lock mutex when loading sm as session is just being
     * being created and handle not returned to caller yet
     */
    DISPATCH_EVENT(st_ses, ev, status);
    return status;
}

int st_session_unload_sm(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;
    st_session_ev_t ev = { .ev_id = ST_SES_EV_UNLOAD_SM };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);

    return status;
}

int st_session_start(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_START };

    /* lock to serialize event handling */
    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_stop(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;
    st_session_ev_t ev = { .ev_id = ST_SES_EV_STOP };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);

    return status;
}

int st_session_restart(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_RESTART };

    /* lock to serialize event handling */
    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_ssr_offline(st_session_t *st_ses,
    enum ssr_event_status ssr_type)
{
    int status = 0;

    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SSR_OFFLINE,
        .payload.ssr = ssr_type };

    pthread_mutex_lock(&st_ses->lock);
    /*
     *  In typical usecases, handle SSR only if it occured on the core we are
     *  currently using. In cases that have an SSR event during transitions,
     *  the exec_mode can be NONE. For these cases, handle SSR on the core
     *  which was in use prior to the transition. For example, if the
     *  ssr_transit_exec_mode is ADSP, then the core prior to the transition
     *  is CPE, so we handle the CPE SSR event.
     */
    if (((ST_EXEC_MODE_CPE == st_ses->exec_mode) &&
         (CPE_STATUS_OFFLINE == ssr_type)) ||
        ((ST_EXEC_MODE_ADSP == st_ses->exec_mode) &&
         (SND_CARD_STATUS_OFFLINE == ssr_type)) ||
        ((ST_EXEC_MODE_NONE == st_ses->exec_mode) &&
         (((ST_EXEC_MODE_CPE == st_ses->ssr_transit_exec_mode) &&
           (SND_CARD_STATUS_OFFLINE == ssr_type)) ||
          ((ST_EXEC_MODE_ADSP == st_ses->ssr_transit_exec_mode) &&
           (CPE_STATUS_OFFLINE == ssr_type)))))
        DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);

    return status;
}

int st_session_ssr_online(st_session_t *st_ses,
    enum ssr_event_status ssr_type)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SSR_ONLINE,
        .payload.ssr = ssr_type };

    pthread_mutex_lock(&st_ses->lock);
    /*
     *  In typical usecases, handle SSR only if it occured on the core we are
     *  currently using. In cases that have an SSR event during transitions,
     *  the exec_mode can be NONE. For these cases, handle SSR on the core
     *  which was in use prior to the transition. For example, if the
     *  ssr_transit_exec_mode is ADSP, then the core prior to the transition
     *  is CPE, so we handle the CPE SSR event.
     */
    if (((ST_EXEC_MODE_CPE == st_ses->exec_mode) &&
         (CPE_STATUS_ONLINE == ssr_type)) ||
        ((ST_EXEC_MODE_ADSP == st_ses->exec_mode) &&
         (SND_CARD_STATUS_ONLINE == ssr_type)) ||
        ((ST_EXEC_MODE_NONE == st_ses->exec_mode) &&
         (((ST_EXEC_MODE_CPE == st_ses->ssr_transit_exec_mode) &&
           (SND_CARD_STATUS_ONLINE == ssr_type)) ||
          ((ST_EXEC_MODE_ADSP == st_ses->ssr_transit_exec_mode) &&
           (CPE_STATUS_ONLINE == ssr_type)))))
        DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);

    return status;
}

int st_session_pause(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;
    st_session_ev_t ev = { .ev_id = ST_SES_EV_PAUSE };
    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_resume(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;
    st_session_ev_t ev = { .ev_id = ST_SES_EV_RESUME };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_disable_device(st_session_t *st_ses)
{
    int status = 0;

    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SET_DEVICE,
        .payload.enable = false };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_enable_device(st_session_t *st_ses)
{
    int status = 0;

    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SET_DEVICE,
        .payload.enable = true };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

bool st_session_is_detected(st_session_t *st_ses)
{
    bool ret;

    pthread_mutex_lock(&st_ses->lock);
    ret = (st_ses->current_state == detected_state_fn) ? true : false;
    pthread_mutex_unlock(&st_ses->lock);

    return ret;
}

bool st_session_is_active(st_session_t *st_ses)
{
    bool ret;

    pthread_mutex_lock(&st_ses->lock);
    ret = (st_ses->current_state == active_state_fn) ? true : false;
    pthread_mutex_unlock(&st_ses->lock);

    return ret;
}

bool st_session_is_buffering(st_session_t *st_ses)
{
    bool ret;

    pthread_mutex_lock(&st_ses->lock);
    ret = (st_ses->current_state == buffering_state_fn) ? true : false;
    pthread_mutex_unlock(&st_ses->lock);

    return ret;
}

bool st_session_is_ssr_state(st_session_t *st_ses)
{
    bool ret;

    pthread_mutex_lock(&st_ses->lock);
    ret = (st_ses->current_state == ssr_state_fn) ? true : false;
    pthread_mutex_unlock(&st_ses->lock);

    return ret;
}

int st_session_read_pcm(st_session_t *st_ses, uint8_t *buff,
    size_t buff_size, size_t *read_size)
{
    int status = 0;
    if (!st_ses || !buff || buff_size == 0 || read_size == 0)
        return -EINVAL;

    st_session_readpcm_payload_t payload = { .out_buff = buff,
        .out_buff_size = buff_size, .actual_read_size = read_size };

    st_session_ev_t ev = { .ev_id = ST_SES_EV_READ_PCM,
        .payload.readpcm = payload };

    /* Do not lock when handling this event, this event
     can go in parallel with other events */
    DISPATCH_EVENT(st_ses, ev, status);
    return status;
}

int st_session_stop_lab(st_session_t *st_ses)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;
    st_session_ev_t ev = { .ev_id = ST_SES_EV_END_BUFFERING };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_set_exec_mode(st_session_t *st_ses, st_exec_mode_t exec)
{
    int status = 0;
    if (!st_ses)
        return -EINVAL;

    ALOGV("%s: exec mode %d", __func__, exec);

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SET_EXEC_MODE,
        .payload.exec_mode = exec };

    pthread_mutex_lock(&st_ses->lock);
    DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_send_custom_chmix_coeff(st_session_t *st_ses, char *str)
{
    int status = 0;

    if (!st_ses)
        return -EINVAL;

    st_session_ev_t ev = { .ev_id = ST_SES_EV_SEND_CHMIX_COEFF,
        .payload.chmix_coeff_str = str};

    pthread_mutex_lock(&st_ses->lock);
    if (ST_EXEC_MODE_ADSP == st_ses->exec_mode)
        DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);
    return status;
}

int st_session_get_config(st_session_t *st_ses, struct pcm_config *config)
{
    if (!st_ses)
        return -EINVAL;

    st_hw_session_t *hw_ses = st_ses->hw_ses_current;
    memcpy(config, &hw_ses->config, sizeof(struct pcm_config));

    return 0;
}

int st_session_get_param_data(st_session_t *st_ses, const char *param,
                              void *payload, size_t payload_size,
                              size_t *param_data_size)
{
    int status = 0;

    if (!st_ses)
        return -EINVAL;

    st_session_getparam_payload_t getparam_payload = { .param = param,
        .payload = payload,
        .payload_size = payload_size,
        .param_data_size = param_data_size};
    st_session_ev_t ev = { .ev_id = ST_SES_EV_GET_PARAM_DATA,
        .payload.getparam = getparam_payload};

    pthread_mutex_lock(&st_ses->lock);
    /* Currently get param data supported for only ARM mode */
    if (ST_EXEC_MODE_ARM == st_ses->exec_mode)
        DISPATCH_EVENT(st_ses, ev, status);
    pthread_mutex_unlock(&st_ses->lock);

    return status;
}

int st_session_init(st_session_t *st_ses, struct sound_trigger_device *stdev,
    st_exec_mode_t exec_mode, sound_model_handle_t sm_handle)
{
    int status = 0;
    struct st_vendor_info *v_info;

    if (!st_ses || !stdev) {
        status = -EINVAL;
        return status;
    }
    st_ses->stdev = stdev;

    /* caller must set vendor_uuid_info directly if present */
    v_info = st_ses->vendor_uuid_info;

    if (v_info && (EXEC_MODE_CFG_DYNAMIC == v_info->exec_mode_cfg)) {
        st_ses->enable_trans = true;

        if (stdev->is_gcs) {
            /* alloc and init cpe session*/
            st_ses->hw_ses_cpe = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_gcs_t));
            if (!st_ses->hw_ses_cpe) {
                status = -ENOMEM;
                goto cleanup;
            }
            status = st_hw_sess_gcs_init(st_ses->hw_ses_cpe, hw_sess_cb,
                (void *)st_ses, ST_EXEC_MODE_CPE, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initializing gcs hw session failed %d", __func__,
                    status);
                goto cleanup;
            }

            /* alloc and init adsp session*/
            st_ses->hw_ses_adsp = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
            if (!st_ses->hw_ses_adsp) {
                st_hw_sess_gcs_deinit(st_ses->hw_ses_cpe);
                status = -ENOMEM;
                goto cleanup;
            }

            status = st_hw_sess_lsm_init(st_ses->hw_ses_adsp, hw_sess_cb,
                (void *)st_ses, ST_EXEC_MODE_ADSP, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initializing lsm session failed", __func__);
                st_hw_sess_gcs_deinit(st_ses->hw_ses_cpe);
                goto cleanup;
            }

        } else {
            /* alloc and init cpe session*/
            st_ses->hw_ses_cpe = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
            if (!st_ses->hw_ses_cpe) {
                status = -ENOMEM;
                goto cleanup;
            }
            status = st_hw_sess_lsm_init(st_ses->hw_ses_cpe, hw_sess_cb,
                (void *)st_ses, ST_EXEC_MODE_CPE, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initialzing lsm hw session failed %d", __func__,
                    status);
                goto cleanup;
            }
            /* alloc and init adsp session*/
            st_ses->hw_ses_adsp = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
            if (!st_ses->hw_ses_adsp) {
                status = -ENOMEM;
                st_hw_sess_lsm_deinit(st_ses->hw_ses_cpe);
                goto cleanup;
            }

            status = st_hw_sess_lsm_init(st_ses->hw_ses_adsp, hw_sess_cb,
                (void *)st_ses, ST_EXEC_MODE_ADSP, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initializing lsm session failed", __func__);
                st_hw_sess_lsm_deinit(st_ses->hw_ses_cpe);
                goto cleanup;
            }
        }

        /* set current hw_session */
        if (exec_mode == ST_EXEC_MODE_CPE)
            st_ses->hw_ses_current = st_ses->hw_ses_cpe;
        else if (exec_mode == ST_EXEC_MODE_ADSP)
            st_ses->hw_ses_current = st_ses->hw_ses_adsp;

    } else if (v_info && (EXEC_MODE_CFG_CPE == v_info->exec_mode_cfg)) {
        st_ses->enable_trans = false;
        if (stdev->is_gcs) {
            ALOGD("%s: initializing gcs hw session", __func__);
            st_ses->hw_ses_cpe = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_gcs_t));
            if (!st_ses->hw_ses_cpe) {
                status = -ENOMEM;
                goto cleanup;
            }
            status = st_hw_sess_gcs_init(st_ses->hw_ses_cpe, hw_sess_cb,
                (void *)st_ses, exec_mode, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initializing gcs hw session failed %d",
                    __func__, status);
                goto cleanup;
            }
        } else {
            st_ses->hw_ses_cpe = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
            if (!st_ses->hw_ses_cpe) {
                status = -ENOMEM;
                goto cleanup;
            }
            status = st_hw_sess_lsm_init(st_ses->hw_ses_cpe, hw_sess_cb,
                (void *)st_ses, exec_mode, v_info, sm_handle, stdev);
            if (status) {
                ALOGE("%s: initializing lsm hw session failed %d",
                    __func__, status);
                goto cleanup;
            }
        }
        st_ses->hw_ses_current = st_ses->hw_ses_cpe;

    } else if (v_info && (EXEC_MODE_CFG_APE == v_info->exec_mode_cfg)) {
        st_ses->enable_trans = false;
        st_ses->hw_ses_adsp = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
        if (!st_ses->hw_ses_adsp) {
            status = -ENOMEM;
            goto cleanup;
        }
        status = st_hw_sess_lsm_init(st_ses->hw_ses_adsp, hw_sess_cb,
            (void *)st_ses, exec_mode, v_info, sm_handle, stdev);
        if (status) {
            ALOGE("%s: initializing lsm hw session failed %d",
                __func__, status);
            goto cleanup;
        }
        st_ses->hw_ses_current = st_ses->hw_ses_adsp;
    } else if (v_info && (EXEC_MODE_CFG_ARM == v_info->exec_mode_cfg)) {
        st_ses->enable_trans = false;
        st_ses->hw_ses_arm = calloc(1, sizeof(st_hw_session_pcm_t));
        if (!st_ses->hw_ses_arm) {
            status = -ENOMEM;
            goto cleanup;
        }
        status = st_hw_sess_pcm_init(st_ses->hw_ses_arm, hw_sess_cb,
            (void *)st_ses, exec_mode, v_info, sm_handle, stdev);
        if (status) {
            ALOGE("%s: initializing pcm hw session failed %d",
                __func__, status);
            goto cleanup;
        }
        st_ses->hw_ses_current = st_ses->hw_ses_arm;
    } else if (!v_info) {
        st_ses->hw_ses_cpe = (st_hw_session_t *)calloc(1, sizeof(st_hw_session_lsm_t));
        if (!st_ses->hw_ses_cpe) {
            status = -ENOMEM;
            goto cleanup;
        }
        status = st_hw_sess_lsm_init(st_ses->hw_ses_cpe, hw_sess_cb,
            (void *)st_ses, exec_mode, v_info, sm_handle, stdev);
        if (status) {
            ALOGE("%s: initializing lsm hw session failed %d",
                __func__, status);
            goto cleanup;
        }
    }

    st_ses->exec_mode = exec_mode;
    st_ses->sm_handle = sm_handle;
    st_ses->ssr_transit_exec_mode = ST_EXEC_MODE_NONE;

    /* start in idle state */
    STATE_TRANSITION(st_ses, idle_state_fn);

    return status;

cleanup:
    if (st_ses->hw_ses_cpe)
        free(st_ses->hw_ses_cpe);
    if (st_ses->hw_ses_adsp)
        free(st_ses->hw_ses_adsp);
    return status;
}

int st_session_deinit(st_session_t *st_ses)
{

    /* deinit cpe session */
    if (st_ses->hw_ses_cpe) {
        if (st_ses->stdev->is_gcs)
            st_hw_sess_gcs_deinit(st_ses->hw_ses_cpe);
        else
            st_hw_sess_lsm_deinit(st_ses->hw_ses_cpe);
        free((void *)st_ses->hw_ses_cpe);
    }

    /* deinit adsp session */
    if (st_ses->hw_ses_adsp) {
        st_hw_sess_lsm_deinit(st_ses->hw_ses_adsp);
        free((void *)st_ses->hw_ses_adsp);
    }

    return 0;
}
