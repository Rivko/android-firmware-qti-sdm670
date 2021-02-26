////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Not a Contribution
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Device.cpp
/// @brief camera3_device implementation
///        provide camera device to QCameraHAL3Test
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraHAL3Device.h"

#define LOG_TAG "CameraTest"
#include <log/log.h>
#include <vector>
#include "BufferManager.h"
#include <string>
#include <signal.h>
#include "QCameraHAL3Test.h"
using namespace android;
static const int   kMinJpegBufferSize = 256 * 1024 + sizeof(camera3_jpeg_blob);

#define CAMX_LIVING_REQUEST_MAX 5
/************************************************************************
* name : QCameraHAL3Device
* function: construct object.
************************************************************************/
QCameraHAL3Device::QCameraHAL3Device(
    camera_module_t* module, int cameraId, int mode) :
    mModule(module),
    mCameraId(cameraId){
    pthread_mutex_init(&mSettingMetaLock,NULL);
    mRequestThread = NULL;
    mResultThread = NULL;
    memset(&mStreamConfig,0,sizeof(camera3_stream_configuration_t));

    pthread_condattr_t attr;
    pthread_mutex_init(&mPendingLock,NULL);
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&mPendingCond, &attr);
    pthread_condattr_destroy(&attr);
    struct camera_info info;
    mModule->get_camera_info(mCameraId, &info);
    mCharacteristics = (camera_metadata_t *)info.static_camera_characteristics;
}

/************************************************************************
* name : ~QCameraHAL3Device
* function: destory object.
************************************************************************/
QCameraHAL3Device::~QCameraHAL3Device()
{
    pthread_mutex_destroy(&mSettingMetaLock);
    pthread_mutex_destroy(&mPendingLock);
    pthread_cond_destroy(&mPendingCond);
}

/************************************************************************
* name : setCallBack
* function: set callback for upper layer.
************************************************************************/
void QCameraHAL3Device::setCallBack(DeviceCallback* callback)
{
    mCallback = callback;
}
/************************************************************************
* name : findStream
* function: find stream index based on stream pointer.
************************************************************************/
int QCameraHAL3Device::findStream(camera3_stream_t* stream)
{

    for (int i = 0;i< (int)mStreams.size();i++) {
        if (mStreams[i] == stream) {
            return i;
        }
    }
    return -1;
}

/************************************************************************
* name : setCurrentMeta
* function: Set A External metadata as current metadata.
************************************************************************/
void QCameraHAL3Device::setCurrentMeta(android::CameraMetadata* meta)
{
    pthread_mutex_lock(&mSettingMetaLock);
    mCurrentMeta = *meta;
    mSettingMetaQ.push_back(mCurrentMeta);
    pthread_mutex_unlock(&mSettingMetaLock);

}
/************************************************************************
 * name : doCapturePostProcess
 * function: Thread for Post process capture result
 ************************************************************************/
void* doCapturePostProcess(void* data)
{
    CameraThreadData* thData = (CameraThreadData*) data;
    QCameraHAL3Device* device = (QCameraHAL3Device*) thData->device;
    ALOGI("capture result handle thread start\n");
    while (true) {
        pthread_mutex_lock(&thData->mutex);
        if (thData->msgQueue.empty()) {
            struct timespec tv;
            clock_gettime(CLOCK_MONOTONIC,&tv);
            tv.tv_sec += 5;
            if(pthread_cond_timedwait(&thData->cond, &thData->mutex,&tv) != 0) {
                ALOGI("No Msg got in 5 sec in Result Process thread");
                pthread_mutex_unlock(&thData->mutex);
                continue;
            }
        }
        CameraPostProcessMsg* msg = (CameraPostProcessMsg*)thData->msgQueue.front();
        thData->msgQueue.pop_front();
        // stop command
        if (msg->stop == 1) {
            delete msg;
            msg = NULL;
            pthread_mutex_unlock(&thData->mutex);
            return nullptr;
        }
        pthread_mutex_unlock(&thData->mutex);
        camera3_capture_result result = msg->result;
        const camera3_stream_buffer_t* buffers = result.output_buffers = msg->streamBuffers.data();
        device->mCallback->CapturePostProcess(device->mCallback,&result);
        // put the buffer back
        if (device->getPutBufferExt() != PUT_BUFFER_EXTERNAL) {
            for (int i = 0;i < result.num_output_buffers ;i++) {
                int index = device->findStream(buffers[i].stream);
                CameraStream* stream = device->mCameraStreams[index];
                stream->bufferManager->PutBuffer(buffers[i].buffer);
            }
        }
        msg->streamBuffers.erase(msg->streamBuffers.begin(),
            msg->streamBuffers.begin() + msg->streamBuffers.size());
        delete msg;
        msg = NULL;
    }
    return nullptr;
}

