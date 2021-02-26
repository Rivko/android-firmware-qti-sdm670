/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

#ifndef QTI_AUDIO_H
#define QTI_AUDIO_H

#include <stdint.h>
#include <system/audio.h>

#define AUDIO_QAF_MAX_CHANNELS 32

/* Device Parameters */
#define AUDIO_QAF_PARAMETER_KEY_DEVICE "o_device"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_SPEAKER "1"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HEADPHONES "2"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HDMI "8"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HEADPHONES_AND_SPK "1,2"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HDMI_AND_SPK "1,8"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HDMI_SPK_HP "1,2,8"
#define AUDIO_QAF_PARAMETER_VALUE_DEVICE_HDMI_HP "2,8"

/* Render format */
#define AUDIO_QAF_PARAMETER_KEY_RENDER_FORMAT "render_format"
#define AUDIO_QAF_PARAMETER_VALUE_REENCODE_AC3 "od"
#define AUDIO_QAF_PARAMETER_VALUE_REENCODE_EAC3 "odp"
#define AUDIO_QAF_PARAMETER_VALUE_REENCODE_DTS "odts"
#define AUDIO_QAF_PARAMETER_VALUE_PCM "pcm"

/* Output Channels */
#define AUDIO_QAF_PARAMETER_KEY_CHANNELS "ch"
#define AUDIO_QAF_PARAMETER_VALUE_DEFAULT_CHANNELS "2"
#define AUDIO_QAF_PARAMETER_VALUE_6_CHANNELS "6"
#define AUDIO_QAF_PARAMETER_VALUE_8_CHANNELS "8"

/* Dap enable or disable values */
#define AUDIO_QAF_PARAMETER_KEY_DAPENABLE "dap_enable"
#define AUDIO_QAF_PARAMETER_VALUE_DAPON "1"
#define AUDIO_QAF_PARAMETER_VALUE_DAPOFF "0"

/* PCM Channel map */
enum qaf_pcm_chmap {
    /* Front left channel. */
    AUDIO_QAF_PCM_CHANNEL_L = 1,
    /* Front right channel. */
    AUDIO_QAF_PCM_CHANNEL_R = 2,
    /* Front center channel. */
    AUDIO_QAF_PCM_CHANNEL_C = 3,
    /* Left surround channel.*/
    AUDIO_QAF_PCM_CHANNEL_LS = 4,
    /* Right surround channel.*/
    AUDIO_QAF_PCM_CHANNEL_RS = 5,
    /* Low frequency effect channel. */
    AUDIO_QAF_PCM_CHANNEL_LFE = 6,
    /* Center surround channel; rear center channel. */
    AUDIO_QAF_PCM_CHANNEL_CS = 7,
    /* Center back channel. */
    AUDIO_QAF_PCM_CHANNEL_CB = AUDIO_QAF_PCM_CHANNEL_CS,
    /* Left back channel; rear left channel. */
    AUDIO_QAF_PCM_CHANNEL_LB = 8,
    /* Right back channel; rear right channel. */
    AUDIO_QAF_PCM_CHANNEL_RB = 9,
    /* Top surround channel.*/
    AUDIO_QAF_PCM_CHANNEL_TS = 10,
    /* Center vertical height channel.*/
    AUDIO_QAF_PCM_CHANNEL_CVH = 11,
    /* Top front center channel.*/
    AUDIO_QAF_PCM_CHANNEL_TFC = AUDIO_QAF_PCM_CHANNEL_CVH,
    /* Mono surround channel.*/
    AUDIO_QAF_PCM_CHANNEL_MS = 12,
    /* Front left of center channel. */
    AUDIO_QAF_PCM_CHANNEL_FLC = 13,
    /* Front right of center channel. */
    AUDIO_QAF_PCM_CHANNEL_FRC = 14,
    /* Rear left of center channel. */
    AUDIO_QAF_PCM_CHANNEL_RLC = 15,
    /* Rear right of center channel. */
    AUDIO_QAF_PCM_CHANNEL_RRC = 16,
    /* Secondary low frequency effect channel. */
    AUDIO_QAF_PCM_CHANNEL_LFE2 = 17,
    /* Side left channel. */
    AUDIO_QAF_PCM_CHANNEL_SL = 18,
    /* Side right channel. */
    AUDIO_QAF_PCM_CHANNEL_SR = 19,
    /* Top front left channel. */
    AUDIO_QAF_PCM_CHANNEL_TFL = 20,
    /* Left vertical height channel. */
    AUDIO_QAF_PCM_CHANNEL_LVH = AUDIO_QAF_PCM_CHANNEL_TFL,
    /* Top front right channel. */
    AUDIO_QAF_PCM_CHANNEL_TFR = 21,
    /* Right vertical height channel. */
    AUDIO_QAF_PCM_CHANNEL_RVH = AUDIO_QAF_PCM_CHANNEL_TFR,
    /* Top center channel. */
    AUDIO_QAF_PCM_CHANNEL_TC = 22,
    /* Top back left channel. */
    AUDIO_QAF_PCM_CHANNEL_TBL = 23,
    /* Top back right channel. */
    AUDIO_QAF_PCM_CHANNEL_TBR = 24,
    /* Top side left channel. */
    AUDIO_QAF_PCM_CHANNEL_TSL = 25,
    /* Top side right channel. */
    AUDIO_QAF_PCM_CHANNEL_TSR = 26,
    /* Top back center channel. */
    AUDIO_QAF_PCM_CHANNEL_TBC = 27,
    /* Bottom front center channel. */
    AUDIO_QAF_PCM_CHANNEL_BFC = 28,
    /* Bottom front left channel. */
    AUDIO_QAF_PCM_CHANNEL_BFL = 29,
    /* Bottom front right channel. */
    AUDIO_QAF_PCM_CHANNEL_BFR = 30,
    /* Left wide channel. */
    AUDIO_QAF_PCM_CHANNEL_LW = 31,
    /* Right wide channel. */
    AUDIO_QAF_PCM_CHANNEL_RW = 32,
    /* Left side direct channel. */
    AUDIO_QAF_PCM_CHANNEL_LSD = 33,
    /* Right side direct channel. */
    AUDIO_QAF_PCM_CHANNEL_RSD = 34
};

