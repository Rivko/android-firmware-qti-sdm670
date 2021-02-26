/* st_hw_extn_intf.c
 *
 * This library contains the API that are add ons to
 * sound trigger interface.

 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "sound_trigger_hw_ext_intf"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <errno.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <hardware/sound_trigger.h>

#include "st_common_defs.h"
#include "sound_trigger_platform.h"
#include "sound_trigger_hw.h"
#include "st_session.h"
#include "st_hw_common.h"
#include "sound_trigger_prop_intf.h"

static size_t get_frame_size(struct pcm_config config)
{
    size_t chan_samp_sz;
    int format = config.format;

    chan_samp_sz = pcm_format_to_bits(format) >> 3;

    return config.channels * chan_samp_sz;
}

/* Add set of functions exposed via fptrs here */

static int sthw_extn_set_parameters(const struct sound_trigger_hw_device *dev,
                           sound_model_handle_t sound_model_handle,
                           const char *kvpairs)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    struct str_parms *parms;
    char value[32];
    int ret = 0, err, pause, bad_mic_channel_index;
    bool backend_cfg_change = false;
    audio_devices_t ec_ref_dev;

    ALOGD("%s:[%d] Enter kv pairs %s", __func__, sound_model_handle, kvpairs);

    if (!stdev) {
        ALOGE("%s: Invalid sound trigger device passed", __func__);
        return -EINVAL;
    }

    pthread_mutex_lock(&stdev->lock);
    parms = str_parms_create_str(kvpairs);
    if (!parms) {
        ALOGD("%s: str_parms returned query NULL", __func__);
        goto exit;
    }

    /* process global set parameters if sound model handle is sent as 0 */
    if (sound_model_handle == 0) {
        err = str_parms_get_int(parms, QSTHW_PARAMETER_BAD_MIC_CHANNEL_INDEX, &bad_mic_channel_index);
        if (err >= 0) {
            str_parms_del(parms, QSTHW_PARAMETER_BAD_MIC_CHANNEL_INDEX);
            ALOGV("%s: bad mic channel index 0x%x", __func__, bad_mic_channel_index);
            ret = platform_stdev_update_bad_mic_channel_index(stdev->platform,
                                                              bad_mic_channel_index,
                                                              &backend_cfg_change);
            if (ret)
                goto exit;
            /*
             * re route to new device if backend cfg change due to bad mic index update
             * Set session as NULL to stop and start all the currently running sessions
             */
            if (backend_cfg_change) {
                stop_other_sessions(stdev, NULL);
                start_other_sessions(stdev, NULL);
            }
        }

        err = str_parms_get_int(parms, QSTHW_PARAMETER_EC_REF_DEVICE, (int *)&ec_ref_dev);
        if (err >= 0) {
            str_parms_del(parms, QSTHW_PARAMETER_EC_REF_DEVICE);
            ALOGV("%s: ec ref device 0x%x", __func__, ec_ref_dev);
            stdev->ec_ref_dev = ec_ref_dev;
        }

        goto exit;
    }

    /* process session specific set parameters */
    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        ret = -EINVAL;
        goto exit;
    }

    err = str_parms_get_str(parms, QSTHW_PARAMETER_CUSTOM_CHANNEL_MIXING, value, sizeof(value));
    if (err >= 0) {
        str_parms_del(parms, QSTHW_PARAMETER_CUSTOM_CHANNEL_MIXING);
            ALOGV("%s: [%d] set custom channel mixing coeff", __func__, sound_model_handle);
            ret = st_session_send_custom_chmix_coeff(st_session, value);
    }

    err = str_parms_get_int(parms, QSTHW_PARAMETER_SESSION_PAUSE, &pause);
    if (err >= 0) {
        str_parms_del(parms, QSTHW_PARAMETER_SESSION_PAUSE);
        if (pause) {
            ALOGV("%s: [%d] pause session", __func__, sound_model_handle);
            st_session_pause(st_session);
        } else {
            ALOGV("%s: [%d] resume session", __func__, sound_model_handle);
            st_session_resume(st_session);
        }
    }

exit:
    if (parms)
        str_parms_destroy(parms);
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static size_t sthw_extn_get_buffer_size(const struct sound_trigger_hw_device *dev,
                                        sound_model_handle_t sound_model_handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    size_t buf_size = 0;
    struct pcm_config config;

    if (!stdev) {
        ALOGE("%s: Invalid sound trigger device passed", __func__);
        return -EINVAL;
    }

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        return -EINVAL;
    }

    if (st_session_get_config(st_session, &config)) {
        ALOGE("%s: Could not get config for session %d", __func__, sound_model_handle);
        return -EINVAL;
    }

    buf_size = config.period_size * get_frame_size(config);
    return buf_size;
}

static int sthw_extn_read_buffer(const struct sound_trigger_hw_device *dev,
                                 sound_model_handle_t sound_model_handle,
                                 unsigned char *buf,
                                 size_t bytes)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    struct pcm_config config;
    int ret = 0;
    size_t bytes_read = 0;

    if (!stdev || !buf || !bytes) {
        ALOGE("%s: Invalid params", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&stdev->lock);

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        ret = -EINVAL;
        pthread_mutex_unlock(&stdev->lock);
        goto exit;
    }

    st_session_get_config(st_session, &config);
    pthread_mutex_unlock(&stdev->lock);
    ret = st_session_read_pcm(st_session, buf,
                              bytes, &bytes_read);
    if (ret != 0) {
        memset(buf, 0, bytes);
        ALOGE("%s: read failed status %d - sleep for buffer duration", __func__, ret);
        usleep((bytes * 1000000) /
               (get_frame_size(config) * config.rate));
    }
exit:
    return ret;
}

static int sthw_extn_stop_buffering(const struct sound_trigger_hw_device *dev,
                                    sound_model_handle_t sound_model_handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    int ret = 0;

    if (!stdev) {
        ALOGE("%s: Invalid sound trigger device passed", __func__);
        return -EINVAL;
    }

    pthread_mutex_lock(&stdev->lock);
    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        ret = -EINVAL;
        goto exit;
    }

    st_session_stop_lab(st_session);
exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}


static int sthw_extn_get_param_data(const struct sound_trigger_hw_device *dev,
                           sound_model_handle_t sound_model_handle,
                           const char *param,
                           void *payload,
                           size_t payload_size,
                           size_t *param_data_size)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    int status = 0;

    if (!stdev) {
        ALOGE("%s: Invalid sound trigger device passed", __func__);
        goto exit;
    }

    pthread_mutex_lock(&stdev->lock);

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__, sound_model_handle);
        status = -EINVAL;
        goto exit;
    }

    status = st_session_get_param_data(st_session, param, payload, payload_size,
                                       param_data_size);
    if (status)
        ALOGE("%s: ERROR. fetching get param data %d", __func__, status);

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGV("%s: exit: return - %d", __func__, status);
    return status;
}

void sthw_extn_get_fptrs(sthw_extn_fptrs_t *fptrs)
{
    if (!fptrs) {
        ALOGE("%s: ERROR Invalid argument", __func__);
        return;
    }

    fptrs->set_parameters = sthw_extn_set_parameters;
    fptrs->get_buffer_size = sthw_extn_get_buffer_size;
    fptrs->read_buffer = sthw_extn_read_buffer;
    fptrs->stop_buffering = sthw_extn_stop_buffering;
    fptrs->get_param_data = sthw_extn_get_param_data;
}
