/* Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "qti_audio_server"

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

#include <sys/prctl.h>
#include <system/thread_defs.h>
#include <sys/resource.h>

#include "qti_audio_server.h"
extern "C"
{
#include <mm-audio/qahw/inc/qahw.h>
}

namespace audiohal {

using namespace android;

static int stream_out_event_cb(qahw_stream_callback_event_t event,
                                   void *param,
                                   void *cookie) {
    n_stream_handle *n_stream = (n_stream_handle *)cookie;
    sp<IStreamOutCallback> cb_binder = n_stream->cb.cb_binder;
    if (cb_binder == NULL) {
        ALOGE("%s Invalid binder handle", __func__);
        return -EINVAL;
    }
    ALOGD("%s event(%d) for stream (%p)",
        __func__, event, cookie);

    return cb_binder->event_cb(event,
                               param,
                               n_stream->cb.cookie1,
                               n_stream->cb.cookie2);
}

int32_t qti_audio_server::onTransact(uint32_t code,
                                   const Parcel& data,
                                   Parcel* reply,
                                   uint32_t flags) {
    ALOGV("qti_audio_server::onTransact(%i) %i", code, flags);
    data.checkInterface(this);
    int rc = -EINVAL;

    switch(code) {
        case LOAD_HAL: {
            const char *hal_id = data.readCString();
            ALOGD("%s %s LOAD_HAL",hal_id, __func__);
            qahw_module_handle_t *hal_handle = (qahw_module_handle_t *)qahw_load_module(hal_id);
            reply->write(&hal_handle, sizeof(qahw_module_handle_t *));
            ALOGV("hal handle (%p)", hal_handle);
        }
        break;

        case UNLOAD_HAL: {
            qahw_module_handle_t *handle = NULL;
            data.read(&handle, sizeof(qahw_module_handle_t *));
            ALOGD("%s UNLOAD_HAL(%p)", __func__, handle);
            return qahw_unload_module(handle);
        }
        break;

        case OPEN_OUTPUT_STREAM: {
            n_stream_handle *n_stream =
                (n_stream_handle *)calloc(1, sizeof(n_stream_handle));

            if (0 == n_stream) {
                ALOGE("failed: unable to allocate mem for stream data");
                reply->writeUint32((uint32_t)NULL);
                return -EINVAL;
                break;
            }
            ALOGD("OPEN_OUTPUT_STREAM: Allocated Server side stream mem %p bytes %zd",
                            n_stream, sizeof(n_stream_handle));

            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            audio_io_handle_t handle;
            data.read(&handle, sizeof(audio_io_handle_t));

            ALOGV("%s OPEN_OUTPUT_STREAM (%x)", __func__, handle);
            audio_devices_t devices;
            audio_output_flags_t flags;
            data.read(&devices, sizeof(audio_devices_t));
            data.read(&flags, sizeof(audio_output_flags_t));

            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            struct audio_config config;
            memset(&config, 0x0, sizeof(config));
            memscpy(&config, sizeof(struct audio_config), blob.data(), blob_size);
            blob.release();
            const char *address = data.readCString();
            ALOGV("OPEN_OUTPUT_STREAM:%d: devices %d flags %d address %s",
                            __LINE__, devices, flags, address);
            rc = qahw_open_output_stream(hw_module,
                                     handle,
                                     devices,
                                     flags,
                                     &config,
                                     &n_stream->qahw_stream,
                                     address);
            if (0 == n_stream->qahw_stream) {
                ALOGE("%s: Could not open output %d", __func__, rc);
                if (n_stream != NULL) {
                    n_stream->sh_mem_handle.clear();
                    free(n_stream);
                    return rc;
                }
            }
            // stream handle
            reply->write(&n_stream, sizeof(n_stream_handle *));
            ALOGV("OPEN_OUTPUT_STREAM: server side stream handle(%p) wrapping qahw hal handle(%p)",
                            n_stream, n_stream->qahw_stream);
            return rc;
        }
        break;

        case CLOSE_OUTPUT_STREAM: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            ALOGV("%s CLOSE_OUTPUT_STREAM (%p)", __func__, n_stream);
            if (n_stream != NULL) {
                rc = qahw_close_output_stream(n_stream->qahw_stream);
                n_stream->sh_mem_handle.clear();
                free(n_stream);
                ALOGD("CLOSE_OUTPUT_STREAM:%s: cleared shared mem reference \
                            and free malloced stuff", __func__);
            } else {
                ALOGE("%s Invalid n_stream handle", __func__);
            }
            return rc;
        }
        break;

        case OPEN_INPUT_STREAM: {
            n_stream_handle *n_stream =
                (n_stream_handle *)calloc(1, sizeof(n_stream_handle));

            if (0 == n_stream) {
                ALOGE("%s: failed: unable to allocate mem for stream data", __func__);
                reply->writeUint32((uint32_t)NULL);
                return -EINVAL;
            }
            ALOGV("OPEN_INPUT_STREAM: %s: Allocated Server side stream mem %p bytes %zd",
                        __func__, n_stream, sizeof(n_stream_handle));

            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            audio_io_handle_t handle;
            data.read(&handle, sizeof(audio_io_handle_t));

            ALOGV("%s OPEN_INPUT_STREAM (%x)", __func__, handle);
            audio_devices_t devices;
            data.read(&devices, sizeof(audio_devices_t));

            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            struct audio_config config;
            memset(&config, 0x0, sizeof(config));
            memscpy(&config, sizeof(struct audio_config), blob.data(), blob_size);
            blob.release();
            audio_input_flags_t flags;
            data.read(&flags, sizeof(audio_input_flags_t));
            const char *address = data.readCString();
            audio_source_t source;
            data.read(&source, sizeof(audio_source_t));

            ALOGV("OPEN_INPUT_STREAM: %d: devices %d flags %d address %s source %d",
                            __LINE__, devices, flags, address, source);

            rc = qahw_open_input_stream(hw_module,
                                   handle,
                                   devices,
                                   &config,
                                   &n_stream->qahw_stream,
                                   flags,
                                   address,
                                   source);
            if (0 == n_stream->qahw_stream) {
                ALOGE("%s: Could not open input %d", __func__, rc);
                 if (n_stream != NULL) {
                    n_stream->sh_mem_handle.clear();
                    free(n_stream);
                    return rc;
                }
            }
            // stream handle
            reply->write(&n_stream, sizeof(n_stream_handle *));
            ALOGV("OPEN_INPUT_STREAM: %s: server side stream handle(%p) wrapping qahw hal handle(%p)",
                        __func__, n_stream, n_stream->qahw_stream);
            return rc;
        }
        break;

        case CLOSE_INPUT_STREAM: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            ALOGV("%s CLOSE_INPUT_STREAM (%p)", __func__, n_stream);
            if (n_stream != NULL) {
                rc = qahw_close_input_stream(n_stream->qahw_stream);
                n_stream->sh_mem_handle.clear();
                free(n_stream);
                ALOGD("CLOSE_INPUT_STREAM:%s: cleared shared mem reference \
                                    and free malloced stuff", __func__);
            } else {
                ALOGE("%s Invalid n_stream handle", __func__);
            }
            return rc;
        }
        break;

        case PREPARE_FOR_WRITE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            ALOGV("%s PREPARE_FOR_WRITING (%p)", __func__, n_stream);
            n_stream->sh_mem_handle =
                    interface_cast<IMemory>(data.readStrongBinder());
            n_stream->shmem_data =
                    (sh_mem_data *)n_stream->sh_mem_handle->pointer();

            sh_mem_data *ptr = n_stream->shmem_data;
            n_stream->shmem_data_buff = (void*)++ptr;
            ALOGV("shmem_data buff header(%p) buff(%p)buff size %zd",
                        &n_stream->shmem_data->buf,
                        n_stream->shmem_data_buff,
                        sizeof(sh_mem_data));
            return prepare_for_write((qahw_stream_handle_t *)n_stream);
        }
        break;

        case PREPARE_FOR_READ:
        {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            ALOGV("%s PREPARE_FOR_READ (%p)", __func__, n_stream);
            n_stream->sh_mem_handle =
                    interface_cast<IMemory>(data.readStrongBinder());
            n_stream->shmem_data =
                    (sh_mem_data *)n_stream->sh_mem_handle->pointer();

            sh_mem_data *ptr = n_stream->shmem_data;
            n_stream->shmem_data_buff = (void*)++ptr;
            ALOGV("shmem_data buff header(%p) buff(%p)buff size %zd",
                        &n_stream->shmem_data->buf,
                        n_stream->shmem_data_buff,
                        sizeof(sh_mem_data));
            return prepare_for_read((qahw_stream_handle_t *)n_stream);
        }
        break;

        case SET_PARAMS: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            const char *kv_pairs = data.readCString();
            ALOGV("%s SET_PARAMS, %s", __func__, kv_pairs);
            return qahw_set_parameters(hw_module, kv_pairs);
        }
        break;

        case SET_PARAM_DATA: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            qahw_param_id param_id;
            qahw_param_payload param_payload;
            data.read(&param_id, sizeof(qahw_param_id));
            ALOGV("%s SET_PARAM_DATA, param_id %d", __func__, param_id);
            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(&param_payload, 0x0, sizeof(qahw_param_payload));
            memscpy(&param_payload, sizeof(qahw_param_payload), blob.data(), blob_size);
            blob.release();
            return qahw_set_param_data(hw_module, param_id, &param_payload);
        }
        break;

        case GET_PARAM_DATA: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            qahw_param_id param_id;
            qahw_param_payload param_payload;
            data.read(&param_id, sizeof(qahw_param_id));
            ALOGV("%s GET_PARAM_DATA, param_id %d", __func__, param_id);

            memset(&param_payload, 0x0, sizeof(qahw_param_payload));
            rc = qahw_get_param_data(hw_module, param_id, &param_payload);
            if (rc != 0) {
                ALOGE("error in GET_PARAM_DATA");
                return -EINVAL;
            }

            uint32_t param_size = sizeof(qahw_param_payload);
            reply->writeUint32(param_size);
            android::Parcel::WritableBlob blob;
            status = reply->writeBlob(param_size, false, &blob);
            if (status) {
                ALOGE("%s: Error in writeblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(blob.data(), 0x0, param_size);
            memscpy(blob.data(), param_size, &param_payload, param_size);
            blob.release();
            return rc;
        }
        break;

        case OUT_GET_SAMPLE_RATE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            uint32_t rate = qahw_out_get_sample_rate(handle);
            ALOGV("OUT_GET_SAMPLE_RATE: rate %d", rate);
            reply->writeInt32(rate);
        }
        break;

        case OUT_SET_SAMPLE_RATE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            uint32_t rate = data.readUint32();
            ALOGV("OUT_SET_SAMPLE_RATE: rate %d", rate);
            return qahw_out_set_sample_rate(handle, rate);
        }
        break;

        case OUT_GET_BUFF_SIZE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            size_t size = qahw_out_get_buffer_size(handle);
            ALOGV("OUT_GET_BUFF_SIZE: size %zu", size);
            reply->writeUint32((uint32_t)size);
        }
        break;

        case OUT_GET_CHANNELS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            audio_channel_mask_t ch = qahw_out_get_channels(handle);
            ALOGV("OUT_GET_CHANNELS: ch %d", ch);
            reply->writeUint32((uint32_t)ch);
        }
        break;

        case OUT_GET_FORMAT: {
            ALOGV("%s OUT_GET_FORMAT", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            audio_format_t format = qahw_out_get_format(handle);
            reply->writeUint32((uint32_t)format);
        }
        break;

        case OUT_SET_PARAMS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            const char *kv_pairs = data.readCString();
            ALOGV("%s OUT_SET_PARAMS, %s", __func__, kv_pairs);
            return qahw_out_set_parameters(handle, kv_pairs);
        }
        break;

        case OUT_GET_PARAMS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            char *keys = (char *)data.readCString();
            char *value = qahw_out_get_parameters(handle, keys);
            ALOGV("%s OUT_GET_PARAMS, %s", __func__, keys);
            reply->writeCString(value);
        }
        break;

        case OUT_SET_PARAM_DATA: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            qahw_param_id param_id;
            data.read(&param_id, sizeof(qahw_param_id));

            ALOGV("%s OUT_SET_PARAM_DATA, param_id %d", __func__, param_id);
            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            qahw_param_payload payload;
            memset(&payload, 0x0, sizeof(qahw_param_payload));
            memscpy(&payload, sizeof(qahw_param_payload), blob.data(), blob_size);
            blob.release();
            return qahw_out_set_param_data(handle, param_id, &payload);
        }
        break;

        case OUT_GET_PARAM_DATA: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            qahw_param_id param_id;
            qahw_param_payload payload;
            data.read(&param_id, sizeof(qahw_param_id));
            ALOGV("%s OUT_GET_PARAM_DATA, param_id %d", __func__, param_id);

            memset(&payload, 0x0, sizeof(qahw_param_payload));

            rc = qahw_out_get_param_data(handle, param_id, &payload);
            if (rc != 0) {
                ALOGE("error in OUT_SET_PARAM_DATA");
                return -EINVAL;
            }
            uint32_t param_size = sizeof(qahw_param_payload);
            reply->writeUint32(param_size);
            android::Parcel::WritableBlob blob;
            status_t status = reply->writeBlob(param_size, false, &blob);
            if (status) {
                ALOGE("%s: Error in writeblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(blob.data(), 0x0, param_size);
            memscpy(blob.data(), param_size, &payload, sizeof(qahw_param_payload));
            blob.release();
            return rc;
        }
        break;

        case OUT_SET_VOLUME: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            float left = data.readFloat();
            float right = data.readFloat();
            ALOGV("%d: left %f right %f", __LINE__, left, right);
            return qahw_out_set_volume(handle, left, right);
        }
        break;

        case OUT_STANDBY: {
            ALOGV("%s OUT_STANDBY", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            return qahw_out_standby(handle);
        }
        break;

        case OUT_GET_RENDER_POS: {
            ALOGV("%s OUT_GET_RENDER_POS", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            uint32_t dsp_frames = 0;
            rc = qahw_out_get_render_position(handle, &dsp_frames);
            reply->writeUint32(dsp_frames);
            return rc;
        }
        break;

        case OUT_SET_CALLBACK: {
            ALOGV("%s OUT_SET_CALLBACK", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            sp<IBinder> binder = data.readStrongBinder();
            n_stream->cb.cb_binder = interface_cast<IStreamOutCallback>(binder);
            data.read(&n_stream->cb.cookie1, sizeof(qahw_stream_callback_t *));
            data.read(&n_stream->cb.cookie2, sizeof(qahw_stream_callback_t *));
            return qahw_out_set_callback(n_stream->qahw_stream,
                            stream_out_event_cb, n_stream);
        }
        break;

        case OUT_PAUSE: {
            ALOGV("%s OUT_PAUSE", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            return qahw_out_pause(n_stream->qahw_stream);
        }
        break;

        case OUT_RESUME: {
            ALOGV("%s OUT_RESUME", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            return qahw_out_resume(n_stream->qahw_stream);
        }
        break;

        case OUT_DRAIN: {
            ALOGV("%s OUT_DRAIN", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_drain_type_t d_type;
            data.read(&d_type, sizeof(qahw_drain_type_t));
            return qahw_out_drain(n_stream->qahw_stream, d_type);
        }
        break;

        case OUT_FLUSH: {
            ALOGV("%s OUT_FLUSH", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            return qahw_out_flush(n_stream->qahw_stream);
        }
        break;

        case IN_GET_SAMPLE_RATE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            size_t sr = qahw_in_get_sample_rate(handle);
            ALOGV("IN_GET_SAMPLE_RATE: rate %zu", sr);
            reply->writeUint32((uint32_t)sr);
        }
        break;

        case IN_SET_SAMPLE_RATE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            int rate = data.readUint32();
            ALOGV("IN_SET_SAMPLE_RATE: rate %d", rate);
            return qahw_in_set_sample_rate(handle, rate);
        }
        break;

        case IN_GET_BUFF_SIZE: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            size_t size = qahw_in_get_buffer_size(handle);
            ALOGV("IN_GET_BUFF_SIZE: size %zu", size);
            reply->writeUint32((uint32_t)size);
        }
        break;

        case IN_GET_CHANNELS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            audio_channel_mask_t ch = qahw_in_get_channels(handle);
            ALOGV("IN_GET_CHANNELS: ch %d", ch);
            reply->writeUint32((uint32_t)ch);
        }
        break;

        case IN_GET_FORMAT: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            audio_format_t format = qahw_in_get_format(handle);
            ALOGV("IN_GET_FORMAT: format %d", format);
            reply->writeUint32((uint32_t)format);
        }
        break;

        case IN_SET_FORMAT: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            audio_format_t format;
            data.read(&format, sizeof(audio_format_t));
            ALOGV("IN_SET_FORMAT: format %d", format);
            return qahw_in_set_format(handle, format);
        }
        break;

        case IN_STANDBY: {
            ALOGV("%s IN_STANDBY", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            return qahw_in_standby(handle);
        }
        break;

        case IN_SET_PARAMS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            const char *kv_pairs = data.readCString();
            ALOGV("%s IN_SET_PARAMS, %s", __func__, kv_pairs);
            return qahw_in_set_parameters(handle, kv_pairs);
        }
        break;

        case IN_GET_PARAMS: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            char *keys = (char *)data.readCString();
            char *value = qahw_in_get_parameters(handle, keys);
            ALOGV("%s IN_GET_PARAMS, %s", __func__, keys);
            reply->writeCString(value);
        }
        break;

        case GET_INPUT_FRAMES_LOST: {
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *handle = n_stream->qahw_stream;
            uint32_t cnt = qahw_in_get_input_frames_lost(handle);
            ALOGV("GET_INPUT_FRAMES_LOST: cnt %d", cnt);
            reply->writeUint32(cnt);
        }
        break;

        case GET_CAPTURE_POS: {
            ALOGV("%s GET_CAPTURE_POS", __func__);
            n_stream_handle *n_stream = NULL;
            data.read(&n_stream, sizeof(n_stream_handle *));
            qahw_stream_handle_t *in_handle = n_stream->qahw_stream;
            int64_t *frames = (int64_t *)data.readInt64();
            int64_t *time = (int64_t *)data.readInt64();
            return qahw_in_get_capture_position(in_handle, frames, time);
        }
        break;

        case INIT_CHECK: {
            ALOGV("%s INIT_CHECK", __func__);
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            return qahw_init_check(hw_module);
        }
        break;

        case SET_VOICE_VOL: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            float vol = data.readFloat();
            ALOGV("%s SET_VOICE_VOL %f", __func__, vol);
            return qahw_set_voice_volume(hw_module, vol);
        }
        break;

        case SET_MODE: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            audio_mode_t mode;
            data.read(&mode, sizeof(audio_mode_t));
            ALOGV("%s SET_MODE %d", __func__, mode);
            return qahw_set_mode(hw_module, mode);
        }
        break;

        case GET_PARAMS: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            char *keys = (char *)data.readCString();
            char *value = qahw_get_parameters(hw_module, keys);
            ALOGV("%s GET_PARAMS, %s", __func__, keys);
            reply->writeCString(value);
        }
        break;

        case SET_MIC_MUTE: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            int state = data.readUint32();
            ALOGV("%s SET_MIC_MUTE %d", __func__, state);
            return qahw_set_mic_mute(hw_module, state);
        }
        break;

        case GET_MIC_MUTE: {
            ALOGV("%s GET_MIC_MUTE", __func__);
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            bool state = false;
            rc = qahw_get_mic_mute(hw_module, &state);
            reply->writeUint32(state);
        }
        break;

        case INPUT_BUFF_SIZE: {
            ALOGV("%s INPUT_BUFF_SIZE", __func__);
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            audio_config config;
            memset(&config, 0x0, sizeof(audio_config));
            rc = qahw_get_input_buffer_size(hw_module, &config);
             if (rc != 0) {
                ALOGE("error in INPUT_BUFF_SIZE");
                return -EINVAL;
            }

            uint32_t param_size = sizeof(audio_config);
            reply->writeUint32(param_size);
            android::Parcel::WritableBlob blob;
            status_t status = reply->writeBlob(param_size, false, &blob);
            if (status) {
                ALOGE("%s: Error in writeblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(blob.data(), 0x0, param_size);
            memscpy(blob.data(), param_size, &config, param_size);
            blob.release();
            return rc;
        }
        break;

        case CREATE_AUD_PATCH: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            unsigned int num_sources = data.readUint32();
            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            audio_port_config sources;
            memset((void *)&sources, 0x0, sizeof(audio_port_config));
            memscpy((void *) &sources, sizeof(audio_port_config), blob.data(), blob_size);

            unsigned int num_sinks = data.readUint32();
            data.readUint32(&blob_size);
            status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            struct audio_port_config sinks;
            memset((void *) &sinks, 0x0, sizeof(audio_port_config));
            memscpy((void *) &sinks, sizeof(audio_port_config), blob.data(), blob_size);
            blob.release();
            audio_patch_handle_t *handle = NULL;
            data.read(&handle, sizeof(audio_patch_handle_t));
            return qahw_create_audio_patch(hw_module, num_sources,
                                         &sources, num_sinks, &sinks,
                                         handle);
        }
        break;

        case RELEASE_AUD_PATCH: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            audio_patch_handle_t handle;
            data.read(&handle, sizeof(audio_patch_handle_t));
            return qahw_release_audio_patch(hw_module, handle);
        }
        break;

        case GET_AUD_PORT: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));
            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            audio_port port;
            memset(&port, 0x0, sizeof(audio_port));
            memscpy(&port, sizeof(audio_port), blob.data(), blob_size);
            blob.release();
            rc = qahw_get_audio_port(hw_module, &port);
            if (rc != 0) {
                ALOGE("error in GET_AUD_PORT");
                return -EINVAL;
            }
            uint32_t size = sizeof(audio_port);
            reply->writeUint32(size);
            android::Parcel::WritableBlob write_blob;
            status = reply->writeBlob(size, false, &write_blob);
            if (status) {
                ALOGE("%s: Error in writeblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(write_blob.data(), 0x0, size);
            memscpy(write_blob.data(), size, &port, size);
            write_blob.release();
            return rc;
        }
        break;

        case SET_AUD_PORT_CFG: {
            qahw_module_handle_t *hw_module = NULL;
            data.read(&hw_module, sizeof(qahw_module_handle_t *));

            audio_port_config cfg;

            android::Parcel::ReadableBlob blob;
            uint32_t blob_size;
            data.readUint32(&blob_size);
            status_t status = data.readBlob(blob_size, &blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            memset(&cfg, 0x0, sizeof(audio_port_config));
            memscpy((void *) &cfg, sizeof(audio_port_config), blob.data(), blob_size);
            blob.release();
            return qahw_set_audio_port_config(hw_module, &cfg);
        }
        break;

        case GET_VERSION: {
            ALOGV("%s GET_VERSION", __func__);
            return qahw_get_version();
        }
        break;

        case EFFECT_LOAD_LIBRARY: {
            ALOGV("%s EFFECT_LOAD_LIBRARY", __func__);
            const char *lib_name = (const char *)data.readCString();
            ALOGV("%s: calling qahw_effect_load_library", __func__);
            qahw_effect_lib_handle_t handle = qahw_effect_load_library(lib_name);
            reply->write(&handle, sizeof(qahw_effect_lib_handle_t));
        }
        break;

        case EFFECT_UNLOAD_LIBRARY: {
            ALOGV("%s EFFECT_UNLOAD_LIBRARY", __func__);
            qahw_effect_lib_handle_t handle;
            int32_t status;

            data.read(&handle, sizeof(qahw_effect_lib_handle_t));
            status = qahw_effect_unload_library(handle);
            reply->writeInt32(status);
        }
        break;

        case EFFECT_CREATE: {
            ALOGV("%s EFFECT_CREATE", __func__);
            qahw_effect_lib_handle_t handle;
            qahw_effect_uuid_t uuid;
            int32_t io_handle;
            qahw_effect_handle_t effect_handle;
            int32_t status;

            data.read(&handle, sizeof(qahw_effect_lib_handle_t));
            data.read(&uuid, sizeof(qahw_effect_uuid_t));
            data.readInt32(&io_handle);
            status = qahw_effect_create(handle, (const qahw_effect_uuid_t *)&uuid,
                                        io_handle, &effect_handle);
            reply->write(&effect_handle, sizeof(qahw_effect_handle_t));
            reply->writeInt32(status);
        }
        break;

        case EFFECT_RELEASE: {
            ALOGV("%s EFFECT_RELEASE", __func__);
            qahw_effect_lib_handle_t handle;
            qahw_effect_handle_t effect_handle;
            int32_t status;

            data.read(&handle, sizeof(qahw_effect_lib_handle_t));
            data.read(&effect_handle, sizeof(qahw_effect_handle_t));
            status = qahw_effect_release(handle, effect_handle);
            reply->writeInt32(status);
        }
        break;

        case EFFECT_GET_DESCRIPTOR: {
            ALOGV("%s EFFECT_GET_DESCRIPTOR", __func__);
            qahw_effect_lib_handle_t handle;
            qahw_effect_uuid_t uuid;
            qahw_effect_descriptor_t effect_desc;
            int32_t status;

            data.read(&handle, sizeof(qahw_effect_lib_handle_t));
            data.read(&uuid, sizeof(qahw_effect_uuid_t));
            status = qahw_effect_get_descriptor(handle,
                          (const qahw_effect_uuid_t *)&uuid, &effect_desc);
            reply->write(&effect_desc, sizeof(qahw_effect_descriptor_t));
            reply->writeInt32(status);
        }
        break;

        case EFFECT_GET_VERSION: {
            ALOGV("%s EFFECT_GET_VERSION", __func__);
            return qahw_effect_get_version();
        }
        break;

        case EFFECT_COMMAND: {
            qahw_effect_lib_handle_t handle;
            uint32_t cmd_code;
            uint32_t cmd_size;
            void *cmd_data = NULL;
            uint32_t reply_size;
            void *reply_data = NULL;
            int32_t status;
            android::Parcel::ReadableBlob r_blob;
            android::Parcel::WritableBlob w_blob;

            data.read(&handle, sizeof(qahw_effect_lib_handle_t));
            data.readUint32(&cmd_code);
            data.readUint32(&cmd_size);
            ALOGV("%s: EFFECT_COMMAND cmd_code %d, cmd_size %d", __func__,
                            cmd_code, cmd_size);
            cmd_data = calloc(1, cmd_size);
            if (!cmd_data) {
                ALOGE("%s: could not allocate memory for cmd data", __func__);
                return -EINVAL;
            }
            status_t ret = data.readBlob(cmd_size, &r_blob);
            if (status) {
                ALOGE("%s: Error in readblob, status %d", __func__, status);
                return -EINVAL;
            }
            memscpy(cmd_data, cmd_size, r_blob.data(), cmd_size);
            data.read(&reply_size, sizeof(reply_size));

            reply_data = calloc(1, reply_size);
            if (!reply_data) {
                ALOGE("%s: could not allocate memory for reply data", __func__);
                free(cmd_data);
                r_blob.release();
                return -EINVAL;
            }
            status = qahw_effect_command(handle, cmd_code, cmd_size, cmd_data,
                                         &reply_size, reply_data);
            ret = reply->writeBlob(reply_size, false, &w_blob);
            if (ret) {
                ALOGE("%s: Error in writeblob, status %d", __func__, ret);
                r_blob.release();
                return -EINVAL;
            }
            memset(w_blob.data(), 0x0, reply_size);
            memscpy(w_blob.data(), reply_size, reply_data, reply_size);
            reply->writeInt32(status);
            free(cmd_data);
            free(reply_data);
            r_blob.release();
            w_blob.release();
        }
        break;

        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
    return 0;
}


int qti_audio_server::qahw_out_set_callback(qahw_stream_handle_t *out_handle,
                              qahw_stream_callback_t callback,
                              void *cookie) {
    return qahw_out_set_callback_l(out_handle, callback, cookie);
}


void* qti_audio_server::qahw_load_module(const char *hw_module_id) {
    return qahw_load_module_l(hw_module_id);
}

int qti_audio_server::qahw_open_output_stream(qahw_module_handle_t *hw_module,
                                    audio_io_handle_t handle,
                                    audio_devices_t devices,
                                    audio_output_flags_t flags,
                                    struct audio_config *config,
                                    qahw_stream_handle_t **out_handle,
                                    const char *address) {
    return qahw_open_output_stream_l(hw_module,
                             handle,
                             devices,
                             flags,
                             config,
                             out_handle,
                             address);
}

int qti_audio_server::qahw_close_output_stream(qahw_stream_handle_t *handle) {
    return qahw_close_output_stream_l(handle);
}


int qti_audio_server::qahw_open_input_stream(qahw_module_handle_t *hw_module,
                                    audio_io_handle_t handle,
                                    audio_devices_t devices,
                                    struct audio_config *config,
                                    qahw_stream_handle_t **stream_in,
                                    audio_input_flags_t flags,
                                    const char *address,
                                    audio_source_t source) {
    return qahw_open_input_stream_l(hw_module,
                           handle,
                           devices,
                           config,
                           stream_in,
                           flags,
                           address,
                           source);
}

int qti_audio_server::qahw_out_drain(qahw_stream_handle_t *out_handle,
                                    qahw_drain_type_t type ) {
    return qahw_out_drain_l(out_handle, type);
}
audio_channel_mask_t qti_audio_server::qahw_out_get_channels
                                (const qahw_stream_handle_t *out_handle) {
    return qahw_out_get_channels_l(out_handle);
}
audio_format_t qti_audio_server::qahw_out_get_format
                                (const qahw_stream_handle_t *out_handle) {
    return qahw_out_get_format_l(out_handle);
}

size_t qti_audio_server::qahw_out_get_buffer_size(
                                const qahw_stream_handle_t *out_handle) {
    return qahw_out_get_buffer_size_l(out_handle);
}

int qti_audio_server::qahw_out_standby(qahw_stream_handle_t *out_handle) {
    return qahw_out_standby_l(out_handle);
}

int qti_audio_server::qahw_unload_module(qahw_module_handle_t *handle) {
    return qahw_unload_module_l(handle);
}

int qti_audio_server::qahw_out_get_render_position(
                                const qahw_stream_handle_t *out_handle,
                                uint32_t *dsp_frames) {
    return qahw_out_get_render_position_l(out_handle, dsp_frames);
}

int qti_audio_server::qahw_out_set_volume(qahw_stream_handle_t *out_handle,
                                    float left, float right) {
    return qahw_out_set_volume_l(out_handle, left, right);
}

uint32_t qti_audio_server::qahw_out_get_sample_rate(const qahw_stream_handle_t
                                    *out_handle) {
    return qahw_out_get_sample_rate_l(out_handle);
}

int qti_audio_server::qahw_out_set_sample_rate(qahw_stream_handle_t *out_handle,
                                    uint32_t rate) {
    return qahw_out_set_sample_rate_l(out_handle, rate);
}

uint32_t qti_audio_server::qahw_out_get_latency(const qahw_stream_handle_t
                                    *out_handle __unused) {
//dummy funtion
    return -ENOSYS;
}

int qti_audio_server::qahw_out_set_parameters(qahw_stream_handle_t *out_handle,
                                    const char *kv_pairs) {
    return qahw_out_set_parameters_l(out_handle, kv_pairs);
}

char *qti_audio_server::qahw_out_get_parameters(const qahw_stream_handle_t *out_handle,
                                   const char *keys) {
    return qahw_out_get_parameters_l(out_handle, keys);
}

int qti_audio_server::qahw_out_pause(qahw_stream_handle_t *out_handle) {
    return qahw_out_pause_l(out_handle);
}

int qti_audio_server::qahw_out_resume(qahw_stream_handle_t *out_handle) {
    return qahw_out_resume_l(out_handle);
}

int qti_audio_server::qahw_out_flush(qahw_stream_handle_t *out_handle) {
    return qahw_out_flush_l(out_handle);
}

int qti_audio_server::qahw_out_get_presentation_position(const qahw_stream_handle_t *out_handle __unused,
                                           uint64_t *frames __unused,
                                           struct timespec *timestamp __unused) {
//dummy function.
    return -ENOSYS;
}

int qti_audio_server::qahw_out_set_param_data(qahw_stream_handle_t *out_handle,
                                    qahw_param_id param_id,
                                    qahw_param_payload *payload) {
    return qahw_out_get_param_data_l(out_handle, param_id, payload);
}

int qti_audio_server::qahw_out_get_param_data(qahw_stream_handle_t *out_handle,
                                    qahw_param_id param_id,
                                    qahw_param_payload *payload) {
    return qahw_out_get_param_data_l(out_handle, param_id, payload);
}

int qti_audio_server::qahw_close_input_stream(qahw_stream_handle_t *in_handle) {
    return qahw_close_input_stream_l(in_handle);
}

int qti_audio_server::qahw_in_standby(qahw_stream_handle_t *out_handle) {
    return qahw_in_standby_l(out_handle);
}

int qti_audio_server::qahw_set_parameters(qahw_module_handle_t *hw_module, const char *kv_pairs) {
    return qahw_set_parameters_l(hw_module, kv_pairs);
}

int qti_audio_server::qahw_in_set_parameters(qahw_stream_handle_t *in_handle, const char *kv_pairs) {
    return qahw_in_set_parameters_l(in_handle, kv_pairs);
}

size_t qti_audio_server::qahw_in_get_buffer_size(const qahw_stream_handle_t *in_handle) {
    return qahw_in_get_buffer_size_l(in_handle);
}

int qti_audio_server::qahw_set_param_data(const qahw_module_handle_t *hw_module,
                                    qahw_param_id param_id,
                                    qahw_param_payload *payload) {
    return qahw_set_param_data_l(hw_module, param_id, payload);
}

int qti_audio_server::qahw_get_param_data(const qahw_module_handle_t *hw_module,
                                qahw_param_id param_id,
                                qahw_param_payload *payload) {
    return qahw_get_param_data_l(hw_module, param_id, payload);
}

uint32_t qti_audio_server::qahw_in_get_sample_rate(const qahw_stream_handle_t *in_handle) {
    return qahw_in_get_sample_rate_l(in_handle);
}

int qti_audio_server::qahw_in_set_sample_rate(qahw_stream_handle_t *in_handle, uint32_t rate) {
    return qahw_in_set_sample_rate_l(in_handle, rate);
}

audio_channel_mask_t qti_audio_server::qahw_in_get_channels
                        (const qahw_stream_handle_t *in_handle) {
    return qahw_in_get_channels_l(in_handle);
}

audio_format_t qti_audio_server::qahw_in_get_format
                                (const qahw_stream_handle_t *handle) {
    return qahw_in_get_format_l(handle);
}

int qti_audio_server::qahw_in_set_format
                (qahw_stream_handle_t *handle, audio_format_t format) {
    return qahw_in_set_format_l(handle, format);
}

char* qti_audio_server::qahw_in_get_parameters(const qahw_stream_handle_t *in_handle,
                                   const char *keys) {
    return qahw_in_get_parameters_l(in_handle, keys);
}

uint32_t qti_audio_server::qahw_in_get_input_frames_lost(qahw_stream_handle_t *in_handle) {
    return qahw_in_get_input_frames_lost_l(in_handle);
}

int qti_audio_server::qahw_in_get_capture_position(const qahw_stream_handle_t *in_handle,
                             int64_t *frames, int64_t *time) {
    return qahw_in_get_capture_position_l(in_handle, frames, time);
}

int qti_audio_server::qahw_init_check(const qahw_module_handle_t *hw_module) {
    return qahw_init_check_l(hw_module);
}

int qti_audio_server::qahw_set_voice_volume(qahw_module_handle_t *hw_module, float volume) {
    return qahw_set_voice_volume_l(hw_module, volume);

}

int qti_audio_server::qahw_set_mode(qahw_module_handle_t *hw_module, audio_mode_t mode) {
    return qahw_set_mode_l(hw_module, mode);
}

int qti_audio_server::qahw_set_mic_mute(qahw_module_handle_t *hw_module, bool state) {
    return qahw_set_mic_mute_l(hw_module, state);
}

int qti_audio_server::qahw_get_mic_mute(qahw_module_handle_t *hw_module, bool *state) {
    return qahw_get_mic_mute_l(hw_module, state);
}

char* qti_audio_server::qahw_get_parameters(const qahw_module_handle_t *hw_module,
                           const char *keys) {
    return qahw_get_parameters_l(hw_module, keys);
}

size_t qti_audio_server::qahw_get_input_buffer_size(const qahw_module_handle_t *hw_module,
                                  const struct audio_config *config) {
    return qahw_get_input_buffer_size_l(hw_module, config);
}

qahw_effect_lib_handle_t qti_audio_server::qahw_effect_load_library(const char *lib_name) {
    return qahw_effect_load_library_l(lib_name);
}

int32_t qti_audio_server::qahw_effect_unload_library(qahw_effect_lib_handle_t handle) {
    return qahw_effect_unload_library_l(handle);
}

int32_t qti_audio_server::qahw_effect_create(qahw_effect_lib_handle_t handle,
                           const qahw_effect_uuid_t *uuid,
                           int32_t io_handle,
                           qahw_effect_handle_t *effect_handle) {
    return qahw_effect_create_l(handle, uuid, io_handle, effect_handle);
}

int32_t qti_audio_server::qahw_effect_release(qahw_effect_lib_handle_t handle,
                            qahw_effect_handle_t effect_handle) {
    return qahw_effect_release_l(handle, effect_handle);
}

int32_t qti_audio_server::qahw_effect_get_descriptor(qahw_effect_lib_handle_t handle,
                                   const qahw_effect_uuid_t *uuid,
                                   qahw_effect_descriptor_t *effect_desc) {
    return qahw_effect_get_descriptor_l(handle, uuid, effect_desc);
}

int32_t qti_audio_server::qahw_effect_get_version() {
    return qahw_effect_get_version_l();
}

int32_t qti_audio_server::qahw_effect_process(qahw_effect_handle_t self,
                            qahw_audio_buffer_t *in_buffer,
                            qahw_audio_buffer_t *out_buffer) {
    return qahw_effect_process_l(self, in_buffer, out_buffer);
}

int32_t qti_audio_server::qahw_effect_command(qahw_effect_handle_t self,
                            uint32_t cmd_code,
                            uint32_t cmd_size,
                            void *cmd_data,
                            uint32_t *reply_size,
                            void *reply_data) {
    return qahw_effect_command_l(self, cmd_code, cmd_size, cmd_data, reply_size, reply_data);
}

int32_t qti_audio_server::qahw_effect_process_reverse(qahw_effect_handle_t self,
                                    qahw_audio_buffer_t *in_buffer,
                                    qahw_audio_buffer_t *out_buffer) {
    return qahw_effect_process_reverse_l(self, in_buffer, out_buffer);
}

int qti_audio_server::qahw_create_audio_patch(qahw_module_handle_t *hw_module,
                        unsigned int num_sources,
                        const struct audio_port_config *sources,
                        unsigned int num_sinks,
                        const struct audio_port_config *sinks,
                        audio_patch_handle_t *handle)
{
    return qahw_create_audio_patch_l(hw_module, num_sources,
                                         sources, num_sinks, sinks,
                                         handle);
}

int qti_audio_server::qahw_release_audio_patch(qahw_module_handle_t *hw_module,
                        audio_patch_handle_t handle)
{
    return qahw_release_audio_patch_l(hw_module, handle);
}

int qti_audio_server::qahw_get_audio_port(qahw_module_handle_t *hw_module,
                      struct audio_port *port)
{
    return qahw_get_audio_port_l(hw_module, port);
}

int qti_audio_server::qahw_set_audio_port_config(qahw_module_handle_t *hw_module,
                     const struct audio_port_config *config)
{
    return qahw_set_audio_port_config_l(hw_module, config);
}

int qti_audio_server::qahw_get_version() {
    return qahw_get_version_l();
}

static void * write_stream_thread_loop(void *context) {
    int rc = 0;
    n_stream_handle *n_stream = (n_stream_handle *)context;
    sh_mem_data *shmem_data = n_stream->shmem_data;

    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
    prctl(PR_SET_NAME, (unsigned long)"Write Stream Thread", 0, 0, 0);

    while (1) {
        pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);
        pthread_mutex_unlock(&shmem_data->c_mutex);
        switch(shmem_data->cmd) {
            case WRITE:
            {
                shmem_data->buf.out_buf.buffer= n_stream->shmem_data_buff;
                rc = qahw_out_write_l(n_stream->qahw_stream, &shmem_data->buf.out_buf);

                // copy return value(number of bytes consumed or error if any)
                int *return_value = (int *)shmem_data->buf.out_buf.buffer;
                *return_value = rc;

                //wakeup calling thread.
                shmem_data->status = 0;
                //make sure proxy side waits for server to wait on c_mutex
                ALOGD("%s:Taking cmd mutex", __func__);
                pthread_mutex_lock(&shmem_data->c_mutex);
                ALOGD("%s:Taken cmd mutex", __func__);
                pthread_cond_signal(&shmem_data->c_cond);
                break;
            }
            case GET_PRESENTATION_TIME:
            {
                presentation_time_struct *shmem_pts_buff =
                    (presentation_time_struct *)n_stream->shmem_data_buff;
                shmem_pts_buff->ret_val = qahw_out_get_presentation_position_l
                                                     (n_stream->qahw_stream,
                                                      &shmem_pts_buff->frames,
                                                      &shmem_pts_buff->timestamp);
                //wakeup calling thread.
                shmem_data->status = 0;
                //make sure proxy side waits for server to wait on c_mutex
                ALOGD("%s:Taking cmd mutex", __func__);
                pthread_mutex_lock(&shmem_data->c_mutex);
                ALOGD("%s:Taken cmd mutex", __func__);
                pthread_cond_signal(&shmem_data->c_cond);
                break;
            }
            case GET_LATENCY:
            {
                uint32_t *latency = (uint32_t *)n_stream->shmem_data_buff;
                *latency = qahw_out_get_latency_l(
                                    n_stream->qahw_stream);
                //wakeup calling thread.
                shmem_data->status = 0;
                //make sure proxy side waits for server to wait on c_mutex
                ALOGD("%s:Taking cmd mutex", __func__);
                pthread_mutex_lock(&shmem_data->c_mutex);
                ALOGD("%s:Taken cmd mutex", __func__);
                pthread_cond_signal(&shmem_data->c_cond);
                break;
            }
            case STOP:
            {
                shmem_data->status = 0;
                pthread_cond_signal(&shmem_data->c_cond);
                ALOGD("%s: exiting", __func__);
                pthread_exit(0);
                break;
            }
            default:
            {
                ALOGE("%u: unknown command", shmem_data->cmd);
                break;
            }
        }
    }
}

int qti_audio_server::prepare_for_write(qahw_stream_handle_t *stream,
                                         qahw_out_buffer_t *buff __unused) {
    int rc = 0;
    n_stream_handle *n_stream = (n_stream_handle *)stream;
    rc = pthread_mutex_lock(&n_stream->shmem_data->c_mutex);
    rc = pthread_create(&n_stream->thread,
                   (const pthread_attr_t *)NULL, //ashok:thread priority?
                   write_stream_thread_loop,
                   n_stream);
    ALOGD("created Write Stream Thread");
    return rc;

}

static void * read_stream_thread_loop(void *context) {
    int rc = 0;
    n_stream_handle *n_stream = (n_stream_handle *)context;
    sh_mem_data *shmem_data = n_stream->shmem_data;

    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
    prctl(PR_SET_NAME, (unsigned long)"Read Stream Thread", 0, 0, 0);

    while (1) {
        pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);
        pthread_mutex_unlock(&shmem_data->c_mutex);
        switch(shmem_data->cmd) {
            case READ:
            {
                shmem_data->buf.in_buf.buffer = n_stream->shmem_data_buff;
                ALOGV("%d: %s data %p, bytes %zd", __LINE__, __func__,
                    n_stream->shmem_data_buff, shmem_data->buf.in_buf.bytes);
                rc = qahw_in_read_l(n_stream->qahw_stream, &shmem_data->buf.in_buf);

                if (shmem_data->buf.in_buf.bytes != (size_t)rc) {
                    ALOGD("%d: %s:cmd recvd(%d)", __LINE__, __func__, shmem_data->cmd);
                    shmem_data->buf.in_buf.bytes = rc;
                }

                //wakeup calling thread.
                ALOGV("%d: %s: fire status signal(%zd:%zd)",
                    __LINE__, __func__, n_stream, n_stream->qahw_stream);
                shmem_data->status = 0;
                ALOGD("%s:Taking cmd mutex", __func__);
                pthread_mutex_lock(&shmem_data->c_mutex);
                ALOGD("%s:Taken cmd mutex", __func__);
                pthread_cond_signal(&shmem_data->c_cond);
                break;
            }
            case STOP:
            {
                shmem_data->status = 0;
                pthread_cond_signal(&shmem_data->c_cond);
                ALOGD("%s: exiting", __func__);
                pthread_exit(0);
                break;
            }

            default:
            {
                ALOGE("%d: unknown command", shmem_data->cmd);
                break;
            }
        }
    }
}

int qti_audio_server::prepare_for_read(qahw_stream_handle_t *stream,
                                         qahw_in_buffer_t *buff __unused) {
    int rc = 0;
    n_stream_handle *n_stream = (n_stream_handle *)stream;
    rc = pthread_mutex_lock(&n_stream->shmem_data->c_mutex);
    rc = pthread_create(&n_stream->thread,
                   (const pthread_attr_t *)NULL,
                   read_stream_thread_loop,
                   n_stream);
    ALOGD("created read Stream Thread");
    return rc;

}

ssize_t qti_audio_server::qahw_out_write(qahw_stream_handle_t *stream __unused,
                               qahw_out_buffer_t *out_buf __unused) {
//dummy funtion
    return -ENOSYS;
}

ssize_t qti_audio_server::qahw_in_read(qahw_stream_handle_t *in_handle __unused,
                             qahw_in_buffer_t *in_buf __unused) {
//dummy funtion
    return -ENOSYS;
}


}; // namespace audiohal