typedef struct audio_stream_config {
    uint32_t sample_rate;
    uint32_t channels;
    audio_format_t  format;
} audio_stream_config_t;

/**
 * Enum to identify stream type
 **/
typedef enum audio_stream_type {
    AUDIO_STREAM_MAIN            = 1,
    AUDIO_STREAM_ASSOCIATED,
    AUDIO_STREAM_MAIN_ASSOCIATED,
    AUDIO_STREAM_APP_SOUND,
    AUDIO_STREAM_SYSTEM_TONE,
    AUDIO_STREAM_EXTERN_PCM,
    AUDIO_STREAM_MAX
} stream_type_t;

typedef void* audio_session_handle_t;
typedef void* audio_stream_handle_t;

typedef enum audio_event_id {
    AUDIO_ERROR_EVENT = -1,
    AUDIO_EOS_EVENT,
    AUDIO_DATA_EVENT,
    AUDIO_FLUSH_EVENT,
    AUDIO_EOS_MAIN_DD_DDP_EVENT,
    AUDIO_EOS_MAIN_AAC_EVENT,
    AUDIO_EOS_ASSOC_AAC_EVENT,
    AUDIO_EOS_MAIN_AC4_EVENT,
    AUDIO_EOS_ASSOC_AC4_EVENT,
    AUDIO_EOS_ASSOC_DD_DDP_EVENT,
    AUDIO_EOS_MAIN_2_DD_DDP_EVENT,
    AUDIO_SEC_FAIL_EVENT,

    /* When Stop api is called on main stream,
     * Module wrapper returns this event once data drain is done.
     * payload: NULL
     */
    AUDIO_MAIN_EOS_EVENT,

    /* When Stop api is called on associated stream,
     * Module wrapper returns this event once data drain is done.
     * payload: NULL
     */
    AUDIO_ASSOC_EOS_EVENT,

    /* This event is used by wrapper module to notify
     * the media format of an output stream.
     * payload : audio_qaf_media_format_t
     */
    AUDIO_OUTPUT_MEDIA_FORMAT_EVENT,

    /* This event is used by wrapper module to notify
     * the data buffer of an output stream.
     * payload: audio_qaf_out_buffer_t
     * Note: Device ID is provided as callback function argument.
     */
    AUDIO_DATA_EVENT_V2,

    /* This event is used by wrapper module to notify
     * the metadata.
     */
    AUDIO_METADATA_EVENT
} audio_event_id_t;

/* Payload Structure associated with AUDIO_OUTPUT_MEDIA_FORMAT_EVENT event.*/
typedef struct audio_qaf_media_format {
    audio_format_t format;
    uint32_t sample_rate;
    uint16_t channels;
    uint8_t ch_map[AUDIO_QAF_MAX_CHANNELS];
    uint16_t bit_width;
    bool is_interleaved;

    /* Non-Zero Unique Output ID */
    uint32_t output_id;
} audio_qaf_media_format_t;

