/*
 *
 * This library contains the API to load sound models with
 * DSP and start/stop detection of associated key phrases.
 *
 * Copyright (c) 2013-2017 QUALCOMM Technologies, Inc. All Rights Reserved.
 * QUALCOMM Technologies Proprietary and Confidential.
 *
 */

#define LOG_TAG "sound_trigger_hw"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <system/thread_defs.h>
#include <hardware/sound_trigger.h>

#include "sound_trigger_platform.h"
#include "sound_trigger_hw.h"

#define XSTR(x) STR(x)
#define STR(x) #x

/* Routing layer functions to route to either ADSP or CPE */
static int route_reg_sm_ape(sound_trigger_session_t *p_ses);
static int route_reg_sm_params_ape(sound_trigger_session_t* p_ses);
static int route_dereg_sm_ape(sound_trigger_session_t* p_ses);
static int route_prepare_ape(sound_trigger_session_t *p_ses);
static int route_unprepare_ape(sound_trigger_session_t *p_ses);
static int route_start_ape(sound_trigger_session_t* p_ses);
static int route_stop_ape(sound_trigger_session_t* p_ses);
static int route_stop_buffering_ape(sound_trigger_session_t* p_ses);
static int route_set_device_ape(sound_trigger_session_t* p_ses,
                                bool enable, audio_devices_t device);
static int route_read_pcm_ape(sound_trigger_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes);
static void route_lab_capture_ape(sound_trigger_session_t* p_ses);

static int route_reg_sm_cpe(sound_trigger_session_t *p_ses);
static int route_reg_sm_params_cpe(sound_trigger_session_t* p_ses);
static int route_dereg_sm_cpe(sound_trigger_session_t* p_ses);
static int route_prepare_cpe(sound_trigger_session_t *p_ses);
static int route_unprepare_cpe(sound_trigger_session_t *p_ses);
static int route_start_cpe(sound_trigger_session_t* p_ses);
static int route_stop_cpe(sound_trigger_session_t* p_ses);
static int route_stop_buffering_cpe(sound_trigger_session_t* p_ses);
static int route_set_device_cpe(sound_trigger_session_t* p_ses,
                                bool enable, audio_devices_t device);
static int route_read_pcm_cpe(sound_trigger_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes);
static void route_lab_capture_cpe(sound_trigger_session_t* p_ses);
static int stop_recognition_l(struct sound_trigger_session *p_ses);
static int stop_recognition_internal(struct sound_trigger_session* p_ses,
                                     bool lock);
static int start_recognition_internal(struct sound_trigger_session* p_ses);

/* count of sound trigger hal clients */
static unsigned int stdev_ref_cnt = 0;
static pthread_mutex_t stdev_init_lock;
static struct sound_trigger_device *stdev = NULL;

/* default properties which will later be updated based on platform configuration */
static struct sound_trigger_properties hw_properties = {
        "QUALCOMM Technologies, Inc", // implementor
        "Sound Trigger HAL", // description
        1, // version
        { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
        1, // max_sound_models
        1, // max_key_phrases
        1, // max_users
        RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_GENERIC_TRIGGER, // recognition_modes
        true, // capture_transition
        0, // max_capture_ms
        false, // concurrent_capture
        false, //trigger_in_event
        0 // power_consumption_mw
};

static struct pcm_config stdev_ape_pcm_config = {
    .channels = SOUND_TRIGGER_CHANNEL_MODE,
    .rate = SOUND_TRIGGER_SAMPLING_RATE,
    .period_size = SOUND_TRIGGER_APE_PERIOD_SIZE,
    .period_count = SOUND_TRIGGER_APE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

static struct pcm_config stdev_cpe_pcm_config = {
    .channels = SOUND_TRIGGER_CHANNEL_MODE,
    .rate = SOUND_TRIGGER_SAMPLING_RATE,
    .period_size = SOUND_TRIGGER_CPE_PERIOD_SIZE,
    .period_count = SOUND_TRIGGER_CPE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct sound_trigger_session_fptrs ape_fptrs = {
    .reg_sm = route_reg_sm_ape,
    .reg_sm_params = route_reg_sm_params_ape,
    .dereg_sm = route_dereg_sm_ape,
    .prepare = route_prepare_ape,
    .unprepare = route_unprepare_ape,
    .start = route_start_ape,
    .stop = route_stop_ape,
    .stop_buffering = route_stop_buffering_ape,
    .set_device = route_set_device_ape,
    .read_pcm = route_read_pcm_ape,
    .process_lab_capture = route_lab_capture_ape,
};

struct sound_trigger_session_fptrs cpe_fptrs = {
    .reg_sm = route_reg_sm_cpe,
    .reg_sm_params = route_reg_sm_params_cpe,
    .dereg_sm = route_dereg_sm_cpe,
    .prepare = route_prepare_cpe,
    .unprepare = route_unprepare_cpe,
    .start = route_start_cpe,
    .stop = route_stop_cpe,
    .stop_buffering = route_stop_buffering_cpe,
    .set_device = route_set_device_cpe,
    .read_pcm = route_read_pcm_cpe,
    .process_lab_capture = route_lab_capture_cpe,
};

int pcm_ioctl(struct pcm *pcm, int request, ...)
{
    va_list ap;
    void * arg;
    int pcm_fd = *(int*)pcm;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    return ioctl(pcm_fd, request, arg);
}

#ifdef LSM_POLLING_ENABLE_SUPPORT
static int lsm_set_port(sound_trigger_session_t *p_ses)
{
    int status;

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_PORT);
    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_SET_PORT, status=%d", __func__, status);
    return status;
}
#else
static int lsm_set_port(sound_trigger_session_t *p_ses __unused)
{
    return 0;
}
#endif

static bool is_ssr_active(void)
{
    if ((!stdev->run_on_ape &&
         CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE)) ||
        CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_APE))
        return true;

    return false;
}

void update_hw_mad_exec_mode(st_exec_mode_t mode)
{
    if (stdev->exec_mode != mode) {
        platform_stdev_connect_mad(stdev->platform, mode);
        stdev->exec_mode = mode;
        ALOGV("%s: new exec_mode 0x%x", __func__, mode);
    }
}

static int sound_trigger_set_device
(
   sound_trigger_session_t *p_ses,
   bool enable,
   audio_devices_t device
)
{
    char device_name[DEVICE_NAME_MAX_SIZE] = {0};
    int ref_cnt_idx = 0;
    int status = 0;
    st_device_t st_device;

    st_device = platform_stdev_get_device(stdev->platform, p_ses, device);
    if (st_device < ST_DEVICE_MIN || st_device >= ST_DEVICE_MAX) {
        ALOGE("%s: Invalid sound trigger device %d", __func__, st_device);
        return -EINVAL;
    }

    if (platform_stdev_get_device_name(stdev->platform, p_ses->exec_mode,
                                       st_device, device_name) < 0 ) {
        ALOGE("%s: Invalid sound trigger device returned", __func__);
        return -EINVAL;
    }

    ref_cnt_idx = (p_ses->exec_mode * ST_DEVICE_MAX) + st_device;
    if (enable) {
        stdev->dev_ref_cnt[ref_cnt_idx]++;
        if (stdev->dev_ref_cnt[ref_cnt_idx] == 1) {
            status = platform_stdev_send_calibration(stdev->platform,
                                                     device,
                                                     p_ses,
                                                     ACDB_LSM_APP_TYPE_NO_TOPOLOGY,
                                                     false, ST_DEVICE_CAL);
            if (!status) {
                ALOGD("%s: enable device (%x) = %s", __func__, st_device, device_name);
                audio_route_apply_and_update_path(stdev->audio_route, device_name);
                update_hw_mad_exec_mode(p_ses->exec_mode);
            }
        }
    } else {
        stdev->dev_ref_cnt[ref_cnt_idx]--;
        if (stdev->dev_ref_cnt[ref_cnt_idx] == 0) {
            ALOGD("%s: disable device (%x) = %s", __func__, st_device, device_name);
            audio_route_reset_and_update_path(stdev->audio_route, device_name);
            update_hw_mad_exec_mode(ST_DEVICE_EXEC_MODE_NONE);
        }
    }
    return status;
}

unsigned int get_num_sessions_in_state(enum sound_trigger_states state)
{
    struct listnode *ses_node;
    sound_trigger_session_t *p_ses;
    unsigned int num_sessions = 0;

    if ((list_head(&stdev->sound_model_list) == NULL) ||
                  (list_tail(&stdev->sound_model_list) == NULL)) {
        ALOGE("%s: sound model list is yet to be initialized", __func__);
        return num_sessions;
    }

    list_for_each(ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(ses_node, sound_trigger_session_t, list_node);
        if (CHECK_STATE(p_ses->state, state))
            num_sessions++;
    }

    return num_sessions;
}

static void check_and_exit_lab(sound_trigger_session_t *p_ses)
{
    if (p_ses->lab_buffers_allocated && !p_ses->exit_lab_processing) {
        p_ses->exit_lab_processing = true;
        pthread_mutex_lock(&p_ses->pcm_out_buf_lock);
        pthread_cond_signal(&p_ses->pcm_out_buf_cond);
        pthread_mutex_unlock(&p_ses->pcm_out_buf_lock);
    }
}

static void handle_ssr_online(enum ssr_event_status event)
{
    struct listnode *p_ses_node;
    sound_trigger_session_t *p_ses;
    int status = 0;

    ALOGD("%s: Enter", __func__);
    pthread_mutex_lock(&stdev->lock);

    /* Events allowed: SND_CARD ONLINE or CPE ONLINE
     * 1. Clear state if event is applicable to configured platform(ape/cpe).
     * 2. CPE ONLINE for transitions is handled separately by call to func
     *    transit_handle_cpe_ssr_event and complete tear-down is not required.
     */
    if (event == SND_CARD_STATUS_ONLINE && stdev->run_on_ape) {
        CLEAR_STATE(stdev->ssr_state, ST_DEVICE_SSR_APE);
    } else if (event == CPE_STATUS_ONLINE && !stdev->run_on_ape) {
        CLEAR_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE);
    } else if (event == CPE_STATUS_ONLINE && transit_enabled(stdev) &&
               !CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_APE)) {
        CLEAR_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE);
        transit_handle_cpe_ssr_event(stdev, event);
        pthread_cond_signal(&stdev->cond);
        goto ssr_exit;
    } else {
        ALOGV("%s: nothing to do!!!", __func__);
        goto ssr_exit;
    }

    /* reload and reactive each previously active session */
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

        pthread_mutex_lock(&p_ses->lock);
        status = p_ses->fptrs->reg_sm(p_ses);
        if (!status && p_ses->client_started &&
            !stdev->audio_concurrency_active) {
            p_ses->send_sm_params = true;
            start_recognition_internal(p_ses);
        }
        pthread_mutex_unlock(&p_ses->lock);
    }
    pthread_cond_broadcast(&stdev->cond);

ssr_exit:
    ALOGD("%s: Exit event %d, ssr_state %d", __func__, event, stdev->ssr_state);
    pthread_mutex_unlock(&stdev->lock);
}

static void handle_ssr_offline(enum ssr_event_status event)
{
    struct listnode *p_ses_node;
    sound_trigger_session_t *p_ses;

    ALOGD("%s: Enter", __func__);
    pthread_mutex_lock(&stdev->lock);

    /* Events allowed: SND_CARD OFFLINE or CPE OFFLINE
     * 1. Set state if event is applicable to configured platform(ape/cpe).
     * 2. CPE offline has to follow SND_CARD offline if platform supports both
     *    ape and cpe, i.e transition enabled.
     *    Hence set CPE flag as well on SND_CARD offline event if transition
     *    is enabled.
     * 3. CPE OFFLINE for transitions is handled separately by call to func
     *    transit_handle_cpe_ssr_event and further restoration is not required.
     */
    if (event == SND_CARD_STATUS_OFFLINE && stdev->run_on_ape) {
        SET_STATE(stdev->ssr_state, ST_DEVICE_SSR_APE);
        if (transit_enabled(stdev))
            SET_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE);
    } else if (event == CPE_STATUS_OFFLINE && !stdev->run_on_ape) {
        SET_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE);
    } else if (event == CPE_STATUS_OFFLINE && transit_enabled(stdev) &&
               !CHECK_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE)) {
        SET_STATE(stdev->ssr_state, ST_DEVICE_SSR_CPE);
        transit_handle_cpe_ssr_event(stdev, event);
        goto ssr_exit;
    } else {
        ALOGV("%s: nothing to do!!!", __func__);
        goto ssr_exit;
    }

    /* teardown each session */
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

        /* If lab processing loop in progress, indicate to exit
        before acquiring the lock here */
        check_and_exit_lab(p_ses);
        pthread_mutex_lock(&p_ses->lock);
        stop_recognition_internal(p_ses, false);
        p_ses->fptrs->dereg_sm(p_ses);
        pthread_mutex_unlock(&p_ses->lock);
    }

ssr_exit:
    ALOGD("%s: Exit event %d, ssr_state %d", __func__, event, stdev->ssr_state);
    pthread_mutex_unlock(&stdev->lock);
}

static void handle_audio_concurrency(audio_event_type_t event_type)
{
    struct listnode *p_ses_node;
    sound_trigger_session_t *p_ses;
    bool session_allowed;
    unsigned int num_sessions;
    int status = 0;
    ALOGV("%s: Enter, event type = %d", __func__, event_type);

    /*
    UC1:
        1. start_recognition
        2. audio record_active
        3. audio_record_inactive
        4. stop_recognition
    UC1:
        1. start_recognition
        2. audio record_active
        3. stop_recognition
        4. audio_record_inactive
    UC2:
        1. audio_record_active
        2. start_recognition
        3. stop_recogntion
        4. audio_record_inactive
    UC3:
        1. audio_record_active
        2. start_recognition
        3. audio_record_inactive
        4. stop_recogntion
    */
    pthread_mutex_lock(&stdev->lock);
    num_sessions = get_num_sessions_in_state(SES_CREATED);
    session_allowed = platform_stdev_check_and_update_concurrency(stdev->platform,
                                                event_type, num_sessions);

    if (!num_sessions) {
       pthread_mutex_unlock(&stdev->lock);
       return;
    }

    if (session_allowed && stdev->audio_concurrency_active) {
        stdev->audio_concurrency_active = false;

        if (is_ssr_active()) {
            ALOGD("%s: SSR in progress, concurrency %d", __func__,
                  stdev->audio_concurrency_active);
            goto exit;
        }

        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

            pthread_mutex_lock(&p_ses->lock);
            if (stdev->sw_mad)
                status = p_ses->fptrs->reg_sm(p_ses);
            if (!status && p_ses->client_started)
                start_recognition_internal(p_ses);
            pthread_mutex_unlock(&p_ses->lock);
        }
    } else if (!session_allowed && !stdev->audio_concurrency_active) {
        stdev->audio_concurrency_active = true;

        if (is_ssr_active()) {
            ALOGD("%s: SSR in progress, concurrency %d", __func__,
                  stdev->audio_concurrency_active);
            goto exit;
        }

        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

            /* If lab processing loop in progress, indicate to exit
            before acquiring the lock here */
            check_and_exit_lab(p_ses);
            pthread_mutex_lock(&p_ses->lock);
            stop_recognition_internal(p_ses, false);
            if (stdev->sw_mad)
                p_ses->fptrs->dereg_sm(p_ses);
            pthread_mutex_unlock(&p_ses->lock);
        }
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGV("%s: Exit", __func__);
}

