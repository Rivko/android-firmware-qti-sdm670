/* Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "qti_audio_proxy"

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

#include <pthread.h>
#include "qti_audio_server.h"

struct listnode stream_list;
pthread_mutex_t list_lock;

namespace audiohal {

using namespace android;

class qti_audio_proxy: public ::android:: BpInterface<Iqti_audio_server> {

public:
qti_audio_proxy(const sp<IBinder>& impl) : BpInterface<Iqti_audio_server>(impl) {
    ALOGD("qti_audio_proxy::qti_audio_proxy()");
}

~qti_audio_proxy() {
    ALOGD("~qti_audio_proxy::qti_audio_proxy()");
}

void *qahw_load_module(const char *hw_module_id) {
    ALOGV("%s %d hal string(%s)", __func__, __LINE__, hw_module_id);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.writeCString(hw_module_id);

    remote()->transact(LOAD_HAL, data, &reply);
    qahw_module_handle_t *audio_handle = NULL;
    reply.read(&audio_handle, sizeof(qahw_module_handle_t *));
    return (void *)(audio_handle);
}

int qahw_unload_module(qahw_module_handle_t *hw_module) {
    ALOGV("%s %d hal handle(%p)", __func__, __LINE__, hw_module);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    remote()->transact(UNLOAD_HAL, data, &reply);
    return reply.readInt32();
}

int qahw_open_output_stream(qahw_module_handle_t *hw_module,
                             audio_io_handle_t handle,
                             audio_devices_t devices,
                             audio_output_flags_t flags,
                             struct audio_config *config,
                             qahw_stream_handle_t **out_handle,
                             const char *address) {
    ALOGV("%s %d hal_handle(%x)", __func__, __LINE__, handle);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&handle, sizeof(audio_io_handle_t));
    data.write(&devices, sizeof(audio_devices_t));
    data.write(&flags, sizeof(audio_output_flags_t));

    uint32_t param_size = sizeof(audio_config);
    data.writeUint32(param_size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(param_size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, param_size);
    memscpy(blob.data(), param_size, config, param_size);

    data.writeCString(address);

    remote()->transact(OPEN_OUTPUT_STREAM, data, &reply);
    blob.release();
    p_stream_handle *p_stream = (p_stream_handle *)calloc(1, sizeof(p_stream_handle));
    if (0 == p_stream)
        goto exit;

    // stream handle
    reply.read(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    ALOGD("Allocated Proxy side stream mem %p wrapping server handle %p",
        p_stream, p_stream->n_stream_handle);

    if (0 == p_stream->n_stream_handle)
        goto exit;

    *out_handle = (qahw_stream_handle_t *)p_stream;
    pthread_mutex_lock(&list_lock);
    list_add_tail(&stream_list, &p_stream->list);
    pthread_mutex_unlock(&list_lock);
    return reply.readInt32();

exit:
    if (p_stream != NULL) {
        p_stream->sh_mem_dealer.clear();
        p_stream->sh_mem_handle.clear();
        free(p_stream);
    }
    ALOGE("%s: failed: unable to open output stream", __func__);
    return -EINVAL;
}

int qahw_close_output_stream(qahw_stream_handle_t *out_handle) {
    int rc = 0;
    p_stream_handle *handle = NULL;
    struct listnode *node = NULL;
    struct listnode *tempnode = NULL;

    ALOGV("%s %d out_handle(%p)", __func__, __LINE__, out_handle);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;

    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));

    /*Closing stream thread */
    if (p_stream->prepare_done == true) {
        sh_mem_data *shmem_data = p_stream->shmem_data;
        pthread_mutex_lock(&p_stream->shmem_lock);
        if (shmem_data != NULL) {
            shmem_data->cmd = STOP;
            shmem_data->status = -1;
            pthread_mutex_lock(&shmem_data->c_mutex);
            ALOGV("Waking up stream thread(%x:%x)", p_stream, p_stream->n_stream_handle);
            rc = pthread_cond_signal(&shmem_data->c_cond);
            while (shmem_data->status == -1)
                pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);
            pthread_mutex_unlock(&shmem_data->c_mutex);
        } else {
            ALOGE("%s Cannot dereference a NULL pointer", __func__);
        }
        pthread_mutex_unlock(&p_stream->shmem_lock);
    }

    remote()->transact(CLOSE_OUTPUT_STREAM, data, &reply);
    rc = reply.readInt32();
    if (rc != 0)
        goto exit;

    pthread_mutex_lock(&list_lock);
    list_for_each_safe(node, tempnode, &stream_list) {
        handle = node_to_item(node, p_stream_handle, list);
        if (handle != NULL && handle == p_stream) {
            ALOGV("%s %d: clear memory of handle %p", __func__, __LINE__, handle);
            handle->sh_mem_dealer.clear();
            handle->sh_mem_handle.clear();
            list_remove(node);
            free(node_to_item(node, p_stream_handle, list));
        }
    }
    pthread_mutex_unlock(&list_lock);
    ALOGV("unalloc shared mem as well as free malloced stuff");
    return rc;

exit:
   ALOGE("%s failed", __func__);
   return -EINVAL;
}

int qahw_out_drain(qahw_stream_handle_t *out_handle,
                    qahw_drain_type_t type ) {
    ALOGV("%s %d, type %d", __func__, __LINE__, type);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.write(&type, sizeof(qahw_drain_type_t));
    return remote()->transact(OUT_DRAIN, data, &reply);
}

int qahw_out_standby(qahw_stream_handle_t *out_handle) {
    ALOGV("%s %d", __func__, __LINE__);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    return remote()->transact(OUT_STANDBY, data, &reply);
}

