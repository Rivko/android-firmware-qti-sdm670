////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestVideo.h
/// @brief test video case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_VIDEO_
#define _QCAMERA_HAL3_TEST_VIDEO_
#include "QCameraHAL3Device.h"
#include "QCameraHAL3Test.h"
#ifdef ENABLE_VIDEO_ENCODER
#include "QCameraTestVideoEncoder.h"
#endif

typedef enum {
    VIDEO_MODE_NORMAL = 0, // for normal 1~30fps
    VIDEO_MODE_HFR60,  // for HFR 30~60fps
    VIDEO_MODE_HFR120, // for HFR 60~120fps
    VIDEO_MODE_MAX,
}VideoMode;

class QCameraHAL3TestVideo :public DeviceCallback, public QCameraHAL3Test
{
public:
    QCameraHAL3TestVideo(camera_module_t* module,QCameraHAL3Device* device,TestConfig* config);
    ~QCameraHAL3TestVideo();
    void run();
    void stop();
    void CapturePostProcess(DeviceCallback* cb,camera3_capture_result *hal_result) override;
    void EnqueueFrameBuffer(CameraStream *stream,buffer_handle_t *buf_handle);
    void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) {};
    void updataMetaDatas(android::CameraMetadata* meta);
    android::CameraMetadata* getCurrentMeta();
    void setCurrentMeta(android::CameraMetadata* meta);
    void RequestCaptures(StreamCapture requst);
    void dumpPreview(int count);
    void dumpVideo(int count);
    int PreinitVideoStreams();
private:
    #ifdef ENABLE_VIDEO_ENCODER
    QCameraTestVideoEncoder* mVideoEncoder;
    #endif
    int                      mIsVideo;
    int                      mDumpcount;
    int                      mDumppreviewcount;
    int                      mDumpvideocount;
    bool                     mAllowDumpVideo;
    bool                     mAllowDumpPreview;
    int                      mFrameCount;
    int                      mLastFrameCount;
    nsecs_t                  mLastFpsTime;
    android::CameraMetadata* mMetadataExt;
    int                      mVideoFrameCount;
    int                      mVideoLastFrameCount;
    nsecs_t                  mVideoLastFpsTime;
    VideoMode                mVideoMode;
    bool                     mIsStoped;
    int initVideoStreams();
    void showPreviewFPS();
    void showVideoFPS();
    void selectOpMode(uint32_t *operation_mode,int width,int height,int fps);
};
#endif
