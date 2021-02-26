/*
 * This file contains the platform specific functionality.
 *
 * Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "sound_trigger_platform"
#define ATRACE_TAG (ATRACE_TAG_HAL)
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0
#ifdef LINUX_ENABLED
/*temporay change since defination is not found for msmcal-hwdep.h*/
typedef unsigned int __u32;
typedef unsigned char __u8;
#endif

#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include <dlfcn.h>
#include <expat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sound/msmcal-hwdep.h>
#include "sound_trigger_platform.h"
#include "sound_trigger_hw.h"
#include "st_hw_session_gcs.h" /* for gcs_init/deinit */
#include "st_hw_session_pcm.h" /* for pcm_init/deinit */

/* platform info keys */
#define ST_PARAM_KEY_MAX_CPE_SESSIONS "max_cpe_sessions"
#define ST_PARAM_KEY_MAX_APE_SESSIONS "max_ape_sessions"
#define ST_PARAM_KEY_MAX_WDSP_SESSIONS "max_wdsp_sessions"
#define ST_PARAM_KEY_SW_MAD "sw_mad"
#define ST_PARAM_KEY_ENABLE_FAILURE_DETECTION "enable_failure_detection"
#define ST_PARAM_KEY_CPE_FE_TO_BE_FIXED "cpe_fe_to_be_fixed"
#define ST_PARAM_KEY_RX_CONCURRENCY_DISABLED "rx_concurrency_disabled"
#define ST_PARAM_KEY_RX_MAX_CONC_SESSIONS "rx_conc_max_st_ses"
#define ST_PARAM_KEY_CONCURRENT_CAPTURE "concurrent_capture"
#define ST_PARAM_KEY_CONCURRENT_VOICE_CALL "concurrent_voice_call"
#define ST_PARAM_KEY_CONCURRENT_VOIP_CALL "concurrent_voip_call"


#define ST_PARAM_KEY_FIRMWARE_IMAGE "firmware_image"
#define ST_PARAM_KEY_SM_VENDOR_UUID "vendor_uuid"
#define ST_PARAM_KEY_APP_TYPE "app_type"
#define ST_PARAM_KEY_LIBRARY "library"
#define ST_PARAM_KEY_MAX_CPE_PHRASES "max_cpe_phrases"
#define ST_PARAM_KEY_MAX_APE_USERS "max_ape_users"
#define ST_PARAM_KEY_MAX_APE_PHRASES "max_ape_phrases"
#define ST_PARAM_KEY_MAX_CPE_USERS "max_cpe_users"
#define ST_PARAM_KEY_SAMPLE_RATE "sample_rate"
#define ST_PARAM_KEY_BIT_WIDTH "bit_width"
#define ST_PARAM_KEY_CHANNEL_COUNT "channel_count"
#define ST_PARAM_KEY_ADM_CFG_PROFILE "adm_cfg_profile"
#define ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS "load_sound_model_ids"
#define ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS "unload_sound_model_ids"
#define ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS "confidence_levels_ids"
#define ST_PARAM_KEY_OPERATION_MODE_IDS "operation_mode_ids"
#define ST_PARAM_KEY_POLLING_ENABLE_IDS "polling_enable_ids"
#define ST_PARAM_KEY_CUSTOM_CONFIG_IDS "custom_config_ids"
#define ST_PARAM_KEY_DETECTION_EVENT_IDS "detection_event_ids"
#define ST_PARAM_KEY_READ_REQ_IDS "read_cmd_ids"
#define ST_PARAM_KEY_READ_RSP_IDS "read_rsp_ids"
#define ST_PARAM_KEY_START_ENGINE_IDS "start_engine_ids"
#define ST_PARAM_KEY_RESTART_ENGINE_IDS "restart_engine_ids"
#define ST_PARAM_KEY_UID "uid"
#define ST_PARAM_KEY_ACDB_DEVICES "acdb_devices"
#define ST_PARAM_KEY_CAPTURE_KEYWORD "capture_keyword"
#define ST_PARAM_KEY_CLIENT_CAPTURE_READ_DELAY "client_capture_read_delay"
#define ST_PARAM_KEY_EXECUTION_TYPE "execution_type"
#define ST_PARAM_KEY_EVENT_TIMESTAMP_MODE "event_timestamp_mode"
#define ST_PARAM_KEY_BACKEND_PORT_NAME "backend_port_name"
#define ST_PARAM_KEY_BACKEND_DAI_NAME "backend_dai_name"
#define ST_PARAM_KEY_SND_CARD_NAME "snd_card_name"
#define ST_PARAM_KEY_FLUENCE_TYPE "fluence_type"
#define ST_PARAM_KEY_WDSP_FLUENCE_TYPE "wdsp_fluence_type"
#define ST_PARAM_KEY_SUPPORT_DEVICE_SWITCH "support_device_switch"
#define ST_PARAM_KEY_EXECUTION_MODE "execution_mode"
#define ST_PARAM_KEY_TRANSIT_TO_ADSP_ON_PLAYBACK "transit_to_adsp_on_playback"
#define ST_PARAM_KEY_TRANSIT_WAIT_TIME "transit_wait_time"
#define ST_PARAM_KEY_SPLIT_EC_REF_DATA "split_ec_ref_data"
#define ST_PARAM_KEY_EC_REF_CHANNEL_COUNT "ec_ref_channel_count"
#define ST_PARAM_KEY_IMPLEMENTER_UUID "implementer_uuid"

#define ST_PARAM_KEY_DEVICE_HANDSET_APE "DEVICE_HANDSET_MIC_APE"
#define ST_PARAM_KEY_DEVICE_HANDSET_CPE "DEVICE_HANDSET_MIC_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP "DEVICE_HANDSET_MIC_ECPP_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE "DEVICE_HANDSET_DMIC_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE "DEVICE_HANDSET_TMIC_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE "DEVICE_HANDSET_QMIC_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE "DEVICE_HANDSET_DMIC_APE"
#define ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE "DEVICE_HANDSET_TMIC_APE"
#define ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE "DEVICE_HANDSET_QMIC_APE"
#define ST_PARAM_KEY_DEVICE_HEADSET_CPE "DEVICE_HEADSET_MIC_CPE"
#define ST_PARAM_KEY_DEVICE_HANDSET_APE_PP "DEVICE_HANDSET_MIC_PP_APE"
#define ST_PARAM_KEY_DEVICE_HANDSET_6MIC_ARM "DEVICE_HANDSET_6MIC_ARM"
#define ST_PARAM_KEY_DEVICE_HANDSET_8MIC_ARM "DEVICE_HANDSET_8MIC_ARM"

#define ST_BACKEND_PORT_NAME_MAX_SIZE 25
#define ST_MAX_LENGTH_MIXER_CONTROL 128
#define ST_MAX_NUM_CHANNELS 4
#define ST_MAX_SND_CARD_NAME_LENGTH 100

typedef int  (*acdb_loader_init_v2_t)(const char *, const char *, int);
typedef void (*acdb_loader_deallocate_t)(void);
typedef void (*acdb_loader_send_listen_device_cal_t)(int, int, int, int);
typedef int  (*acdb_loader_send_listen_lsm_cal_t)(int, int, int, int);
typedef int  (*acdb_loader_get_calibration_t)(char *, int, void *);

struct st_device_index
st_device_name_idx[ST_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
   {
       {"DEVICE_HANDSET_MIC_APE", ST_DEVICE_HANDSET_MIC},
       {"DEVICE_HANDSET_DMIC_APE", ST_DEVICE_HANDSET_DMIC},
       {"DEVICE_HANDSET_TMIC_APE", ST_DEVICE_HANDSET_TMIC},
       {"DEVICE_HANDSET_QMIC_APE", ST_DEVICE_HANDSET_QMIC},
       {"DEVICE_HANDSET_MIC_PP_APE", ST_DEVICE_HANDSET_MIC_PP},
   },
   {
       {"DEVICE_HANDSET_MIC_CPE", ST_DEVICE_HANDSET_MIC},
       {"DEVICE_HANDSET_MIC_ECPP_CPE", ST_DEVICE_HANDSET_MIC_ECPP},
       {"DEVICE_HANDSET_DMIC_CPE", ST_DEVICE_HANDSET_DMIC},
       {"DEVICE_HANDSET_TMIC_CPE", ST_DEVICE_HANDSET_TMIC},
       {"DEVICE_HANDSET_QMIC_CPE", ST_DEVICE_HANDSET_QMIC},
       {"DEVICE_HEADSET_MIC_CPE", ST_DEVICE_HEADSET_MIC},
   },
   {
       {"DEVICE_HANDSET_6MIC_ARM", ST_DEVICE_HANDSET_6MIC},
       {"DEVICE_HANDSET_8MIC_ARM", ST_DEVICE_HANDSET_8MIC},
   },
};

static const char * const
st_device_table[ST_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
        /* ADSP SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-ape-handset-mic",
        [ST_DEVICE_HANDSET_DMIC] = "listen-ape-handset-dmic",
        [ST_DEVICE_HANDSET_TMIC] = "listen-ape-handset-tmic",
        [ST_DEVICE_HANDSET_QMIC] = "listen-ape-handset-qmic",
        [ST_DEVICE_HANDSET_MIC_PP] = "listen-ape-handset-mic-preproc",
    },
    {
        /* CPE SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-cpe-handset-mic",
        [ST_DEVICE_HANDSET_MIC_ECPP] = "listen-cpe-handset-mic-ecpp",
        [ST_DEVICE_HANDSET_DMIC] = "listen-cpe-handset-dmic",
        [ST_DEVICE_HANDSET_TMIC] = "listen-cpe-handset-tmic",
        [ST_DEVICE_HANDSET_QMIC] = "listen-cpe-handset-qmic",
        [ST_DEVICE_HEADSET_MIC] = "listen-cpe-headset-mic",
    },
    {
        /* ARM SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_6MIC] = "listen-handset-6mic",
        [ST_DEVICE_HANDSET_8MIC] = "listen-handset-8mic",
    },
};

/* ACDB IDs for each device for both CDSP and ADSP */
static int acdb_device_table[ST_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_APE_ACDB_ID,
      [ST_DEVICE_HANDSET_DMIC] = DEVICE_HANDSET_APE_ACDB_ID,
      [ST_DEVICE_HANDSET_TMIC] = DEVICE_HANDSET_APE_ACDB_ID,
      [ST_DEVICE_HANDSET_QMIC] = DEVICE_HANDSET_APE_ACDB_ID,
      [ST_DEVICE_HANDSET_MIC_PP] = DEVICE_HANDSET_APE_ACDB_ID,
    },
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HANDSET_MIC_ECPP] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HANDSET_DMIC] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HANDSET_TMIC] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HANDSET_QMIC] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HEADSET_MIC] = DEVICE_HEADSET_CPE_ACDB_ID,
    },
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_6MIC] = DEVICE_HANDSET_ARM_ACDB_ID,
      [ST_DEVICE_HANDSET_8MIC] = DEVICE_HANDSET_ARM_ACDB_ID,
    },
};

/*
 * backend type for each device for both ADSP and CPE.
 * The backend type is used to determine whether two st_devices
 * share common backend or not.
 */
static char *st_device_backend_table[ST_EXEC_MODE_MAX][ST_DEVICE_MAX];

/* Qualcomm Technologies Inc. vendorUuid */
static const sound_trigger_uuid_t qti_uuid =
    { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };

/* Platform xml tags */
typedef enum {
    TAG_ROOT,
    TAG_COMMON,
    TAG_ACDB,
    TAG_BACKEND_TYPE,
    TAG_SOUND_MODEL,
    TAG_GCS_USECASE,
    TAG_LSM_USECASE,
    TAG_ADM_CFG
} st_xml_tags_t;

typedef void (*st_xml_process_fn)(void *platform, const XML_Char **attr);
static void platform_stdev_process_kv_params(void *platform, const XML_Char **attr);

/* function pointers for xml tag info parsing */
static st_xml_process_fn process_table[] = {
    [TAG_ROOT] = NULL,
    [TAG_COMMON] = platform_stdev_process_kv_params,
    [TAG_ACDB] = platform_stdev_process_kv_params,
    [TAG_BACKEND_TYPE] = platform_stdev_process_kv_params,
    [TAG_SOUND_MODEL] = platform_stdev_process_kv_params,
    [TAG_GCS_USECASE] = platform_stdev_process_kv_params,
    [TAG_LSM_USECASE] = platform_stdev_process_kv_params,
    [TAG_ADM_CFG] = platform_stdev_process_kv_params
};

struct platform_data {
    int hwdep_fd;
    int prev_acdb_id;
    sound_trigger_device_t *stdev;
    void *acdb_handle;
    acdb_loader_send_listen_device_cal_t acdb_send_device_cal;
    acdb_loader_send_listen_lsm_cal_t acdb_send_lsm_cal;
    acdb_loader_get_calibration_t acdb_get_cal;
    acdb_loader_init_v2_t acdb_init;
    audio_hw_acdb_init_t audio_hw_acdb_init;
    acdb_loader_deallocate_t acdb_deinit;
    struct st_vendor_info *vendor_uuid_info;
    int param_tag_tracker;

    int xml_version;
    st_xml_tags_t st_xml_tag;
    struct str_parms *kvpairs;

    bool cpe_fe_to_be_fixed;
    char codec_version[15];

    char backend_port[ST_BACKEND_PORT_NAME_MAX_SIZE];
    st_codec_backend_cfg_t codec_backend_cfg;
    bool ec_ref_enabled;
    char ec_ref_mixer_path[ST_MAX_LENGTH_MIXER_CONTROL];
    int bad_mic_channel_index;

    int max_be_dai_names;
    struct st_be_dai_name_table *be_dai_name_table;
    char backend_dai_name[ST_BE_DAI_NAME_MAX_LENGTH];
    char snd_card_name[ST_SND_CARD_NAME_MAX_LENGTH];
};

static int load_smlib(struct st_vendor_info *sm_info, const char *name)
{
    int status = 0;

    sm_info->smlib_handle = dlopen(name, RTLD_NOW);
    if (!sm_info->smlib_handle) {
        ALOGE("%s: ERROR. %s", __func__, dlerror());
        return -ENODEV;
    }

    dlerror();
    DLSYM(sm_info->smlib_handle, sm_info->generate_st_phrase_recognition_event,
          generate_sound_trigger_phrase_recognition_event, status);
    if (status)
        goto cleanup;
    DLSYM(sm_info->smlib_handle, sm_info->generate_st_phrase_recognition_event_v2,
          generate_sound_trigger_phrase_recognition_event_v2, status);
    if (status)
        goto cleanup;
    DLSYM(sm_info->smlib_handle,
          sm_info->generate_st_recognition_config_payload,
          generate_sound_trigger_recognition_config_payload, status);
    if (status)
        goto cleanup;
    DLSYM(sm_info->smlib_handle,
          sm_info->generate_st_recognition_config_payload_v2,
          generate_sound_trigger_recognition_config_payload_v2, status);
    if (status)
        goto cleanup;
    DLSYM(sm_info->smlib_handle,
          sm_info->generate_st_phrase_recognition_event_v3,
          generate_sound_trigger_phrase_recognition_event_v3, status);
    /* Ignore error for this symbol */

    return 0;

cleanup:
    if (sm_info->smlib_handle) {
        dlclose(sm_info->smlib_handle);
        sm_info->smlib_handle = NULL;
    }
    return status;
}

static int load_acdb(struct platform_data *my_data)
{
    int status = 0;

    my_data->acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (!my_data->acdb_handle) {
        ALOGE("%s: ERROR. %s", __func__, dlerror());
        return -ENODEV;
    }

    dlerror();
    DLSYM(my_data->stdev->audio_hal_handle, my_data->audio_hw_acdb_init,
          acdb_init, status);
    if (status) {
        DLSYM(my_data->acdb_handle, my_data->acdb_init,
              acdb_loader_init_v2, status);
        if (status)
            goto cleanup;
    }

    DLSYM(my_data->acdb_handle, my_data->acdb_deinit,
          acdb_loader_deallocate_ACDB, status);
    if (status)
        goto cleanup;
    DLSYM(my_data->acdb_handle, my_data->acdb_send_device_cal,
          acdb_loader_send_listen_device_cal,
          status);
    if (status)
        goto cleanup;
    DLSYM(my_data->acdb_handle, my_data->acdb_send_lsm_cal,
          acdb_loader_send_listen_lsm_cal, status);
    if (status)
        goto cleanup;
    DLSYM(my_data->acdb_handle, my_data->acdb_get_cal,
          acdb_loader_get_calibration, status);
    if (status)
        goto cleanup;

    return 0;

cleanup:
    if (my_data->acdb_handle) {
        dlclose(my_data->acdb_handle);
        my_data->acdb_handle = NULL;
    }
    return status;
}

