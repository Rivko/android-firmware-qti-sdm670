/* sound_trigger_platform.c
 *
 * Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define LOG_TAG "sound_trigger_platform"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0
#ifdef LINUX_ENABLED
/*temporay change since defination is not found for msmcal-hwdep.h*/
typedef unsigned int __u32;
typedef unsigned char __u8;
#endif
#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <dlfcn.h>
#include <expat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sound/msmcal-hwdep.h>
#include "sound_trigger_platform.h"

/* macros used to verify all param id tags defined for vendor uuid
   in platform file */
#define PARAM_LOAD_SOUND_MODEL_BIT (1 << 0)
#define PARAM_UNLOAD_SOUND_MODEL_BIT (1 << 1)
#define PARAM_CONFIDENCE_LEVELS_BIT (1 << 2)
#define PARAM_OPERATION_MODE_BIT (1 << 3)
#define PARAM_ID_MANDATORY_BITS \
    (PARAM_LOAD_SOUND_MODEL_BIT | PARAM_UNLOAD_SOUND_MODEL_BIT | \
     PARAM_OPERATION_MODE_BIT)

/* platform info keys */
#define ST_PARAM_KEY_EXECUTION_TYPE "execution_type"
#define ST_PARAM_KEY_MAX_CPE_SESSIONS "max_cpe_sessions"
#define ST_PARAM_KEY_MAX_APE_SESSIONS "max_ape_sessions"
#define ST_PARAM_KEY_ENABLE_FAILURE_DETECTION "enable_failure_detection"
#define ST_PARAM_KEY_CPE_FE_TO_BE_FIXED "cpe_fe_to_be_fixed"
#define ST_PARAM_KEY_RX_CONCURRENCY_DISABLED "rx_concurrency_disabled"
#define ST_PARAM_KEY_RX_MAX_CONC_SESSIONS "rx_conc_max_st_ses"

#define ST_PARAM_KEY_FIRMWARE_IMAGE "firmware_image"
#define ST_PARAM_KEY_SM_VENDOR_UUID "vendor_uuid"
#define ST_PARAM_KEY_APP_TYPE "app_type"
#define ST_PARAM_KEY_LIBRARY "library"
#define ST_PARAM_KEY_MAX_CPE_PHRASES "max_cpe_phrases"
#define ST_PARAM_KEY_MAX_APE_USERS "max_ape_users"
#define ST_PARAM_KEY_MAX_APE_PHRASES "max_ape_phrases"
#define ST_PARAM_KEY_MAX_CPE_USERS "max_cpe_users"
#define ST_PARAM_KEY_SAMPLE_RATE "sample_rate"
#define ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS "load_sound_model_ids"
#define ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS "unload_sound_model_ids"
#define ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS "confidence_levels_ids"
#define ST_PARAM_KEY_OPERATION_MODE_IDS "operation_mode_ids"
#define ST_PARAM_KEY_CAPTURE_KEYWORD "capture_keyword"
#define ST_PARAM_KEY_CLIENT_CAPTURE_READ_DELAY "client_capture_read_delay"

#define ST_PARAM_KEY_DEVICE_HANDSET_APE_ACDB_ID "DEVICE_HANDSET_APE_ACDB_ID"
#define ST_PARAM_KEY_DEVICE_HANDSET_CPE_ACDB_ID "DEVICE_HANDSET_CPE_ACDB_ID"
#define ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP_ACDB_ID "DEVICE_HANDSET_CPE_ECPP_ACDB_ID"

#define ST_MAX_SND_CARD_NAME_LENGTH 100

typedef int  (*acdb_loader_init_v2_t)(const char *, const char *, int);
typedef void (*acdb_loader_deallocate_t)(void);
typedef void (*acdb_loader_send_listen_device_cal_t)(int, int, int, int);
typedef int  (*acdb_loader_send_listen_lsm_cal_t)(int, int, int, int);
typedef int  (*acdb_loader_get_calibration_t)(char *, int, void *);

struct st_device_index
st_device_name_idx[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
   {
       {"DEVICE_HANDSET_APE_ACDB_ID", ST_DEVICE_HANDSET_MIC},
   },
   {
       {"DEVICE_HANDSET_CPE_ACDB_ID", ST_DEVICE_HANDSET_MIC},
       {"DEVICE_HANDSET_CPE_ECPP_ACDB_ID", ST_DEVICE_HANDSET_MIC_ECPP},
   },
};

static const char * const
st_device_table[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
        /* ADSP SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-ape-handset-mic",
    },
    {
        /* CPE SVA devices */
        [ST_DEVICE_NONE] = "none",
        [ST_DEVICE_HANDSET_MIC] = "listen-cpe-handset-mic",
        [ST_DEVICE_HANDSET_MIC_ECPP] = "listen-cpe-handset-mic-ecpp",
    },
};