void reg_hal_event_session(sound_trigger_session_t *p_ses)
{
    /* Pass the pcm information to audio hal for capturing LAB */
    if (p_ses->capture_requested && stdev->audio_hal_cb) {
        ALOGD("%s: ST_EVENT_SESSION_REGISTER capture_handle %d session %p",
              __func__, p_ses->capture_handle, p_ses);
        p_ses->event_info.st_ses.p_ses = (void *)p_ses;
        p_ses->event_info.st_ses.capture_handle = p_ses->capture_handle;
        p_ses->event_info.st_ses.config = p_ses->config;
        stdev->audio_hal_cb(ST_EVENT_SESSION_REGISTER, &p_ses->event_info);
    }
}

void dereg_hal_event_session(sound_trigger_session_t *p_ses)
{
    /* Indicate to audio hal that buffering is stopped to stop reading LAB data */
    if (p_ses->capture_requested && stdev->audio_hal_cb) {
        ALOGD("%s: ST_EVENT_SESSION_DEREGISTER capture_handle %d session %p", __func__,
              p_ses->event_info.st_ses.capture_handle, p_ses);
        stdev->audio_hal_cb(ST_EVENT_SESSION_DEREGISTER, &p_ses->event_info);
    }
}

static void ape_enable_use_case(bool enable, sound_trigger_session_t *p_ses)
{
    if (enable) {
        ALOGD("%s: enable use case = %s", __func__,
            stdev->ape_pcm_use_cases[p_ses->use_case_idx].use_case);

        audio_route_apply_and_update_path(stdev->audio_route,
            stdev->ape_pcm_use_cases[p_ses->use_case_idx].use_case);

    } else {
        ALOGD("%s: disable use case = %s", __func__,
              stdev->ape_pcm_use_cases[p_ses->use_case_idx].use_case);

        audio_route_reset_and_update_path(stdev->audio_route,
            stdev->ape_pcm_use_cases[p_ses->use_case_idx].use_case);
    }
}

static int read_pcm_data(sound_trigger_session_t *p_ses,
                     unsigned char *buf,
                     unsigned int bytes)
{
    unsigned int copy_bytes, read_bytes;
    unsigned char *updated_rd_ptr;
    struct timespec tspec;
    int ret = 0;

    pthread_mutex_lock(&p_ses->pcm_out_buf_lock);
    ALOGVV("%s: bytes=%d, unread_bytes=%d size=%d", __func__,
            bytes, p_ses->pcm_out_buf_unread_bytes, p_ses->pcm_out_buf_size);

    while (!p_ses->exit_lab_processing && (bytes > 0)) {
        if (!p_ses->pcm_out_buf_unread_bytes) {
            ALOGVV("%s: waiting on cond, bytes=%d", __func__, bytes);
            /* Time out to unblock read thread in case if write thread is
               stuck filling the buffers */
            clock_gettime(CLOCK_REALTIME, &tspec);
            tspec.tv_sec += ST_READ_WAIT_TIME_OUT_SEC;
            ret = pthread_cond_timedwait(&p_ses->pcm_out_buf_cond,
                                         &p_ses->pcm_out_buf_lock, &tspec);
            if (ret) {
                ALOGE("%s: ERROR. read wait timed out, ret %d", __func__, ret);
                p_ses->exit_lab_processing = true;
                ret = -EIO;
                goto exit;
            }
            if (p_ses->exit_lab_processing) {
                ALOGVV("%s: buffering stopped while waiting on cond, exiting", __func__);
                ret = -EIO;
                goto exit;
            }
        }
        read_bytes = MIN(bytes, p_ses->pcm_out_buf_unread_bytes);

        if (p_ses->pcm_out_buf_end_ptr < (p_ses->pcm_out_buf_rd_ptr + read_bytes)) {
            copy_bytes = p_ses->pcm_out_buf_end_ptr - p_ses->pcm_out_buf_rd_ptr;
            memcpy(buf, p_ses->pcm_out_buf_rd_ptr, copy_bytes);
            memcpy(buf + copy_bytes, p_ses->pcm_out_buf_start_ptr, read_bytes - copy_bytes);
            updated_rd_ptr = p_ses->pcm_out_buf_start_ptr + (read_bytes - copy_bytes);
            p_ses->pcm_out_buf_rd_ptr = updated_rd_ptr;
            p_ses->pcm_out_buf_unread_bytes -= read_bytes;
        } else {
            memcpy(buf, p_ses->pcm_out_buf_rd_ptr, read_bytes);
            p_ses->pcm_out_buf_rd_ptr += read_bytes;
            p_ses->pcm_out_buf_unread_bytes -= read_bytes;
        }
        pthread_cond_signal(&p_ses->pcm_out_buf_cond);
        bytes -= read_bytes;
        buf += read_bytes;
    }

exit:
    pthread_mutex_unlock(&p_ses->pcm_out_buf_lock);
    return 0;
}

static int write_pcm_data(sound_trigger_session_t *p_ses,
                     unsigned char *buf,
                     unsigned int bytes)
{
    unsigned int free_bytes, write_bytes;
    unsigned int copy_bytes;
    unsigned char *updated_wr_ptr;

    pthread_mutex_lock(&p_ses->pcm_out_buf_lock);
    ALOGVV("%s: bytes=%d, unread_bytes=%d size=%d", __func__,
            bytes, p_ses->pcm_out_buf_unread_bytes, p_ses->pcm_out_buf_size);

    while (!p_ses->exit_lab_processing && (bytes > 0)) {
        free_bytes = p_ses->pcm_out_buf_size - p_ses->pcm_out_buf_unread_bytes;
        if (!free_bytes) {
            ALOGVV("%s: waiting on cond, bytes=%d", __func__, bytes);
            /* wait till space is avaialable to write */
            pthread_cond_wait(&p_ses->pcm_out_buf_cond, &p_ses->pcm_out_buf_lock);
            if (p_ses->exit_lab_processing) {
                ALOGVV("%s: buffering stopped while waiting on cond, exiting", __func__);
                pthread_mutex_unlock(&p_ses->pcm_out_buf_lock);
                return -EIO;
            }
        }
        write_bytes = MIN(bytes, free_bytes);

        if (p_ses->pcm_out_buf_end_ptr < (p_ses->pcm_out_buf_wr_ptr + write_bytes)) {
            copy_bytes = p_ses->pcm_out_buf_end_ptr - p_ses->pcm_out_buf_wr_ptr;
            memcpy(p_ses->pcm_out_buf_wr_ptr, buf, copy_bytes);
            memcpy(p_ses->pcm_out_buf_start_ptr,
                   buf + copy_bytes, write_bytes - copy_bytes);
            updated_wr_ptr = p_ses->pcm_out_buf_start_ptr + (write_bytes - copy_bytes);
            p_ses->pcm_out_buf_wr_ptr = updated_wr_ptr;
            p_ses->pcm_out_buf_unread_bytes += write_bytes;
        } else {
            memcpy(p_ses->pcm_out_buf_wr_ptr, buf, write_bytes);
            p_ses->pcm_out_buf_wr_ptr += write_bytes;
            p_ses->pcm_out_buf_unread_bytes += write_bytes;
        }
        pthread_cond_signal(&p_ses->pcm_out_buf_cond);
        bytes -= write_bytes;
        buf += write_bytes;
    }
    pthread_mutex_unlock(&p_ses->pcm_out_buf_lock);
    return 0;
}

static int read_capture_bytes(struct pcm *pcm, unsigned char *buf, size_t bytes)
{
    struct listnode *p_ses_node;
    sound_trigger_session_t *p_ses;
    bool ses_found = false;
    int ret = 0;

    if (!pcm || !buf || !bytes) {
        ALOGE("%s: Invalid params", __func__);
        return -EINVAL;
    }

    pthread_mutex_lock(&stdev->lock);
    if (is_ssr_active()) {
        ALOGE("%s: SSR in progress", __func__);
        pthread_mutex_unlock(&stdev->lock);
        return -EIO;
    }
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);
        if (p_ses->pcm == pcm) {
            ses_found = true;
            break;
        }
    }
    if (!ses_found) {
        ALOGE("%s: Session not active", __func__);
        pthread_mutex_unlock(&stdev->lock);
        return -EIO;
    }
    pthread_mutex_unlock(&stdev->lock);

    ret = p_ses->fptrs->read_pcm(p_ses, buf, bytes);

    ALOGVV("%s: Exit ret=%d", __func__, ret);
    return ret;
}

static bool search_for_syncword(sound_trigger_session_t *p_ses,
                                unsigned char *in_buf,
                                unsigned int in_size,
                                unsigned int *pos)
{

    unsigned char *buf_ptr = in_buf;
    unsigned int inc_bytes = 2;

    /* S16_LE is sampled at 16bit boundary, 24_LE and 32_LE will be
       sampled at 4byte boundary */
    if ((p_ses->lab_config.format == PCM_FORMAT_S32_LE) ||
        (p_ses->lab_config.format == PCM_FORMAT_S24_LE)) {
        inc_bytes += 2;
    }
    *pos = 0;

    while (buf_ptr < (in_buf + in_size)) {
        if (*buf_ptr == CPE_PACKET_SYNC_WORD) {
            *pos = buf_ptr - in_buf;
            ALOGVV("%s: first syncword found at idx %d for format %d",
                   __func__, *pos, p_ses->lab_config.format);
            return true;
        }
        buf_ptr += inc_bytes;
    }
    return false;
}

static void get_ftrt_processing_data(sound_trigger_session_t *p_ses,
                              unsigned char *in_buf,
                              unsigned int in_size,
                              unsigned char **out_buf,
                              unsigned int *out_size)
{
    unsigned char *src, *dst;

    if (p_ses->lab_config.format == PCM_FORMAT_S24_LE) {
        /* Construct ftrt packed buffer without gaps by skipping MSB byte */
        src = dst = in_buf;
        while (src < (in_buf + in_size)) {
            memmove(dst, src, 3);
            src += 4;
            dst += 3;
        }
        *out_buf = in_buf;
        *out_size = (dst - *out_buf);
    } else {
        *out_buf = in_buf;
        *out_size = in_size;
    }
}