int qahw_open_input_stream(qahw_module_handle_t *hw_module,
                               audio_io_handle_t handle,
                               audio_devices_t devices,
                               struct audio_config *config,
                               qahw_stream_handle_t **stream_in,
                               audio_input_flags_t flags,
                               const char *address,
                               audio_source_t source) {
    ALOGV("%s %d", __func__, __LINE__);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&handle, sizeof(audio_io_handle_t));
    data.write(&devices, sizeof(audio_devices_t));

    uint32_t param_size = sizeof(audio_config);
    data.writeUint32(param_size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(param_size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, param_size);
    memscpy(blob.data(), param_size, config, param_size);

    data.write(&flags, sizeof(audio_input_flags_t));
    data.writeCString(address);
    data.write(&source, sizeof(audio_source_t));

    remote()->transact(OPEN_INPUT_STREAM, data, &reply);
    blob.release();
    p_stream_handle *p_stream =(p_stream_handle *)calloc(1, sizeof(p_stream_handle));
    if (0 == p_stream)
        goto exit;

    // stream handle
    reply.read(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    ALOGD("%s: Allocated Proxy side stream mem %p wrapping server handle %p",
        __func__, p_stream, p_stream->n_stream_handle);
    if (0 == p_stream->n_stream_handle)
        goto exit;

    *stream_in = (qahw_stream_handle_t *)p_stream;
    pthread_mutex_lock(&list_lock);
    list_add_tail(&stream_list, &p_stream->list);
    pthread_mutex_unlock(&list_lock);
    return reply.readInt32();

exit:
        if (p_stream != NULL) {
            p_stream->sh_mem_dealer.clear();
            p_stream->sh_mem_handle.clear();
            free(p_stream);
        }
        ALOGE("%s: failed: unable to open input stream", __func__);
        return -EINVAL;
}

int qahw_close_input_stream(qahw_stream_handle_t *in_handle) {
    int rc = 0;
    p_stream_handle *handle = NULL;
    struct listnode *node = NULL;
    struct listnode *tempnode = NULL;

    ALOGV("%s %d in_handle(%p)", __func__, __LINE__, in_handle);
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));

    /*Closing stream thread */
    if (p_stream->prepare_done == true) {
        sh_mem_data *shmem_data = p_stream->shmem_data;
        if (shmem_data != NULL) {
            shmem_data->cmd = STOP;
            shmem_data->status = -1;
            pthread_mutex_lock(&shmem_data->c_mutex);
            ALOGV("Waking up stream thread(%x:%x)", p_stream, p_stream->n_stream_handle);
            rc = pthread_cond_signal(&shmem_data->c_cond);
            while (shmem_data->status == -1)
                pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);
            pthread_mutex_unlock(&shmem_data->c_mutex);
        } else {
            ALOGE("%s Cannot dereference a NULL pointer", __func__);
        }
    }

    remote()->transact(CLOSE_INPUT_STREAM, data, &reply);
    rc = reply.readInt32();
    if (rc != 0)
        goto exit;

    pthread_mutex_lock(&list_lock);
    list_for_each_safe(node, tempnode, &stream_list) {
        handle = node_to_item(node, p_stream_handle, list);
        if (handle != NULL && handle == p_stream) {
            ALOGV("%s %d: clear memory of handle %p", __func__, __LINE__, handle);
            handle->sh_mem_dealer.clear();
            handle->sh_mem_handle.clear();
            list_remove(node);
            free(node_to_item(node, p_stream_handle, list));
        }
    }
    pthread_mutex_unlock(&list_lock);
    ALOGV("unalloc shared mem as well as free malloced stuff");
    return rc;

exit:
    ALOGE("%s failed", __func__);
    return -EINVAL;
}

ssize_t qahw_out_write(qahw_stream_handle_t *stream,
                       qahw_out_buffer_t *out_buf) {
    ssize_t rc = 0;
    int *return_value = NULL;
    p_stream_handle *p_stream = (p_stream_handle *)stream;
    sh_mem_data *shmem_data = NULL;
    qahw_out_buffer_t *shmem_buff_header = NULL;
    void *shmem_data_buff = NULL;
    out_buf->offset = 0;

    ALOGD("Taking shared mem mutex %p", &p_stream->shmem_lock);
    rc = pthread_mutex_lock(&p_stream->shmem_lock);

    if (p_stream->prepare_done == false) {
        rc = prepare_for_write((qahw_stream_handle_t *)p_stream, out_buf);
        if (rc != 0)
            goto exit;
    }

    shmem_data = p_stream->shmem_data;
    shmem_buff_header = &shmem_data->buf.out_buf;
    shmem_data_buff = p_stream->shmem_data_buff;

    //copy buff
    if (out_buf->bytes > shmem_buff_header->bytes) { //some freaking problem
        ALOGE("Received more bytes than allocated memory");
        goto exit;
    }

    memscpy((void *)shmem_data_buff, shmem_buff_header->bytes, (char *)out_buf->buffer, out_buf->bytes);
    ALOGV("%s: copied %zd to sh mem", __func__, out_buf->bytes);

    //copy buff header
    memscpy(shmem_buff_header, sizeof(qahw_out_buffer_t), out_buf, sizeof(qahw_out_buffer_t));

    shmem_data->cmd = WRITE;

    // is the server thread waiting for commands
    ALOGD("Taking command mutex %p", &shmem_data->c_mutex);
    rc = pthread_mutex_lock(&shmem_data->c_mutex);

    shmem_data->status = -1;

    //wakeup server stream thread.
    ALOGD("Waking up stream thread(%p:%p)",
        p_stream, p_stream->n_stream_handle);
    rc = pthread_cond_signal(&shmem_data->c_cond);
    ALOGD("Woken up stream thread(%p:%p) %d",
        p_stream, p_stream->n_stream_handle, (int)rc);

    while (shmem_data->status == -1) {
        ALOGD("Waiting for status signal stream(%p:%p)",
            p_stream, p_stream->n_stream_handle);

        rc = pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);

        ALOGD("Waiting ended for status signal stream(%p:%p) %d",
            p_stream, p_stream->n_stream_handle, (int)rc);
    }
    rc = pthread_mutex_unlock(&shmem_data->c_mutex);
    return_value = (int *)shmem_data_buff;
    rc = *return_value;
    ALOGD("Unlock shared mem mutex %p", &p_stream->shmem_lock);
    pthread_mutex_unlock(&p_stream->shmem_lock);
    ALOGD("%s: bytes consumed by server %d", __func__, (int)rc);
    return rc;
