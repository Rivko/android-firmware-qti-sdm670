/*
 *
 * This library contains the API to load sound models with
 * DSP and start/stop detection of associated key phrases.
 *
 * Copyright (c) 2015 QUALCOMM Technologies, Inc. All Rights Reserved.
 * QUALCOMM Technologies Proprietary and Confidential.
 *
 */

#define LOG_TAG "sound_trigger_transition"
/*#define LOG_NDEBUG 0*/
#define LOG_NDDEBUG 0

#include <dlfcn.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <system/thread_defs.h>
#include <hardware/sound_trigger.h>

#include "sound_trigger_platform.h"
#include "sound_trigger_hw.h"

static int transit_route_reg_sm(sound_trigger_session_t* p_ses);
static int transit_route_reg_sm_params(sound_trigger_session_t* p_ses);
static int transit_route_dereg_sm(sound_trigger_session_t* p_ses);
static int transit_route_dereg_sm_params(sound_trigger_session_t* p_ses);
static int transit_route_start(sound_trigger_session_t* p_ses);
static int transit_route_stop(sound_trigger_session_t* p_ses);
static int transit_route_stop_buffering(sound_trigger_session_t* p_ses);
static int transit_route_set_device(sound_trigger_session_t* p_ses,
                                bool enable, audio_devices_t device);

struct sound_trigger_session_fptrs transit_fptrs = {
    .reg_sm = transit_route_reg_sm,
    .reg_sm_params = transit_route_reg_sm_params,
    .dereg_sm = transit_route_dereg_sm,
    .dereg_sm_params = transit_route_dereg_sm_params,
    .start = transit_route_start,
    .stop = transit_route_stop,
    .stop_buffering = transit_route_stop_buffering,
};

static int send_transition_cmd_l
(
    struct sound_trigger_device *stdev,
    sound_model_handle_t sm_handle,
    enum sound_trigger_transition_cmds command
)
{
    ALOGD("%s:[%d] command %d", __func__, sm_handle, command);
    stdev->transit_cmd.cmd = command;
    stdev->transit_cmd.sm_handle = sm_handle;
    pthread_cond_signal(&stdev->transit_cond);
    return 0;
}

static int alloc_transit_ses(sound_trigger_session_t* p_ses)
{
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;
    struct sound_trigger_session *transit_ses = NULL;
    ALOGV("%s:[%d] Enter state 0x%x", __func__, p_ses->sm_handle, p_ses->state);

    if (p_ses) {
        if (p_ses->sm_data->num_phrases > stdev->avail_cpe_phrases ||
            p_ses->num_users > stdev->avail_cpe_users) {
            ALOGI("%s: ses not supported on CPE KW=%u UP=%u CKW=%u CKWUP=%u",
                        __func__, p_ses->sm_data->num_phrases, p_ses->num_users,
                        stdev->avail_cpe_phrases, stdev->avail_cpe_users);
            return -EINVAL;
        }
        transit_ses = calloc(1, sizeof(struct sound_trigger_session));
        if (!transit_ses) {
            ALOGE("%s: cannot allocate transit_ses!!", __func__);
            return -ENOMEM;
        }
        *transit_ses = *p_ses;
        transit_ses->fptrs = NULL;
        transit_ses->exec_mode = ST_DEVICE_EXEC_MODE_CPE;
        transit_ses->pcm = NULL;
        transit_ses->exit_callback_thread = false;
        transit_ses->pcm_id = 0;
        transit_ses->use_case_idx = 0;
        pthread_mutex_init(&transit_ses->lock,
                           (const pthread_mutexattr_t *) NULL);

        transit_ses->transit_ses = p_ses;
        p_ses->transit_ses = transit_ses;

    }
    ALOGD("%s: Exit p_ses %p, transit_ses %p", __func__, p_ses, transit_ses);
    return 0;
}

