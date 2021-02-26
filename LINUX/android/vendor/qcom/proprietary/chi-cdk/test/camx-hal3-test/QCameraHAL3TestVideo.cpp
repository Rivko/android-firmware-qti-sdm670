////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestVideo.cpp
/// @brief TestCase for Video
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraHAL3TestVideo.h"

#define PREVIEW_STREAM 0
#define VIDEO_STREAM 1
#define SNAPSHOT_STREAM 2

#define LIVING_REQUEST_APPEND 5
using namespace android;

QCameraHAL3TestVideo::QCameraHAL3TestVideo(camera_module_t* module,
    QCameraHAL3Device* device, TestConfig* config)
{
    mModule = module;
    mConfig = config;
    mDevice = device;
    #ifdef ENABLE_VIDEO_ENCODER
    mVideoEncoder = new QCameraTestVideoEncoder(mConfig);
    #endif
    mIsVideo = 1;
    mDumppreviewcount = 0;
    mDumpvideocount = 0;
    mAllowDumpPreview = false;
    mAllowDumpVideo = false;
    mFrameCount = 0;
    mLastFrameCount = 0;
    mLastFpsTime = 0;
    mMetadataExt = NULL;
    mVideoFrameCount = 0;
    mVideoLastFrameCount = 0;
    mVideoLastFpsTime = 0;
    mVideoMode = VIDEO_MODE_NORMAL;
    mIsStoped = true;
}

/************************************************************************
* name : ~QCameraHAL3TestVideo
* function: destruct object
************************************************************************/
QCameraHAL3TestVideo::~QCameraHAL3TestVideo()
{
}

/************************************************************************
* name : getCurrentMeta
* function: Get current metadata setting.
************************************************************************/
android::CameraMetadata* QCameraHAL3TestVideo::getCurrentMeta()
{
    return &(mDevice->mCurrentMeta);
}

/************************************************************************
* name : setCurrentMeta
* function: Set A External metadata as current metadata.
************************************************************************/
void QCameraHAL3TestVideo::setCurrentMeta(android::CameraMetadata* meta)
{
    mMetadataExt = meta;
}
/************************************************************************
* name : updataMetaDatas
* function: update request command.
************************************************************************/
void QCameraHAL3TestVideo::updataMetaDatas(android::CameraMetadata* meta)
{
    mDevice->updateMetadataForNextRequest(meta);
}
/************************************************************************
* name : DumpFrame
* function: dump frames
************************************************************************/
void DumpFrame(CameraStream *stream, buffer_handle_t *buf,int frameNum,int dumpType)
{
    char fname[256];
    if (dumpType)
        sprintf(fname, "%s/camera_video_%d.%s",CAMERA_STORAGE_DIR,frameNum, "yuv");
    else
        sprintf(fname, "%s/camera_preview_%d.%s",CAMERA_STORAGE_DIR,frameNum, "yuv");
    FILE* fd = fopen(fname, "wb");
    BufferInfo* info = stream->bufferManager->getBufferInfo(buf);
    void* data = info->vaddr;
    int size = info->size;
    fwrite(data, size, 1, fd);
    fclose(fd);
}

/************************************************************************
* name : showPreviewFPS
* function: show preview frame FPS.
************************************************************************/
void QCameraHAL3TestVideo::showPreviewFPS()
{
    double fps = 0;
    mFrameCount++;
    nsecs_t now = systemTime();
    nsecs_t diff = now - mLastFpsTime;
    if (diff > s2ns(1)) {
        fps = (((double)(mFrameCount - mLastFrameCount)) *
                (double)(s2ns(1))) / (double)diff;
        ALOGI("PROFILE_PREVIEW_FRAMES_PER_SECOND CAMERA %d: %.4f: mFrameCount=%d",
              mConfig->mCameraId, fps, mFrameCount);
        mLastFpsTime = now;
        mLastFrameCount = mFrameCount;
    }
}


