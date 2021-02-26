////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraTestVideoEncoder.cpp
/// @brief middle object between omx encoder and video test case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraTestVideoEncoder.h"

using namespace android;
/************************************************************************
* name : QCameraTestVideoEncoder
* function: init default setting
************************************************************************/
QCameraTestVideoEncoder::QCameraTestVideoEncoder(TestConfig *config)
    :mCoder(NULL),
    mConfig({}),
    mBufferQueue(NULL)
{
    mCoder =  QCameraOMX_Encoder::getInstance();
    mConfig = {
        .componentName = (char *)"OMX.qcom.video.encoder.avc",
        .inputcolorfmt = HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS,
        /*
         * Support fmt: 0x7fa30c06 HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC
         * Support fmt: 0x7fa30c04 HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS
         * Support fmt: 0x7fa30c00 HAL_PIXEL_FORMAT_NV21_ENCODEABLE or OMX_QCOM_COLOR_FormatYVU420SemiPlanar or QOMX_COLOR_FormatYVU420SemiPlanar
         * Support fmt: 0x7fa30c09 HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC
         * Support fmt: 0x7fa30c0a HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS
         * Support fmt: 0x7fa30c08
         * Support fmt: 0x7fa30c07 QOMX_VIDEO_CodingMVC
         * Support fmt: 0x7f000789 OMX_COLOR_FormatAndroidOpaque
         * Support fmt: 0x15
         */
        .codec = OMX_VIDEO_CodingAVC,
        /*avc config*/
        .npframe = 29,
        .nbframes = 0,
        .eprofile = OMX_VIDEO_AVCProfileBaseline,
        .elevel = OMX_VIDEO_AVCLevel3,
        .bcabac = 0,
        .nframerate = 30,

        /*buf config*/
        .storemeta = (DISABLE_META_MODE==1)?0:1,

        /*input port param*/
        .input_w = 1920,
        .input_h = 1080,
        .input_buf_cnt = 20,

        /*output port param*/
        .output_w = 1920,
        .output_h = 1080,
        .output_buf_cnt = 20,
    };
    mConfig.input_w = mConfig.output_w = config->mVideoStream.width;
    mConfig.input_h = mConfig.output_h = config->mVideoStream.height;
    mConfig.bitrate = config->mVideoRateConfig.bitrate;
    mConfig.targetBitrate = config->mVideoRateConfig.targetBitrate;
    mConfig.isBitRateConstant = config->mVideoRateConfig.isBitRateConstant;
    mTimeOffsetInc = 30000;
    mConfig.nframerate = config->mFpsRange[1];
    if (mConfig.nframerate > 60) {
        mTimeOffsetInc = 15000;
    }
    char path[256] = {0};
    if (config->mIsH265) {
        mConfig.componentName = (char *)"OMX.qcom.video.encoder.hevc",
        mConfig.codec = OMX_VIDEO_CodingHEVC,
        mConfig.eprofile = OMX_VIDEO_HEVCProfileMain,
        mConfig.elevel = OMX_VIDEO_HEVCHighTierLevel3,
        sprintf(path,"%s/camera_0.h265",CAMERA_STORAGE_DIR, "w+");
    } else {
        sprintf(path,"%s/camera_0.h264",CAMERA_STORAGE_DIR, "w+");
    }
    mOutFd = fopen(path,"w+");

    mTimeOffset = 0;
    mBufferQueue = new list<buffer_handle_t *>;
    pthread_mutex_init(&mLock, NULL);
    pthread_condattr_t cattr;
    pthread_condattr_init(&cattr);
    pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC);
    pthread_cond_init(&mCond, &cattr);
    pthread_condattr_destroy(&cattr);
    mCoder->setConfig(&mConfig, this);
    mIsStop = false;
}

/************************************************************************
* name : ~QCameraTestVideoEncoder
* function: ~QCameraTestVideoEncoder
************************************************************************/
QCameraTestVideoEncoder::~QCameraTestVideoEncoder()
{
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCond);
    if (mOutFd != NULL){
        fclose(mOutFd);
        mOutFd = NULL;
    }
}

/************************************************************************
* name : run
* function: start encoder thread
************************************************************************/
void QCameraTestVideoEncoder::run()
{
    mCoder->start();
}