void process_raw_lab_data(sound_trigger_session_t *p_ses)
{
    int status = 0;
    static int file_cnt= 0;

    ST_DBG_DECALRE(FILE *fptr_drv = NULL;);
    ST_DBG_FILE_OPEN_WR(fptr_drv, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_drv_data", "pcm", file_cnt++);

    /* Initialize pcm output buffer pointers */
    p_ses->pcm_out_buf_start_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_end_ptr = p_ses->pcm_out_buf + p_ses->pcm_out_buf_size;
    p_ses->pcm_out_buf_wr_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_rd_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_unread_bytes = 0;

    while (!p_ses->exit_lab_processing) {
        ALOGVV("%s: pcm_read reading bytes=%d", __func__, p_ses->lab_drv_buf_size);
        status = pcm_read(p_ses->pcm, p_ses->lab_drv_buf, p_ses->lab_drv_buf_size);
        ALOGVV("%s: pcm_read done", __func__);

        ST_DBG_FILE_WRITE(fptr_drv, p_ses->lab_drv_buf, p_ses->lab_drv_buf_size);

        if (status) {
            ALOGE("%s: pcm read failed status %d - %s", __func__, status,
                  pcm_get_error(p_ses->pcm));
            p_ses->exit_lab_processing = true;
            p_ses->fptrs->stop_buffering(p_ses);
            break;
        }

        status = write_pcm_data(p_ses, p_ses->lab_drv_buf, p_ses->lab_drv_buf_size);
        if (status)
            break;
    }

    ST_DBG_FILE_CLOSE(fptr_drv);
    ALOGVV("%s: Exit status=%d", __func__, status);
}

void process_packetized_lab_data(sound_trigger_session_t *p_ses)
{
    struct cpe_packet_hdr *prev_packet = NULL;
    bool first_sync_word_found = false;
    bool read_next_dr_buf = false;
    int sync_max_retry_cnt = SOUND_TRIGGER_SYNC_WORD_MAX_RETRY_CNT;
    int status = 0, dec_status = 0, out_samples = 0;
    unsigned int rt_period_bytes, copy_bytes, dec_bytes, tmp_size;
    unsigned char *driver_rd_buf;
    unsigned int driver_rd_bytes, dr_buf_idx = 0;
    unsigned char *ftrt_buf_ptr;
    unsigned int ftrt_buf_size, ftrt_rem_bytes, ftrt_buf_idx = 0;
    unsigned char *frame_buf_ptr;
    unsigned int frame_buf_idx = 0;
    unsigned char *dec_out_buf;
    unsigned char packet_hdr[sizeof(struct cpe_packet_hdr)];
    unsigned int hdr_size = sizeof(struct cpe_packet_hdr);
    unsigned int hdr_buf_idx = 0;

    ST_DBG_DECALRE(FILE *fptr_drv = NULL, *fptr_pcm = NULL, *fptr_dec = NULL;
                   static int fcnt = 0, dec_fcnt = 0;);
    ST_DBG_FILE_OPEN_WR(fptr_drv, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_drv_data", "bin", fcnt);
    ST_DBG_FILE_OPEN_WR(fptr_pcm, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_output", "pcm", fcnt++);

    /* Initialize pcm output buffer pointers */
    p_ses->pcm_out_buf_start_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_end_ptr = p_ses->pcm_out_buf + p_ses->pcm_out_buf_size;
    p_ses->pcm_out_buf_wr_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_rd_ptr = p_ses->pcm_out_buf;
    p_ses->pcm_out_buf_unread_bytes = 0;

    driver_rd_buf = p_ses->lab_drv_buf;
    driver_rd_bytes = p_ses->lab_drv_buf_size;

    /*
    1. Read from driver
    2. Find first sync word
    3. Skip MSB
    4. Skip Null packets
    5. Store in buffer
    */
    while (!p_ses->exit_lab_processing) {

        ALOGVV("%s: pcm_read reading bytes=%d", __func__, driver_rd_bytes);
        status = pcm_read(p_ses->pcm, driver_rd_buf, driver_rd_bytes);
        ALOGVV("%s: pcm_read done", __func__);

        ST_DBG_FILE_WRITE(fptr_drv, driver_rd_buf, driver_rd_bytes);

        if (status) {
            ALOGE("%s: pcm read failed status %d - %s", __func__, status,
                  pcm_get_error(p_ses->pcm));
            break;
        }

        if (!first_sync_word_found) {
            first_sync_word_found = search_for_syncword(p_ses, driver_rd_buf,
                                                        driver_rd_bytes, &dr_buf_idx);
            if (!first_sync_word_found) {
                if(--sync_max_retry_cnt <= 0) {
                    ALOGE("%s: max tries for finding sync word reached", __func__);
                    goto exit;
                } else {
                    /* Read next buffer from driver */
                    dr_buf_idx = 0;
                    continue;
                }
            } else {
                /* adpcm decoder needs to be initialized upon every utterance.
                   TODO: format check if compressed/uncompressed switch */
                if (CHECK_BIT(p_ses->vendor_uuid_info->kw_capture_format,
                              ADPCM)) {
                    dec_status = stdev->adpcm_dec_init(p_ses->adpcm_dec_state);
                    if (dec_status) {
                        ALOGE("%s: ERROR. ADPCM decoder init failed status %d",
                               __func__, dec_status);
                        goto exit;
                    }
                    ST_DBG_FILE_OPEN_WR(fptr_dec, ST_DEBUG_DUMP_LOCATION,
                                        "st_lab_adpcm_input", "bin", dec_fcnt++);
                }
            }
        }
        get_ftrt_processing_data(p_ses, &driver_rd_buf[dr_buf_idx],
                                 driver_rd_bytes - dr_buf_idx,
                                 &ftrt_buf_ptr, &ftrt_buf_size);

        while (!p_ses->exit_lab_processing) {

            if (!prev_packet) {
                /* construct new packet */
                tmp_size = MIN((hdr_size - hdr_buf_idx), (ftrt_buf_size - ftrt_buf_idx));
                memcpy(&packet_hdr[hdr_buf_idx], &ftrt_buf_ptr[ftrt_buf_idx], tmp_size);
                hdr_buf_idx += tmp_size;
                ftrt_buf_idx += tmp_size;
                if (hdr_buf_idx == hdr_size) {
                    /* We have a new packet now, parse it */
                    prev_packet = (struct cpe_packet_hdr *)packet_hdr;
                    hdr_buf_idx = 0;
                    /* sync word must be present at packet boundary */
                    if (prev_packet->sync_word != CPE_PACKET_SYNC_WORD) {
                        ALOGE("%s: ERROR. sync word not present", __func__);
                        goto exit;
                    }
                    if (prev_packet->format == CPE_PACKET_FORMAT_EOS) {
                        ALOGVV("%s: EOS ??", __func__);
                        goto exit; /* TODO-V: Not expected now. FTRT to RT switch */
                    }
                }
            }

            if (prev_packet) {
                if (prev_packet->format == CPE_PACKET_FORMAT_NULL) {
                    /* Skip the NULL packets */
                    tmp_size = MIN(prev_packet->size, (ftrt_buf_size - ftrt_buf_idx));
                    ALOGVV("%s: null packet skip: size=%d ftrt_buf_idx=%d ftrt_buf_size=%d prev_packet->size=%d",
                          __func__, tmp_size, ftrt_buf_idx, ftrt_buf_size, prev_packet->size);
                    prev_packet->size -= tmp_size;
                    ftrt_buf_idx += tmp_size;
                } else if (prev_packet->format == CPE_PACKET_FORMAT_PCM) {
                    copy_bytes = MIN(prev_packet->size, (ftrt_buf_size - ftrt_buf_idx));
                    status = write_pcm_data(p_ses, &ftrt_buf_ptr[ftrt_buf_idx], copy_bytes);
                    if (status)
                        goto exit;

                    ST_DBG_FILE_WRITE(fptr_pcm, &ftrt_buf_ptr[ftrt_buf_idx], copy_bytes);
                    ftrt_buf_idx += copy_bytes;
                    prev_packet->size -= copy_bytes;
                } else if (prev_packet->format == CPE_PACKET_FORMAT_ADPCM) {
                    dec_out_buf = p_ses->dec_buf + ADPCM_MAX_IN_FRAME_SIZE;

                    while (!p_ses->exit_lab_processing &&
                           (ftrt_buf_idx < ftrt_buf_size) &&
                           (prev_packet->size > 0)) {

                        dec_bytes = MIN(prev_packet->size, ADPCM_MAX_IN_FRAME_SIZE);
                        ftrt_rem_bytes = ftrt_buf_size - ftrt_buf_idx;

                        if (!frame_buf_idx && (ftrt_rem_bytes >= dec_bytes)) {
                            /* There are no cached partial frame samples in frame buf.
                               We have full frame in ftrt buf for decoding. Use ftrt
                               buf directly as decoder input */
                            frame_buf_ptr = &ftrt_buf_ptr[ftrt_buf_idx];
                        } else {
                            /* Compute least of all buffer sizes to not to overflow */
                            dec_bytes = MIN(MIN(dec_bytes, ftrt_rem_bytes), (ADPCM_MAX_IN_FRAME_SIZE - frame_buf_idx));
                            memcpy(&p_ses->dec_buf[frame_buf_idx], &ftrt_buf_ptr[ftrt_buf_idx], dec_bytes);
                            frame_buf_ptr = p_ses->dec_buf;
                        }
                        frame_buf_idx += dec_bytes;
                        ftrt_buf_idx += dec_bytes;
                        prev_packet->size -= dec_bytes;

                        ST_DBG_FILE_WRITE(fptr_dec, frame_buf_ptr, frame_buf_idx);
                        if (!prev_packet->size || (frame_buf_idx == ADPCM_MAX_IN_FRAME_SIZE)) {
                            /* if packet size is zero, we may have partial frame to be decoded */
                            ALOGVV("%s: enter for decode- frame_buf_idx=%d ftrt_buf_idx=%d prev_packet->size=%d",
                                   __func__, frame_buf_idx, ftrt_buf_idx, prev_packet->size);

                            /* Decode ADPCM frame and copy the data to pcm out buffer */
                            dec_status = stdev->adpcm_dec_process((short *)frame_buf_ptr,
                                                                  (short *)dec_out_buf,
                                                                  frame_buf_idx,
                                                                  &out_samples,
                                                                  p_ses->adpcm_dec_state);
                            if (dec_status) {
                                ALOGE("%s: ERROR. adpcm dec failed status %d", __func__, dec_status);
                                goto exit;
                            }
                            ALOGVV("%s: adpcm_dec_process done. frame_buf_idx=%d out_samples=%d",
                                   __func__, frame_buf_idx, out_samples);
                            if (out_samples) {
                                ST_DBG_FILE_WRITE(fptr_pcm, dec_out_buf, out_samples << 1);
                                status = write_pcm_data(p_ses, dec_out_buf, out_samples << 1);
                                if (status)
                                    goto exit;
                            }
                            frame_buf_idx = 0;
                        }
                    }
                } else {
                    ALOGE("%s: Uknown packet format %d ", __func__, prev_packet->format);
                    goto exit;
                }

                if (!prev_packet->size) {
                    /* packet is processed, remove its reference */
                    prev_packet = NULL;
                }
            }

            if (ftrt_buf_idx == ftrt_buf_size) {
                ALOGVV("%s: ftrt buffer exhausted, read next buffer ftrt_buf_idx=%d",
                       __func__, ftrt_buf_idx);
                ftrt_buf_idx = dr_buf_idx = 0;
                break;
            }
        }
    }

exit:
    if (!p_ses->exit_lab_processing) {
        p_ses->exit_lab_processing = true;
        p_ses->fptrs->stop_buffering(p_ses);
    }

    ST_DBG_FILE_CLOSE(fptr_drv);
    ST_DBG_FILE_CLOSE(fptr_dec);
    ST_DBG_FILE_CLOSE(fptr_pcm);

    ALOGVV("%s: Exit status=%d", __func__, status);
}

static int send_phrase_recognition_event(sound_trigger_session_t *p_ses,
                                         int event_status,
                                         struct snd_lsm_event_status *params)
{
    struct sound_trigger_phrase_recognition_event *event = NULL;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm= p_ses->sm_data;
    recognition_callback_t callback;
    void *cookie;

    int status = 0;
    unsigned int i, j;

    if (v_info && v_info->smlib_handle) {
        /* if smlib is present, get the event from it, else send the
           DSP recieved payload as it is to App */
        status = v_info->generate_st_phrase_recognition_event(
                                        phrase_sm,
                                        p_ses->rc_config,
                                        params->payload,
                                        params->payload_size, &event);

        if (status || !event) {
            ALOGW("%s: smlib fill recognition event failed, status %d",
                  __func__, status);
            if (event)
                free(event);
            status = -ENOMEM;
            goto exit;
        }
    } else if (!v_info && stdev->smlib_handle) {
        /* This is SVA non topology solution */
        status = stdev->generate_st_phrase_recognition_event(
                                       phrase_sm,
                                       p_ses->rc_config,
                                       params->payload,
                                       params->payload_size, &event);
        if (status || !event) {
            ALOGW("%s: SVA smlib fill recognition event failed, status %d",
                  __func__, status);
            if (event)
                free(event);
            status = -ENOMEM;
            goto exit;
        }
    } else {
        event = calloc(1, sizeof(*event) + params->payload_size);
        if (!event) {
            ALOGE("%s: event allocation failed, size %d", __func__,
                  params->payload_size);
            status = -ENOMEM;
            goto exit;
        }

        memcpy(event->phrase_extras, p_ses->rc_config->phrases,
               p_ses->rc_config->num_phrases *
               sizeof(struct sound_trigger_phrase_recognition_extra));

        event->num_phrases = p_ses->rc_config->num_phrases;
        event->common.data_offset = sizeof(*event);
        event->common.data_size = params->payload_size;
        memcpy((char *)event + event->common.data_offset,
               params->payload, params->payload_size);
    }

    /* fill the remaining recognition event parameters not specific
       to soundmodel lib */
    event->common.status = event_status;
    event->common.type = p_ses->sm_type;
    event->common.model = p_ses->sm_handle;
    event->common.capture_available = p_ses->capture_requested;
    event->common.capture_delay_ms = 0;
    event->common.capture_preamble_ms = 0;
    event->common.audio_config.sample_rate = v_info ?
            v_info->sample_rate : SOUND_TRIGGER_SAMPLING_RATE;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    for (i = 0; i < event->num_phrases; i++) {
        ALOGVV("%s: [%d] kw_id %d level %d", __func__,i,
              event->phrase_extras[i].id,
              event->phrase_extras[i].confidence_level);
        for (j = 0; j < event->phrase_extras[i].num_levels; j++) {
            ALOGVV("%s: [%d] user_id %d level %d ", __func__, i,
                  event->phrase_extras[i].levels[j].user_id,
                  event->phrase_extras[i].levels[j].level);
        }
    }

    ALOGI("%s:[%d] send keyphrase recognition event %d", __func__,
                     p_ses->sm_handle, event_status);
    ALOGV("%s:[%d] status=%d, type=%d, model=%d, capture_avaiable=%d, "
           "num_phrases=%d id=%d", __func__, p_ses->sm_handle,
           event->common.status, event->common.type, event->common.model,
           event->common.capture_available, event->num_phrases,
           event->phrase_extras[0].id);

    p_ses->client_started = false;
    callback = p_ses->callback;
    cookie = p_ses->cookie;
    pthread_mutex_unlock(&p_ses->lock);
    if (callback)
        callback(&event->common, cookie);
    pthread_mutex_lock(&p_ses->lock);
    free(event);

exit:
    return status;
}

static int send_generic_recognition_event(sound_trigger_session_t *p_ses,
                                          int event_status,
                                          struct snd_lsm_event_status *params)
{
    struct sound_trigger_generic_recognition_event *event = NULL;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    recognition_callback_t callback;
    void *cookie;
    int status = 0;

    event = calloc(1, sizeof(*event) + params->payload_size);
    if (!event) {
        ALOGE("%s: event allocation failed, size %d", __func__,
              params->payload_size);
        status = -ENOMEM;
        goto exit;
    }

    event->common.status = event_status;
    event->common.type = p_ses->sm_type;
    event->common.model = p_ses->sm_handle;
    event->common.capture_available = p_ses->capture_requested;
    event->common.capture_delay_ms = 0;
    event->common.capture_preamble_ms = 0;
    event->common.audio_config.sample_rate = v_info ?
            v_info->sample_rate : SOUND_TRIGGER_SAMPLING_RATE;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    event->common.data_offset = sizeof(*event);
    event->common.data_size = params->payload_size;
    memcpy((char *)event + event->common.data_offset,
           params->payload, params->payload_size);

    ALOGI("%s:[%d] send generic recognition event %d", __func__,
                     p_ses->sm_handle, event_status);

    ALOGV("%s:[%d] status=%d, type=%d, model=%d, capture_avaiable=%d",
          __func__, p_ses->sm_handle, event->common.status,
          event->common.type, event->common.model,
          event->common.capture_available);

    p_ses->client_started = false;
    callback = p_ses->callback;
    cookie = p_ses->cookie;
    pthread_mutex_unlock(&p_ses->lock);
    if (callback)
        callback(&event->common, cookie);
    pthread_mutex_lock(&p_ses->lock);
    free(event);

exit:
    return status;
}

static void *callback_thread_loop(void *context)
{
    struct sound_trigger_session *st_session =
                                 (struct sound_trigger_session *)context;
    struct sound_trigger_session *p_session = st_session;
    struct sound_trigger_device *stdev = NULL;
    struct snd_lsm_event_status *params;
    unsigned int payload_alloc_size = SOUND_TRIGGER_MAX_EVNT_PAYLOAD_SIZE;
    int status = 0;
    int event_status;
    unsigned int i, j;

    if (st_session == NULL) {
        ALOGE("%s: ERROR. null context.. exiting", __func__);
        return NULL;
    }
    ALOGD("%s:[%d] Enter", __func__, st_session->sm_handle);

    transit_retrieve_st_session(&st_session);

    stdev = (struct sound_trigger_device *)st_session->device;
    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_DEFAULT);
    prctl(PR_SET_NAME, (unsigned long)"sound trigger callback", 0, 0, 0);

    pthread_mutex_lock(&st_session->lock);
    params = calloc(1, sizeof(*params) + payload_alloc_size);
    if (params == NULL) {
        ALOGE("%s: ERROR. insufficient memory for payload", __func__);
        goto exit;
    }

    while (!p_session->exit_callback_thread) {
        params->payload_size = payload_alloc_size;
        ALOGI("%s:[%d] Waiting for SNDRV_LSM_EVENT_STATUS",
                            __func__, st_session->sm_handle);
        pthread_mutex_unlock(&st_session->lock);
        status = pcm_ioctl(p_session->pcm, SNDRV_LSM_EVENT_STATUS, params);
        pthread_mutex_lock(&st_session->lock);
        ALOGI("%s:[%d] Received SNDRV_LSM_EVENT_STATUS status=%d",
              __func__, st_session->sm_handle, status);
        if (status < 0) {
            if (errno == ENOMEM) {
                payload_alloc_size = payload_alloc_size << 1;
                params = realloc(params, sizeof(*params) + payload_alloc_size);

                if (params == NULL) {
                    ALOGE("%s: ERROR. Not enough memory for payload",
                          __func__);
                    p_session->exit_callback_thread = true;
                    break;
                }
                continue;
            } else {
                ALOGE("%s: ERROR. SNDRV_LSM_EVENT_STATUS  failed status %d",__func__, status);
                p_session->exit_callback_thread = true;
                break;
            }
        }
        if (p_session->exit_callback_thread)
            break;
        if (!st_session->callback)
            continue;

        ALOGD("%s: params status %d", __func__, params->status);
        switch (params->status) {
        case LSM_VOICE_WAKEUP_STATUS_RUNNING:
            continue;
        case LSM_VOICE_WAKEUP_STATUS_DETECTED:
            event_status = RECOGNITION_STATUS_SUCCESS;
            if (st_session->capture_requested)
                SET_STATE(st_session->state, SES_BUFFERING);

            if (!st_session->client_started ||
                stdev->audio_concurrency_active) {
                ALOGD("%s: ignore event, client_started %d concurrency %d",
                      __func__, st_session->client_started,
                      stdev->audio_concurrency_active);
                st_session->fptrs->stop_buffering(st_session);
                continue;
            }
            break;
        case LSM_VOICE_WAKEUP_STATUS_END_SPEECH:
        case LSM_VOICE_WAKEUP_STATUS_REJECTED:
            event_status = RECOGNITION_STATUS_FAILURE;
            break;
        default:
            ALOGW("%s: Unknown status %d", __func__, params->status);
            continue;
        }

        if (st_session->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
            status = send_phrase_recognition_event(st_session, event_status, params);
        else
            status = send_generic_recognition_event(st_session, event_status, params);

        /* Since the client event callback is called without ses_lock held,
         * it is possible that the client would have done stop/unload even
         * before the contorl is here after reaquiring the ses_lock. Check
         * the session state accordingly.
         */
        if (!st_session->callback)
            continue;

        if (status) {
            /* Stop buffering if this is not a successful detection and
             * LAB is triggered in lsm driver. Continue waiting for next event
             */
            st_session->fptrs->stop_buffering(st_session);
            continue;
        }

        if (st_session->capture_requested) {
            if (event_status == RECOGNITION_STATUS_SUCCESS) {
                /* Cache lab data to internal buffers */
                st_session->fptrs->process_lab_capture(st_session);
            }
        } else {
            /* Stop recognition after sending detection event. If LAB is
             * in progress, stop recognition after buffering stopped from AHAL.
             * Also it is possible that client would have restarted recognition
             * in callback context. Check and stop only if need to.
             */
            if (!st_session->client_started) {
                ALOGV("%s: stopping after sending detection event", __func__);
                stop_recognition_internal(st_session, true);
            }
        }
    }
    if (params)
        free(params);

exit:
    pthread_mutex_unlock(&st_session->lock);
    ALOGD("%s:[%d] Exit", __func__, st_session->sm_handle);
    return NULL;
}

static int deallocate_lab_buffers(sound_trigger_session_t* p_ses)
{
    ALOGVV("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (p_ses->adpcm_dec_state) {
        free(p_ses->adpcm_dec_state);
        p_ses->adpcm_dec_state = NULL;
    }
    if (p_ses->dec_buf) {
        free(p_ses->dec_buf);
        p_ses->dec_buf = NULL;
    }
    if (p_ses->pcm_out_buf) {
        free(p_ses->pcm_out_buf);
        p_ses->pcm_out_buf = NULL;
    }
    if (p_ses->lab_drv_buf) {
        free(p_ses->lab_drv_buf);
        p_ses->lab_drv_buf = NULL;
    }
    pthread_cond_destroy(&p_ses->pcm_out_buf_cond);
    pthread_mutex_destroy(&p_ses->pcm_out_buf_lock);
    p_ses->lab_buffers_allocated = false;

    return 0;
}

static int configure_lab(sound_trigger_session_t* p_ses)
{
    int status = 0, buf_en = 0;
    struct snd_lsm_output_format_cfg media_cfg;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;

    media_cfg.format = LSM_OUT_FORMAT_PCM;
    media_cfg.packing = LSM_OUT_DATA_RAW;
    media_cfg.mode = LSM_OUT_TRANSFER_MODE_RT;
    media_cfg.events = LSM_OUT_DATA_EVENTS_DISABLED;

    if (v_info && (v_info->kw_transfer_mode == FTRT_TRANSFER_MODE)) {
        media_cfg.packing = LSM_OUT_DATA_PACKED;
        media_cfg.mode = LSM_OUT_TRANSFER_MODE_FTRT;

        if (CHECK_BIT(v_info->kw_capture_format, ADPCM))
            media_cfg.format = LSM_OUT_FORMAT_ADPCM;
    }

    ALOGD("%s: media_cfg format=%d, packing=%d, events=%d, mode=%d", __func__,
          media_cfg.format, media_cfg.packing, media_cfg.events, media_cfg.mode);
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_OUT_FORMAT_CFG, &media_cfg);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_OUT_FORMAT_CFG, status=%d", __func__, status);
        return status;
    }

    buf_en = p_ses->capture_requested ? 1 : 0;
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d", __func__, status);

    return status;
}

static int allocate_lab_buffers(sound_trigger_session_t* p_ses)
{
    int status = 0;
    int adpcm_dec_scratch_size;
    int dec_status;
    unsigned int kw_tranfer_duration, rt_bytes_one_sec, ftrt_to_rt_factor;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;

    ALOGVV("%s:[%d] Enter", __func__, p_ses->sm_handle);

    pthread_mutex_init(&p_ses->pcm_out_buf_lock, (const  pthread_condattr_t *) NULL);
    pthread_cond_init(&p_ses->pcm_out_buf_cond, (const  pthread_condattr_t *) NULL);

    p_ses->lab_drv_buf_size = pcm_frames_to_bytes(p_ses->pcm, p_ses->lab_config.period_size);
    p_ses->lab_drv_buf = (unsigned char *) calloc(1, p_ses->lab_drv_buf_size);
    if (!p_ses->lab_drv_buf) {
        ALOGE("%s: ERROR. Can not allcoate lab buffer size %d", __func__, p_ses->lab_drv_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated lab buffer period size bytes =%d",
          __func__, p_ses->lab_drv_buf_size);

    rt_bytes_one_sec = (p_ses->config.rate * p_ses->config.channels *
                        (pcm_format_to_bits(p_ses->config.format) >> 3));

    if (v_info) {
        p_ses->pcm_out_buf_size = (v_info->client_capture_read_delay * rt_bytes_one_sec) / 1000;
        if (v_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {
            ftrt_to_rt_factor = (p_ses->lab_config.rate * pcm_format_to_bits(p_ses->lab_config.format)) /
                                (p_ses->config.rate * pcm_format_to_bits(p_ses->config.format));

            kw_tranfer_duration = v_info->kw_duration / ftrt_to_rt_factor;
            ALOGV("%s: kw_tranfer_duration %d, kw_duration %d, ftrt_to_rt_factor %d",
                   __func__, kw_tranfer_duration, v_info->kw_duration, ftrt_to_rt_factor);
            if (kw_tranfer_duration < v_info->client_capture_read_delay) {
                /* Full keyword gets transmitted within App delay, accomodate full
                   keyword duration size plus the client read delay time of real time data size.
                   Ex: 2sec keyword duration at 384KHz/32bit FTRT speed gets transfered in ~40ms.
                   If configured App delay is 1sec, accomdate realtime data for 1sec delay in
                   addition to 2sec keyword data. So the buffer size becomes
                   (2000ms + 1000ms) * 32KB/1000ms = ~96KB */
                p_ses->pcm_out_buf_size = (v_info->kw_duration + v_info->client_capture_read_delay) *
                                          rt_bytes_one_sec / 1000;
            } else {
                /* Accomdate client read delay worth of ftrt keyword data.
                   Ex: 2sec keyword duration at 16KHz/32bit FTRT speed gets transfered
                   in 1000ms. If configured App delay is 500ms, all that we need to store
                   is 500ms worth for FTRT data. So the buffer size becomes
                   (500ms * 2) * 32KB/1000ms = 32KB */
                p_ses->pcm_out_buf_size *= ftrt_to_rt_factor;
            }
        }
    } else {
        p_ses->pcm_out_buf_size = (DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS * rt_bytes_one_sec) / 1000;
    }

    p_ses->pcm_out_buf = (unsigned char *) calloc(1, p_ses->pcm_out_buf_size);
    if (!p_ses->pcm_out_buf) {
        ALOGE("%s: ERROR. Can not allcoate out buffer size %d",
              __func__, p_ses->pcm_out_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated out buffer size=%d", __func__, p_ses->pcm_out_buf_size);

    if (v_info && CHECK_BIT(v_info->kw_capture_format, ADPCM)) {
        p_ses->dec_buf = (unsigned char *) calloc(1, ADPCM_MAX_IN_FRAME_SIZE +
                                                  ADPCM_MAX_OUT_FRAME_SIZE);
        if (!p_ses->dec_buf) {
            ALOGE("%s: ERROR. Can not allcoate decoder buffer size %d",
                  __func__, ADPCM_MAX_IN_FRAME_SIZE + ADPCM_MAX_OUT_FRAME_SIZE);
            status = -ENOMEM;
            goto error_exit;
        }
        ALOGV("%s: Allocated decoder buffers size=%d", __func__,
               ADPCM_MAX_IN_FRAME_SIZE + ADPCM_MAX_OUT_FRAME_SIZE);

        /* Instantiate the decoder */
        status = stdev->adpcm_dec_get_scratch_size(&adpcm_dec_scratch_size);
        if (status) {
            ALOGE("%s: ERROR. Can not get adpcm dec scratch", __func__);
            status = -EINVAL;
            goto error_exit;
        }

        p_ses->adpcm_dec_state = (void *)calloc(1, adpcm_dec_scratch_size);
        if(!p_ses->adpcm_dec_state)
        {
            ALOGE("%s: ERROR. Can not allcoate decoder scratch", __func__);
            status = -ENOMEM;
            goto error_exit;
        }
        ALOGV("%s: Allocated adpcm_dec_state size=%d", __func__,
              adpcm_dec_scratch_size);
    }
    p_ses->lab_buffers_allocated = true;

    return 0;

error_exit:
    deallocate_lab_buffers(p_ses);
    return status;
}

static int start_pcm(sound_trigger_session_t* p_ses)
{
    int status = 0, retry_num = 0;

    ALOGV("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_start(p_ses->pcm);
    while (status && (retry_num < SOUND_TRIGGER_PCM_MAX_RETRY)) {
        usleep(SOUND_TRIGGER_PCM_SLEEP_WAIT);
        retry_num++;
        ALOGI("%s: pcm_start %s, retry cnt %d", __func__,
              pcm_get_error(p_ses->pcm), retry_num);
        status = pcm_start(p_ses->pcm);
    }
    if (status)
        ALOGE("%s: ERROR. pcm_start failed, %s", __func__,
              pcm_get_error(p_ses->pcm));

    ALOGV("%s:[%d] Exit", __func__, p_ses->sm_handle);
    return status;
}

static int stop_pcm(sound_trigger_session_t* p_ses)
{
    int status = 0;

    ALOGV("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (p_ses->pcm) {
        status = pcm_stop(p_ses->pcm);
        if (status)
            ALOGE("%s: ERROR. pcm_stop failed, %s", __func__,
                  pcm_get_error(p_ses->pcm));
    }
    ALOGV("%s:[%d] Exit", __func__, p_ses->sm_handle);
    return status;
}

int ape_reg_sm(sound_trigger_session_t *p_ses)
{
    int status = 0;
    struct snd_lsm_sound_model_v2 lsm_sm_v2;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm = p_ses->sm_data;
    struct sound_trigger_sound_model *common_sm= p_ses->sm_data;
    struct snd_lsm_session_data ses_data;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    pthread_attr_t attr;
    int app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
    bool use_topology = false;


    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);

    p_ses->pcm_id = platform_ape_get_pcm_device_id(stdev->platform, p_ses);
    if(p_ses->pcm_id < 0)
        return -ENODEV;

    if(v_info) {
        app_type = v_info->app_type;
        use_topology = true;
    }

    status = platform_stdev_send_calibration(stdev->platform,
                                              AUDIO_DEVICE_NONE,
                                              p_ses,
                                              app_type, use_topology,
                                              ST_SESSION_CAL);

    ALOGD("%s: app_type=%d use_topo=%d cal status=%d",
          __func__, app_type, use_topology, status);

    /* Backward compatibility checks
       1. If no vendor present, use regular SVA non-topology solution.
       2. If vendor present is QC SVA and acdb doesn't have topology id,
        use regular QC non-topology solution.
       3. If vendor present is ISV and acdb doesn't have topology id,
          return error */
    if (use_topology && (status == -ENODATA) && v_info->is_qc_uuid) {
        /* In case of old acdb data files, calibration hadn't been
           sent to driver due to app_type we requested is for static
           topology. Resend calibration with non topology app_type */
        app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
        use_topology = false;
        status = platform_stdev_send_calibration(stdev->platform,
                                                  AUDIO_DEVICE_NONE,
                                                  p_ses,
                                                  app_type, use_topology,
                                                  ST_SESSION_CAL);
    }
    if (status) {
        ALOGE("%s: ERROR. sending calibration failed status %d topo %d",
              __func__, status, use_topology);
        goto sm_error;
    }
    p_ses->use_topology_intf = use_topology;

    p_ses->config = stdev_ape_pcm_config;
    ALOGD("%s: opening pcm device=%d topo %d", __func__, p_ses->pcm_id,
          p_ses->use_topology_intf);
    ALOGV("%s: config: channels=%d rate=%d, period_size=%d, period_cnt=%d, format=%d",
          __func__, p_ses->config.channels, p_ses->config.rate,
          p_ses->config.period_size, p_ses->config.period_count,
          p_ses->config.format);
    p_ses->pcm = pcm_open(SOUND_CARD, p_ses->pcm_id,
                          PCM_IN, &p_ses->config);
    if (!p_ses->pcm) {
        ALOGE("%s: ERROR. pcm_open failed", __func__);
        status = -ENODEV;
        goto sm_error;
    }
    if (!pcm_is_ready(p_ses->pcm)) {
        ALOGE("%s: ERROR. pcm_is_ready failed err=%s", __func__,
              pcm_get_error(p_ses->pcm));
        status = -ENODEV;
        goto sm_error;
    }

    /* Although app_id is valid only for non-topology solution. The driver
       opens DSP LSM session with this ioctl cmd */
    ses_data.app_id = LSM_VOICE_WAKEUP_APP_ID_V2;
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_SESSION_DATA, &ses_data);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_SET_SESSION_DATA failed status %d",
              __func__, status);
        goto sm_error;
    }

    if(p_ses->use_topology_intf) {
        param_info.param_type = LSM_REG_SND_MODEL;
        param_info.module_id = v_info->params[LOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[LOAD_SOUND_MODEL].param_id;
        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
            param_info.param_size = phrase_sm->common.data_size;
            param_info.param_data =
                (unsigned char*)phrase_sm + phrase_sm->common.data_offset;
        } else {
            param_info.param_size = common_sm->data_size;
            param_info.param_data =
                (unsigned char*)common_sm + common_sm->data_offset;
        }

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size =
            lsm_params.num_params * sizeof(struct lsm_params_info);

        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->reg status=%d",
                  __func__, status);
            goto sm_error;
        }
    } else {
        memset(&lsm_sm_v2, 0, sizeof(lsm_sm_v2));
        lsm_sm_v2.data = (unsigned char*)phrase_sm + phrase_sm->common.data_offset;
        lsm_sm_v2.data_size = phrase_sm->common.data_size;
        ALOGD("%s: data=%p, data_size=%d, conf_level=%p, num_conf_level=%d",
               __func__, lsm_sm_v2.data, lsm_sm_v2.data_size,
              lsm_sm_v2.confidence_level, lsm_sm_v2.num_confidence_levels);
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_REG_SND_MODEL_V2, &lsm_sm_v2);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_REG_SND_MODEL_V2 failed, status=%d",
                  __func__, status);
            goto sm_error;
        }
    }

    /* Callback thread to wait on event detection */
    p_ses->exit_callback_thread = false;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&p_ses->callback_thread, &attr,
                        callback_thread_loop, p_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return 0;

