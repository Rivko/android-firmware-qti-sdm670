////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestSnapshot.cpp
/// @brief Test for snapshot
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraHAL3TestSnapshot.h"
#define LOG_TAG "CameraTest"

using namespace android;
#define NONZSL_STABLE_FRAME_COUNT 4
#define JPEG_BLOB_ID 0xFF
#define PREVIEW_STREAM 0
#define SNAPSHOT_STREAM 1

/************************************************************************
* name : QCameraHAL3TestSnapshot
* function: construct object.
************************************************************************/
QCameraHAL3TestSnapshot::QCameraHAL3TestSnapshot(camera_module_t* module,
    QCameraHAL3Device* device,
    TestConfig* config)
{
    mModule = module;
    mConfig = config;
    mDevice = device;

    mDumpPreviewNum = 0;
    mFrameCount = 0;
    mLastFrameCount = 0;
    mLastFpsTime = 0;

    mSnapshotNum = 0;
    pthread_mutex_init(&mCaptureSyncMutex,NULL);
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&mCaptureSyncCond, &attr);
    pthread_condattr_destroy(&attr);
    mMetadataExt = NULL;
    mFfbmSnapshotCb = NULL;
}

/************************************************************************
* name : ~QCameraHAL3TestSnapshot
* function: destory object.
************************************************************************/
QCameraHAL3TestSnapshot::~QCameraHAL3TestSnapshot()
{
    pthread_mutex_destroy(&mCaptureSyncMutex);
}

/************************************************************************
* name : getCurrentMeta
* function: Get current metadata setting.
************************************************************************/
android::CameraMetadata* QCameraHAL3TestSnapshot::getCurrentMeta()
{
    return &(mDevice->mCurrentMeta);
}

/************************************************************************
* name : setCurrentMeta
* function: Set A External metadata as current metadata. this is used by nonZSL snapshot now
************************************************************************/
void QCameraHAL3TestSnapshot::setCurrentMeta(android::CameraMetadata* meta)
{
    mMetadataExt = meta;
}
/************************************************************************
* name : updataMetaDatas
* function: update request command.
************************************************************************/
void QCameraHAL3TestSnapshot::updataMetaDatas(android::CameraMetadata* meta)
{
    mDevice->updateMetadataForNextRequest(meta);
}