static int load_mulaw_decoder(sound_trigger_device_t *stdev)
{
    int status = 0;

    stdev->mulaw_dec_lib_handle = dlopen(LIB_MULAW_DECODER, RTLD_NOW);
    if (!stdev->mulaw_dec_lib_handle) {
        ALOGE("%s: ERROR. %s", __func__, dlerror());
        return -ENODEV;
    }

    dlerror();
    DLSYM(stdev->mulaw_dec_lib_handle, stdev->mulaw_dec_process,
          mulaw_dec_process, status);
    if (status) {
        dlclose(stdev->mulaw_dec_lib_handle);
        stdev->mulaw_dec_lib_handle = NULL;
    }
    return status;
}

static int load_adpcm_decoder(sound_trigger_device_t *stdev)
{
    int status = 0;

    stdev->adpcm_dec_lib_handle = dlopen(LIB_ADPCM_DECODER, RTLD_NOW);
    if (!stdev->adpcm_dec_lib_handle) {
        ALOGE("%s: ERROR. %s", __func__, dlerror());
        return -ENODEV;
    }

    dlerror();
    DLSYM(stdev->adpcm_dec_lib_handle, stdev->adpcm_dec_init,
          g722_init_decoder, status);
    if (status)
        goto cleanup;
    DLSYM(stdev->adpcm_dec_lib_handle, stdev->adpcm_dec_get_scratch_size,
          g722_dec_get_total_byte_size, status);
    if (status)
        goto cleanup;
    DLSYM(stdev->adpcm_dec_lib_handle, stdev->adpcm_dec_process,
          g722_dec_process, status);
    if (status)
        goto cleanup;

    return 0;

cleanup:
    if (stdev->adpcm_dec_lib_handle) {
        dlclose(stdev->adpcm_dec_lib_handle);
        stdev->adpcm_dec_lib_handle = NULL;
    }
    return status;
}

static int platform_stdev_set_acdb_id(void *userdata __unused, const char* device, int acdb_id)
{
    int i, j;
    int ret = 0;
    int dev_idx = ST_DEVICE_NONE;

    if (device == NULL) {
       ALOGE("%s: device name is NULL", __func__);
       ret = -ENODEV;
       goto done;
    }

    for (i = 0; i < ST_EXEC_MODE_MAX; i++) {
        for (j = 0; j < ST_DEVICE_MAX; j++) {
           if (strcmp(st_device_name_idx[i][j].name, device) == 0) {
               dev_idx = st_device_name_idx[i][j].index;
               break;
           }
        }
        if (dev_idx != ST_DEVICE_NONE)
            break;
    }
    if (dev_idx == ST_DEVICE_NONE) {
       ALOGE("%s: Could not find index for device name = %s",
               __func__, device);
       ret = -ENODEV;
       goto done;
    }

    acdb_device_table[i][dev_idx] = acdb_id;

done:
    return ret;

}

static const char *get_snd_card_name_for_acdb_loader(const char *snd_card_name)
{
    const char *acdb_card_name = NULL;
    char *substring = NULL;
    char string[ST_MAX_SND_CARD_NAME_LENGTH] = {0};
    int length = 0;

    if (snd_card_name == NULL)
        return NULL;

    /* Both tasha & tasha-lite uses tasha ACDB files
       simulate sound card name for tasha lite, so that
       ACDB module loads tasha ACDB files for tasha lite */
    if ((substring = strstr(snd_card_name, "tashalite"))) {
        ALOGD("%s: using tasha ACDB files for tasha-lite", __func__);
        length = substring - snd_card_name + 1;
        snprintf(string, length, "%s", snd_card_name);
        strlcat(string, "tasha-snd-card", sizeof(string));
        acdb_card_name = strdup(string);
        return acdb_card_name;
    }
    acdb_card_name = strdup(snd_card_name);
    return acdb_card_name;
}

static void set_default_backend_type()
{
    int i, j;

    /* backend type can be overridden from platform XML file */
    for (i = 0; i < ST_EXEC_MODE_MAX; i++) {
        for (j = 0; j < ST_DEVICE_MAX; j++) {
            if (j == ST_DEVICE_NONE)
                st_device_backend_table[i][j] = strdup("NONE");
            else
                st_device_backend_table[i][j] = strdup("BACKEND_DEFAULT");
        }
    }
};

static int set_backend_type(void *userdata __unused, const char* device, char *type)
{
    int i, j;
    int ret = 0;
    int dev_idx = ST_DEVICE_NONE;

    if (device == NULL) {
       ALOGE("%s: device name is NULL", __func__);
       ret = -ENODEV;
       goto done;
    }

    for (i = 0; i < ST_EXEC_MODE_MAX; i++) {
        for (j = 0; j < ST_DEVICE_MAX; j++) {
           if (strcmp(st_device_name_idx[i][j].name, device) == 0) {
               dev_idx = st_device_name_idx[i][j].index;
               break;
           }
        }
        if (dev_idx != ST_DEVICE_NONE)
            break;
    }
    if (dev_idx == ST_DEVICE_NONE) {
       ALOGE("%s: Could not find index for device name = %s",
               __func__, device);
       ret = -ENODEV;
       goto done;
    }

    if (st_device_backend_table[i][dev_idx])
        free(st_device_backend_table[i][dev_idx]);

    st_device_backend_table[i][dev_idx] = strdup(type);

done:
    return ret;
}

static void platform_stdev_set_default_config(struct platform_data *platform)
{
    sound_trigger_device_t *stdev = platform->stdev;

    stdev->run_on_ape =  true;
    stdev->max_ape_sessions = 1;
    stdev->avail_ape_phrases = 0;
    stdev->avail_ape_users = 0;
    stdev->max_cpe_sessions = 1;
    stdev->max_wdsp_sessions = 1;
    stdev->avail_cpe_phrases  = 0;
    stdev->avail_cpe_users = 0;
    stdev->max_arm_sessions = 1;
    stdev->rx_conc_max_st_ses = UINT_MAX;
    stdev->support_dev_switch = false;
    stdev->transit_to_adsp_on_playback = false;
    stdev->transit_wait_time = DEFAULT_TRANSIT_WAIT_TIME_SEC;
    stdev->ssr_offline_received = false;
    stdev->conc_capture = false;
    stdev->conc_voice_call = false;
    stdev->conc_voip_call = false;

    platform->cpe_fe_to_be_fixed = true;
    platform->codec_backend_cfg.sample_rate = SOUND_TRIGGER_SAMPLING_RATE_48000;
    platform->codec_backend_cfg.format = PCM_FORMAT_S16_LE;
    platform->codec_backend_cfg.channel_count = SOUND_TRIGGER_CHANNEL_MODE_MONO;
    platform->bad_mic_channel_index = 0;
    platform->ec_ref_enabled = false;
    platform->be_dai_name_table = NULL;
    platform->max_be_dai_names = 0;

    set_default_backend_type();
}

static void platform_stdev_set_config(void *userdata, const char* param, const char* value)
{
    struct platform_data *platform = userdata;
    sound_trigger_device_t *stdev;

    if (!platform) {
        ALOGE("%s: platform data NULL", __func__);
        return;
    }
    if (!platform->stdev) {
        ALOGE("%s: platform stdev data NULL", __func__);
        return;
    }

    stdev = platform->stdev;
    if (!strcmp(param, "execution_type")) {
        if (!strcmp(value, "CPE"))
            stdev->run_on_ape =  false;
    }
    else if (!strcmp(param, "max_ape_sessions")) {
        stdev->max_ape_sessions = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_sessions")) {
        stdev->max_cpe_sessions = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_phrases")) {
        stdev->avail_cpe_phrases = atoi(value);
    }
    else if (!strcmp(param, "max_cpe_users")) {
        stdev->avail_cpe_users = atoi(value);
    }
    else if (!strcmp(param, "max_ape_phrases")) {
        stdev->avail_ape_phrases = atoi(value);
    }
    else if (!strcmp(param, "max_ape_users")) {
        stdev->avail_ape_users = atoi(value);
    }
    else if (!strcmp(param, "rx_concurrency_disabled")) {
        stdev->rx_concurrency_disabled =
           !strncasecmp(value, "true", 4) ? true : false;
        ALOGD("%s:rx_concurrency_disabled = %d",
                 __func__, stdev->rx_concurrency_disabled);
    }
    else if (!strcmp(param, "rx_conc_max_st_ses")) {
        stdev->rx_conc_max_st_ses = atoi(value);
        ALOGD("%s:rx_conc_max_st_ses = %d",
                 __func__, stdev->rx_conc_max_st_ses);
    }
    else if (!strcmp(param, "enable_failure_detection")) {
        stdev->detect_failure =
           !strncasecmp(value, "true", 4) ? true : false;
    }
    else
        ALOGD("%s: unknown config param, ignoring..", __func__);
}

static int string_to_uuid(const char *str, sound_trigger_uuid_t *uuid)
{
    int tmp[10];

    if (str == NULL || uuid == NULL) {
        return -EINVAL;
    }

    if (sscanf(str, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
            tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5, tmp + 6,
            tmp + 7, tmp+ 8, tmp+ 9) < 10) {
        return -EINVAL;
    }
    uuid->timeLow = (uint32_t)tmp[0];
    uuid->timeMid = (uint16_t)tmp[1];
    uuid->timeHiAndVersion = (uint16_t)tmp[2];
    uuid->clockSeq = (uint16_t)tmp[3];
    uuid->node[0] = (uint8_t)tmp[4];
    uuid->node[1] = (uint8_t)tmp[5];
    uuid->node[2] = (uint8_t)tmp[6];
    uuid->node[3] = (uint8_t)tmp[7];
    uuid->node[4] = (uint8_t)tmp[8];
    uuid->node[5] = (uint8_t)tmp[9];

    return 0;
}
static void platform_stdev_set_vendor_config
(
   void *userdata,
   const char *tag_name,
   const char** attr,
   bool is_new_uuid
)
{
    struct platform_data *platform = userdata;
    sound_trigger_device_t *stdev;
    struct st_vendor_info *vendor_uuid_info = NULL;

    if (!platform || !attr) {
        ALOGE("%s: NULL attr or platform ", __func__);
        return;
    }
    if (!platform->stdev) {
        ALOGE("%s: platform stdev data NULL", __func__);
        return;
    }
    stdev = platform->stdev;

    if (is_new_uuid) {
        if (strcmp(attr[0], "value")) {
            ALOGE("%s: 'value' not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }

        vendor_uuid_info = calloc(1, sizeof(struct st_vendor_info));
        if (!vendor_uuid_info) {
            ALOGE("%s: can't allocate vendor_uuid_info", __func__);
            goto cleanup;
        }
        if (string_to_uuid(attr[1], &vendor_uuid_info->uuid) < 0) {
            ALOGE("%s: string_to_uuid failed", __func__);
            goto cleanup;
        }
        if (attr[2] && !strcmp(attr[2], "app_type"))
           vendor_uuid_info->app_type = strtoul(attr[3], NULL, 16);

        /* if soundmodel library for ISV vendor uuid is mentioned, use it. If not
           ignore and continue sending the opaque data from HAL to DSP */
        if (attr[4] && !strcmp(attr[4], "library") && strcmp(attr[5], "none")) {
            ALOGV("%s: vendor library present %s", __func__, attr[5]);
            vendor_uuid_info->smlib_handle = dlopen(attr[5], RTLD_NOW);
            if (!vendor_uuid_info->smlib_handle) {
                ALOGE("%s: ERROR. dlopen failed for %s", __func__, attr[5]);
                goto cleanup;
            }
            vendor_uuid_info->generate_st_phrase_recognition_event =
                (smlib_generate_sound_trigger_phrase_recognition_event_t)
                dlsym(vendor_uuid_info->smlib_handle,
                      "generate_sound_trigger_phrase_recognition_event");

            if (!vendor_uuid_info->generate_st_phrase_recognition_event) {
                ALOGE("%s: dlsym error %s for generate_sound_trigger_phrase_recognition_event",
                      __func__, dlerror());
                goto cleanup;
            }

            vendor_uuid_info->generate_st_recognition_config_payload =
                (smlib_generate_sound_trigger_recognition_config_payload_t)
                dlsym(vendor_uuid_info->smlib_handle,
                      "generate_sound_trigger_recognition_config_payload");

            if (!vendor_uuid_info->generate_st_recognition_config_payload) {
                ALOGE("%s: dlsym error %s for generate_sound_trigger_recognition_config_payload",
                      __func__, dlerror());
                goto cleanup;
            }
        }
        vendor_uuid_info->kw_transfer_mode = RT_TRANSFER_MODE;
        vendor_uuid_info->kw_capture_format = PCM_RAW;
        vendor_uuid_info->kw_duration = DEFAULT_MAX_KEYWORD_DURATION_MS;
        vendor_uuid_info->client_capture_read_delay = DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS;
        vendor_uuid_info->sample_rate = SOUND_TRIGGER_SAMPLING_RATE_16000;
        platform->vendor_uuid_info = vendor_uuid_info;
        return;
    }

    if (!platform->vendor_uuid_info)
        return;

    vendor_uuid_info = platform->vendor_uuid_info;

    /* process the vendor_uuid elements */
    if (!strcmp(tag_name, "firmware_image") && !stdev->run_on_ape) {
        if (strcmp(attr[0], "name")) {
            ALOGE("%s: firmware_image not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        strlcpy(vendor_uuid_info->cpe_firmware_image,
                attr[1], sizeof(vendor_uuid_info->cpe_firmware_image));
        ALOGV("%s: firmware_image name is %s",
               __func__, vendor_uuid_info->cpe_firmware_image);
    } else if (!strcmp(tag_name, "capture_keyword")) {
        if (strcmp(attr[0], "format") || strcmp(attr[2], "transfer_mode")) {
            ALOGE("%s: capture_keyword info not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        if (!strcmp(attr[1], "ADPCM_packet")) {
            vendor_uuid_info->kw_capture_format |= ADPCM_CUSTOM_PACKET;
        }
        else if (!strcmp(attr[1], "ADPCM_raw")) {
            vendor_uuid_info->kw_capture_format |= ADPCM_RAW;
        }
        else if (!strcmp(attr[1], "PCM_packet")) {
            vendor_uuid_info->kw_capture_format |= PCM_CUSTOM_PACKET;
        }
        else if (!strcmp(attr[1], "PCM_raw")) {
            vendor_uuid_info->kw_capture_format |= PCM_RAW;
        }
        else {
            ALOGE("%s: Unknown catpure format for vendor_uuid tag", __func__);
            goto cleanup;
        }

        if (!strcmp(attr[3], "FTRT")) {
            vendor_uuid_info->kw_transfer_mode = FTRT_TRANSFER_MODE;
        }
        else if (!strcmp(attr[3], "RT")) {
            vendor_uuid_info->kw_transfer_mode = RT_TRANSFER_MODE;
        }
        else {
            ALOGE("%s: Unknown catpure mode for vendor_uuid tag", __func__);
            goto cleanup;
        }

        if (attr[4] && !strcmp(attr[4], "kw_duration")) {
            vendor_uuid_info->kw_duration = atoi(attr[5]);
        }
    } else if (!strcmp(tag_name, "param_type")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "module_id") ||
            strcmp(attr[4], "param_id")) {
            ALOGE("%s: param_type info not found! for vendor_uuid tag",
                  __func__);
            goto cleanup;
        }
        if (!strcmp(attr[1], "load_sound_model")) {
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][LOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][LOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_CPE][LOAD_SOUND_MODEL] =
                vendor_uuid_info->params[ST_EXEC_MODE_ADSP][LOAD_SOUND_MODEL];

            platform->param_tag_tracker |= PARAM_LOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "unload_sound_model")) {
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][UNLOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][UNLOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_CPE][UNLOAD_SOUND_MODEL] =
                vendor_uuid_info->params[ST_EXEC_MODE_ADSP][UNLOAD_SOUND_MODEL];

            platform->param_tag_tracker |= PARAM_UNLOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "confidence_levels")) {
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][CONFIDENCE_LEVELS].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][CONFIDENCE_LEVELS].param_id =
                                                    strtoul(attr[5], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_CPE][CONFIDENCE_LEVELS] =
                vendor_uuid_info->params[ST_EXEC_MODE_ADSP][CONFIDENCE_LEVELS];

            platform->param_tag_tracker |= PARAM_CONFIDENCE_LEVELS_BIT;
        } else if (!strcmp(attr[1], "operation_mode")) {
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][OPERATION_MODE].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_ADSP][OPERATION_MODE].param_id =
                                                    strtoul(attr[5], NULL, 16);
            vendor_uuid_info->params[ST_EXEC_MODE_CPE][OPERATION_MODE] =
                vendor_uuid_info->params[ST_EXEC_MODE_ADSP][OPERATION_MODE];

            platform->param_tag_tracker |= PARAM_OPERATION_MODE_BIT;
        } else {
            ALOGD("%s: unknown config param, ignoring..", __func__);
        }
    } else if (!strcmp(tag_name, "ctrl")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
           ALOGE("%s: 'name' or 'value' not found! for ctrl tag", __func__);
           goto cleanup;
        }
        if (!strcmp(attr[1], "max_cpe_phrases")) {
            vendor_uuid_info->avail_cpe_phrases = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_cpe_users")) {
            vendor_uuid_info->avail_cpe_users = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_ape_phrases")) {
            vendor_uuid_info->avail_ape_phrases = atoi(attr[3]);
        } else if (!strcmp(attr[1], "max_ape_users")) {
            vendor_uuid_info->avail_ape_users = atoi(attr[3]);
        } else if (!strcmp(attr[1], "client_capture_read_delay")) {
            vendor_uuid_info->client_capture_read_delay = atoi(attr[3]);
        } else {
            ALOGD("%s: unknown config param, ignoring..", __func__);
        }
    } else {
        ALOGD("%s: unknown tag, ignoring..", __func__);
    }

    return;

