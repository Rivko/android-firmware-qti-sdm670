/* sound_trigger_prop_intf.h
 *
 *  Interface between Audio Hal and Sound Trigger Hal for
 *  notifying events
 *
 * Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.

 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "tinyalsa/asoundlib.h"

#ifndef SOUND_TRIGGER_PROP_INTF_H
#define SOUND_TRIGGER_PROP_INTF_H

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
    AUDIO_EVENT_SVA_EXEC_MODE_STATUS
};
typedef enum audio_event_type audio_event_type_t;

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
    }u;
};
typedef struct audio_event_info audio_event_info_t;

/* STHAL callback which is called by AHAL */
typedef int (*sound_trigger_hw_call_back_t)(enum audio_event_type,
                                  struct audio_event_info*);

/* AHAL callback which is called by STHAL */
typedef void (*audio_hw_call_back_t)(enum sound_trigger_event_type,
                          struct sound_trigger_event_info*);

/* AHAL function which is called by STHAL */
typedef int (*audio_hw_acdb_init_t)(int snd_card_num);

/* AHAL extn util function which is called by STHAL */
typedef int (*audio_hw_get_snd_card_num_t)();

#endif /* SOUND_TRIGGER_PROP_INTF_H */
