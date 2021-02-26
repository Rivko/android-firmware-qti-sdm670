/* sound_trigger_platform.h
 *
 * Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SOUND_TRIGGER_PLATFORM_H
#define SOUND_TRIGGER_PLATFORM_H

#include <cutils/list.h>
#include <hardware/sound_trigger.h>
#include "st_common_defs.h"
#include "sound_trigger_prop_intf.h"
#include "st_hw_defs.h"

struct sound_trigger_device;

#define PCM_DEVICE_FILE "/proc/asound/pcm"

#define MIXER_PATH_XML_WCD9306 "/vendor/etc/sound_trigger_mixer_paths_wcd9306.xml"
#define MIXER_PATH_XML_WCD9330 "/vendor/etc/sound_trigger_mixer_paths_wcd9330.xml"

#ifdef LINUX_ENABLED
#define MIXER_PATH_XML "/etc/sound_trigger_mixer_paths.xml"
#define MIXER_PATH_XML_WCD9335 "/etc/sound_trigger_mixer_paths_wcd9335.xml"
#define MIXER_PATH_XML_SKUW "/etc/sound_trigger_mixer_paths_skuw.xml"
#define PLATFORM_PATH_XML "/etc/sound_trigger_platform_info.xml"
#else
#define MIXER_PATH_XML "/vendor/etc/sound_trigger_mixer_paths.xml"
#define MIXER_PATH_XML_WCD9335 "/vendor/etc/sound_trigger_mixer_paths_wcd9335.xml"
#define MIXER_PATH_XML_SKUW "/vendor/etc/sound_trigger_mixer_paths_skuw.xml"
#define PLATFORM_PATH_XML "/vendor/etc/sound_trigger_platform_info.xml"
#endif
#define MIXER_PATH_XML_WCD9340 "/vendor/etc/sound_trigger_mixer_paths_wcd9340.xml"

#define LIB_ACDB_LOADER "libacdbloader.so"
#define LIB_ADPCM_DECODER "libadpcmdec.so"
#define LIB_MULAW_DECODER "libmulawdec.so"
#define BUF_SIZE 1024

#define MAX_SND_CARD 8
#define MAX_GCS_USECASE_ACDB_IDS 8
#define MIXER_OPEN_MAX_NUM_RETRY 10
#define RETRY_US 500000
#define PLATFORM_XML_VERSION_0x0102 0x0102

/* Default ACDB ids. TODO-V: can be removed as default xml is anyway hosting these */
#define DEVICE_HANDSET_APE_ACDB_ID   (127)
#define DEVICE_HANDSET_CPE_ACDB_ID   (128)
#define DEVICE_HEADSET_CPE_ACDB_ID   (139)
#define DEVICE_HANDSET_ARM_ACDB_ID   (4)

#define DEVICE_NAME_MAX_SIZE 128

/* ACDB app type for LSM non topology */
#define ACDB_LSM_APP_TYPE_NO_TOPOLOGY (1)

/* Maximum firmware image name length */
#define CPE_IMAGE_FNAME_SIZE_MAX (64)

/* Default keyword duration in milliseconds to be captured using FTRT mode */
#define DEFAULT_MAX_KEYWORD_DURATION_MS (2000)

/* Default maximum delay expected from client for first lab data read request
   after sending detection event */
#define DEFAULT_MAX_CLIENT_LAB_READ_DELAY_MS (2000)

/* macros used to verify all param id tags defined for vendor uuid
   in platform file */
#define PARAM_LOAD_SOUND_MODEL_BIT (1 << 0)
#define PARAM_UNLOAD_SOUND_MODEL_BIT (1 << 1)
#define PARAM_CONFIDENCE_LEVELS_BIT (1 << 2)
#define PARAM_OPERATION_MODE_BIT (1 << 3)
#define PARAM_POLLING_ENABLE_BIT (1 << 4)
#define PARAM_CUSTOM_CONFIG_BIT (1 << 5)
#define PARAM_ID_MANDATORY_BITS \
    (PARAM_LOAD_SOUND_MODEL_BIT | PARAM_UNLOAD_SOUND_MODEL_BIT)

/* ACDB app type for device default topology */
#define SOUND_TRIGGER_DEVICE_DEFAULT_APP_TYPE (69938)

/* Supported audio catpure devices */
#define ST_CAPTURE_DEVICES (AUDIO_DEVICE_IN_BUILTIN_MIC | \
                            AUDIO_DEVICE_IN_WIRED_HEADSET)