/************************************************************************
* name : HandleMetaData
* function: analysis meta info from capture result.
************************************************************************/
void QCameraHAL3TestSnapshot::HandleMetaData(DeviceCallback* cb, camera3_capture_result *result)
{
    ALOGI("HandleMetaData Enter\n");
    const camera3_stream_buffer_t* buffers = NULL;
    QCameraHAL3TestSnapshot* testsnap = (QCameraHAL3TestSnapshot*)cb;
    QCameraHAL3Device* device = testsnap->mDevice;
    sp<VendorTagDescriptor> vTags =
        android::VendorTagDescriptor::getGlobalVendorTagDescriptor();

    if (result->partial_result >=1)
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
            if (exptime != mConfig->mMetaStat.expTime) {
                mConfig->mMetaStat.expTime = exptime;
                mConfig->mDump->print("frame:%d exposure value = %llu\n",
                    result->frame_number,exptime);
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
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_AE_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                aemode = entry.data.u8[0];
            }

            if (aemode != mConfig->mMetaStat.aeMode) {
                switch (aemode)
                {
                case ANDROID_CONTROL_AE_MODE_OFF:
                    mConfig->mDump->print("frame:%d ae mode:AE MODE OFF\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AE_MODE_ON:
                    mConfig->mDump->print("frame:%d ae mode:AE MODE ON\n",
                        result->frame_number);
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
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_CONTROL_AWB_MODE, &entry);
            if ((0 == res) && (entry.count > 0)) {
                awbmode = entry.data.u8[0];
            }
            if (awbmode != mConfig->mMetaStat.awbMode) {
                switch (awbmode)
                {
                case ANDROID_CONTROL_AWB_MODE_OFF:
                    mConfig->mDump->print("frame:%d awb mode:AWB MODE OFF\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AWB_MODE_AUTO:
                    mConfig->mDump->print("frame:%d awb mode:AWB MODE AUTO\n",
                        result->frame_number);
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
                    mConfig->mDump->print("frame:%d af mode:AF MODE OFF\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AF_MODE_AUTO:
                    mConfig->mDump->print("frame:%d af mode:AF MODE AUTO\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_AF_MODE_MACRO:
                    mConfig->mDump->print("frame:%d af mode:AF MODE MACRO\n",
                        result->frame_number);
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
                    mConfig->mDump->print("frame:%d control mode:OFF\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_MODE_AUTO:
                    mConfig->mDump->print("frame:%d control mode:AUTO\n",
                        result->frame_number);
                    break;
                case ANDROID_CONTROL_MODE_USE_SCENE_MODE:
                    mConfig->mDump->print("frame:%d control mode:ON\n",
                        result->frame_number);
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
                    "frame:%d asdresults   Backlight %d, Landscape %d, Snow %d, Hdr %d\n",\
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
                mConfig->mDump->print("frame:%d is hdr scene = %d\n",
                    result->frame_number,hdrmode);
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
                if (zsl != mConfig->mMetaStat.zslMode) {
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
                    mConfig->mDump->print("frame:%d selelt priority:%d\n",
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
        if (mConfig->mMetaDump.jpegquality) {
            camera_metadata_ro_entry entry;
            int jpeg_quality = 0;
            int res = 0;
            res = find_camera_metadata_ro_entry(result->result,
                ANDROID_JPEG_QUALITY, &entry);
            if ((0 == res) && (entry.count > 0)) {
                jpeg_quality = entry.data.u8[0];
            }
            if (jpeg_quality != mConfig->mMetaStat.jpegquality) {
                mConfig->mDump->print("frame:%d jpegquality value = %d mMetaStat %d\n",
                    result->frame_number,jpeg_quality, mConfig->mMetaStat.jpegquality);
                mConfig->mMetaStat.jpegquality = jpeg_quality;
            }
        }
    }
}

/************************************************************************
* name : showPreviewFPS
* function: show preview frame FPS.
************************************************************************/
void QCameraHAL3TestSnapshot::showPreviewFPS()
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

void QCameraHAL3TestSnapshot::setFfbmSnapshotCb(void (*cb)(void*, int)) {
    ALOGI("%s:%d register", __func__, __LINE__);
    mFfbmSnapshotCb = cb;
}

/************************************************************************
* name : CapturePostProcess
* function: callback for postprocess capture result.
************************************************************************/
void QCameraHAL3TestSnapshot::CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result)
{
    ALOGI("CapturePostProcess Enter\n");
    const camera3_stream_buffer_t* buffers = NULL;
    QCameraHAL3TestSnapshot* testsnap = (QCameraHAL3TestSnapshot*)cb;
    QCameraHAL3Device* device = testsnap->mDevice;
    buffers = result->output_buffers;
    for (uint32_t i = 0;i < result->num_output_buffers ;i++) {
        int index = device->findStream(buffers[i].stream);
        CameraStream* stream = device->mCameraStreams[index];
        // Dump frame
        char fname[256];
        time_t timer;
        struct tm * t;
        time(&timer);
        if (stream->streamType == CAMERA3_TEMPLATE_STILL_CAPTURE) {
            struct Camera3JPEGBlob jpegBlob;
            //if nonZSL mode skip several frames
            if (!(!mConfig->mZslEnabled && result->frame_number < NONZSL_STABLE_FRAME_COUNT)
                && mSnapshotNum > 0) {
                BufferInfo* info = stream->bufferManager->getBufferInfo(buffers[i].buffer);
                void* data = info->vaddr;
                uint8_t* outBuffer = (uint8_t*)info->vaddr;
                int size = info->size;
                if (mFfbmSnapshotCb != NULL) {
                    (*mFfbmSnapshotCb)(data, size);
                    mSnapshotNum--;
                    if (!mConfig->mZslEnabled && mSnapshotNum == 0){
                        pthread_mutex_lock(&mCaptureSyncMutex);
                        pthread_cond_signal(&mCaptureSyncCond);
                        pthread_mutex_unlock(&mCaptureSyncMutex);
                    }
                    break;
                }
                t = localtime(&timer);
                sprintf(fname, "%s/camera_capture_%d_%4d_%02d_%02d_%02d_%02d_%02d.%s",
                    CAMERA_STORAGE_DIR, result->frame_number,t->tm_year+1900,t->tm_mon+1,
                    t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,"jpg");
                size_t jpeg_eof_offset = (size_t)(size - (size_t)sizeof(jpegBlob));
                uint8_t* jpeg_eof = &outBuffer[jpeg_eof_offset];
                memcpy(&jpegBlob, jpeg_eof, sizeof(Camera3JPEGBlob));

                if (jpegBlob.JPEGBlobId == JPEG_BLOB_ID) {
                    mSnapshotNum--;
                    FILE* fd = fopen(fname, "w");
                    fwrite(data, jpegBlob.JPEGBlobSize, 1, fd);
                    fclose(fd);
                } else {
                    ALOGE("Failed to Get Picture size:%d\n",size);
                    FILE* fd = fopen(fname, "w");
                    fwrite(data, size, 1, fd);
                    fclose(fd);
                }
                ALOGI("Get One Picture %d Last\n",mSnapshotNum);
                if (!mConfig->mZslEnabled && mSnapshotNum == 0) {
                    pthread_mutex_lock(&mCaptureSyncMutex);
                    pthread_cond_signal(&mCaptureSyncCond);
                    pthread_mutex_unlock(&mCaptureSyncMutex);
                }
            }
        }
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW && testsnap->mDumpPreviewNum > 0) {
            t = localtime(&timer);
            testsnap->mDumpPreviewNum--;
            sprintf(fname, "%s/camera_preview_%d_%4d_%02d_%02d_%02d_%02d_%02d.%s",
                CAMERA_STORAGE_DIR,result->frame_number,t->tm_year+1900,t->tm_mon+1,
                t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec,"yuv");
            FILE* fd = fopen(fname, "wb");
            BufferInfo* info = stream->bufferManager->getBufferInfo(buffers[i].buffer);
            int size = info->size;
            void* data = info->vaddr;
            fwrite(data, size, 1, fd);
            fclose(fd);
        }
        // FPS Show
        if (stream->streamType == CAMERA3_TEMPLATE_PREVIEW && mConfig->mShowFps) {
            showPreviewFPS();
        }
    }
}

/************************************************************************
* name : initSnapshotStreams
* function: private function for init streams for snapshot.
************************************************************************/
int QCameraHAL3TestSnapshot::initSnapshotStreams()
{
    int res = 0;
    camera3_stream_t previewStream;
    camera3_stream_t snapshotStream;
    std::vector<camera3_stream_t*> streams;
    int stream_num = 2;
    //init stream configure
    bool supportsPartialResults;
    uint32_t partialResultCount;
    std::vector<AvailableStream> outputPreviewStreams;
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
    // for Full Test UseCase
    if (mConfig->mZslEnabled){
        previewStream.usage = GRALLOC_USAGE_HW_COMPOSER | GRALLOC_USAGE_HW_TEXTURE;
    } else {
        previewStream.usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
    }
    previewStream.rotation = 0;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    previewStream.max_buffers = 0;
    previewStream.priv = 0;

    std::vector<AvailableStream> outputSnapshotStreams;
    AvailableStream snapshotThreshold = {mConfig->mSnapshotStream.width,
        mConfig->mSnapshotStream.height,mConfig->mSnapshotStream.format};
    if (res == 0){
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

    snapshotStream.stream_type = CAMERA3_STREAM_OUTPUT;
    snapshotStream.width = outputSnapshotStreams[0].width;
    snapshotStream.height = outputSnapshotStreams[0].height;
    snapshotStream.format = outputSnapshotStreams[0].format;
    snapshotStream.data_space = HAL_DATASPACE_JFIF;
    snapshotStream.usage = GRALLOC_USAGE_SW_READ_OFTEN;
    snapshotStream.rotation = 0;
    // Fields to be filled by HAL (max_buffers, priv) are initialized to 0
    snapshotStream.max_buffers = 0;
    snapshotStream.priv = 0;

    streams.resize(stream_num);
    streams[PREVIEW_STREAM] = &previewStream;
    streams[SNAPSHOT_STREAM] = &snapshotStream;

    mDevice->setPutBufferExt(PUT_BUFFER_INTERNAL);
    mDevice->setFpsRange(mConfig->mFpsRange[0], mConfig->mFpsRange[1]);
    mDevice->configureStreams(streams);
    mDevice->constructDefaultRequestSettings(PREVIEW_STREAM,CAMERA3_TEMPLATE_PREVIEW,
        mConfig->mZslEnabled ? true: false);
    mDevice->constructDefaultRequestSettings(SNAPSHOT_STREAM,CAMERA3_TEMPLATE_STILL_CAPTURE);
    if (!mConfig->mZslEnabled) {
        mDevice->setCurrentMeta(mMetadataExt);
    }
    uint8_t jpegquality = JPEG_QUALITY_DEFAULT;
    android::CameraMetadata* metaUpdate = getCurrentMeta();
    (*metaUpdate).update(ANDROID_JPEG_QUALITY,&(jpegquality),sizeof(jpegquality));
    updataMetaDatas(metaUpdate);
    return res;
}

int QCameraHAL3TestSnapshot::PreinitSnapshotStreams()
{
    int res = 0;
    ALOGI("preinit snapshot streams start\n");
    camera3_stream_t previewStream;
    camera3_stream_t snapshotStream;
    std::vector<camera3_stream_t*> streams;
    int stream_num = 2;

    previewStream.stream_type = CAMERA3_STREAM_OUTPUT;
    previewStream.width = mConfig->mPreviewStream.width;
    previewStream.height = mConfig->mPreviewStream.height;
    previewStream.format = mConfig->mPreviewStream.format;
    previewStream.data_space = HAL_DATASPACE_UNKNOWN;
    // for Full Test UseCase
    if (mConfig->mZslEnabled) {
        previewStream.usage =
            GRALLOC_USAGE_HW_COMPOSER | GRALLOC_USAGE_HW_TEXTURE;
    } else {
        previewStream.usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
    }
    previewStream.rotation = 0;
    previewStream.max_buffers = PREVIEW_STREAM_BUFFER_MAX;
    previewStream.priv = 0;

    snapshotStream.stream_type = CAMERA3_STREAM_OUTPUT;
    snapshotStream.width = mConfig->mSnapshotStream.width;
    snapshotStream.height = mConfig->mSnapshotStream.height;
    snapshotStream.format = mConfig->mSnapshotStream.format;
    snapshotStream.data_space = HAL_DATASPACE_JFIF;
    snapshotStream.usage = GRALLOC_USAGE_SW_READ_OFTEN;
    snapshotStream.rotation = 0;
    snapshotStream.max_buffers = SNAPSHOT_STREAM_BUFFER_MAX;
    snapshotStream.priv = 0;

    streams.resize(stream_num);
    streams[PREVIEW_STREAM] = &previewStream;
    streams[SNAPSHOT_STREAM] = &snapshotStream;

    mDevice->PreAllocateStreams(streams);
    ALOGI("preinit snapshot streams end\n");
    return res;
}

/************************************************************************
* name : run
* function: interface for create snapshot thread.
************************************************************************/
void QCameraHAL3TestSnapshot::run()
{
    //open camera
    int res = 0;
    printf("CameraId:%d\n",mConfig->mCameraId);
    mDevice->setCallBack(this);
    res = initSnapshotStreams();
    CameraThreadData* resultThread = new CameraThreadData();

    CameraThreadData* requestThread = new CameraThreadData();
    if (mConfig->mZslEnabled)
        requestThread->requestNumber[PREVIEW_STREAM] = REQUEST_NUMBER_UMLIMIT;
    else
        requestThread->requestNumber[PREVIEW_STREAM] = 0;
    requestThread->requestNumber[SNAPSHOT_STREAM] = 0;
    mDevice->processCaptureRequestOn(requestThread,resultThread);
}

/************************************************************************
* name : stop
* function:  stop all the thread and release the device object.
************************************************************************/
void QCameraHAL3TestSnapshot::stop()
{
    mDevice->stopStreams();
}

/************************************************************************
* name : RequestCaptures
* function: populate capture request.
************************************************************************/
int QCameraHAL3TestSnapshot::RequestCaptures(StreamCapture request)
{
    // send a message to request thread
    int res = 0;
    pthread_mutex_lock(&mDevice->mRequestThread->mutex);
    CameraRequestMsg* msg = new CameraRequestMsg();
    memset(msg,0,sizeof(CameraRequestMsg));
    msg->requestNumber[SNAPSHOT_STREAM] = request.count +
        (mConfig->mZslEnabled ? 0 : NONZSL_STABLE_FRAME_COUNT);
    msg->mask = 1 << SNAPSHOT_STREAM;
    msg->msgType = REQUEST_CHANGE;
    mSnapshotNum = request.count;
    mDevice->mRequestThread->msgQueue.push_back(msg);
    ALOGI("Msg for capture picture mask%x \n",msg->mask);
    pthread_cond_signal(&mDevice->mRequestThread->cond);
    pthread_mutex_unlock(&mDevice->mRequestThread->mutex);
    if (!mConfig->mZslEnabled) {
       pthread_mutex_lock(&mCaptureSyncMutex);
       ALOGI("nonZSL snapshot %d \n",mSnapshotNum);
       struct timespec tv;
       clock_gettime(CLOCK_MONOTONIC,&tv);
       tv.tv_sec += (NONZSL_STABLE_FRAME_COUNT+request.count) * 2;
       if (pthread_cond_timedwait(&mCaptureSyncCond, &mCaptureSyncMutex,&tv) != 0) {
           ALOGE("ERROR: nonZSL snapshot Timeout \n");
           res = -1;
       }
       pthread_mutex_unlock(&mCaptureSyncMutex);
       stop();
    }
    return res;
}

/************************************************************************
* name : dumpPreview
* function: set frame count of preveiw dump.
************************************************************************/
void QCameraHAL3TestSnapshot::dumpPreview(int count)
{
    mDumpPreviewNum += count;
}