cleanup:
    if (vendor_uuid_info) {
        if (vendor_uuid_info->smlib_handle)
            dlclose(vendor_uuid_info->smlib_handle);
        free(vendor_uuid_info);
        platform->vendor_uuid_info = NULL;
    }
}

static int platform_stdev_set_module_param_ids
(
   struct st_module_param_info *mp_info,
   char *value,
   bool legacy_params
)
{
    char *id, *test_r;
    int ret = 0;

    id = strtok_r(value, ", ", &test_r);
    if (!id) {
        ALOGE("%s: incorrect module id", __func__);
        ret = -EINVAL;
        goto error_exit;
    }
    mp_info->module_id = strtoul(id, NULL, 16);

    id = strtok_r(NULL, ", ", &test_r);
    if (!id) {
        ALOGE("%s: incorrect %s", __func__,
            legacy_params ? "param id" : "instance id");
        ret = -EINVAL;
        goto error_exit;
    }
    if (legacy_params) {
        mp_info->instance_id = 0;
        mp_info->param_id = strtoul(id, NULL, 16);
    } else {
        mp_info->instance_id = strtoul(id, NULL, 16);

        id = strtok_r(NULL, ", ", &test_r);
        if (!id) {
            ALOGE("%s: incorrect param id", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
        mp_info->param_id = strtoul(id, NULL, 16);
    }
    return 0;

error_exit:
    return ret;
}

static int acdb_device_to_id(const char* device)
{
    int i, j;
    int ret = 0;
    int dev_idx = ST_DEVICE_NONE;

    if (device == NULL) {
       ALOGE("%s: device name is NULL", __func__);
       ret = -ENODEV;
       goto done;
    }

    for (i = 0; i < ST_EXEC_MODE_MAX; i++) {
        for (j = 0; j < ST_DEVICE_MAX; j++) {
           if (strcmp(st_device_name_idx[i][j].name, device) == 0) {
               dev_idx = st_device_name_idx[i][j].index;
               break;
           }
        }
        if (dev_idx != ST_DEVICE_NONE)
            break;
    }
    if (dev_idx == ST_DEVICE_NONE) {
       ALOGE("%s: Could not find index for device name = %s",
               __func__, device);
       ret = -ENODEV;
       goto done;
    }

    ret = acdb_device_table[i][dev_idx];

done:
    return ret;
}

static int platform_stdev_set_gcs_usecase_acdb_ids
(
   unsigned int *acdb_ids,
   char *value
)
{
    char *device, *test_r = value;
    int ret = 0, num_ids = 0;

    while ((device = strtok_r(test_r, ", ", &test_r))) {
        if (num_ids < MAX_GCS_USECASE_ACDB_IDS) {
            acdb_ids[num_ids] = acdb_device_to_id(device);
            num_ids++;
        } else {
            ALOGE("%s: Exceeded max number of acdb ids", __func__);
            ret = -EINVAL;
        }
    }
    if (!num_ids) {
        ALOGE("%s: No valid acdb_ids", __func__);
        ret = -EINVAL;
    }

    return ret;
}

static int platform_stdev_set_capture_keyword_config
(
   struct st_vendor_info *sm_info,
   char *value
)
{
    char *id, *test_r;
    int ret = 0;

    id = strtok_r(value, ", ", &test_r);
    if (!id) {
        ALOGE("%s: Incorrect capture format", __func__);
        ret = -EINVAL;
        goto error_exit;
    }
    if (!strcmp(id, "ADPCM_packet")) {
        sm_info->kw_capture_format |= ADPCM_CUSTOM_PACKET;
    } else if (!strcmp(id, "ADPCM_raw")) {
        sm_info->kw_capture_format |= ADPCM_RAW;
    } else if (!strcmp(id, "MULAW_raw")) {
        sm_info->kw_capture_format |= MULAW_RAW;
    } else if (!strcmp(id, "PCM_packet")) {
        sm_info->kw_capture_format |= PCM_CUSTOM_PACKET;
    } else if (!strcmp(id, "PCM_raw")) {
        sm_info->kw_capture_format |= PCM_RAW;
    } else {
        ALOGE("%s: Unknown catpure format", __func__);
        ret = -EINVAL;
        goto error_exit;
    }

    id = strtok_r(NULL, ", ", &test_r);
    if (!id) {
        ALOGE("%s: Incorrect capture mode", __func__);
        ret = -EINVAL;
        goto error_exit;
    }
    if (!strcmp(id, "FTRT")) {
        sm_info->kw_transfer_mode = FTRT_TRANSFER_MODE;
    }
    else if (!strcmp(id, "RT")) {
        sm_info->kw_transfer_mode = RT_TRANSFER_MODE;
    }
    else {
        ALOGE("%s: Unknown catpure mode", __func__);
        ret = -EINVAL;
        goto error_exit;
    }

    id = strtok_r(NULL, ", ", &test_r);
    if (id)
        sm_info->kw_duration = atoi(id);

    return 0;

error_exit:
    return ret;
}

static int platform_set_common_config
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    char str_value[128];
    int value, err;

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_CPE_SESSIONS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_CPE_SESSIONS);
        stdev->max_cpe_sessions = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_APE_SESSIONS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_APE_SESSIONS);
        stdev->max_ape_sessions = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_WDSP_SESSIONS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_WDSP_SESSIONS);
        stdev->max_wdsp_sessions = value;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_SW_MAD,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SW_MAD);
        stdev->sw_mad = !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_ENABLE_FAILURE_DETECTION,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_ENABLE_FAILURE_DETECTION);
        stdev->detect_failure = !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CPE_FE_TO_BE_FIXED,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CPE_FE_TO_BE_FIXED);
        my_data->cpe_fe_to_be_fixed = !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_RX_CONCURRENCY_DISABLED,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_RX_CONCURRENCY_DISABLED);
        stdev->rx_concurrency_disabled = !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_RX_MAX_CONC_SESSIONS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_RX_MAX_CONC_SESSIONS);
        stdev->rx_conc_max_st_ses = value;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_SUPPORT_DEVICE_SWITCH,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SUPPORT_DEVICE_SWITCH);
        stdev->support_dev_switch = !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_TRANSIT_TO_ADSP_ON_PLAYBACK,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_TRANSIT_TO_ADSP_ON_PLAYBACK);
        stdev->transit_to_adsp_on_playback =
            !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_TRANSIT_WAIT_TIME, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_TRANSIT_WAIT_TIME);
        stdev->transit_wait_time = value;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_BACKEND_PORT_NAME,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_BACKEND_PORT_NAME);
        strlcpy(my_data->backend_port,
                str_value, sizeof(my_data->backend_port));
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_BACKEND_DAI_NAME,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_BACKEND_DAI_NAME);
        strlcpy(my_data->backend_dai_name,
                str_value, sizeof(my_data->backend_dai_name));
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_IMPLEMENTER_UUID,
        str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_IMPLEMENTER_UUID);
        if (string_to_uuid(str_value, &stdev->hw_properties->uuid) < 0) {
           ALOGE("%s: string_to_uuid failed", __func__);
           return -EINVAL;
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_SND_CARD_NAME,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SND_CARD_NAME);
        strlcpy(my_data->snd_card_name,
                str_value, sizeof(my_data->snd_card_name));
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CONCURRENT_CAPTURE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CONCURRENT_CAPTURE);
        stdev->conc_capture =
            !strncasecmp(str_value, "true", 4) ? true : false;
    }

    if (stdev->conc_capture) {
        err = str_parms_get_str(parms, ST_PARAM_KEY_CONCURRENT_VOICE_CALL,
                                str_value, sizeof(str_value));
        if (err >= 0) {
            str_parms_del(parms, ST_PARAM_KEY_CONCURRENT_VOICE_CALL);
            stdev->conc_voice_call =
                !strncasecmp(str_value, "true", 4) ? true : false;
        }

        err = str_parms_get_str(parms, ST_PARAM_KEY_CONCURRENT_VOIP_CALL,
                                str_value, sizeof(str_value));
        if (err >= 0) {
            str_parms_del(parms, ST_PARAM_KEY_CONCURRENT_VOIP_CALL);
            stdev->conc_voip_call =
                !strncasecmp(str_value, "true", 4) ? true : false;
        }
    }

    return 0;
}

static int platform_set_acdb_ids
(
   void *platform,
   struct str_parms *parms
)
{
    int ret = 0, value, err;

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_APE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HEADSET_CPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HEADSET_CPE);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HEADSET_CPE, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE_PP, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE_PP);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_APE_PP, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_6MIC_ARM, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_6MIC_ARM);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_6MIC_ARM, value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_8MIC_ARM, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_8MIC_ARM);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_8MIC_ARM, value);
    }

exit:
    return ret;
}

static int platform_stdev_process_backend_type
(
   void *platform,
   struct str_parms *parms
)
{
    int ret = 0, err;
    char *kv_pairs = str_parms_to_str(parms);
    char str_value[128];

    if (!kv_pairs) {
        ALOGE("%s: key-value pair is NULL",__func__);
        return -EINVAL;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_CPE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_CPE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_CPE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_APE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_DMIC_APE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_TMIC_APE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HANDSET_QMIC_APE, str_value);
        if (ret)
            goto exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DEVICE_HEADSET_CPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HEADSET_CPE);
        ret = set_backend_type(platform, ST_PARAM_KEY_DEVICE_HEADSET_CPE, str_value);
        if (ret)
            goto exit;
    }

exit:
    free(kv_pairs);
    return ret;
}

static int string_to_profile_type(const char *str, st_profile_type_t *type)
{
    int ret = 0;

    if (!strncmp(str, "NONE", sizeof("NONE"))) {
        *type = ST_PROFILE_TYPE_NONE;
    } else if (!strncmp(str, "DEFAULT", sizeof("DEFAULT"))) {
        *type = ST_PROFILE_TYPE_DEFAULT;
    } else if (!strncmp(str, "EC", sizeof("EC"))) {
        *type = ST_PROFILE_TYPE_EC;
    } else if (!strncmp(str, "UNPROCESSED", sizeof("UNPROCESSED"))) {
        *type = ST_PROFILE_TYPE_UNPROCESSED;
    } else if (!strncmp(str, "FLUENCE", sizeof("FLUENCE"))) {
        *type = ST_PROFILE_TYPE_FLUENCE;
    } else if (!strncmp(str, "FLUENCE_STEREO", sizeof("FLUENCE_STEREO"))) {
        *type = ST_PROFILE_TYPE_FLUENCE_STEREO;
    } else {
        ALOGE("unknown profile string %s", str);
        ret = -EINVAL;
    }

    return ret;
}

static int string_to_fluence_type(const char *str, st_fluence_type_t *type)
{
    int ret = 0;

    if (!strncmp(str, "NONE", sizeof("NONE"))) {
        *type = ST_FLUENCE_TYPE_NONE;
    } else if (!strncmp(str, "FLUENCE", sizeof("FLUENCE"))) {
        *type = ST_FLUENCE_TYPE_MONO;
    } else if (!strncmp(str, "FLUENCE_DMIC", sizeof("FLUENCE_DMIC"))) {
        *type = ST_FLUENCE_TYPE_DMIC;
    } else if (!strncmp(str, "FLUENCE_TMIC", sizeof("FLUENCE_TMIC"))) {
        *type = ST_FLUENCE_TYPE_TMIC;
    } else if (!strncmp(str, "FLUENCE_QMIC", sizeof("FLUENCE_QMIC"))) {
        *type = ST_FLUENCE_TYPE_QMIC;
    } else {
        ALOGE("unknown fluence string %s", str);
        ret = -EINVAL;
    }

    return ret;
}

static int get_channel_cnt_from_fluence_type(st_fluence_type_t type)
{
    int channel_cnt = 0;

    switch (type) {
    case ST_FLUENCE_TYPE_NONE:
    case ST_FLUENCE_TYPE_MONO:
        channel_cnt = 1;
        break;
    case ST_FLUENCE_TYPE_DMIC:
        channel_cnt = 2;
        break;
    case ST_FLUENCE_TYPE_TMIC:
        channel_cnt = 3;
        break;
    case ST_FLUENCE_TYPE_QMIC:
        channel_cnt = 4;
        break;
    default:
        ALOGE("%s: Invalid fluence type", __func__);
    }

    return channel_cnt;
}

static int platform_stdev_set_gcs_params
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct st_gcs_params *gcs_params;
    char str_value[128];
    char *kv_pairs = str_parms_to_str(parms);
    int ret = 0, err;
    struct listnode *sm_info_node = NULL;
    struct st_vendor_info *sm_info = NULL;

    if (kv_pairs == NULL) {
        ALOGE("%s: key-value pair is NULL", __func__);
        return -EINVAL;
    }

    gcs_params = calloc(1, sizeof(*gcs_params));
    if (!gcs_params) {
        ALOGE("%s: new_params allcoation failed", __func__);
        ret = -ENOMEM;
        goto err_exit;
    }

    /* populate new params */
    err = str_parms_get_str(parms, ST_PARAM_KEY_UID,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_UID);
        gcs_params->uid = strtoul(str_value, NULL, 16);
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_ACDB_DEVICES,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_ACDB_DEVICES);
        ret = platform_stdev_set_gcs_usecase_acdb_ids(gcs_params->acdb_ids, str_value);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[LOAD_SOUND_MODEL], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[UNLOAD_SOUND_MODEL], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[CONFIDENCE_LEVELS], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_OPERATION_MODE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_OPERATION_MODE_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[OPERATION_MODE], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_DETECTION_EVENT_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DETECTION_EVENT_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[DETECTION_EVENT], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_READ_REQ_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_READ_REQ_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[READ_REQ], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_READ_RSP_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_READ_RSP_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[READ_RSP], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[CUSTOM_CONFIG], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_START_ENGINE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_START_ENGINE_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[START_ENGINE], str_value, false);
        if (ret)
            goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_RESTART_ENGINE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_RESTART_ENGINE_IDS);
        ret = platform_stdev_set_module_param_ids(&gcs_params->params[RESTART_ENGINE], str_value, false);
        if (ret)
            goto err_exit;
    }

    /* store in vendor_info */
    sm_info_node = list_tail(&my_data->stdev->vendor_uuid_list);
    if (sm_info_node) {
        sm_info = node_to_item(sm_info_node, struct st_vendor_info, list_node);
        list_add_tail(&sm_info->gcs_usecase_list, &gcs_params->list_node);
    } else {
        ALOGE("%s: found NULL sm_info", __func__);
        ret = -EINVAL;
        goto err_exit;
    }
    free(kv_pairs);
    return 0;

err_exit:
    if (gcs_params)
        free(gcs_params);
    free(kv_pairs);
    return ret;
}

static int platform_stdev_set_lsm_params
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    char str_value[128];
    char *kv_pairs = str_parms_to_str(parms);
    int ret = 0, err;
    struct listnode *sm_info_node = NULL;
    struct st_vendor_info *sm_info = NULL;
    st_exec_mode_t exec_mode = ST_EXEC_MODE_NONE;

    if (kv_pairs == NULL) {
        ALOGE("%s: key-value pair is NULL", __func__);
        return -EINVAL;
    }

    /* Get the last added vendor_info node */
    sm_info_node = list_tail(&my_data->stdev->vendor_uuid_list);
    if (sm_info_node) {
        sm_info = node_to_item(sm_info_node, struct st_vendor_info, list_node);
    } else {
        ALOGE("%s: found NULL sm_info", __func__);
        ret = -EINVAL;
        goto err_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_EXECUTION_MODE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_EXECUTION_MODE);
        if (!strcmp(str_value, "WDSP"))
            exec_mode = ST_EXEC_MODE_CPE;
        else if (!strcmp(str_value, "ADSP"))
            exec_mode = ST_EXEC_MODE_ADSP;
        else
            ALOGE("%s: Invalid execution mode set %s", __func__, str_value);
    }

    if ((exec_mode <= ST_EXEC_MODE_NONE) ||
        (exec_mode >= ST_EXEC_MODE_MAX)) {
        ALOGE("%s: Invalid execution mode %x", __func__, exec_mode);
        goto err_exit;
    }

    ALOGV("%s: Process params for exec mode %x", __func__, exec_mode);
    err = str_parms_get_str(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][LOAD_SOUND_MODEL],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_LOAD_SOUND_MODEL_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][UNLOAD_SOUND_MODEL],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_UNLOAD_SOUND_MODEL_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][CONFIDENCE_LEVELS],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_CONFIDENCE_LEVELS_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_OPERATION_MODE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_OPERATION_MODE_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][OPERATION_MODE],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_OPERATION_MODE_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_POLLING_ENABLE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_POLLING_ENABLE_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][POLLING_ENABLE],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_POLLING_ENABLE_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[exec_mode][CUSTOM_CONFIG],
                                                  str_value, true);
        if (ret)
            goto err_exit;
        sm_info->param_tag_tracker[exec_mode] |= PARAM_CUSTOM_CONFIG_BIT;
    }