exit:
    ALOGE("%s failed", __func__);
    rc = pthread_mutex_unlock(&p_stream->shmem_lock);
    return -EINVAL;
}

int qahw_out_get_presentation_position(const qahw_stream_handle_t *out_handle,
                                       uint64_t *frames,
                                       struct timespec *timestamp) {
    int rc = 0;
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    sh_mem_data *shmem_data = NULL;
    presentation_time_struct *shmem_pts_buff = NULL;
    ALOGV("%s %d", __func__, __LINE__);

    rc = pthread_mutex_lock(&p_stream->shmem_lock);
    if (p_stream->prepare_done == false) {
        goto exit;
    }

    shmem_pts_buff = (presentation_time_struct *)&p_stream->shmem_data_buff;
    shmem_data = p_stream->shmem_data;

    shmem_data->cmd = GET_PRESENTATION_TIME;

    // is the server thread waiting for commands
    ALOGD("Taking command mutex %zd", &shmem_data->c_mutex);
    rc = pthread_mutex_lock(&shmem_data->c_mutex);

    shmem_data->status = -1;

    //wakeup server stream thread.
    ALOGD("Waking up stream thread(%zd:%zd)",
        p_stream, p_stream->n_stream_handle);
    rc = pthread_cond_signal(&shmem_data->c_cond);
    ALOGD("Woken up stream thread(%zd:%zd) %d",
        p_stream, p_stream->n_stream_handle, rc);

    while (shmem_data->status == -1) {
        ALOGD("Waiting for status signal stream(%zd:%zd)",
            p_stream, p_stream->n_stream_handle);

        rc = pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);

        ALOGD("Waiting ended for status signal stream(%zd:%zd) %d",
            p_stream, p_stream->n_stream_handle, rc);
    }
    rc = pthread_mutex_unlock(&shmem_data->c_mutex);
    *frames = shmem_pts_buff->frames;
    memscpy(timestamp, sizeof(timestamp), &shmem_pts_buff->timestamp, sizeof(timestamp));
    rc = shmem_pts_buff->ret_val;

    rc = pthread_mutex_unlock(&p_stream->shmem_lock);
    return rc;

exit:
    ALOGE("%s failed", __func__);
    rc = pthread_mutex_unlock(&p_stream->shmem_lock);
    return -EINVAL;
}

uint32_t qahw_out_get_latency(const qahw_stream_handle_t *out_handle) {
    int rc = 0;
    int *return_value = NULL;
    uint32_t latency = 0;
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    sh_mem_data *shmem_data = NULL;
    void *shmem_data_buff = NULL;
    ALOGV("%s %d", __func__, __LINE__);

    rc = pthread_mutex_lock(&p_stream->shmem_lock);

    if (p_stream->prepare_done == false) {
        goto exit;
    }

    shmem_data = p_stream->shmem_data;
    shmem_data_buff = p_stream->shmem_data_buff;

    shmem_data->cmd = GET_LATENCY;

    // is the server thread waiting for commands
    ALOGD("Taking command mutex %zd", &shmem_data->c_mutex);
    rc = pthread_mutex_lock(&shmem_data->c_mutex);

    shmem_data->status = -1;

    //wakeup server stream thread.
    ALOGD("Waking up stream thread(%zd:%zd)",
             p_stream, p_stream->n_stream_handle);
    rc = pthread_cond_signal(&shmem_data->c_cond);
    ALOGD("Woken up stream thread(%zd:%zd) %d",
                p_stream, p_stream->n_stream_handle, rc);

    while (shmem_data->status == -1) {
        ALOGD("Waiting for status signal stream(%zd:%zd)",
                    p_stream, p_stream->n_stream_handle);

        rc = pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);

        ALOGD("Waiting ended for status signal stream(%zd:%zd) %d",
                    p_stream, p_stream->n_stream_handle, rc);
    }
    rc = pthread_mutex_unlock(&shmem_data->c_mutex);
    return_value = (int *)shmem_data_buff;
    latency = *return_value;
    rc = pthread_mutex_unlock(&p_stream->shmem_lock);
    return latency;

exit:
    ALOGE("%s failed", __func__);
    rc = pthread_mutex_unlock(&p_stream->shmem_lock);
    return -EINVAL;
}