sm_error:
    platform_ape_free_pcm_device_id(stdev->platform, p_ses);
    if (p_ses->pcm) {
        pcm_close(p_ses->pcm);
        p_ses->pcm = NULL;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int ape_dereg_sm(sound_trigger_session_t *p_ses)
{
    int status = 0, buf_en;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);

    if (!p_ses->pcm) {
        ALOGV("%s: pcm NULL", __func__);
        return status;
    }

    /* Exit the callback thread waiting on event detection */
    p_ses->exit_callback_thread = true;
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_ABORT_EVENT);
    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_ABORT_EVENT failed, status=%d", __func__,
              status);

    pthread_mutex_unlock(&p_ses->lock);
    pthread_mutex_unlock(&stdev->lock);
    pthread_join(p_ses->callback_thread, (void **) NULL);
    pthread_mutex_lock(&stdev->lock);
    pthread_mutex_lock(&p_ses->lock);

    if (p_ses->capture_requested) {
        buf_en = 0;
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d", __func__, status);
    }

    if (p_ses->use_topology_intf) {
        /* Topology based */
        param_info.param_type = LSM_DEREG_SND_MODEL;
        param_info.module_id = v_info->params[UNLOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[UNLOAD_SOUND_MODEL].param_id;
        param_info.param_size = 0;

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size =
            lsm_params.num_params * sizeof(struct lsm_params_info);

        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->dereg status=%d",
                  __func__, status);
    } else {
        /* Non topology based */
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_DEREG_SND_MODEL);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_DEREG_SND_MODEL failed", __func__);
    }

    pcm_close(p_ses->pcm);
    p_ses->pcm = NULL;
    platform_ape_free_pcm_device_id(stdev->platform, p_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int ape_reg_sm_params(sound_trigger_session_t* p_ses)
{
    int status = 0, buf_en = 0, idx = 0;
    int retry_num = 0;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_detection_params params;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info[3];
    struct lsm_params_info *cfl_params;
    struct lsm_params_info *op_params;
    struct lsm_params_info *cus_params;
    struct snd_lsm_detect_mode det_mode;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    /* SVA doesn't support per keyword recogntion mode.
       Use the per soundmodel recognition mode */
    if(p_ses->recognition_mode & RECOGNITION_MODE_VOICE_TRIGGER){
        det_mode.mode= LSM_MODE_KEYWORD_ONLY_DETECTION;
        if (p_ses->recognition_mode & RECOGNITION_MODE_USER_IDENTIFICATION)
            det_mode.mode = LSM_MODE_USER_KEYWORD_DETECTION;
    } else {
        ALOGE("%s: Unknown recognition mode %d", __func__, p_ses->recognition_mode);
        goto exit;
    }


    if (p_ses->use_topology_intf) {
        lsm_params.num_params = 0;
        /* Topology based:
           if smlib is not present, pass only the opaque data as custom params and
           ignore sending all other params */
        if (v_info->smlib_handle) {
            det_mode.detect_failure = stdev->detect_failure;
            /* fill confidence level params */
            cfl_params = &param_info[idx++];
            cfl_params->param_type = LSM_MIN_CONFIDENCE_LEVELS;
            cfl_params->module_id = v_info->params[CONFIDENCE_LEVELS].module_id;
            cfl_params->param_id = v_info->params[CONFIDENCE_LEVELS].param_id;
            cfl_params->param_size = p_ses->num_conf_levels;
            cfl_params->param_data = p_ses->conf_levels;
            lsm_params.num_params++;

            /* fill operation mode params */
            op_params = &param_info[idx++];
            op_params->param_type = LSM_OPERATION_MODE;
            op_params->module_id = v_info->params[OPERATION_MODE].module_id;
            op_params->param_id = v_info->params[OPERATION_MODE].param_id;
            op_params->param_size = sizeof(det_mode);
            op_params->param_data = (unsigned char *)&det_mode;
            lsm_params.num_params++;

            {
                unsigned int i;
                ALOGV("%s: dm %d df %d ncl %d lab %d", __func__,
                      det_mode.mode, det_mode.detect_failure,
                      cfl_params->param_size, p_ses->capture_requested);
                for (i = 0; i < cfl_params->param_size; i++) {
                    ALOGV("%s: conf_levels[%d] = %d", __func__,
                                        i, cfl_params->param_data[i]);
                }
            }
        }

        if (p_ses->rc_config->data_size) {
            /* fill opaque data as custom params */
            cus_params = &param_info[idx++];
            cus_params->param_type = LSM_CUSTOM_PARAMS;
            cus_params->param_size = p_ses->rc_config->data_size;
            cus_params->param_data =
                (unsigned char *)p_ses->rc_config + p_ses->rc_config->data_offset;
            lsm_params.num_params++;
        }

        if (lsm_params.num_params) {
            lsm_params.params = (unsigned char*)param_info;
            lsm_params.data_size =
                lsm_params.num_params * sizeof(struct lsm_params_info);

            status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
            if (status) {
                ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->reg_params, status=%d",
                      __func__, status);
                goto exit;
            }
        }
    } else {
        /* Non topology based */
        params.conf_level = p_ses->conf_levels;
        params.num_confidence_levels = p_ses->num_conf_levels;
        params.detect_failure = stdev->detect_failure;
        params.detect_mode = det_mode.mode;

        {
            unsigned int i;
            ALOGV("%s: dm %d df %d ncl %d lab %d", __func__,
                  params.detect_mode, params.detect_failure,
                  params.num_confidence_levels, p_ses->capture_requested);
            for (i = 0; i < params.num_confidence_levels; i++) {
                ALOGV("%s: conf_levels[%d] = %d", __func__,
                                    i, params.conf_level[i]);
            }
        }

        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_PARAMS, &params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_PARAMS, status=%d", __func__, status);
            goto exit;
        }
    }

    buf_en = p_ses->capture_requested ? 1: 0;
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d", __func__, status);

