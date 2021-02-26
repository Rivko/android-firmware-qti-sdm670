/* sound_trigger_prop_intf.h
 *
 * Interface for sound trigger related communication
 * across modules.
 *
 * Copyright (c) 2014, 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef SOUND_TRIGGER_PROP_INTF_H
#define SOUND_TRIGGER_PROP_INTF_H

#include <hardware/sound_trigger.h>
#include "tinyalsa/asoundlib.h"

#define MAKE_HAL_VERSION(maj, min) ((((maj) & 0xff) << 8) | ((min) & 0xff))
#define MAJOR_VERSION(ver) (((ver) & 0xff00) >> 8)
#define MINOR_VERSION(ver) ((ver) & 0x00ff)

/* Proprietary interface version used for compatibility with AHAL */
#define STHAL_PROP_API_VERSION_1_0 MAKE_HAL_VERSION(1, 0)
#define STHAL_PROP_API_CURRENT_VERSION STHAL_PROP_API_VERSION_1_0

#define ST_EVENT_CONFIG_MAX_STR_VALUE 32

enum sound_trigger_event_type {
    ST_EVENT_SESSION_REGISTER,
    ST_EVENT_SESSION_DEREGISTER
};
typedef enum sound_trigger_event_type sound_trigger_event_type_t;

enum audio_event_type {
    AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE,
    AUDIO_EVENT_CAPTURE_DEVICE_ACTIVE,
    AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE,
    AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE,
    AUDIO_EVENT_STOP_LAB,
    AUDIO_EVENT_SSR,
    AUDIO_EVENT_NUM_ST_SESSIONS,
    AUDIO_EVENT_READ_SAMPLES,
    AUDIO_EVENT_DEVICE_CONNECT,
    AUDIO_EVENT_DEVICE_DISCONNECT,
    AUDIO_EVENT_SVA_EXEC_MODE,
    AUDIO_EVENT_SVA_EXEC_MODE_STATUS,
    AUDIO_EVENT_CAPTURE_STREAM_INACTIVE,
    AUDIO_EVENT_CAPTURE_STREAM_ACTIVE,
};
typedef enum audio_event_type audio_event_type_t;

typedef enum {
    USECASE_TYPE_PCM_PLAYBACK,
    USECASE_TYPE_PCM_CAPTURE,
    USECASE_TYPE_VOICE_CALL,
    USECASE_TYPE_VOIP_CALL,
} audio_stream_usecase_type_t;

enum ssr_event_status {
    SND_CARD_STATUS_OFFLINE,
    SND_CARD_STATUS_ONLINE,
    CPE_STATUS_OFFLINE,
    CPE_STATUS_ONLINE
};

struct sound_trigger_session_info {
    void* p_ses; /* opaque pointer to st_session obj */
    int capture_handle;
    struct pcm *pcm;
    struct pcm_config config;
};

struct audio_read_samples_info {
    struct sound_trigger_session_info *ses_info;
    void *buf;
    size_t num_bytes;
};

struct audio_hal_usecase {
    audio_stream_usecase_type_t type;
};

struct sound_trigger_event_info {
    struct sound_trigger_session_info st_ses;
};
typedef struct sound_trigger_event_info sound_trigger_event_info_t;

struct audio_event_info {
    union {
        enum ssr_event_status status;
        int value;
        struct sound_trigger_session_info ses_info;
        struct audio_read_samples_info aud_info;
        char str_value[ST_EVENT_CONFIG_MAX_STR_VALUE];
        struct audio_hal_usecase usecase;
    }u;
};
typedef struct audio_event_info audio_event_info_t;

struct sthw_extn_fptrs {
    int (*set_parameters)(const struct sound_trigger_hw_device *dev,
                          sound_model_handle_t sound_model_handle,
                          const char *kv_pairs);
    size_t (*get_buffer_size)(const struct sound_trigger_hw_device *dev,
                              sound_model_handle_t sound_model_handle);
    int (*read_buffer)(const struct sound_trigger_hw_device *dev,
                       sound_model_handle_t sound_model_handle,
                       unsigned char *buf,
                       size_t bytes);
    int (*stop_buffering)(const struct sound_trigger_hw_device *dev,
                          sound_model_handle_t sound_model_handle);
    int (*get_param_data)(const struct sound_trigger_hw_device *dev,
                          sound_model_handle_t sound_model_handle,
                          const char *param,
                          void *payload,
                          size_t payload_size,
                          size_t *param_data_size);
};
typedef struct sthw_extn_fptrs sthw_extn_fptrs_t;

/* STHAL callback which is called by AHAL */
typedef int (*sound_trigger_hw_call_back_t)(enum audio_event_type,
                                  struct audio_event_info*);

/* STHAL version queried by AHAL */
typedef int (*sound_trigger_hw_get_version_t)();

/* STHAL extn interface functionality called by qti wrapper */
typedef void (*sthw_extn_get_fptrs_t)(sthw_extn_fptrs_t *fptrs);

/* AHAL callback which is called by STHAL */
typedef void (*audio_hw_call_back_t)(enum sound_trigger_event_type,
                          struct sound_trigger_event_info*);

/* AHAL function which is called by STHAL */
typedef int (*audio_hw_acdb_init_t)(int snd_card_num);

/* AHAL extn util function which is called by STHAL */
typedef int (*audio_hw_get_snd_card_num_t)();

#endif /* SOUND_TRIGGER_PROP_INTF_H */