ssize_t qahw_in_read(qahw_stream_handle_t *stream,
                     qahw_in_buffer_t *in_buf) {
    int rc = 0;
    p_stream_handle *p_stream = (p_stream_handle *)stream;
    sh_mem_data *shmem_data = NULL;
    qahw_in_buffer_t *shmem_buff_header = NULL;
    void *shmem_data_buff = NULL;
    in_buf->offset = 0;
    int64_t *timestamp = NULL;
    if (in_buf->timestamp)
        timestamp = (int64_t *)in_buf->timestamp;

    if (p_stream->prepare_done == false) {
        rc = prepare_for_read((qahw_stream_handle_t *)p_stream, in_buf);
        if (rc != 0)
            goto exit;
    }

    shmem_data = p_stream->shmem_data;
    shmem_buff_header = &shmem_data->buf.in_buf;
    shmem_data_buff = p_stream->shmem_data_buff;

    ALOGV("%s %d shmem_data_buff %p", __func__, __LINE__, shmem_data_buff);

    memscpy(shmem_buff_header, sizeof(qahw_in_buffer_t), in_buf, sizeof(qahw_in_buffer_t));
    shmem_buff_header->bytes = in_buf->bytes;
    shmem_buff_header->offset = in_buf->offset;

    shmem_data->cmd = READ;

    // is the server thread waiting for commands
    ALOGD("Taking command mutex %zd", &shmem_data->c_mutex);
    rc = pthread_mutex_lock(&shmem_data->c_mutex);

    shmem_data->status = -1;

    //wakeup server stream thread.
    ALOGD("Waking up stream thread(%p:%p)",
        p_stream, p_stream->n_stream_handle);
    rc = pthread_cond_signal(&shmem_data->c_cond);
    ALOGD("Woken up stream thread(%p:%p) %d",
        p_stream, p_stream->n_stream_handle, rc);
    while (shmem_data->status == -1) {
        ALOGD("Waiting for status signal stream(%p:%p)",
            p_stream, p_stream->n_stream_handle);
        rc = pthread_cond_wait(&shmem_data->c_cond, &shmem_data->c_mutex);
        ALOGD("Waiting ended for status signal stream(%p:%p) %d",
            p_stream, p_stream->n_stream_handle, rc);
    }
    rc = pthread_mutex_unlock(&shmem_data->c_mutex);
    ALOGV("%s: %x %x %d", __func__, shmem_buff_header->buffer, in_buf->buffer, in_buf->bytes);
    memscpy(in_buf->buffer, in_buf->bytes, (void *)shmem_data_buff, shmem_buff_header->bytes);

    if (in_buf->timestamp)
        memscpy(timestamp, sizeof(int64_t), shmem_buff_header->timestamp, sizeof(int64_t));

    in_buf->bytes = shmem_buff_header->bytes;
    rc = in_buf->bytes;
    return rc;
exit:
    ALOGE("%s: read failed", __func__);
    return -EINVAL;
}

int qahw_set_parameters(qahw_module_handle_t *hw_module, const char *kv_pairs) {
    Parcel data, reply;
    ALOGV("%s %d, kvpairs %s", __func__, __LINE__, kv_pairs);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.writeCString(kv_pairs);
    return remote()->transact(SET_PARAMS, data, &reply);
}

int qahw_set_param_data(const qahw_module_handle_t *hw_module,
                        qahw_param_id param_id,
                        qahw_param_payload *payload) {
    Parcel data, reply;
    ALOGV("%s %d, param_id %d", __func__, __LINE__, param_id);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&param_id, sizeof(qahw_param_id));

    uint32_t param_size = sizeof(qahw_param_payload);
    data.writeUint32(param_size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(param_size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, param_size);
    memscpy(blob.data(), param_size, payload, param_size);

    int rc = remote()->transact(SET_PARAM_DATA, data, &reply);
    blob.release();
    return rc;
}

int qahw_get_param_data(const qahw_module_handle_t *hw_module,
                        qahw_param_id param_id,
                        qahw_param_payload *payload) {
    Parcel data, reply;
    ALOGV("%s %d, param_id %d", __func__, __LINE__, param_id);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&param_id, sizeof(qahw_param_id));
    int rc = remote()->transact(GET_PARAM_DATA, data, &reply);

    if (rc != 0)
        return -EINVAL;

    android::Parcel::ReadableBlob blob;
    uint32_t blob_size;
    reply.readUint32(&blob_size);
    status = reply.readBlob(blob_size, &blob);
    if (status) {
        ALOGE("%s: Error in readblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(payload, 0x0, sizeof(qahw_param_payload));
    memscpy(payload, sizeof(qahw_param_payload), blob.data(), blob_size);
    blob.release();
    return rc;
}

size_t qahw_out_get_buffer_size(const qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(OUT_GET_BUFF_SIZE, data, &reply);
    return (size_t)(reply.readUint32());
}

audio_channel_mask_t qahw_out_get_channels(
                    const qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(OUT_GET_CHANNELS, data, &reply);
    return (audio_channel_mask_t)reply.readUint32();
}

audio_format_t qahw_out_get_format(const qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(OUT_GET_FORMAT, data, &reply);
    return (audio_format_t)reply.readUint32();
}

uint32_t qahw_out_get_sample_rate(const qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(OUT_GET_SAMPLE_RATE, data, &reply);
    return reply.readUint32();
}

int qahw_out_set_sample_rate(qahw_stream_handle_t *out_handle,
                                uint32_t rate) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeUint32(rate);
    return remote()->transact(OUT_SET_SAMPLE_RATE, data, &reply);
}