exit:
    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int ape_start(sound_trigger_session_t* p_ses)
{
    int status = 0;
    pthread_attr_t attr;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_START);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_START failed, status=%d", __func__, status);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int ape_stop(sound_trigger_session_t* p_ses)
{
    int status = 0;

    ALOGI("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_STOP);
    if(status)
        ALOGE("%s: ERROR. SNDDRV_LSM_STOP failed, status=%d", __func__, status);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int ape_stop_buffering(sound_trigger_session_t* p_ses)
{
    int status = 0;

    ALOGD("%s:[%d] Enter pcm %p", __func__, p_ses->sm_handle, p_ses->pcm);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_STOP_LAB);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_STOP_BUFFERING failed status %d", __func__, status);
    } else {
        status = pcm_ioctl(p_ses->pcm, SNDRV_PCM_IOCTL_RESET);
        if(status)
            ALOGE("%s: ERROR. SNDRV_PCM_IOCTL_RESET failed status %d", __func__, status);
    }
    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int cpe_reg_sm(sound_trigger_session_t *p_ses)
{
    int status = 0, fd;
    int app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
    struct snd_lsm_sound_model_v2 lsm_sm_v2;
    struct sound_trigger_phrase_sound_model *phrase_sm = p_ses->sm_data;
    struct sound_trigger_sound_model *common_sm= p_ses->sm_data;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct pcm_config  *config;
    pthread_attr_t attr;
    bool use_topology = false;
    int sample_rate = SOUND_TRIGGER_SAMPLING_RATE;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);

    if (v_info) {
        app_type = v_info->app_type;
        sample_rate = v_info->sample_rate;
        use_topology = true;
    }

    p_ses->pcm_id = platform_cpe_get_pcm_device_id(stdev->platform, p_ses, sample_rate);
    if (p_ses->pcm_id < 0)
        return -ENODEV;

    status = platform_stdev_send_calibration(stdev->platform,
                                              AUDIO_DEVICE_NONE,
                                              p_ses,
                                              app_type, use_topology,
                                              ST_SESSION_CAL);

    ALOGD("%s: app_type=%d use_topo=%d cal status=%d",
          __func__, app_type, use_topology, status);

    /* Backward compatibility checks
       1. If no vendor present, use regular SVA non-topology solution.
       2. If vendor present is QC SVA and acdb doesn't have topology id,
        use regular QC non-topology solution.
       3. If vendor present is ISV and acdb doesn't have topology id,
          return error */
    if (use_topology && (status == -ENODATA) && v_info->is_qc_uuid) {
        /* In case of old acdb data files, calibration hadn't been
           sent to driver due to app_type we requested is for static
           topology. Resend calibration with non topology app_type */
        app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
        use_topology = false;
        status = platform_stdev_send_calibration(stdev->platform,
                                                  stdev->capture_device,
                                                  p_ses,
                                                  app_type, use_topology,
                                                  ST_SESSION_CAL);
    }

    if (status) {
        ALOGE("%s: ERROR. sending session calibration failed status %d",
               __func__, status);
        goto sm_error;
    }

    if (v_info && (strcmp(v_info->cpe_firmware_image,""))) {
        ALOGD("%s: cpe firmware image %s", __func__, v_info->cpe_firmware_image);
        /* Write the vendor firmware image name in sysfs entry */
        fd = open("/sys/kernel/wcd_cpe0/fw_name", O_WRONLY);
        if (fd >= 0) {
            write(fd, v_info->cpe_firmware_image, sizeof(v_info->cpe_firmware_image));
            close(fd);
        }
        else {
            ALOGE("%s: ERROR. sysfs open to write firmware name failed : %s",
                  __func__, strerror(errno));
            goto sm_error;
        }
    }
    p_ses->use_topology_intf = use_topology;
    p_ses->config = stdev_cpe_pcm_config;
    p_ses->lab_config = stdev_cpe_pcm_config;
    config = &p_ses->lab_config;

    if (v_info) {
        p_ses->lab_config.rate = v_info->sample_rate;
        if (v_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {

            platform_stdev_check_and_update_pcm_config(stdev->platform,
                                                       &p_ses->lab_config);

            p_ses->lab_config.period_size =
                CALCULATE_PERIOD_SIZE(SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS,
                                      p_ses->lab_config.rate,
                                      SOUND_TRIGGER_CPE_PERIOD_COUNT, 32);

            ALOGV("%s: ftrt rate=%d format=%d", __func__, p_ses->lab_config.rate,
                  p_ses->lab_config.format);

            if(CHECK_BIT(v_info->kw_capture_format, ADPCM))
                p_ses->config.period_size = ADPCM_MAX_OUT_SAMPLES_PER_FRAME;
        }
    }

    ALOGD("%s: opening pcm device=%d topo %d", __func__, p_ses->pcm_id,
          p_ses->use_topology_intf);
    ALOGD("%s: config: channels=%d rate=%d, period_size=%d, "
          "period_cnt=%d, format=%d", __func__, config->channels,
          config->rate, config->period_size,
          config->period_count, config->format);

    p_ses->pcm = pcm_open(SOUND_CARD, p_ses->pcm_id,
                          PCM_IN, config);

    if (!p_ses->pcm) {
        ALOGE("%s: ERROR. pcm_open failed", __func__);
        status = -ENODEV;
        goto sm_error;
    }
    if (!pcm_is_ready(p_ses->pcm)) {
        ALOGE("%s: ERROR. pcm_is_ready failed err=%s", __func__,
              pcm_get_error(p_ses->pcm));
        status = -ENODEV;
        goto sm_error;
    }

    if (p_ses->use_topology_intf) {
        param_info.param_type = LSM_REG_SND_MODEL;
        param_info.module_id = v_info->params[LOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[LOAD_SOUND_MODEL].param_id;

        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
            param_info.param_size = phrase_sm->common.data_size;
            param_info.param_data =
                (unsigned char*)phrase_sm + phrase_sm->common.data_offset;
        } else {
            param_info.param_size = common_sm->data_size;
            param_info.param_data =
                (unsigned char*)common_sm + common_sm->data_offset;
        }

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size = lsm_params.num_params * sizeof(struct lsm_params_info);

        ALOGV("%s: sm data size=%d", __func__, param_info.param_size);
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->reg status=%d",
                  __func__, status);
            goto sm_error;
        }
    } else {
        memset(&lsm_sm_v2, 0, sizeof(lsm_sm_v2));
        lsm_sm_v2.data = (unsigned char*)phrase_sm + phrase_sm->common.data_offset;
        lsm_sm_v2.data_size = phrase_sm->common.data_size;
        ALOGD("%s: data=%p, data_size=%d, conf_level=%p, num_conf_level=%d",
               __func__, lsm_sm_v2.data, lsm_sm_v2.data_size,
              lsm_sm_v2.confidence_level, lsm_sm_v2.num_confidence_levels);
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_REG_SND_MODEL_V2, &lsm_sm_v2);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_REG_SND_MODEL_V2 failed, status=%d",
                  __func__, status);
            goto sm_error;
        }
    }

    /* Callback thread to wait on event detection */
    p_ses->exit_callback_thread = false;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&p_ses->callback_thread, &attr,
                        callback_thread_loop, p_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return 0;

