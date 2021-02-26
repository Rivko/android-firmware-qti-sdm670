////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestPreview.h
/// @brief Preview only mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraHAL3TestPreview.h"
#define LOG_TAG "CameraTest"

using namespace android;
#define PREVIEW_STREAM 0

/************************************************************************
* name : QCameraHAL3TestPreview
* function: construct object.
************************************************************************/
QCameraHAL3TestPreview::QCameraHAL3TestPreview(camera_module_t* module,QCameraHAL3Device* device,TestConfig* config)
{
    mModule = module;
    mConfig = config;
    mDumpPreviewNum = 0;
    mDevice = device;
    mFrameCount = 0;
    mLastFrameCount = 0;
    mLastFpsTime = 0;
    mMetadataExt = NULL;
    mFfbmPreviewCb = NULL;
}

/************************************************************************
* name : ~QCameraHAL3TestPreview
* function: destory object.
************************************************************************/
QCameraHAL3TestPreview::~QCameraHAL3TestPreview()
{

}

/************************************************************************
* name : showPreviewFPS
* function: show preview frame FPS.
************************************************************************/
void QCameraHAL3TestPreview::showPreviewFPS()
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
* name : getCurrentMeta
* function: Get current metadata setting.
************************************************************************/
android::CameraMetadata* QCameraHAL3TestPreview::getCurrentMeta()
{
    return &(mDevice->mCurrentMeta);
}

/************************************************************************
* name : setCurrentMeta
* function: Set A External metadata as current metadata. this is used by nonZSL snapshot now
************************************************************************/
void QCameraHAL3TestPreview::setCurrentMeta(android::CameraMetadata* meta)
{
    mMetadataExt = meta;
}

/************************************************************************
* name : updataMetaDatas
* function: update request command.
************************************************************************/
void QCameraHAL3TestPreview::updataMetaDatas(android::CameraMetadata* meta)
{
    mDevice->updateMetadataForNextRequest(meta);
}