/* ACDB IDs for each device for both CDSP and ADSP */
static int acdb_device_table[ST_DEVICE_EXEC_MODE_MAX][ST_DEVICE_MAX] = {
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_APE_ACDB_ID,
    },
    {
      [ST_DEVICE_NONE] = -1,
      [ST_DEVICE_HANDSET_MIC] = DEVICE_HANDSET_CPE_ACDB_ID,
      [ST_DEVICE_HANDSET_MIC_ECPP] = DEVICE_HANDSET_CPE_ACDB_ID,
    }
};

/* Qualcomm vendorUuid for SVA soundmodel */
static const sound_trigger_uuid_t qc_uuid =
    { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };

/* Platform xml tags */
typedef enum {
    TAG_ROOT,
    TAG_COMMON,
    TAG_ACDB,
    TAG_SOUND_MODEL
} st_xml_tags_t;

typedef void (*st_xml_process_fn)(void *platform, const XML_Char **attr);
static void platform_stdev_process_kv_params(void *platform, const XML_Char **attr);

/* function pointers for xml tag info parsing */
static st_xml_process_fn process_table[] = {
    [TAG_ROOT] = NULL,
    [TAG_COMMON] = platform_stdev_process_kv_params,
    [TAG_ACDB] = platform_stdev_process_kv_params,
    [TAG_SOUND_MODEL] = platform_stdev_process_kv_params
};

struct platform_data {
    int hwdep_fd;
    int prev_acdb_id;
    sound_trigger_device_t *stdev;
    void *acdb_handle;
    acdb_loader_send_listen_device_cal_t acdb_send_device_cal;
    acdb_loader_send_listen_lsm_cal_t acdb_send_lsm_cal;
    acdb_loader_get_calibration_t acdb_get_cal;
    acdb_loader_deallocate_t acdb_deinit;
    struct st_vendor_info *vendor_uuid_info;
    int param_tag_tracker;

    int xml_version;
    st_xml_tags_t st_xml_tag;
    struct str_parms *kvpairs;