/* Payload structure associated with AUDIO_DATA_EVENT_V2 event.*/
typedef struct audio_qaf_out_buffer {
    /* Buffer  pointer */
    int8_t* data;
    /* Offset to the first valid data sample in the buffer */
    uint32_t offset;
    /* Buffer Size */
    uint32_t size;

    /* Non-Zero Unique Output ID */
    uint32_t output_id;

    /* Timestamp of the first valid data sample in the buffer.
     * unit: microseconds*/
    int64_t timestamp;

    /* Pointer to metadata. */
    void* metadata;
} audio_qaf_out_buffer_t;


/* Structure for input buffer.*/
typedef struct audio_qaf_in_buffer {
    /* Buffer  pointer */
    int8_t* data;
    /* Offset to the first valid data sample in the buffer */
    uint32_t offset;
    /* Buffer Size */
    uint32_t size;

     /* Timestamp of the first valid data sample in the buffer.
      * unit: microseconds*/
    int64_t timestamp;

    /* True: Timestamp is in continuation of previous buffer
     * False: Timestamp is to be taken from "timestamp".
     */
    bool is_ts_cont;

    /* True: mark to buffer as end of stream buffer. */
    bool is_eos;
} audio_qaf_in_buffer_t;

/* TODO: Update this compress out format to platform
   independent value
*/
typedef enum audio_compressed_out_format {
    AUDIO_COMPRESSED_OUT_DD = AUDIO_FORMAT_AC3,
    AUDIO_COMPRESSED_OUT_DDP = AUDIO_FORMAT_E_AC3,
} audio_compressed_out_format_t;

typedef struct device_license_config {
    unsigned char*  p_license;
    size_t l_size;
    unsigned long manufacturer_id;
} device_license_config_t;

/* To identify session type */
typedef enum audio_session_type {
    AUDIO_SESSION_BROADCAST,
    AUDIO_SESSION_DECODE,
    AUDIO_SESSION_ENCODE,
} audio_session_type_t;

/* Mixing metadata */
typedef struct audio_ms12_mix_metadata {
    bool mixmdate;
    bool extpgmscle;
    int extpgmscl;
    int mixdef;
    bool extpgmlscle;
    int extpgmlscl;
    bool extpgmcscle;
    int extpgmcscl;
    bool extpgmrscle;
    int extpgmrscl;
    bool extpgmlsscle;
    int extpgmlsscl;
    bool extpgmrsscle;
    int extpgmrsscl;
    bool extpgmlfescle;
    int extpgmlfescl;
    bool dmixscle;
    int dmixscl;
    bool addche;
    bool extpgmaux1scle;
    int extpgmaux1scl;
    bool extpgmaux2scle;
    int extpgmaux2scl;
    bool paninfoe;
    int panmean;
} audio_ms12_mix_metadata_t;

/**
 * Opens a new session with wrapper. This handle is expected to be passed as part of stream specific calls for this session.
 *
 * @param session_handle       - session handle
 * @param s_type               - type of session
 * @param p_data               - qaf handle
 * @param license_data         - license data
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_session_open(audio_session_handle_t* session_handle, audio_session_type_t s_type, void *p_data, void* license_data);

/**
 * Creates an audio stream based on the configuration passed by the client. Output configuration depends on output devices passed to this method.
 * Format for each device is set via set_param call.
 *
 * @param session_handle       - session handle
 * @param stream_handle        - stream handle to be created in by wrapper
 * @param audio_stream_config  - audio configuration of the input stream
 * @param devices              - output devices for this stream
 * @param stream types         - stream types representing the i/p stream. Ex:  main, associated, app sound, system sound etc...
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_open(audio_session_handle_t session_handle, audio_stream_handle_t* stream_handle, audio_stream_config_t input_config, audio_devices_t devices, stream_type_t flags);

/**
 * sets audio stream parameters. The function accepts a list of
 * parameter key value pairs in the form: key1=value1;key2=value2;...
 *
 * @param stream_handle - stream handle
 * @param kv_pairs      - string containing key=value
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_set_param(audio_stream_handle_t stream_handle, const char* kv_pairs);

/**
 * sets audio session or global parameters. The function accepts a list of
 * parameter key value pairs in the form: key1=value1;key2=value2;...
 *
 * @param session_handle - session handle
 * @param kv_pairs        - string containing key=value
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_session_set_param(audio_session_handle_t handle, const char* kv_pairs);

/**
 * gets audio stream parameters
 * @param stream_handle - session handle
 * @param key           - key for which the value is sought
 *
 * @return string containing the key=value for the given key or NULL if the entry is not found
 **/