/************************************************************************
* name : HandleMetaData
* function: analysis meta info from capture result.
************************************************************************/
void QCameraHAL3TestPreview::HandleMetaData(DeviceCallback* cb, camera3_capture_result *result)
{
    const camera3_stream_buffer_t* buffers = NULL;
    QCameraHAL3TestPreview* testpre = (QCameraHAL3TestPreview*)cb;
    QCameraHAL3Device* device = testpre->mDevice;
    sp<VendorTagDescriptor> vTags =
        android::VendorTagDescriptor::getGlobalVendorTagDescriptor();

    if (result->partial_result >= 1)
    {
        if (mConfig->mMetaDump.exposureValue) {
            camera_metadata_ro_entry entry;
            uint64_t exptime = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_SENSOR_EXPOSURE_TIME, &entry);
            if ((0 == res) && (entry.count > 0)) {
                exptime = entry.data.i64[0];
            }
            if (exptime != mConfig->mMetaStat.expTime){
                mConfig->mDump->print("frame:%d exposure value = %llu\n",
                    result->frame_number,exptime);
                mConfig->mMetaStat.expTime = exptime;
            }
        }
        if (mConfig->mMetaDump.isoValue) {
            camera_metadata_ro_entry entry;
            int iso = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_SENSOR_SENSITIVITY, &entry);
            if ((0 == res) && (entry.count > 0)) {
                iso = entry.data.i32[0];
            }
            if (iso != mConfig->mMetaStat.isoValue) {
                mConfig->mDump->print("frame:%d iso value = %d\n",
                    result->frame_number,iso);
                mConfig->mMetaStat.isoValue = iso;
            }
        }
        if (mConfig->mMetaDump.aeMode) {
            camera_metadata_ro_entry entry;
            int aemode = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result, ANDROID_CONTROL_AE_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                aemode = entry.data.u8[0];
            }

            if (aemode != mConfig->mMetaStat.aeMode) {
                switch (aemode)
                {
                case ANDROID_CONTROL_AE_MODE_OFF:
                    mConfig->mDump->print("frame:%d ae mode:AE MODE OFF\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_AE_MODE_ON:
                    mConfig->mDump->print("frame:%d ae mode:AE MODE ON\n",result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.aeMode = aemode;
            }
        }
        if (mConfig->mMetaDump.awbMode) {
            camera_metadata_ro_entry entry;
            int awbmode = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result, ANDROID_CONTROL_AWB_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                awbmode = entry.data.u8[0];
            }
            if (awbmode != mConfig->mMetaStat.awbMode) {
                switch (awbmode)
                {
                case ANDROID_CONTROL_AWB_MODE_OFF:
                    mConfig->mDump->print("frame:%d awb mode:AWB MODE OFF\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_AWB_MODE_AUTO:
                    mConfig->mDump->print("frame:%d awb mode:AWB MODE AUTO\n",result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.awbMode = awbmode;
            }
        }
        if (mConfig->mMetaDump.afMode) {
            camera_metadata_ro_entry entry;
            int afmode = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_AF_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                afmode = entry.data.u8[0];
            }
            if (afmode != mConfig->mMetaStat.afMode) {
                switch (afmode)
                {
                case ANDROID_CONTROL_AF_MODE_OFF:
                    mConfig->mDump->print("frame:%d af mode:AF MODE OFF\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_AF_MODE_AUTO:
                    mConfig->mDump->print("frame:%d af mode:AF MODE AUTO\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_AF_MODE_MACRO:
                    mConfig->mDump->print("frame:%d af mode:AF MODE MACRO\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO:
                    mConfig->mDump->print("frame:%d af mode:AF MODE CONTINUOUS VIDEO\n",
                        result->frame_number);
                case ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE:
                    mConfig->mDump->print("frame:%d af mode:AF MODE CONTINUOUS PICTURE\n",
                        result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.afMode = afmode;
            }
        }
        if (mConfig->mMetaDump.afValue) {
            camera_metadata_ro_entry entry;
            float afvalue = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_LENS_FOCUS_DISTANCE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                afvalue = entry.data.f[0];
            }
            if (afvalue != mConfig->mMetaStat.afValue) {
                mConfig->mDump->print("frame:%d af focus distance = %f\n",
                    result->frame_number,afvalue);
                mConfig->mMetaStat.afValue = afvalue;
            }
        }
        if (mConfig->mMetaDump.aeAntiMode) {
            camera_metadata_ro_entry entry;
            int antimode = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_AE_ANTIBANDING_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                antimode = entry.data.u8[0];
            }
            if (antimode != mConfig->mMetaStat.aeAntiMode) {
                switch (antimode)
                {
                case ANDROID_CONTROL_AE_ANTIBANDING_MODE_OFF:
                    mConfig->mDump->print("frame:%d aeAnti mode:AE ANTI MODE OFF\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ:
                    mConfig->mDump->print("frame:%d aeAnti mode:AE ANTI MODE 50HZ\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AE_ANTIBANDING_MODE_60HZ:
                    mConfig->mDump->print("frame:%d aeAnti mode:AE ANTI MODE 60HZ\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO:
                    mConfig->mDump->print("frame:%d aeAnti mode:AE ANTI MODE AUTO\n",
                        result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.aeAntiMode = antimode;
            }
        }
        if (mConfig->mMetaDump.colorCorrectMode) {
            camera_metadata_ro_entry entry;
            int colormode = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_COLOR_CORRECTION_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                colormode = entry.data.u8[0];
            }
            if (colormode != mConfig->mMetaStat.colorCorrectMode) {
                switch (colormode)
                {
                case ANDROID_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX:
                    mConfig->mDump->print("frame:%d color correction mode:TRANSFORM_MATRIX\n",
                        result->frame_number);
                    break;
                case ANDROID_COLOR_CORRECTION_MODE_FAST:
                    mConfig->mDump->print("frame:%d color correction mode:FAST\n",
                        result->frame_number);
                    break;
                case ANDROID_COLOR_CORRECTION_MODE_HIGH_QUALITY:
                    mConfig->mDump->print("frame:%d color correction mode:HIGH QUALLITY\n",
                        result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.colorCorrectMode = colormode;
            }
        }
        if (mConfig->mMetaDump.colorCorrectValue) {
            camera_metadata_ro_entry entry;
            float colorvalue = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_COLOR_CORRECTION_GAINS, &entry);
            if ((0 == res) && (entry.count > 0)) {
                colorvalue = entry.data.f[0];
            }
            if (colorvalue != mConfig->mMetaStat.colorCorrectValue) {
                mConfig->mDump->print("frame:%d color correction gain = %f\n",
                    result->frame_number,colorvalue);
                mConfig->mMetaStat.colorCorrectValue = colorvalue;
            }
        }
        if (mConfig->mMetaDump.controlMode) {
            camera_metadata_ro_entry entry;
            int ctrlvalue = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                ctrlvalue = entry.data.u8[0];
            }
            if (ctrlvalue != mConfig->mMetaStat.controlMode) {
                switch (ctrlvalue)
                {
                case ANDROID_CONTROL_MODE_OFF:
                    mConfig->mDump->print("frame:%d control mode:OFF\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_MODE_AUTO:
                    mConfig->mDump->print("frame:%d control mode:AUTO\n",result->frame_number);
                    break;
                case ANDROID_CONTROL_MODE_USE_SCENE_MODE:
                    mConfig->mDump->print("frame:%d control mode:ON\n",result->frame_number);
                    break;
                default:
                    break;
                }
                mConfig->mMetaStat.controlMode = ctrlvalue;
            }
        }
        if (mConfig->mMetaDump.sceneMode) {
            camera_metadata_ro_entry entry;
            int32_t asdresults[10] = {0};
            int res = 0;
            uint32_t tag = 0;
            int i = 0;
            CameraMetadata::getTagFromName("org.quic.camera2.asdresults.ASDResults",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result, tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                memcpy(asdresults,&(entry.data.i32[0]),sizeof(asdresults));
                for (i = 6; i < 10; i++) {
                    if (asdresults[i] != mConfig->mMetaStat.asdresults[i]) {
                        break;
                    }
                }
            }
            if (i < 10) {
                mConfig->mDump->print(
                    "frame:%d asdresults Backlight %d, Landscape %d, Snow %d, Hdr %d\n",\
                    result->frame_number, asdresults[6], asdresults[7], \
                    asdresults[8], asdresults[9]);
                memcpy(mConfig->mMetaStat.asdresults, asdresults, sizeof(asdresults));
            }
        }
        if (mConfig->mMetaDump.hdrMode) {
            camera_metadata_ro_entry entry;
            int hdrmode = 0;
            int res = 0;
            uint32_t tag = 0;
            CameraMetadata::getTagFromName("org.codeaurora.qcamera3.stats.is_hdr_scene",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result, tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                hdrmode = entry.data.u8[0];
            }
            if (hdrmode != mConfig->mMetaStat.hdrMode) {
                mConfig->mDump->print("frame:%d is hdr scene = %d\n",result->frame_number,hdrmode);
                mConfig->mMetaStat.hdrMode = hdrmode;
            }
        }
        if (mConfig->mMetaDump.zoomValue) {
            camera_metadata_ro_entry entry;
            int32_t cropregion[4] = {0};
            int res = 0;
            int i = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_SCALER_CROP_REGION, &entry);
            if ((0 == res) && (entry.count > 0)) {
                memcpy(cropregion,&(entry.data.i32[0]),sizeof(cropregion));
            }
            for (i = 0;i < 4;i++) {
                if (cropregion[i] != mConfig->mMetaStat.cropregion[i]) {
                    break;
                }
            }
            if (i<4) {
                mConfig->mDump->print("frame:%d ZOOM:crop region[%d,%d,%d,%d]\n",
                    result->frame_number,cropregion[0],cropregion[1],
                    cropregion[2],cropregion[3]);
                memcpy(mConfig->mMetaStat.cropregion,cropregion,sizeof(cropregion));
            }
        }

        if (mConfig->mMetaDump.zslMode) {
            camera_metadata_ro_entry entry;
            uint8_t zsl = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_ENABLE_ZSL, &entry);
            if ((0 == res) && (entry.count > 0)) {
                zsl = entry.data.u8[0];
                if (zsl != mConfig->mMetaStat.zslMode){
                    mConfig->mDump->print("frame:%d ZSL mode:%d\n",result->frame_number,zsl);
                    mConfig->mMetaStat.zslMode = zsl;
                }
            }
        }

        if (mConfig->mMetaDump.numFrames) {
            camera_metadata_ro_entry entry;
            uint32_t tag = 0;
            int res = 0;
            int32_t numFrames = -1;
            CameraMetadata::getTagFromName("org.quic.camera2.mfnrconfigs.MFNRTotalNumFrames",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result,
                tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                numFrames = entry.data.i32[0];
                if (numFrames != mConfig->mMetaStat.numFrames) {
                    mConfig->mDump->print("frame:%d num Frames:%d\n",
                        result->frame_number,numFrames);
                    mConfig->mMetaStat.numFrames = numFrames;
                }
            }
        }

        if (mConfig->mMetaDump.expMetering) {
            camera_metadata_ro_entry entry;
            uint32_t tag = 0;
            int res = 0;
            int32_t metering = -1;
            CameraMetadata::getTagFromName(
                "org.codeaurora.qcamera3.exposure_metering.exposure_metering_mode",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result,
                tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                metering = entry.data.i32[0];
                if (metering != mConfig->mMetaStat.expMetering) {
                    mConfig->mDump->print("frame:%d expMetering:%d\n",
                        result->frame_number,metering);
                    mConfig->mMetaStat.expMetering = metering;
                }
            }
        }

        if (mConfig->mMetaDump.selPriority) {
            camera_metadata_ro_entry entry;
            uint32_t tag = 0;
            int res = 0;
            int32_t priority = -1;
            CameraMetadata::getTagFromName(
                "org.codeaurora.qcamera3.iso_exp_priority.select_priority",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result,
                tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                priority = entry.data.i32[0];
                if (priority != mConfig->mMetaStat.selPriority) {
                    mConfig->mDump->print("frame:%d select priority:%d\n",
                        result->frame_number,priority);
                    mConfig->mMetaStat.selPriority = priority;
                }
            }
        }

        if (mConfig->mMetaDump.expPriority) {
            camera_metadata_ro_entry entry;
            uint32_t tag = 0;
            int res = 0;
            int64_t priority = -1;
            CameraMetadata::getTagFromName(
                "org.codeaurora.qcamera3.iso_exp_priority.use_iso_exp_priority",
                vTags.get(), &tag);
            res = find_camera_metadata_ro_entry(result->result,
                tag, &entry);
            if ((0 == res) && (entry.count > 0)) {
                priority = entry.data.i64[0];
                if (priority != mConfig->mMetaStat.expPriority){
                    mConfig->mDump->print("frame:%d ios exp priority:%lld\n",
                        result->frame_number,priority);
                    mConfig->mMetaStat.expPriority = priority;
                }
            }
        }
    }
}

void QCameraHAL3TestPreview::setFfbmPreviewCb(void (*cb)(void*, int)) {
    ALOGI("%s:%d register", __func__, __LINE__);
    mFfbmPreviewCb = cb;
}

/************************************************************************
* name : CapturePostProcess
* function: handle capture result.
************************************************************************/
void QCameraHAL3TestPreview::CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result)
{
    ALOGI("CapturePostProcess \n");
    const camera3_stream_buffer_t* buffers = NULL;
    QCameraHAL3TestPreview* testpre = (QCameraHAL3TestPreview*)cb;
    QCameraHAL3Device* device = testpre->mDevice;
    buffers = result->output_buffers;

    for (uint32_t i = 0;i < result->num_output_buffers ;i++) {
        int index = device->findStream(buffers[i].stream);
        CameraStream* stream = device->mCameraStreams[index];
        // Dump frame
        char fname[256];
        time_t timer;
        struct tm * t;
        time(&timer);
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW && testpre->mDumpPreviewNum > 0) {
            BufferInfo* info = stream->bufferManager->getBufferInfo(buffers[i].buffer);
            int size = info->size;
            void* data = info->vaddr;
            if (mFfbmPreviewCb != NULL) {
                (*mFfbmPreviewCb)(data, size);
                testpre->mDumpPreviewNum--;
                break;
            }
            t = localtime(&timer);
            sprintf(fname, "%s/camera_preview_%d_%4d_%02d_%02d_%02d_%02d_%02d.%s",
                CAMERA_STORAGE_DIR, result->frame_number,t->tm_year+1900,t->tm_mon+1,
                t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,"yuv");
            FILE* fd = fopen(fname, "wb");
            fwrite(data, size, 1, fd);
            fclose(fd);
            testpre->mDumpPreviewNum--;
        }
        // FPS Show
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW && mConfig->mShowFps) {
            showPreviewFPS();
        }
    }
    return ;
}

/************************************************************************
* name : initPreviewStream
* function: do prepare for preview stream.
************************************************************************/
int QCameraHAL3TestPreview::initPreviewStream()
{
    int res = 0;
    camera3_stream_t previewStream;
    std::vector<camera3_stream_t*> streams;
    int stream_num = 1;
    //init stream configure
    bool supportsPartialResults;
    uint32_t partialResultCount;
    std::vector<AvailableStream> outputPreviewStreams;
    printf("preview:%dx%d %d\n",mConfig->mPreviewStream.width,mConfig->mPreviewStream.height,
        mConfig->mPreviewStream.format);
    AvailableStream previewThreshold = {mConfig->mPreviewStream.width,
        mConfig->mPreviewStream.height,mConfig->mPreviewStream.format};
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

    previewStream.stream_type = CAMERA3_STREAM_OUTPUT;
    previewStream.width = outputPreviewStreams[0].width;
    previewStream.height = outputPreviewStreams[0].height;
    previewStream.format = outputPreviewStreams[0].format;
    previewStream.data_space = HAL_DATASPACE_UNKNOWN;
    previewStream.usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
    previewStream.rotation = 0;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    previewStream.max_buffers = 0;
    previewStream.priv = 0;


    streams.resize(stream_num);
    streams[PREVIEW_STREAM] = &previewStream;

    mDevice->setFpsRange(mConfig->mFpsRange[0], mConfig->mFpsRange[1]);
    mDevice->configureStreams(streams);
    if (mMetadataExt) {
        mDevice->setCurrentMeta(mMetadataExt);
        mDevice->constructDefaultRequestSettings(PREVIEW_STREAM,CAMERA3_TEMPLATE_PREVIEW);
    } else {
        mDevice->constructDefaultRequestSettings(PREVIEW_STREAM,CAMERA3_TEMPLATE_PREVIEW,true);
    }
    outputPreviewStreams.erase(outputPreviewStreams.begin(),
        outputPreviewStreams.begin() + outputPreviewStreams.size());
    return res;
}

int QCameraHAL3TestPreview::PreinitPreviewStream()
{
    int res = 0;
    camera3_stream_t previewStream;
    std::vector<camera3_stream_t*> streams;
    int stream_num = 1;

    printf("preview:%dx%d %d\n",mConfig->mPreviewStream.width,mConfig->mPreviewStream.height,
        mConfig->mPreviewStream.format);

    previewStream.stream_type = CAMERA3_STREAM_OUTPUT;
    previewStream.width = mConfig->mPreviewStream.width;
    previewStream.height = mConfig->mPreviewStream.height;
    previewStream.format = mConfig->mPreviewStream.format;
    previewStream.data_space = HAL_DATASPACE_UNKNOWN;
    previewStream.usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
    previewStream.rotation = 0;
    previewStream.max_buffers = PREVIEW_STREAM_BUFFER_MAX;
    previewStream.priv = 0;

    streams.resize(stream_num);
    streams[PREVIEW_STREAM] = &previewStream;

    mDevice->PreAllocateStreams(streams);

    return res;
}

/************************************************************************
* name : run
* function: interface for create snapshot thread.
************************************************************************/
void QCameraHAL3TestPreview::run()
{
    //open camera
    int res = 0;
    mDevice->setCallBack(this);
    res = initPreviewStream();
    CameraThreadData* resultThreadPreview = new CameraThreadData();
    CameraThreadData* requestThreadPreview = new CameraThreadData();
    requestThreadPreview->requestNumber[PREVIEW_STREAM] = REQUEST_NUMBER_UMLIMIT;
    mDevice->processCaptureRequestOn(requestThreadPreview,resultThreadPreview);
}

/************************************************************************
* name : stop
* function: interface for stop snapshot thread.
************************************************************************/
void QCameraHAL3TestPreview::stop()
{
    mDevice->stopStreams();
}


/************************************************************************
* name : dumpPreview
* function: set frame count of preveiw dump.
************************************************************************/
void QCameraHAL3TestPreview::dumpPreview(int count)
{
    mDumpPreviewNum = count;
}
