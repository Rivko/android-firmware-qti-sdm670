/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#pragma once

#include <stdlib.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>

#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <binder/IPCThreadState.h>

#include "qahw.h"
#include "qahw_effect_api.h"

namespace audiohal {

using namespace android;

typedef enum{
    LOAD_HAL = ::android::IBinder::FIRST_CALL_TRANSACTION,
    UNLOAD_HAL,
    SET_PARAMS,
    GET_PARAMS,
    SET_PARAM_DATA,
    GET_PARAM_DATA,
    OPEN_OUTPUT_STREAM,
    OPEN_INPUT_STREAM,
    WRITE,
    READ,
    STOP,
    CLOSE_OUTPUT_STREAM,
    CLOSE_INPUT_STREAM,
    GET_PRESENTATION_TIME,
    GET_LATENCY,
    PREPARE_FOR_WRITE,
    PREPARE_FOR_READ,
    OUT_GET_SAMPLE_RATE,
    OUT_SET_SAMPLE_RATE,
    OUT_GET_BUFF_SIZE,
    OUT_GET_CHANNELS,
    OUT_GET_FORMAT,
    OUT_STANDBY,
    OUT_SET_PARAMS,
    OUT_GET_PARAMS,
    OUT_SET_PARAM_DATA,
    OUT_GET_PARAM_DATA,
    OUT_SET_VOLUME,
    OUT_GET_RENDER_POS,
    OUT_SET_CALLBACK,
    OUT_PAUSE,
    OUT_RESUME,
    OUT_DRAIN,
    OUT_FLUSH,
    IN_GET_SAMPLE_RATE,
    IN_SET_SAMPLE_RATE,
    IN_GET_BUFF_SIZE,
    IN_GET_CHANNELS,
    IN_GET_FORMAT,
    IN_SET_FORMAT,
    IN_STANDBY,
    IN_SET_PARAMS,
    IN_GET_PARAMS,
    GET_INPUT_FRAMES_LOST,
    GET_CAPTURE_POS,
    INIT_CHECK,
    SET_VOICE_VOL,
    SET_MODE,
    SET_MIC_MUTE,
    GET_MIC_MUTE,
    INPUT_BUFF_SIZE,
    CREATE_AUD_PATCH,
    RELEASE_AUD_PATCH,
    GET_AUD_PORT,
    SET_AUD_PORT_CFG,
    GET_VERSION,
    EFFECT_LOAD_LIBRARY,
    EFFECT_UNLOAD_LIBRARY,
    EFFECT_CREATE,
    EFFECT_RELEASE,
    EFFECT_GET_DESCRIPTOR,
    EFFECT_GET_VERSION,
    EFFECT_COMMAND
}stream_command;

union buf_ptr {
    qahw_out_buffer_t out_buf;
    qahw_in_buffer_t in_buf;
};

typedef struct {
    pthread_cond_t  c_cond;
    pthread_mutex_t c_mutex;
    stream_command cmd;
    int status;
    union buf_ptr buf;
} sh_mem_data;

typedef struct {
   uint64_t frames;
   struct timespec timestamp;
   int ret_val;
} presentation_time_struct;


class Iqti_audio_server: public ::android::IInterface {
    public:
        DECLARE_META_INTERFACE(qti_audio_server);
        virtual void* qahw_load_module(const char *hw_module_id) = 0;
        virtual int qahw_open_output_stream(qahw_module_handle_t *hw_module,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   qahw_stream_handle_t **out_handle,
                                   const char *address) = 0;
        virtual int qahw_open_input_stream(qahw_module_handle_t *hw_module,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   struct audio_config *config,
                                   qahw_stream_handle_t **stream_in,
                                   audio_input_flags_t flags,
                                   const char *address,
                                   audio_source_t source) = 0;
        virtual ssize_t qahw_out_write(qahw_stream_handle_t *stream,
                            qahw_out_buffer_t *out_buf) = 0;
        virtual ssize_t qahw_in_read(qahw_stream_handle_t *in_handle,
                            qahw_in_buffer_t *in_buf) = 0;
        virtual int prepare_for_write(qahw_stream_handle_t *stream,
                            qahw_out_buffer_t * buff) = 0;
        virtual int prepare_for_read(qahw_stream_handle_t *stream,
                            qahw_in_buffer_t * buff) = 0;
        virtual int qahw_close_output_stream
            (qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_close_input_stream
            (qahw_stream_handle_t *in_handle) = 0;
        virtual int qahw_out_standby(qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_in_standby(qahw_stream_handle_t *stream) = 0;
        virtual int qahw_out_set_callback(qahw_stream_handle_t *out_handle,
                                  qahw_stream_callback_t callback,
                                  void *cookie) = 0;
        virtual int qahw_unload_module(qahw_module_handle_t *hw_module) = 0;
        virtual int qahw_out_drain(qahw_stream_handle_t *out_handle,
            qahw_drain_type_t type ) = 0;
        virtual uint32_t qahw_out_get_sample_rate(const qahw_stream_handle_t
            *out_handle) = 0;
        virtual int qahw_out_set_sample_rate(qahw_stream_handle_t *out_handle,
            uint32_t rate) = 0;
        virtual size_t qahw_out_get_buffer_size(const
            qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_out_set_volume(qahw_stream_handle_t *out_handle,
            float left, float right) = 0;
        virtual uint32_t qahw_out_get_latency(const qahw_stream_handle_t
            *out_handle) = 0;
        virtual audio_channel_mask_t qahw_out_get_channels
            (const qahw_stream_handle_t *out_handle) = 0;
        virtual audio_format_t qahw_out_get_format
            (const qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_set_parameters(qahw_module_handle_t *hw_module,
                                        const char *kv_pairs) = 0;
        virtual int qahw_out_set_parameters(qahw_stream_handle_t *out_handle,
            const char *kv_pairs) = 0;
        virtual char *qahw_out_get_parameters(const qahw_stream_handle_t *out_handle,
                                           const char *keys) = 0;
        virtual int qahw_out_pause(qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_out_resume(qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_out_flush(qahw_stream_handle_t *out_handle) = 0;
        virtual int qahw_out_get_presentation_position(
                                   const qahw_stream_handle_t *out_handle,
                                   uint64_t *frames,
                                   struct timespec *timestamp) = 0;
        virtual int qahw_out_set_param_data(qahw_stream_handle_t *out_handle,
                                qahw_param_id param_id,
                                qahw_param_payload *payload) = 0;
        virtual int qahw_out_get_param_data(qahw_stream_handle_t *out_handle,
                                qahw_param_id param_id,
                                qahw_param_payload *payload) = 0;
        virtual int qahw_in_set_parameters(qahw_stream_handle_t *in_handle,
                                const char *kv_pairs) = 0;
       virtual size_t qahw_in_get_buffer_size
                      (const qahw_stream_handle_t *in_handle) = 0;
       virtual int qahw_get_param_data(const qahw_module_handle_t *hw_module,
                               qahw_param_id param_id,
                               qahw_param_payload *payload) = 0;
       virtual int qahw_set_param_data(const qahw_module_handle_t *hw_module,
                               qahw_param_id param_id,
                               qahw_param_payload *payload) = 0;
        virtual int qahw_out_get_render_position(const qahw_stream_handle_t *out_handle,
                                                uint32_t *dsp_frames) = 0;
        virtual int qahw_in_set_sample_rate(qahw_stream_handle_t *in_handle, uint32_t rate) = 0;
        virtual uint32_t qahw_in_get_sample_rate(const qahw_stream_handle_t *in_handle) = 0;
        virtual audio_channel_mask_t qahw_in_get_channels(const qahw_stream_handle_t *in_handle) = 0;
        virtual audio_format_t qahw_in_get_format(const qahw_stream_handle_t *in_handle) = 0;
        virtual int qahw_in_set_format(qahw_stream_handle_t *in_handle, audio_format_t format) = 0;
        virtual char* qahw_in_get_parameters(const qahw_stream_handle_t *in_handle,
                                          const char *keys) = 0;
        virtual uint32_t qahw_in_get_input_frames_lost(qahw_stream_handle_t *in_handle) = 0;
        virtual int qahw_in_get_capture_position(const qahw_stream_handle_t *in_handle,
                                         int64_t *frames, int64_t *time) = 0;
        virtual int qahw_init_check(const qahw_module_handle_t *hw_module) = 0;
        virtual int qahw_set_voice_volume(qahw_module_handle_t *hw_module, float volume) = 0;
        virtual int qahw_set_mode(qahw_module_handle_t *hw_module, audio_mode_t mode) = 0;
        virtual int qahw_set_mic_mute(qahw_module_handle_t *hw_module, bool state) = 0;
        virtual int qahw_get_mic_mute(qahw_module_handle_t *hw_module, bool *state) = 0;
        virtual char* qahw_get_parameters(const qahw_module_handle_t *hw_module,
                               const char *keys) = 0;
        virtual size_t qahw_get_input_buffer_size(const qahw_module_handle_t *hw_module,
                                          const struct audio_config *config) = 0;
        virtual int qahw_create_audio_patch(qahw_module_handle_t *hw_module,
                                unsigned int num_sources,
                                const struct audio_port_config *sources,
                                unsigned int num_sinks,
                                const struct audio_port_config *sinks,
                                audio_patch_handle_t *handle) = 0;
        virtual int qahw_release_audio_patch(qahw_module_handle_t *hw_module,
                                audio_patch_handle_t handle) = 0;
        virtual int qahw_get_audio_port(qahw_module_handle_t *hw_module,
                              struct audio_port *port) = 0;
        virtual int qahw_set_audio_port_config(qahw_module_handle_t *hw_module,
                             const struct audio_port_config *config) = 0;
        virtual int qahw_get_version() = 0;
        /* Audio effects API */
        virtual qahw_effect_lib_handle_t qahw_effect_load_library(const char *lib_name) = 0;
        virtual int32_t qahw_effect_unload_library(qahw_effect_lib_handle_t handle) = 0;
        virtual int32_t qahw_effect_create(qahw_effect_lib_handle_t handle,
                                     const qahw_effect_uuid_t *uuid,
                                     int32_t io_handle,
                                     qahw_effect_handle_t *effect_handle) = 0;
        virtual int32_t qahw_effect_release(qahw_effect_lib_handle_t handle,
                                      qahw_effect_handle_t effect_handle) = 0;
        virtual int32_t qahw_effect_get_descriptor(qahw_effect_lib_handle_t handle,
                                             const qahw_effect_uuid_t *uuid,
                                             qahw_effect_descriptor_t *effect_desc) = 0;
        virtual int32_t qahw_effect_get_version() = 0;
        virtual int32_t qahw_effect_process(qahw_effect_handle_t self,
                                      qahw_audio_buffer_t *in_buffer,
                                      qahw_audio_buffer_t *out_buffer) = 0;
        virtual int32_t qahw_effect_command(qahw_effect_handle_t self,
                                      uint32_t cmd_code,
                                      uint32_t cmd_size,
                                      void *cmd_data,
                                      uint32_t *reply_size,
                                      void *reply_data) = 0;
        virtual int32_t qahw_effect_process_reverse(qahw_effect_handle_t self,
                                              qahw_audio_buffer_t *in_buffer,
                                              qahw_audio_buffer_t *out_buffer) = 0;
};
}; // namespace audiohal