err_exit:
    free(kv_pairs);
    return ret;
}
static int platform_stdev_create_sm_config_params
(
    void *platform
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct st_vendor_info *sm_info;

    /*
     * Allocate the vendor sound model config. Set default values to
     * config params. Push this sound model config to list.
     */
    sm_info = calloc(1, sizeof(*sm_info));
    if (!sm_info) {
        ALOGE("%s: sm_info allcoation failed", __func__);
        return -ENOMEM;
    }
    /* initialize to deault config */
    sm_info->kw_transfer_mode = RT_TRANSFER_MODE;
    sm_info->kw_capture_format = PCM_RAW;
    sm_info->kw_duration = DEFAULT_MAX_KEYWORD_DURATION_MS;
    sm_info->client_capture_read_delay = DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS;
    sm_info->sample_rate = SOUND_TRIGGER_SAMPLING_RATE_16000;
    sm_info->format = PCM_FORMAT_S16_LE;
    sm_info->channel_count = SOUND_TRIGGER_CHANNEL_MODE_MONO;
    sm_info->profile_type = ST_PROFILE_TYPE_NONE;
    sm_info->fwk_mode = SOUND_TRIGGER_EVENT_NON_TIME_STAMP_MODE;
    sm_info->fluence_type = ST_FLUENCE_TYPE_NONE;
    sm_info->wdsp_fluence_type = ST_FLUENCE_TYPE_NONE;
    sm_info->split_ec_ref_data = false;
    sm_info->ec_ref_channel_cnt = SOUND_TRIGGER_CHANNEL_MODE_MONO;

    list_init(&sm_info->gcs_usecase_list);

    list_add_tail(&my_data->stdev->vendor_uuid_list,
        &sm_info->list_node);

    return 0;
}

static int platform_stdev_set_sm_config_params
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    struct listnode *sm_info_node, *gcs_node;
    struct st_vendor_info *sm_info = NULL;
    char str_value[128];
    char *kv_pairs = str_parms_to_str(parms);
    int ret = 0, err, value;
    struct listnode *tmp_node;
    struct st_gcs_params *gcs_params;

    ALOGV("%s: enter: %s", __func__, kv_pairs);
    if (kv_pairs == NULL) {
        ALOGE("%s: key-value pair is NULL", __func__);
        return -EINVAL;
    }

    sm_info_node = list_tail(&my_data->stdev->vendor_uuid_list);
    if (!sm_info_node) {
        ALOGE("%s: found NULL sm_info_node", __func__);
        ret = -ENOMEM;
        goto error_exit;
    }

    sm_info = node_to_item(sm_info_node, struct st_vendor_info, list_node);

    /* Set the platform configured params */
    err = str_parms_get_str(parms, ST_PARAM_KEY_SM_VENDOR_UUID,
        str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SM_VENDOR_UUID);
        if (string_to_uuid(str_value, &sm_info->uuid) < 0) {
           ALOGE("%s: string_to_uuid failed", __func__);
           ret = -EINVAL;
           goto error_exit;
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_EXECUTION_TYPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_EXECUTION_TYPE);
        if (!strcmp(str_value, "WDSP")) {
            sm_info->exec_mode_cfg = EXEC_MODE_CFG_CPE;
            my_data->stdev->run_on_ape = false;
        } else if (!strcmp(str_value, "ADSP")) {
            sm_info->exec_mode_cfg = EXEC_MODE_CFG_APE;
            my_data->stdev->run_on_ape = true;
        } else if (!strcmp(str_value, "DYNAMIC")) {
            sm_info->exec_mode_cfg = EXEC_MODE_CFG_DYNAMIC;
        } else if (!strcmp(str_value, "ARM")) {
            sm_info->exec_mode_cfg = EXEC_MODE_CFG_ARM;
            my_data->stdev->run_on_ape = false;
        } else {
            ALOGE("%s: invalid exec type set: %s", __func__, str_value);
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_APP_TYPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_APP_TYPE);
        sm_info->app_type = strtoul(str_value, NULL, 16);
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_LIBRARY,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_LIBRARY);
        /* if soundmodel library for ISV vendor uuid is mentioned, use it. If not
           ignore and continue sending the opaque data from HAL to DSP */
        if (strcmp(str_value, "none"))
            load_smlib(sm_info, str_value);
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_CPE_PHRASES, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_CPE_PHRASES);
        sm_info->avail_cpe_phrases = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_CPE_USERS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_CPE_USERS);
        sm_info->avail_cpe_users = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_APE_PHRASES, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_APE_PHRASES);
        sm_info->avail_ape_phrases = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_MAX_APE_USERS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_MAX_APE_USERS);
        sm_info->avail_ape_users = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_SAMPLE_RATE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SAMPLE_RATE);
        sm_info->sample_rate = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_CLIENT_CAPTURE_READ_DELAY, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CLIENT_CAPTURE_READ_DELAY);
        sm_info->client_capture_read_delay = value;
    }

    /*
     * In cases where common module and param ids are used for both modes like
     * in older XML files, copy ADSP ids to CPE params after parsing each key.
     */
    err = str_parms_get_str(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][LOAD_SOUND_MODEL],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][LOAD_SOUND_MODEL] =
                 sm_info->params[ST_EXEC_MODE_ADSP][LOAD_SOUND_MODEL];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_LOAD_SOUND_MODEL_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_LOAD_SOUND_MODEL_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][UNLOAD_SOUND_MODEL],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][UNLOAD_SOUND_MODEL] =
                 sm_info->params[ST_EXEC_MODE_ADSP][UNLOAD_SOUND_MODEL];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_UNLOAD_SOUND_MODEL_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_UNLOAD_SOUND_MODEL_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][CONFIDENCE_LEVELS],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][CONFIDENCE_LEVELS] =
                 sm_info->params[ST_EXEC_MODE_ADSP][CONFIDENCE_LEVELS];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_CONFIDENCE_LEVELS_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_CONFIDENCE_LEVELS_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_OPERATION_MODE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_OPERATION_MODE_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][OPERATION_MODE],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][OPERATION_MODE] =
                 sm_info->params[ST_EXEC_MODE_ADSP][OPERATION_MODE];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_OPERATION_MODE_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_OPERATION_MODE_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_POLLING_ENABLE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_POLLING_ENABLE_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][POLLING_ENABLE],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][POLLING_ENABLE] =
                 sm_info->params[ST_EXEC_MODE_ADSP][POLLING_ENABLE];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_POLLING_ENABLE_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_POLLING_ENABLE_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CUSTOM_CONFIG_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[ST_EXEC_MODE_ADSP][CUSTOM_CONFIG],
                                                  str_value, true);
        if (ret)
            goto error_exit;
        sm_info->params[ST_EXEC_MODE_CPE][CUSTOM_CONFIG] =
                 sm_info->params[ST_EXEC_MODE_ADSP][CUSTOM_CONFIG];

        sm_info->param_tag_tracker[ST_EXEC_MODE_ADSP] |= PARAM_CUSTOM_CONFIG_BIT;
        sm_info->param_tag_tracker[ST_EXEC_MODE_CPE] |= PARAM_CUSTOM_CONFIG_BIT;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CAPTURE_KEYWORD,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CAPTURE_KEYWORD);
        ret = platform_stdev_set_capture_keyword_config(sm_info, str_value);
        if (ret)
            goto error_exit;

    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_FIRMWARE_IMAGE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_FIRMWARE_IMAGE);
        strlcpy(sm_info->cpe_firmware_image,
                str_value, sizeof(sm_info->cpe_firmware_image));
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_EVENT_TIMESTAMP_MODE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_EVENT_TIMESTAMP_MODE);
        if (!strncasecmp(str_value, "true", 4))
            sm_info->fwk_mode = SOUND_TRIGGER_EVENT_TIME_STAMP_MODE;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_BIT_WIDTH, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_BIT_WIDTH);
        if (value == 16) {
            sm_info->format = PCM_FORMAT_S16_LE;
        } else if (value == 24) {
            sm_info->format = PCM_FORMAT_S24_LE;
        } else {
            ALOGE("%s: invalid bit width for profile", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_CHANNEL_COUNT, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CHANNEL_COUNT);
        sm_info->channel_count = value;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_ADM_CFG_PROFILE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_ADM_CFG_PROFILE);
        if (string_to_profile_type(str_value, &sm_info->profile_type) < 0) {
            ALOGE("%s: string_to_profile_type failed", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_FLUENCE_TYPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_FLUENCE_TYPE);
        if (string_to_fluence_type(str_value, &sm_info->fluence_type) < 0) {
            ALOGE("%s: string_to_fluence_type failed", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_WDSP_FLUENCE_TYPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_WDSP_FLUENCE_TYPE);
        if (string_to_fluence_type(str_value, &sm_info->wdsp_fluence_type) < 0) {
            ALOGE("%s: string_to_fluence_type failed", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_SPLIT_EC_REF_DATA,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SPLIT_EC_REF_DATA);
        sm_info->split_ec_ref_data =
            !strncasecmp(str_value, "true", 4) ? true : false;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_EC_REF_CHANNEL_COUNT, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_EC_REF_CHANNEL_COUNT);
        sm_info->ec_ref_channel_cnt = value;
    }

    sm_info->avail_transit_ape_phrases = sm_info->avail_ape_phrases;
    sm_info->avail_transit_ape_users = sm_info->avail_ape_users;
    sm_info->avail_transit_cpe_phrases = sm_info->avail_cpe_phrases;
    sm_info->avail_transit_cpe_users = sm_info->avail_cpe_users;

    /* store combined keyphrases/users of all engines */
    stdev->avail_ape_phrases += sm_info->avail_ape_phrases;
    stdev->avail_ape_users += sm_info->avail_ape_users;
    stdev->avail_cpe_phrases += sm_info->avail_cpe_phrases;
    stdev->avail_cpe_users += sm_info->avail_cpe_users;

    free(kv_pairs);
    ALOGV("%s: exit", __func__);
    return 0;

error_exit:
    if (sm_info) {
        /* remove and free the gcs uid params */
        list_for_each_safe(gcs_node, tmp_node, &sm_info->gcs_usecase_list) {
            gcs_params = node_to_item(gcs_node, struct st_gcs_params, list_node);
            list_remove(gcs_node);
            free(gcs_params);
        }
        list_remove(sm_info_node);
        free(sm_info);
    }
    free(kv_pairs);
    return ret;
}

static int platform_stdev_set_adm_cfg_params
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct adm_cfg_info *cfg_info;
    char str_value[128];
    char *kv_pairs = str_parms_to_str(parms);
    int ret = 0, err, value;

    ALOGV("%s: enter: %s", __func__, kv_pairs);
    if (kv_pairs == NULL) {
        ALOGE("%s: key-value pair is NULL", __func__);
        return -EINVAL;
    }

    /* Allocate the app type profile info
     * Set the platform configured profile params.
     * Push this profile to platform data.
     */
    cfg_info = calloc(1, sizeof(*cfg_info));
    if (!cfg_info) {
        ALOGE("%s: cfg info allcoation failed", __func__);
        ret = -ENOMEM;
        goto error_exit;
    }

    /* initialize to deault config */
    cfg_info->profile_type = ST_PROFILE_TYPE_DEFAULT;
    cfg_info->app_type = SOUND_TRIGGER_DEVICE_DEFAULT_APP_TYPE;
    cfg_info->sample_rate = SOUND_TRIGGER_SAMPLING_RATE_16000;
    cfg_info->bit_width = SOUND_TRIGGER_BIT_WIDTH;

    /* Set the platform configured params */
    err = str_parms_get_str(parms, ST_PARAM_KEY_ADM_CFG_PROFILE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_ADM_CFG_PROFILE);
        if (string_to_profile_type(str_value, &cfg_info->profile_type) < 0) {
            ALOGE("%s: string_to_profile_type failed", __func__);
            ret = -EINVAL;
            goto error_exit;
        }
    } else {
        ALOGE("%s: Error: adm cfg defined without profile", __func__);
        ret = -EINVAL;
        goto error_exit;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_APP_TYPE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_APP_TYPE);
        cfg_info->app_type = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_SAMPLE_RATE, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_SAMPLE_RATE);
        cfg_info->sample_rate = value;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_BIT_WIDTH, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_BIT_WIDTH);
        cfg_info->bit_width = value;
    }

    list_add_tail(&my_data->stdev->adm_cfg_list,
                  &cfg_info->list_node);
    free(kv_pairs);
    return 0;

error_exit:
    if (cfg_info)
        free(cfg_info);
    free(kv_pairs);
    return ret;
}

int platform_stdev_set_parameters
(
   void *platform,
   struct str_parms *parms
)
{
    char *kv_pairs = str_parms_to_str(parms);

    ALOGV("%s: enter with key-value pair: %s", __func__, kv_pairs);
    if (!kv_pairs) {
        ALOGE("%s: key-value pair is NULL",__func__);
        return -EINVAL;
    }

    platform_set_common_config(platform, parms);
    platform_set_acdb_ids(platform, parms);
    free(kv_pairs);

    return 0;
}

static void platform_stdev_process_kv_params
(
   void *platform,
   const XML_Char **attr
)
{
    struct platform_data *my_data = (struct platform_data *)platform;

    ALOGV("%s: %s:%s ", __func__, attr[0],attr[1]);
    str_parms_add_str(my_data->kvpairs, (char*)attr[0], (char*)attr[1]);
    return;
}

static void platform_stdev_process_versioned_xml_data
(
   struct platform_data *my_data,
   const XML_Char *tag_name,
   const XML_Char **attr
)
{
    st_xml_process_fn fn_ptr;

    if (!strcmp(tag_name, "common_config")) {
        my_data->st_xml_tag = TAG_COMMON;
    } else if (!strcmp(tag_name, "acdb_ids")) {
        my_data->st_xml_tag = TAG_ACDB;
    } else if (!strcmp(tag_name, "sound_model_config")) {
        my_data->st_xml_tag = TAG_SOUND_MODEL;
        /* create an entry for storing sound_model_config */
        platform_stdev_create_sm_config_params(my_data);
    } else if (!strcmp(tag_name, "gcs_usecase")) {
        my_data->st_xml_tag = TAG_GCS_USECASE;
    } else if (!strcmp(tag_name, "lsm_usecase")) {
        my_data->st_xml_tag = TAG_LSM_USECASE;
    } else if (!strcmp(tag_name, "adm_config")) {
        my_data->st_xml_tag = TAG_ADM_CFG;
    } else if (!strcmp(tag_name, "backend_type")) {
        my_data->st_xml_tag = TAG_BACKEND_TYPE;
    } else if (!strcmp(tag_name, "param")) {
        if ((my_data->st_xml_tag != TAG_ROOT) && (my_data->st_xml_tag != TAG_COMMON) &&
            (my_data->st_xml_tag != TAG_ACDB) && (my_data->st_xml_tag != TAG_SOUND_MODEL) &&
            (my_data->st_xml_tag != TAG_GCS_USECASE) &&
            (my_data->st_xml_tag != TAG_LSM_USECASE) &&
            (my_data->st_xml_tag != TAG_ADM_CFG) &&
            (my_data->st_xml_tag != TAG_BACKEND_TYPE)) {
            ALOGE("%s: param under unknown tag", __func__);
            return;
        }
        fn_ptr = process_table[my_data->st_xml_tag];
        if (fn_ptr)
            fn_ptr(my_data, attr);
    }
}

static void start_tag(void *userdata, const XML_Char *tag_name,
                      const XML_Char **attr)
{
    int ret;
    struct platform_data *platform = (void *)userdata;

    if (!platform || !tag_name || !attr) {
        ALOGE("%s: NULL platform/tag_name/attr", __func__);
        return;
    }

    if ((platform->st_xml_tag == TAG_ROOT) &&
        !strcmp(tag_name, "param") && !strcmp(attr[0], "version") ) {
        /* This must be the first param for versioned XML file */
        platform->xml_version = strtoul(attr[1], NULL, 16);
        ALOGV("%s: xml_version 0x%04x", __func__, platform->xml_version);
    } else if (platform->xml_version) {
        platform_stdev_process_versioned_xml_data(platform, tag_name, attr);
    } else if (!strcmp(tag_name, "device")) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
            ALOGE("%s: 'name' or 'value' not found! for device tag", __func__);
            goto done;
        }
        ret = platform_stdev_set_acdb_id(userdata, (const char *)attr[1], atoi((const char *)attr[3]));
        if (ret < 0) {
            ALOGE("%s: Device %s in platform xml not found, no ACDB ID set!",
                  __func__, attr[1]);
            goto done;
        }
    } else if (!strcmp(tag_name, "ctrl") && !platform->vendor_uuid_info) {
        if (strcmp(attr[0], "name") || strcmp(attr[2], "value")) {
            ALOGE("%s: 'name' or 'value' not found! for ctrl tag", __func__);
            goto done;
        }
        platform_stdev_set_config(userdata, (const char *)attr[1],
                                  (const char *)attr[3]);
    } else if (!strcmp(tag_name, "vendor_uuid") || platform->vendor_uuid_info) {
        platform_stdev_set_vendor_config(userdata, (const char *)tag_name,
                                         (const char **)attr, !platform->vendor_uuid_info);
    }