#define ST_CAPTURE_DEVICE_IS_SUPPORTED(device) \
    (ST_CAPTURE_DEVICES & ~AUDIO_DEVICE_BIT_IN & device)

#define ST_BE_DAI_NAME_MAX_LENGTH 24
#define ST_SND_CARD_NAME_MAX_LENGTH 64

#define DEFAULT_TRANSIT_WAIT_TIME_SEC 5

/* SVA ARM mode definitions */
#define CAPTURE_PCM_USECASE_STRING "MultiMedia2"
#define CAPTURE_PCM_USECASE_PATH "audio-capture"
#define CAPTURE_PCM_USECASE_INDEX 0
#define CAPTURE_EC_REF_USECASE_STRING "MultiMedia9"
#define CAPTURE_EC_REF_USECASE_PATH "ec-ref-audio-capture"
#define CAPTURE_EC_REF_USECASE_INDEX 1

typedef enum  {
    CPE_PCM_BACK_END_DEFAULT,
    CPE_PCM_BACK_END_HWMAD = CPE_PCM_BACK_END_DEFAULT,
    CPE_PCM_BACK_END_ECPP
} st_cpe_pcm_be_type_t;

enum {
    ST_DEVICE_NONE = 0,
    ST_DEVICE_MIN,
    ST_DEVICE_HANDSET_MIC = ST_DEVICE_MIN,
    ST_DEVICE_HANDSET_MIC_ECPP,
    ST_DEVICE_HANDSET_DMIC,
    ST_DEVICE_HANDSET_TMIC,
    ST_DEVICE_HANDSET_QMIC,
    ST_DEVICE_HEADSET_MIC,
    ST_DEVICE_HANDSET_MIC_PP,
    ST_DEVICE_HANDSET_6MIC,
    ST_DEVICE_HANDSET_8MIC,
    ST_DEVICE_MAX,
};

typedef int st_device_t;

typedef enum {
    ST_SESSION_CAL, /* lsm cal */
    ST_DEVICE_CAL,  /* hwmad, afe cal */
} st_cal_type;

struct st_device_index {
    char name[100];
    unsigned int index;
};

struct hwdep_cal_param_data {
    int use_case;
    int acdb_id;
    int get_size;
    int buff_size;
    int data_size;
    void *buff;
};

typedef enum {
    ADPCM_CUSTOM_PACKET = 0x01,
    ADPCM_RAW = 0x02,
    PCM_CUSTOM_PACKET = 0x04,
    PCM_RAW = 0x08,
    MULAW_RAW = 0x10,
    ADPCM = (ADPCM_CUSTOM_PACKET | ADPCM_RAW),
    PCM = (PCM_CUSTOM_PACKET | PCM_RAW),
    MULAW = MULAW_RAW
} st_capture_format_t;

typedef enum {
    RT_TRANSFER_MODE,
    FTRT_TRANSFER_MODE,
} st_capture_mode_t;

enum st_param_id_type {
    LOAD_SOUND_MODEL,
    UNLOAD_SOUND_MODEL,
    CONFIDENCE_LEVELS,
    OPERATION_MODE,
    POLLING_ENABLE,
    DETECTION_EVENT,
    READ_REQ,
    READ_RSP,
    CUSTOM_CONFIG,
    START_ENGINE,
    RESTART_ENGINE,
    MAX_PARAM_IDS
};

struct st_module_param_info {
    unsigned int module_id;
    unsigned int instance_id;
    unsigned int param_id;
};

typedef enum {
    ST_PROFILE_TYPE_NONE, /* Connect LSM to AFE directly */
    ST_PROFILE_TYPE_DEFAULT, /* Connect LSM to ADM with default config */
    ST_PROFILE_TYPE_UNPROCESSED, /* Connect LSM to ADM with Unprocessed config */
    ST_PROFILE_TYPE_EC, /* Connect LSM to ADM with Echo Cancellation config */
    ST_PROFILE_TYPE_FLUENCE, /*Connect LSM to ADM w/ Fluence Pro config - Mono out */
    ST_PROFILE_TYPE_FLUENCE_STEREO /*Connect LSM to ADM w/ Fluence Pro config - Stereo out*/
} st_profile_type_t;

typedef enum {
    ST_FLUENCE_TYPE_NONE,
    ST_FLUENCE_TYPE_MONO,
    ST_FLUENCE_TYPE_DMIC,
    ST_FLUENCE_TYPE_TMIC,
    ST_FLUENCE_TYPE_QMIC
} st_fluence_type_t;

