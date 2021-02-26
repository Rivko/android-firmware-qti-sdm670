/*
 *
 * This file implements the hw session functionality specific to LSM HW
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "sound_trigger_hw"
#define ATRACE_TAG (ATRACE_TAG_HAL)
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/trace.h>
#include <system/thread_defs.h>

#include <stdarg.h>
#include <unistd.h>

#include "st_common_defs.h"
#include "sound_trigger_platform.h"
#include "st_hw_session_lsm.h"
#include "sound_trigger_hw.h"

#define XSTR(x) STR(x)
#define STR(x) #x

static int ape_reg_sm(st_hw_session_t* p_ses, void *sm_data,
           sound_trigger_sound_model_type_t sm_type);
static int ape_reg_sm_params(st_hw_session_t* p_ses, unsigned int recognition_mode,
    bool capture_requested, struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data);

static int ape_dereg_sm(st_hw_session_t* p_ses, bool capture_requested);
static int ape_dereg_sm_params(st_hw_session_t* p_ses);
static int ape_start(st_hw_session_t* p_ses);
static int ape_stop(st_hw_session_t* p_ses);
static int ape_stop_buffering(st_hw_session_t* p_ses);

static int cpe_reg_sm(st_hw_session_t* p_ses,void *sm_data, sound_trigger_sound_model_type_t sm_type);
static int cpe_reg_sm_params(st_hw_session_t* p_ses, unsigned int recognition_mode,
    bool capture_requested, struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data);

static int cpe_dereg_sm(st_hw_session_t* p_ses, bool capture_requested);
static int cpe_dereg_sm_params(st_hw_session_t* p_ses);
static int cpe_start(st_hw_session_t* p_ses);
static int cpe_stop(st_hw_session_t* p_ses);
static int cpe_stop_buffering(st_hw_session_t *p_ses, bool capture_requested);

/* Routing layer functions to route to either ADSP or CPE */
static int route_reg_sm_ape(st_hw_session_t *p_ses,
    void *sm_data, sound_trigger_sound_model_type_t sm_type);
static int route_reg_sm_params_ape(st_hw_session_t* p_ses,
    unsigned int recognition_mode, bool capture_requested,
    struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data);
static int route_dereg_sm_ape(st_hw_session_t* p_ses, bool capture_requested);
static int route_dereg_sm_params_ape(st_hw_session_t* p_ses);
static int route_start_ape(st_hw_session_t* p_ses);
static int route_stop_ape(st_hw_session_t* p_ses);
static int route_stop_buffering_ape(st_hw_session_t* p_ses, bool capture_requested);
static int route_set_device_ape(st_hw_session_t* p_ses,
                                bool enable);
static int route_read_pcm_ape(st_hw_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes);
static void route_audio_capture_ape(st_hw_session_t* p_ses);
static int route_send_custom_chmix_coeff_ape(st_hw_session_t *p_ses, char *str);

static int route_reg_sm_cpe(st_hw_session_t *p_ses,
    void *sm_data, sound_trigger_sound_model_type_t sm_type);
static int route_reg_sm_params_cpe(st_hw_session_t* p_ses,
    unsigned int recognition_mode, bool capture_requested,
    struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data);
static int route_dereg_sm_cpe(st_hw_session_t* p_ses, bool capture_requested);
static int route_dereg_sm_params_cpe(st_hw_session_t* p_ses);
static int route_start_cpe(st_hw_session_t* p_ses);
static int route_restart(st_hw_session_t* p_ses, unsigned int recognition_mode,
   bool capture_requested, struct sound_trigger_recognition_config *rc_config __unused,
   sound_trigger_sound_model_type_t sm_type, void *sm_data);
static int route_stop_cpe(st_hw_session_t* p_ses);
static int route_stop_buffering_cpe(st_hw_session_t* p_ses, bool capture_requested);
static int route_set_device_cpe(st_hw_session_t* p_ses,
                                bool enable);
static int route_read_pcm_cpe(st_hw_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes);
static void route_audio_capture_cpe(st_hw_session_t* p_ses);
static int route_send_custom_chmix_coeff_cpe(st_hw_session_t *p_ses, char *str);
static int route_disable_device(st_hw_session_t *p_ses, bool setting_device);
static int route_enable_device(st_hw_session_t *p_ses, bool setting_device);
static void request_exit_callback_thread(st_hw_session_lsm_t *p_lsm_ses);

static int dbg_trace_lab_buf_cnt = -1;

static struct pcm_config stdev_ape_pcm_config = {
    .channels = SOUND_TRIGGER_CHANNEL_MODE_MONO,
    .rate = SOUND_TRIGGER_SAMPLING_RATE_16000,
    .period_size = SOUND_TRIGGER_APE_PERIOD_SIZE,
    .period_count = SOUND_TRIGGER_APE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

static struct pcm_config stdev_cpe_pcm_config = {
    .channels = SOUND_TRIGGER_CHANNEL_MODE_MONO,
    .rate = SOUND_TRIGGER_SAMPLING_RATE_16000,
    .period_size = SOUND_TRIGGER_CPE_PERIOD_SIZE,
    .period_count = SOUND_TRIGGER_CPE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct st_session_fptrs ape_fptrs = {

    .reg_sm = route_reg_sm_ape ,
    .reg_sm_params = route_reg_sm_params_ape,
    .dereg_sm = route_dereg_sm_ape ,
    .dereg_sm_params = route_dereg_sm_params_ape,
    .start = route_start_ape,
    .restart = route_restart,
    .stop = route_stop_ape,
    .stop_buffering = route_stop_buffering_ape,
    .set_device = route_set_device_ape,
    .read_pcm = route_read_pcm_ape,
    .process_lab_capture = route_audio_capture_ape,
    .send_custom_chmix_coeff = route_send_custom_chmix_coeff_ape,
    .disable_device = route_disable_device,
    .enable_device = route_enable_device,
};

struct st_session_fptrs cpe_fptrs = {
    .reg_sm = route_reg_sm_cpe ,
    .reg_sm_params = route_reg_sm_params_cpe,
    .dereg_sm = route_dereg_sm_cpe  ,
    .dereg_sm_params = route_dereg_sm_params_cpe,
    .start = route_start_cpe,
    .restart = route_restart,
    .stop = route_stop_cpe,
    .stop_buffering = route_stop_buffering_cpe,
    .set_device = route_set_device_cpe,
    .read_pcm = route_read_pcm_cpe,
    .process_lab_capture = route_audio_capture_cpe,
    .send_custom_chmix_coeff = route_send_custom_chmix_coeff_cpe,
    .disable_device = route_disable_device,
    .enable_device = route_enable_device,
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

#ifdef LSM_EVENT_TIMESTAMP_MODE_SUPPORT
static int set_lsm_fwk_mode(st_hw_session_lsm_t *p_lsm_ses)
{
    int status;
    struct st_vendor_info *v_info = p_lsm_ses->common.vendor_uuid_info;
    unsigned int fwk_mode = LSM_EVENT_NON_TIME_STAMP_MODE;

    /* set fwk mode is not supported in CPE mode */
    if (p_lsm_ses->common.exec_mode == ST_EXEC_MODE_CPE)
        return 0;

    if (v_info && (v_info->fwk_mode == SOUND_TRIGGER_EVENT_TIME_STAMP_MODE))
        fwk_mode = LSM_EVENT_TIME_STAMP_MODE;

    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_FWK_MODE_CONFIG,
                       &fwk_mode);
    if (status)
        ALOGE("%s: SNDRV_LSM_SET_FWK_MODE_CONFIG status=%d", __func__, status);