int qahw_out_set_parameters(qahw_stream_handle_t *out_handle,
                            const char *kv_pairs) {
    Parcel data, reply;
    ALOGV("%s %d, kvpairs %s", __func__, __LINE__, kv_pairs);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeCString(kv_pairs);
    return remote()->transact(OUT_SET_PARAMS, data, &reply);
}

char *qahw_out_get_parameters(const qahw_stream_handle_t *out_handle,
                               const char *keys) {
    Parcel data, reply;
    ALOGV("%s %d, keys %s", __func__, __LINE__, keys);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeCString(keys);
    remote()->transact(OUT_GET_PARAMS, data, &reply);
    return (char *)reply.readCString();
}

int qahw_out_set_param_data(qahw_stream_handle_t *out_handle,
                        qahw_param_id param_id,
                        qahw_param_payload *payload) {
    Parcel data, reply;
    ALOGV("%s %d, param_id %d", __func__, __LINE__, param_id);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.write(&param_id, sizeof(qahw_param_id));

    uint32_t param_size = sizeof(qahw_param_payload);
    data.writeUint32(param_size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(param_size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, param_size);
    memscpy(blob.data(), param_size, payload, param_size);

    remote()->transact(OUT_SET_PARAM_DATA, data, &reply);
    blob.release();
    return reply.readUint32();
}

int qahw_out_get_param_data(qahw_stream_handle_t *out_handle,
                        qahw_param_id param_id,
                        qahw_param_payload *payload) {
    Parcel data, reply;
    ALOGV("%s %d, param_id %d", __func__, __LINE__, param_id);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;

    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.write(&param_id, sizeof(qahw_param_id));
    int rc = remote()->transact(OUT_GET_PARAM_DATA, data, &reply);

    if (rc != 0)
        return -EINVAL;

    android::Parcel::ReadableBlob blob;
    uint32_t blob_size;
    reply.readUint32(&blob_size);
    status_t status = reply.readBlob(blob_size, &blob);
    if (status) {
        ALOGE("%s: Error in readblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(payload, 0x0, sizeof(qahw_param_payload));
    memscpy(payload, sizeof(qahw_param_payload), blob.data(), blob_size);
    blob.release();
    return reply.readInt32();
}

int qahw_out_set_volume(qahw_stream_handle_t *out_handle,
                            float left, float right) {
    Parcel data, reply;
    ALOGV("%s %d, left %f, right %f", __func__, __LINE__, left, right);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeFloat(left);
    data.writeFloat(right);
    return remote()->transact(OUT_SET_VOLUME, data, &reply);
}

int qahw_out_set_callback(qahw_stream_handle_t *out_handle,
                              qahw_stream_callback_t callback,
                              void *cookie) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));

    sp<IBinder> binder = new QTIStreamOutCallback();
    android::ProcessState::self()->startThreadPool();
    sp<IStreamOutCallback> cb = interface_cast<IStreamOutCallback>(binder);

    data.writeStrongBinder(IInterface::asBinder(cb));
    data.write(&callback, sizeof(qahw_stream_callback_t *));// cookie1
    data.write(&cookie, sizeof(qahw_stream_callback_t *));// cookie2

    return remote()->transact(OUT_SET_CALLBACK, data, &reply);
}

int qahw_out_get_render_position(const qahw_stream_handle_t *out_handle,
                                    uint32_t *dsp_frames) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));

    remote()->transact(OUT_GET_RENDER_POS, data, &reply);
    *dsp_frames = reply.readUint32();
    return reply.readUint32();
}

int qahw_out_pause(qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    return remote()->transact(OUT_PAUSE, data, &reply);
}

int qahw_out_resume(qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    return remote()->transact(OUT_RESUME, data, &reply);
}

int qahw_out_flush(qahw_stream_handle_t *out_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)out_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    return remote()->transact(OUT_FLUSH, data, &reply);
}

uint32_t qahw_in_get_sample_rate(const qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(IN_GET_SAMPLE_RATE, data, &reply);
    return reply.readUint32();
}

int qahw_in_set_sample_rate(qahw_stream_handle_t *in_handle, uint32_t rate) {
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeUint32(rate);
    ALOGV("%s %d, rate %d", __func__, __LINE__, rate);
    return remote()->transact(IN_SET_SAMPLE_RATE, data, &reply);
}

size_t qahw_in_get_buffer_size(const qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(IN_GET_BUFF_SIZE, data, &reply);
    return (size_t)(reply.readUint32());
}

audio_channel_mask_t qahw_in_get_channels(const qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(IN_GET_CHANNELS, data, &reply);
    return (audio_channel_mask_t)(reply.readUint32());
}

audio_format_t qahw_in_get_format(const qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(IN_GET_FORMAT, data, &reply);
    return (audio_format_t)reply.readUint32();
}

int qahw_in_set_format(qahw_stream_handle_t *in_handle, audio_format_t format) {
    Parcel data, reply;
    ALOGV("%s %d, format %d", __func__, __LINE__, format);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.write(&format, sizeof(audio_format_t));
    return remote()->transact(IN_SET_FORMAT, data, &reply);
}

int qahw_in_standby(qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    return remote()->transact(IN_STANDBY, data, &reply);
}

int qahw_in_set_parameters(qahw_stream_handle_t *in_handle, const char *kv_pairs) {
    Parcel data, reply;
    ALOGV("%s %d, kvpairs %s", __func__, __LINE__, kv_pairs);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeCString(kv_pairs);
    return remote()->transact(IN_SET_PARAMS, data, &reply);
}