enum st_bad_mic_ch_index {
    BAD_MIC_CH_INDEX_0 = 0x1,
    BAD_MIC_CH_INDEX_1 = 0x2,
    BAD_MIC_CH_INDEX_2 = 0x4,
    BAD_MIC_CH_INDEX_3 = 0x8,
    BAD_MIC_CH_SUPPORTED_INDICES = (BAD_MIC_CH_INDEX_0 |
                                    BAD_MIC_CH_INDEX_1 |
                                    BAD_MIC_CH_INDEX_2 |
                                    BAD_MIC_CH_INDEX_3),
};

/* soundmodel library wrapper functions */
typedef int (*smlib_generate_sound_trigger_phrase_recognition_event_t)
(
    const struct sound_trigger_phrase_sound_model *sm,
    const struct sound_trigger_recognition_config *config,
    const void *payload,
    unsigned int payload_size,
    struct sound_trigger_phrase_recognition_event **r_event
);

typedef int (*smlib_generate_sound_trigger_recognition_config_payload_t)
(
   const struct sound_trigger_phrase_sound_model *sm,
   const struct sound_trigger_recognition_config *config,
   unsigned char **out_payload,
   unsigned int *out_payload_size
);

typedef int (*smlib_generate_sound_trigger_phrase_recognition_event_v3_t)
(
    const struct sound_trigger_phrase_sound_model *sm,
    const struct sound_trigger_recognition_config *config,
    const void *payload,
    unsigned int payload_size,
    qsthw_recognition_event_type_t event_type,
    void **r_event
);

struct st_gcs_params {
    struct listnode list_node;
    unsigned int uid;
    unsigned int acdb_ids[MAX_GCS_USECASE_ACDB_IDS];
    struct st_module_param_info params[MAX_PARAM_IDS];
};

struct st_vendor_info {
    struct listnode list_node;
    sound_trigger_uuid_t uuid;
    int app_type;
    int sample_rate;
    bool is_qc_uuid;
    unsigned int fwk_mode;
    int format;
    int channel_count;
    st_profile_type_t profile_type;
    int param_tag_tracker[ST_EXEC_MODE_MAX];
    st_fluence_type_t fluence_type;
    st_fluence_type_t wdsp_fluence_type;
    bool split_ec_ref_data;
    int ec_ref_channel_cnt;

    char cpe_firmware_image[CPE_IMAGE_FNAME_SIZE_MAX];
    st_capture_format_t kw_capture_format;
    st_capture_mode_t kw_transfer_mode;
    unsigned int kw_duration;
    unsigned int client_capture_read_delay;
    unsigned int avail_cpe_phrases;
    unsigned int avail_cpe_users;
    unsigned int avail_ape_phrases;
    unsigned int avail_ape_users;
    unsigned int avail_transit_cpe_phrases;
    unsigned int avail_transit_cpe_users;
    unsigned int avail_transit_ape_phrases;
    unsigned int avail_transit_ape_users;
    st_exec_mode_config_t exec_mode_cfg;

    struct st_module_param_info params[ST_EXEC_MODE_MAX][MAX_PARAM_IDS];
    struct listnode gcs_usecase_list; /* list of gcs usecases one entry per uid */

    void *smlib_handle;
    smlib_generate_sound_trigger_recognition_config_payload_t
                                    generate_st_recognition_config_payload;
    smlib_generate_sound_trigger_recognition_config_payload_t
                                    generate_st_recognition_config_payload_v2;

    smlib_generate_sound_trigger_phrase_recognition_event_t
                                    generate_st_phrase_recognition_event;
    smlib_generate_sound_trigger_phrase_recognition_event_t
                                    generate_st_phrase_recognition_event_v2;
    smlib_generate_sound_trigger_phrase_recognition_event_v3_t
                                        generate_st_phrase_recognition_event_v3;
};

typedef struct st_codec_backend_cfg {
    int sample_rate;
    int format;
    int channel_count;
    char *samplerate_mixer_ctl;
    char *format_mixer_ctl;
    char *channelcount_mixer_ctl;
} st_codec_backend_cfg_t;

struct adm_cfg_info {
    struct listnode list_node;
    st_profile_type_t profile_type;
    int app_type;
    int sample_rate;
    int bit_width;
};

struct st_be_dai_name_table {
    unsigned int be_id;
    char be_name[ST_BE_DAI_NAME_MAX_LENGTH];
};

void *platform_stdev_init(struct sound_trigger_device *stdev);

void platform_stdev_deinit(void *platform);