sm_error:
    platform_cpe_free_pcm_device_id(stdev->platform, p_ses);
    if (p_ses->pcm) {
        pcm_close(p_ses->pcm);
        p_ses->pcm = NULL;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int cpe_dereg_sm(sound_trigger_session_t *p_ses)
{
    int status = 0, buf_en;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);

    if (!p_ses->pcm) {
        ALOGV("%s: pcm NULL", __func__);
        return status;
    }

    /* Exit the callback thread waiting on event detection */
    p_ses->exit_callback_thread = true;
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_ABORT_EVENT);
    if (status)
        ALOGE("%s: SNDRV_LSM_ABORT_EVENT failed, status=%d", __func__, status);

    pthread_mutex_unlock(&p_ses->lock);
    pthread_mutex_unlock(&stdev->lock);
    pthread_join(p_ses->callback_thread, (void **) NULL);
    pthread_mutex_lock(&stdev->lock);
    pthread_mutex_lock(&p_ses->lock);

    if (p_ses->capture_requested) {
        buf_en = 0;
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d", __func__, status);

        if (p_ses->lab_buffers_allocated) {
            /* Deallocate lab buffes allocated during start_recognition */
            deallocate_lab_buffers(p_ses);
        }
    }

    if (p_ses->use_topology_intf) {
        param_info.param_type = LSM_DEREG_SND_MODEL;
        param_info.module_id = v_info->params[UNLOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[UNLOAD_SOUND_MODEL].param_id;
        param_info.param_size = 0;

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size =
            lsm_params.num_params * sizeof(struct lsm_params_info);

        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->dereg status=%d",
                  __func__, status);
    } else {
        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_DEREG_SND_MODEL);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_DEREG_SND_MODEL failed %d", __func__, status);
    }

    pcm_close(p_ses->pcm);
    p_ses->pcm = NULL;
    platform_cpe_free_pcm_device_id(stdev->platform, p_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int cpe_reg_sm_params(sound_trigger_session_t* p_ses)
{
    int status = 0, idx = 0;
    int retry_num = 0;
    unsigned char temp_conf_levels[1];
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_detection_params params;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info[3];
    struct lsm_params_info *cfl_params;
    struct lsm_params_info *op_params;
    struct lsm_params_info *cus_params;
    struct snd_lsm_detect_mode det_mode;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    memset(&det_mode, 0, sizeof(struct snd_lsm_detect_mode));
    if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        /* SVA doesn't support per keyword recogntion mode.
         * Use the per soundmodel recognition mode.
         */
        if(p_ses->recognition_mode & RECOGNITION_MODE_VOICE_TRIGGER){
            det_mode.mode= LSM_MODE_KEYWORD_ONLY_DETECTION;
            if (p_ses->recognition_mode & RECOGNITION_MODE_USER_IDENTIFICATION)
                det_mode.mode = LSM_MODE_USER_KEYWORD_DETECTION;
        } else {
            ALOGE("%s: Unknown recognition mode %d", __func__, p_ses->recognition_mode);
            goto err_exit_1;
        }
    }

    if (p_ses->use_topology_intf) {
        /* If this is generic sound model or smlib is not present, pass only the
         * opaque data as custom params and ignore sending all other params.
         */
        lsm_params.num_params = 0;
        if (v_info->smlib_handle &&
            (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)) {
            det_mode.detect_failure = stdev->detect_failure;

            /* fill confidence level params if present */
            if (p_ses->conf_levels) {
                cfl_params = &param_info[idx++];
                cfl_params->param_type = LSM_MIN_CONFIDENCE_LEVELS;
                cfl_params->module_id = v_info->params[CONFIDENCE_LEVELS].module_id;
                cfl_params->param_id = v_info->params[CONFIDENCE_LEVELS].param_id;
                cfl_params->param_size = p_ses->num_conf_levels;
                cfl_params->param_data = p_ses->conf_levels;
                lsm_params.num_params++;

                {
                    unsigned int i;
                    ALOGD("%s: dm %d df %d ncl %d lab %d", __func__,
                          det_mode.mode, det_mode.detect_failure,
                          cfl_params->param_size, p_ses->capture_requested);
                    for (i = 0; i < cfl_params->param_size; i++) {
                        ALOGV("%s: conf_levels[%d] = %d", __func__,
                                            i, cfl_params->param_data[i]);
                    }
                }
            }

            /* fill operation mode params */
            op_params = &param_info[idx++];
            op_params->param_type = LSM_OPERATION_MODE;
            op_params->module_id = v_info->params[OPERATION_MODE].module_id;
            op_params->param_id = v_info->params[OPERATION_MODE].param_id;
            op_params->param_size = sizeof(det_mode);
            op_params->param_data = (unsigned char *)&det_mode;
            lsm_params.num_params++;
        }

        if (p_ses->rc_config->data_size) {
            /* fill opaque data as custom params */
            cus_params = &param_info[idx++];
            cus_params->param_type = LSM_CUSTOM_PARAMS;
            cus_params->param_size = p_ses->rc_config->data_size;
            cus_params->param_data =
                (unsigned char *)p_ses->rc_config + p_ses->rc_config->data_offset;
            lsm_params.num_params++;
        }

        if (lsm_params.num_params) {
            lsm_params.params = (unsigned char*)param_info;
            lsm_params.data_size =
                lsm_params.num_params * sizeof(struct lsm_params_info);

            status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
            if (status) {
                ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->reg_params %d, status=%d",
                      __func__, lsm_params.num_params, status);
                goto err_exit_1;
            }
        }
    } else {
        params.conf_level = (p_ses->conf_levels != NULL) ?
                                    p_ses->conf_levels : temp_conf_levels;
        params.num_confidence_levels = (p_ses->conf_levels != NULL) ?
                                    p_ses->num_conf_levels : 1;
        params.detect_failure = stdev->detect_failure;
        params.detect_mode = det_mode.mode;

        {
            unsigned int i;
            ALOGD("%s: dm %d df %d ncl %d lab %d", __func__,
                  params.detect_mode, params.detect_failure,
                  params.num_confidence_levels, p_ses->capture_requested);
            for (i = 0; i < params.num_confidence_levels; i++) {
                ALOGV("%s: conf_levels[%d] = %d", __func__,
                                    i, params.conf_level[i]);
            }
        }

        status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_SET_PARAMS, &params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_PARAMS, status=%d", __func__, status);
            goto err_exit_1;
        }
    }

    if (p_ses->capture_requested) {
        if (!p_ses->lab_buffers_allocated) {
            /* Instead of allocating and deallocating during frequent
               start/stop_recogntion, allocate once and dellocate during
               unload_sound_model */
            status = allocate_lab_buffers(p_ses);
            if (status)
                goto err_exit_1;
        }
        status = configure_lab(p_ses);
        if (status)
            goto err_exit_1;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return 0;

err_exit_1:
    if (p_ses->lab_buffers_allocated)
        deallocate_lab_buffers(p_ses);

    pcm_stop(p_ses->pcm);

    return status;
}

int cpe_start(sound_trigger_session_t* p_ses)
{
    int status = 0;
    pthread_attr_t attr;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_START);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_START failed, status=%d", __func__, status);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int cpe_stop(sound_trigger_session_t* p_ses)
{
    int status = 0;

    ALOGI("%s:[%d] Enter", __func__, p_ses->sm_handle);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_STOP);
    if(status)
        ALOGE("%s: ERROR. SNDDRV_LSM_STOP failed, status=%d", __func__, status);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

int cpe_stop_buffering(sound_trigger_session_t* p_ses)
{
    int status = 0;

    ALOGD("%s:[%d] Enter pcm %p", __func__, p_ses->sm_handle, p_ses->pcm);
    if (!p_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_STOP_LAB);
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_STOP_BUFFERING failed status %d", __func__, status);
    } else {
        status = pcm_ioctl(p_ses->pcm, SNDRV_PCM_IOCTL_RESET);
        if(status)
            ALOGE("%s: ERROR. SNDRV_PCM_IOCTL_RESET failed status %d", __func__, status);
    }
    if (p_ses->capture_requested) {
        pthread_mutex_lock(&p_ses->pcm_out_buf_lock);
        pthread_cond_signal(&p_ses->pcm_out_buf_cond);
        pthread_mutex_unlock(&p_ses->pcm_out_buf_lock);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

static int route_reg_sm_ape(sound_trigger_session_t *p_ses)
{
    return ape_reg_sm(p_ses);
}

static int route_reg_sm_params_ape(sound_trigger_session_t* p_ses)
{
    return ape_reg_sm_params(p_ses);
}

static int route_dereg_sm_ape(sound_trigger_session_t* p_ses)
{
    int status = 0;

    status = ape_dereg_sm(p_ses);

    return status;
}

static int route_prepare_ape(sound_trigger_session_t* p_ses)
{
    ape_enable_use_case(true, p_ses);
    lsm_set_port(p_ses);
    return start_pcm(p_ses);
}

static int route_unprepare_ape(sound_trigger_session_t* p_ses)
{
    int status = 0;

    status = stop_pcm(p_ses);
    ape_enable_use_case(false, p_ses);

    return status;
}

static int route_start_ape(sound_trigger_session_t* p_ses)
{
    reg_hal_event_session(p_ses);
    return ape_start(p_ses);
}

static int route_stop_ape(sound_trigger_session_t* p_ses)
{
    dereg_hal_event_session(p_ses);
    return ape_stop(p_ses);
}

static int route_stop_buffering_ape(sound_trigger_session_t* p_ses)
{
    int status = 0;

    if (!CHECK_STATE(p_ses->state, SES_BUFFERING)) {
        return status;
    }
    status = ape_stop_buffering(p_ses);

    CLEAR_STATE(p_ses->state, SES_BUFFERING);
    return status;
}

static int route_set_device_ape(sound_trigger_session_t *p_ses,
                                bool enable, audio_devices_t device)
{
    return sound_trigger_set_device(p_ses, enable, device);
}

static int route_read_pcm_ape(sound_trigger_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes)
{
    int status = 0;

    pthread_mutex_lock(&p_ses->lock);
    if (!CHECK_STATE(p_ses->state, SES_BUFFERING)) {
        ALOGE("%s: No active buffering", __func__);
        pthread_mutex_unlock(&p_ses->lock);
        return -EIO;
    }

    /* Read PCM frames directly from driver */
    status = pcm_read(p_ses->pcm, buf, bytes);
    if (status < 0)
        status = -errno;

    pthread_mutex_unlock(&p_ses->lock);
    return status;
}

static void route_lab_capture_ape(sound_trigger_session_t *p_ses __unused)
{
    /* No internal buffering supported for ADSP */
    return;
}

static int cpe_stop_other_sessions(sound_trigger_session_t *cur_ses)
{
    sound_trigger_session_t *p_ses;
    struct listnode *p_ses_node;

    ALOGV("%s: list empty %s", __func__,
          list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;

        pthread_mutex_lock(&p_ses->lock);
        if (CHECK_STATE(p_ses->state, SES_STARTED)) {
            p_ses->fptrs->stop_buffering(p_ses);
            p_ses->fptrs->stop(p_ses);
        }
        pthread_mutex_unlock(&p_ses->lock);
    }
    return 0;
}

static int cpe_restart_other_sessions(sound_trigger_session_t *cur_ses)
{
    sound_trigger_session_t *p_ses;
    struct listnode *p_ses_node;

    ALOGV("%s: list empty %s", __func__,
          list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;

        pthread_mutex_lock(&p_ses->lock);
        if (CHECK_STATE(p_ses->state, SES_STARTED))
            p_ses->fptrs->start(p_ses);
        pthread_mutex_unlock(&p_ses->lock);
    }
    return 0;
}

static int route_reg_sm_cpe(sound_trigger_session_t *p_ses)
{
    int status = 0;

    /* CPE DRAM can only be accessed by single client, i.e. Apps or CPE,
     * but not both in parallel. Sending sound model here can access DRAM.
     * If another session is already active, stop it and restart after
     * sending this session sound model.
     */
    cpe_stop_other_sessions(p_ses);
    status = cpe_reg_sm(p_ses);
    cpe_restart_other_sessions(p_ses);

    return status;
}

static int route_reg_sm_params_cpe(sound_trigger_session_t* p_ses)
{
    int status = 0;

    /* CPE DRAM can only be accessed by single client, i.e. Apps or CPE,
     * but not both in parallel. Sending params here can access DRAM.
     * If another session is already active, stop it and restart after
     * sending this session params.
     */
    cpe_stop_other_sessions(p_ses);
    status = cpe_reg_sm_params(p_ses);
    cpe_restart_other_sessions(p_ses);

    return status;
}

static int route_dereg_sm_cpe(sound_trigger_session_t* p_ses)
{
    return cpe_dereg_sm(p_ses);
}

static int route_start_cpe(sound_trigger_session_t* p_ses)
{
    reg_hal_event_session(p_ses);
    return cpe_start(p_ses);
}

static int route_stop_cpe(sound_trigger_session_t* p_ses)
{
    dereg_hal_event_session(p_ses);
    return cpe_stop(p_ses);
}

static int route_prepare_cpe(sound_trigger_session_t* p_ses)
{
    lsm_set_port(p_ses);
    return start_pcm(p_ses);
}

static int route_unprepare_cpe(sound_trigger_session_t* p_ses)
{
    return stop_pcm(p_ses);
}


static int route_stop_buffering_cpe(sound_trigger_session_t* p_ses)
{
    int status = 0;

    if (!CHECK_STATE(p_ses->state, SES_BUFFERING)) {
        return status;
    }
    status = cpe_stop_buffering(p_ses);

    CLEAR_STATE(p_ses->state, SES_BUFFERING);
    return status;
}

static int route_set_device_cpe(sound_trigger_session_t *p_ses,
                                bool enable, audio_devices_t device)
{
    return sound_trigger_set_device(p_ses, enable, device);
}

static int route_read_pcm_cpe(sound_trigger_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes)
{
    int status = 0;

    if (p_ses->exit_lab_processing) {
        ALOGE("%s: No active buffering", __func__);
        return -EIO;
    }
    status = read_pcm_data(p_ses, buf, bytes);

    return status;
}

static void route_lab_capture_cpe(sound_trigger_session_t *p_ses)
{

    if (p_ses->vendor_uuid_info &&
        p_ses->vendor_uuid_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {
        process_packetized_lab_data(p_ses);
    } else {
        process_raw_lab_data(p_ses);
    }
}

struct sound_trigger_session* get_sound_trigger_session(
                    struct sound_trigger_device *stdev,
                    sound_model_handle_t sound_model_handle)
{
    struct sound_trigger_session *st_session = NULL;
    struct listnode *node;
    list_for_each(node, &stdev->sound_model_list) {
        st_session = node_to_item(node, struct sound_trigger_session, list_node);
        if (st_session->sm_handle == sound_model_handle)
            return st_session;
    }
    return NULL;
}

static void update_available_phrase_info
(
   struct sound_trigger_session *p_ses,
   struct sound_trigger_phrase_sound_model *phrase_sm,
   bool add
)
{
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;

    if (add) {
        if (p_ses->exec_mode == ST_DEVICE_EXEC_MODE_CPE) {
            if (v_info) {
                v_info->avail_cpe_phrases  += phrase_sm->num_phrases;
                v_info->avail_cpe_users  += p_ses->num_users;
            } else {
                stdev->avail_cpe_phrases  += phrase_sm->num_phrases;
                stdev->avail_cpe_users  += p_ses->num_users;
            }
        } else {
            if (v_info) {
                v_info->avail_ape_phrases  += phrase_sm->num_phrases;
                v_info->avail_ape_users  += p_ses->num_users;
            } else {
                stdev->avail_ape_phrases  += phrase_sm->num_phrases;
                stdev->avail_ape_users  += p_ses->num_users;
            }
        }
    } else {
        if (p_ses->exec_mode == ST_DEVICE_EXEC_MODE_CPE) {
            if (v_info) {
                v_info->avail_cpe_phrases  -= phrase_sm->num_phrases;
                v_info->avail_cpe_users  -= p_ses->num_users;
            } else {
                stdev->avail_cpe_phrases  -= phrase_sm->num_phrases;
                stdev->avail_cpe_users  -= p_ses->num_users;
            }
        } else {
            if (v_info) {
                v_info->avail_ape_phrases  -= phrase_sm->num_phrases;
                v_info->avail_ape_users  -= p_ses->num_users;
            } else {
                stdev->avail_ape_phrases  -= phrase_sm->num_phrases;
                stdev->avail_ape_users  -= p_ses->num_users;
            }
        }
    }
}

static int get_confidence_levels_payload
(
   sound_trigger_device_t *stdev,
   sound_trigger_session_t *p_ses,
   const struct sound_trigger_recognition_config *config
)
{
    int status = 0;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm = p_ses->sm_data;

    if (v_info) {
        /* Can be QC SVA or ISV vendor. Get from corresponding smlib defined
           in platform file. if soundmodel library for ISV vendor uuid is
           mentioned, use it. If not ignore and continue sending the opaque
           data from HAL to DSP */
        if (v_info->smlib_handle) {
            status = v_info->generate_st_recognition_config_payload(
                                            phrase_sm, config,
                                            &p_ses->conf_levels,
                                            &p_ses->num_conf_levels);

            if (status || !p_ses->conf_levels) {
                ALOGE("%s: smlib get confidence level failed, status %d",
                      __func__, status);
                if (p_ses->conf_levels) {
                    free(p_ses->conf_levels);
                    p_ses->conf_levels = NULL;
                }
            }
        } else {
            ALOGD("%s: No smlib for uuid, sending opaque data", __func__);
        }
    } else {
        /* non topology solution: get from default SVA smlib */
        if (stdev->smlib_handle) {
            status = stdev->generate_st_recognition_config_payload(
                                            phrase_sm, config,
                                            &p_ses->conf_levels,
                                            &p_ses->num_conf_levels);

            if (status || !p_ses->conf_levels) {
                ALOGE("%s: Smlib get confidence level failed, status %d",
                      __func__, status);
                if (p_ses->conf_levels) {
                    free(p_ses->conf_levels);
                    p_ses->conf_levels = NULL;
                }
            }
        }
    }
    return status;
}

static int start_recognition_l
(
   struct sound_trigger_session *p_ses,
   const struct sound_trigger_recognition_config *config,
   recognition_callback_t callback,
   void *cookie
)
{
    int status = 0;
    bool concurrency_active, config_changed;

    if (!p_ses->pcm || !p_ses->fptrs) {
        ALOGE("%s: ERROR. null pcm, fptrs", __func__);
        return -EINVAL;
    }

    concurrency_active = !platform_stdev_is_session_allowed(stdev->platform,
                               get_num_sessions_in_state(SES_CREATED), true);

    /* If this is for subsequent detections of an existing session,
     * check and push if any config params changed and start the detections.
     */
    if (p_ses->callback) {
        /* It is possible that while buffering in progress after detection,
         * App can start next recognition without explicitely stopping buffering
         * thru AudioRecord in erroneous cases. Stop here accordingly.
         */
        status = p_ses->fptrs->stop_buffering(p_ses);
        if (status)
            goto exit;

        config_changed = !!memcmp(p_ses->rc_config, config,
                                  sizeof(*(p_ses->rc_config)) +
                                  p_ses->rc_config->data_size);

        if (!config_changed) {
            if (!concurrency_active)
                status = start_recognition_internal(p_ses);
            else
                ALOGD("%s: audio concurrency active, start deferred",
                      __func__);
            goto exit;
        }
        /* Config changed. If we stopped buffering implicitely, stop the
         * remaining recognition part.
         */
        status = stop_recognition_internal(p_ses, false);
        if (status)
            goto exit;
    }

    if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        ALOGVV("%s: num_phrases=%d, id=%d", __func__,
               config->num_phrases, config->phrases[0].id);
        status = get_confidence_levels_payload(stdev, p_ses, config);
        if (status)
            goto exit;
    }

    /* Store the recogntion configuration for sending opaque data
     * and for SSR, passing to sounmodel wrapper library.
     */
    p_ses->rc_config = calloc(1, sizeof(*config) + config->data_size);
    if (!p_ses->rc_config) {
        ALOGE("%s: No memory for rc_config", __func__);
        status = -ENOMEM;
        goto exit;
    }

    memcpy(p_ses->rc_config, (char *)config, sizeof(*config));
    memcpy((char *)p_ses->rc_config + p_ses->rc_config->data_offset,
           (char *)config + config->data_offset, config->data_size);

    p_ses->callback = callback;
    p_ses->cookie = cookie;
    /* capture_handle will be shared with AHAL for reading LAB data */
    p_ses->capture_handle = config->capture_handle;
    p_ses->capture_requested = config->capture_requested;
    stdev->capture_device = config->capture_device;
    p_ses->send_sm_params = true;

    if (!concurrency_active)
        status = start_recognition_internal(p_ses);
    else
        ALOGD("%s: active audio concurrency, start deferred", __func__);

exit:
    if (!status) {
        p_ses->client_started = true;
        p_ses->exit_lab_processing = false;
    } else {
        p_ses->callback = NULL;
        if (p_ses->conf_levels) {
            free(p_ses->conf_levels);
            p_ses->conf_levels = NULL;
        }
        if (p_ses->rc_config) {
            free(p_ses->rc_config);
            p_ses->rc_config = NULL;
        }
    }
    return status;
}

static int start_recognition_internal(struct sound_trigger_session *p_ses)
{
    int status = 0;

    if (CHECK_STATE(p_ses->state, SES_STARTED))
        return status;

    status = p_ses->fptrs->set_device(p_ses, true, stdev->capture_device);
    if (status)
        return status;

    status = p_ses->fptrs->prepare(p_ses);
    if (status)
        goto prepare_failed;

    if (p_ses->send_sm_params) {
        /* Caller decides to send the sound model config params or not
           everytime, depending on the concurrent context */
        p_ses->send_sm_params = false;
        status = p_ses->fptrs->reg_sm_params(p_ses);
        if (status)
            goto reg_sm_params_failed;
    }

    status = p_ses->fptrs->start(p_ses);
    if (status)
        goto start_failed;

    SET_STATE(p_ses->state, SES_STARTED);
    return status;

start_failed:
reg_sm_params_failed:
    p_ses->fptrs->unprepare(p_ses);

prepare_failed:
    p_ses->fptrs->set_device(p_ses, false, stdev->capture_device);

    return status;
}

static int stop_recognition_internal(struct sound_trigger_session *p_ses,
                                     bool lock)
{
    int status = 0, rc = 0;
    /* Can be called from callback_thread/AHAL/App contexts */

    if (!CHECK_STATE(p_ses->state, SES_STARTED))
        return status;

    CLEAR_STATE(p_ses->state, SES_STARTED);
    status = p_ses->fptrs->stop_buffering(p_ses);
    if (status)
        rc = status;

    status = p_ses->fptrs->stop(p_ses);
    if (status)
        rc = status;

    status = p_ses->fptrs->unprepare(p_ses);
    if (status)
        rc = status;

    if (lock) {
        /* Called in event callback without dev lock held.
           Maintain the lock sequence of dev_lock followed by ses_lock */
        pthread_mutex_unlock(&p_ses->lock);
        pthread_mutex_lock(&stdev->lock);
    }

    status = p_ses->fptrs->set_device(p_ses, false, stdev->capture_device);
    if (status)
        rc = status;

    if (lock) {
        pthread_mutex_unlock(&stdev->lock);
        pthread_mutex_lock(&p_ses->lock);
    }

    return rc;
}

static int stop_recognition_l(struct sound_trigger_session *st_session)
{
    int status = 0;

    st_session->callback = NULL;
    st_session->client_started = false;
    status = stop_recognition_internal(st_session, false);

    if (st_session->conf_levels) {
        free(st_session->conf_levels);
        st_session->conf_levels = NULL;
    }
    if (st_session->rc_config) {
        free(st_session->rc_config);
        st_session->rc_config = NULL;
    }
    return status;
}

static int stdev_get_properties(const struct sound_trigger_hw_device *dev,
                                struct sound_trigger_properties *properties)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;

    ALOGI("%s", __func__);
    if (properties == NULL) {
        ALOGE("%s: NULL properties", __func__);
        return -EINVAL;
    }

    if (stdev->run_on_ape) {
        hw_properties.max_sound_models = stdev->max_ape_sessions;
        hw_properties.max_key_phrases = stdev->avail_ape_phrases;
        hw_properties.max_users = stdev->avail_ape_users;
        hw_properties.max_buffer_ms = SOUND_TRIGGER_APE_BUFFER_DURATION_MS;
    }
    else {
        hw_properties.max_sound_models = stdev->max_cpe_sessions;
        hw_properties.max_key_phrases = stdev->avail_cpe_phrases ;
        hw_properties.max_users = stdev->avail_cpe_users ;
        hw_properties.max_buffer_ms = SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS;
    }
    ALOGVV("%s recognition_modes=%d, capture_transition=%d", __func__,
          hw_properties.recognition_modes, hw_properties.capture_transition);
    memcpy(properties, &hw_properties, sizeof(struct sound_trigger_properties));
    return 0;
}