static int unload_transit_ses(sound_trigger_session_t* p_ses)
{
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;
    struct sound_trigger_session *transit_ses;
    int status = 0;

    ALOGD("%s:[%d] state 0x%x", __func__, p_ses->sm_handle, p_ses->state);
    if (p_ses == NULL)
        return -EINVAL;

    if (!CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED|SES_TRANSIT_LOADING))
        return -EINVAL;

    transit_ses = p_ses->transit_ses;
    if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED)) {
        pthread_mutex_unlock(&p_ses->lock);
        pthread_mutex_lock(&transit_ses->lock);
        cpe_dereg_sm(transit_ses);
        pthread_mutex_unlock(&transit_ses->lock);
        pthread_mutex_lock(&p_ses->lock);
        CLEAR_STATE(p_ses->state, SES_TRANSIT_LOADED);
    }
    CLEAR_STATE(p_ses->state, SES_TRANSIT_INVALIDATED);
    stdev->avail_cpe_phrases  += transit_ses->sm_data->num_phrases;
    stdev->avail_cpe_users  += transit_ses->num_users;
    pthread_mutex_destroy(&transit_ses->lock);
    free(transit_ses);
    p_ses->transit_ses = NULL;
    return status;
}

static int unload_transit_sm
(
    struct sound_trigger_device *stdev,
    sound_model_handle_t sm_handle
)
{
    int status = 0;
    struct sound_trigger_session *p_ses;

    ALOGV("%s:[%d] Enter", __func__, sm_handle);

    pthread_mutex_lock(&stdev->lock);
    p_ses = get_sound_trigger_session(stdev, sm_handle);
    if (p_ses) {
        pthread_mutex_lock(&p_ses->lock);
        status = unload_transit_ses(p_ses);
        pthread_mutex_unlock(&p_ses->lock);
    }
    pthread_mutex_unlock(&stdev->lock);

    ALOGV("%s:[%d] Exit", __func__, sm_handle);
    return status;
}

static int load_transit_sm
(
    struct sound_trigger_device *stdev,
    sound_model_handle_t sm_handle
)
{
    int status = 0, ret = 0;
    bool ses_invalidated = false;
    sound_trigger_session_t *transit_ses = NULL, *p_ses = NULL;
    ALOGD("%s:[%d] Enter", __func__, sm_handle);

    pthread_mutex_lock(&stdev->lock);
    while (CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE)) {
        /* Wait till CPE SSR completes */
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }
    p_ses = get_sound_trigger_session(stdev, sm_handle);
    if (!p_ses) {
        pthread_mutex_unlock(&stdev->lock);
        ALOGI("%s:[%d] ses not found", __func__, sm_handle);
        return -EINVAL;
    }
    /* release stdev lock only after acquiring ses lock,
     * so that the handle doesn't get invalidated in between
     */
    pthread_mutex_lock(&p_ses->lock);
    pthread_mutex_unlock(&stdev->lock);

    if (CHECK_STATE(p_ses->state,
        (SES_INVALIDATED|SES_TRANSIT_INVALIDATED|SES_TRANSIT_SUSPENDED))) {
        /* UC: S1 removed or failed earlier
         * S1 started -> transition requested -> deregistered -> cmd picked
         * S1 strted -> error while transitioning
         */
        ALOGD("%s:[%d] ignore call in state 0x%x",
                __func__, sm_handle, p_ses->state);
        status = -EINVAL;
    } else if (CHECK_STATE(p_ses->state, (SES_TRANSIT_LOADED))) {
        /* UC:
         * S1 strted -> stopped while loading -> started
         * do not update return status, we need to return success.
         */
        ALOGD("%s:[%d] ignore call in state 0x%x",
                __func__, sm_handle, p_ses->state);
    } else {
        SET_STATE(p_ses->state, SES_TRANSIT_LOADING);
        status = alloc_transit_ses(p_ses);
        if (!status) {
            transit_ses = p_ses->transit_ses;
            stdev->avail_cpe_phrases  -= transit_ses->sm_data->num_phrases;
            stdev->avail_cpe_users  -= transit_ses->num_users;
            /* ses will not be freed from stdev_unload_sm, when  in state
             * SES_TRANSIT_LOADING, allowing us to release lock here,
             * but free transit and p_ses if flag SES_INVALIDATED is set.
             */
            pthread_mutex_unlock(&p_ses->lock);
            status = cpe_reg_sm(transit_ses);
            pthread_mutex_lock(&p_ses->lock);

            /* set state to loaded if reg_sm succeeded */
            if (!status)
                SET_STATE(p_ses->state, SES_TRANSIT_LOADED);

            /* check if ses invalidated while loading,
             * if invalidated, set status to error and avoid furhter processing.
             * note: p_ses' free still cannot be done based on this status code,
             *       since error code is not unique to invalidation use case.
             */
            if (CHECK_STATE(p_ses->state, SES_INVALIDATED)) {
                ses_invalidated = true;
                status = -EINVAL;
            } else if (CHECK_STATE(p_ses->state, SES_TRANSIT_INVALIDATED)) {
                /* only transit handle is invalidated. (UC: CPE SSR while loading)
                 * Unload present CPE session and return error.
                 * Transition command for reload needs to be issue separatey.
                 */
                status = -EINTR;
            }

            if (!status && CHECK_STATE(p_ses->state, SES_STARTED))
                status = cpe_reg_sm_params(transit_ses);

            if (status) {
                ALOGE("%s:[%d] failed or invalidated, error %d, state 0x%x",
                                  __func__, sm_handle, status, p_ses->state);
                unload_transit_ses(p_ses);
            }
        }
        CLEAR_STATE(p_ses->state, SES_TRANSIT_LOADING);
        /* prevent further transition for the session for unhandled errors */
        if (status && !CHECK_STATE(p_ses->state, SES_TRANSIT_INVALIDATED))
            SET_STATE(p_ses->state, SES_TRANSIT_SUSPENDED);
        CLEAR_STATE(p_ses->state, SES_TRANSIT_INVALIDATED);
    }
    pthread_mutex_unlock(&p_ses->lock);

    if (ses_invalidated) {
        ALOGD("%s:[%d] freeing ses invalidated while loading, state 0x%x",
                                  __func__, p_ses->sm_handle, p_ses->state);
        pthread_mutex_destroy(&p_ses->lock);
        free(p_ses->sm_data);
        free(p_ses);
    }

    ALOGD("%s:[%d] EXIT: status %d", __func__, sm_handle, status);
    return status;
}

