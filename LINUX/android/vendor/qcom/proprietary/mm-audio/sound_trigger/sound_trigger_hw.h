/* sound_trigger_hw.h
 *
 * This library contains the API to load sound models with
 * DSP and start/stop detection of associated key phrases.
 *
 * Copyright (c) 2013-2015, 2017 QUALCOMM Technologies, Inc. All Rights Reserved.
 * QUALCOMM Technologies Proprietary and Confidential.
 *
 */

#ifndef SOUND_TRIGGER_HW_H
#define SOUND_TRIGGER_HW_H

#include <cutils/list.h>
#include <pthread.h>
#include <errno.h>
#include <hardware/sound_trigger.h>
#include <tinyalsa/asoundlib.h>
#include <audio_route/audio_route.h>

#include <sound/lsm_params.h>
#include "sound_trigger_prop_intf.h"

#define PCM_DEVICE_FILE "/proc/asound/pcm"

#ifdef __LP64__
#define AUDIO_HAL_LIBRARY_PATH "/vendor/lib64/hw/audio.primary.%s.so"
#else
#define AUDIO_HAL_LIBRARY_PATH "/vendor/lib/hw/audio.primary.%s.so"
#endif

#define SOUND_TRIGGER_SAMPLING_RATE (16000)
#define SOUND_TRIGGER_SAMPLING_RATE_48000 (48000)
#define SOUND_TRIGGER_SAMPLING_RATE_384000 (384000)
#define SOUND_TRIGGER_CHANNEL_MODE  (1)

#define SOUND_TRIGGER_MAX_EVNT_PAYLOAD_SIZE (256)

#define SOUND_TRIGGER_PCM_MAX_RETRY (10)
#define SOUND_TRIGGER_PCM_SLEEP_WAIT (1000)

#define ST_READ_WAIT_TIME_OUT_SEC (2)

#define SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS (240)
#define SOUND_TRIGGER_CPE_PERIOD_COUNT (6)
#define CALCULATE_PERIOD_SIZE(duration_ms, sample_rate, period_cnt, align) \
       ((((sample_rate * duration_ms) /(period_cnt * 1000)) + (align - 1)) & ~(align-1))

#define CEIL(a, b) (((a) + ((b)-1)) / (b))
/* Number of pcm reads to try to find the first sync word */
#define SOUND_TRIGGER_CPE_FTRT_SYNC_MAX_RETRY_DURATION_MS (160)
#define SOUND_TRIGGER_SYNC_WORD_MAX_RETRY_CNT ( \
    (CEIL((SOUND_TRIGGER_CPE_FTRT_SYNC_MAX_RETRY_DURATION_MS * \
     SOUND_TRIGGER_CPE_PERIOD_COUNT), \
     SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS)))

#define SOUND_TRIGGER_CPE_PERIOD_SIZE \
     CALCULATE_PERIOD_SIZE(SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS, \
     SOUND_TRIGGER_SAMPLING_RATE, SOUND_TRIGGER_CPE_PERIOD_COUNT, 32)

#define SOUND_TRIGGER_APE_BUFFER_DURATION_MS (1000)
#define SOUND_TRIGGER_APE_PERIOD_COUNT (8)
#define SOUND_TRIGGER_APE_PERIOD_SIZE \
    ((((SOUND_TRIGGER_SAMPLING_RATE * SOUND_TRIGGER_APE_BUFFER_DURATION_MS) \
       /(SOUND_TRIGGER_APE_PERIOD_COUNT * 1000)) + 0x1f) & ~0x1f)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SET_BIT(a, b) (a |= b)
#define CLEAR_BIT(a, b) (a &= ~(b))
#define CHECK_BIT(a, b) ((a) & (b))

#define SET_STATE(a, b) SET_BIT(a, b)
#define CLEAR_STATE(a, b) CLEAR_BIT(a, b)
#define CHECK_STATE(a, b) CHECK_BIT(a, b)

#define USECASE_STRING_SIZE (50)

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