char* audio_stream_get_param(audio_stream_handle_t stream_handle, const char* key);

/**
 * gets audio session or global parameters
 * @param session_handle - session handle
 * @param key            - key for which the value is sought
 *
 * @return string containing the key=value pair for the given key or NULL if the entry is not found
 **/
char* audio_session_get_param(audio_session_handle_t handle, const char* key);

/**
 * Callback to receive event data from Wrapper
 * @param session_handle- session handle
 * @param priv_adev     - Private data that client passes while registering this call. This can be used to access any client specific
                          data from the call back. For Ex: HAL can check if there are new devices connected etc.
 * @param data          - pointer to the data buffer
 * @event_id            - Event id for which data buffer is received
 * @param size          - size of the buffer
 * @param device        - Output device for which the output buffer is passed. If multiple device are active, wrapper will invoke this
 *                        method for each device
 **/
typedef void (*notify_event_callback_t)(audio_session_handle_t session_handle, void* priv_data, void* data, audio_event_id_t event_id, int size, int device);

/**
 * Registers the callback to receive event data from Wrapper library. Client needs to register this call back before starting any stream
 * @param session_handle  - session handle
 * @param priv_data       - Private data that client expected Wrapper to pass from event callback
 * @param event_callback  - Callback to receive event data from Wrapper
 *
 **/
void register_event_callback(audio_session_handle_t session_handle, void *priv_data, notify_event_callback_t event_callback, audio_event_id_t event_id);

/**
 * Starts the audio stream with stream_handle.
 * @param handle     - stream handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_start(audio_stream_handle_t handle);

/**
 * Stops the audio stream with stream_handle.
 *
 * @param stream_handle     - stream handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_stop(audio_stream_handle_t stream_handle);

/**
 * Pauses the audio stream with the given stream_handle. This method will pause the processing of this stream within the Wrapper.
 *
 * @param stream_handle     - stream handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_pause(audio_stream_handle_t stream_handle);

/**
 * Flushes the audio stream with the given stream_handle
 *
 * @param stream_handle     - stream handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_flush(audio_stream_handle_t stream_handle);

/**
 * Passes input data for the given stream. Returns number of bytes written or a negative value in cases of failure. The client writes buffers in the wrapper
 * and the input node in the wrapper is expected to read the data from the allocated buffer.
 *
 * @param stream_handle - stream handle
 * @param buf           - input buffer payload
 *
 * @return bytes written when SUCCESS. -1 when in failure case
 **/
int audio_stream_write_v2(audio_stream_handle_t stream_handle, audio_qaf_in_buffer_t *buf);

/** TODO: Should be deprecated.
 * Passes input data for the given stream. Returns number of bytes written or a negative value in cases of failure.
 * The client writes buffers in the wrapper and the input node in the wrapper is expected to read the data from the allocated buffer.
 *
 * @param stream_handle - stream handle
 * @param buf           - pointer to the data buffer
 * @param size          - size of the data buffer
 *
 * @return bytes written when SUCCESS. -1 when in failure case
 **/
int audio_stream_write(audio_stream_handle_t stream_handle, void* buf, int size);

/**
 * Closes the audio stream for the given stream_handle
 *
 * @param stream_handle - stream handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_stream_close(audio_stream_handle_t stream_handle);

/**
 * Closes the audio session for the given session_handle
 *
 * @param session_handle - session handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_session_close(audio_session_handle_t session_handle);

/**
 * Callback to raise for security check
 * @param session_handle- session handle
 * @param payload       - pointer to the payload buffer
 * @param cookie        - private data to send back
 **/
typedef int (*audio_sec_cb_t)(void *cookie, void *payload);

/**
 * Open the audio security module for the given
 * session_handle
 *
 * @param session_handle - session handle
 * @param cb             - callback for security check
 * @param shm_buf        - buffer address
 * @param priv_data      - private data to be passed in callback
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_security_open(audio_session_handle_t session_handle,
                        audio_sec_cb_t cb, void *shm_buf, void *priv_data);

/**
 * Closes the audio security module for the given session_handle
 *
 * @param session_handle - session handle
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_security_close(audio_session_handle_t session_handle);

/**
 * Security check acknowledgement for the given session_handle
 *
 * @param session_handle - session handle
 * @param info - ack information
 *
 * @return 0 on SUCCESS and negative values on failure
 **/
int audio_security_check_ack(audio_session_handle_t session_handle,
                             void *info);

#endif // QTI_AUDIO_H