/************************************************************************
* name : ProcessCaptureResult
* function: callback for process capture result.
************************************************************************/
void QCameraHAL3Device::CallbackOps::ProcessCaptureResult(const camera3_callback_ops *cb,const camera3_capture_result *result)
{
    CallbackOps*  cbOps = (CallbackOps*)cb;
    const camera3_stream_buffer_t* buffers = result->output_buffers;
    CameraStream* stream = NULL;
    int index = -1;

    if (result->partial_result >=1) {
        // handle the metadata callback
        cbOps->mParent->mCallback->HandleMetaData(cbOps->mParent->mCallback, (camera3_capture_result *)result);
    }

    if (result->num_output_buffers > 0) {
        pthread_mutex_lock(&cbOps->mParent->mResultThread->mutex);
        if (!cbOps->mParent->mResultThread->stopped) {
            CameraPostProcessMsg* msg = new CameraPostProcessMsg();
            msg->result = *(result);
            for (int i = 0;i < result->num_output_buffers;i++) {
                msg->streamBuffers.push_back(result->output_buffers[i]);
            }
            cbOps->mParent->mResultThread->msgQueue.push_back(msg);
            pthread_cond_signal(&cbOps->mParent->mResultThread->cond);
        }
        pthread_mutex_unlock(&cbOps->mParent->mResultThread->mutex);
    }
    pthread_mutex_lock(&cbOps->mParent->mPendingLock);
    index = cbOps->mParent->mPendingVector.indexOfKey(result->frame_number);
    RequestPending* pend = cbOps->mParent->mPendingVector.editValueAt(index);
    pend->mNumOutputbuffer += result->num_output_buffers;
    pend->mNumMetadata += result->partial_result;
    if (pend->mNumOutputbuffer >= pend->mRequest.num_output_buffers && pend->mNumMetadata) {
        cbOps->mParent->mPendingVector.removeItemsAt(index, 1);
        pthread_cond_signal(&cbOps->mParent->mPendingCond);
        delete pend;
        pend = NULL;
    }
    pthread_mutex_unlock(&cbOps->mParent->mPendingLock);

}

/************************************************************************
* name : Notify
* function: TODO.
************************************************************************/
void QCameraHAL3Device::CallbackOps::Notify(const struct camera3_callback_ops *cb,
    const camera3_notify_msg_t *msg)
{

}

/************************************************************************
* name : openCamera
* function: open camera device.
************************************************************************/
void QCameraHAL3Device::openCamera()
{
    int res = 0;
    ALOGI("open Camera: %d\n",mCameraId);
    struct camera_info info;
    char camera_name[20] = {0};
    sprintf(camera_name, "%d", mCameraId);
    res = mModule->common.methods->open(&mModule->common, camera_name, (hw_device_t**)(&mDevice));
    if (0 != res) {
        ALOGE("open Camera failed\n");
        return;
    }

    mCBOps = new CallbackOps(this);
    res = mDevice->ops->initialize(mDevice, mCBOps);
    res = mModule->get_camera_info(mCameraId, &info);
    mCharacteristics = (camera_metadata_t *)info.static_camera_characteristics;
}