    return status;
}

static void update_lsm_event_status_info(int *request,
                                         char **st_lsm_event_cmd)
{
    *request = SNDRV_LSM_EVENT_STATUS_V3;
    *st_lsm_event_cmd = strdup("SNDRV_LSM_EVENT_STATUS_V3");
}

static uint64_t get_event_timestamp(st_lsm_event_status_t *params)
{
    uint64_t timestamp;

    timestamp = ((uint64_t)params->timestamp_msw << 32) |
                           params->timestamp_lsw;
    return timestamp;
}
#else
static int set_lsm_fwk_mode(st_hw_session_lsm_t *p_lsm_ses __unused)
{
    /* set fwk mode not supported */
    return 0;
}

static void update_lsm_event_status_info(int *request,
                                         char **st_lsm_event_cmd)
{
    *request = SNDRV_LSM_EVENT_STATUS;
    *st_lsm_event_cmd = strdup("SNDRV_LSM_EVENT_STATUS");
}

static uint64_t get_event_timestamp(st_lsm_event_status_t *params __unused)
{
    /* timestamp mode not supported */
    return 0;
}
#endif

#ifdef LSM_POLLING_ENABLE_SUPPORT
static int lsm_set_port(st_hw_session_lsm_t *p_lsm_ses)
{
    int status;

    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_PORT);
    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_SET_PORT, status=%d", __func__, status);
    return status;
}

static bool fill_lsm_poll_enable_params(struct st_vendor_info *v_info,
                                    st_exec_mode_t exec_mode,
                                    st_lsm_poll_enable_t *poll_enable,
                                    struct lsm_params_info *poll_en_params)
{
    poll_enable->poll_en = v_info->profile_type == ST_PROFILE_TYPE_NONE;
    poll_en_params->param_type = LSM_POLLING_ENABLE;
    poll_en_params->module_id = v_info->params[exec_mode][POLLING_ENABLE].module_id;
    poll_en_params->param_id = v_info->params[exec_mode][POLLING_ENABLE].param_id;
    poll_en_params->param_size = sizeof(*poll_enable);
    poll_en_params->param_data = (unsigned char *)poll_enable;
    return true;
}
#else
static int lsm_set_port(st_hw_session_lsm_t *p_lsm_ses __unused)
{
    return 0;
}

static bool fill_lsm_poll_enable_params(struct st_vendor_info *v_info __unused,
                                    st_exec_mode_t exec_mode __unused,
                                    st_lsm_poll_enable_t *poll_enable __unused,
                                    struct lsm_params_info *poll_en_params __unused)
{
    return false;
}
#endif

static void fill_set_params_payload(struct lsm_setparam_payload *custom_conf_setparam,
               uint32_t data_payload_size, uint32_t data_payload_addr_lsw,
               uint32_t data_payload_addr_msw, uint32_t mem_map_handle)
{
    custom_conf_setparam->data_payload_size = data_payload_size;
    custom_conf_setparam->data_payload_addr_lsw = data_payload_addr_lsw;
    custom_conf_setparam->data_payload_addr_msw = data_payload_addr_msw;
    custom_conf_setparam->mem_map_handle = mem_map_handle;
}

static st_profile_type_t get_profile_type(struct st_vendor_info *v_info)
{
    st_profile_type_t profile_type;

    profile_type = v_info ? v_info->profile_type : ST_PROFILE_TYPE_NONE;
    return profile_type;
}

void check_and_exit_lab(st_hw_session_t *p_ses)
{
    st_hw_session_lsm_t *p_lsm_ses =
        (st_hw_session_lsm_t *)p_ses;

    if ((p_lsm_ses->lab_buffers_allocated && !p_lsm_ses->exit_lab_processing) &&
        CHECK_STATE(p_lsm_ses->common.state, SES_BUFFERING)) {

        pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
        p_lsm_ses->exit_lab_processing = true;
        pthread_cond_signal(&p_lsm_ses->pcm_out_buf_cond);
        pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
    }
}

static void ape_enable_use_case(bool enable, st_hw_session_t *p_ses)
{
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;
    st_profile_type_t profile_type;
    char use_case[USECASE_STRING_SIZE];

    profile_type = get_profile_type(p_ses->vendor_uuid_info);
    strlcpy(use_case,
            p_ses->stdev->ape_pcm_use_cases[p_ses->use_case_idx].use_case,
            USECASE_STRING_SIZE);
    platform_stdev_check_and_append_usecase(p_ses->stdev->platform,
                                           use_case, profile_type);
    if (enable) {
        ALOGD("%s: enable use case = %s", __func__, use_case);
        platform_stdev_send_stream_app_type_cfg(p_ses->stdev->platform,
                                   p_lsm_ses->pcm_id, p_ses->st_device,
                                   p_ses->exec_mode, profile_type);

        platform_stdev_send_ec_ref_cfg(p_ses->stdev->platform, profile_type, true);


        ATRACE_BEGIN("sthal:lsm: audio_route_apply_and_update_path");
        audio_route_apply_and_update_path(p_ses->stdev->audio_route, use_case);
        ATRACE_END();

    } else {
        ALOGD("%s: disable use case = %s", __func__, use_case);
        ATRACE_BEGIN("sthal:lsm: audio_route_reset_and_update_path");
        audio_route_reset_and_update_path(p_ses->stdev->audio_route, use_case);
        ATRACE_END();

        platform_stdev_send_ec_ref_cfg(p_ses->stdev->platform, profile_type, false);
    }
}

static int read_pcm_data(st_hw_session_lsm_t *p_ses,
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
            ALOGVV("%s: done waiting on cond", __func__);
            if (ret) {
                ALOGE("%s: ERROR. read wait timed out, ret %d", __func__, ret);
                p_ses->exit_lab_processing = true;
                ret = -EIO;
                goto exit;
            }
            if (p_ses->exit_lab_processing) {
                 ALOGVV("%s: buffering stopped while waiting on cond, exiting",
                    __func__);
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

static int write_pcm_data_cpe(st_hw_session_lsm_t *p_lsm_ses,
                     unsigned char *buf,
                     unsigned int bytes)
{
    unsigned int free_bytes, write_bytes;
    unsigned int copy_bytes;
    unsigned char *updated_wr_ptr;

    pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
    ALOGVV("%s: bytes=%d, unread_bytes=%d size=%d", __func__,
       bytes, p_lsm_ses->pcm_out_buf_unread_bytes, p_lsm_ses->pcm_out_buf_size);

    while (!p_lsm_ses->exit_lab_processing && (bytes > 0)) {
        free_bytes = p_lsm_ses->pcm_out_buf_size - p_lsm_ses->pcm_out_buf_unread_bytes;
        if (!free_bytes) {
            ALOGVV("%s: waiting on cond, bytes=%d", __func__, bytes);
            /* wait till space is avaialable to write */
            pthread_cond_wait(&p_lsm_ses->pcm_out_buf_cond, &p_lsm_ses->pcm_out_buf_lock);
            if (p_lsm_ses->exit_lab_processing) {
                 ALOGVV("%s: buffering stopped while waiting on cond, exiting", __func__);
                pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
                return -EIO;
            }
        }
        write_bytes = MIN(bytes, free_bytes);

        if (p_lsm_ses->pcm_out_buf_end_ptr < (p_lsm_ses->pcm_out_buf_wr_ptr + write_bytes)) {
            copy_bytes = p_lsm_ses->pcm_out_buf_end_ptr - p_lsm_ses->pcm_out_buf_wr_ptr;
            memcpy(p_lsm_ses->pcm_out_buf_wr_ptr, buf, copy_bytes);
            memcpy(p_lsm_ses->pcm_out_buf_start_ptr,
                   buf + copy_bytes, write_bytes - copy_bytes);
            updated_wr_ptr = p_lsm_ses->pcm_out_buf_start_ptr + (write_bytes - copy_bytes);
            p_lsm_ses->pcm_out_buf_wr_ptr = updated_wr_ptr;
            p_lsm_ses->pcm_out_buf_unread_bytes += write_bytes;
        } else {
            memcpy(p_lsm_ses->pcm_out_buf_wr_ptr, buf, write_bytes);
            p_lsm_ses->pcm_out_buf_wr_ptr += write_bytes;
            p_lsm_ses->pcm_out_buf_unread_bytes += write_bytes;
        }
        ALOGV("%s: about to signal condition", __func__);
        pthread_cond_signal(&p_lsm_ses->pcm_out_buf_cond);
        bytes -= write_bytes;
        buf += write_bytes;
    }
    pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
    return 0;
}

static int write_pcm_data_ape(st_hw_session_lsm_t *p_lsm_ses,
                     unsigned char *buf,
                     unsigned int bytes)
{
    unsigned int free_bytes;
    unsigned int copy_bytes;
    unsigned char *updated_wr_ptr;

    ALOGVV("%s: bytes=%d, unread_bytes=%d size=%d", __func__,
       bytes, p_lsm_ses->pcm_out_buf_unread_bytes, p_lsm_ses->pcm_out_buf_size);

    /*
     * Unlike the cpe design, if there is not enough space in the buffer, the existing
     * data will be overwritten.
     */
    free_bytes = p_lsm_ses->pcm_out_buf_size - p_lsm_ses->pcm_out_buf_unread_bytes;

    if (p_lsm_ses->pcm_out_buf_end_ptr < (p_lsm_ses->pcm_out_buf_wr_ptr + bytes)) {
        copy_bytes = p_lsm_ses->pcm_out_buf_end_ptr - p_lsm_ses->pcm_out_buf_wr_ptr;
        memcpy(p_lsm_ses->pcm_out_buf_wr_ptr, buf, copy_bytes);
        memcpy(p_lsm_ses->pcm_out_buf_start_ptr,
               buf + copy_bytes, bytes - copy_bytes);
        updated_wr_ptr = p_lsm_ses->pcm_out_buf_start_ptr + (bytes - copy_bytes);
        p_lsm_ses->pcm_out_buf_wr_ptr = updated_wr_ptr;
        p_lsm_ses->pcm_out_buf_unread_bytes += bytes;
    } else {
        memcpy(p_lsm_ses->pcm_out_buf_wr_ptr, buf, bytes);
        p_lsm_ses->pcm_out_buf_wr_ptr += bytes;
        p_lsm_ses->pcm_out_buf_unread_bytes += bytes;
    }

    if (bytes > free_bytes) {
        ALOGW("%s: Buffer overwrite occurred: bytes %d, free_bytes %d",
                __func__, bytes, free_bytes);
        p_lsm_ses->pcm_out_buf_rd_ptr = p_lsm_ses->pcm_out_buf_wr_ptr;
        p_lsm_ses->pcm_out_buf_unread_bytes = p_lsm_ses->pcm_out_buf_size;
    }

    ALOGVV("%s: about to signal condition", __func__);
    pthread_cond_signal(&p_lsm_ses->pcm_out_buf_cond);
    return 0;
}

static bool search_for_syncword(st_hw_session_lsm_t *p_ses,
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

static void get_ftrt_processing_data(st_hw_session_lsm_t *p_ses,
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

void process_raw_lab_data_cpe(st_hw_session_lsm_t *p_lsm_ses)
{
    int status = 0;

    ST_DBG_DECLARE(FILE *fptr_drv = NULL; static int file_cnt = 0);
    ST_DBG_FILE_OPEN_WR(fptr_drv, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_drv_data_cpe", "pcm", file_cnt++);

    /* Initialize pcm output buffer pointers */
    p_lsm_ses->pcm_out_buf_start_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_end_ptr = p_lsm_ses->pcm_out_buf + p_lsm_ses->pcm_out_buf_size;
    p_lsm_ses->pcm_out_buf_wr_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_rd_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_unread_bytes = 0;

    ST_DBG_ATRACE_ASYNC_BEGIN_IF(!(dbg_trace_lab_buf_cnt = 0),"sthal:lsm:cpe: lsm_buffer_read",
                                    p_lsm_ses->common.sm_handle);
    while (!p_lsm_ses->exit_lab_processing) {
        ALOGVV("%s: pcm_read reading bytes=%d", __func__, p_lsm_ses->lab_drv_buf_size);
        status = pcm_read(p_lsm_ses->pcm, p_lsm_ses->lab_drv_buf, p_lsm_ses->lab_drv_buf_size);
        ST_DBG_ATRACE_ASYNC_END_IF((++dbg_trace_lab_buf_cnt == dbg_trace_max_lab_reads),
                                    "sthal:lsm:cpe: lsm_buffer_read", p_lsm_ses->common.sm_handle);

        ALOGVV("%s: pcm_read done", __func__);

        ST_DBG_FILE_WRITE(fptr_drv, p_lsm_ses->lab_drv_buf,
            p_lsm_ses->lab_drv_buf_size);

        if (status) {
            ALOGE("%s: pcm read failed status %d - %s", __func__, status,
                  pcm_get_error(p_lsm_ses->pcm));
            p_lsm_ses->exit_lab_processing = true;
            p_lsm_ses->common.fptrs->stop_buffering(&p_lsm_ses->common, true);
            break;
        }

        status = write_pcm_data_cpe(p_lsm_ses, p_lsm_ses->lab_drv_buf, p_lsm_ses->lab_drv_buf_size);
        if (status)
                break;
    }

    ST_DBG_FILE_CLOSE(fptr_drv);
    ALOGVV("%s: Exit status=%d", __func__, status);
}

void process_packetized_lab_data(st_hw_session_lsm_t *p_ses)
{
    struct cpe_packet_hdr *prev_packet = NULL;
    bool first_sync_word_found = false;
    int sync_max_retry_cnt = SOUND_TRIGGER_SYNC_WORD_MAX_RETRY_CNT;
    int status = 0, dec_status = 0, out_samples = 0;
    unsigned int copy_bytes, dec_bytes, tmp_size;
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

    ST_DBG_DECLARE(FILE *fptr_drv = NULL, *fptr_pcm = NULL, *fptr_dec = NULL;
                   static int fcnt = 0, dec_fcnt = 0;);
    ST_DBG_FILE_OPEN_WR(fptr_drv, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_drv_data_cpe", "bin", fcnt);
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
    ST_DBG_ATRACE_ASYNC_BEGIN_IF(!(dbg_trace_lab_buf_cnt = 0),"sthal:lsm:cpe: lsm_buffer_read",
                                p_ses->common.sm_handle);
    while (!p_ses->exit_lab_processing) {

        ALOGVV("%s: pcm_read reading bytes=%d", __func__, driver_rd_bytes);
        status = pcm_read(p_ses->pcm, driver_rd_buf, driver_rd_bytes);
        ST_DBG_ATRACE_ASYNC_END_IF(++dbg_trace_lab_buf_cnt == dbg_trace_max_lab_reads,
                                    "sthal:lsm:cpe: lsm_buffer_read", p_ses->common.sm_handle);
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
                if (--sync_max_retry_cnt <= 0) {
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
                if (CHECK_BIT(p_ses->common.vendor_uuid_info->kw_capture_format,
                              ADPCM)) {
                    ATRACE_BEGIN("sthal:lsm: adpcm_dec_init");
                    dec_status = p_ses->common.stdev->adpcm_dec_init(p_ses->adpcm_dec_state);
                    ATRACE_END();
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
                    status = write_pcm_data_cpe(p_ses, &ftrt_buf_ptr[ftrt_buf_idx], copy_bytes);
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
                            dec_bytes = MIN(MIN(dec_bytes, ftrt_rem_bytes),
                                (ADPCM_MAX_IN_FRAME_SIZE - frame_buf_idx));
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
                            ATRACE_BEGIN("sthal:lsm: adpcm_dec_process");
                            dec_status = p_ses->common.stdev->adpcm_dec_process((short *)frame_buf_ptr,
                                                                  (short *)dec_out_buf,
                                                                  frame_buf_idx,
                                                                  &out_samples,
                                                                  p_ses->adpcm_dec_state);
                            ATRACE_END();
                            if (dec_status) {
                                ALOGE("%s: ERROR. adpcm dec failed status %d", __func__, dec_status);
                                goto exit;
                            }
                            ALOGVV("%s: adpcm_dec_process done. frame_buf_idx=%d out_samples=%d",
                                   __func__, frame_buf_idx, out_samples);
                            if (out_samples) {
                                ST_DBG_FILE_WRITE(fptr_pcm, dec_out_buf, out_samples << 1);
                                status = write_pcm_data_cpe(p_ses, dec_out_buf, out_samples << 1);
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
        p_ses->common.fptrs->stop_buffering(&p_ses->common, true);
    }

    ST_DBG_FILE_CLOSE(fptr_drv);
    ST_DBG_FILE_CLOSE(fptr_dec);
    ST_DBG_FILE_CLOSE(fptr_pcm);

    ALOGVV("%s: Exit status=%d", __func__, status);
}

void process_raw_lab_data_ape(st_hw_session_lsm_t *p_lsm_ses)
{
    int status = 0;

    ST_DBG_DECLARE(FILE *fptr_drv = NULL; static int file_cnt = 0);
    ST_DBG_FILE_OPEN_WR(fptr_drv, ST_DEBUG_DUMP_LOCATION,
                        "st_lab_drv_data_ape", "pcm", file_cnt++);

    /* Initialize pcm output buffer pointers */
    pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
    p_lsm_ses->pcm_out_buf_start_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_end_ptr = p_lsm_ses->pcm_out_buf + p_lsm_ses->pcm_out_buf_size;
    p_lsm_ses->pcm_out_buf_wr_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_rd_ptr = p_lsm_ses->pcm_out_buf;
    p_lsm_ses->pcm_out_buf_unread_bytes = 0;
    p_lsm_ses->lab_processing_active = true;

    ST_DBG_ATRACE_ASYNC_BEGIN_IF(!(dbg_trace_lab_buf_cnt = 0),"sthal:lsm:ape: lsm_buffer_read",
                                    p_lsm_ses->common.sm_handle);

    while (!p_lsm_ses->exit_lab_processing) {
        ALOGVV("%s: pcm_read reading bytes=%d", __func__, p_lsm_ses->lab_drv_buf_size);
        pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
        status = pcm_read(p_lsm_ses->pcm, p_lsm_ses->lab_drv_buf, p_lsm_ses->lab_drv_buf_size);
        pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
        ST_DBG_ATRACE_ASYNC_END_IF((++dbg_trace_lab_buf_cnt == dbg_trace_max_lab_reads),
                                    "sthal:lsm:ape: lsm_buffer_read", p_lsm_ses->common.sm_handle);

        ALOGVV("%s: pcm_read done", __func__);
        ST_DBG_FILE_WRITE(fptr_drv, p_lsm_ses->lab_drv_buf,
            p_lsm_ses->lab_drv_buf_size);

        if (status) {
            ALOGE("%s: pcm read failed status %d - %s", __func__, status,
                  pcm_get_error(p_lsm_ses->pcm));
            break;
        }

        write_pcm_data_ape(p_lsm_ses, p_lsm_ses->lab_drv_buf, p_lsm_ses->lab_drv_buf_size);
    }

    ape_stop_buffering(&p_lsm_ses->common);
    p_lsm_ses->lab_processing_active = false;
    pthread_cond_broadcast(&p_lsm_ses->pcm_out_buf_cond);
    pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
    ST_DBG_FILE_CLOSE(fptr_drv);
    ALOGVV("%s: Exit status=%d", __func__, status);
}

static void *callback_thread_loop(void *context)
{
    st_hw_session_lsm_t *p_lsm_ses =
                                 (st_hw_session_lsm_t *)context;
    st_lsm_event_status_t *params;
    char *st_lsm_event_cmd = NULL;
    unsigned int payload_alloc_size = SOUND_TRIGGER_MAX_EVNT_PAYLOAD_SIZE;
    int status = 0;
    int event_status, request;
    st_hw_sess_event_t hw_sess_event; /* used to report event to st_session */

    if (p_lsm_ses == NULL) {
        ALOGE("%s: ERROR. null context.. exiting", __func__);
        return NULL;
    }
    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);

    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_DEFAULT);
    prctl(PR_SET_NAME, (unsigned long)"sound trigger callback", 0, 0, 0);

    pthread_mutex_lock(&p_lsm_ses->callback_thread_lock);
    params = calloc(1, sizeof(*params) + payload_alloc_size);
    if (params == NULL) {
        ALOGE("%s: ERROR. insufficient memory for payload", __func__);
        goto exit;
    }

    set_lsm_fwk_mode(p_lsm_ses);
    update_lsm_event_status_info(&request, &st_lsm_event_cmd);

    while (!p_lsm_ses->exit_callback_thread) {
        params->payload_size = payload_alloc_size;
        ALOGI("%s:[%d] Waiting for %s",
               __func__, p_lsm_ses->common.sm_handle, st_lsm_event_cmd);
        pthread_mutex_unlock(&p_lsm_ses->callback_thread_lock);
        status = pcm_ioctl(p_lsm_ses->pcm, request, params);
        pthread_mutex_lock(&p_lsm_ses->callback_thread_lock);

        ATRACE_ASYNC_BEGIN("sthal: event_callback", p_lsm_ses->common.sm_handle);
        ALOGI("%s:[%d] Received %s status=%d",
              __func__, p_lsm_ses->common.sm_handle, st_lsm_event_cmd, status);
        if (status < 0) {
            if (errno == ENOMEM) {
                payload_alloc_size = payload_alloc_size << 1;
                params = realloc(params, sizeof(*params) + payload_alloc_size);

                if (params == NULL) {
                    ALOGE("%s: ERROR. Not enough memory for payload",
                          __func__);
                    p_lsm_ses->exit_callback_thread = true;
                    break;
                }
                continue;
            } else {
                ALOGE("%s: ERROR. %s failed status %d",
                    __func__, st_lsm_event_cmd, status);
                p_lsm_ses->exit_callback_thread = true;
                break;
            }
        }
        if (p_lsm_ses->exit_callback_thread)
            break;

        ALOGD("%s: params status %d", __func__, params->status);
        switch (params->status) {
        case LSM_VOICE_WAKEUP_STATUS_RUNNING:
            continue;
        case LSM_VOICE_WAKEUP_STATUS_DETECTED:
            event_status = RECOGNITION_STATUS_SUCCESS;
            /*
             * note: since now we are dispatching the detected event to the
             * state machine there is no need to check if buffering enabled or
             * concurrency is active here, it will now be handled by the st_session
             * state machine
             */
            break;

        case LSM_VOICE_WAKEUP_STATUS_END_SPEECH:
        case LSM_VOICE_WAKEUP_STATUS_REJECTED:
            event_status = RECOGNITION_STATUS_FAILURE;
            break;
        default:
            ALOGW("%s: Unknown status %d", __func__, params->status);
            continue;
        }

        /* inform st_sessoin of the event */
        hw_sess_event.event_id = ST_HW_SESS_EVENT_DETECTED;
        hw_sess_event.payload.detected.timestamp = get_event_timestamp(params);
        hw_sess_event.payload.detected.detect_status = event_status;
        hw_sess_event.payload.detected.detect_payload =
            (void*)params->payload;
        hw_sess_event.payload.detected.payload_size = params->payload_size;
        pthread_mutex_unlock(&p_lsm_ses->callback_thread_lock);
        p_lsm_ses->common.callback_to_st_session(&hw_sess_event,
                p_lsm_ses->common.cookie);
        pthread_mutex_lock(&p_lsm_ses->callback_thread_lock);
    }

    if (st_lsm_event_cmd)
        free(st_lsm_event_cmd);

    if (params)
        free(params);

exit:
    pthread_cond_signal(&p_lsm_ses->callback_thread_cond);
    pthread_mutex_unlock(&p_lsm_ses->callback_thread_lock);
    ALOGD("%s:[%d] Exit", __func__, p_lsm_ses->common.sm_handle);
    return NULL;
}

static int deallocate_lab_buffers_ape(st_hw_session_lsm_t* p_ses)
{
    ALOGVV("%s:[%d] Enter", __func__, p_ses->common.sm_handle);
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

static int deallocate_lab_buffers_cpe(st_hw_session_lsm_t* p_ses)
{
    ALOGVV("%s:[%d] Enter", __func__, p_ses->common.sm_handle);
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

static int configure_lab(st_hw_session_lsm_t* p_ses, bool capture_requested)
{
    int status = 0, buf_en = 0;
    struct snd_lsm_output_format_cfg media_cfg;
    struct st_vendor_info *v_info = p_ses->common.vendor_uuid_info;

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
    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_out_format_cfg)");
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_OUT_FORMAT_CFG, &media_cfg);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_OUT_FORMAT_CFG, status=%d", __func__,
           status);
        return status;
    }

    buf_en = capture_requested ? 1 : 0;
    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_lab_control");
    status = pcm_ioctl(p_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
    ATRACE_END();
    if (status) ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d",
        __func__, status);

    return status;
}

static int allocate_lab_buffers_cpe(st_hw_session_lsm_t* p_ses)
{
    int status = 0;
    int adpcm_dec_scratch_size;
    unsigned int kw_tranfer_duration, rt_bytes_one_sec, ftrt_to_rt_factor;
    struct st_vendor_info *v_info = p_ses->common.vendor_uuid_info;

    ALOGVV("%s:[%d] Enter", __func__, p_ses->common.sm_handle);

    pthread_mutex_init(&p_ses->pcm_out_buf_lock,
        (const  pthread_mutexattr_t *) NULL);
    pthread_cond_init(&p_ses->pcm_out_buf_cond,
        (const  pthread_condattr_t *) NULL);

    p_ses->lab_drv_buf_size = pcm_frames_to_bytes(p_ses->pcm, p_ses->lab_config.period_size);
    p_ses->lab_drv_buf = (unsigned char *)calloc(1, p_ses->lab_drv_buf_size);
    if (!p_ses->lab_drv_buf) {
        ALOGE("%s: ERROR. Can not allocate lab buffer size %d", __func__, p_ses->lab_drv_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated lab buffer period size bytes =%d",
          __func__, p_ses->lab_drv_buf_size);

    rt_bytes_one_sec = (p_ses->common.config.rate * p_ses->common.config.channels *
                        (pcm_format_to_bits(p_ses->common.config.format) >> 3));

    if (v_info) {
        p_ses->pcm_out_buf_size = (v_info->client_capture_read_delay * rt_bytes_one_sec) / 1000;
        if (v_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {
            ftrt_to_rt_factor = (p_ses->lab_config.rate * pcm_format_to_bits(p_ses->lab_config.format)) /
                                (p_ses->common.config.rate * pcm_format_to_bits(p_ses->common.config.format));

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

    p_ses->pcm_out_buf = (unsigned char *)calloc(1, p_ses->pcm_out_buf_size);
    if (!p_ses->pcm_out_buf) {
        ALOGE("%s: ERROR. Can not allocate out buffer size %d",
              __func__, p_ses->pcm_out_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated out buffer size=%d", __func__, p_ses->pcm_out_buf_size);

    if (v_info && CHECK_BIT(v_info->kw_capture_format, ADPCM)) {
        p_ses->dec_buf = (unsigned char *)calloc(1, ADPCM_MAX_IN_FRAME_SIZE +
                                                  ADPCM_MAX_OUT_FRAME_SIZE);
        if (!p_ses->dec_buf) {
            ALOGE("%s: ERROR. Can not allocate decoder buffer size %d",
                  __func__, ADPCM_MAX_IN_FRAME_SIZE + ADPCM_MAX_OUT_FRAME_SIZE);
            status = -ENOMEM;
            goto error_exit;
        }
        ALOGV("%s: Allocated decoder buffers size=%d", __func__,
               ADPCM_MAX_IN_FRAME_SIZE + ADPCM_MAX_OUT_FRAME_SIZE);

        /* Instantiate the decoder */
        status = p_ses->common.stdev->adpcm_dec_get_scratch_size(&adpcm_dec_scratch_size);
        if (status) {
            ALOGE("%s: ERROR. Can not get adpcm dec scratch", __func__);
            status = -EINVAL;
            goto error_exit;
        }

        p_ses->adpcm_dec_state = (void *)calloc(1, adpcm_dec_scratch_size);
        if (!p_ses->adpcm_dec_state)
        {
            ALOGE("%s: ERROR. Can not allocate decoder scratch", __func__);
            status = -ENOMEM;
            goto error_exit;
        }
        ALOGV("%s: Allocated adpcm_dec_state size=%d", __func__,
              adpcm_dec_scratch_size);
    }
    p_ses->lab_buffers_allocated = true;

    return 0;

error_exit:
    deallocate_lab_buffers_cpe(p_ses);
    return status;
}

static int allocate_lab_buffers_ape(st_hw_session_lsm_t* p_lsm_ses)
{
    int status = 0;
    struct st_vendor_info *v_info = p_lsm_ses->common.vendor_uuid_info;
    unsigned int rt_bytes_one_sec;

    pthread_mutex_init(&p_lsm_ses->pcm_out_buf_lock,
        (const  pthread_mutexattr_t *) NULL);
    pthread_cond_init(&p_lsm_ses->pcm_out_buf_cond,
        (const  pthread_condattr_t *) NULL);

    p_lsm_ses->lab_drv_buf_size = pcm_frames_to_bytes(p_lsm_ses->pcm,
                                                      p_lsm_ses->common.config.period_size);
    p_lsm_ses->lab_drv_buf = (unsigned char *)calloc(1, p_lsm_ses->lab_drv_buf_size);
    if (!p_lsm_ses->lab_drv_buf) {
        ALOGE("%s: ERROR. Can not allocate lab buffer size %d", __func__,
            p_lsm_ses->lab_drv_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated lab buffer period size bytes =%d",
        __func__, p_lsm_ses->lab_drv_buf_size);

    rt_bytes_one_sec = (p_lsm_ses->common.config.rate *
                        p_lsm_ses->common.config.channels *
                        (pcm_format_to_bits(p_lsm_ses->common.config.format) >> 3));

    if (v_info)
        p_lsm_ses->pcm_out_buf_size = ((v_info->kw_duration +
            v_info->client_capture_read_delay) * rt_bytes_one_sec) / 1000;
    else
        p_lsm_ses->pcm_out_buf_size = (DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS *
                                       rt_bytes_one_sec) / 1000;

    p_lsm_ses->pcm_out_buf = (unsigned char *)calloc(1, p_lsm_ses->pcm_out_buf_size);
    if (!p_lsm_ses->pcm_out_buf) {
        ALOGE("%s: ERROR. Can not allocate out buffer size %d",
            __func__, p_lsm_ses->pcm_out_buf_size);
        status = -ENOMEM;
        goto error_exit;
    }
    ALOGV("%s: Allocated out buffer size=%d", __func__, p_lsm_ses->pcm_out_buf_size);
    p_lsm_ses->lab_buffers_allocated = true;

    return status;

error_exit:
    deallocate_lab_buffers_ape(p_lsm_ses);
    return status;
}

static int sound_trigger_set_device
(
   st_hw_session_t* p_ses,
   bool enable
)
{
    char st_device_name[DEVICE_NAME_MAX_SIZE] = { 0 };
    int ref_cnt_idx = 0, ref_cnt = 0;
    int status = 0;
    st_device_t st_device;
    audio_devices_t capture_device;
    int app_type;
    st_profile_type_t profile_type;

    profile_type = get_profile_type(p_ses->vendor_uuid_info);
    if (enable) {
        capture_device = platform_stdev_get_capture_device(p_ses->stdev->platform);
        st_device = platform_stdev_get_device(p_ses->stdev->platform,
            p_ses->vendor_uuid_info, capture_device, p_ses->exec_mode);

        if (platform_stdev_get_device_name(p_ses->stdev->platform, p_ses->exec_mode,
                st_device, st_device_name) < 0) {
            ALOGE("%s: Invalid sound trigger device name returned", __func__);
            return -EINVAL;
        }

        ref_cnt_idx = (p_ses->exec_mode * ST_DEVICE_MAX) + st_device;
        pthread_mutex_lock(&p_ses->stdev->ref_cnt_lock);
        ref_cnt = ++(p_ses->stdev->dev_ref_cnt[ref_cnt_idx]);
        pthread_mutex_unlock(&p_ses->stdev->ref_cnt_lock);

        if (1 == ref_cnt) {
            app_type = platform_stdev_get_device_app_type(p_ses->stdev->platform,
                                                          profile_type);

            status = platform_stdev_send_calibration(p_ses->stdev->platform,
                capture_device,
                p_ses->exec_mode,
                p_ses->vendor_uuid_info,
                app_type,
                false, ST_DEVICE_CAL);

            if (!status) {
                ALOGD("%s: enable device (%x) = %s", __func__, st_device,
                      st_device_name);
                ATRACE_BEGIN("sthal:lsm: audio_route_apply_and_update_path");
                audio_route_apply_and_update_path(p_ses->stdev->audio_route,
                                                  st_device_name);
                ATRACE_END();
                update_hw_mad_exec_mode(p_ses->exec_mode, profile_type);
                p_ses->stdev->capture_device = capture_device;
            }
        }
        p_ses->st_device = st_device;
        p_ses->st_device_name = strdup(st_device_name);
    } else {
        if (!p_ses->st_device_name) {
            ALOGE("%s: Invalid sound trigger device name", __func__);
            return -EINVAL;
        }

        ref_cnt_idx = (p_ses->exec_mode * ST_DEVICE_MAX) + p_ses->st_device;
        pthread_mutex_lock(&p_ses->stdev->ref_cnt_lock);
        ref_cnt = p_ses->stdev->dev_ref_cnt[ref_cnt_idx];
        if (0 < ref_cnt) {
            ref_cnt = --(p_ses->stdev->dev_ref_cnt[ref_cnt_idx]);
            pthread_mutex_unlock(&p_ses->stdev->ref_cnt_lock);
        } else {
            ALOGV("%s: ref_cnt = %d", __func__, ref_cnt);
            pthread_mutex_unlock(&p_ses->stdev->ref_cnt_lock);
            return status;
        }

        if (0 == ref_cnt) {
            ALOGD("%s: disable device (%x) = %s", __func__, p_ses->st_device,
                  p_ses->st_device_name);
            ATRACE_BEGIN("sthal:lsm: audio_route_reset_and_update_path");
            audio_route_reset_and_update_path(p_ses->stdev->audio_route,
                                              p_ses->st_device_name);
            ATRACE_END();
            update_hw_mad_exec_mode(ST_EXEC_MODE_NONE, profile_type);
        }
        free(p_ses->st_device_name);
    }
    return status;
}

static int get_confidence_levels_payload
(
   struct sound_trigger_phrase_sound_model *phrase_sm,
   st_hw_session_t *p_ses,
   const struct sound_trigger_recognition_config *config
)
{
    int status = 0;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;

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
        if (p_ses->stdev->smlib_handle) {
            status = p_ses->stdev->generate_st_recognition_config_payload(
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

static int ape_reg_sm(st_hw_session_t *p_ses, void *sm_data,
    sound_trigger_sound_model_type_t sm_type)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
        (st_hw_session_lsm_t*)p_ses;
    struct snd_lsm_sound_model_v2 lsm_sm_v2;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm =
       (struct sound_trigger_phrase_sound_model*)sm_data;
    struct sound_trigger_sound_model *common_sm=
       (struct sound_trigger_sound_model*)sm_data;
    struct snd_lsm_session_data ses_data;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    pthread_attr_t attr;
    int app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
    bool use_topology = false;

    ALOGD("%s:[%d] Enter", __func__, p_ses->sm_handle);

    p_lsm_ses->pcm_id = platform_ape_get_pcm_device_id(p_ses->stdev->platform,
        &p_ses->use_case_idx);
    if (p_lsm_ses->pcm_id < 0)
        return -ENODEV;

    if (v_info) {
        app_type = v_info->app_type;
        use_topology = true;
    }

    status = platform_stdev_send_calibration(p_ses->stdev->platform,
                                              p_ses->stdev->capture_device,
                                              p_ses->exec_mode,
                                              p_ses->vendor_uuid_info,
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
        status = platform_stdev_send_calibration(p_ses->stdev->platform,
                                                  p_ses->stdev->capture_device,
                                                  p_ses->exec_mode,
                                                  p_ses->vendor_uuid_info,
                                                  app_type, use_topology,
                                                  ST_SESSION_CAL);
    }
    if (status) {
        ALOGE("%s: ERROR. sending calibration failed status %d topo %d",
              __func__, status, use_topology);
        goto sm_error;
    }
    p_lsm_ses->use_topology_intf = use_topology;
    p_lsm_ses->common.config = stdev_ape_pcm_config;

    platform_stdev_check_and_update_pcm_config(p_ses->stdev->platform,
                                               &p_lsm_ses->common.config,
                                               v_info, p_ses->exec_mode);

    ALOGD("%s: opening pcm device=%d topo %d", __func__, p_lsm_ses->pcm_id,
          p_lsm_ses->use_topology_intf);
    ALOGV("%s: config: channels=%d rate=%d, period_size=%d, period_cnt=%d, format=%d",
          __func__, p_lsm_ses->common.config.channels, p_lsm_ses->common.config.rate,
          p_lsm_ses->common.config.period_size, p_lsm_ses->common.config.period_count,
          p_lsm_ses->common.config.format);
    ATRACE_BEGIN("sthal:lsm: pcm_open");
    p_lsm_ses->pcm = pcm_open(p_ses->stdev->snd_card, p_lsm_ses->pcm_id,
                          PCM_IN, &p_lsm_ses->common.config);
    ATRACE_END();
    if (!p_lsm_ses->pcm) {
        ALOGE("%s: ERROR. pcm_open failed", __func__);
        status = -ENODEV;
        goto sm_error;
    }
    if (!pcm_is_ready(p_lsm_ses->pcm)) {
        ALOGE("%s: ERROR. pcm_is_ready failed err=%s", __func__,
              pcm_get_error(p_lsm_ses->pcm));
        status = -ENODEV;
        goto sm_error;
    }

    /* Although app_id is valid only for non-topology solution. The driver
       opens DSP LSM session with this ioctl cmd */
    ses_data.app_id = LSM_VOICE_WAKEUP_APP_ID_V2;
    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_session_data");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_SESSION_DATA, &ses_data);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_SET_SESSION_DATA failed status %d",
              __func__, status);
        goto sm_error;
    }

    if (p_lsm_ses->use_topology_intf) {
        param_info.param_type = LSM_REG_SND_MODEL;
        param_info.module_id = v_info->params[p_ses->exec_mode][LOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[p_ses->exec_mode][LOAD_SOUND_MODEL].param_id;
        if (sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
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

        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
        ATRACE_END();
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
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_REG_SND_MODEL_V2, &lsm_sm_v2);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_REG_SND_MODEL_V2 failed, status=%d",
                  __func__, status);
            goto sm_error;
        }
    }

    /* Callback thread to wait on event detection */
    p_lsm_ses->exit_callback_thread = false;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&p_lsm_ses->callback_thread, &attr,
                        callback_thread_loop, p_lsm_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return 0;

sm_error:
    platform_ape_free_pcm_device_id(p_ses->stdev->platform, p_lsm_ses->pcm_id);
    if (p_lsm_ses->pcm) {
        pcm_close(p_lsm_ses->pcm);
        p_lsm_ses->pcm = NULL;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__,
        p_lsm_ses->common.sm_handle, status);
    return status;
}

static int ape_dereg_sm(st_hw_session_t *p_ses, bool capture_requested)
{
    int status = 0, buf_en;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);

    if (!p_lsm_ses->pcm) {
        ALOGV("%s: pcm NULL", __func__);
        return status;
    }

    /* Exit the callback thread waiting on event detection */
    request_exit_callback_thread(p_lsm_ses);

    pthread_join(p_lsm_ses->callback_thread, (void **) NULL);

    if (capture_requested) {
        buf_en = 0;
        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_lab_control");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
        ATRACE_END();
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d",
                __func__, status);

        if (p_lsm_ses->lab_buffers_allocated) {
            /* Deallocate lab buffes allocated during start_recognition */
            deallocate_lab_buffers_ape(p_lsm_ses);
        }
    }

    if (p_lsm_ses->use_topology_intf) {
        /* Topology based */
        param_info.param_type = LSM_DEREG_SND_MODEL;
        param_info.module_id = v_info->params[p_ses->exec_mode][UNLOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[p_ses->exec_mode][UNLOAD_SOUND_MODEL].param_id;
        param_info.param_size = 0;

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size =
            lsm_params.num_params * sizeof(struct lsm_params_info);

        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS,
           &lsm_params);
        ATRACE_END();

        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->dereg status=%d",
                  __func__, status);
    } else {
        /* Non topology based */
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_DEREG_SND_MODEL);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_DEREG_SND_MODEL failed", __func__);
    }

    ATRACE_BEGIN("sthal:lsm: pcm_close");
    pcm_close(p_lsm_ses->pcm);
    ATRACE_END();
    p_lsm_ses->pcm = NULL;
    platform_ape_free_pcm_device_id(p_ses->stdev->platform, p_lsm_ses->pcm_id);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int ape_reg_sm_params(st_hw_session_t* p_ses, unsigned int recognition_mode,
    bool capture_requested, struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data)
{
    int status = 0, buf_en = 0, idx = 0;
    st_hw_session_lsm_t *p_lsm_ses =
        (st_hw_session_lsm_t*)p_ses;
    int retry_num = 0, offset = 0;
    uint32_t data_payload_size, lsm_param_payload_size, custom_payload_size;
    uint32_t data_payload_addr_lsw = 0, data_payload_addr_msw = 0, mem_map_handle = 0;
    unsigned char *custom_payload = NULL;
    struct st_vendor_info *v_info = p_lsm_ses->common.vendor_uuid_info;
    struct snd_lsm_detection_params params;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info[LSM_SM_PARAMS_INFO_IDX];
    struct lsm_params_info *cfl_params;
    struct lsm_params_info *op_params;
    struct lsm_params_info *cus_params;
    struct lsm_params_info *poll_en_params;
    struct snd_lsm_detect_mode det_mode;
    st_lsm_poll_enable_t poll_enable;
    struct lsm_param_custom_config custom_conf_params;
    int param_tag_tracker;
    st_exec_mode_t exec_mode = p_ses->exec_mode;
    struct sound_trigger_phrase_sound_model *phrase_sm =
        (struct sound_trigger_phrase_sound_model *)sm_data;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }
    ape_enable_use_case(true, p_ses);

    status = lsm_set_port(p_lsm_ses);
    if (status) {
        ALOGE("%s: ERROR. set port failed, returned status %d",
              __func__, status);
        goto error_exit;
    }

    if (sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        ALOGV("%s: num_phrases=%d, id=%d", __func__,
               rc_config->num_phrases, rc_config->phrases[0].id);
        status = get_confidence_levels_payload(phrase_sm, p_ses, rc_config);
        if (status) {
            ALOGE("%s: failed to get conf levels", __func__);
            return -EINVAL;
        }
    }

    ATRACE_BEGIN("sthal:lsm: pcm_start");
    status = pcm_start(p_lsm_ses->pcm);
    while (status && (retry_num < SOUND_TRIGGER_PCM_MAX_RETRY)) {
        usleep(SOUND_TRIGGER_PCM_SLEEP_WAIT);
        retry_num++;
        ALOGI("%s: pcm_start retrying..status %d errno %d, retry cnt %d",
              __func__, status, errno, retry_num);
        status = pcm_start(p_lsm_ses->pcm);
    }
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. pcm_start failed, returned status %d",
              __func__, status);
        goto error_exit;
    }

    /* SVA doesn't support per keyword recogntion mode.
       Use the per soundmodel recognition mode */
    if (recognition_mode & RECOGNITION_MODE_VOICE_TRIGGER){
        det_mode.mode= LSM_MODE_KEYWORD_ONLY_DETECTION;
        if (recognition_mode & RECOGNITION_MODE_USER_IDENTIFICATION)
            det_mode.mode = LSM_MODE_USER_KEYWORD_DETECTION;
    } else {
        ALOGE("%s: Unknown recognition mode %d", __func__, recognition_mode);
        goto error_exit;
    }

    if (p_lsm_ses->use_topology_intf) {
        lsm_params.num_params = 0;
        param_tag_tracker = v_info->param_tag_tracker[exec_mode];
        /* Topology based:
           if smlib is not present, pass only the opaque data as custom params a
           ignore sending all other params */
        if (v_info->smlib_handle) {
            det_mode.detect_failure = p_ses->stdev->detect_failure;
            ALOGV("%s: dm %d df %d lab %d", __func__,
                   det_mode.mode, det_mode.detect_failure, capture_requested);
            if (param_tag_tracker & PARAM_CONFIDENCE_LEVELS_BIT) {
                /* fill confidence level params */
                cfl_params = &param_info[idx++];
                cfl_params->param_type = LSM_MIN_CONFIDENCE_LEVELS;
                cfl_params->module_id = v_info->params[exec_mode][CONFIDENCE_LEVELS].module_id;
                cfl_params->param_id = v_info->params[exec_mode][CONFIDENCE_LEVELS].param_id;
                cfl_params->param_size = p_ses->num_conf_levels;
                cfl_params->param_data = p_ses->conf_levels;
                lsm_params.num_params++;
                {
                    unsigned int i;
                    ALOGV("%s: ncl %d", __func__, cfl_params->param_size);
                    for (i = 0; i < cfl_params->param_size; i++) {
                        ALOGV("%s: conf_levels[%d] = %d", __func__,
                                     i, cfl_params->param_data[i]);
                    }
                }
            }

            if (param_tag_tracker & PARAM_OPERATION_MODE_BIT) {
                /* fill operation mode params */
                op_params = &param_info[idx++];
                op_params->param_type = LSM_OPERATION_MODE;
                op_params->module_id = v_info->params[exec_mode][OPERATION_MODE].module_id;
                op_params->param_id = v_info->params[exec_mode][OPERATION_MODE].param_id;
                op_params->param_size = sizeof(det_mode);
                op_params->param_data = (unsigned char *)&det_mode;
                lsm_params.num_params++;
            }

            if (param_tag_tracker & PARAM_POLLING_ENABLE_BIT) {
                poll_en_params = &param_info[idx];
                if (fill_lsm_poll_enable_params(v_info, exec_mode, &poll_enable,
                                                poll_en_params)) {
                    idx++;
                    lsm_params.num_params++;
                }
            }
        }
        if (rc_config->data_size) {
            /* fill opaque data as custom params */
            cus_params = &param_info[idx++];
            cus_params->param_type = LSM_CUSTOM_PARAMS;

            if (param_tag_tracker & PARAM_CUSTOM_CONFIG_BIT) {
                /*
                 * Fill module_id and param_id obtained from platform info xml. Also fill custom_config set_param.
                 * Package this with recognition config opaque data in the required format and send it.
                 * data_payload_size = size of payload data + size of opaque data.
                 * p_size = size of opaque data.
                 */
                lsm_param_payload_size = sizeof(custom_conf_params) - sizeof(custom_conf_params.set_param);
                data_payload_size = lsm_param_payload_size + rc_config->data_size;
                fill_set_params_payload(&custom_conf_params.set_param, data_payload_size,
                         data_payload_addr_lsw, data_payload_addr_msw, mem_map_handle);

                custom_conf_params.common.module_id =
                                   v_info->params[exec_mode][CUSTOM_CONFIG].module_id;
                custom_conf_params.common.param_id =
                                   v_info->params[exec_mode][CUSTOM_CONFIG].param_id;
                custom_conf_params.common.p_size = rc_config->data_size;

                custom_payload_size = sizeof(struct lsm_param_custom_config) + rc_config->data_size;
                custom_payload = (unsigned char *)calloc(1, custom_payload_size);
                if (!custom_payload) {
                    ALOGE("%s: ERROR. Cannot allocate memory for custom_payload", __func__);
                    goto error_exit;
                }
                /* copy custom config params to payload */
                memcpy(custom_payload, &custom_conf_params, sizeof(struct lsm_param_custom_config));
                offset += sizeof(struct lsm_param_custom_config);
                /* copy opaque data from recognition config to payload */
                memcpy(custom_payload + offset, (char *)rc_config + rc_config->data_offset, rc_config->data_size);
            } else {
                /* Send opaque data as it is */
                custom_payload_size = rc_config->data_size;
                custom_payload = (unsigned char *)calloc(1, custom_payload_size);
                if (!custom_payload) {
                    ALOGE("%s: ERROR. Cannot allocate memory for custom_payload", __func__);
                    goto error_exit;
                }
                memcpy(custom_payload, (char *)rc_config + rc_config->data_offset, rc_config->data_size);
            }
            cus_params->param_size = custom_payload_size;
            cus_params->param_data = (unsigned char *)custom_payload;
            lsm_params.num_params++;
        }

        if (lsm_params.num_params) {
            lsm_params.params = (unsigned char*)param_info;
            lsm_params.data_size =
                lsm_params.num_params * sizeof(struct lsm_params_info);
            ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
            status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
            ATRACE_END();
            if (status) {
                ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->reg_params, status %d",
                      __func__, status);
                goto error_exit;
            }
        }
    } else {
        /* Non topology based */
        params.conf_level = p_ses->conf_levels;
        params.num_confidence_levels = p_ses->num_conf_levels;
        params.detect_failure = p_ses->stdev->detect_failure;
        params.detect_mode = det_mode.mode;

        {
            unsigned int i;
            ALOGV("%s: dm %d df %d ncl %d lab %d", __func__,
                  params.detect_mode, params.detect_failure,
                  params.num_confidence_levels,
                  capture_requested);
            for (i = 0; i < params.num_confidence_levels; i++) {
                ALOGV("%s: conf_levels[%d] = %d", __func__,
                                    i, params.conf_level[i]);
            }
        }

        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_PARAMS, &params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_PARAMS, status=%d", __func__, status);
            goto error_exit;
        }
    }

    if (capture_requested) {
        if (!p_lsm_ses->lab_buffers_allocated) {
            status = allocate_lab_buffers_ape(p_lsm_ses);
            if (status)
                goto error_exit;
        }
        buf_en = capture_requested ? 1 : 0;
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d",
                __func__, status);
            goto error_exit;
        }
    }

    return status;

error_exit:

    if (p_lsm_ses->lab_buffers_allocated)
        deallocate_lab_buffers_ape(p_lsm_ses);

    pcm_stop(p_lsm_ses->pcm);

    if (custom_payload)
        free(custom_payload);

    ALOGD("%s:[%d] Exit, status=%d", __func__,
        p_lsm_ses->common.sm_handle, status);

    return status;
}

static int ape_dereg_sm_params(st_hw_session_t* p_ses)
{
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (p_lsm_ses->pcm){
        ATRACE_BEGIN("sthal:lsm: pcm_stop");
        pcm_stop(p_lsm_ses->pcm);
        ATRACE_END();
    }
    ape_enable_use_case(false, p_ses);

    if (p_ses->conf_levels) {
        free(p_ses->conf_levels);
        p_ses->conf_levels = NULL;
    }

    return 0;
}

static int ape_start(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    p_lsm_ses->exit_lab_processing = false;

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_start");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_START);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_START failed, status=%d", __func__, status);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int ape_stop(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_stop");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_STOP);
    ATRACE_END();
    if (status)
        ALOGE("%s: ERROR. SNDDRV_LSM_STOP failed, status=%d", __func__, status);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int ape_stop_buffering(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter pcm %p", __func__, p_lsm_ses->common.sm_handle,
        p_lsm_ses->pcm);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_stop_lab");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_STOP_LAB);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_STOP_BUFFERING failed status %d", __func__,
           status);
    } else {
        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_pcm_ioctl_reset");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_PCM_IOCTL_RESET);
        ATRACE_END();
        if (status) ALOGE("%s: ERROR. SNDRV_PCM_IOCTL_RESET failed status %d", __func__,
               status);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

static int cpe_reg_sm(st_hw_session_t *p_ses, void* sm_data,
    sound_trigger_sound_model_type_t sm_type)
{
    int status = 0, fd;
    int app_type = ACDB_LSM_APP_TYPE_NO_TOPOLOGY;
    struct snd_lsm_sound_model_v2 lsm_sm_v2;
    struct sound_trigger_phrase_sound_model *phrase_sm = sm_data;
    struct sound_trigger_sound_model *common_sm= sm_data;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct pcm_config  *config;
    pthread_attr_t attr;
    bool use_topology = false;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;
    int sample_rate = SOUND_TRIGGER_SAMPLING_RATE_16000;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);

    if (v_info) {
        app_type = v_info->app_type;
        sample_rate = v_info->sample_rate;
        use_topology = true;
    }

    p_lsm_ses->pcm_id = platform_cpe_get_pcm_device_id(p_ses->stdev->platform,
        sample_rate, &(p_ses->use_case_idx));
    if (p_lsm_ses->pcm_id < 0)
        return -ENODEV;

    status = platform_stdev_send_calibration(p_ses->stdev->platform,
                                              AUDIO_DEVICE_NONE,
                                              p_ses->exec_mode,
                                              p_ses->vendor_uuid_info,
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
        status = platform_stdev_send_calibration(p_ses->stdev->platform,
                                                  p_ses->stdev->capture_device,
                                                  p_ses->exec_mode,
                                                  p_ses->vendor_uuid_info,
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
            write(fd, v_info->cpe_firmware_image,
               strlen(v_info->cpe_firmware_image));
            close(fd);
        }
        else {
            ALOGE("%s: ERROR. sysfs open to write firmware name failed : %s",
                  __func__, strerror(errno));
            goto sm_error;
        }
    }
    p_lsm_ses->use_topology_intf = use_topology;
    p_lsm_ses->common.config = stdev_cpe_pcm_config;
    p_lsm_ses->lab_config = stdev_cpe_pcm_config;
    config = &p_lsm_ses->lab_config;

    if (v_info) {
        p_lsm_ses->lab_config.rate = v_info->sample_rate;
        if (v_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {

            platform_stdev_check_and_update_pcm_config(p_ses->stdev->platform,
                                                       &p_lsm_ses->lab_config,
                                                       v_info,
                                                       p_ses->exec_mode);

            p_lsm_ses->lab_config.period_size =
               CALCULATE_PERIOD_SIZE(SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS,
                                     p_lsm_ses->lab_config.rate,
                                     SOUND_TRIGGER_CPE_PERIOD_COUNT, 32);

            ALOGV("%s: ftrt rate=%d format=%d", __func__,
                p_lsm_ses->lab_config.rate, p_lsm_ses->lab_config.format);

            if (CHECK_BIT(v_info->kw_capture_format, ADPCM))
               p_lsm_ses->common.config.period_size = ADPCM_MAX_OUT_SAMPLES_PER_FRAME;
        }
    }

    ALOGD("%s: opening pcm device=%d topo %d", __func__, p_lsm_ses->pcm_id,
          p_lsm_ses->use_topology_intf);
    ALOGD("%s: config: channels=%d rate=%d, period_size=%d, "
          "period_cnt=%d, format=%d", __func__, config->channels,
          config->rate, config->period_size,
          config->period_count, config->format);

    ATRACE_BEGIN("sthal:lsm: pcm_open");
    p_lsm_ses->pcm = pcm_open(p_ses->stdev->snd_card, p_lsm_ses->pcm_id,
                          PCM_IN, config);
    ATRACE_END();

    if (!p_lsm_ses->pcm) {
        ALOGE("%s: ERROR. pcm_open failed", __func__);
        status = -ENODEV;
        goto sm_error;
    }
    if (!pcm_is_ready(p_lsm_ses->pcm)) {
        ALOGE("%s: ERROR. pcm_is_ready failed err=%s", __func__,
              pcm_get_error(p_lsm_ses->pcm));
        status = -ENODEV;
        goto sm_error;
    }

    if (p_lsm_ses->use_topology_intf) {
        param_info.param_type = LSM_REG_SND_MODEL;
        param_info.module_id = v_info->params[p_ses->exec_mode][LOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[p_ses->exec_mode][LOAD_SOUND_MODEL].param_id;

        if (sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
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

        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS,
            &lsm_params);
        ATRACE_END();

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
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_REG_SND_MODEL_V2,
            &lsm_sm_v2);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_REG_SND_MODEL_V2 failed, status=%d",
                  __func__, status);
            goto sm_error;
        }
    }

