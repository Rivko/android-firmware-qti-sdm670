/* sound_trigger_platform.h
 *
 * Copyright (c) 2013-2016, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef SOUND_TRIGGER_PLATFORM_H
#define SOUND_TRIGGER_PLATFORM_H

#include "sound_trigger_hw.h"

#define PLATFORM_PATH_XML "/vendor/etc/sound_trigger_platform_info.xml"
#define MIXER_PATH_XML "/vendor/etc/sound_trigger_mixer_paths.xml"
#define MIXER_PATH_XML_WCD9306 "/vendor/etc/sound_trigger_mixer_paths_wcd9306.xml"
#define MIXER_PATH_XML_WCD9330 "/vendor/etc/sound_trigger_mixer_paths_wcd9330.xml"
#define MIXER_PATH_XML_WCD9335 "/vendor/etc/sound_trigger_mixer_paths_wcd9335.xml"
#define LIB_ACDB_LOADER "libacdbloader.so"
#define LIB_SM_WRAPPER "libsmwrapper.so"
#define LIB_ADPCM_DECODER "libadpcmdec.so"
#define BUF_SIZE 1024

#define SOUND_CARD 0
#define MIXER_OPEN_MAX_NUM_RETRY 10
#define RETRY_US 500000

#ifdef PLATFORM_DEFAULT
  #define DEVICE_HANDSET_APE_ACDB_ID   (127)
  #define DEVICE_HANDSET_CPE_ACDB_ID   (128)
#else
  #define DEVICE_HANDSET_APE_ACDB_ID   (100)
  #define DEVICE_HANDSET_CPE_ACDB_ID   (128)
#endif

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
    ST_DEVICE_MAX,
};

typedef int st_device_t; //TODO

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
    ADPCM = (ADPCM_CUSTOM_PACKET | ADPCM_RAW),
    PCM = (PCM_CUSTOM_PACKET | PCM_RAW),
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
    MAX_PARAM_IDS
};

struct st_module_param_info {
    unsigned int module_id;
    unsigned int param_id;
};

struct st_vendor_info {
    struct listnode list_node;
    sound_trigger_uuid_t uuid;
    int app_type;
    int sample_rate;
    bool is_qc_uuid;

    char cpe_firmware_image[CPE_IMAGE_FNAME_SIZE_MAX];
    st_capture_format_t kw_capture_format;
    st_capture_mode_t kw_transfer_mode;
    unsigned int kw_duration;
    unsigned int client_capture_read_delay;
    unsigned int avail_cpe_phrases;
    unsigned int avail_cpe_users;
    unsigned int avail_ape_phrases;
    unsigned int avail_ape_users;

    struct st_module_param_info params[MAX_PARAM_IDS];
    void *smlib_handle;
    smlib_generate_sound_trigger_recognition_config_payload_t
                                    generate_st_recognition_config_payload;
    smlib_generate_sound_trigger_phrase_recognition_event_t
                                    generate_st_phrase_recognition_event;
};

void *platform_stdev_init(sound_trigger_device_t *stdev);

void platform_stdev_deinit(void *platform);

int platform_stdev_get_device
(
   void *platform,
   sound_trigger_session_t *p_ses,
   audio_devices_t device
);

int platform_stdev_get_device_name
(
   void *platform,
   st_exec_mode_t exec_mode,
   st_device_t st_device,
   char *device_name
);

int platform_stdev_send_calibration
(
    void *platform,
    audio_devices_t device,
    sound_trigger_session_t *p_ses,
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
   unsigned int num_sessions,
   bool sound_model_loaded
);

int platform_stdev_connect_mad
(
    void *platform,
    st_exec_mode_t mode
);

struct st_vendor_info* platform_stdev_get_vendor_info
(
   void *platform,
   sound_trigger_uuid_t *vendor_uuid,
   bool *qc_uuid
);

void platform_stdev_check_and_update_pcm_config
(
   void *platform,
   struct pcm_config *config
);

int platform_stdev_get_hw_type(void *platform);

int platform_cpe_get_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses,
   int sample_rate
);

int platform_ape_get_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses
);

void platform_ape_free_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses
);

void platform_cpe_free_pcm_device_id
(
   void *platform,
   sound_trigger_session_t *p_ses
);

#endif /* SOUND_TRIGGER_PLATFORM_H */