    bool cpe_fe_to_be_fixed;
    char codec_version[15];
};

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

    for (i = 0; i < ST_DEVICE_EXEC_MODE_MAX; i++) {
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

static void platform_stdev_set_default_config(struct platform_data *platform)
{
    sound_trigger_device_t *stdev = platform->stdev;

    stdev->run_on_ape =  true;
    stdev->max_ape_sessions = 1;
    stdev->avail_ape_phrases = 1;
    stdev->avail_ape_users = 1;
    stdev->max_cpe_sessions = 1;
    stdev->avail_cpe_phrases  = 1;
    stdev->avail_cpe_users = 1;
    stdev->rx_conc_max_st_ses = UINT_MAX;
    platform->cpe_fe_to_be_fixed = true;
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
        if(!strcmp(value, "CPE"))
            stdev->run_on_ape =  false;
        else if(!strcmp(value, "CPE_AND_APE"))
            stdev->transition_enabled = true;
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
           (0 == strncasecmp(value, "true", 4))? true:false;
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
           (0 == strncasecmp(value, "true", 4))? true:false;
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
        if(!vendor_uuid_info) {
            ALOGE("%s: can't allocate vendor_uuid_info", __func__);
            goto cleanup;
        }
        if(string_to_uuid(attr[1], &vendor_uuid_info->uuid) < 0) {
            ALOGE("%s: string_to_uuid failed", __func__);
            goto cleanup;
        }
        if(attr[2] && !strcmp(attr[2], "app_type"))
           vendor_uuid_info->app_type = strtoul(attr[3], NULL, 16);

        /* if soundmodel library for ISV vendor uuid is mentioned, use it. If not
           ignore and continue sending the opaque data from HAL to DSP */
        if(attr[4] && !strcmp(attr[4], "library") && strcmp(attr[5], "none")) {
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
        vendor_uuid_info->sample_rate = SOUND_TRIGGER_SAMPLING_RATE;
        platform->vendor_uuid_info = vendor_uuid_info;
        return;
    }

    if(!platform->vendor_uuid_info)
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
            vendor_uuid_info->params[LOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[LOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_LOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "unload_sound_model")) {
            vendor_uuid_info->params[UNLOAD_SOUND_MODEL].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[UNLOAD_SOUND_MODEL].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_UNLOAD_SOUND_MODEL_BIT;
        } else if (!strcmp(attr[1], "confidence_levels")) {
            vendor_uuid_info->params[CONFIDENCE_LEVELS].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[CONFIDENCE_LEVELS].param_id =
                                                    strtoul(attr[5], NULL, 16);
            platform->param_tag_tracker |= PARAM_CONFIDENCE_LEVELS_BIT;
        } else if (!strcmp(attr[1], "operation_mode")) {
            vendor_uuid_info->params[OPERATION_MODE].module_id =
                                                    strtoul(attr[3], NULL, 16);
            vendor_uuid_info->params[OPERATION_MODE].param_id =
                                                    strtoul(attr[5], NULL, 16);
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
    if(vendor_uuid_info) {
        if(vendor_uuid_info->smlib_handle)
            dlclose(vendor_uuid_info->smlib_handle);
        free(vendor_uuid_info);
        platform->vendor_uuid_info = NULL;
    }
}

static int platform_stdev_set_module_param_ids
(
   struct st_module_param_info *mp_info,
   char *value
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
        ALOGE("%s: incorrect param id", __func__);
        ret = -EINVAL;
        goto error_exit;
    }
    mp_info->param_id = strtoul(id, NULL, 16);
    return 0;

error_exit:
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
    }
    else if (!strcmp(id, "ADPCM_raw")) {
        sm_info->kw_capture_format |= ADPCM_RAW;
    }
    else if (!strcmp(id, "PCM_packet")) {
        sm_info->kw_capture_format |= PCM_CUSTOM_PACKET;
    }
    else if (!strcmp(id, "PCM_raw")) {
        sm_info->kw_capture_format |= PCM_RAW;
    }
    else {
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

static int platform_stdev_load_sm_lib(struct st_vendor_info *sm_info, const char *name)
{
    int ret = 0;

    sm_info->smlib_handle = dlopen(name, RTLD_NOW);
    if (!sm_info->smlib_handle) {
        ALOGE("%s: dlopen failed for %s", __func__, name);
        ret = -EINVAL;
        goto error_exit;
    }
    sm_info->generate_st_phrase_recognition_event =
        (smlib_generate_sound_trigger_phrase_recognition_event_t)
        dlsym(sm_info->smlib_handle,
              "generate_sound_trigger_phrase_recognition_event");

    if (!sm_info->generate_st_phrase_recognition_event) {
        ALOGE("%s: dlsym error %s for generate_sound_trigger_phrase_recognition_event",
              __func__, dlerror());
        ret = -EINVAL;
        goto error_exit;
    }
    sm_info->generate_st_recognition_config_payload =
        (smlib_generate_sound_trigger_recognition_config_payload_t)
        dlsym(sm_info->smlib_handle,
              "generate_sound_trigger_recognition_config_payload");

    if (!sm_info->generate_st_recognition_config_payload) {
        ALOGE("%s: dlsym error %s for generate_sound_trigger_recognition_config_payload",
              __func__, dlerror());
        ret = -EINVAL;
        goto error_exit;
    }
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

    err = str_parms_get_str(parms, ST_PARAM_KEY_EXECUTION_TYPE,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_EXECUTION_TYPE);
        if (!strcmp(str_value, "CPE"))
            stdev->run_on_ape =  false;
    }

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

    err = str_parms_get_str(parms, ST_PARAM_KEY_ENABLE_FAILURE_DETECTION,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_ENABLE_FAILURE_DETECTION);
        stdev->detect_failure = (!strncasecmp(str_value, "true", 4))? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CPE_FE_TO_BE_FIXED,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CPE_FE_TO_BE_FIXED);
        my_data->cpe_fe_to_be_fixed = (!strncasecmp(str_value, "true", 4))? true : false;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_RX_CONCURRENCY_DISABLED,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_RX_CONCURRENCY_DISABLED);
        stdev->rx_concurrency_disabled = (!strncasecmp(str_value, "true", 4))? true : false;
    }

    err = str_parms_get_int(parms, ST_PARAM_KEY_RX_MAX_CONC_SESSIONS, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_RX_MAX_CONC_SESSIONS);
        stdev->rx_conc_max_st_ses = value;
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

    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ACDB_ID, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ACDB_ID);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ACDB_ID, value);
        if (ret)
            return ret;
    }
    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP_ACDB_ID, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP_ACDB_ID);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_CPE_ECPP_ACDB_ID, value);
    }


    err = str_parms_get_int(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE_ACDB_ID, &value);
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_DEVICE_HANDSET_APE_ACDB_ID);
        ret = platform_stdev_set_acdb_id(platform, ST_PARAM_KEY_DEVICE_HANDSET_APE_ACDB_ID, value);
    }
    return ret;
}