/************************************************************************
* name : configureStreams
* function: configure stream paramaters.
************************************************************************/
void QCameraHAL3Device::configureStreams(std::vector<camera3_stream_t*> streams, int opMode)
{
    // configure
    int res = 0;

    mStreamConfig.num_streams = streams.size();
    mStreams.resize(mStreamConfig.num_streams);
    for (uint32_t i = 0; i < mStreamConfig.num_streams; i++) {
        camera3_stream_t* stream = new camera3_stream_t();
        stream->stream_type = streams[i]->stream_type;//OUTPUT
        stream->width = streams[i]->width;
        stream->height = streams[i]->height;
        stream->format = streams[i]->format;
        stream->data_space = streams[i]->data_space;
        stream->usage = streams[i]->usage;//GRALLOC1_CONSUMER_USAGE_HWCOMPOSER;
        stream->rotation = streams[i]->rotation;
        stream->max_buffers = streams[i]->max_buffers;
        stream->priv = streams[i]->priv;
        mStreams[i] = stream;

        CameraStream* newStream = new CameraStream();
        mCameraStreams[i] = newStream;
        newStream->streamId = i;
        mCameraStreams[i]->bufferManager = mBufferManager[i];
    }

    mStreamConfig.operation_mode = opMode;
    mStreamConfig.streams = mStreams.data();
    res = mDevice->ops->configure_streams(mDevice, &mStreamConfig);
    if (res != 0) {
        ALOGE("configure_streams Error res:%d\n",res);
        return;
    }
}

void QCameraHAL3Device::PreAllocateStreams(std::vector<camera3_stream_t*> streams)
{
    ALOGI("allocate buffer start\n");
    for (uint32_t i = 0; i < streams.size(); i++) {
        BufferManager* bufferManager = new BufferManager();
        int StreamBufferMax = streams[i]->max_buffers;
        if (streams[i]->format == HAL_PIXEL_FORMAT_BLOB) {
            int size = getJpegBufferSize(streams[i]->width, streams[i]->height);
            bufferManager->AllocateBuffers(StreamBufferMax,
                size,
                1,
                (int32_t)(streams[i]->format),
                streams[i]->usage,streams[i]->usage);
        } else {
            bufferManager->AllocateBuffers(StreamBufferMax,
                streams[i]->width,
                streams[i]->height,
                (int32_t)(streams[i]->format),
                streams[i]->usage,streams[i]->usage);
        }
        // add to HAL3Device's mCameraStreams
        mBufferManager[i] = bufferManager;
    }
    ALOGI("allocate buffer end\n");
}

/************************************************************************
* name : constructDefaultRequestSettings
* function: configure strema default paramaters.
************************************************************************/
void QCameraHAL3Device::constructDefaultRequestSettings(int index,camera3_request_template_t type,
    bool isDefaultMeta)
{
    // construct default
    mCameraStreams[index]->metadata =
        (camera_metadata *)mDevice->ops->construct_default_request_settings(mDevice, type);
    mCameraStreams[index]->streamType = type;
    if (isDefaultMeta) {
        pthread_mutex_lock(&mSettingMetaLock);
        camera_metadata *meta = clone_camera_metadata(mCameraStreams[index]->metadata);
        mCurrentMeta = meta;
        mCurrentMeta.update(ANDROID_CONTROL_AE_TARGET_FPS_RANGE, mFpsRange, 2);

        uint32_t tag;
        uint8_t pcr = 0;
        sp<VendorTagDescriptor> vTags =
            android::VendorTagDescriptor::getGlobalVendorTagDescriptor();
        CameraMetadata::getTagFromName("org.quic.camera.EarlyPCRenable.EarlyPCRenable",
            vTags.get(), &tag);
        mCurrentMeta.update(tag, &(pcr), 1);
        mSettingMetaQ.push_back(mCurrentMeta);
        pthread_mutex_unlock(&mSettingMetaLock);
    }
}