    /* Callback thread to wait on event detection */
    p_lsm_ses->exit_callback_thread = false;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&p_lsm_ses->callback_thread, &attr,
                        callback_thread_loop, p_ses);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return 0;

sm_error:
    platform_cpe_free_pcm_device_id(p_ses->stdev->platform, p_lsm_ses->pcm_id);
    if (p_lsm_ses->pcm) {
        pcm_close(p_lsm_ses->pcm);
        p_lsm_ses->pcm = NULL;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int cpe_dereg_sm(st_hw_session_t *p_ses, bool capture_requested)
{
    int status = 0, buf_en;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);

    if (!p_lsm_ses->pcm) {
        ALOGV("%s: pcm NULL", __func__);
        return status;
    }

    /* Exit the callback thread waiting on event detection */
    request_exit_callback_thread(p_lsm_ses);

    /* note see note in ape_dereg_sm */
    pthread_join(p_lsm_ses->callback_thread, (void **) NULL);

    if (capture_requested) {
        buf_en = 0;
        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_lab_control");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_LAB_CONTROL, &buf_en);
        ATRACE_END();
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_LAB_CONTROL failed, status=%d",
            __func__, status);

        if (p_lsm_ses->lab_buffers_allocated) {
            /* Deallocate lab buffes allocated during start_recognition */
            deallocate_lab_buffers_cpe(p_lsm_ses);
        }
    }

    if (p_lsm_ses->use_topology_intf) {
        param_info.param_type = LSM_DEREG_SND_MODEL;
        param_info.module_id = v_info->params[p_ses->exec_mode][UNLOAD_SOUND_MODEL].module_id;
        param_info.param_id = v_info->params[p_ses->exec_mode][UNLOAD_SOUND_MODEL].param_id;
        param_info.param_size = 0;

        lsm_params.num_params = 1;
        lsm_params.params = (unsigned char*)&param_info;
        lsm_params.data_size =
            lsm_params.num_params * sizeof(struct lsm_params_info);

        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS,
           &lsm_params);
        ATRACE_END();

        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_SET_MODULE_PARAMS->dereg status=%d",
                  __func__, status);
    } else {
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_DEREG_SND_MODEL);
        if (status)
            ALOGE("%s: ERROR. SNDRV_LSM_DEREG_SND_MODEL failed %d", __func__, status);
    }

    ATRACE_BEGIN("sthal:lsm: pcm_close");
    pcm_close(p_lsm_ses->pcm);
    ATRACE_END();
    p_lsm_ses->pcm = NULL;
    platform_cpe_free_pcm_device_id(p_ses->stdev->platform, p_lsm_ses->pcm_id);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int cpe_reg_sm_params(st_hw_session_t* p_ses, unsigned int recognition_mode,
    bool capture_requested, struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data)
{
    int status = 0, idx = 0;
    int retry_num = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t*)p_ses;
    unsigned char temp_conf_levels[1];
    struct st_vendor_info *v_info = p_lsm_ses->common.vendor_uuid_info;
    struct snd_lsm_detection_params params;
    struct snd_lsm_module_params lsm_params;
    struct lsm_params_info param_info[3];
    struct lsm_params_info *cfl_params;
    struct lsm_params_info *op_params;
    struct lsm_params_info *cus_params;
    struct snd_lsm_detect_mode det_mode;
    int param_tag_tracker;
    st_exec_mode_t exec_mode = p_ses->exec_mode;
    struct sound_trigger_phrase_sound_model *phrase_sm =
       (struct sound_trigger_phrase_sound_model*)sm_data;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    status = lsm_set_port(p_lsm_ses);
    if (status) {
        ALOGE("%s: ERROR. set port failed, returned status %d",
              __func__, status);
        return status;
    }

    if (sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        ALOGV("%s: num_phrases=%d, id=%d", __func__,
               rc_config->num_phrases, rc_config->phrases[0].id);
        status = get_confidence_levels_payload(phrase_sm, p_ses, rc_config);
        if (status) {
            ALOGE("%s: failed to get conf levels", __func__);
            return -EINVAL;
        }
    }

    ATRACE_BEGIN("sthal:lsm: pcm_start");
    status = pcm_start(p_lsm_ses->pcm);
    while (status && (retry_num < SOUND_TRIGGER_PCM_MAX_RETRY)) {
        usleep(SOUND_TRIGGER_PCM_SLEEP_WAIT);
        retry_num++;
        ALOGI("%s: pcm_start retrying..status %d errno %d, retry cnt %d",
              __func__, status, errno, retry_num);
        status = pcm_start(p_lsm_ses->pcm);
    }
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. pcm_start failed, returned status %d",
              __func__, status);
        return status;
    }

    memset(&det_mode, 0, sizeof(struct snd_lsm_detect_mode));
    if (sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
        /* SVA doesn't support per keyword recogntion mode.
         * Use the per soundmodel recognition mode
         */
        if (recognition_mode & RECOGNITION_MODE_VOICE_TRIGGER){
            det_mode.mode= LSM_MODE_KEYWORD_ONLY_DETECTION;
            if (recognition_mode & RECOGNITION_MODE_USER_IDENTIFICATION)
                det_mode.mode = LSM_MODE_USER_KEYWORD_DETECTION;
        } else {
            ALOGE("%s: Unknown recognition mode %d", __func__, recognition_mode);
            goto err_exit_1;
        }
    }
    if (p_lsm_ses->use_topology_intf) {
        lsm_params.num_params = 0;
        param_tag_tracker = v_info->param_tag_tracker[exec_mode];

        /* If this is generic sound model or smlib is not present, pass only the
         * opaque data as custom params and ignore sending all other params.
         */
        if (v_info->smlib_handle &&
            (sm_type == SOUND_MODEL_TYPE_KEYPHRASE)) {
            det_mode.detect_failure = p_ses->stdev->detect_failure;
            ALOGV("%s: dm %d df %d lab %d", __func__,
                   det_mode.mode, det_mode.detect_failure, capture_requested);

            if ((param_tag_tracker & PARAM_CONFIDENCE_LEVELS_BIT) &&
                 p_ses->conf_levels) {
                /* fill confidence level params if present */
                cfl_params = &param_info[idx++];
                cfl_params->param_type = LSM_MIN_CONFIDENCE_LEVELS;
                cfl_params->module_id = v_info->params[exec_mode][CONFIDENCE_LEVELS].module_id;
                cfl_params->param_id = v_info->params[exec_mode][CONFIDENCE_LEVELS].param_id;
                cfl_params->param_size = p_ses->num_conf_levels;
                cfl_params->param_data = p_ses->conf_levels;
                lsm_params.num_params++;

                {
                    unsigned int i;
                    ALOGD("%s: ncl %d", __func__, cfl_params->param_size);
                    for (i = 0; i < cfl_params->param_size; i++) {
                        ALOGV("%s: conf_levels[%d] = %d", __func__,
                                            i, cfl_params->param_data[i]);
                    }
                }
            }

            if (param_tag_tracker & PARAM_OPERATION_MODE_BIT) {
                /* fill operation mode params */
                op_params = &param_info[idx++];
                op_params->param_type = LSM_OPERATION_MODE;
                op_params->module_id = v_info->params[exec_mode][OPERATION_MODE].module_id;
                op_params->param_id = v_info->params[exec_mode][OPERATION_MODE].param_id;
                op_params->param_size = sizeof(det_mode);
                op_params->param_data = (unsigned char *)&det_mode;
                lsm_params.num_params++;
            }
        }

        if (rc_config->data_size) {
            /* fill opaque data as custom params */
            cus_params = &param_info[idx++];
            cus_params->param_type = LSM_CUSTOM_PARAMS;
            cus_params->param_size = rc_config->data_size;
            cus_params->param_data = (unsigned char *) rc_config +
                rc_config->data_offset;
            lsm_params.num_params++;
        }

        if (lsm_params.num_params) {
            lsm_params.params = (unsigned char*)param_info;
            lsm_params.data_size =
                lsm_params.num_params * sizeof(struct lsm_params_info);

            ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_set_module_params");
            status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_MODULE_PARAMS, &lsm_params);
            ATRACE_END();
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
        params.detect_failure = p_ses->stdev->detect_failure;
        params.detect_mode = det_mode.mode;

        {
            unsigned int i;
            ALOGD("%s: dm %d df %d ncl %d lab %d", __func__,
               params.detect_mode, params.detect_failure,
               params.num_confidence_levels, capture_requested);
            for (i = 0; i < params.num_confidence_levels; i++) {
                ALOGV("%s: conf_levels[%d] = %d", __func__,
                                    i, params.conf_level[i]);
            }
        }

        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_SET_PARAMS, &params);
        if (status) {
            ALOGE("%s: ERROR. SNDRV_LSM_SET_PARAMS, status=%d", __func__, status);
            goto err_exit_1;
        }
    }

    if (capture_requested) {
        if (!p_lsm_ses->lab_buffers_allocated) {
            /* Instead of allocating and deallocating during frequent
               start/stop_recogntion, allocate once and dellocate during
               unload_sound_model */
            status = allocate_lab_buffers_cpe(p_lsm_ses);
            if (status)
                goto err_exit_1;
        }
        status = configure_lab(p_lsm_ses, capture_requested);
        if (status)
            goto err_exit_1;
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle, status);
    return 0;