/************************************************************************
* name : showVideowFPS
* function: show video frame FPS.
************************************************************************/
void QCameraHAL3TestVideo::showVideoFPS()
{
    double fps = 0;
    mVideoFrameCount++;
    nsecs_t now = systemTime();
    nsecs_t diff = now - mVideoLastFpsTime;
    if (diff > s2ns(1)) {
        fps = (((double)(mVideoFrameCount - mVideoLastFrameCount)) *
                (double)(s2ns(1))) / (double)diff;
        ALOGI("PROFILE_VIDEO_FRAMES_PER_SECOND CAMERA %d: %.4f: mVideoFrameCount=%d",
              mConfig->mCameraId, fps, mVideoFrameCount);
        mVideoLastFpsTime = now;
        mVideoLastFrameCount = mVideoFrameCount;
    }
}

/************************************************************************
* name : CapturePostProcess
* function: Do post process
************************************************************************/
void QCameraHAL3TestVideo::CapturePostProcess(DeviceCallback* cb,
    camera3_capture_result *result)
{
    const camera3_stream_buffer_t* buffers = NULL;
    buffers = result->output_buffers;
    for (uint32_t i = 0;i < result->num_output_buffers ;i++) {
        int index = mDevice->findStream(buffers[i].stream);
        CameraStream* stream = mDevice->mCameraStreams[index];
        // Dump frame
        char fname[256];
        if (stream->streamType == CAMERA3_TEMPLATE_STILL_CAPTURE) {
            sprintf(fname, "%s/camera_capture_%d.%s",CAMERA_STORAGE_DIR, result->frame_number, "jpg");
            FILE* fd = fopen(fname, "wb");
            BufferInfo* info = stream->bufferManager->getBufferInfo(buffers[i].buffer);
            void* data = info->vaddr;
            int size = info->size;
            fwrite(data, size, 1, fd);
            fclose(fd);
            stream->bufferManager->PutBuffer(buffers[i].buffer);
        }
        if (stream->streamType == CAMERA3_TEMPLATE_VIDEO_RECORD) {
            if ((buffers != NULL) && (result->num_output_buffers > 0)) {
                if (mAllowDumpVideo && mDumpvideocount--) {
                    DumpFrame(stream,buffers[i].buffer,result->frame_number,1);
                    if (mDumpvideocount == 0) {
                        mAllowDumpVideo = false;
                    }
                }
                if (mIsStoped) {
                    stream->bufferManager->PutBuffer(buffers[i].buffer);
                } else {
                    EnqueueFrameBuffer(stream,buffers[i].buffer);
                }
            }
        }
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW) {
            if (mAllowDumpPreview && mDumppreviewcount--) {
                DumpFrame(stream,buffers[i].buffer,result->frame_number,0);
                if (mDumppreviewcount == 0) {
                    mAllowDumpPreview = false;
                }
            }
            stream->bufferManager->PutBuffer(buffers[i].buffer);
        }
        // FPS Show
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW && mConfig->mShowFps) {
            showPreviewFPS();
        }

        if (stream->streamType == CAMERA3_TEMPLATE_VIDEO_RECORD && mConfig->mShowFps) {
            showVideoFPS();
        }
    }
}

/************************************************************************
* name : selectOpMode
* function: choose suitable operate mode.
************************************************************************/
void QCameraHAL3TestVideo::selectOpMode(uint32_t *operation_mode,int width,int height,
    int fps)
{
    int sensormode = 0;
    uint32_t tags = 0;
    const int32_t *sensorModeTable = NULL;
    int res = 0;
    sp<VendorTagDescriptor> vTags =
        android::VendorTagDescriptor::getGlobalVendorTagDescriptor();
    camera_metadata_ro_entry entry;
    CameraMetadata::getTagFromName("org.quic.camera2.sensormode.info.SensorModeTable",
        vTags.get(), &tags);

    res = find_camera_metadata_ro_entry(mDevice->mCharacteristics, tags, &entry);
    if ((res == 0) && (entry.count > 0)) {
        sensorModeTable = entry.data.i32;
    }

    int modeCount = sensorModeTable[0];
    int modeSize = sensorModeTable[1];
    int sensorMode = -1;
    int a_width,a_height,a_fps;
    for (int i = 0; i < modeCount; i++) {
        a_width =  sensorModeTable[2 + i*modeSize];
        a_height = sensorModeTable[3 + i*modeSize];
        a_fps = sensorModeTable[4 + i*modeSize];
        if ((a_fps == fps)&&(a_width >= width)&&(a_height >= height)) {
            sensorMode = i;
        }
    }
    // use StreamConfigModeSensorMode in camx
    *operation_mode = (sensorMode + 1)<<16;
}