done:
    return;
}

static void end_tag(void *userdata, const XML_Char *tag_name)
{
    struct platform_data *platform = userdata;

    if (!platform || !tag_name) {
        ALOGE("%s: NULL tag or platform", __func__);
        return;
    }

    if (platform->xml_version) {
        if (!strcmp(tag_name, "common_config") || (!strcmp(tag_name, "acdb_ids"))) {
            platform->st_xml_tag = TAG_ROOT;
            platform_stdev_set_parameters(platform, platform->kvpairs);
        } else if (!strcmp(tag_name, "gcs_usecase")) {
            platform->st_xml_tag = TAG_SOUND_MODEL;
            platform_stdev_set_gcs_params(platform, platform->kvpairs);
        } else if (!strcmp(tag_name, "lsm_usecase")) {
            platform->st_xml_tag = TAG_SOUND_MODEL;
            platform_stdev_set_lsm_params(platform, platform->kvpairs);
        } else if (!strcmp(tag_name, "sound_model_config")) {
            platform->st_xml_tag = TAG_ROOT;
            platform_stdev_set_sm_config_params(platform, platform->kvpairs);
        } else if (!strcmp(tag_name, "adm_config")) {
            platform->st_xml_tag = TAG_ROOT;
            platform_stdev_set_adm_cfg_params(platform, platform->kvpairs);
        } else if (!strcmp(tag_name, "backend_type")) {
            platform->st_xml_tag = TAG_ROOT;
            platform_stdev_process_backend_type(platform, platform->kvpairs);
        }
        return;
    }

    if (!strcmp(tag_name, "vendor_uuid")) {
        if (platform->vendor_uuid_info) {
            if (platform->param_tag_tracker & PARAM_ID_MANDATORY_BITS) {
                list_add_tail(&platform->stdev->vendor_uuid_list,
                              &platform->vendor_uuid_info->list_node);
            } else {
                if (platform->vendor_uuid_info->smlib_handle)
                    dlclose(platform->vendor_uuid_info->smlib_handle);
                free(platform->vendor_uuid_info);
                ALOGE("%s: param_type missing for vendor_uuid tag. Bits 0x%x",
                      __func__, platform->param_tag_tracker);
            }
        }
        platform->param_tag_tracker = 0;
        platform->vendor_uuid_info = NULL;
    }
}

static int platform_parse_info(struct platform_data *platform, const char *filename)
{
    XML_Parser      parser;
    FILE            *file;
    int             ret = 0;
    int             bytes_read;
    void            *buf;

    file = fopen(filename, "r");
    if (!file) {
        ALOGD("%s: Failed to open %s, using defaults", __func__, filename);
        ret = -ENODEV;
        goto done;
    }
    platform->st_xml_tag = TAG_ROOT;
    platform->kvpairs = str_parms_create();

    parser = XML_ParserCreate(NULL);
    if (!parser) {
        ALOGE("%s: Failed to create XML parser!", __func__);
        ret = -ENODEV;
        goto err_close_file;
    }

    XML_SetUserData(parser, platform);

    XML_SetElementHandler(parser, start_tag, end_tag);

    while (1) {
        buf = XML_GetBuffer(parser, BUF_SIZE);
        if (buf == NULL) {
            ALOGE("%s: XML_GetBuffer failed", __func__);
            ret = -ENOMEM;
            goto err_free_parser;
        }

        bytes_read = fread(buf, 1, BUF_SIZE, file);
        if (bytes_read < 0) {
            ALOGE("%s: fread failed, bytes read = %d", __func__, bytes_read);
             ret = bytes_read;
            goto err_free_parser;
        }

        if (XML_ParseBuffer(parser, bytes_read,
                            bytes_read == 0) == XML_STATUS_ERROR) {
            ALOGE("%s: XML_ParseBuffer failed, for %s",
                __func__, filename);
            ret = -EINVAL;
            goto err_free_parser;
        }

        if (bytes_read == 0)
            break;
    }

err_free_parser:
    XML_ParserFree(parser);
err_close_file:
    fclose(file);
done:
    return ret;
}

static int get_codec_version(struct platform_data *my_data,
                            const char *snd_card_name,
                            int card_num)
{
    char procfs_path[50];
    FILE *fp;

    /* query the codec type to handle FTRT based on codec capability
       currently applicable to tasha codec versions */
    if (strstr(snd_card_name, "tasha")) {
        snprintf(procfs_path, sizeof(procfs_path),
                 "/proc/asound/card%d/codecs/tasha/version", card_num);
        if ((fp = fopen(procfs_path, "r")) != NULL) {
            fgets(my_data->codec_version, sizeof(my_data->codec_version), fp);
            fclose(fp);
        } else {
            ALOGE("%s: ERROR. cannot open %s", __func__, procfs_path);
            return -ENOENT;
        }
    }
    ALOGD("%s: codec version %s", __func__, my_data->codec_version);

    return 0;
}

static void query_stdev_platform(sound_trigger_device_t *stdev,
                                 const char *snd_card_name,
                                 char *mixer_path_xml)
{
    if (strstr(snd_card_name, "msm8939-tapan")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9306,
                        sizeof(MIXER_PATH_XML_WCD9306));
    } else if (strstr(snd_card_name, "msm8952-tomtom")||
                 strstr(snd_card_name, "msm8996-tomtom")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9330,
                        sizeof(MIXER_PATH_XML_WCD9330));
    } else if (strstr(snd_card_name, "sdm670-skuw")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_SKUW,
                        sizeof(MIXER_PATH_XML_SKUW));
    } else if (strstr(snd_card_name, "msm8976-tasha")||
                 strstr(snd_card_name, "msm8952-tasha") ||
                 strstr(snd_card_name, "msm8953-tasha") ||
                 strstr(snd_card_name, "msm8953-sku3-tasha") ||
                 strstr(snd_card_name, "msm8937-tasha") ||
                 strstr(snd_card_name, "sdm660-tasha") ||
                 strstr(snd_card_name, "sdm670-tasha") ||
                 strstr(snd_card_name, "apq8009-tashalite")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9335,
                        sizeof(MIXER_PATH_XML_WCD9335));
    } else if (strstr(snd_card_name, "tavil")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9340,
            sizeof(MIXER_PATH_XML_WCD9340));
        stdev->is_gcs = true;
    } else {
        strlcpy(mixer_path_xml, MIXER_PATH_XML,
                         sizeof(MIXER_PATH_XML));
    }

    /* Default sw_mad value will be overwritten if it
       is defined in platform info file
    */
    if ((strstr(snd_card_name, "msm8939") ||
        strstr(snd_card_name, "msm8909") ||
        strstr(snd_card_name, "apq8009") ||
        strstr(snd_card_name, "msm8952") ||
        strstr(snd_card_name, "msm8976") ||
        strstr(snd_card_name, "msm8953") ||
        strstr(snd_card_name, "msm8937") ||
        strstr(snd_card_name, "sdm660") ||
        strstr(snd_card_name, "sdm670") ||
        strstr(snd_card_name, "qcs605-lc") ||
        strstr(snd_card_name, "msm8x16")) &&
        !strstr(snd_card_name, "msm8976-tasha") &&
        !strstr(snd_card_name, "msm8952-tasha") &&
        !strstr(snd_card_name, "msm8953-tasha") &&
        !strstr(snd_card_name, "msm8953-sku3-tasha") &&
        !strstr(snd_card_name, "msm8937-tasha") &&
        !strstr(snd_card_name, "msm8952-tomtom") &&
        !strstr(snd_card_name, "sdm660-tasha") &&
        !strstr(snd_card_name, "sdm660-tavil") &&
        !strstr(snd_card_name, "sdm670-tasha") &&
        !strstr(snd_card_name, "sdm670-tavil") &&
        !strstr(snd_card_name, "msm8939-tomtom")) {
        stdev->sw_mad = true;
    }
}

static void init_codec_backend_cfg_mixer_ctl(struct platform_data *my_data)
{
    char mixer_ctl[128];

    if (strcmp(my_data->backend_port, "")) {
        snprintf(mixer_ctl, sizeof(mixer_ctl),
                "%s SampleRate", my_data->backend_port);
        my_data->codec_backend_cfg.samplerate_mixer_ctl = strdup(mixer_ctl);

        snprintf(mixer_ctl, sizeof(mixer_ctl),
                "%s Format", my_data->backend_port);
        my_data->codec_backend_cfg.format_mixer_ctl = strdup(mixer_ctl);

        snprintf(mixer_ctl, sizeof(mixer_ctl),
                "%s Channels", my_data->backend_port);
        my_data->codec_backend_cfg.channelcount_mixer_ctl = strdup(mixer_ctl);
    }
}

static void platform_stdev_send_adm_app_type_cfg
(
   void *platform
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    const char *mixer_ctl_name = "Listen App Type Config";
    struct mixer_ctl *ctl;
    struct listnode *p_node, *temp_node;
    struct adm_cfg_info *cfg_info;;
    int app_type_cfg[ST_MAX_LENGTH_MIXER_CONTROL] = {-1};
    int i, len = 0, num_app_types = 0;
    bool update;

    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        return;
    }

    /*
     * The first entry should define number of app type cfgs being sent
     * following which each app type cfg is populated in below order:
     *     - app type
     *     - sample rate
     *     - bit width
     * ex for 2 app type cfgs being sent:
     * 2, 69938, 16000, 16, 69939, 48000, 24
     *
     * Number of app types is determined at the end before setting mixer control
     */
    app_type_cfg[len++] = num_app_types;
    list_for_each_safe(p_node, temp_node, &stdev->adm_cfg_list) {
        cfg_info = node_to_item(p_node, struct adm_cfg_info, list_node);
        update = true;
        for (i = 0; i < len; i = i+3) {
            /* avoid updating duplicate app types */
            if (app_type_cfg[i+1] == -1) {
                break;
            } else if (app_type_cfg[i+1] == cfg_info->app_type) {
                update = false;
                break;
            }
        }

        if (update && ((len + 3) <= ST_MAX_LENGTH_MIXER_CONTROL)) {
            num_app_types += 1;
            app_type_cfg[len++] = cfg_info->app_type;
            app_type_cfg[len++] = cfg_info->sample_rate;
            app_type_cfg[len++] = cfg_info->bit_width;
        }
    }

    ALOGV("%s: num_app_types: %d", __func__, num_app_types);
    if (num_app_types) {
        app_type_cfg[0] = num_app_types;
        mixer_ctl_set_array(ctl, app_type_cfg, len);
    }
}

static bool is_hwmad_backend
(
   struct platform_data *my_data,
   st_device_t st_device,
   st_exec_mode_t exec_mode,
   st_profile_type_t profile_type
)
{
    sound_trigger_device_t *stdev = my_data->stdev;
    bool hwmad_backend = false;

    /*
     * Hwmad backend is used if following cases are true:
     * 1. SWMAD solution is not used
     * 2. ECPP backend is not enabled
     * 3. Execution mode is ADSP and profile type is NONE
     */
    if ((!stdev->sw_mad) &&
        (st_device != ST_DEVICE_HANDSET_MIC_ECPP) &&
        (exec_mode != ST_EXEC_MODE_ARM)) {
        if ((exec_mode == ST_EXEC_MODE_ADSP) &&
            (profile_type != ST_PROFILE_TYPE_NONE))
            hwmad_backend = false;
        else
            hwmad_backend = true;
    }

    return hwmad_backend;
}

static int platform_stdev_get_device_sample_rate
(
   struct platform_data *my_data,
   st_profile_type_t profile_type
)
{
    sound_trigger_device_t *stdev = my_data->stdev;
    struct listnode *p_node, *temp_node;
    struct adm_cfg_info *cfg_info;;
    /* default device sampling rate in acdb */
    int sample_rate = SOUND_TRIGGER_SAMPLING_RATE_48000;

    list_for_each_safe(p_node, temp_node, &stdev->adm_cfg_list) {
        cfg_info = node_to_item(p_node, struct adm_cfg_info, list_node);
        if (cfg_info->profile_type == profile_type) {
            sample_rate = cfg_info->sample_rate;
            break;
        }
    }
    ALOGD("%s: sample rate %d", __func__, sample_rate);

    return sample_rate;
}

static bool is_ec_profile(st_profile_type_t profile_type)
{
    bool ec_profile = false;

    switch (profile_type) {
    case ST_PROFILE_TYPE_EC:
    case ST_PROFILE_TYPE_FLUENCE:
    case ST_PROFILE_TYPE_FLUENCE_STEREO:
        ec_profile = true;
        break;
    default:
        break;
    }

    return ec_profile;
}

static int init_be_dai_name_table(struct platform_data *my_data)
{
    sound_trigger_device_t *stdev = my_data->stdev;
    const char *mixer_ctl_name = "Backend DAI Name Table";
    struct mixer_ctl *ctl;
    int ret, size;

    /*
     * Retrieves the be_dai_name_table from kernel to enable a mapping
     * between sound device hw interfaces and backend IDs.
     * This allows HAL to specify the backend a specific calibration
     * is needed for.
     */

    if (!strcmp(my_data->backend_dai_name, "")) {
        ALOGE("%s: No backend dai name set\n", __func__);
        ret = -EINVAL;
        goto error;
    }

    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: Could not get ctl for mixer name %s\n",
               __func__, mixer_ctl_name);
        ret = -EINVAL;
        goto error;
    }

    mixer_ctl_update(ctl);

    size = mixer_ctl_get_num_values(ctl);
    if (size <= 0) {
        ALOGE("%s: Failed to get %s size %d\n",
               __func__, mixer_ctl_name, size);
        ret = -EFAULT;
        goto error;
    }

    my_data->be_dai_name_table = (struct st_be_dai_name_table *)calloc(1, size);
    if (my_data->be_dai_name_table == NULL) {
        ALOGE("%s: Failed to allocate memory for %s\n",
               __func__, mixer_ctl_name);
        ret = -ENOMEM;
        goto error;
    }

    ret = mixer_ctl_get_array(ctl, (void *)my_data->be_dai_name_table, size);
    if (ret) {
        ALOGE("%s: Failed to get %s, ret %d\n",
               __func__, mixer_ctl_name, ret);
        ret = -EFAULT;
        goto error;
    }

    my_data->max_be_dai_names = size / sizeof(struct st_be_dai_name_table);
    ALOGV("%s: Successfully got %s, number of be dais is %d\n",
           __func__, mixer_ctl_name, my_data->max_be_dai_names);
    return 0;

error:
    if (my_data->be_dai_name_table) {
        free((void *)my_data->be_dai_name_table);
        my_data->be_dai_name_table = NULL;
    }
    return ret;
}

int get_st_device_backend_index
(
   struct platform_data *my_data,
   st_device_t st_device
)
{
    int i;
    int be_dai_id = -EINVAL;

    if ((st_device < ST_DEVICE_MIN) || (st_device >= ST_DEVICE_MAX)) {
        ALOGE("%s: Invalid st_device = %d", __func__, st_device);
        return be_dai_id;
    }

    /* Check if be dai name table was retrieved successfully */
    if (my_data->be_dai_name_table == NULL) {
        ALOGE("%s: BE DAI Name Table is not present\n", __func__);
        return be_dai_id;
    }

    /*
     * Get backend ID for device specified
     * Note:
     * Backend port is currently same for all supported sound trigger devices
     * and hence part of platform data.
     * To support different backend ports, a global interface table per st_device
     * to store backend backend port names is to be defined and override these
     * values from platform info xml values if required.
     */

    for (i = 0; i < my_data->max_be_dai_names; i++) {
        if (strcmp(my_data->backend_dai_name,
                   my_data->be_dai_name_table[i].be_name) == 0) {
            be_dai_id = my_data->be_dai_name_table[i].be_id;
            break;
        }
    }
    if (i == my_data->max_be_dai_names)
        ALOGE("%s: no interface matching name %s\n",
               __func__, my_data->backend_dai_name);

    ALOGV("%s: return backend dai id %d", __func__, be_dai_id);
    return be_dai_id;
}

