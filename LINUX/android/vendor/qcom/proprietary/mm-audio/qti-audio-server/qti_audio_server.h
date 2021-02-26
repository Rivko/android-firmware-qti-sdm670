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
#include <binder/MemoryDealer.h>
#include <cutils/list.h>

#include <pthread.h>

#include "qti_audio_server_interface.h"
#include "qti_audio_server_callback.h"

#define QTI_AUDIO_SERVER "qti_audio_server"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef memscpy
#define memscpy(dst, dst_size, src, bytes_to_copy) (void) memcpy(dst, src, MIN(dst_size, bytes_to_copy))
#endif

namespace audiohal {

using namespace android;

typedef struct {
    sp<IStreamOutCallback> cb_binder;
    void * cookie1;
    void * cookie2;
} callback_data;

typedef struct {
    qahw_stream_handle_t *qahw_stream;
    sp<IMemory> sh_mem_handle;
    sh_mem_data *shmem_data;
    void *shmem_data_buff;
    pthread_t thread;
    callback_data cb;
} n_stream_handle;

typedef struct {
    struct listnode list;
    bool prepare_done;
    qahw_stream_handle_t *n_stream_handle;
    sp<MemoryDealer> sh_mem_dealer;
    sp<IMemory> sh_mem_handle;
    sh_mem_data *shmem_data;
    pthread_mutex_t shmem_lock;
    void *shmem_data_buff;
} p_stream_handle;

class qti_audio_server : public ::android::BnInterface<Iqti_audio_server> {
public:
    qti_audio_server(){ALOGD("qti_audio_server::qti_audio_server()");};
    ~qti_audio_server(){ALOGD("~qti_audio_server::qti_audio_server()");};
private:
    int32_t onTransact(uint32_t code, const ::android::Parcel& data,
                 ::android::Parcel* reply, uint32_t flags = 0) override;
    void* qahw_load_module(const char *hw_module_id) override;
    int qahw_open_output_stream(qahw_module_handle_t *hw_module,
                                audio_io_handle_t handle,
                                audio_devices_t devices,
                                audio_output_flags_t flags,
                                struct audio_config *config,
                                qahw_stream_handle_t **out_handle,
                                const char *address) override;
    int qahw_open_input_stream(qahw_module_handle_t *hw_module,
                               audio_io_handle_t handle,
                               audio_devices_t devices,
                               struct audio_config *config,
                               qahw_stream_handle_t **stream_in,
                               audio_input_flags_t flags,
                               const char *address,
                               audio_source_t source) override;
    ssize_t qahw_out_write(qahw_stream_handle_t *stream,
                           qahw_out_buffer_t *out_buf) override;
    ssize_t qahw_in_read(qahw_stream_handle_t *in_handle,
                         qahw_in_buffer_t *in_buf) override;
    int prepare_for_write(qahw_stream_handle_t *stream,
                         qahw_out_buffer_t *buff = 0) override;
    int qahw_out_set_callback(qahw_stream_handle_t *out_handle,
                                  qahw_stream_callback_t callback,
                                  void *cookie) override;
    int qahw_close_output_stream(qahw_stream_handle_t *out_handle) override;
    int qahw_out_standby(qahw_stream_handle_t *out_handle) override;
    int qahw_unload_module(qahw_module_handle_t *hw_module) override;
    int qahw_out_drain(qahw_stream_handle_t *out_handle, qahw_drain_type_t
        type ) override;
    uint32_t qahw_out_get_sample_rate
        (const qahw_stream_handle_t *out_handle) override;
    int qahw_out_set_sample_rate
        (qahw_stream_handle_t *out_handle, uint32_t rate) override;
    size_t qahw_out_get_buffer_size
        (const qahw_stream_handle_t *out_handle) override;
    int qahw_out_set_volume
        (qahw_stream_handle_t *out_handle, float left, float right) override;
    uint32_t qahw_out_get_latency
        (const qahw_stream_handle_t *out_handle) override;
    audio_channel_mask_t qahw_out_get_channels
        (const qahw_stream_handle_t *out_handle) override;
    audio_format_t qahw_out_get_format
        (const qahw_stream_handle_t *out_handle) override;
    int qahw_out_set_parameters(qahw_stream_handle_t *out_handle,
        const char *kv_pairs) override;
    char *qahw_out_get_parameters(const qahw_stream_handle_t *out_handle,
        const char *keys) override;
    int qahw_out_pause(qahw_stream_handle_t *out_handle) override;
    int qahw_out_resume(qahw_stream_handle_t *out_handle) override;
    int qahw_out_flush(qahw_stream_handle_t *out_handle) override;
    int qahw_out_get_presentation_position(
                            const qahw_stream_handle_t *out_handle,
                            uint64_t *frames,
                            struct timespec *timestamp) override;
    int qahw_out_set_param_data(qahw_stream_handle_t *out_handle,
                            qahw_param_id param_id,
                            qahw_param_payload *payload) override;
    int qahw_out_get_param_data(qahw_stream_handle_t *out_handle,
                            qahw_param_id param_id,
                            qahw_param_payload *payload) override;
    int qahw_close_input_stream(qahw_stream_handle_t *in_handle) override;
    int prepare_for_read(qahw_stream_handle_t *stream,
                         qahw_in_buffer_t *buff = 0) override;
    int qahw_set_parameters(qahw_module_handle_t *hw_module,
                            const char *kv_pairs) override;
    int qahw_in_set_parameters(qahw_stream_handle_t *in_handle,
                               const char *kv_pairs) override;
    int qahw_in_standby(qahw_stream_handle_t *stream) override;
    size_t qahw_in_get_buffer_size
                        (const qahw_stream_handle_t *in_handle) override;
    int qahw_get_param_data(const qahw_module_handle_t *hw_module,
                            qahw_param_id param_id,
                            qahw_param_payload *payload) override;
    int qahw_set_param_data(const qahw_module_handle_t *hw_module,
                            qahw_param_id param_id,
                            qahw_param_payload *payload) override;
    int qahw_out_get_render_position(const qahw_stream_handle_t *out_handle,
                                        uint32_t *dsp_frames) override;
    int qahw_in_set_sample_rate(qahw_stream_handle_t *in_handle, uint32_t rate) override;
    uint32_t qahw_in_get_sample_rate(const qahw_stream_handle_t *in_handle) override;
    audio_channel_mask_t qahw_in_get_channels(const qahw_stream_handle_t *in_handle) override;
    audio_format_t qahw_in_get_format(const qahw_stream_handle_t *in_handle) override;
    int qahw_in_set_format(qahw_stream_handle_t *in_handle, audio_format_t format) override;
    char* qahw_in_get_parameters(const qahw_stream_handle_t *in_handle,
                                  const char *keys) override;
    uint32_t qahw_in_get_input_frames_lost(qahw_stream_handle_t *in_handle) override;
    int qahw_in_get_capture_position(const qahw_stream_handle_t *in_handle,
                                     int64_t *frames, int64_t *time) override;
    int qahw_init_check(const qahw_module_handle_t *hw_module) override;
    int qahw_set_voice_volume(qahw_module_handle_t *hw_module, float volume) override;
    int qahw_set_mode(qahw_module_handle_t *hw_module, audio_mode_t mode) override;
    int qahw_set_mic_mute(qahw_module_handle_t *hw_module, bool state) override;
    int qahw_get_mic_mute(qahw_module_handle_t *hw_module, bool *state) override;
    char* qahw_get_parameters(const qahw_module_handle_t *hw_module,
                           const char *keys) override;
    size_t qahw_get_input_buffer_size(const qahw_module_handle_t *hw_module,
                                      const struct audio_config *config) override;
    int qahw_create_audio_patch(qahw_module_handle_t *hw_module,
                            unsigned int num_sources,
                            const struct audio_port_config *sources,
                            unsigned int num_sinks,
                            const struct audio_port_config *sinks,
                            audio_patch_handle_t *handle) override;
    int qahw_release_audio_patch(qahw_module_handle_t *hw_module,
                            audio_patch_handle_t handle) override;
    int qahw_get_audio_port(qahw_module_handle_t *hw_module,
                          struct audio_port *port) override;
    int qahw_set_audio_port_config(qahw_module_handle_t *hw_module,
                         const struct audio_port_config *config) override;
    int qahw_get_version() override;
    qahw_effect_lib_handle_t qahw_effect_load_library(const char *lib_name) override;
    int32_t qahw_effect_unload_library(qahw_effect_lib_handle_t handle);
    int32_t qahw_effect_create(qahw_effect_lib_handle_t handle,
                               const qahw_effect_uuid_t *uuid,
                               int32_t io_handle,
                               qahw_effect_handle_t *effect_handle) override;
    int32_t qahw_effect_release(qahw_effect_lib_handle_t handle,
                                qahw_effect_handle_t effect_handle) override;
    int32_t qahw_effect_get_descriptor(qahw_effect_lib_handle_t handle,
                                       const qahw_effect_uuid_t *uuid,
                                       qahw_effect_descriptor_t *effect_desc) override;
    int32_t qahw_effect_get_version() override;
    int32_t qahw_effect_process(qahw_effect_handle_t self,
                                qahw_audio_buffer_t *in_buffer,
                                qahw_audio_buffer_t *out_buffer) override;
    int32_t qahw_effect_command(qahw_effect_handle_t self,
                                uint32_t cmd_code,
                                uint32_t cmd_size,
                                void *cmd_data,
                                uint32_t *reply_size,
                                void *reply_data) override;
    int32_t qahw_effect_process_reverse(qahw_effect_handle_t self,
                                        qahw_audio_buffer_t *in_buffer,
                                        qahw_audio_buffer_t *out_buffer) override;
};
}; // namespace audiohal