err_exit_1:
    if (p_lsm_ses->lab_buffers_allocated)
        deallocate_lab_buffers_cpe(p_lsm_ses);

    pcm_stop(p_lsm_ses->pcm);

    return status;
}

static int cpe_dereg_sm_params(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (p_lsm_ses->pcm) {
        ATRACE_BEGIN("sthal:lsm: pcm_stop");
        status = pcm_stop(p_lsm_ses->pcm);
        ATRACE_END();
        if (status)
            ALOGE("%s: ERROR. pcm_stop failed: %s", __func__,
               pcm_get_error(p_lsm_ses->pcm));

    }

    if (p_ses->conf_levels) {
        free(p_ses->conf_levels);
        p_ses->conf_levels = NULL;
    }

    ALOGD("%s:[%d] Exit", __func__, p_lsm_ses->common.sm_handle);
    return status;
}

static int cpe_start(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_start");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_START);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_START failed, status=%d", __func__, status);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);

    SET_STATE(p_lsm_ses->common.state, SES_STARTED);
    return status;
}

static int cpe_stop(st_hw_session_t* p_ses)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter", __func__, p_lsm_ses->common.sm_handle);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_stop");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_STOP);
    ATRACE_END();
    if (status)
        ALOGE("%s: ERROR. SNDDRV_LSM_STOP failed, status=%d", __func__, status);

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_lsm_ses->common.sm_handle,
        status);
    return status;
}