RequestPending::RequestPending()
{
    mNumOutputbuffer = 0;
    mNumMetadata = 0;
    memset(&mRequest, 0, sizeof(camera3_capture_request_t));
}

/************************************************************************
* name : ProcessOneCaptureRequest
* function: populate one capture request.
************************************************************************/
int QCameraHAL3Device::ProcessOneCaptureRequest(int* requestNumberOfEachStream,int* frameNumber)
{
    pthread_mutex_lock(&mPendingLock);
    if (mPendingVector.size() >= (CAMX_LIVING_REQUEST_MAX + mLivingRequestExtAppend) ) {
        struct timespec tv;
        clock_gettime(CLOCK_MONOTONIC,&tv);
        tv.tv_sec += 5;
        if (pthread_cond_timedwait(&mPendingCond, &mPendingLock,&tv) != 0) {
            ALOGI("ProcessOneCaptureRequest timeout \n");
            pthread_mutex_unlock(&mPendingLock);
            return -1;
        }
    }
    pthread_mutex_unlock(&mPendingLock);

    RequestPending* pend = new RequestPending();
    CameraStream* stream = NULL;
    // Try to get Buffer from bufferManager

    std::vector<camera3_stream_buffer_t> streamBuffers;
    for (int i = 0;i < (int)mStreams.size();i++) {
        if (requestNumberOfEachStream[i] == 0) {
            continue;
        }
        stream = mCameraStreams[i];
        camera3_stream_buffer_t streamBuffer;
        streamBuffer.buffer = (const native_handle_t**)(stream->bufferManager->GetBuffer());

        // make a capture request and send to HAL
        streamBuffer.stream = mStreams[i];
        streamBuffer.status = 0;
        streamBuffer.release_fence = -1;
        streamBuffer.acquire_fence = -1;
        pend->mRequest.num_output_buffers++;
        streamBuffers.push_back(streamBuffer);
    }
    pend->mRequest.frame_number = *frameNumber;
    // using the new metadata if needed
    pend->mRequest.settings = nullptr;
    pthread_mutex_lock(&mSettingMetaLock);
    static android::CameraMetadata setting;
    if (!mSettingMetaQ.empty()) {
        setting = mSettingMetaQ.front();
        mSettingMetaQ.pop_front();
        pend->mRequest.settings = (camera_metadata*) setting.getAndLock();
    } else {
        pend->mRequest.settings =  (camera_metadata*)setting.getAndLock();
    }
    pthread_mutex_unlock(&mSettingMetaLock);
    pend->mRequest.input_buffer = nullptr;
    pend->mRequest.output_buffers = streamBuffers.data();

    pthread_mutex_lock(&mPendingLock);
    mPendingVector.add(*frameNumber,pend);
    pthread_mutex_unlock(&mPendingLock);

    int res = mDevice->ops->process_capture_request(mDevice, &(pend->mRequest));
    if (res != 0) {
        int index = 0;
        ALOGE("wrong process_capture_quest frame:%d",*frameNumber);
        for (int i = 0; i < pend->mRequest.num_output_buffers; i++) {
            index = findStream(streamBuffers[i].stream);
            stream = mCameraStreams[index];
            stream->bufferManager->PutBuffer(streamBuffers[i].buffer);
        }
        pthread_mutex_lock(&mPendingLock);
        index = mPendingVector.indexOfKey(*frameNumber);
        mPendingVector.removeItemsAt(index, 1);
        delete pend;
        pend = NULL;
        pthread_mutex_unlock(&mPendingLock);
    } else {
        (*frameNumber)++;
        if (pend->mRequest.settings != NULL) {
            setting.unlock(pend->mRequest.settings);
        }
    }

    return res;
}