int platform_stdev_get_device
(
   void *platform,
   struct st_vendor_info* v_info,
   audio_devices_t device,
   st_exec_mode_t exec_mode
);

int platform_stdev_get_acdb_id
(
    st_device_t st_device,
    st_exec_mode_t exec_mode
);

int platform_stdev_get_device_name
(
   void *platform,
   st_exec_mode_t exec_mode,
   st_device_t st_device,
   char *device_name
);

audio_devices_t platform_stdev_get_capture_device(void *platform);

int platform_stdev_update_avail_device
(
    void *platform,
    audio_devices_t device,
    bool connect
);

int platform_stdev_send_calibration
(
    void *platform,
    audio_devices_t device,
    st_exec_mode_t exec_mode,
    struct st_vendor_info * v_info,
    int app_id,
    bool use_topology,
    st_cal_type type
);

bool platform_stdev_check_and_update_concurrency
(
   void *platform,
   audio_event_type_t event_type,
   unsigned int num_sessions
);

bool platform_stdev_is_session_allowed
(
   void *platform,
   unsigned int num_sessions
);

int platform_stdev_connect_mad
(
    void *platform,
    st_exec_mode_t mode,
    st_profile_type_t profile_type
);

struct st_vendor_info* platform_stdev_get_vendor_info
(
   void *platform,
   sound_trigger_uuid_t *vendor_uuid
);

void platform_stdev_check_and_update_pcm_config
(
   void *platform,
   struct pcm_config *config,
   struct st_vendor_info *v_info,
   enum st_exec_mode exec_mode
);

int platform_stdev_get_hw_type(void *platform);

int platform_cpe_get_pcm_device_id
(
   void *platform,
   int sample_rate,
   unsigned int* use_case_idx
);

int platform_ape_get_pcm_device_id
(
   void *platform,
   unsigned int* use_case_idx
);

void platform_ape_free_pcm_device_id
(
   void *platform,
   int pcm_id
);

void platform_cpe_free_pcm_device_id
(
   void *platform,
   int pcm_id
);

int platform_arm_get_pcm_device_id
(
   void *platform,
   unsigned int* use_case_idx,
   bool ec_ref_dev
);

void platform_arm_free_pcm_device_id
(
   void *platform,
   int pcm_id,
   bool ec_ref_dev
);

/*
 * Acquires one of the available gcs UIDs in the vendor info, returns
 * NULL if no more use_case ids are available (note this is not thread safe)
 */
void platform_alloc_gcs_usecase
(
   void* platform,
   struct st_vendor_info* v_info,
   struct st_gcs_params** gcs_usecase,
   unsigned int acdb_id
);

/*
 * Frees the provided gcs UID by returning it to the list of availble
 * UIDs in the vendor info (note this is not thread safe)
 */
void platform_free_gcs_usecase
(
   struct st_vendor_info* v_info,
   struct st_gcs_params* gcs_usecase
);

int platform_stdev_check_and_set_codec_backend_cfg
(
   void *platform,
   struct st_vendor_info *v_info,
   bool *backend_cfg_change
);

int platform_stdev_send_stream_app_type_cfg
(
   void *platform,
   int pcm_id,
   st_device_t st_device,
   st_exec_mode_t exec_mode,
   st_profile_type_t profile_type
);

int platform_stdev_get_device_app_type
(
   void *platform,
   st_profile_type_t profile_type
);

void platform_stdev_send_ec_ref_cfg
(
   void *platform,
   st_profile_type_t profile_type,
   bool enable
);

int platform_stdev_send_custom_channel_mixing_coefficients
(
   void *platform,
   struct st_vendor_info *v_info,
   int pcm_id,
   char *str
);

int platform_stdev_update_bad_mic_channel_index
(
   void *platform,
   int bad_mic_channel_index,
   bool *backend_cfg_change
);

bool platform_stdev_check_backends_match
(
   void *platform,
   st_exec_mode_t exec_mode1,
   st_device_t st_device1,
   st_exec_mode_t exec_mode2,
   st_device_t st_device2
);

void platform_stdev_check_and_append_usecase(
   void *platform,
   char *use_case,
   st_profile_type_t profile_type
);

void platform_stdev_check_and_update_ec_ref_config
(
   void *platform,
   struct st_vendor_info *v_info,
   struct pcm_config *config
);

bool platform_stdev_check_concurrent_capture
(
   void *platform,
   audio_stream_usecase_type_t usecase_type
);

#endif /* SOUND_TRIGGER_PLATFORM_H */