static int cpe_stop_buffering(st_hw_session_t* p_ses, bool capture_requested)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses =
       (st_hw_session_lsm_t *)p_ses;

    ALOGD("%s:[%d] Enter pcm %p", __func__, p_lsm_ses->common.sm_handle,
        p_lsm_ses->pcm);
    if (!p_lsm_ses->pcm) {
        ALOGW("%s: pcm NULL", __func__);
        return status;
    }

    ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_stop_lab");
    status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_STOP_LAB);
    ATRACE_END();
    if (status) {
        ALOGE("%s: ERROR. SNDRV_LSM_STOP_BUFFERING failed status %d", __func__,
           status);
    } else {
        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_pcm_ioctl_reset");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_PCM_IOCTL_RESET);
        ATRACE_END();
        if (status)
            ALOGE("%s: ERROR. SNDRV_PCM_IOCTL_RESET failed status %d",
               __func__, status);
    }
    if (capture_requested) {
        pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
        pthread_cond_broadcast(&p_lsm_ses->pcm_out_buf_cond);
        pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
    }

    ALOGD("%s:[%d] Exit, status=%d", __func__, p_ses->sm_handle, status);
    return status;
}

static int route_reg_sm_ape(st_hw_session_t *p_ses,void *sm_data,
        sound_trigger_sound_model_type_t sm_type)
{
    return ape_reg_sm(p_ses, sm_data, sm_type);
}