static int start_transit_sm
(
    struct sound_trigger_device *stdev,
    sound_model_handle_t sm_handle
)
{
    int status = 0, num_ses_started = 0;
    sound_trigger_session_t *transit_ses = NULL, *p_ses = NULL;
    ALOGD("%s:[%d] Enter", __func__, sm_handle);

    pthread_mutex_lock(&stdev->lock);
    while (CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE)) {
        /* Wait till CPE SSR completes */
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }
    p_ses = get_sound_trigger_session(stdev, sm_handle);
    if (!p_ses) {
        pthread_mutex_unlock(&stdev->lock);
        ALOGI("%s:[%d] ses not found", __func__, sm_handle);
        return -EINVAL;
    }
    num_ses_started = get_num_sessions_in_state(SES_STARTED);
    /* release stdev lock only after acquiring other locks,
     * so that the handle doesn't get invalidated in between
     * start needs transit lock as well
     * since start/stop can be called from another ses' context
     * lock ordering to be: stdev->ses->transit
     */
    pthread_mutex_lock(&p_ses->lock);
    pthread_mutex_lock(&stdev->transit_lock);
    pthread_mutex_unlock(&stdev->lock);

    if (num_ses_started > 1) {
        /* mutiple ses active, skip start, session remains on ape */
        ALOGD("%s: mutiple(%d) ses active, skip cpe start",
                                __func__, num_ses_started);
    } else if (CHECK_STATE(p_ses->state, SES_TRANSIT_STARTED)) {
        ALOGD("%s: already stated state 0x%x", __func__, p_ses->state);
    } else if (CHECK_STATE(p_ses->state, SES_STARTED) &&
               CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED)) {
        transit_ses = p_ses->transit_ses;
        if (CHECK_STATE(p_ses->state, SES_BUFFERING)) {
            ALOGD("%s: ses state 0x%x, retry after some time",
                                     __func__, p_ses->state);
            status = -EAGAIN;
        } else {
            dereg_hal_event_session(p_ses);
            ape_stop(p_ses);
            update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_CPE);
            reg_hal_event_session(transit_ses);
            status = cpe_start(transit_ses);
            /* continue running on ape if start on cpe fails */
            if (status) {
                dereg_hal_event_session(transit_ses);
                update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_APE);
                reg_hal_event_session(p_ses);
                ape_start(p_ses);
                SET_STATE(p_ses->state, SES_TRANSIT_SUSPENDED);
                ALOGE("%s:[%d] failed with error %d",
                                __func__, sm_handle, status);
                unload_transit_ses(p_ses);
            } else {
                SET_STATE(p_ses->state, SES_TRANSIT_STARTED);
            }
        }
    }
    pthread_mutex_unlock(&stdev->transit_lock);
    pthread_mutex_unlock(&p_ses->lock);

    ALOGD("%s:[%d] EXIT: status %d", __func__, sm_handle, status);
    return status;
}