/************************************************************************
* name : initVideoStreams
* function: init video stream
************************************************************************/
int QCameraHAL3TestVideo::initVideoStreams()
{
    int res = 0;
    camera3_stream_t previewStream;
    camera3_stream_t videoStream;
    camera3_stream_t snapshotStream;
    std::vector<camera3_stream_t*> streams;
    streams.resize(0);

    uint32_t operation_mode = CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE;
    if (mConfig->mFpsRange[1] > 30 && mConfig->mFpsRange[1] <= 60) {
        // for HFR case such as 4K@60 and 1080p@60
        mVideoMode = VIDEO_MODE_HFR60;
    } else if (mConfig->mFpsRange[1] > 60 && mConfig->mFpsRange[1] <= 120) {
        // for HFR case such as 1080p@120 1080p@90
        operation_mode = CAMERA3_STREAM_CONFIGURATION_CONSTRAINED_HIGH_SPEED_MODE;
        mVideoMode = VIDEO_MODE_HFR120;
    }
    //init stream configure
    bool supportsPartialResults;
    uint32_t partialResultCount;
    std::vector<AvailableStream> outputPreviewStreams;
    printf("preview:%dx%d %d\n",mConfig->mPreviewStream.width,mConfig->mPreviewStream.height,
        mConfig->mPreviewStream.format);
    AvailableStream previewThreshold = {mConfig->mPreviewStream.width, mConfig->mPreviewStream.height,
        mConfig->mPreviewStream.format};
    if (res == 0) {
        camera_metadata_ro_entry entry;
        res = find_camera_metadata_ro_entry(mDevice->mCharacteristics,
            ANDROID_REQUEST_PARTIAL_RESULT_COUNT, &entry);
        if ((0 == res) && (entry.count > 0)) {
            partialResultCount = entry.data.i32[0];
            supportsPartialResults = (partialResultCount > 1);
        }
        res = mDevice->getAvailableOutputStreams(outputPreviewStreams, &previewThreshold);
    }
    if (res < 0 || outputPreviewStreams.size() == 0) {
        ALOGE("Failed to find output stream for preview: w: %d, h: %d, fmt: %d",
            mConfig->mPreviewStream.width, mConfig->mPreviewStream.height,
            mConfig->mPreviewStream.format);
        return -1;
    }

    previewStream.stream_type = CAMERA3_STREAM_OUTPUT;//OUTPUT
    previewStream.width = outputPreviewStreams[0].width;
    previewStream.height = outputPreviewStreams[0].height;
    previewStream.format = outputPreviewStreams[0].format;
    previewStream.data_space = HAL_DATASPACE_UNKNOWN;
    previewStream.usage = GRALLOC_USAGE_HW_COMPOSER  | GRALLOC_USAGE_HW_TEXTURE;
    previewStream.rotation = 0;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    previewStream.max_buffers = 0;
    previewStream.priv = 0;
    streams.push_back(&previewStream);

    /*add video stream*/
    std::vector<AvailableStream> outputVideoStreams;
    AvailableStream videoThreshold = {mConfig->mVideoStream.width, mConfig->mVideoStream.height,
        mConfig->mVideoStream.format};
    //AvailableStream videoThreshold = {1920, 1080, HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED};
    if (res == 0) {
        camera_metadata_ro_entry entry;
        res = find_camera_metadata_ro_entry(mDevice->mCharacteristics,
            ANDROID_REQUEST_PARTIAL_RESULT_COUNT, &entry);
        if ((0 == res) && (entry.count > 0)) {
            partialResultCount = entry.data.i32[0];
            supportsPartialResults = (partialResultCount > 1);
        }
        res = mDevice->getAvailableOutputStreams(outputVideoStreams, &videoThreshold);
    }
    if (res < 0 || outputVideoStreams.size() == 0) {
        ALOGE("Failed to find output stream for video: w: %d, h: %d, fmt: %d",
            mConfig->mVideoStream.width, mConfig->mVideoStream.height,
            mConfig->mVideoStream.format);
        return -1;
    }

    videoStream.stream_type = CAMERA3_STREAM_OUTPUT;//OUTPUT
    videoStream.width = outputVideoStreams[0].width;
    videoStream.height = outputVideoStreams[0].height;
    videoStream.format = outputVideoStreams[0].format;
    videoStream.data_space = HAL_DATASPACE_BT709;
    videoStream.usage = GRALLOC_USAGE_HW_VIDEO_ENCODER;
    videoStream.rotation = 0;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    videoStream.max_buffers = 8;
    videoStream.priv = 0;
    streams.push_back(&videoStream);

    /*add snapshot stream*/
    if (mVideoMode <= VIDEO_MODE_HFR60) {
        std::vector<AvailableStream> outputSnapshotStreams;
        AvailableStream snapshotThreshold = {mConfig->mSnapshotStream.width,
            mConfig->mSnapshotStream.height, mConfig->mSnapshotStream.format};
        if (res == 0) {
            camera_metadata_ro_entry entry;
            res = find_camera_metadata_ro_entry(mDevice->mCharacteristics,
                ANDROID_REQUEST_PARTIAL_RESULT_COUNT, &entry);
            if ((0 == res) && (entry.count > 0)) {
                partialResultCount = entry.data.i32[0];
                supportsPartialResults = (partialResultCount > 1);
            }
            res = mDevice->getAvailableOutputStreams(outputSnapshotStreams, &snapshotThreshold);
        }
        if (res < 0 || outputSnapshotStreams.size() == 0) {
            ALOGE("Failed to find output stream for snapshot: w: %d, h: %d, fmt: %d",
                mConfig->mSnapshotStream.width, mConfig->mSnapshotStream.height,
                mConfig->mSnapshotStream.format);
            return -1;
        }

        snapshotStream.stream_type = CAMERA3_STREAM_OUTPUT;//OUTPUT
        snapshotStream.width = outputSnapshotStreams[0].width;
        snapshotStream.height = outputSnapshotStreams[0].height;
        snapshotStream.format = outputSnapshotStreams[0].format;
        snapshotStream.data_space = HAL_DATASPACE_JFIF;
        snapshotStream.usage = GRALLOC_USAGE_SW_READ_OFTEN;
        snapshotStream.rotation = 0;
        // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
        snapshotStream.max_buffers = 0;
        snapshotStream.priv = 0;
        streams.push_back(&snapshotStream);
    }

    mDevice->setPutBufferExt(PUT_BUFFER_EXTERNAL);
    mDevice->setFpsRange(mConfig->mFpsRange[0], mConfig->mFpsRange[1]);
    if (mVideoMode == VIDEO_MODE_HFR60) {
        // for HFR case
        int stream_size = 0;
        int stream_index = 0;
        for (int i = 0;i < (int)streams.size();i++) {
            if ((streams[i]->width * streams[i]->height) > stream_size) {
                stream_size = streams[i]->width * streams[i]->height;
                stream_index = i;
            }
        }
        selectOpMode(&operation_mode,streams[stream_index]->width,
            streams[stream_index]->height,mConfig->mFpsRange[1]);
    }

    mDevice->configureStreams(streams,operation_mode);
    if (mMetadataExt) {
        mDevice->setCurrentMeta(mMetadataExt);
        mDevice->constructDefaultRequestSettings(VIDEO_STREAM,CAMERA3_TEMPLATE_VIDEO_RECORD);
    } else {
        mDevice->constructDefaultRequestSettings(VIDEO_STREAM,CAMERA3_TEMPLATE_VIDEO_RECORD,true);
    }
    mDevice->constructDefaultRequestSettings(PREVIEW_STREAM,CAMERA3_TEMPLATE_PREVIEW);
    if (mVideoMode <= VIDEO_MODE_HFR60) {
        mDevice->constructDefaultRequestSettings(SNAPSHOT_STREAM,CAMERA3_TEMPLATE_STILL_CAPTURE);
    }

    android::CameraMetadata* metaUpdate = getCurrentMeta();
    sp<VendorTagDescriptor> vTags =
        android::VendorTagDescriptor::getGlobalVendorTagDescriptor();

    uint8_t antibanding = ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO;
    metaUpdate->update(ANDROID_CONTROL_AE_ANTIBANDING_MODE,&(antibanding),sizeof(antibanding));

    uint8_t afmode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO;
    metaUpdate->update(ANDROID_CONTROL_AF_MODE, &(afmode), 1);
    uint8_t reqFaceDetectMode =  (uint8_t)ANDROID_STATISTICS_FACE_DETECT_MODE_OFF;
    metaUpdate->update(ANDROID_STATISTICS_FACE_DETECT_MODE,
            &reqFaceDetectMode, 1);

    if (mVideoMode <= VIDEO_MODE_HFR60) {
        uint8_t jpegquality = JPEG_QUALITY_DEFAULT;
        metaUpdate->update(ANDROID_JPEG_QUALITY,&(jpegquality),sizeof(jpegquality));
    }

    if (mVideoMode != VIDEO_MODE_NORMAL) {
        uint8_t videohdr = 0;
        uint8_t PCREnable = 0;
        uint8_t EISEnable = 0;
        uint32_t tag;
        uint8_t reqVstabMode = ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
        metaUpdate->update(ANDROID_CONTROL_VIDEO_STABILIZATION_MODE, &reqVstabMode, 1);

        uint8_t nr = ANDROID_NOISE_REDUCTION_MODE_FAST;
        metaUpdate->update(ANDROID_NOISE_REDUCTION_MODE, &(nr), 1);

        uint8_t antibanding = ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ;
        metaUpdate->update(ANDROID_CONTROL_AE_ANTIBANDING_MODE,&antibanding,1);

        CameraMetadata::getTagFromName("org.quic.camera2.streamconfigs.HDRVideoMode",
            vTags.get(), &tag);
        metaUpdate->update(tag, &(videohdr), 1);

        CameraMetadata::getTagFromName("org.quic.camera.EarlyPCRenable.EarlyPCRenable",
            vTags.get(), &tag);
        metaUpdate->update(tag, &(PCREnable), 1);

        CameraMetadata::getTagFromName("org.quic.camera.eis3enable.EISV3Enable",
            vTags.get(), &tag);
        metaUpdate->update(tag, &(EISEnable), 1);
    }

    updataMetaDatas(metaUpdate);
    return res;
}