static int route_reg_sm_params_ape(st_hw_session_t* p_ses,
    unsigned int recognition_mode, bool capture_requested,
    struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data)
{
    return ape_reg_sm_params(p_ses, recognition_mode, capture_requested,
        rc_config, sm_type, sm_data);
}

static int route_dereg_sm_params_ape(st_hw_session_t* p_ses)
{
    return ape_dereg_sm_params(p_ses);
}

static int route_dereg_sm_ape(st_hw_session_t* p_ses,
    bool capture_requested)
{
    int status = 0;

    status = ape_dereg_sm(p_ses, capture_requested);

    return status;
}

static int route_start_ape(st_hw_session_t* p_ses)
{

    return ape_start(p_ses);
}

static int route_stop_ape(st_hw_session_t* p_ses)
{

    return ape_stop(p_ses);
}

static int route_stop_buffering_ape(st_hw_session_t* p_ses, bool capture_requested __unused)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;
    struct timespec tspec;

    p_lsm_ses->exit_lab_processing = true;
    pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);

    /* wait for buffering thread to exit */
    while (p_lsm_ses->lab_processing_active) {
        clock_gettime(CLOCK_REALTIME, &tspec);
        tspec.tv_sec += ST_READ_WAIT_TIME_OUT_SEC;
        ALOGV("%s: waiting on exit cond", __func__);
        status = pthread_cond_timedwait(&(p_lsm_ses->pcm_out_buf_cond),
            &p_lsm_ses->pcm_out_buf_lock, &tspec);
        ALOGV("%s: done waiting on exit cond", __func__);
        if (status) {
            ALOGE("%s: ERROR. wait timed out, ret %d", __func__, status);
            status = -status;
            break;
        }
    }
    pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);

    return status;
}