static int transit_set_device (
   struct sound_trigger_device *stdev,
   st_exec_mode_t exec_mode,
   bool enable,
   audio_devices_t device)
{
    char device_name[DEVICE_NAME_MAX_SIZE] = {0};
    int ref_cnt_idx = 0;
    int status = 0;
    st_device_t st_device;

    st_device = platform_stdev_get_device(stdev->platform, device);
    if (st_device < ST_DEVICE_MIN || st_device >= ST_DEVICE_MAX) {
        ALOGE("%s: Invalid sound trigger device %d", __func__, st_device);
        return -EINVAL;
    }

    if (platform_stdev_get_device_name(stdev->platform, exec_mode, st_device, device_name) < 0 ) {
        ALOGE("%s: Invalid sound trigger device returned", __func__);
        return -EINVAL;
    }

    ref_cnt_idx = (exec_mode * ST_DEVICE_MAX) + st_device;
    if (enable) {
        stdev->dev_ref_cnt[ref_cnt_idx]++;
        if (stdev->dev_ref_cnt[ref_cnt_idx] == 1) {
            status = platform_stdev_send_calibration(stdev->platform,
                                                     device,
                                                     exec_mode,
                                                     ACDB_LSM_APP_TYPE_NO_TOPOLOGY,
                                                     false, ST_DEVICE_CAL);
            if (!status) {
                ALOGD("%s: enable device (%x) = %s", __func__, st_device, device_name);
                audio_route_apply_and_update_path(stdev->audio_route, device_name);
            }
        }
    } else {
        stdev->dev_ref_cnt[ref_cnt_idx]--;
        if (stdev->dev_ref_cnt[ref_cnt_idx] == 0) {
            ALOGD("%s: disable device (%x) = %s", __func__, st_device, device_name);
            audio_route_reset_and_update_path(stdev->audio_route, device_name);
        }
    }
}

void *transition_thread_loop(void *context)
{
    struct sound_trigger_device *stdev =
                       (struct sound_trigger_device *)context;
    sound_model_handle_t prev_sm_handle = -1;
    int status = 0;

    if (!stdev) {
        ALOGE("%s: NULL context!!!, exiting", __func__);
        return NULL;
    }

    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_DEFAULT);
    prctl(PR_SET_NAME, (unsigned long)"sound trigger transition", 0, 0, 0);

    ALOGV("%s", __func__);

    pthread_mutex_lock(&stdev->transit_lock);
    while(true) {
        struct sound_trigger_transition_cmd cmd = stdev->transit_cmd;

        if (cmd.cmd == ST_TRANSIT_CMD_EXIT)
            break;

        if (cmd.cmd == ST_TRANSIT_CMD_NONE) {
            ALOGD("%s: wait for command", __func__);
            pthread_cond_wait(&stdev->transit_cond, &stdev->transit_lock);
            ALOGD("%s: out of wait for command", __func__);
            continue;
        }
        status = 0;
        stdev->transit_cmd.cmd = ST_TRANSIT_CMD_NONE;
        ALOGV("%s: received command = %d", __func__, cmd.cmd);

        pthread_mutex_unlock(&stdev->transit_lock);
        if (cmd.cmd == ST_TRANSIT_CMD_LOAD_AND_START) {

            if (cmd.sm_handle != prev_sm_handle)
                unload_transit_sm(stdev, prev_sm_handle);

            status = load_transit_sm(stdev, cmd.sm_handle);
            if (!status) {
                do {
                    status = start_transit_sm(stdev, cmd.sm_handle);
                    /* session may be buffering, wait for completion */
                    if (status == -EAGAIN) {
                        pthread_mutex_lock(&stdev->transit_lock);
                        ALOGV("%s:[%d] wait before retry start",
                                               __func__, cmd.sm_handle);
                        pthread_cond_wait(&stdev->transit_cond,
                                          &stdev->transit_lock);
                        ALOGV("%s:[%d] out of wait, retry start",
                                               __func__, cmd.sm_handle);
                        pthread_mutex_unlock(&stdev->transit_lock);
                    }
                } while (status == -EAGAIN);
                /* if completed without error, cache sm in our context */
                if (!status)
                    prev_sm_handle = cmd.sm_handle;
            }
        } else {
            ALOGE("%s: ignored command %d", __func__, cmd.cmd);
        }
        pthread_mutex_lock(&stdev->transit_lock);
    }
    pthread_mutex_unlock(&stdev->transit_lock);

    return NULL;
}