/************************************************************************
* name : doprocessCaptureRequest
* function: Thread for CaptureRequest ,handle the capture request from upper layer
************************************************************************/
void* doprocessCaptureRequest(void* data)
{
    CameraThreadData* thData = (CameraThreadData*) data;
    CameraStream* stream = NULL;
    camera3_stream_buffer_t* streamBuffer = NULL;
    while (!thData->stopped) { //need repeat and has not trigger out until stopped
        QCameraHAL3Device* device = (QCameraHAL3Device*) thData->device;
        pthread_mutex_lock(&thData->mutex);
        bool empty = thData->msgQueue.empty();
        /* Send request till the requestNumber is 0;
        ** Check the msgQueue for Message from other thread to
        ** change setting or stop Capture Request
        */
        bool hasRequest = false;
        for (int i = 0;i < (int)device->mStreams.size();i++) {
            if (thData->requestNumber[i] != 0) {
                hasRequest = true;
                break;
            }
            ALOGI("has no Request :%d\n",i);
        }
        if (empty && !hasRequest) {
            ALOGI("Waiting Msg :%d or Buffer return at thread:%p\n",
                thData->msgQueue.size(),thData);
            struct timespec tv;
            clock_gettime(CLOCK_MONOTONIC,&tv);
            tv.tv_sec += 5;
            if (pthread_cond_timedwait(&thData->cond, &thData->mutex,&tv) != 0) {
                ALOGI("No Msg Got!\n",thData->msgQueue.size(),hasRequest);
            }
            pthread_mutex_unlock(&thData->mutex);
            continue;
        } else if ( !empty ) {
            CameraRequestMsg* msg = (CameraRequestMsg*)thData->msgQueue.front();
            thData->msgQueue.pop_front();
            switch (msg->msgType) {
            case START_STOP: {
                // stop command
                ALOGI("Get Msg for stop request\n");
                if (msg->stop == 1) {
                    thData->stopped = 1;
                    pthread_mutex_unlock(&thData->mutex);
                    delete msg;
                    msg = NULL;
                    continue;
                }
                break;
            }
            case REQUEST_CHANGE: {
                ALOGI("Get Msg for change request\n");
                for (int i = 0;i < device->mStreams.size();i++) {
                    if (msg->mask & (1<<i)) {
                        (thData->requestNumber[i]==-1) ? thData->requestNumber[i] =
                            msg->requestNumber[i] : thData->requestNumber[i] +=
                            msg->requestNumber[i];
                    }
                }
                pthread_mutex_unlock(&thData->mutex);
                delete msg;
                msg = NULL;
                continue;
                break;
            }
            default:
                break;
            }
            delete msg;
            msg = NULL;
        }
        pthread_mutex_unlock(&thData->mutex);

        // request one each time
        int requestNumberOfEachStream[MAXSTREAM] = {0};
        for (int i = 0;i < MAXSTREAM;i++) {
            if (thData->requestNumber[i] != 0) {
                int skip = thData->skipPattern[i];
                if ((thData->frameNumber % skip) == 0) {
                    requestNumberOfEachStream[i] = 1;
                }
            }
        }
        int res = device->ProcessOneCaptureRequest(requestNumberOfEachStream,&(thData->frameNumber));
        if (res != 0) {
            ALOGE("ProcessOneCaptureRequest Error res:%d\n",res);
            return nullptr;
        }
        // reduce request number each time
        pthread_mutex_lock(&thData->mutex);
        for (int i = 0;i < (int)device->mStreams.size();i++) {
            if (thData->requestNumber[i] > 0)
                thData->requestNumber[i] = thData->requestNumber[i] -
                    requestNumberOfEachStream[i];
        }
        pthread_mutex_unlock(&thData->mutex);
    }
    return nullptr;
}