static int platform_stdev_set_sm_config_params
(
   void *platform,
   struct str_parms *parms
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    struct st_vendor_info *sm_info;
    char str_value[128];
    char *kv_pairs = str_parms_to_str(parms);
    int ret = 0, err, value;

    ALOGV("%s: enter: %s", __func__, kv_pairs);
    if (kv_pairs == NULL) {
        ALOGE("%s: key-value pair is NULL", __func__);
        return -EINVAL;
    }

    /* Allocate the vendor sound model config.
       Set the platform configured params.
       Push this sound model config to list.
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
    sm_info->sample_rate = SOUND_TRIGGER_SAMPLING_RATE;

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
            platform_stdev_load_sm_lib(sm_info, str_value);
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

    err = str_parms_get_str(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_LOAD_SOUND_MODEL_IDS);
        platform_stdev_set_module_param_ids(&sm_info->params[LOAD_SOUND_MODEL], str_value);
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_UNLOAD_SOUND_MODEL_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[UNLOAD_SOUND_MODEL], str_value);
        if (ret)
            goto error_exit;
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_CONFIDENCE_LEVELS_IDS);
        platform_stdev_set_module_param_ids(&sm_info->params[CONFIDENCE_LEVELS], str_value);
    }

    err = str_parms_get_str(parms, ST_PARAM_KEY_OPERATION_MODE_IDS,
                            str_value, sizeof(str_value));
    if (err >= 0) {
        str_parms_del(parms, ST_PARAM_KEY_OPERATION_MODE_IDS);
        ret = platform_stdev_set_module_param_ids(&sm_info->params[OPERATION_MODE], str_value);
        if (ret)
            goto error_exit;
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
        if (!my_data->stdev->run_on_ape)
            strlcpy(sm_info->cpe_firmware_image,
                    str_value, sizeof(sm_info->cpe_firmware_image));
        if (ret)
            goto error_exit;
    }

    list_add_tail(&my_data->stdev->vendor_uuid_list,
                  &sm_info->list_node);
    return 0;

error_exit:
    if (sm_info)
        free(sm_info);
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
    }else if (!strcmp(tag_name, "acdb_ids")) {
        my_data->st_xml_tag = TAG_ACDB;
    } else if (!strcmp(tag_name, "sound_model_config")) {
        my_data->st_xml_tag = TAG_SOUND_MODEL;
    } else if (!strcmp(tag_name, "param")) {
        if ((my_data->st_xml_tag != TAG_ROOT) && (my_data->st_xml_tag != TAG_COMMON) &&
            (my_data->st_xml_tag != TAG_ACDB) && (my_data->st_xml_tag != TAG_SOUND_MODEL)) {
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
        } else if (!strcmp(tag_name, "sound_model_config")) {
            platform->st_xml_tag = TAG_ROOT;
            platform_stdev_set_sm_config_params(platform, platform->kvpairs);
        }
        return;
    }

    if (!strcmp(tag_name, "vendor_uuid")) {
        if (platform->vendor_uuid_info) {
            if (platform->param_tag_tracker & PARAM_ID_MANDATORY_BITS) {
                list_add_tail(&platform->stdev->vendor_uuid_list,
                              &platform->vendor_uuid_info->list_node);
            } else {
                if(platform->vendor_uuid_info->smlib_handle)
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
    } else if (strstr(snd_card_name, "msm8976-tasha")||
                 strstr(snd_card_name, "msm8952-tasha") ||
                 strstr(snd_card_name, "msm8953-tasha") ||
                 strstr(snd_card_name, "msm8953-sku3-tasha") ||
                 strstr(snd_card_name, "msm8937-tasha")) {
        strlcpy(mixer_path_xml, MIXER_PATH_XML_WCD9335,
                        sizeof(MIXER_PATH_XML_WCD9335));
    } else {
        strlcpy(mixer_path_xml, MIXER_PATH_XML,
                         sizeof(MIXER_PATH_XML));
    }

    if ((strstr(snd_card_name, "msm8939") ||
        strstr(snd_card_name, "msm8909") ||
        strstr(snd_card_name, "msm8952") ||
        strstr(snd_card_name, "msm8976") ||
        strstr(snd_card_name, "msm8953") ||
        strstr(snd_card_name, "msm8937") ||
        strstr(snd_card_name, "msm8917") ||
        strstr(snd_card_name, "msm8x16")) &&
        !strstr(snd_card_name, "msm8976-tasha") &&
        !strstr(snd_card_name, "msm8952-tasha") &&
        !strstr(snd_card_name, "msm8953-tasha") &&
        !strstr(snd_card_name, "msm8953-sku3-tasha") &&
        !strstr(snd_card_name, "msm8937-tasha") &&
        !strstr(snd_card_name, "msm8952-tomtom") &&
        !strstr(snd_card_name, "msm8939-tomtom")) {
        stdev->sw_mad = true;
    }
}

void *platform_stdev_init(sound_trigger_device_t *stdev)
{
    int ret = 0, retry_num = 0;
    struct platform_data *my_data = NULL;
    const char *snd_card_name = NULL;
    const char *acdb_card_name = NULL;
    int snd_card_num = SOUND_CARD;

    acdb_loader_init_v2_t acdb_init = NULL;
    audio_hw_acdb_init_t audio_hw_acdb_init = NULL;
    audio_hw_get_snd_card_num_t audio_hw_get_snd_card_num = NULL;

    char mixer_path_xml[100];
    struct listnode *v_node, *temp_node;
    struct st_vendor_info* v_info;
    char dev_name[256];

    ALOGI("%s: Enter", __func__);
    my_data = calloc(1, sizeof(struct platform_data));

    if (!my_data || !stdev) {
        ALOGE("%s: ERROR. NULL param", __func__);
        if(my_data)
            free(my_data);
        return NULL;
    }
    my_data->stdev = stdev;
    list_init(&stdev->vendor_uuid_list);

    audio_hw_get_snd_card_num = (audio_hw_get_snd_card_num_t)dlsym(stdev->audio_hal_handle,
                                              "audio_extn_utils_get_snd_card_num");
    if (audio_hw_get_snd_card_num) {
        snd_card_num = audio_hw_get_snd_card_num();
        if (snd_card_num < 0) {
            ALOGE("%s: ERROR. Unable to get sound card num", __func__);
            goto cleanup;
        }
    } else {
        ALOGE("%s: ERROR. dlsym error %s for audio_extn_utils_get_snd_card_num", __func__, dlerror());
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
    stdev->audio_route = audio_route_init(SOUND_CARD, mixer_path_xml);
    if (!stdev->audio_route) {
        ALOGE("%s: ERROR. Failed to init audio route controls, aborting.",
                __func__);
        goto cleanup;
    }

    my_data->acdb_handle = dlopen(LIB_ACDB_LOADER, RTLD_NOW);
    if (my_data->acdb_handle == NULL) {
        ALOGE("%s: ERROR. dlopen failed for %s", __func__, LIB_ACDB_LOADER);
        goto cleanup;
    }

    audio_hw_acdb_init = (audio_hw_acdb_init_t)dlsym(stdev->audio_hal_handle,
                                              "acdb_init");
    if (audio_hw_acdb_init == NULL) {
        ALOGE("%s: ERROR. dlsym error %s for acdb_init", __func__, dlerror());

        /* Fallback if 'audio_hw_acdb_init' is unavailable */
        acdb_init = (acdb_loader_init_v2_t)dlsym(my_data->acdb_handle,
                                                  "acdb_loader_init_v2");
        if (acdb_init == NULL) {
            ALOGE("%s: dlsym error %s for acdb_loader_init_v2", __func__, dlerror());
            goto cleanup;
        }
    }

    my_data->acdb_deinit = (acdb_loader_deallocate_t)dlsym(my_data->acdb_handle,
                                           "acdb_loader_deallocate_ACDB");
    if (my_data->acdb_deinit == NULL) {
        ALOGE("%s: dlsym error %s for acdb_loader_deallocate_ACDB", __func__, dlerror());
        goto cleanup;
    }

    my_data->acdb_send_device_cal = (acdb_loader_send_listen_device_cal_t)
              dlsym(my_data->acdb_handle, "acdb_loader_send_listen_device_cal");

    if (my_data->acdb_send_device_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_send_listen_device_cal", __func__,
               dlerror());
        goto cleanup;
    }

    my_data->acdb_send_lsm_cal = (acdb_loader_send_listen_lsm_cal_t)
              dlsym(my_data->acdb_handle, "acdb_loader_send_listen_lsm_cal");

    if (my_data->acdb_send_lsm_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_send_listen_lsm_cal", __func__,
               dlerror());
        goto cleanup;
    }

    my_data->acdb_get_cal = (acdb_loader_get_calibration_t)
              dlsym(my_data->acdb_handle, "acdb_loader_get_calibration");
    if (my_data->acdb_get_cal == NULL) {
        ALOGE("%s: ERROR. dlsym Error:%s acdb_loader_get_calibration", __func__,
               dlerror());
        goto cleanup;
    }

    platform_stdev_set_default_config(my_data);

    platform_parse_info(my_data, PLATFORM_PATH_XML);

    /* Using non topology solution still need QC smlib wrapper APIs */
    stdev->smlib_handle = dlopen(LIB_SM_WRAPPER, RTLD_NOW);
    if (stdev->smlib_handle) {
        stdev->generate_st_phrase_recognition_event =
            (smlib_generate_sound_trigger_phrase_recognition_event_t)dlsym(stdev->smlib_handle,
                                                  "generate_sound_trigger_phrase_recognition_event");
        if (!stdev->generate_st_phrase_recognition_event) {
            ALOGE("%s: dlsym error %s for generate_sound_trigger_phrase_recognition_event", __func__,
                  dlerror());
            goto cleanup;
        }

        stdev->generate_st_recognition_config_payload =
            (smlib_generate_sound_trigger_recognition_config_payload_t)dlsym(stdev->smlib_handle,
                                                  "generate_sound_trigger_recognition_config_payload");
        if (!stdev->generate_st_recognition_config_payload) {
            ALOGE("%s: dlsym error %s for generate_sound_trigger_recognition_config_payload",
                  __func__, dlerror());
            goto cleanup;
        }
    } else {
        ALOGW("%s: dlopen failed for %s", __func__, LIB_SM_WRAPPER);
    }

    /* Check if ISV vendor_uuid is present and force disable transitions */
    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if(!memcmp(&v_info->uuid, &qc_uuid, sizeof(sound_trigger_uuid_t))) {
            v_info->is_qc_uuid = true;
        } else {
            ALOGV("%s: ISV uuid present, force disable transitions",
              __func__);
            stdev->transition_enabled = false;
        }
        if (!stdev->adpcm_dec_lib_handle &&
             (v_info->kw_capture_format & ADPCM)) {
            /* Load ADPCM decoder library */
            stdev->adpcm_dec_lib_handle = dlopen(LIB_ADPCM_DECODER, RTLD_NOW);
            if (!stdev->adpcm_dec_lib_handle) {
                ALOGE("%s: ERROR. dlopen failed for %s", __func__, LIB_ADPCM_DECODER);
                goto cleanup;
            }
            stdev->adpcm_dec_init =
                (g722_init_decoder_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_init_decoder");
            if (!stdev->adpcm_dec_init) {
                ALOGE("%s: dlsym error %s for g722_init_decoder", __func__,
                      dlerror());
                goto cleanup;
            }

            stdev->adpcm_dec_get_scratch_size =
                (g722_dec_get_total_byte_size_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_dec_get_total_byte_size");
            if (!stdev->adpcm_dec_get_scratch_size) {
                ALOGE("%s: dlsym error %s for g722_dec_get_total_byte_size", __func__,
                      dlerror());
                goto cleanup;
            }

            stdev->adpcm_dec_process =
                (g722_dec_process_t)dlsym(stdev->adpcm_dec_lib_handle,
                                                      "g722_dec_process");
            if (!stdev->adpcm_dec_process) {
                ALOGE("%s: dlsym error %s for g722_dec_process", __func__,
                      dlerror());
                goto cleanup;
            }
        }
        ALOGV("%s: vendor config: kcf=%d, ktm=%d, ckw=%d, cu=%d, akw=%d, au=%d",
              __func__, v_info->kw_capture_format, v_info->kw_transfer_mode,
              v_info->avail_cpe_phrases, v_info->avail_cpe_users,
              v_info->avail_ape_phrases, v_info->avail_ape_users);
    }

    if (audio_hw_acdb_init) {
        ALOGD("call audio_hw_acdb_init ");
        ret = audio_hw_acdb_init(snd_card_num);
        if (ret) {
             ALOGE("%s: ERROR. audio_hw_acdb_init failed status %d", __func__, ret);
             goto cleanup;
        }
    } else {
        acdb_card_name = get_snd_card_name_for_acdb_loader(snd_card_name);
        ALOGI("%s: acdb_init: %s", __func__, acdb_card_name);
        ret = acdb_init(acdb_card_name, NULL, 0);
        if (ret) {
            ALOGE("%s: ERROR. acdb_loader_init_v2 failed status %d", __func__, ret);
            goto cleanup;
        }
        free((void*)acdb_card_name);
        acdb_card_name = NULL;
    }

    snprintf(dev_name, sizeof(dev_name), "/dev/snd/hwC%uD%u",
                               SOUND_CARD, WCD9XXX_CODEC_HWDEP_NODE);
    ALOGD("%s Opening device %s", __func__, dev_name);
    my_data->hwdep_fd = open(dev_name, O_WRONLY);
    if (my_data->hwdep_fd < 0) {
        ALOGE("%s: cannot open device '%s'", __func__, dev_name);
        my_data->acdb_deinit();
        goto cleanup;
    }
    my_data->prev_acdb_id = -1;

    ret = get_codec_version(my_data, snd_card_name, SOUND_CARD);
    if (ret)
        goto cleanup;

    return my_data;

cleanup:
    if(stdev->adpcm_dec_lib_handle)
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
    struct listnode *v_node, *temp_node;
    struct st_vendor_info* v_info;

    ALOGI("%s: Enter", __func__);
    if (my_data) {
        list_for_each_safe(v_node, temp_node, &my_data->stdev->vendor_uuid_list) {
            v_info = node_to_item(v_node, struct st_vendor_info, list_node);
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
        audio_route_free(my_data->stdev->audio_route);
        mixer_close(my_data->stdev->mixer);
        if (my_data->hwdep_fd >= 0) {
            close(my_data->hwdep_fd);
            my_data->hwdep_fd = -1;
        }
        if (my_data->kvpairs)
            str_parms_destroy(my_data->kvpairs);
        free(my_data);
    }
}

struct st_vendor_info* platform_stdev_get_vendor_info
(
   void *platform,
   sound_trigger_uuid_t *vendor_uuid,
   bool *is_qc_uuid
)
{
    struct listnode *v_node;
    struct st_vendor_info* v_info = NULL;
    struct platform_data *my_data;
    sound_trigger_device_t *stdev;

    if (!platform || !is_qc_uuid || !vendor_uuid) {
        ALOGE("%s: NULL inputs", __func__);
        return NULL;
    }
    my_data = (struct platform_data *)platform;
    if (!my_data->stdev) {
        ALOGE("%s: platform stdev data is NULL", __func__);
        return NULL;
    }
    stdev = my_data->stdev;
    *is_qc_uuid = false;

    list_for_each(v_node, &stdev->vendor_uuid_list) {
        v_info = node_to_item(v_node, struct st_vendor_info, list_node);
        if(!memcmp(&v_info->uuid, vendor_uuid, sizeof(sound_trigger_uuid_t))) {
            ALOGV("%s: Matched uuid", __func__);
            break;
        }
    }

    if (!memcmp(&qc_uuid, vendor_uuid, sizeof(sound_trigger_uuid_t)))
        *is_qc_uuid = true;

    return v_info;
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
        (exec_mode > ST_DEVICE_EXEC_MODE_NONE && exec_mode < ST_DEVICE_EXEC_MODE_MAX)) {
        strlcpy(device_name, st_device_table[exec_mode][st_device], DEVICE_NAME_MAX_SIZE);

        if ((strstr(my_data->codec_version, "WCD9335_1_0") ||
            strstr(my_data->codec_version, "WCD9335_1_1")) &&
            !my_data->stdev->run_on_ape) {
            strlcat(device_name, " low-speed-intf", DEVICE_NAME_MAX_SIZE);
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
   sound_trigger_session_t *p_ses,
   audio_devices_t device
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    struct st_vendor_info *sm_info = p_ses->vendor_uuid_info;
    st_device_t st_device = ST_DEVICE_NONE;

    /* DEVICE_NONE request is a valid case when sending the session
       calibration prior to knowing the device. Hence return default
       device as handset mic */
    if (device == AUDIO_DEVICE_NONE)
        ALOGV("%s: device none",__func__);

    if (!stdev->run_on_ape && sm_info &&
        (sm_info->sample_rate == SOUND_TRIGGER_SAMPLING_RATE_48000)) {
        st_device = ST_DEVICE_HANDSET_MIC_ECPP;
    } else {
        st_device = ST_DEVICE_HANDSET_MIC;
    }
    ALOGV("%s: sample_rate=%d st_device %d", __func__,
          sm_info ? sm_info->sample_rate : 0,
          st_device);
    return st_device;
}

static int platform_stdev_send_hwmad_cal
(
    void *platform,
    int acdb_id
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int ret = 0;
    struct wcdcal_ioctl_buffer codec_buffer;
    struct hwdep_cal_param_data calib;
    sound_trigger_device_t *stdev = my_data->stdev;
    int hwdep_fd = my_data->hwdep_fd;

    if ((stdev->sw_mad) || (my_data->prev_acdb_id == acdb_id)) {
        ALOGD("%s: previous acdb_id %d new acdb_id %d, sw_mad %d return",
              __func__, my_data->prev_acdb_id, acdb_id, stdev->sw_mad);
        return 0;
    }

    calib.acdb_id = acdb_id;
    calib.get_size = 1;
    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
        ALOGE("%s: get_calibration to get size failed", __func__);
        return ret;
    }

    calib.get_size = 0;
    calib.buff = malloc(calib.buff_size);
    if (!calib.buff) {
        ALOGE("%s: malloc calib of buff size %d failed",
                  __func__, calib.buff_size);
        return -ENOMEM;
    }

    ret = my_data->acdb_get_cal("mad_cal", sizeof(struct hwdep_cal_param_data),
                                                            &calib);
    if (ret < 0) {
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
    return ret;
}

static int platform_stdev_send_device_cal
(
    void *platform,
    st_device_t st_device,
    int acdb_id
)
{
    int status = 0;

    /* send hwmad cal for HWMAD backend only. */
    if (st_device != ST_DEVICE_HANDSET_MIC_ECPP)
        status = platform_stdev_send_hwmad_cal(platform, acdb_id);

    return status;
}

int platform_stdev_send_calibration
(
    void *platform,
    audio_devices_t device,
    sound_trigger_session_t *p_ses,
    int app_id,
    bool use_topology,
    st_cal_type type
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    int acdb_id;
    st_device_t st_device;
    int status = 0;
    int hw_type;

    st_device = platform_stdev_get_device(platform, p_ses, device);
    if (st_device == ST_DEVICE_NONE) {
        ALOGE("%s: Could not find valid device",__func__);
        return -EINVAL;
    }

    acdb_id = acdb_device_table[p_ses->exec_mode][st_device];
    if (acdb_id < 0) {
        ALOGE("%s: Could not find acdb id for device(%d)",
              __func__, st_device);
        return -EINVAL;
    }
    hw_type = (p_ses->exec_mode == ST_DEVICE_EXEC_MODE_CPE) ? 0 : 1;

    switch (type) {
    case ST_SESSION_CAL:
        if (my_data->acdb_send_lsm_cal) {
            ALOGD("%s: sending lsm calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            /* ACDB modes: topology=1, non-topology=0 */
            status = my_data->acdb_send_lsm_cal(acdb_id, app_id,
                                                (use_topology == true),
                                                hw_type);
        }
        break;

    case ST_DEVICE_CAL:
        if (platform_stdev_send_device_cal(platform, st_device, acdb_id))
            return -EINVAL;

        if (my_data->acdb_send_device_cal) {
            ALOGD("%s: sending afe calibration for device(%d) acdb_id(%d)",
                                             __func__, st_device, acdb_id);
            my_data->acdb_send_device_cal(acdb_id, hw_type, app_id, SOUND_TRIGGER_SAMPLING_RATE_48000);
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
        stdev->tx_concurrency_active++;
        break;
    case AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE:
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
    unsigned int num_sessions,
    bool sound_model_loaded
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
        /* hw_mad case only applicable only when sound_model is loaded */
        if (sound_model_loaded) {
            if (stdev->tx_concurrency_active > 0)
                session_allowed = false;
            stdev->audio_concurrency_active = session_allowed ? false: true;
        }
    } else {
        /* sw_mad case applicable only before sound_model is loaded */
        /* num_sessions does not reflect current session yet */
        if (!sound_model_loaded) {
            if ((stdev->tx_concurrency_active > 0) ||
                (stdev->rx_concurrency_disabled &&
                stdev->rx_concurrency_active > 0 &&
                (num_sessions + 1) > stdev->rx_conc_max_st_ses)) {
                session_allowed = false;
            } else {
                stdev->audio_concurrency_active = false;
            }
        }
    }

    ALOGD("%s: stdev->audio_concurrency_active %d session_allowed %d", __func__,
          stdev->audio_concurrency_active, session_allowed);
    return session_allowed;
}

void platform_stdev_check_and_update_pcm_config
(
   void *platform,
   struct pcm_config *config
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    const char *snd_card_name;

    snd_card_name = mixer_get_name(my_data->stdev->mixer);
    if (strstr(snd_card_name, "tomtom") ||
        strstr(my_data->codec_version, "WCD9335_1_0") ||
        strstr(my_data->codec_version, "WCD9335_1_1")) {
        /* tomtom, Tasha1.0, Tasha1.1 support max 16KHz/24bit bandwidth */
        config->rate = SOUND_TRIGGER_SAMPLING_RATE;
        if (config->format != PCM_FORMAT_S16_LE)
            config->format = PCM_FORMAT_S24_LE;
    } else {
        /* Other codecs configured at ftrt 384Khz/32bit */
        config->rate = SOUND_TRIGGER_SAMPLING_RATE_384000;
        config->format = PCM_FORMAT_S32_LE;
    }
}

int platform_stdev_connect_mad
(
   void *platform,
   st_exec_mode_t mode
)
{
    /* This mixer control is only valid for CPE supported codec */
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    int status = 0;
    struct mixer_ctl *ctl = NULL;
    const char *mixer_ctl_name = "MAD_SEL MUX";

    if (stdev->sw_mad || (mode == ST_DEVICE_EXEC_MODE_NONE))
        return 0;

    ctl = mixer_get_ctl_by_name(stdev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: ERROR. Could not get ctl for mixer cmd - %s",
        __func__, mixer_ctl_name);
        return -EINVAL;
    }
    if (mode == ST_DEVICE_EXEC_MODE_APE)
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
    unsigned int i = 0, j = 0;
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
        }
    }
    free(line);
    fclose(fp);

    if (!CHECK_BIT(stdev->hw_type, ST_DEVICE_HW_APE|ST_DEVICE_HW_CPE)) {
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
              __func__, stdev->max_ape_sessions, j);
        stdev->max_cpe_sessions = j;
    }
    return 0;
}

int platform_cpe_get_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses,
   int sample_rate
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
                p_ses->use_case_idx = i;
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
   sound_trigger_session_t *p_ses
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
            p_ses->use_case_idx = i;
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
   sound_trigger_session_t *p_ses
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;

    for (i = 0; i < stdev->max_ape_sessions; i++) {
        if(stdev->ape_pcm_use_cases[i].pcm_id == p_ses->pcm_id) {
            stdev->ape_pcm_use_cases[i].active = false;
            break;
        }
    }
}

void platform_cpe_free_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses
)
{
    struct platform_data *my_data = (struct platform_data *)platform;
    sound_trigger_device_t *stdev = my_data->stdev;
    unsigned int i;

    for (i = 0; i < stdev->max_cpe_sessions; i++) {
        if(stdev->cpe_pcm_use_cases[i].pcm_id == p_ses->pcm_id) {
            stdev->cpe_pcm_use_cases[i].active = false;
            break;
        }
    }
}