static int transit_route_start(sound_trigger_session_t* p_ses)
{
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;
    unsigned int num_ses_started = 0, self = 0;
    struct sound_trigger_session *other_ses;
    struct listnode *item;
    struct list_head *h;
    int status = 0;

    ALOGV("%s:[%d] state 0x%x", __func__, p_ses->sm_handle, p_ses->state);
    pthread_mutex_lock(&stdev->transit_lock);

    if (CHECK_STATE(p_ses->state, SES_STARTED))
        self = 1;
    num_ses_started = get_num_sessions_in_state(SES_STARTED);
    ALOGD("%s: num of sessions started %d, self %d",
                    __func__, num_ses_started, self);

    if (num_ses_started - self >= 1) {
        /* UC: S(2+) started
         * more than one session active, start sessions on APE
         */
        if (num_ses_started == 1 &&
            stdev->exec_mode == ST_DEVICE_EXEC_MODE_CPE) {
            /* UC: S2 started
             * S1 started -> transitioned -> S2 started
             * coming session (S2) need to switch S1 to ADSP
             */
            ALOGV("%s: switch session to adsp", __func__);
            // get started session, could be more in just loaded state
            list_for_each_safe(item, h, &stdev->sound_model_list) {
                other_ses
                     = node_to_item(item, sound_trigger_session_t, list_node);
                if (CHECK_STATE(other_ses->state, SES_TRANSIT_STARTED)) {
                    ALOGV("%s: switch sm %d to adsp",
                                  __func__, other_ses->sm_handle);

                    /* cpe buffering to be preempted, cannot defer ape start */
                    if (CHECK_STATE(other_ses->state, SES_BUFFERING)) {
                        cpe_stop_buffering(other_ses->transit_ses);
                        CLEAR_STATE(other_ses->state, SES_BUFFERING);
                    }

                    dereg_hal_event_session(other_ses->transit_ses);
                    cpe_stop(other_ses->transit_ses);
                    CLEAR_STATE(other_ses->state, SES_TRANSIT_STARTED);

                    update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_APE);
                    reg_hal_event_session(other_ses);
                    ape_start(other_ses);

                    break;
                }
            }
        }
        reg_hal_event_session(p_ses);
        status = ape_start(p_ses);
    } else {
        /* UC: S1 started
         */
        if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED)) {
            /* UC: S1 started with ses already transitioned
             * S1 started -> transitioned -> stopped -> starting
             * not sure if mad on cpe, update mad
             */
            update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_CPE);
            reg_hal_event_session(p_ses->transit_ses);
            status = cpe_start(p_ses->transit_ses);
            /* start on ape if cpe start fails */
            if (status) {
                dereg_hal_event_session(p_ses->transit_ses);
                update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_APE);
                reg_hal_event_session(p_ses);
                ape_start(p_ses);
                SET_STATE(p_ses->state, SES_TRANSIT_SUSPENDED);
                ALOGW("%s:[%d] cpe start failed status %d",
                                __func__, p_ses->sm_handle, status);
                unload_transit_ses(p_ses);
            } else {
                SET_STATE(p_ses->state, SES_TRANSIT_STARTED);
            }
        } else {
            /* UC: S1 started without transit ses
             * S1 starting
             * S1 started -> transitioned -> stopped ->
             * -> transit ses unloaded -> started
             */
            reg_hal_event_session(p_ses);
            status = ape_start(p_ses);
            send_transition_cmd_l(stdev, p_ses->sm_handle,
                              ST_TRANSIT_CMD_LOAD_AND_START);
        }
    }

    pthread_mutex_unlock(&stdev->transit_lock);
    return status;
}