char* qahw_in_get_parameters(const qahw_stream_handle_t *in_handle,
                              const char *keys) {
    Parcel data, reply;
    ALOGV("%s %d, keys %s", __func__, __LINE__, keys);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeCString(keys);
    remote()->transact(IN_GET_PARAMS, data, &reply);
    return (char *)reply.readCString();
}

uint32_t qahw_in_get_input_frames_lost(qahw_stream_handle_t *in_handle) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    remote()->transact(GET_INPUT_FRAMES_LOST, data, &reply);
    return reply.readUint32();
}

int qahw_in_get_capture_position(const qahw_stream_handle_t *in_handle,
                                int64_t *frames, int64_t *time) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    p_stream_handle *p_stream = (p_stream_handle *)in_handle;
    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeInt64((int64_t)frames);
    data.writeInt64((int64_t)time);
    return remote()->transact(GET_CAPTURE_POS, data, &reply);
}

int qahw_init_check(const qahw_module_handle_t *hw_module) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    return remote()->transact(INIT_CHECK, data, &reply);
}

int qahw_set_voice_volume(qahw_module_handle_t *hw_module, float volume) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.writeFloat(volume);
    return remote()->transact(SET_VOICE_VOL, data, &reply);
}

int qahw_set_mode(qahw_module_handle_t *hw_module, audio_mode_t mode) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&mode, sizeof(audio_mode_t));
    return remote()->transact(SET_MODE, data, &reply);
}

int qahw_set_mic_mute(qahw_module_handle_t *hw_module, bool state) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&state, sizeof(bool));
    return remote()->transact(SET_MIC_MUTE, data, &reply);
}

int qahw_get_mic_mute(qahw_module_handle_t *hw_module, bool *state) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&state, sizeof(bool));
    return remote()->transact(GET_MIC_MUTE, data, &reply);
}

char* qahw_get_parameters(const qahw_module_handle_t *hw_module,
                               const char *keys) {
    Parcel data, reply;
    ALOGV("%s %d, keys %s", __func__, __LINE__, keys);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.writeCString(keys);
    remote()->transact(GET_PARAMS, data, &reply);
    return (char *)reply.readCString();
}

size_t qahw_get_input_buffer_size(const qahw_module_handle_t *hw_module,
                                  const struct audio_config *config) {
    Parcel data, reply;
    ALOGV("%s %d", __func__, __LINE__);
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));

    int rc = remote()->transact(INPUT_BUFF_SIZE, data, &reply);
    if (rc != 0)
        return -EINVAL;

    android::Parcel::ReadableBlob blob;
    uint32_t blob_size;
    reply.readUint32(&blob_size);
    status_t status = reply.readBlob(blob_size, &blob);
    if (status) {
        ALOGE("%s: Error in readblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset((void *)config, 0x0, sizeof(audio_config));
    memscpy((void *)config, sizeof(audio_config), blob.data(), blob_size);
    blob.release();
    return reply.readInt32();
}

qahw_effect_lib_handle_t qahw_effect_load_library(const char *lib_name) {
    Parcel data, reply;
    qahw_effect_lib_handle_t handle;
    ALOGV("%s %d, lib_name %s", __func__, __LINE__, lib_name);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.writeCString(lib_name);
    status_t status = remote()->transact(EFFECT_LOAD_LIBRARY, data, &reply);
    if (status)
        return NULL;
    reply.read(&handle, sizeof(qahw_effect_lib_handle_t));

    return handle;
}

int32_t qahw_effect_unload_library(qahw_effect_lib_handle_t handle) {
    Parcel data, reply;
    ALOGV("%s %d, handle %p", __func__, __LINE__, handle);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&handle, sizeof(qahw_effect_lib_handle_t));
    status_t status = remote()->transact(EFFECT_UNLOAD_LIBRARY, data, &reply);
    if (status)
        return (int32_t)status;
    return reply.readInt32();
}

int32_t qahw_effect_create(qahw_effect_lib_handle_t handle,
                                   const qahw_effect_uuid_t *uuid,
                                   int32_t io_handle,
                                   qahw_effect_handle_t *effect_handle) {
    Parcel data, reply;
    ALOGV("%s %d, handle %p", __func__, __LINE__, handle);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&handle, sizeof(qahw_effect_lib_handle_t));
    data.write(uuid, sizeof(qahw_effect_uuid_t));
    data.writeInt32(io_handle);
    status_t status = remote()->transact(EFFECT_CREATE, data, &reply);
    if (status)
        return (int32_t)status;

    reply.read(effect_handle, sizeof(qahw_effect_handle_t));
    return reply.readInt32();
}

int32_t qahw_effect_release(qahw_effect_lib_handle_t handle,
                            qahw_effect_handle_t effect_handle) {
    Parcel data, reply;
    ALOGV("%s %d, handle %p", __func__, __LINE__, handle);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&handle, sizeof(qahw_effect_lib_handle_t));
    data.write(&effect_handle, sizeof(qahw_effect_handle_t));
    status_t status = remote()->transact(EFFECT_RELEASE, data, &reply);
    if (status)
        return (int32_t)status;

    return reply.readInt32();
}

int32_t qahw_effect_get_descriptor(qahw_effect_lib_handle_t handle,
                                   const qahw_effect_uuid_t *uuid,
                                   qahw_effect_descriptor_t *effect_desc) {
    Parcel data, reply;
    ALOGV("%s %d, handle %p", __func__, __LINE__, handle);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&handle, sizeof(qahw_effect_lib_handle_t));
    data.write(uuid, sizeof(qahw_effect_uuid_t));
    status_t status = remote()->transact(EFFECT_GET_DESCRIPTOR, data, &reply);
    if (status)
        return (int32_t)status;

    reply.read(effect_desc, sizeof(qahw_effect_descriptor_t));
    return reply.readInt32();
}