int QCameraHAL3TestVideo::PreinitVideoStreams()
{
    int res = 0;
    camera3_stream_t previewStream;
    camera3_stream_t videoStream;
    camera3_stream_t snapshotStream;
    std::vector<camera3_stream_t*> streams;
    int stream_num = 3;

    printf("PreinitVideoStreams \n");

    previewStream.stream_type = CAMERA3_STREAM_OUTPUT;
    previewStream.width = mConfig->mPreviewStream.width;
    previewStream.height = mConfig->mPreviewStream.height;
    previewStream.format = mConfig->mPreviewStream.format;
    previewStream.data_space = HAL_DATASPACE_UNKNOWN;
    previewStream.usage = GRALLOC_USAGE_HW_COMPOSER  | GRALLOC_USAGE_HW_TEXTURE;
    previewStream.rotation = 0;
    previewStream.max_buffers = PREVIEW_STREAM_BUFFER_MAX;
    previewStream.priv = 0;

    // add video stream
    videoStream.stream_type = CAMERA3_STREAM_OUTPUT;//OUTPUT
    videoStream.width = mConfig->mVideoStream.width;
    videoStream.height = mConfig->mVideoStream.height;
    videoStream.format = mConfig->mVideoStream.format;
    videoStream.data_space = (android_dataspace_t)0;
    videoStream.usage = GRALLOC_USAGE_HW_VIDEO_ENCODER;
    videoStream.rotation = 0;
    videoStream.max_buffers = VIDEO_STREAM_BUFFER_MAX;
    videoStream.priv = 0;

    // add snapshot stream
    snapshotStream.stream_type = CAMERA3_STREAM_OUTPUT;//OUTPUT
    snapshotStream.width = mConfig->mSnapshotStream.width;
    snapshotStream.height = mConfig->mSnapshotStream.height;
    snapshotStream.format = mConfig->mSnapshotStream.format;
    snapshotStream.data_space = HAL_DATASPACE_JFIF;
    snapshotStream.usage = GRALLOC_USAGE_SW_READ_OFTEN;
    snapshotStream.rotation = 0;
    snapshotStream.max_buffers = SNAPSHOT_STREAM_BUFFER_MAX;
    snapshotStream.priv = 0;

    streams.resize(stream_num);
    streams[VIDEO_STREAM] = &videoStream;
    streams[PREVIEW_STREAM] = &previewStream;
    streams[SNAPSHOT_STREAM] = &snapshotStream;

    mDevice->PreAllocateStreams(streams);

    return res;
}