static int route_set_device_ape(st_hw_session_t *p_ses,
                                bool enable)
{
    return sound_trigger_set_device(p_ses, enable);
}

static int route_read_pcm_ape(st_hw_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    ATRACE_BEGIN("sthal:lsm: read_pcm_data");
    status = read_pcm_data(p_lsm_ses, buf, bytes);
    ATRACE_END();

    return status;
}

static void route_audio_capture_ape(st_hw_session_t *p_ses)
{
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    ATRACE_BEGIN("sthal:lsm: process_raw_lab_data_ape");
    process_raw_lab_data_ape(p_lsm_ses);
    ATRACE_END();
}

int route_send_custom_chmix_coeff_ape(st_hw_session_t *p_ses, char *str)
{
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    return platform_stdev_send_custom_channel_mixing_coefficients(p_ses->stdev->platform,
                                                                  p_ses->vendor_uuid_info,
                                                                  p_lsm_ses->pcm_id,
                                                                  str);
}

static int route_reg_sm_cpe(st_hw_session_t *p_ses, void* sm_data,
    sound_trigger_sound_model_type_t sm_type)
{
    return cpe_reg_sm(p_ses, sm_data, sm_type);
}

static int route_reg_sm_params_cpe(st_hw_session_t* p_ses,
    unsigned int recognition_mode, bool capture_requested,
    struct sound_trigger_recognition_config *rc_config,
    sound_trigger_sound_model_type_t sm_type, void *sm_data)
{
    return cpe_reg_sm_params(p_ses, recognition_mode, capture_requested,
        rc_config, sm_type, sm_data);
}