static int transit_route_stop(sound_trigger_session_t* p_ses)
{
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;
    int status = 0;
    unsigned int num_ses_started = 0;
    struct list_head *h;
    struct listnode *item;
    struct sound_trigger_session *other_ses;

    ALOGV("%s:[%d] state 0x%x", __func__, p_ses->sm_handle, p_ses->state);
    pthread_mutex_lock(&stdev->transit_lock);

    /* 1. first stop session on(ape/cpe) where its running */
    if (CHECK_STATE(p_ses->state, SES_TRANSIT_STARTED)) {
        /* UC:
         * S1 started -> transitioned -> stopping
         */
        dereg_hal_event_session(p_ses->transit_ses);
        status = cpe_stop(p_ses->transit_ses);
        CLEAR_STATE(p_ses->state, SES_TRANSIT_STARTED);
    } else {
        /* UC:
         * S(1+) started -> Sn stopping,
         * S1 started-> S1 stopping (did not transitioned yet)
         */
        dereg_hal_event_session(p_ses);
        status = ape_stop(p_ses);
    }

    num_ses_started = get_num_sessions_in_state(SES_STARTED);
    ALOGD("%s: num of sessions started %d", __func__, num_ses_started);

    if (num_ses_started == 1 &&
        stdev->exec_mode == ST_DEVICE_EXEC_MODE_APE) {
        /* UC:
         * S2 stopping, get sm_handle of S1 and call load & start for that
         */
        list_for_each_safe(item, h, &stdev->sound_model_list) {
            other_ses = node_to_item(item, sound_trigger_session_t, list_node);
            if (other_ses->state & SES_STARTED) {
                ALOGV("%s: send load and start for sm %d",
                            __func__, other_ses->sm_handle);
                send_transition_cmd_l(stdev, other_ses->sm_handle,
                                    ST_TRANSIT_CMD_LOAD_AND_START);
                break;
            }
        }
    }
    /* UC:
     * S(3+) or S1 stopping then do nothing
     * for this case unload will be from dereg_sm/transit_thread
     */

    pthread_mutex_unlock(&stdev->transit_lock);
    return status;
}

static int transit_route_reg_sm(sound_trigger_session_t* p_ses)
{
    return ape_reg_sm(p_ses);
}

static int transit_route_reg_sm_params(sound_trigger_session_t* p_ses)
{
    int status = 0;
    sound_trigger_session_t* transit_ses = NULL;

    status = ape_reg_sm_params(p_ses);

    if (CHECK_STATE(p_ses->state, (SES_TRANSIT_LOADED|SES_TRANSIT_LOADING))) {
        transit_ses = p_ses->transit_ses;
        transit_ses->conf_levels = p_ses->conf_levels;
        transit_ses->rc_config = p_ses->rc_config;
        transit_ses->capture_handle = p_ses->capture_handle;
        transit_ses->capture_requested = p_ses->capture_requested;
        if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED))
            cpe_reg_sm_params(transit_ses);
    }

    return status;
}

static int transit_route_dereg_sm(sound_trigger_session_t* p_ses)
{
    int status = 0;
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED))
        unload_transit_ses(p_ses);

    status = ape_dereg_sm(p_ses);
    return status;
}

static int transit_route_dereg_sm_params(sound_trigger_session_t* p_ses)
{
    int status = 0;

    status = ape_dereg_sm_params(p_ses);
    if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED)) {
        cpe_dereg_sm_params(p_ses->transit_ses);
    }

    return status;
}