static int get_snd_card_num(struct platform_data *my_data)
{
    struct mixer *mixer = NULL;
    int retry_num = 0, retry_limit = 0, snd_card_num = 0;
    const char* snd_card_name = NULL;
    bool card_verifed[MAX_SND_CARD] = {false};
    char value[PROPERTY_VALUE_MAX];
    char *default_value = "10";

    property_get("vendor.audio.snd_card.open.retries",
                                 value, default_value);
    retry_limit = atoi(value);

    for (;;) {
        if (snd_card_num >= MAX_SND_CARD) {
            if (retry_num++ >= retry_limit) {
                ALOGE("%s: primary sound card not found", __func__);
                break;
            }
            snd_card_num = 0;
            usleep(RETRY_US);
            continue;
        }
        if (card_verifed[snd_card_num]) {
            snd_card_num++;
            continue;
        }

        mixer = mixer_open(snd_card_num);
        if (!mixer) {
            ALOGE("%s: Unable to open the mixer card %d",
                  __func__, snd_card_num);
            snd_card_num++;
            continue;
        }

        snd_card_name = mixer_get_name(mixer);
        if (!strcmp(my_data->snd_card_name, "")) {
            if (strstr(snd_card_name, "msm") || strstr(snd_card_name, "sdm") ||
                strstr(snd_card_name, "apq"))
                break;
        } else {
            if (!strncmp(snd_card_name, my_data->snd_card_name,
                sizeof(my_data->snd_card_name)))
                break;
        }

        ALOGD("%s: sound card %s is not primary, skipping",
               __func__, snd_card_name);
        mixer_close(mixer);
        mixer = NULL;
        card_verifed[snd_card_num] = true;
        snd_card_num++;
    }
    if (mixer)
        mixer_close(mixer);

    return snd_card_num;
}

void *platform_stdev_init(sound_trigger_device_t *stdev)
{
    int ret = 0, retry_num = 0, snd_card_num = 0;
    struct platform_data *my_data = NULL;
    const char *snd_card_name = NULL;
    const char *acdb_card_name = NULL;
    audio_hw_get_snd_card_num_t audio_hw_get_snd_card_num = NULL;
    char mixer_path_xml[100];
    struct listnode *v_node, *temp_node;
    struct st_vendor_info* v_info;
    char dev_name[256];

    ALOGI("%s: Enter", __func__);
    my_data = calloc(1, sizeof(struct platform_data));

    if (!my_data || !stdev) {
        ALOGE("%s: ERROR. NULL param", __func__);
        if (my_data)
            free(my_data);
        return NULL;
    }
    my_data->stdev = stdev;
    list_init(&stdev->vendor_uuid_list);
    list_init(&stdev->adm_cfg_list);

    platform_stdev_set_default_config(my_data);
    platform_parse_info(my_data, PLATFORM_PATH_XML);

    audio_hw_get_snd_card_num =
        (audio_hw_get_snd_card_num_t)dlsym(stdev->audio_hal_handle,
                                          "audio_extn_utils_get_snd_card_num");
    if (audio_hw_get_snd_card_num) {
        snd_card_num = audio_hw_get_snd_card_num();
        if (snd_card_num < 0) {
            ALOGE("%s: ERROR. Unable to get sound card num", __func__);
            goto cleanup;
        }
    } else {
        ALOGE("%s: ERROR. dlsym error %s for audio_extn_utils_get_snd_card_num",
             __func__, dlerror());

        snd_card_num = get_snd_card_num(my_data);
        if (snd_card_num < 0) {
            ALOGE("%s: ERROR. Unable to get sound card num", __func__);
            goto cleanup;
        }
    }

    stdev->mixer = mixer_open(snd_card_num);
    while (!stdev->mixer && retry_num < MIXER_OPEN_MAX_NUM_RETRY) {
        usleep(RETRY_US);
        stdev->mixer = mixer_open(snd_card_num);
        retry_num++;
    }

    if (!stdev->mixer) {
        ALOGE("%s: ERROR. Unable to open the mixer, aborting", __func__);
        goto cleanup;
    }

    snd_card_name = mixer_get_name(stdev->mixer);

    query_stdev_platform(stdev, snd_card_name, mixer_path_xml);
    stdev->audio_route = audio_route_init(snd_card_num, mixer_path_xml);
    if (!stdev->audio_route) {
        ALOGE("%s: ERROR. Failed to init audio route controls, aborting.",
                __func__);
        goto cleanup;
    }
    stdev->snd_card = snd_card_num;

    ret = load_acdb(my_data);
    if (ret)
        goto cleanup;

    if (my_data->audio_hw_acdb_init) {
        ret = my_data->audio_hw_acdb_init(snd_card_num);
        if (ret) {
            ALOGE("%s: ERROR. audio_hw_acdb_init failed status %d", __func__, ret);
            goto cleanup;
        }
    } else {
        acdb_card_name = get_snd_card_name_for_acdb_loader(snd_card_name);
        ALOGI("%s: acdb_init: %s", __func__, acdb_card_name);
        ret = my_data->acdb_init(acdb_card_name, NULL, 0);
        if (acdb_card_name) {
            free((void*)acdb_card_name);
            acdb_card_name = NULL;
        }
        if (ret) {
            ALOGE("%s: ERROR. acdb_loader_init_v2 failed status %d", __func__, ret);
            goto cleanup;
        }
    }

    snprintf(dev_name, sizeof(dev_name), "/dev/snd/hwC%uD%u",
                               snd_card_num, WCD9XXX_CODEC_HWDEP_NODE);
    ALOGD("%s Opening device %s", __func__, dev_name);
    my_data->hwdep_fd = open(dev_name, O_WRONLY);
    if (my_data->hwdep_fd < 0) {
        ALOGE("%s: cannot open device '%s'", __func__, dev_name);
        my_data->acdb_deinit();
        goto cleanup;
    }
    my_data->prev_acdb_id = -1;

    ret = get_codec_version(my_data, snd_card_name, snd_card_num);
    if (ret)
        goto cleanup;

    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if (!memcmp(&v_info->uuid, &qti_uuid, sizeof(sound_trigger_uuid_t))) {
            v_info->is_qc_uuid = true;
        } else {
            ALOGV("%s: ISV uuid present", __func__);
        }
        if (!stdev->adpcm_dec_lib_handle &&
             (v_info->kw_capture_format & ADPCM)) {
            ret = load_adpcm_decoder(stdev);
            if (ret)
                goto cleanup;
        }
        if (!stdev->mulaw_dec_lib_handle &&
             (v_info->kw_capture_format & MULAW)) {
            ret = load_mulaw_decoder(stdev);
            if (ret)
                goto cleanup;
        }
        ALOGV("%s: vendor config: kcf=%d, ktm=%d, ckw=%d, cu=%d, akw=%d, au=%d",
              __func__, v_info->kw_capture_format, v_info->kw_transfer_mode,
              v_info->avail_cpe_phrases, v_info->avail_cpe_users,
              v_info->avail_ape_phrases, v_info->avail_ape_users);
    }

    /* gcs init must happen after acdb inititlization as gcs client has
       dependency on acdb inititialization */
    if (stdev->is_gcs) {
        ret = st_hw_gcs_init();
        if (ret)
            goto cleanup;
    }
    ret = st_hw_pcm_init();
    if (ret)
        goto cleanup_2;

    init_codec_backend_cfg_mixer_ctl(my_data);

    platform_stdev_send_adm_app_type_cfg(my_data);

    init_be_dai_name_table(my_data);

    return my_data;

cleanup_2:
    if (stdev->is_gcs)
        st_hw_gcs_deinit();

cleanup:
    if (stdev->mulaw_dec_lib_handle)
        dlclose(stdev->mulaw_dec_lib_handle);

    if (stdev->adpcm_dec_lib_handle)
        dlclose(stdev->adpcm_dec_lib_handle);

    if (my_data->acdb_handle)
        dlclose(my_data->acdb_handle);

    if (stdev->smlib_handle)
        dlclose(stdev->smlib_handle);

    if (stdev->audio_route)
        audio_route_free(stdev->audio_route);

    if (stdev->mixer)
        mixer_close(stdev->mixer);

    list_for_each_safe(v_node, temp_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if (v_info->smlib_handle)
            dlclose(v_info->smlib_handle);
        list_remove(v_node);
        free(v_info);
    }
    if (my_data->hwdep_fd >= 0) {
        close(my_data->hwdep_fd);
        my_data->hwdep_fd = -1;
    }
    if (my_data->kvpairs)
        str_parms_destroy(my_data->kvpairs);

    free(my_data);
    return NULL;
}

void platform_stdev_deinit(void *platform)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct listnode *v_node, *temp_node, *gcs_node, *gcs_temp_node;
    struct st_vendor_info* v_info;
    struct st_gcs_params *gcs_info;

    ALOGI("%s: Enter", __func__);
    if (my_data) {
        list_for_each_safe(v_node, temp_node, &my_data->stdev->vendor_uuid_list) {
            v_info = node_to_item(v_node, struct st_vendor_info, list_node);

            /* remove and free the gcs uid params */
            list_for_each_safe(gcs_node, gcs_temp_node, &v_info->gcs_usecase_list) {
                gcs_info = node_to_item(gcs_node, struct st_gcs_params, list_node);
                list_remove(gcs_node);
                free(gcs_info);
            }

            if (v_info->smlib_handle)
                dlclose(v_info->smlib_handle);
            list_remove(v_node);
            free(v_info);
        }
        my_data->acdb_deinit();
        dlclose(my_data->acdb_handle);
        if (my_data->stdev->smlib_handle)
            dlclose(my_data->stdev->smlib_handle);
        if (my_data->stdev->adpcm_dec_lib_handle)
            dlclose(my_data->stdev->adpcm_dec_lib_handle);
        if (my_data->stdev->mulaw_dec_lib_handle)
            dlclose(my_data->stdev->mulaw_dec_lib_handle);
        audio_route_free(my_data->stdev->audio_route);
        mixer_close(my_data->stdev->mixer);
        if (my_data->hwdep_fd >= 0) {
            close(my_data->hwdep_fd);
            my_data->hwdep_fd = -1;
        }
        if (my_data->kvpairs)
            str_parms_destroy(my_data->kvpairs);

        if (my_data->stdev->is_gcs) {
            st_hw_gcs_deinit();
        }
        st_hw_pcm_deinit();

        if (my_data->be_dai_name_table)
            free((void *)my_data->be_dai_name_table);

        free(my_data);
    }
}

struct st_vendor_info* platform_stdev_get_vendor_info
(
   void *platform,
   sound_trigger_uuid_t *vendor_uuid
)
{
    struct listnode *v_node;
    struct st_vendor_info *v_info = NULL, *vendor_info = NULL;
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;

    if (!platform || !vendor_uuid) {
        ALOGE("%s: NULL inputs", __func__);
        return NULL;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return NULL;
    }
    stdev = my_data->stdev;

    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if (!memcmp(&v_info->uuid, vendor_uuid, sizeof(sound_trigger_uuid_t))) {
            ALOGV("%s: Matched uuid", __func__);
            vendor_info = v_info;
            break;
        }
    }

    return vendor_info;
}

static void check_and_append_device_name
(
   struct platform_data *my_data,
   char *device_name
)
{
    int alt_mixer_control_value = 0;
    char device_name_suffix[10];

    /* Alternate configuration is supported for quad mic currently.
     * Do not add suffix if bad mic channel index is not set.
     */
    if ((my_data->codec_backend_cfg.channel_count != SOUND_TRIGGER_CHANNEL_MODE_QUAD) ||
        (my_data->bad_mic_channel_index == 0)) {
        ALOGV("%s: No requirement to add suffix to device name", __func__);
        return;
    }

    switch (my_data->bad_mic_channel_index) {
    case BAD_MIC_CH_INDEX_0:
        alt_mixer_control_value = 1;
        break;
    case BAD_MIC_CH_INDEX_1:
        alt_mixer_control_value = 2;
        break;
    case BAD_MIC_CH_INDEX_2:
        alt_mixer_control_value = 3;
        break;
    case BAD_MIC_CH_INDEX_3:
    default:
        break;
    }

    if (alt_mixer_control_value) {
        snprintf(device_name_suffix, sizeof(device_name_suffix), "-alt%d",
                              alt_mixer_control_value);
        strlcat(device_name, device_name_suffix, DEVICE_NAME_MAX_SIZE);
        ALOGV("%s: appended device name %s", __func__, device_name);
    }
}

int platform_stdev_get_device_name
(
   void *platform,
   st_exec_mode_t exec_mode,
   st_device_t st_device,
   char *device_name
)
{
    struct platform_data *my_data = (struct platform_data *)platform;

    if ((st_device >= ST_DEVICE_MIN && st_device < ST_DEVICE_MAX) &&
        (exec_mode > ST_EXEC_MODE_NONE && exec_mode < ST_EXEC_MODE_MAX)) {
        strlcpy(device_name, st_device_table[exec_mode][st_device], DEVICE_NAME_MAX_SIZE);

        if ((strstr(my_data->codec_version, "WCD9335_1_0") ||
            strstr(my_data->codec_version, "WCD9335_1_1")) &&
            !my_data->stdev->run_on_ape) {
            strlcat(device_name, " low-speed-intf", DEVICE_NAME_MAX_SIZE);
        } else if (my_data->stdev->run_on_ape) {
            check_and_append_device_name(my_data, device_name);
        }
    } else {
        strlcpy(device_name, "", DEVICE_NAME_MAX_SIZE);
        return -EINVAL;
    }
    return 0;
}

int platform_stdev_get_device
(
   void *platform,
   struct st_vendor_info* v_info,
   audio_devices_t device,
   enum st_exec_mode exec_mode
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    st_device_t st_device = ST_DEVICE_NONE;
    int channel_count;

    switch (device) {
    case AUDIO_DEVICE_IN_WIRED_HEADSET:
        if (ST_EXEC_MODE_CPE == exec_mode) {
            st_device = ST_DEVICE_HEADSET_MIC;
            break;
        }
    case AUDIO_DEVICE_IN_BUILTIN_MIC:
        if (ST_EXEC_MODE_CPE == exec_mode) {
            channel_count = (v_info == NULL) ? SOUND_TRIGGER_CHANNEL_MODE_MONO :
                   get_channel_cnt_from_fluence_type(v_info->wdsp_fluence_type);

            if (v_info &&
                (v_info->sample_rate == SOUND_TRIGGER_SAMPLING_RATE_48000))
                st_device = ST_DEVICE_HANDSET_MIC_ECPP;
            else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_QUAD)
                st_device = ST_DEVICE_HANDSET_QMIC;
            else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_TRI)
                st_device = ST_DEVICE_HANDSET_TMIC;
            else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_STEREO)
                st_device = ST_DEVICE_HANDSET_DMIC;
            else
                st_device = ST_DEVICE_HANDSET_MIC;
        } else {
            /* ADSP/ARM mode */
            channel_count = my_data->codec_backend_cfg.channel_count;
            if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_OCT) {
                st_device = ST_DEVICE_HANDSET_8MIC;
            } else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_HEX) {
                st_device = ST_DEVICE_HANDSET_6MIC;
            } else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_QUAD) {
                st_device = ST_DEVICE_HANDSET_QMIC;
            } else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_TRI) {
                st_device = ST_DEVICE_HANDSET_TMIC;
            } else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_STEREO) {
                st_device = ST_DEVICE_HANDSET_DMIC;
            } else if (channel_count == SOUND_TRIGGER_CHANNEL_MODE_MONO) {
                if (v_info &&
                    (v_info->profile_type != ST_PROFILE_TYPE_NONE))
                    st_device = ST_DEVICE_HANDSET_MIC_PP;
                else
                    st_device = ST_DEVICE_HANDSET_MIC;
            } else {
                ALOGE("%s: Invalid channel count %d", __func__, channel_count);
            }
        }
        break;
    default:
        ALOGD("%s: returing handset mic device", __func__);
        st_device = ST_DEVICE_HANDSET_MIC;
        break;
    }

    ALOGV("%s: sample_rate=%d st_device %d", __func__,
          v_info ? v_info->sample_rate : 0, st_device);
    return st_device;
}

audio_devices_t platform_stdev_get_capture_device
(
   void *platform
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    audio_devices_t device = AUDIO_DEVICE_NONE;
    audio_devices_t avail_devices = stdev->available_devices &
                                     ~AUDIO_DEVICE_BIT_IN;

    if (avail_devices & AUDIO_DEVICE_IN_WIRED_HEADSET)
        device = AUDIO_DEVICE_IN_WIRED_HEADSET;
    else if (avail_devices & AUDIO_DEVICE_IN_BUILTIN_MIC)
        device = AUDIO_DEVICE_IN_BUILTIN_MIC;

    ALOGD("%s: available devices 0x%x, device 0x%x", __func__,
                         stdev->available_devices, device);
    return device;
}

int platform_stdev_update_avail_device
(
    void *platform,
    audio_devices_t device,
    bool connect
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;

    if (connect)
        stdev->available_devices |= device;
    else
        stdev->available_devices &= ~device;

    return 0;
}

