////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestPreview.h
/// @brief preview only mode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_PREVIEW_
#define _QCAMERA_HAL3_TEST_PREVIEW_
#include "QCameraHAL3Test.h"

class QCameraHAL3TestPreview :public DeviceCallback, public QCameraHAL3Test{
public:
    QCameraHAL3TestPreview(camera_module_t* module,QCameraHAL3Device* device, TestConfig* config);
    ~QCameraHAL3TestPreview();
    void run();
    void stop();
    void dumpPreview(int count);
    void CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result) override;
    void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) override;
    void updataMetaDatas(android::CameraMetadata* meta);
    android::CameraMetadata* getCurrentMeta();
    void setCurrentMeta(android::CameraMetadata* meta);
    typedef void (*ffbmPreviewCb)(void*, int);
    void setFfbmPreviewCb(ffbmPreviewCb);
    ffbmPreviewCb mFfbmPreviewCb;

    int mDumpPreviewNum;
    int PreinitPreviewStream();
private:
    int mFrameCount;
    int mLastFrameCount;
    nsecs_t mLastFpsTime;
    android::CameraMetadata* mMetadataExt;

    int initPreviewStream();
    void showPreviewFPS();
};
#endif