static int transit_route_stop_buffering(sound_trigger_session_t* p_ses)
{
    int status = 0;
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    if (!CHECK_STATE(p_ses->state, SES_BUFFERING)) {
        return status;
    }

    /* buffering could be only on one place(ape/cpe)
     * check and stop where its buffering
     * need to take transit lock
     * since can be called from other ses' context like start/stop
     */
    pthread_mutex_lock(&stdev->transit_lock);

    if (CHECK_STATE(p_ses->state, SES_TRANSIT_STARTED))
        status = cpe_stop_buffering(p_ses->transit_ses);
    else
        status = ape_stop_buffering(p_ses);
    CLEAR_STATE(p_ses->state, SES_BUFFERING);

    pthread_mutex_unlock(&stdev->transit_lock);
    /* signal wakeup, transit start could be waiting for buffering complete */
    pthread_cond_signal(&stdev->transit_cond);

    return status;
}

static int transit_route_set_device(sound_trigger_session_t *p_ses,
                                bool enable, audio_devices_t device)
{
   struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;
   st_device_t st_device;
   int idx = 0, status = 0;

   st_device = platform_stdev_get_device(stdev->platform, device);
   if (st_device < ST_DEVICE_MIN || st_device >= ST_DEVICE_MAX) {
       ALOGE("%s: Invalid sound trigger device %d", __func__, st_device);
       return -EINVAL;
   }

   /* Enable both ADSP and CDSP device mixer controls and connect to ADSP first.
      The background run time transition logic will decide later on which device
      to connect */
   status = transit_set_device(stdev, ST_DEVICE_EXEC_MODE_APE, enable, device);
   if (!status) {
       idx = (ST_DEVICE_EXEC_MODE_APE * ST_DEVICE_MAX) + st_device;
       if (enable && stdev->dev_ref_cnt[idx] == 1)
           update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_APE);
       else if (!enable && stdev->dev_ref_cnt[idx] == 0)
           update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_NONE);
       status = transit_set_device(stdev, ST_DEVICE_EXEC_MODE_CPE, enable, device);
   }

   return status;
}

bool transit_ses_buffering(sound_trigger_session_t* p_ses, audio_io_handle_t capture_handle)
{
    bool ses_buffering = false;
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    if (stdev->transition_enabled) {
        pthread_mutex_lock(&stdev->transit_lock);
        if (CHECK_STATE(p_ses->state, SES_TRANSIT_STARTED) &&
            CHECK_STATE(p_ses->state, SES_BUFFERING) &&
            p_ses->transit_ses->capture_handle == capture_handle)
            ses_buffering = true;
        pthread_mutex_unlock(&stdev->transit_lock);
    }

    return ses_buffering;
}

bool transit_ses_loading(struct sound_trigger_session *p_ses)
{
    bool ses_loading = false;
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    if (stdev->transition_enabled) {
        pthread_mutex_lock(&p_ses->lock);
        if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADING)) {
            ALOGD("%s:[%d] still loading on cpe, do not free, state 0x%x",
                                  __func__, p_ses->sm_handle, p_ses->state);
            ses_loading = true;
        }
        pthread_mutex_unlock(&p_ses->lock);
    }
    return ses_loading;
}

void transit_retrieve_st_session(struct sound_trigger_session **pp_ses)
{
    struct sound_trigger_session *p_ses = *pp_ses;
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    /* if present ses exec mode is CPE, st_session is linked as transit ses */
    if (stdev->transition_enabled &&
        p_ses->exec_mode == ST_DEVICE_EXEC_MODE_CPE)
        *pp_ses = p_ses->transit_ses;
}

void transit_reassign_fptrs(struct sound_trigger_session *p_ses)
{
    struct sound_trigger_device *stdev =
            (struct sound_trigger_device *)p_ses->device;

    if (stdev->transition_enabled) {
        ALOGD("%s:[%d] set fptrs to transit_fptrs", __func__, p_ses->sm_handle);
        p_ses->fptrs = &transit_fptrs;
    }
}

bool transit_enabled(struct sound_trigger_device *stdev)
{
    ALOGD("%s: stdev run_on_ape %d, transition_enabled %d",
        __func__, stdev->run_on_ape, stdev->transition_enabled);
    if (stdev->transition_enabled)
        return true;

    return false;
}

void transit_handle_cpe_ssr_event(struct sound_trigger_device *stdev,
                                  enum ssr_event_status event)
{
    struct sound_trigger_session *p_ses;
    struct listnode *item;
    struct list_head *h;

    ALOGD("%s: event %d", __func__, event);