/************************************************************************
* name : EnqueueFrameBuffer
* function: enqueue a frame to video encoder
************************************************************************/
void QCameraHAL3TestVideo::EnqueueFrameBuffer(CameraStream *stream,buffer_handle_t *buf_handle) {
    #ifdef ENABLE_VIDEO_ENCODER
    mVideoEncoder->EnqueueFrameBuffer(stream,buf_handle);
	#endif
}

/************************************************************************
* name : stop
* function: stop video tests
************************************************************************/
void QCameraHAL3TestVideo::stop()
{
    mIsStoped = true;
    #ifdef ENABLE_VIDEO_ENCODER
    mVideoEncoder->stop();
    #endif
    mDevice->stopStreams();
    #ifdef ENABLE_VIDEO_ENCODER
    delete mVideoEncoder;
    mVideoEncoder = NULL;
    #endif
    mDevice->setPutBufferExt(PUT_BUFFER_INTERNAL);
}

/************************************************************************
* name : RequestCaptures
* function: impliment "s" command
************************************************************************/
void QCameraHAL3TestVideo::RequestCaptures(StreamCapture request)
{
    // send a message to request thread
    pthread_mutex_lock(&mDevice->mRequestThread->mutex);
    CameraRequestMsg* msg = new CameraRequestMsg();
    memset(msg,0,sizeof(CameraRequestMsg));
    msg->requestNumber[SNAPSHOT_STREAM] = request.count;
    msg->mask |= 1 << SNAPSHOT_STREAM;
    msg->msgType = REQUEST_CHANGE;
    mDevice->mRequestThread->msgQueue.push_back(msg);
    ALOGI("Msg for capture picture mask%x \n",msg->mask);
    pthread_cond_signal(&mDevice->mRequestThread->cond);
    pthread_mutex_unlock(&mDevice->mRequestThread->mutex);
}