struct cpe_packet_hdr {
    unsigned char sync_word;
    unsigned char stream_type;
    unsigned char stream_id;
    unsigned char format;
    unsigned short size;
} __packed;

typedef enum {
    ST_DEVICE_HW_NONE,
    ST_DEVICE_HW_APE,
    ST_DEVICE_HW_CPE
}st_hw_type_t;

typedef enum {
    ST_DEVICE_EXEC_MODE_NONE = -1,
    ST_DEVICE_EXEC_MODE_APE,
    ST_DEVICE_EXEC_MODE_CPE,
    ST_DEVICE_EXEC_MODE_MAX
}st_exec_mode_t;

typedef enum {
    ST_DEVICE_SSR_APE = 0x1,
    ST_DEVICE_SSR_CPE = 0x2
}st_ssr_state_t;

/* guideline for locks needed to guard ses states:
 * 1. SES_STARTED  must be under stdev lock, ses lock optional.
 * 2. SES_BUFFERING and SES_TRANSIT_STARTED must have transit lock
 *    since BUFFERING and TRANSIT_START can be updated from other ses.
 * 3. All other states must have ses lock.
 */
enum sound_trigger_states {
    SES_CREATED = 0x01,
    SES_STARTED = 0x02,
    SES_BUFFERING = 0x04,
    SES_INVALIDATED = 0x08,
    SES_TRANSIT_LOADING = 0x10,
    SES_TRANSIT_LOADED = 0x20,
    SES_TRANSIT_STARTED = 0x40,
    SES_TRANSIT_SUSPENDED = 0x80,
    SES_TRANSIT_INVALIDATED = 0x100
};

#ifdef ST_TRANSITION_ENABLED
enum sound_trigger_transition_cmds {
    ST_TRANSIT_CMD_NONE,
    ST_TRANSIT_CMD_LOAD_AND_START,
    ST_TRANSIT_CMD_EXIT
};

struct sound_trigger_transition_cmd {
    enum sound_trigger_transition_cmds cmd;
    sound_model_handle_t sm_handle;
};
#endif

struct use_case_info {
    bool active;
    int pcm_id;
    int pcm_back_end;
    char use_case[USECASE_STRING_SIZE];
};

struct sound_trigger_session {
    struct listnode list_node;
    struct sound_trigger_hw_device *device;
    struct sound_trigger_session_fptrs *fptrs;
    st_exec_mode_t exec_mode;

    sound_model_handle_t sm_handle;
    sound_trigger_sound_model_type_t sm_type;

    void *sm_data;
    struct sound_trigger_recognition_config *rc_config;

    recognition_callback_t callback;
    void *cookie;
    pthread_t callback_thread;
    bool exit_callback_thread;
    audio_io_handle_t capture_handle;
    bool capture_requested;
    unsigned int recognition_mode;
    bool client_started;
    bool send_sm_params;

    struct pcm *pcm;
    struct pcm_config  config;
    struct pcm_config  lab_config;
    int pcm_id;

    enum sound_trigger_states state;
    unsigned int use_case_idx;
    unsigned int num_phrases;
    unsigned int num_users;
    unsigned int num_conf_levels;
    unsigned char *conf_levels;

    pthread_mutex_t lock;
    struct sound_trigger_event_info event_info;
    struct st_vendor_info *vendor_uuid_info;
    bool use_topology_intf;

    struct sound_trigger_session *transit_ses;

    /* Buffers used in FTRT mode */
    unsigned char *lab_drv_buf;
    unsigned int lab_drv_buf_size;
    unsigned char *dec_buf;
    unsigned char *pcm_out_buf;
    unsigned int pcm_out_buf_size;
    unsigned int pcm_out_buf_unread_bytes;
    unsigned char *pcm_out_buf_start_ptr;
    unsigned char *pcm_out_buf_end_ptr;
    unsigned char *pcm_out_buf_wr_ptr;
    unsigned char *pcm_out_buf_rd_ptr;
    pthread_cond_t pcm_out_buf_cond;
    pthread_mutex_t pcm_out_buf_lock;