static st_exec_mode_t get_session_exec_mode
(
   sound_trigger_session_t *p_ses,
   struct sound_trigger_sound_model *common_sm
)
{
    unsigned int num_sessions = 0;
    unsigned int num_users = 0;
    unsigned int num_phrases = 0;
    unsigned int i, j;
    st_exec_mode_t exec_mode = ST_DEVICE_EXEC_MODE_NONE;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm = NULL;

    num_sessions = get_num_sessions_in_state(SES_CREATED);

    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        phrase_sm = (struct sound_trigger_phrase_sound_model *)common_sm;
        num_phrases = phrase_sm->num_phrases;
        for (i = 0; i < phrase_sm->num_phrases; i++) {
            for (j = 0; j < phrase_sm->phrases[i].num_users; j++) {
                num_users++;
            }
        }
        p_ses->num_users = num_users;

        ALOGD("%s: mode=%d NS=%d RKW=%u RU=%u VACKW=%u VACU=%u VADKW=%d, VADU=%d",
              __func__, stdev->run_on_ape, num_sessions, num_phrases, num_users,
              v_info ? v_info->avail_cpe_phrases : stdev->avail_cpe_phrases,
              v_info ? v_info->avail_cpe_users : stdev->avail_cpe_users,
              v_info ? v_info->avail_ape_phrases : stdev->avail_ape_phrases,
              v_info ? v_info->avail_ape_users : stdev->avail_ape_users);
    }

    if (!stdev->run_on_ape) {
        /* Platform configured for CPE */
        if (v_info && (num_sessions < stdev->max_cpe_sessions)) {
            if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                /* Keyphrase sound model */
                if ((num_phrases <= v_info->avail_cpe_phrases) &&
                    (num_users <= v_info->avail_cpe_users)) {
                    exec_mode = ST_DEVICE_EXEC_MODE_CPE;
                }
            } else {
                /* Generic sound model without keyphrases */
                exec_mode = ST_DEVICE_EXEC_MODE_CPE;
            }
        } else if ((num_sessions < stdev->max_cpe_sessions) &&
            (num_phrases <= stdev->avail_cpe_phrases) &&
            (num_users <= stdev->avail_cpe_users)) {
            exec_mode = ST_DEVICE_EXEC_MODE_CPE;
        }
    } else {
        if (v_info && (num_sessions < stdev->max_ape_sessions)) {
            if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                /* Keyphrase sound model */
                if ((num_phrases <= v_info->avail_ape_phrases) &&
                    (num_users <= v_info->avail_ape_users)) {
                    exec_mode = ST_DEVICE_EXEC_MODE_APE;
                }
            } else {
                /* Generic sound model without keyphrases */
                exec_mode = ST_DEVICE_EXEC_MODE_APE;
            }
        } else if ((num_sessions < stdev->max_ape_sessions) &&
            (num_phrases <= stdev->avail_ape_phrases ) &&
            (num_users <= stdev->avail_ape_users )) {
            exec_mode = ST_DEVICE_EXEC_MODE_APE;
        }
    }

    ALOGV("%s: Exit  exec_mode=%d", __func__, exec_mode);
    return exec_mode;
}

static int stdev_load_sound_model(const struct sound_trigger_hw_device *dev,
                                  struct sound_trigger_sound_model *sound_model,
                                  sound_model_callback_t callback __unused,
                                  void *cookie __unused,
                                  sound_model_handle_t *handle)
{
    int status = 0;
    unsigned int i = 0, j = 0;
    st_exec_mode_t exec_mode;
    struct sound_trigger_session *st_session = NULL;
    struct sound_trigger_phrase_sound_model *phrase_sm = NULL;
    struct sound_trigger_sound_model *common_sm = NULL;
    unsigned int sm_size = 0;

    ALOGD("%s", __func__);

    pthread_mutex_lock(&stdev->lock);
    /* Wait on SSR if
     * 1. SSR_CPE flag is set and platform is configured for CPE.
     * OR
     * 2. SSR_APE flag is set.
     *
     * SSR handler is suppossed to add only those flags which affects
     * configured platform, but when both APE and CPE are applicable
     * i.e. transitions enabled(run_on_ape=true), no need to wait on CPE.
     */
    if (is_ssr_active()) {
        ALOGV("%s: waiting on SSR completion", __func__);
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }

    if (handle == NULL || sound_model == NULL) {
        status = -EINVAL;
        goto exit;
    }
    if (!platform_stdev_is_session_allowed(stdev->platform,
                         get_num_sessions_in_state(SES_CREATED), false)) {
        ALOGW("%s: session prevented by concurrency", __func__);
        status = -ENODEV;
        goto exit;
    }

    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        phrase_sm = (struct sound_trigger_phrase_sound_model*)sound_model;
        if ((phrase_sm->common.data_size == 0) ||
            (phrase_sm->common.data_offset < sizeof(*phrase_sm)) ||
            (phrase_sm->common.type != SOUND_MODEL_TYPE_KEYPHRASE) ||
            (phrase_sm->num_phrases == 0)) {
            ALOGE("%s: Invalid sound model params data size=%d, data offset=%d, "
                  "type=%d phrases=%d", __func__, phrase_sm->common.data_size,
                  phrase_sm->common.data_offset, phrase_sm->common.type,
                  phrase_sm->num_phrases);
            status = -EINVAL;
            goto exit;
        }
        common_sm = &phrase_sm->common;
        sm_size = sizeof(*phrase_sm) + phrase_sm->common.data_size;
    } else if (sound_model->type == SOUND_MODEL_TYPE_GENERIC) {
        if (!sound_model->data_size ||
            (sound_model->data_offset < sizeof(*common_sm))) {
            ALOGE("%s: Invalid Generic sound model params data size=%d, data offset=%d",
                  __func__, sound_model->data_size, sound_model->data_offset);
            status = -EINVAL;
            goto exit;
        }
        common_sm = sound_model;
        sm_size = sizeof(*common_sm) + common_sm->data_size;
    } else {
        ALOGE("%s: Unknown sound model type %d", __func__, sound_model->type);
        status = -EINVAL;
        goto exit;
    }

    ALOGV("%s: sm_size=%d data_size=%d", __func__, sm_size, common_sm->data_size);
    st_session = calloc(1, sizeof(struct sound_trigger_session));
    if (!st_session) {
        status = -ENOMEM;
        goto exit;
    }

    /* CPE takes time to become online, so parse for the pcm devices
       here instead during boot time */
    if (!CHECK_BIT(stdev->hw_type, ST_DEVICE_HW_APE|ST_DEVICE_HW_CPE)) {
        status = platform_stdev_get_hw_type(stdev->platform);
        if (status)
            goto exit;
    }

    /* Use cases:
       1. No vendor uuids defined in platform file                                                    .
            - Use default non topology based SVA solution.
       2. vendor uuids defined in platform file and vendor uuid doesn't match                         .
            - Return error.
       3. vendor uuids defined in platform file and vendor uuid matches                               .
            - Use topology based solution.
     */

    /* TODO: Google search app doesn't pass the vendor_uuid yet.
     * Update vendor_uuid accordingly to maintain backward compatibility with
     * old platform file hosting vendor uuid. With multisession support App
     * has to pass the sound model uuid matching the uuid under updated platform
     * file
     */
    sound_trigger_uuid_t temp_uuid;
    memset(&temp_uuid, 0, sizeof(temp_uuid));
    if (!memcmp(&common_sm->vendor_uuid, &temp_uuid, sizeof(sound_trigger_uuid_t))) {
        ALOGV("%s: no valid vendor_uuid present, using SVA uuid", __func__);
        common_sm->vendor_uuid = hw_properties.uuid;
    }

    bool is_qc_uuid = false;
    st_session->vendor_uuid_info = platform_stdev_get_vendor_info(
                                       stdev->platform,
                                       &common_sm->vendor_uuid,
                                       &is_qc_uuid);

    if (!st_session->vendor_uuid_info && !is_qc_uuid) {
        ALOGE("%s: no matching vendor uuid found", __func__);
        status = -EINVAL;
        goto exit;
    }

    exec_mode = get_session_exec_mode(st_session, common_sm);
    if (exec_mode == ST_DEVICE_EXEC_MODE_NONE) {
        status = -EINVAL;
        goto exit;
    }

    st_session->device = (void *)dev;
    st_session->exec_mode = exec_mode;

    if (ST_DEVICE_EXEC_MODE_CPE == exec_mode) {
        ALOGD("%s: CPE mode", __func__);
        st_session->fptrs = &cpe_fptrs;
    } else {
        ALOGD("%s: APE mode", __func__);
        st_session->fptrs = &ape_fptrs;
    }
    transit_reassign_fptrs(st_session);

    /* Store the sound model information for handling SSR
       and interaction with smlib */
    st_session->sm_data = calloc(1, sm_size);
    if (!st_session->sm_data) {
        status = -ENOMEM;
        goto exit;
    }

    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        memcpy(st_session->sm_data, (char *)phrase_sm, sizeof(*phrase_sm));
        memcpy((char *)st_session->sm_data + phrase_sm->common.data_offset,
               (char *)phrase_sm + phrase_sm->common.data_offset,
               phrase_sm->common.data_size);
        /* TODO: SVA doesn't support per keyword recognition mode.
         * So use the first phrase recognition mode. App is supposed
         * to set the proper recognition mode in the first phrase
         */
        st_session->recognition_mode = phrase_sm->phrases[0].recognition_mode;
        ALOGD("%s: sm magic number 0x%x rm %d", __func__,
              ((int *)((char *)st_session->sm_data + phrase_sm->common.data_offset))[0],
              phrase_sm->phrases[0].recognition_mode);

    } else {
        memcpy(st_session->sm_data, (char *)common_sm, sizeof(*common_sm));
        memcpy((char *)st_session->sm_data + common_sm->data_offset,
               (char *)common_sm + common_sm->data_offset,
               common_sm->data_size);
        ALOGD("%s: sm magic number 0x%x", __func__,
              ((int *)((char *)st_session->sm_data + common_sm->data_offset))[0]);
    }

    pthread_mutex_init(&st_session->lock, (const pthread_mutexattr_t *) NULL);
    st_session->sm_type = sound_model->type;
    st_session->sm_handle = android_atomic_inc(&stdev->session_id);
    *handle = st_session->sm_handle;

    /* No need for session lock as session is not pushed to list and
       the ssr/concurrencies are guarded with dev lock */
    status = st_session->fptrs->reg_sm(st_session);
    if (status)
        goto exit;

    /* Keyphrase, user information is valid only for keyphrase sound models */
    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE)
        update_available_phrase_info(st_session, phrase_sm, false);

    /* Add the session to the list of registered sound models list */
    list_add_tail(&stdev->sound_model_list, &st_session->list_node);
    SET_STATE(st_session->state, SES_CREATED);

    pthread_mutex_unlock(&stdev->lock);

    ALOGD("%s: success, sound_model_handle %d", __func__, st_session->sm_handle);
    return 0;