static int route_dereg_sm_params_cpe(st_hw_session_t* p_ses)
{
    return cpe_dereg_sm_params(p_ses);
}

static int route_dereg_sm_cpe(st_hw_session_t* p_ses, bool capture_requested)
{
    return cpe_dereg_sm(p_ses, capture_requested);
}

static int route_start_cpe(st_hw_session_t* p_ses)
{
    return cpe_start(p_ses);
}

static int route_restart(st_hw_session_t* p_ses __unused, unsigned int recognition_mode __unused,
   bool capture_requested __unused, struct sound_trigger_recognition_config *rc_config __unused,
   sound_trigger_sound_model_type_t sm_type __unused, void *sm_data __unused)
{
    /*
     * this is a noop, in the case of LSM if session is already
     * started there is no need to stop/start even after detection
     */
    return 0;
}

static int route_stop_cpe(st_hw_session_t* p_ses)
{
    return cpe_stop(p_ses);
}

static int route_stop_buffering_cpe(st_hw_session_t* p_ses,
    bool capture_requested)
{
    int status = 0;

    if (!CHECK_STATE(p_ses->state, SES_BUFFERING)) {
        return status;
    }
    check_and_exit_lab(p_ses);
    status = cpe_stop_buffering(p_ses, capture_requested);

    CLEAR_STATE(p_ses->state, SES_BUFFERING);
    return status;
}

static int route_set_device_cpe(st_hw_session_t *p_ses,
                                bool enable)
{
    return sound_trigger_set_device(p_ses, enable);
}

static int route_read_pcm_cpe(st_hw_session_t *p_ses,
                              unsigned char *buf,
                              unsigned int bytes)
{
    int status = 0;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
    if (p_lsm_ses->exit_lab_processing) {
        ALOGE("%s: No active buffering", __func__);
        pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);
        return -EIO;
    }
    pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);

    status = read_pcm_data(p_lsm_ses, buf, bytes);

    return status;
}

static void route_audio_capture_cpe(st_hw_session_t *p_ses)
{
    SET_STATE(p_ses->state, SES_BUFFERING);
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    pthread_mutex_lock(&p_lsm_ses->pcm_out_buf_lock);
    p_lsm_ses->exit_lab_processing = false;
    pthread_mutex_unlock(&p_lsm_ses->pcm_out_buf_lock);

    if (p_ses->vendor_uuid_info &&
        p_ses->vendor_uuid_info->kw_transfer_mode == FTRT_TRANSFER_MODE) {
        process_packetized_lab_data(p_lsm_ses);
    } else {
        process_raw_lab_data_cpe(p_lsm_ses);
    }
}

int route_send_custom_chmix_coeff_cpe(st_hw_session_t *p_ses __unused, char *str __unused)
{
    /* custom chmix coeff not supported for CPE */
    return 0;
}

static int route_disable_device(st_hw_session_t *p_ses __unused, bool setting_device __unused)
{
    ALOGV("%s: Unsupported", __func__);
    return 0;
}

static int route_enable_device(st_hw_session_t *p_ses __unused, bool setting_device __unused)
{
    ALOGV("%s: Unsupported", __func__);
    return 0;
}

static void request_exit_callback_thread(st_hw_session_lsm_t *p_lsm_ses)
{
    int status;
    int wait_result;
    struct timespec timeout;

    pthread_mutex_lock(&p_lsm_ses->callback_thread_lock);
    p_lsm_ses->exit_callback_thread = true;
    for (int i = 0; i < LSM_ABORT_RETRY_COUNT; i++) {
        ATRACE_BEGIN("sthal:lsm: pcm_ioctl sndrv_lsm_abort_event");
        status = pcm_ioctl(p_lsm_ses->pcm, SNDRV_LSM_ABORT_EVENT);
        ATRACE_END();
        GET_WAIT_TIMESPEC(timeout, LSM_ABORT_WAIT_TIMEOUT_NS);
        wait_result = pthread_cond_timedwait(&p_lsm_ses->callback_thread_cond,
                                                 &p_lsm_ses->callback_thread_lock, &timeout);
        if (wait_result == ETIMEDOUT) {
            ALOGW("%s: wait for callback thread exit timeout, retry...", __func__);
            continue;
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&p_lsm_ses->callback_thread_lock);

    if (status)
        ALOGE("%s: ERROR. SNDRV_LSM_ABORT_EVENT failed, status=%d", __func__,
              status);
}

int st_hw_sess_lsm_init(st_hw_session_t *const p_ses,
    hw_ses_event_callback_t cb, void* cookie, st_exec_mode_t exec_mode,
    struct st_vendor_info* v_info, sound_model_handle_t sm_handle,
    sound_trigger_device_t *stdev)
{
    int status = 0;
    pthread_condattr_t c_attr;
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    p_ses->exec_mode = exec_mode;
    if (ST_EXEC_MODE_CPE == exec_mode) {
        ALOGD("%s: CPE mode", __func__);
        p_ses->fptrs = &cpe_fptrs;
    } else if (ST_EXEC_MODE_ADSP == exec_mode){
        ALOGD("%s: APE mode", __func__);
        p_ses->fptrs = &ape_fptrs;
    }

    p_ses->callback_to_st_session = cb;
    p_ses->cookie = cookie;
    p_ses->vendor_uuid_info = v_info;
    p_ses->sm_handle = sm_handle;
    p_ses->stdev = stdev;

    p_lsm_ses->exit_lab_processing = false;
    p_lsm_ses->lab_processing_active = false;

    pthread_condattr_init(&c_attr);
    pthread_condattr_setclock(&c_attr, CLOCK_MONOTONIC);
    pthread_cond_init(&p_lsm_ses->callback_thread_cond, &c_attr);
    pthread_mutex_init(&p_lsm_ses->callback_thread_lock, NULL);
    return status;

}

void st_hw_sess_lsm_deinit(st_hw_session_t *const p_ses __unused)
{
    st_hw_session_lsm_t *p_lsm_ses = (st_hw_session_lsm_t *)p_ses;

    pthread_cond_destroy(&p_lsm_ses->callback_thread_cond);
    pthread_mutex_destroy(&p_lsm_ses->callback_thread_lock);
}