int qahw_effect_get_version() {
    Parcel data, reply;
    return remote()->transact(EFFECT_GET_VERSION, data, &reply);
}

int32_t qahw_effect_process(qahw_effect_handle_t self,
                            qahw_audio_buffer_t *in_buffer,
                            qahw_audio_buffer_t *out_buffer) {
    ALOGV("%s %d, handle %p", __func__, __LINE__, self);
    /* TODO - implement effect_process */
    return -ENOSYS;
}

int32_t qahw_effect_command(qahw_effect_handle_t self,
                                    uint32_t cmd_code,
                                    uint32_t cmd_size,
                                    void *cmd_data,
                                    uint32_t *reply_size,
                                    void *reply_data) {
    Parcel data, reply;
    android::Parcel::WritableBlob w_blob;
    android::Parcel::ReadableBlob r_blob;

    ALOGV("%s %d, handle %p", __func__, __LINE__, self);

    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&self, sizeof(qahw_effect_lib_handle_t));
    data.writeUint32(cmd_code);
    data.writeUint32(cmd_size);

    status_t status = data.writeBlob(cmd_size, false, &w_blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(w_blob.data(), 0x0, cmd_size);
    memscpy(w_blob.data(), cmd_size, cmd_data, cmd_size);

    data.write(reply_size, sizeof(*reply_size));
    status = remote()->transact(EFFECT_COMMAND, data, &reply);
    if (status) {
        w_blob.release();
        return (int32_t)status;
    }

    status = reply.readBlob(*reply_size, &r_blob);
    if (status) {
        ALOGE("%s: Error in readblob, status %d", __func__, status);
        w_blob.release();
        return -EINVAL;
    }
    memscpy(reply_data, *reply_size, r_blob.data(), *reply_size);
    w_blob.release();
    r_blob.release();
    return reply.readInt32();
}

int32_t qahw_effect_process_reverse(qahw_effect_handle_t self,
                            qahw_audio_buffer_t *in_buffer,
                            qahw_audio_buffer_t *out_buffer) {
    ALOGV("%s %d, handle %p", __func__, __LINE__, self);
    /* TODO - implement effect_reverse_process */
    return -ENOSYS;
}

int qahw_create_audio_patch(qahw_module_handle_t *hw_module,
                        unsigned int num_sources,
                        const struct audio_port_config *sources,
                        unsigned int num_sinks,
                        const struct audio_port_config *sinks,
                        audio_patch_handle_t *handle) {
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.writeUint32(num_sources);

    uint32_t size = sizeof(audio_port_config);
    data.writeUint32(size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, size);
    memcpy(blob.data(), sources, size);

    data.writeUint32(num_sinks);

    data.writeUint32(size);
    status = data.writeBlob(size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, size);
    memcpy(blob.data(), sinks, size);

    data.write(&handle, sizeof(audio_patch_handle_t));

    return remote()->transact(CREATE_AUD_PATCH, data, &reply);
}

int qahw_release_audio_patch(qahw_module_handle_t *hw_module,
                        audio_patch_handle_t handle) {
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    data.write(&handle, sizeof(audio_patch_handle_t));
    return remote()->transact(RELEASE_AUD_PATCH, data, &reply);
}

int qahw_get_audio_port(qahw_module_handle_t *hw_module,
                      struct audio_port *port) {
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    uint32_t size = sizeof(audio_port);
    data.writeUint32(size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, size);
    memscpy(blob.data(), size, port, size);
    int rc = remote()->transact(GET_AUD_PORT, data, &reply);
    if (rc != 0) {
        blob.release();
        return -EINVAL;
    }
    android::Parcel::ReadableBlob read_blob;
    uint32_t blob_size;
    reply.readUint32(&blob_size);
    status = reply.readBlob(blob_size, &read_blob);
    if (status) {
        ALOGE("%s: Error in readblob, status %d", __func__, status);
        blob.release();
        return -EINVAL;
    }
    memset((void *)port, 0x0, sizeof(audio_port));
    memscpy((void *)port, blob_size, read_blob.data(), blob_size);
    blob.release();
    read_blob.release();
    return reply.readInt32();
}

int qahw_set_audio_port_config(qahw_module_handle_t *hw_module,
                     const struct audio_port_config *config) {
    Parcel data, reply;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());
    data.write(&hw_module, sizeof(qahw_module_handle_t *));
    uint32_t size = sizeof(audio_port_config);
    data.writeUint32(size);
    android::Parcel::WritableBlob blob;
    status_t status = data.writeBlob(size, false, &blob);
    if (status) {
        ALOGE("%s: Error in writeblob, status %d", __func__, status);
        return -EINVAL;
    }
    memset(blob.data(), 0x0, size);
    memcpy(blob.data(), config, size);
    return remote()->transact(SET_AUD_PORT_CFG, data, &reply);
}

int qahw_get_version() {
    Parcel data, reply;
    return remote()->transact(GET_VERSION, data, &reply);
}

private:

int init_ipc_primitives(sh_mem_data *shmem_data) {
    int rc = 0;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    rc = pthread_mutex_init(&shmem_data->c_mutex, &attr);
    if (rc != 0)
        goto exit;

    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
    rc = pthread_cond_init(&shmem_data->c_cond, &cattr);
    if (rc != 0)
        goto exit;

    ALOGD("IPC primitives inited");
    return rc;

exit:
    ALOGE("Unable to setup IPC primitives");
    return -EINVAL;
}