exit:
    if (st_session != NULL) {
        if (st_session->sm_data) {
            free(st_session->sm_data);
            pthread_mutex_destroy(&st_session->lock);
        }
        free(st_session);
        *handle = -1;
    }
    pthread_mutex_unlock(&stdev->lock);
    return status;
}

static int stdev_unload_sound_model(const struct sound_trigger_hw_device *dev,
                                    sound_model_handle_t handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct sound_trigger_session *st_session = NULL;
    int status = 0;

    ALOGD("%s: Enter sound model %d", __func__, handle);

    pthread_mutex_lock(&stdev->lock);
    if (is_ssr_active()) {
        ALOGV("%s: waiting on SSR completion", __func__);
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }

    st_session = get_sound_trigger_session(stdev, handle);
    if (!st_session || !st_session->fptrs) {
        ALOGE("%s Could not find sound model %d", __func__, handle);
        status = -EINVAL;
        goto exit;
    }

    /* If ftrt processing loop in progress, indicate to exit
       before acquiring the lock here */
    check_and_exit_lab(st_session);
    pthread_mutex_lock(&st_session->lock);
    if (st_session->callback)
        status = stop_recognition_l(st_session);

    SET_STATE(st_session->state, SES_INVALIDATED);
    status = st_session->fptrs->dereg_sm(st_session);

    list_remove(&st_session->list_node);

    if (st_session->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
        update_available_phrase_info(st_session, st_session->sm_data, true);

    pthread_mutex_unlock(&st_session->lock);

    if(!get_num_sessions_in_state(SES_CREATED))
        stdev->exec_mode = ST_DEVICE_EXEC_MODE_NONE;

    if (transit_ses_loading(st_session))
        goto exit;

    pthread_mutex_destroy(&st_session->lock);
    free(st_session->sm_data);
    free(st_session);

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s: Exit status %d", __func__, status);
    return status;
}

static int stdev_start_recognition
(
   const struct sound_trigger_hw_device *dev __unused,
   sound_model_handle_t sound_model_handle,
   const struct sound_trigger_recognition_config *config,
   recognition_callback_t callback,
   void *cookie
)
{
    struct sound_trigger_session *st_session = NULL;
    int status = 0;

    ALOGD("%s:[%d] Enter", __func__, sound_model_handle);

    pthread_mutex_lock(&stdev->lock);
    if (is_ssr_active()) {
        ALOGV("%s: waiting on SSR completion", __func__);
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }

    if (!callback || !config) {
        ALOGE("%s: ERROR. NULL params", __func__);
        status = -EINVAL;
        goto exit;
    }

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        status = -EINVAL;
        goto exit;
    }

    /* If ftrt processing loop in progress, indicate to exit
       before acquiring the lock here */
    check_and_exit_lab(st_session);
    pthread_mutex_lock(&st_session->lock);
    status = start_recognition_l(st_session, config, callback, cookie);
    pthread_mutex_unlock(&st_session->lock);

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s:[%d] Exit status %d", __func__, sound_model_handle, status);
    return status;
}

static int stdev_stop_recognition(const struct sound_trigger_hw_device *dev,
                                 sound_model_handle_t sound_model_handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct sound_trigger_session *st_session = NULL;
    int status = 0;

    ALOGD("%s:[%d] Enter", __func__, sound_model_handle);

    pthread_mutex_lock(&stdev->lock);
    if (is_ssr_active()) {
        ALOGV("%s: waiting on SSR completion", __func__);
        pthread_cond_wait(&stdev->cond, &stdev->lock);
    }
    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (st_session == NULL) {
        ALOGE("%s: Could not find sound model %d", __func__, sound_model_handle);
        status = -EINVAL;
        pthread_mutex_unlock(&stdev->lock);
        return status;
    }

    /* If ftrt processing loop in progress, indicate to exit
       before acquiring the lock here */
    check_and_exit_lab(st_session);
    pthread_mutex_lock(&st_session->lock);
    if (st_session->callback)
        status = stop_recognition_l(st_session);
    pthread_mutex_unlock(&st_session->lock);

    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s:[%d] Exit status %d", __func__, sound_model_handle, status);
    return status;
}

static int stdev_close(hw_device_t *device)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)device;

    ALOGD("%s: count=%d", __func__, stdev_ref_cnt);

    pthread_mutex_lock(&stdev_init_lock);
    if (!stdev || (--stdev_ref_cnt != 0)) {
        goto exit;
    }

    pthread_mutex_lock(&stdev->lock);
    platform_stdev_deinit(stdev->platform);
    free(stdev->cpe_pcm_use_cases);
    free(stdev->ape_pcm_use_cases);
    free(stdev->dev_ref_cnt);
    transit_stdev_deinit(stdev);
    pthread_mutex_unlock(&stdev->lock);

    pthread_mutex_destroy(&stdev->lock);
    free(device);
    stdev = NULL;

exit:
    pthread_mutex_unlock(&stdev_init_lock);
    ALOGD("%s: Exit device=%p cnt=%d ", __func__, stdev, stdev_ref_cnt);
    return 0;
}

static int stdev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    int status = 0;
    char audio_hal_lib[100];

    ALOGD("%s: Enter", __func__);

    if (strcmp(name, SOUND_TRIGGER_HARDWARE_INTERFACE) != 0) {
        ALOGE("%s: ERROR. wrong interface", __func__);
        status = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&stdev_init_lock);
    if (stdev_ref_cnt != 0) {
        *device = &stdev->device.common;
        stdev_ref_cnt++;
        ALOGD("%s: returning existing stdev instance, exit", __func__);
        pthread_mutex_unlock(&stdev_init_lock);
        return status;
    }

    stdev = calloc(1, sizeof(struct sound_trigger_device));
    if (!stdev) {
        ALOGE("%s: ERROR. stdev alloc failed", __func__);
        status = -ENOMEM;
        goto exit;
    }

    snprintf(audio_hal_lib, sizeof(audio_hal_lib),
             AUDIO_HAL_LIBRARY_PATH, XSTR(SOUND_TRIGGER_PLATFORM));
    stdev->audio_hal_handle = dlopen(audio_hal_lib, RTLD_NOW);
    if (!stdev->audio_hal_handle){
        ALOGE("%s: ERROR. dlopen failed for %s", __func__, audio_hal_lib);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->platform = platform_stdev_init(stdev);
    if (!stdev->platform) {
        ALOGE("%s: ERROR. platform init failed", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->audio_hal_cb = (audio_hw_call_back_t)dlsym(stdev->audio_hal_handle,
                                              "audio_hw_call_back");
    if (!stdev->audio_hal_cb){
        ALOGE("%s: ERROR. dlsym error %s for audio_hw_call_back", __func__, dlerror());
        status = -ENODEV;
        goto exit_1;
    }

    stdev->ape_pcm_use_cases =
        calloc(stdev->max_ape_sessions, sizeof(struct use_case_info));

    if (!stdev->ape_pcm_use_cases) {
        ALOGE("%s: ERROR. Mem alloc failed for ape use cases", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->cpe_pcm_use_cases =
        calloc(stdev->max_cpe_sessions, sizeof(struct use_case_info));

    if (!stdev->cpe_pcm_use_cases) {
        ALOGE("%s: ERROR. Mem alloc failed for cpe use cases", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->dev_ref_cnt =
        calloc(ST_DEVICE_EXEC_MODE_MAX * ST_DEVICE_MAX, sizeof(int));

    if (!stdev->dev_ref_cnt) {
        ALOGE("%s: ERROR. Mem alloc failed dev ref cnt", __func__);
        status = -ENOMEM;
        goto exit_1;
    }

    stdev->device.common.tag = HARDWARE_DEVICE_TAG;
    stdev->device.common.version = SOUND_TRIGGER_DEVICE_API_VERSION_1_0;
    stdev->device.common.module = (struct hw_module_t *) module;
    stdev->device.common.close = stdev_close;
    stdev->device.get_properties = stdev_get_properties;
    stdev->device.load_sound_model = stdev_load_sound_model;
    stdev->device.unload_sound_model = stdev_unload_sound_model;
    stdev->device.start_recognition = stdev_start_recognition;
    stdev->device.stop_recognition = stdev_stop_recognition;
    stdev->session_id = 1;
    stdev->exec_mode = ST_DEVICE_EXEC_MODE_NONE;

    pthread_mutex_init(&stdev->lock, (const pthread_mutexattr_t *) NULL);
    list_init(&stdev->sound_model_list);

    transit_stdev_init(stdev);

    *device = &stdev->device.common;
    stdev_ref_cnt++;
    pthread_mutex_unlock(&stdev_init_lock);

    return 0;

exit_1:
    if (stdev->cpe_pcm_use_cases)
        free(stdev->cpe_pcm_use_cases);

    if (stdev->ape_pcm_use_cases)
        free(stdev->ape_pcm_use_cases);

    if(stdev->audio_hal_handle)
        dlclose(stdev->audio_hal_handle);

    if(stdev->platform)
       platform_stdev_deinit(stdev->platform);

exit:

    if(stdev)
        free(stdev);
    stdev = NULL;
    *device = NULL;

    pthread_mutex_unlock(&stdev_init_lock);
    ALOGD("%s: Exit status %d", __func__, status);
    return status;
}

/* Audio hal calls this callback for notifying Subsystem restart,
   lab stop and concurrency events */
int sound_trigger_hw_call_back(audio_event_type_t event,
                               audio_event_info_t* config)
{
    struct listnode *p_ses_node;
    sound_trigger_session_t *p_ses;
    int ret = 0;

    if (!stdev)
        return -ENODEV;

    switch (event) {
    case AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE:
    case AUDIO_EVENT_CAPTURE_DEVICE_ACTIVE:
    case AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE:
    case AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE:
        handle_audio_concurrency(event);
        break;
    case AUDIO_EVENT_STOP_LAB:
        if(!config || !config->u.ses_info.p_ses) {
            ALOGE("%s: NULL params for stop lab", __func__);
            break;
        }
        pthread_mutex_lock(&stdev->lock);
        /* check capture_handle to ensure pointer sanity */
        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, sound_trigger_session_t, list_node);
            if (p_ses->capture_handle == config->u.ses_info.capture_handle ||
                transit_ses_buffering(p_ses, config->u.ses_info.capture_handle)) {
                /* If lab processing loop in progress, indicate to exit
                before acquiring the lock here */
                check_and_exit_lab(p_ses);
                pthread_mutex_lock(&p_ses->lock);
                if (p_ses->callback)
                    stop_recognition_internal(p_ses, false);
                pthread_mutex_unlock(&p_ses->lock);
                break;
            }
        }
        pthread_mutex_unlock(&stdev->lock);
        break;

    case AUDIO_EVENT_SSR:
        if (!config) {
            ALOGE("%s: NULL config for SSR", __func__);
            break;
        }

        if (config->u.status == SND_CARD_STATUS_OFFLINE ||
            config->u.status == CPE_STATUS_OFFLINE)
            handle_ssr_offline(config->u.status);

        if (config->u.status == SND_CARD_STATUS_ONLINE ||
            config->u.status == CPE_STATUS_ONLINE)
            handle_ssr_online(config->u.status);

        break;
    case AUDIO_EVENT_NUM_ST_SESSIONS:
        if (!config) {
            ALOGE("%s: NULL config for AUDIO_EVENT_NUM_ST_SESSIONS", __func__);
            break;
        }
        pthread_mutex_lock(&stdev->lock);
        ALOGV("%s: num sessions configured %d", __func__, config->u.value);
        stdev->num_sessions_configured = config->u.value;
        pthread_mutex_unlock(&stdev->lock);
        break;

    case AUDIO_EVENT_READ_SAMPLES:
        if(!config) {
            ALOGE("%s: Invalid config", __func__);
            ret = -EINVAL;
            break;
        }
        p_ses = (sound_trigger_session_t*)config->u.aud_info.ses_info->p_ses;
        ret = read_capture_bytes(p_ses->pcm, config->u.aud_info.buf,
                                 config->u.aud_info.num_bytes);

        break;

    default:
        ALOGW("%s: Unknown event %d", __func__, event);
        break;
    }

    return ret;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = stdev_open,
};

struct sound_trigger_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SOUND_TRIGGER_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SOUND_TRIGGER_HARDWARE_MODULE_ID,
        .name = "Sound trigger HAL",
        .author = "QUALCOMM Technologies, Inc",
        .methods = &hal_module_methods,
    },
};
