/* st_hw_session_lsm.h
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */
#include <pthread.h>
#include <tinyalsa/asoundlib.h>
#include <audio_route/audio_route.h>
#include <sound/lsm_params.h>

#include "sound_trigger_platform.h"
#include "st_hw_session.h"
#include "st_common_defs.h"

struct sound_trigger_device;

#define SOUND_TRIGGER_MAX_EVNT_PAYLOAD_SIZE (256)
#define LSM_SM_PARAMS_INFO_IDX (4)

#define SOUND_TRIGGER_PCM_MAX_RETRY (10)
#define SOUND_TRIGGER_PCM_SLEEP_WAIT (1000)

#define SOUND_TRIGGER_APE_PERIOD_COUNT (4)
#define SOUND_TRIGGER_APE_PERIOD_SIZE \
    ((((SOUND_TRIGGER_SAMPLING_RATE_16000 * SOUND_TRIGGER_APE_BUFFER_DURATION_MS) \
       /(SOUND_TRIGGER_APE_PERIOD_COUNT * 1000)) + 0x1f) & ~0x1f)

#define SOUND_TRIGGER_CPE_PERIOD_COUNT (6)
#define SOUND_TRIGGER_CPE_PERIOD_SIZE \
    CALCULATE_PERIOD_SIZE(SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS, \
    SOUND_TRIGGER_SAMPLING_RATE_16000, SOUND_TRIGGER_CPE_PERIOD_COUNT, 32)

#define CEIL(a, b) (((a) + ((b)-1)) / (b))
/* Number of pcm reads to try to find the first sync word */
#define SOUND_TRIGGER_CPE_FTRT_SYNC_MAX_RETRY_DURATION_MS (160)
#define SOUND_TRIGGER_SYNC_WORD_MAX_RETRY_CNT ( \
    (CEIL((SOUND_TRIGGER_CPE_FTRT_SYNC_MAX_RETRY_DURATION_MS * \
     SOUND_TRIGGER_CPE_PERIOD_COUNT), \
     SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS)))

/* ADPCM decoder sample sizes */
#define ADPCM_COMPRESSION_RATIO (4)
#define ADPCM_MAX_IN_FRAME_SIZE (162)
#define ADPCM_MAX_OUT_SAMPLES_PER_FRAME (320)
#define ADPCM_MAX_OUT_FRAME_SIZE (ADPCM_MAX_OUT_SAMPLES_PER_FRAME << 1)

/* CPE LAB Buffer configuration params  */
#define CPE_PACKET_SYNC_WORD (0xAA)
#define CPE_PACKET_FORMAT_MEDIA_TYPE (0x80)
#define CPE_PACKET_FORMAT_EOS (0x81)
#define CPE_PACKET_FORMAT_NULL (0xFF)
#define CPE_PACKET_FORMAT_ADPCM (0x01)
#define CPE_PACKET_FORMAT_PCM (0x00)
#define CPE_PACKET_DATA_FORMAT_RAW (0x01)
#define CPE_PACKET_DATA_FORMAT_PACKED (0x00)

#define LSM_ABORT_RETRY_COUNT (5)
#define LSM_ABORT_WAIT_TIMEOUT_NS (30 * 1000000LL)

#ifdef LSM_EVENT_TIMESTAMP_MODE_SUPPORT
  typedef struct snd_lsm_event_status_v3 st_lsm_event_status_t;
#else
  typedef struct snd_lsm_event_status st_lsm_event_status_t;
#endif

#ifdef LSM_POLLING_ENABLE_SUPPORT
  typedef struct snd_lsm_poll_enable st_lsm_poll_enable_t;
#else
  typedef void* st_lsm_poll_enable_t;
#endif

struct lsm_setparam_payload {
    uint32_t data_payload_size;
    uint32_t data_payload_addr_lsw;
    uint32_t data_payload_addr_msw;
    uint32_t mem_map_handle;
}__packed;

struct lsm_param_payload {
    uint32_t module_id;
    uint32_t param_id;
    uint32_t p_size;
}__packed;

struct lsm_param_custom_config {
    struct lsm_setparam_payload set_param;
    struct lsm_param_payload common;
}__packed;

struct cpe_packet_hdr {
    unsigned char sync_word;
    unsigned char stream_type;
    unsigned char stream_id;
    unsigned char format;
    unsigned short size;
} __packed;

struct st_hw_session_lsm {
    st_hw_session_t common;

    pthread_t callback_thread;
    bool exit_callback_thread;

    int pcm_id;
    struct pcm *pcm;
    struct pcm_config  lab_config;

    bool use_topology_intf;

    struct st_hw_session *transit_ses;

    /* Buffers used in FTRT mode */
    unsigned char *lab_drv_buf; /* small buffer to hold one chunk received from
                                   pcm read */
    unsigned int lab_drv_buf_size;
    unsigned char *dec_buf;
    unsigned char *pcm_out_buf; /* larger buffer that stores samples pulled from driver
                                   this buffer should hold enough samples to handle the
                                   delay from generating the recognition event untill
                                   upper layers start pulling data */
    unsigned int pcm_out_buf_size;
    unsigned int pcm_out_buf_unread_bytes;
    unsigned char *pcm_out_buf_start_ptr;
    unsigned char *pcm_out_buf_end_ptr;
    unsigned char *pcm_out_buf_wr_ptr;
    unsigned char *pcm_out_buf_rd_ptr;
    pthread_cond_t pcm_out_buf_cond;
    pthread_mutex_t pcm_out_buf_lock;

    bool exit_lab_processing;
    bool lab_processing_active;
    pthread_cond_t callback_thread_cond;
    pthread_mutex_t callback_thread_lock;

    bool lab_buffers_allocated;

    void *adpcm_dec_state;
};

typedef struct st_hw_session_lsm st_hw_session_lsm_t;

int st_hw_sess_lsm_init(st_hw_session_t *const p_ses,
   hw_ses_event_callback_t cb, void *cookie, st_exec_mode_t exec_mode,
   struct st_vendor_info *v_info, sound_model_handle_t sm_handle,
   struct sound_trigger_device *stdev);
void st_hw_sess_lsm_deinit(st_hw_session_t *const p_ses);