/************************************************************************
* name : processCaptureRequestOn
* function: create request and result thread.
************************************************************************/
int QCameraHAL3Device::processCaptureRequestOn(CameraThreadData* requestThread,
    CameraThreadData* resultThread)
{
    // init result threads
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_mutex_init(&resultThread->mutex,NULL);
    pthread_mutex_init(&requestThread->mutex,NULL);
    pthread_cond_init(&requestThread->cond, &attr);
    pthread_cond_init(&resultThread->cond, &attr);
    pthread_condattr_destroy(&attr);

    pthread_attr_t resultAttr;
    pthread_attr_init(&resultAttr);
    pthread_attr_setdetachstate(&resultAttr, PTHREAD_CREATE_JOINABLE);
    resultThread->device = this;
    pthread_create(&(resultThread->thread),&resultAttr,doCapturePostProcess,resultThread);
    mResultThread = resultThread;
    pthread_attr_destroy(&resultAttr);

    // init request thread
    pthread_attr_t requestAttr;
    pthread_attr_init(&requestAttr);
    pthread_attr_setdetachstate(&requestAttr, PTHREAD_CREATE_JOINABLE);
    requestThread->device = this;
    pthread_create(&(requestThread->thread),&requestAttr,doprocessCaptureRequest,requestThread);
    mRequestThread = requestThread;
    pthread_attr_destroy(&resultAttr);
    return 0;
}

/************************************************************************
* name : flush
* function: flush when stop the stream
************************************************************************/
void QCameraHAL3Device::flush()
{
    mDevice->ops->flush(mDevice);
}

/************************************************************************
* name : stopStreams
* function: stop streams.
************************************************************************/
void QCameraHAL3Device::stopStreams()
{
    // stop the request thread
    pthread_mutex_lock(&mRequestThread->mutex);
    CameraRequestMsg* rqMsg = new CameraRequestMsg();
    rqMsg->msgType = START_STOP;
    rqMsg->stop = 1;
    mRequestThread->msgQueue.push_back(rqMsg);
    ALOGI("Msg for stop request queue size:%d\n",mRequestThread->msgQueue.size());
    pthread_cond_signal(&mRequestThread->cond);
    pthread_mutex_unlock(&mRequestThread->mutex);
    pthread_join(mRequestThread->thread,NULL);
    pthread_mutex_destroy(&mRequestThread->mutex);
    pthread_cond_destroy(&mRequestThread->cond);
    delete mRequestThread;
    mRequestThread = NULL;
    // then flush all the request
    //flush();
    // then stop the result process thread
    pthread_mutex_lock(&mResultThread->mutex);
    CameraPostProcessMsg* msg = new CameraPostProcessMsg();
    msg->stop = 1;
    mResultThread->stopped = 1;
    mResultThread->msgQueue.push_back(msg);
    ALOGI("Msg for stop result queue size:%d\n",mResultThread->msgQueue.size());
    pthread_cond_signal(&mResultThread->cond);
    pthread_mutex_unlock(&mResultThread->mutex);
    pthread_join(mResultThread->thread,NULL);
    // wait for all request back
    pthread_mutex_lock(&mPendingLock);
    int tryCount = 5;
    while (!mPendingVector.isEmpty() && tryCount > 0) {
        ALOGI("wait for pending vector empty:%d\n",mPendingVector.size());
        struct timespec tv;
        clock_gettime(CLOCK_MONOTONIC,&tv);
        tv.tv_sec += 1;
        if (pthread_cond_timedwait(&mPendingCond, &mPendingLock,&tv) != 0) {
            tryCount--;
        }
        continue;
    }
    if (!mPendingVector.isEmpty()) {
        ALOGE("ERROR: request pending vector not empty after stop frame:%d !!\n",
            mPendingVector.keyAt(0));
        mPendingVector.clear();
    }
    pthread_mutex_unlock(&mPendingLock);

    pthread_mutex_destroy(&mResultThread->mutex);
    pthread_cond_destroy(&mResultThread->cond);
    delete mResultThread;
    mResultThread = NULL;
    int size = (int)mStreams.size();
    mCurrentMeta.clear();
    for (int i = 0;i< size;i++) {
        delete mStreams[i];
        mStreams[i] = NULL;
        delete mCameraStreams[i]->bufferManager;
        mCameraStreams[i]->bufferManager = NULL;
        delete mCameraStreams[i];
        mCameraStreams[i] = NULL;
    }
    mStreams.erase(mStreams.begin(),mStreams.begin() + mStreams.size());
    memset(&mStreamConfig,0,sizeof(camera3_stream_configuration_t));
}
/************************************************************************
* name : closeCamera
* function: close the camera
************************************************************************/
void QCameraHAL3Device::closeCamera()
{
    mDevice->common.close(&mDevice->common);
    delete mCBOps;
    mCBOps = NULL;
}