static int platform_stdev_send_hwmad_cal
(
   struct platform_data *my_data,
   int acdb_id
)
{
    int ret = 0;
    struct wcdcal_ioctl_buffer codec_buffer;
    struct hwdep_cal_param_data calib;
    int hwdep_fd = my_data->hwdep_fd;

    if (my_data->prev_acdb_id == acdb_id) {
        ALOGD("%s: previous acdb_id %d matches new acdb_id %d return",
              __func__, my_data->prev_acdb_id, acdb_id);
        return 0;
    }

    ATRACE_BEGIN("sthal: send_hwmad_cal");
    calib.acdb_id = acdb_id;
    calib.get_size = 1;
    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
        ATRACE_END();
        ALOGE("%s: get_calibration to get size failed", __func__);
        return ret;
    }

    calib.get_size = 0;
    calib.buff = malloc(calib.buff_size);
    if (!calib.buff) {
        ATRACE_END();
        ALOGE("%s: malloc calib of buff size %d failed",
                  __func__, calib.buff_size);
        return -ENOMEM;
    }

    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
        ATRACE_END();
        ALOGE("%s: get_calibration to get size failed", __func__);
        free(calib.buff);
        return ret;
    }

    codec_buffer.buffer = calib.buff;
    codec_buffer.size = calib.data_size;
    codec_buffer.cal_type = WCD9XXX_MAD_CAL;
    ret = ioctl(hwdep_fd, SNDRV_CTL_IOCTL_HWDEP_CAL_TYPE, &codec_buffer);
    if (ret < 0) {
        ALOGE("%s: failed to call ioctl err=%d",__func__, errno);
    } else {
        my_data->prev_acdb_id = acdb_id;
        ALOGD("%s hwmad cal sent for acdb_id (%d)", __func__, acdb_id);
    }

    free(calib.buff);
    ATRACE_END();
    return ret;
}

int platform_stdev_get_acdb_id
(
    st_device_t st_device,
    st_exec_mode_t exec_mode
)
{
    int acdb_id;

    acdb_id = acdb_device_table[exec_mode][st_device];
    if (acdb_id < 0) {
        ALOGE("%s: Could not find acdb id for device(%d)",
              __func__, st_device);
        return -EINVAL;
    }

    return acdb_id;
}

int platform_stdev_send_calibration
(
    void *platform,
    audio_devices_t device,
    st_exec_mode_t exec_mode,
    struct st_vendor_info *v_info,
    int app_id,
    bool use_topology,
    st_cal_type type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int acdb_id;
    st_device_t st_device;
    int status = 0;
    int hw_type, sample_rate;
    st_profile_type_t profile_type;

    st_device = platform_stdev_get_device(my_data,
        v_info, device, exec_mode);
    if (st_device == ST_DEVICE_NONE) {
        ALOGE("%s: Could not find valid device",__func__);
        return -EINVAL;
    }

    acdb_id = acdb_device_table[exec_mode][st_device];
    if (acdb_id < 0) {
        ALOGE("%s: Could not find acdb id for device(%d)",
              __func__, st_device);
        return -EINVAL;
    }
    hw_type = (exec_mode == ST_EXEC_MODE_CPE) ? 0 : 1;

    switch (type) {
    case ST_SESSION_CAL:
        if (my_data->acdb_send_lsm_cal) {
            ALOGD("%s: sending lsm calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            /* ACDB modes: topology=1, non-topology=0 */
            ATRACE_BEGIN("sthal: acdb_send_lsm_cal");
            status = my_data->acdb_send_lsm_cal(acdb_id, app_id,
                                                (use_topology == true),
                                                hw_type);
            ATRACE_END();
        }
        break;

    case ST_DEVICE_CAL:
        profile_type = v_info ?
                       v_info->profile_type : ST_PROFILE_TYPE_NONE;
        sample_rate = platform_stdev_get_device_sample_rate(my_data, profile_type);

        if (is_hwmad_backend(platform, st_device, exec_mode, profile_type)) {
            if (platform_stdev_send_hwmad_cal(platform, acdb_id))
                return -EINVAL;
        }

        if (my_data->acdb_send_device_cal) {
            ALOGD("%s: sending device calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            ATRACE_BEGIN("sthal: acdb_send_device_cal");
            my_data->acdb_send_device_cal(acdb_id, hw_type, app_id, sample_rate);
            ATRACE_END();
        }
        break;

    default:
        ALOGE("%s: invalid cal type %d",__func__,type);
        return -EINVAL;
    }

    return status;
}

bool platform_stdev_check_and_update_concurrency
(
   void *platform,
   audio_event_type_t event_type,
   unsigned int num_sessions
)
{
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;
    bool concurrency_ses_allowed = true;

    if (!platform) {
        ALOGE("%s: NULL platform", __func__);
        return false;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return false;
    }
    stdev = my_data->stdev;

    switch (event_type) {
    case AUDIO_EVENT_CAPTURE_DEVICE_ACTIVE:
    case AUDIO_EVENT_CAPTURE_STREAM_ACTIVE:
        stdev->tx_concurrency_active++;
        break;
    case AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE:
    case AUDIO_EVENT_CAPTURE_STREAM_INACTIVE:
        if (stdev->tx_concurrency_active > 0)
            stdev->tx_concurrency_active--;
        break;
    case AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE:
            stdev->rx_concurrency_active++;
        break;
    case AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE:
        if (stdev->rx_concurrency_active > 0)
            stdev->rx_concurrency_active--;
        break;
    default:
        break;
    }
    if ((stdev->tx_concurrency_active > 0) ||
        (stdev->rx_concurrency_disabled &&
          stdev->rx_concurrency_active > 0 &&
          num_sessions > stdev->rx_conc_max_st_ses)) {
        concurrency_ses_allowed = false;
    }

    ALOGD("%s: concurrency active %d, tx %d, rx %d, concurrency session_allowed %d",
          __func__, stdev->audio_concurrency_active, stdev->tx_concurrency_active,
          stdev->rx_concurrency_active, concurrency_ses_allowed);
    return concurrency_ses_allowed;
}

bool platform_stdev_is_session_allowed
(
    void *platform,
    unsigned int num_sessions
)
{
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;
    bool session_allowed = true;

    if (!platform) {
        ALOGE("%s: NULL platform", __func__);
        return false;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return false;
    }
    stdev = my_data->stdev;

    if (!stdev->sw_mad) {
        if (stdev->tx_concurrency_active > 0)
            session_allowed = false;
        stdev->audio_concurrency_active = session_allowed ? false: true;

    } else {
        /* sw_mad case num_sessions does not reflect
            current session yet */
        if ((stdev->tx_concurrency_active > 0) ||
            (stdev->rx_concurrency_disabled &&
            stdev->rx_concurrency_active > 0 &&
            (num_sessions + 1) > stdev->rx_conc_max_st_ses)) {
            session_allowed = false;
        }

        stdev->audio_concurrency_active = session_allowed ? false: true;
    }

    ALOGD("%s: stdev->audio_concurrency_active %d session_allowed %d", __func__,
          stdev->audio_concurrency_active, session_allowed);
    return session_allowed;
}

void platform_stdev_check_and_update_pcm_config
(
   void *platform,
   struct pcm_config *config,
   struct st_vendor_info *v_info,
   enum st_exec_mode exec_mode
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    const char *snd_card_name;

    /* Update config with params in vendor info */
    if (v_info) {
        config->rate = v_info->sample_rate;
        config->format = v_info->format;
        config->channels = v_info->channel_count;
    }

    if (exec_mode == ST_EXEC_MODE_CPE) {
        if (my_data->stdev->is_gcs) {
            if (v_info && (v_info->wdsp_fluence_type == ST_FLUENCE_TYPE_DMIC))
                config->channels = SOUND_TRIGGER_CHANNEL_MODE_STEREO;
            else
                config->channels = SOUND_TRIGGER_CHANNEL_MODE_MONO;
        } else {
            snd_card_name = mixer_get_name(my_data->stdev->mixer);
            if (strstr(snd_card_name, "tomtom") ||
                strstr(my_data->codec_version, "WCD9335_1_0") ||
                strstr(my_data->codec_version, "WCD9335_1_1")) {
                /* tomtom, Tasha1.0, Tasha1.1 support max 16KHz/24bit bandwidth */
                config->rate = SOUND_TRIGGER_SAMPLING_RATE_16000;
                if (config->format != PCM_FORMAT_S16_LE)
                    config->format = PCM_FORMAT_S24_LE;
            } else {
                /* Other codecs configured at ftrt 384Khz/32bit */
                config->rate = SOUND_TRIGGER_SAMPLING_RATE_384000;
                config->format = PCM_FORMAT_S32_LE;
            }
        }
    }
}

int platform_stdev_connect_mad
(
   void *platform,
   st_exec_mode_t mode,
   st_profile_type_t profile_type
)
{
    /* This mixer control is only valid for CPE supported codec */
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    int status = 0;
    struct mixer_ctl *ctl = NULL;
    const char *mixer_ctl_name = "MAD_SEL MUX";

    if (stdev->sw_mad || (mode == ST_EXEC_MODE_NONE) ||
        ((mode == ST_EXEC_MODE_ADSP) &&
         (profile_type != ST_PROFILE_TYPE_NONE)) ||
        (mode == ST_EXEC_MODE_ARM))
        return 0;

    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        return -EINVAL;
    }
    if (mode == ST_EXEC_MODE_ADSP)
        status = mixer_ctl_set_enum_by_string(ctl, "MSM");
    else
        status = mixer_ctl_set_enum_by_string(ctl, "SPE");

    if (status)
        ALOGE("%s: ERROR. Mixer ctl set failed", __func__);

    return status;
}

int platform_stdev_get_hw_type(void *platform)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;

    int dev_id;
    unsigned int i = 0, j = 0, k = 0;
    char comp_string[USECASE_STRING_SIZE];
    char *line = NULL;
    size_t len = 0;
    FILE *fp;

    fp = fopen(PCM_DEVICE_FILE, "r");

    if (!fp) {
        ALOGE("%s: ERROR. %s file open failed", __func__, PCM_DEVICE_FILE);
        return -ENODEV;
    }
    snprintf(comp_string, USECASE_STRING_SIZE, "Listen %d Audio Service", i+1);

    while(getline(&line, &len, fp) != -1) {
        if (strstr(line, comp_string) && (i < stdev->max_ape_sessions)) {
            stdev->hw_type |= ST_DEVICE_HW_APE;
            snprintf(stdev->ape_pcm_use_cases[i].use_case,
                     USECASE_STRING_SIZE, "listen-voice-wakeup-%d", i+1);

            if (sscanf(&line[3], "%d", &dev_id) == 1)
                stdev->ape_pcm_use_cases[i++].pcm_id = dev_id;
            else
                ALOGE("%s: ERROR. adsp pcm dev_id parse failed", __func__);

            snprintf(comp_string, USECASE_STRING_SIZE, "Listen %d Audio Service", i+1);
        } else if (strstr(line, "CPE Listen") && (j < stdev->max_cpe_sessions)) {
            stdev->hw_type |= ST_DEVICE_HW_CPE;
            /* store the Back End types associated with the Front End*/
            if (strstr(line, "ECPP"))
                stdev->cpe_pcm_use_cases[j].pcm_back_end = CPE_PCM_BACK_END_ECPP;
            else
                stdev->cpe_pcm_use_cases[j].pcm_back_end = CPE_PCM_BACK_END_DEFAULT;

            if (sscanf(&line[3], "%d", &dev_id) == 1)
                stdev->cpe_pcm_use_cases[j++].pcm_id = dev_id;
            else
                ALOGE("%s: ERROR. cpe pcm dev_id parse failed", __func__);
        } else if (strstr(line, CAPTURE_PCM_USECASE_STRING)) {
            stdev->hw_type |= ST_DEVICE_HW_ARM;
            if (sscanf(&line[3], "%d", &dev_id) == 1)
                stdev->arm_pcm_use_cases[CAPTURE_PCM_USECASE_INDEX].pcm_id = dev_id;
            else
                ALOGE("%s: ERROR. arm pcm dev_id parse failed", __func__);

            strlcpy(stdev->arm_pcm_use_cases[CAPTURE_PCM_USECASE_INDEX].use_case,
                    CAPTURE_PCM_USECASE_PATH, USECASE_STRING_SIZE);
            k++;
        } else if (strstr(line, CAPTURE_EC_REF_USECASE_STRING)) {
            if (sscanf(&line[3], "%d", &dev_id) == 1)
                stdev->arm_pcm_use_cases[CAPTURE_EC_REF_USECASE_INDEX].pcm_id = dev_id;
            else
                ALOGE("%s: ERROR. arm pcm dev_id parse failed", __func__);

            strlcpy(stdev->arm_pcm_use_cases[CAPTURE_EC_REF_USECASE_INDEX].use_case,
                    CAPTURE_EC_REF_USECASE_PATH, USECASE_STRING_SIZE);
        }
    }

    free(line);
    fclose(fp);

    if (!CHECK_BIT(stdev->hw_type,
            ST_DEVICE_HW_APE|ST_DEVICE_HW_CPE|ST_DEVICE_HW_ARM)) {
        ALOGE("%s: ERROR. No Listen device present", __func__);
        return -ENODEV;
    }

    if (stdev->max_ape_sessions > i) {
        ALOGW("%s: max_ape_sessions platform configured=%d > driver supported=%d",
              __func__, stdev->max_ape_sessions, i);
        stdev->max_ape_sessions = i;
    }
    if (stdev->max_cpe_sessions > j) {
        ALOGW("%s: max_cpe_sessions platform configured=%d > driver supported=%d",
              __func__, stdev->max_cpe_sessions, j);
        stdev->max_cpe_sessions = j;
    }
    if (stdev->max_arm_sessions > k) {
        ALOGW("%s: max_arm_sessions platform configured=%d > driver supported=%d",
              __func__, stdev->max_arm_sessions, k);
        stdev->max_arm_sessions = k;
    }

    return 0;
}

int platform_cpe_get_pcm_device_id
(
   void *platform,
   int sample_rate,
   unsigned int* use_case_idx
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;
    int ret = -1;
    bool found = false;

    for (i = 0; i < stdev->max_cpe_sessions; i++) {
        if (!stdev->cpe_pcm_use_cases[i].active) {
            /* pcm device nodes are fixed based on backend types. Check for
            supported combination. */
            if (my_data->cpe_fe_to_be_fixed &&
                sample_rate == SOUND_TRIGGER_SAMPLING_RATE_48000) {
                if (stdev->cpe_pcm_use_cases[i].pcm_back_end == CPE_PCM_BACK_END_ECPP) {
                    found = true;
                }
            } else {
                found = true;
            }

            if (found) {
                stdev->cpe_pcm_use_cases[i].active = true;
                ret = stdev->cpe_pcm_use_cases[i].pcm_id;
                *use_case_idx = i;
                break;
            }
        }
    }

    if (ret < 0)
        ALOGE("%s: ERROR. no free pcm device available", __func__);

    return ret;
}

int platform_ape_get_pcm_device_id
(
   void *platform,
   unsigned int* use_case_idx
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;
    int ret = -1;

    for (i = 0; i < stdev->max_ape_sessions; i++) {
        if (!stdev->ape_pcm_use_cases[i].active) {
            stdev->ape_pcm_use_cases[i].active = true;
            ret = stdev->ape_pcm_use_cases[i].pcm_id;
            *use_case_idx = i;
            break;
        }
    }
    if (ret < 0)
        ALOGE("%s: ERROR. no free pcm device available", __func__);

    return ret;
}

void platform_ape_free_pcm_device_id
(
   void *platform,
   int pcm_id
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;

    for (i = 0; i < stdev->max_ape_sessions; i++) {
        if (stdev->ape_pcm_use_cases[i].pcm_id == pcm_id) {
            stdev->ape_pcm_use_cases[i].active = false;
            break;
        }
    }
}

void platform_cpe_free_pcm_device_id
(
   void *platform,
   int pcm_id
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;

    for (i = 0; i < stdev->max_cpe_sessions; i++) {
        if (stdev->cpe_pcm_use_cases[i].pcm_id == pcm_id) {
            stdev->cpe_pcm_use_cases[i].active = false;
            break;
        }
    }
}

int platform_arm_get_pcm_device_id
(
   void *platform,
   unsigned int* use_case_idx,
   bool ec_ref_dev
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i = CAPTURE_PCM_USECASE_INDEX;
    int ret = -1;

    if (ec_ref_dev)
        i = CAPTURE_EC_REF_USECASE_INDEX;

    if (!stdev->arm_pcm_use_cases[i].active) {
        stdev->arm_pcm_use_cases[i].active = true;
        ret = stdev->arm_pcm_use_cases[i].pcm_id;
        *use_case_idx = i;
    }
    if (ret < 0)
        ALOGE("%s: ERROR. no free pcm device available", __func__);

    return ret;
}

void platform_arm_free_pcm_device_id
(
   void *platform,
   int pcm_id,
   bool ec_ref_dev
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i = CAPTURE_PCM_USECASE_INDEX;

    if (ec_ref_dev)
        i = CAPTURE_EC_REF_USECASE_INDEX;

    if (stdev->arm_pcm_use_cases[i].pcm_id == pcm_id)
        stdev->arm_pcm_use_cases[i].active = false;
}