    bool exit_lab_processing;
    bool lab_buffers_allocated;

    void *adpcm_dec_state;
};
typedef struct sound_trigger_session sound_trigger_session_t;

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

/* ADPCM decoder APIs */
typedef int (*g722_init_decoder_t)(void *decoder_inp);
typedef int (*g722_dec_get_total_byte_size_t)(int *total_byte_size);
typedef int (*g722_dec_process_t)(short *in_buf, short *out_buf,
                                   int in_bytes, int *out_samples,
                                   void *decoder_inp);

/* Function pointers to routing layers */
typedef int (*sound_trigger_reg_sm_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_reg_sm_params_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_dereg_sm_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_prepare_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_unprepare_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_start_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_stop_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_stop_buffering_t)(sound_trigger_session_t *);
typedef int (*sound_trigger_set_device_t)(sound_trigger_session_t *,
                                          bool, audio_devices_t);
typedef int (*sound_trigger_read_pcm_t)(sound_trigger_session_t *,
                              unsigned char *, unsigned int );
typedef void (*sound_trigger_lab_capture_t)(sound_trigger_session_t *);


struct sound_trigger_session_fptrs {
    sound_trigger_reg_sm_t reg_sm;
    sound_trigger_reg_sm_params_t reg_sm_params;
    sound_trigger_dereg_sm_t dereg_sm;
    sound_trigger_prepare_t prepare;
    sound_trigger_prepare_t unprepare;
    sound_trigger_start_t start;
    sound_trigger_stop_t stop;
    sound_trigger_stop_buffering_t stop_buffering;
    sound_trigger_set_device_t set_device;
    sound_trigger_read_pcm_t read_pcm;
    sound_trigger_lab_capture_t process_lab_capture;
};

struct sound_trigger_device {
    struct sound_trigger_hw_device device;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    struct listnode sound_model_list;
    struct mixer *mixer;
    struct audio_route *audio_route;
    volatile int session_id;
    st_ssr_state_t ssr_state;
    audio_devices_t capture_device;
    int *dev_ref_cnt;

    unsigned int tx_concurrency_active;
    unsigned int rx_concurrency_active;
    bool audio_concurrency_active;
    bool rx_concurrency_disabled;
    bool sw_mad;
    bool run_on_ape;
    bool transition_enabled;
    int num_sessions_configured;
    unsigned int max_cpe_sessions;
    unsigned int avail_cpe_phrases;
    unsigned int avail_cpe_users;
    unsigned int max_ape_sessions;
    unsigned int avail_ape_phrases;
    unsigned int avail_ape_users;
    unsigned int rx_conc_max_st_ses;
    struct use_case_info *ape_pcm_use_cases;
    struct use_case_info *cpe_pcm_use_cases;
    bool detect_failure;
    void *platform;
    st_hw_type_t hw_type;
    st_exec_mode_t exec_mode;

    void *audio_hal_handle;
    audio_hw_call_back_t audio_hal_cb;

    struct listnode vendor_uuid_list;
    void *smlib_handle;
    smlib_generate_sound_trigger_recognition_config_payload_t
                                    generate_st_recognition_config_payload;
    smlib_generate_sound_trigger_phrase_recognition_event_t
                                    generate_st_phrase_recognition_event;

#ifdef ST_TRANSITION_ENABLED
    pthread_mutex_t transit_lock;
    pthread_cond_t  transit_cond;
    struct sound_trigger_transition_cmd transit_cmd;
    pthread_t transition_thread;
#endif

    void *adpcm_dec_lib_handle;
    g722_init_decoder_t adpcm_dec_init;
    g722_dec_get_total_byte_size_t adpcm_dec_get_scratch_size;
    g722_dec_process_t adpcm_dec_process;
};
typedef struct sound_trigger_device sound_trigger_device_t;