/************************************************************************
* name : getJpegBufferSize
* function: private function for getting the jpeg buffer size
************************************************************************/
int QCameraHAL3Device::getJpegBufferSize(uint32_t width, uint32_t height)
{

    int res = 0;
    int maxJpegBufferSize = 0;
    // Get max jpeg buffer size
    camera_metadata_ro_entry jpegBufMaxSize;
    res = find_camera_metadata_ro_entry(mCharacteristics,
        ANDROID_JPEG_MAX_SIZE, &jpegBufMaxSize);
    if (jpegBufMaxSize.count == 0) {
        ALOGE("%s: Camera %d: Can't find maximum JPEG size in static metadata!",
            __FUNCTION__, mCameraId);
        return 0;
    }
    maxJpegBufferSize = jpegBufMaxSize.data.i32[0];

    // Calculate final jpeg buffer size for the given resolution.
    float scaleFactor = (1.0f * width * height) /
        ((maxJpegBufferSize - sizeof(camera3_jpeg_blob))/3.0f);
    int jpegBufferSize = scaleFactor * (maxJpegBufferSize - kMinJpegBufferSize) +
        kMinJpegBufferSize;

    return jpegBufferSize;
}

/************************************************************************
* name : updateMetadataForNextRequest
* function: used for new metadata change request from user
************************************************************************/
int QCameraHAL3Device::updateMetadataForNextRequest(android::CameraMetadata* meta)
{
    pthread_mutex_lock(&mSettingMetaLock);
    android::CameraMetadata _meta(*meta);
    mSettingMetaQ.push_back(_meta);
    pthread_mutex_unlock(&mSettingMetaLock);
    return 0;
}

/************************************************************************
* name : getAvailableOutputStreams
* function: Retrieve all valid output stream resolutions from the camera static characteristics.
************************************************************************/
int QCameraHAL3Device::getAvailableOutputStreams(std::vector<AvailableStream> &outputStreams,
    const AvailableStream *threshold)
{
    if (nullptr == mCharacteristics) {
        return -1;
    }

    camera_metadata_ro_entry entry;
    int rc = find_camera_metadata_ro_entry(mCharacteristics,
        ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry);
    if ((0 != rc) || (0 != (entry.count % 4))) {
        return -1;
    }

    for (size_t i = 0; i < entry.count; i+=4) {
        if (ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT ==
            entry.data.i32[i + 3]) {
            if (nullptr == threshold) {
                AvailableStream s = {entry.data.i32[i+1],
                    entry.data.i32[i+2], entry.data.i32[i]};
                outputStreams.push_back(s);
            } else {
                if ((threshold->format == entry.data.i32[i]) &&
                    (threshold->width == entry.data.i32[i+1]) &&
                    (threshold->height == entry.data.i32[i+2])) {
                    AvailableStream s = {entry.data.i32[i+1],
                        entry.data.i32[i+2], threshold->format};
                    outputStreams.push_back(s);
                }
            }
            ALOGD("format:%d %dx%d\n",entry.data.i32[i],entry.data.i32[i+1],entry.data.i32[i+2]);
        }

    }
    return 0;
}