    if (event == CPE_STATUS_OFFLINE) {
        list_for_each_safe(item, h, &stdev->sound_model_list) {
            p_ses = node_to_item(item, sound_trigger_session_t, list_node);
            pthread_mutex_lock(&p_ses->lock);
            SET_STATE(p_ses->state, SES_TRANSIT_INVALIDATED);
            if (CHECK_STATE(p_ses->state, SES_TRANSIT_LOADED)) {
                if (!stdev->audio_concurrency_active &&
                    CHECK_STATE(p_ses->state, SES_STARTED)) {
                    pthread_mutex_lock(&stdev->transit_lock);
                    if (CHECK_STATE(p_ses->state, SES_TRANSIT_STARTED)) {
                        ALOGD("%s: remove sm %d from CPE",
                              __func__, p_ses->sm_handle);
                        if (CHECK_STATE(p_ses->state, SES_BUFFERING)) {
                            cpe_stop_buffering(p_ses->transit_ses);
                            CLEAR_STATE(p_ses->state, SES_BUFFERING);
                        }
                        dereg_hal_event_session(p_ses->transit_ses);
                        cpe_stop(p_ses->transit_ses);
                        CLEAR_STATE(p_ses->state, SES_TRANSIT_STARTED);

                        /* start ses on ADSP if only CPE went offline */
                        if (!CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_APE)) {
                            update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_APE);
                            reg_hal_event_session(p_ses);
                            ape_start(p_ses);
                        }
                    }
                    pthread_mutex_unlock(&stdev->transit_lock);
                    cpe_dereg_sm_params(p_ses->transit_ses);
                }
                unload_transit_ses(p_ses);
            }
            /* reset TRANSIT_SUSPENDED, give it a chance after cpe fw reload */
            CLEAR_STATE(p_ses->state, SES_TRANSIT_SUSPENDED);
            pthread_mutex_unlock(&p_ses->lock);
        }
    } else { /* CPE ONLINE event */
        unsigned int num_ses_started = get_num_sessions_in_state(SES_STARTED);
        if (num_ses_started == 1) {
            list_for_each_safe(item, h, &stdev->sound_model_list) {
                p_ses =
                    node_to_item(item, sound_trigger_session_t, list_node);
                if (!stdev->audio_concurrency_active &&
                    CHECK_STATE(p_ses->state, SES_STARTED)) {
                    pthread_mutex_lock(&p_ses->lock);
                    /* do not clear invalidated flag for loading case
                     * in case of loading it needs to do cleanup for there
                     * and reset this flag from that context.
                     */
                    if (!CHECK_STATE(p_ses->state, SES_TRANSIT_LOADING))
                        CLEAR_STATE(p_ses->state, SES_TRANSIT_INVALIDATED);
                    pthread_mutex_lock(&stdev->transit_lock);
                    ALOGD("%s: send load and start for sm %d",
                          __func__, p_ses->sm_handle);
                    send_transition_cmd_l(stdev, p_ses->sm_handle,
                                          ST_TRANSIT_CMD_LOAD_AND_START);
                    pthread_mutex_unlock(&stdev->transit_lock);
                    pthread_mutex_unlock(&p_ses->lock);
                    break;
                }
            }
        }
    }
}

void transit_stdev_init(struct sound_trigger_device *stdev)
{
    pthread_attr_t attr;

    ALOGD("%s: stdev run_on_ape %d, transition_enabled %d",
        __func__, stdev->run_on_ape, stdev->transition_enabled);
    if (stdev->transition_enabled) {
        pthread_mutex_init(&stdev->transit_lock, (const pthread_mutexattr_t *) NULL);
        pthread_cond_init(&stdev->transit_cond, (const pthread_condattr_t *) NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&stdev->transition_thread, &attr, transition_thread_loop, stdev);
    }
}

void transit_stdev_deinit(struct sound_trigger_device *stdev)
{
    if (stdev->transition_enabled) {
        send_transition_cmd_l(stdev, -1, ST_TRANSIT_CMD_EXIT);
        pthread_join(stdev->transition_thread, (void **) NULL);
        pthread_mutex_destroy(&stdev->transit_lock);
        pthread_cond_destroy(&stdev->transit_cond);
    }
}