unsigned int get_num_sessions_in_state(enum sound_trigger_states state);
struct sound_trigger_session* get_sound_trigger_session
(
    struct sound_trigger_device *stdev,
    sound_model_handle_t sound_model_handle
);
void reg_hal_event_session(sound_trigger_session_t *p_ses);
void dereg_hal_event_session(sound_trigger_session_t *p_ses);
void update_hw_mad_exec_mode(st_exec_mode_t mode);


int ape_reg_sm(sound_trigger_session_t* p_ses);
int ape_reg_sm_params(sound_trigger_session_t* p_ses);
int ape_dereg_sm(sound_trigger_session_t* p_ses);
int ape_start(sound_trigger_session_t* p_ses);
int ape_stop(sound_trigger_session_t* p_ses);
int ape_stop_buffering(sound_trigger_session_t* p_ses);

int cpe_reg_sm(sound_trigger_session_t* p_ses);
int cpe_reg_sm_params(sound_trigger_session_t* p_ses);
int cpe_dereg_sm(sound_trigger_session_t* p_ses);
int cpe_start(sound_trigger_session_t* p_ses);
int cpe_stop(sound_trigger_session_t* p_ses);
int cpe_stop_buffering(sound_trigger_session_t* p_ses);

#ifdef ST_TRANSITION_ENABLED
void transit_stdev_init(struct sound_trigger_device *stdev);
void transit_stdev_deinit(struct sound_trigger_device *stdev);
void transit_reassign_fptrs(struct sound_trigger_session *p_ses);
void transit_retrieve_st_session(struct sound_trigger_session **p_ses);
bool transit_ses_loading(struct sound_trigger_session *p_ses);
bool transit_ses_buffering(sound_trigger_session_t* p_ses,
    audio_io_handle_t capture_handle);
bool transit_enabled(struct sound_trigger_device *stdev);
void transit_handle_cpe_ssr_event(struct sound_trigger_device *stdev,
                                  enum ssr_event_status event);
#else
#define transit_stdev_init(stdev) (0)
#define transit_stdev_deinit(stdev) (0)
#define transit_reassign_fptrs(p_ses) (0)
#define transit_retrieve_st_session(p_ses) (0)
#define transit_ses_loading(p_ses) (0)
#define transit_ses_buffering(p_ses, pcm) (0)
#define transit_enabled(stdev) (0)
#define transit_handle_cpe_ssr_event(stdev, event) (0)
#endif

/* Debug helper macros */
/* #define ST_DEBUG_DUMP */
#define ST_DEBUG_DUMP_LOCATION "/data/vendor/misc/audio"

#ifdef ST_DEBUG_DUMP
#define ST_DBG_DECALRE(args...) args

#define ST_DBG_FILE_OPEN_WR(fptr, fpath, fname, fextn, fcount) \
do {\
    char fptr_fn[100];\
\
    snprintf(fptr_fn, sizeof(fptr_fn), fpath"/"fname"_%d."fextn, fcount);\
    fptr = fopen(fptr_fn, "wb");\
    if (!fptr) { \
        ALOGE("%s: File open failed %s: %s", \
              __func__, fptr_fn, strerror(errno)); \
    } \
} while (0)

#define ST_DBG_FILE_CLOSE(fptr) \
do {\
    if (fptr) { fclose (fptr); }\
} while (0)

#define ST_DBG_FILE_WRITE(fptr, buf, buf_size) \
do {\
    if (fptr) {\
        int ret_bytes = fwrite(buf, 1, buf_size, fptr);\
        if (ret_bytes != (int)buf_size) {\
            ALOGE("%s: fwrite %d < %d", __func__, ret_bytes, buf_size);\
        }\
        fflush(fptr);\
    }\
} while (0)
#else
#define ST_DBG_DECALRE(args...) do { } while(0)
#define ST_DBG_FILE_OPEN_WR(fptr, path, fname, fextn, fcount) (0)
#define ST_DBG_FILE_CLOSE(fptr) (0)
#define ST_DBG_FILE_WRITE(fptr, buf, buf_size) (0)
#endif

#endif /* SOUND_TRIGGER_HW_H */