/************************************************************************
* name : stop
* function: stop encoder
************************************************************************/
void QCameraTestVideoEncoder::stop()
{
    //set stop state
    mIsStop = true;
    mCoder->stop();
    delete mCoder;
    mCoder = NULL;
    buffer_handle_t * buf_handle;
    while (true) {
        pthread_mutex_lock(&mLock);
        ALOGE("QCameraTestVideoEncoder::stop: mBufferQueue:%d\n",mBufferQueue->size());
        if (mBufferQueue->size() > 0) {
            buf_handle = mBufferQueue->front();
            mBufferQueue->pop_front();
        } else {
            delete mBufferQueue;
            mBufferQueue = NULL;
            pthread_mutex_unlock(&mLock);
            break;
        }
        pthread_mutex_unlock(&mLock);
        mStream->bufferManager->PutBuffer(buf_handle);
        buf_handle = NULL;
    }
}

/************************************************************************
* name : Read
* function: handler to fill a buffer of handle to omx input port
************************************************************************/
int QCameraTestVideoEncoder::Read(OMX_BUFFERHEADERTYPE *buf)
{
    encoder_media_buffer_type *meta_buffer;
    meta_buffer = (encoder_media_buffer_type*)(buf->pBuffer);//8 Byte;
    int ret = 0;
    int readLen = 0;
    if (!meta_buffer) {
        ALOGE("meta_buffer is empty");
        return OMX_ErrorNone;
    }

    struct timespec tv;
    pthread_mutex_lock(&mLock);
    if (mBufferQueue != NULL) {
        while (mBufferQueue->size() == 0) {
            clock_gettime(CLOCK_MONOTONIC,&tv);
            tv.tv_sec += 1;//1s
            ret = pthread_cond_timedwait(&mCond, &mLock,&tv);
            if (ret != 0) {
                /*CallStack e;
                  e.update();
                  e.log("TestVideoEncoder");*/
                pthread_mutex_unlock(&mLock);
                ALOGE("%s %d waiting timeout",__func__,__LINE__);
                return -1;
            }
        }
    } else {
        ALOGE("read exit");
        pthread_mutex_unlock(&mLock);
        return -1;
    }
    buffer_handle_t *buf_handle = mBufferQueue->front();
    mBufferQueue->pop_front();
    pthread_mutex_unlock(&mLock);
    if (!buf_handle) {
        ALOGE("buffer handle is NULL");
        return -1;
    } else {

#if DISABLE_META_MODE
        memcpy(buf->pBuffer,mStream->bufferManager->getBufferInfo(buf_handle)->vaddr,buf->nAllocLen);
        mStream->bufferManager->PutBuffer(buf_handle);
        readLen = buf->nAllocLen;
#else
        meta_buffer->buffer_type = kMetadataBufferTypeGrallocSource;     //0:camera_source 1:gralloc_source
        meta_buffer->meta_handle = *buf_handle;
        ALOGI("meta_buffer->meta_handle %p", meta_buffer->meta_handle);
#endif
    }
    buf->nFilledLen = readLen;
    buf->nTimeStamp = mTimeOffset;
    mTimeOffset += mTimeOffsetInc;
    return ret;
}

/************************************************************************
* name : Write
* function: handler to write omx output data
************************************************************************/
OMX_ERRORTYPE QCameraTestVideoEncoder::Write(OMX_BUFFERHEADERTYPE *buf)
{
    fwrite(buf->pBuffer, 1, buf->nFilledLen, mOutFd);
    return OMX_ErrorNone;
}

/************************************************************************
* name : EmptyDone
* function: handler to put input buffer
************************************************************************/
OMX_ERRORTYPE QCameraTestVideoEncoder::EmptyDone(OMX_BUFFERHEADERTYPE *buf)
{

#if (!DISABLE_META_MODE)
    encoder_media_buffer_type *meta_buffer;
    meta_buffer = (encoder_media_buffer_type*)(buf->pBuffer);//8 Byte;
    if (!meta_buffer) {
        ALOGE("meta_buffer is empty");
        return OMX_ErrorNone;
    }
    if (meta_buffer->meta_handle != NULL) {
        int fd = meta_buffer->meta_handle->data[0];
        if (mFrameHandleMap.find(fd)!=mFrameHandleMap.end()) {
            mStream->bufferManager->PutBuffer(mFrameHandleMap[fd]);
        }
    }
#endif
    return OMX_ErrorNone;
}

/************************************************************************
* name : EnqueueFrameBuffer
* function: enq a buffer to input port queue
************************************************************************/
void QCameraTestVideoEncoder::EnqueueFrameBuffer(CameraStream *stream, buffer_handle_t *buf_handle){
    mStream = stream;
    mFrameHandleMap[(*buf_handle)->data[0]] = buf_handle;
    pthread_mutex_lock(&mLock);
    mBufferQueue->push_back(buf_handle);
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mLock);
}