int prepare_for_write(qahw_stream_handle_t *stream,
                qahw_out_buffer_t * buff) {
    p_stream_handle *p_stream = (p_stream_handle *)stream;
    int rc = 0;
    Parcel data, reply;
    sh_mem_data *ptr = NULL;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    //allocate shared mem
    int shmem_data_buff_size = qahw_out_get_buffer_size(stream);
    int total_sh_mem = sizeof(sh_mem_data) + shmem_data_buff_size;

    ALOGD("Total shared mem needed %d", total_sh_mem);

    p_stream->sh_mem_dealer = new MemoryDealer(total_sh_mem, "qas");
    if (p_stream->sh_mem_dealer == NULL)
        goto exit;
    p_stream->sh_mem_handle =
        p_stream->sh_mem_dealer->allocate(total_sh_mem);
    if (0 == p_stream->sh_mem_handle->size())
        goto exit;

    ALOGV("Allocated shared mem %p size %zu",
                p_stream->sh_mem_handle->pointer(),
                p_stream->sh_mem_handle->size());

    p_stream->shmem_data =static_cast<sh_mem_data*>
        (p_stream->sh_mem_handle->pointer());

    rc = init_ipc_primitives(p_stream->shmem_data);
    if (rc != 0)
        goto exit;

    ptr = p_stream->shmem_data;
    ptr->buf.out_buf.bytes = shmem_data_buff_size;
    p_stream->shmem_data_buff = (void*)++ptr;
    ALOGV("shmem_data buff header(%p) buff(%p)buff size %zu",
                &p_stream->shmem_data->buf,
                p_stream->shmem_data_buff,
                sizeof(sh_mem_data));

    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeStrongBinder(IInterface::asBinder
            (p_stream->sh_mem_handle));
    remote()->transact(PREPARE_FOR_WRITE, data, &reply);

    rc = reply.readUint32();
    p_stream->prepare_done = true;
    ALOGV("%s done %d", __func__, rc);
    return rc;
exit:
    ALOGE("Unable to prepare for write");
    return -EINVAL;
}

int prepare_for_read(qahw_stream_handle_t *stream, qahw_in_buffer_t * buff) {
    p_stream_handle *p_stream = (p_stream_handle *)stream;
    int rc = 0;
    Parcel data, reply;
    sh_mem_data *ptr = NULL;
    data.writeInterfaceToken(Iqti_audio_server::getInterfaceDescriptor());

    //allocate shared mem
    size_t shmem_data_buff_size = qahw_in_get_buffer_size(stream);
    int total_sh_mem = sizeof(sh_mem_data) + shmem_data_buff_size;

    ALOGD("%s: Total shared mem needed %d", __func__, total_sh_mem);

    p_stream->sh_mem_dealer = new MemoryDealer(total_sh_mem, "qas");
    if (p_stream->sh_mem_dealer == NULL)
        goto exit;
    p_stream->sh_mem_handle =
        p_stream->sh_mem_dealer->allocate(total_sh_mem);
    if (0 == p_stream->sh_mem_handle->size())
        goto exit;

    ALOGV("%s: Allocated shared mem %p size %zu", __func__,
                p_stream->sh_mem_handle->pointer(),
                p_stream->sh_mem_handle->size());

    p_stream->shmem_data =static_cast<sh_mem_data*>
            (p_stream->sh_mem_handle->pointer());

    rc = init_ipc_primitives(p_stream->shmem_data);
    if (rc != 0)
        goto exit;

    ptr = p_stream->shmem_data;
    ptr->buf.in_buf.bytes = shmem_data_buff_size;
    p_stream->shmem_data_buff = (void*)++ptr;
    ALOGV("%s: shmem_data buff header(%p) buff(%p)buff size %zu",
                __func__, &p_stream->shmem_data->buf.in_buf,
                p_stream->shmem_data_buff,
                sizeof(sh_mem_data));

    data.write(&p_stream->n_stream_handle, sizeof(qahw_stream_handle_t *));
    data.writeStrongBinder(IInterface::asBinder
                (p_stream->sh_mem_handle));
    remote()->transact(PREPARE_FOR_READ, data, &reply);

    rc = reply.readInt32();
    p_stream->prepare_done = true;
    ALOGV("%s done %d", __func__, rc);
    return rc;
exit:
    ALOGE("%s: Unable to prepare for read", __func__);
    return -EINVAL;
}
};

const android::String16 Iqti_audio_server::descriptor("Iqti_audio_server");
const android::String16& Iqti_audio_server::getInterfaceDescriptor() const {
    return Iqti_audio_server::descriptor;
}

android::sp<Iqti_audio_server> Iqti_audio_server::asInterface
    (const android::sp<android::IBinder>& obj) {
    ALOGD("Iqti_audio_server::asInterface()");
    android::sp<Iqti_audio_server> intr;
    if (obj != NULL) {
        intr = static_cast<Iqti_audio_server*>(obj->queryLocalInterface(Iqti_audio_server::descriptor).get());
        ALOGD("Iqti_audio_server::asInterface() interface %s",
            ((intr == 0)?"zero":"non zero"));

        if (intr == NULL) {
            intr = new qti_audio_proxy(obj);
        }
    }
    return intr;
}

Iqti_audio_server::Iqti_audio_server()
    { ALOGD("Iqti_audio_server::Iqti_audio_server()"); }
Iqti_audio_server::~Iqti_audio_server()
    { ALOGD("Iqti_audio_server::~Iqti_audio_server()"); }


}; // namespace audiohal