/************************************************************************
* name : run
* function: start video test case
************************************************************************/
void QCameraHAL3TestVideo::run()
{
    //open camera
    int res = 0;
    int isVideoMode = 1;

    mDevice->setCallBack(this);
    res = initVideoStreams();

    if (mVideoMode != VIDEO_MODE_NORMAL) {
        mDevice->mLivingRequestExtAppend = LIVING_REQUEST_APPEND;
    }
    #ifdef ENABLE_VIDEO_ENCODER
    mVideoEncoder->run();
    #endif
    mIsStoped = false;
    CameraThreadData* resultThreadVideo = new CameraThreadData();

    CameraThreadData* requestThreadVideo = new CameraThreadData();
    requestThreadVideo->requestNumber[VIDEO_STREAM] = REQUEST_NUMBER_UMLIMIT;
    requestThreadVideo->requestNumber[PREVIEW_STREAM] = REQUEST_NUMBER_UMLIMIT;
    if (mVideoMode > VIDEO_MODE_HFR60) {
        requestThreadVideo->skipPattern[PREVIEW_STREAM] = 2;
    }

    mDevice->processCaptureRequestOn(requestThreadVideo,resultThreadVideo);
}

/************************************************************************
* name : dumpPreview
* function: set frame count of preveiw dump.
************************************************************************/
void QCameraHAL3TestVideo::dumpPreview(int count)
{
    mAllowDumpPreview = true;
    mDumppreviewcount += count;
}

/************************************************************************
* name : dumpVideo
* function: set frame count of Video dump.
************************************************************************/
void QCameraHAL3TestVideo::dumpVideo(int count)
{
    mAllowDumpVideo = true;
    mDumpvideocount += count;
}