void platform_alloc_gcs_usecase
(
    void *platform,
    struct st_vendor_info *v_info,
    struct st_gcs_params **gcs_usecase,
    unsigned int acdb_id
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct st_gcs_params *usecase = NULL;
    struct listnode *gcs_node, *node;
    int i;

    ALOGV("%s: enter", __func__);

    if (!v_info || !gcs_usecase) {
        ALOGE("%s: received null params", __func__);
        return;
    }

    if (list_empty(&v_info->gcs_usecase_list)) {
        *gcs_usecase = NULL;
        ALOGE("%s: gcs usecase not available", __func__);
        return;
    }

    if (my_data->xml_version >= PLATFORM_XML_VERSION_0x0102) {
        list_for_each_safe(gcs_node, node, &v_info->gcs_usecase_list) {
            usecase = node_to_item(gcs_node, struct st_gcs_params, list_node);
            i = 0;
            while ((i < MAX_GCS_USECASE_ACDB_IDS) && usecase->acdb_ids[i]) {
                if (usecase->acdb_ids[i] == acdb_id) {
                    *gcs_usecase = usecase;
                    list_remove(gcs_node);
                    return;
                }
                i++;
            }
        }
        ALOGE("%s: Found no matching acdb id", __func__);
        *gcs_usecase = NULL;
    } else {
        gcs_node = list_head(&v_info->gcs_usecase_list);
        *gcs_usecase = node_to_item(gcs_node, struct st_gcs_params, list_node);
        list_remove(gcs_node);
    }
}

void platform_free_gcs_usecase
(
    struct st_vendor_info *v_info,
    struct st_gcs_params *gcs_usecase
)
{
    ALOGV("%s: enter", __func__);

    if (!v_info || !gcs_usecase) {
        ALOGE("%s: received null params", __func__);
        return;
    }

    list_add_head(&v_info->gcs_usecase_list, &gcs_usecase->list_node);
}

int platform_stdev_check_and_set_codec_backend_cfg
(
   void *platform,
   struct st_vendor_info *v_info,
   bool *backend_cfg_change
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    struct  mixer_ctl *ctl;
    int channel_count;

    if (!backend_cfg_change) {
        ALOGE("%s: NULL backend", __func__);
        return -EINVAL;
    }

    *backend_cfg_change = false;

    if (!v_info)
        return 0;

    if (v_info->format != my_data->codec_backend_cfg.format) {
        if (my_data->codec_backend_cfg.format != PCM_FORMAT_S24_LE &&
            my_data->codec_backend_cfg.format_mixer_ctl) {
            ctl = mixer_get_ctl_by_name(stdev->mixer,
                        my_data->codec_backend_cfg.format_mixer_ctl);
            if (!ctl) {
                ALOGE("%s: Could not get ctl for mixer command - %s",
                      __func__, my_data->codec_backend_cfg.format_mixer_ctl);
                return -EINVAL;
            }

            if (v_info->format == PCM_FORMAT_S24_LE) {
                mixer_ctl_set_enum_by_string(ctl, "S24_LE");
            } else if (v_info->format == PCM_FORMAT_S16_LE) {
                mixer_ctl_set_enum_by_string(ctl, "S16_LE");
            } else {
                ALOGE("%s: Invalid format %d", __func__, v_info->format);
                return -EINVAL;
            }
            *backend_cfg_change = true;
            my_data->codec_backend_cfg.format = v_info->format;
        }
    }

    /*
     * Channel count for backend is determined from configuration of
     * lsm session except in case of fluence profile.
     * In case of fluence, backend configuration is obtained from
     * fluence type set.
     */
    if (v_info->profile_type == ST_PROFILE_TYPE_FLUENCE ||
        v_info->profile_type == ST_PROFILE_TYPE_FLUENCE_STEREO)
        channel_count = get_channel_cnt_from_fluence_type(v_info->fluence_type);
    else
        channel_count = v_info->channel_count;

    if ((channel_count != my_data->codec_backend_cfg.channel_count) &&
        (my_data->codec_backend_cfg.channel_count != SOUND_TRIGGER_CHANNEL_MODE_OCT)) {
        *backend_cfg_change = true;
        my_data->codec_backend_cfg.channel_count = channel_count;
    }

    return 0;
}

int platform_stdev_send_stream_app_type_cfg
(
   void *platform,
   int pcm_id,
   st_device_t st_device,
   st_exec_mode_t exec_mode,
   st_profile_type_t profile_type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    char mixer_ctl_name[ST_MAX_LENGTH_MIXER_CONTROL];
    int app_type_cfg[ST_MAX_LENGTH_MIXER_CONTROL] = {0};
    int len = 0;
    struct mixer_ctl *ctl;
    int status = 0, acdb_id;
    struct listnode *p_node, *temp_node;
    struct adm_cfg_info *cfg_info;
    bool found_profile = false;
    int st_device_be_idx = -EINVAL;

    if (profile_type == ST_PROFILE_TYPE_NONE) {
        ALOGV("%s: Profile set to None, ignore sending app type cfg",__func__);
        goto exit;
    }

    if (st_device == ST_DEVICE_NONE) {
        ALOGE("%s: Invalid device sent",__func__);
        status = -EINVAL;
        goto exit;
    }

    acdb_id = acdb_device_table[exec_mode][st_device];
    if (acdb_id < 0) {
        ALOGE("%s: Could not find acdb id for device(%d)",
              __func__, st_device);
        status = -EINVAL;
        goto exit;
    }

    st_device_be_idx = get_st_device_backend_index(my_data, st_device);
    if (st_device_be_idx < 0)
        ALOGE("%s: Couldn't get backend index for device %d ret = %d",
              __func__, st_device, st_device_be_idx);

    snprintf(mixer_ctl_name, sizeof(mixer_ctl_name),
             "Listen Stream %d App Type Cfg", pcm_id);
    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        status = -EINVAL;
        goto exit;
    }

    list_for_each_safe(p_node, temp_node, &stdev->adm_cfg_list) {
        cfg_info = node_to_item(p_node, struct adm_cfg_info, list_node);
        if (cfg_info->profile_type == profile_type) {
            found_profile = true;
            app_type_cfg[len++] = cfg_info->app_type;
            app_type_cfg[len++] = acdb_id;
            app_type_cfg[len++] = cfg_info->sample_rate;
            if (st_device_be_idx >= 0)
                app_type_cfg[len++] = st_device_be_idx;
            break;
        }
    }

    if (found_profile) {
        status = mixer_ctl_set_array(ctl, app_type_cfg, len);
        if (status)
            ALOGE("%s: ERROR. Mixer ctl set failed", __func__);
    }

exit:
    return status;
}

int platform_stdev_get_device_app_type
(
   void *platform,
   st_profile_type_t profile_type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    struct listnode *p_node, *temp_node;
    struct adm_cfg_info *cfg_info;;
    int app_type = 0;

    list_for_each_safe(p_node, temp_node, &stdev->adm_cfg_list) {
        cfg_info = node_to_item(p_node, struct adm_cfg_info, list_node);
        if (cfg_info->profile_type == profile_type) {
            app_type = cfg_info->app_type;
            break;
        }
    }

    ALOGV("%s: app type %d for profile %d", __func__, app_type, profile_type);
    return app_type;
}

void platform_stdev_send_ec_ref_cfg
(
   void *platform,
   st_profile_type_t profile_type,
   bool enable
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;

    if (is_ec_profile(profile_type)) {
        strlcpy(my_data->ec_ref_mixer_path, "echo-reference",
                sizeof(my_data->ec_ref_mixer_path));
        if (enable) {
            my_data->ec_ref_enabled = enable;
            audio_route_apply_and_update_path(stdev->audio_route,
                    my_data->ec_ref_mixer_path);
        } else {
            if (my_data->ec_ref_enabled) {
                audio_route_reset_and_update_path(stdev->audio_route,
                        my_data->ec_ref_mixer_path);
                my_data->ec_ref_enabled = enable;
            } else {
                ALOGV("%s: EC Reference is already disabled", __func__);
            }
        }
    } else {
        ALOGV("%s: Non-EC Profile, ignore sending ec ref cfg",__func__);
    }
}

int platform_stdev_send_custom_channel_mixing_coefficients
(
   void *platform,
   struct st_vendor_info *v_info,
   int pcm_id,
   char *str
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    char mixer_ctl_name[ST_MAX_LENGTH_MIXER_CONTROL];
    int cust_ch_mixer_cfg[ST_MAX_LENGTH_MIXER_CONTROL], len = 0;
    struct mixer_ctl *ctl;
    char *test_r = NULL;
    int err = -EINVAL;
    int ip_channel_cnt, op_channel_cnt, i;
    char *params;
    int select_channel;
    int ch_weightage_coeff[ST_MAX_NUM_CHANNELS][ST_MAX_NUM_CHANNELS];

    if (!v_info)
        return 0;

    if (str == NULL) {
        ALOGE("%s: Invalid string passed", __func__);
        goto exit;
    }

    ip_channel_cnt = my_data->codec_backend_cfg.channel_count;
    op_channel_cnt = v_info->channel_count;
    ALOGD("%s: i/p channel count %d, o/p channel count %d", __func__,
           ip_channel_cnt, op_channel_cnt);
    if (ip_channel_cnt > ST_MAX_NUM_CHANNELS ||
        op_channel_cnt > ST_MAX_NUM_CHANNELS) {
        ALOGE("%s: ERROR: Channel count out of bounds", __func__);
        goto exit;
    }

    snprintf(mixer_ctl_name, sizeof(mixer_ctl_name),
             "Listen Stream %d Channel Mix Cfg", pcm_id);
    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        goto exit;
    }

    /* Input channel count corresponds to backend configuration channels.
     * Output channel count corresponds to LSM session channels.
     * Set params is called with channels that need to be selected from
     * input to generate output.
     * ex: "1,4" to downmix from 4 to 2 i.e. to downmix from 4 to 2,
     * select channels 1 and 4 from input to generate output.
     *
     * This mixer control takes values in the following sequence:
     * - input channel count(m)
     * - output channel count(n)
     * - weight coeff for [out ch#1, in ch#1]
     * ....
     * - weight coeff for [out ch#1, in ch#m]
     *
     * - weight coeff for [out ch#2, in ch#1]
     * ....
     * - weight coeff for [out ch#2, in ch#m]
     *
     * - weight coeff for [out ch#n, in ch#1]
     * ....
     * - weight coeff for [out ch#n, in ch#m]
     *
     * weightage coeff is set to Unity for channels that are passed as part
     * of set params string.
     * In case of upmixing, coefficients is set to 0 for all output channels
     * which are greater than input channel count.
     * This is in sync with ADM Matrix mixer.
     */
    cust_ch_mixer_cfg[len++] = ip_channel_cnt;
    cust_ch_mixer_cfg[len++] = op_channel_cnt;
    for (i = 0; i < op_channel_cnt; i++) {
        memset(ch_weightage_coeff[i], 0, sizeof(*ch_weightage_coeff[i]));

        if (i >= ip_channel_cnt) {
            ALOGD("%s: Upmixing case. Ignore select channel", __func__);
        } else {
            if (i == 0)
                params = strtok_r(str, ",", &test_r);
            else
                params = strtok_r(NULL, ",", &test_r);

            if (params == NULL) {
                ALOGE("%s: incorrect channel mixing options\n", __func__);
                break;
            }

            select_channel = atoi(params);
            if ((select_channel > 0) && (select_channel <= ip_channel_cnt)) {
                ALOGD("%s: o/p [%d] select channel from i/p [%d]", __func__,
                      i+1, select_channel);
                ch_weightage_coeff[i][select_channel - 1] = 1;
            } else {
                ALOGE("%s: Invalid o/p channel count entered", __func__);
                break;
            }
        }

        if (len > ST_MAX_LENGTH_MIXER_CONTROL) {
            ALOGE("%s: ERROR: Mixer control length out of bounds", __func__);
            goto exit;
        }

        memcpy(&cust_ch_mixer_cfg[len], &ch_weightage_coeff[i][0],
               ip_channel_cnt);
        len += ip_channel_cnt;
    }

    if (i != op_channel_cnt)
        goto exit;

    err = mixer_ctl_set_array(ctl, cust_ch_mixer_cfg, len);
    if (err)
        ALOGE("%s: ERROR. Mixer ctl set failed", __func__);

exit:
    return err;
}

int platform_stdev_update_bad_mic_channel_index
(
   void *platform,
   int bad_mic_channel_index,
   bool *backend_cfg_change
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int ret = 0;

    *backend_cfg_change = false;

    if (!(bad_mic_channel_index & BAD_MIC_CH_SUPPORTED_INDICES)) {
         ALOGE("%s: Invalid bad mic channel index 0x%x",
               __func__, bad_mic_channel_index);
         ret = -EINVAL;
         return ret;
    }

    /* Currently bad mic index can be set once and
     * to only one of the supported bad indices.
     *
     * Determine if backend config change is required depending on current
     * channel configuration -
     * 4 channels - always reconfigure backend
     * 2 channels - reconfigure backend if index 0 or 1 goes bad
     * 1 channel - reconfigure backend if index 0 goes bad
     */

    if (my_data->bad_mic_channel_index) {
        ALOGE("%s: bad mic channel index already set to 0x%x",
               __func__, my_data->bad_mic_channel_index);
         ret = -EINVAL;
         return ret;
    }

    if (popcount(bad_mic_channel_index) > 1) {
        ALOGE("%s: bad mic channel index popcount more than one 0x%x",
               __func__, bad_mic_channel_index);
         ret = -EINVAL;
         return ret;
    }

    switch (my_data->codec_backend_cfg.channel_count) {
    case SOUND_TRIGGER_CHANNEL_MODE_QUAD:
    case SOUND_TRIGGER_CHANNEL_MODE_TRI:
        *backend_cfg_change = true;
        break;
    case SOUND_TRIGGER_CHANNEL_MODE_STEREO:
        if ((bad_mic_channel_index == BAD_MIC_CH_INDEX_0) ||
            (bad_mic_channel_index == BAD_MIC_CH_INDEX_1))
             *backend_cfg_change = true;
        break;
    case SOUND_TRIGGER_CHANNEL_MODE_MONO:
        if (bad_mic_channel_index == BAD_MIC_CH_INDEX_0)
            *backend_cfg_change = true;
        break;
    default:
        break;
    }

    my_data->bad_mic_channel_index = bad_mic_channel_index;
    ALOGV("%s: Bad mic channel index set to 0x%x, backend cfg change %d",
               __func__, my_data->bad_mic_channel_index, *backend_cfg_change);
    return ret;
}

bool platform_stdev_check_backends_match
(
   void *platform __unused,
   st_exec_mode_t exec_mode1,
   st_device_t st_device1,
   st_exec_mode_t exec_mode2,
   st_device_t st_device2
)
{
    bool match = false;

    if ((st_device1 < ST_DEVICE_MIN) || (st_device1 >= ST_DEVICE_MAX)) {
        ALOGE("%s: Invalid st_device1 = %d", __func__, st_device1);
        return match;
    }

    if ((st_device2 < ST_DEVICE_MIN) || (st_device2 >= ST_DEVICE_MAX)) {
        ALOGE("%s: Invalid st_device2 = %d", __func__, st_device2);
        return match;
    }

    char *be_itf1 = st_device_backend_table[exec_mode1][st_device1];
    char *be_itf2 = st_device_backend_table[exec_mode2][st_device2];

    if ((NULL != be_itf1) && (NULL != be_itf2)) {
        if (!strcmp(be_itf1, be_itf2))
            match = true;
    }
    ALOGV("%s: be_itf1 %s, be_itf2 %s match %d", __func__, be_itf1, be_itf2, match);

    return match;
}

void platform_stdev_check_and_append_usecase
(
   void *platform __unused,
   char *use_case,
   st_profile_type_t profile_type
)
{
    if (profile_type != ST_PROFILE_TYPE_NONE)
        strlcat(use_case, " preproc", USECASE_STRING_SIZE);

    ALOGV("%s: return usecase %s", __func__, use_case);
}

void platform_stdev_check_and_update_ec_ref_config
(
   void *platform __unused,
   struct st_vendor_info *v_info,
   struct pcm_config *config
)
{
    /* Update channels with ec ref channel count in vendor info */
    if (v_info)
        config->channels = v_info->ec_ref_channel_cnt;
}

bool platform_stdev_check_concurrent_capture
(
   void *platform,
   audio_stream_usecase_type_t usecase_type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    bool conc_allowed = false;

    /* Restrict it only for WDSP. ADSP support will be added later */
    if (stdev->is_gcs && stdev->conc_capture) {
        conc_allowed = true;
        if (((usecase_type == USECASE_TYPE_VOICE_CALL) &&
             !stdev->conc_voice_call) ||
            ((usecase_type == USECASE_TYPE_VOIP_CALL) &&
             !stdev->conc_voip_call))
            conc_allowed = false;
    }
    ALOGV("%s: capture concurency %s allowed, usecase type %d", __func__,
          conc_allowed ? "": "not", usecase_type);
    return conc_allowed;
}
