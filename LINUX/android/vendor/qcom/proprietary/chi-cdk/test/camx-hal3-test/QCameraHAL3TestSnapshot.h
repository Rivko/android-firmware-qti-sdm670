////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestSnapshot.h
/// @brief test snapshot case
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_SNAPSHOT_
#define _QCAMERA_HAL3_TEST_SNAPSHOT_
#include "QCameraHAL3Test.h"

struct Camera3JPEGBlob {
    uint16_t JPEGBlobId;
    uint32_t JPEGBlobSize;
};
class QCameraHAL3TestSnapshot :public DeviceCallback, public QCameraHAL3Test{
public:
    QCameraHAL3TestSnapshot(camera_module_t* module, QCameraHAL3Device* device, TestConfig* config);
    ~QCameraHAL3TestSnapshot();
    void run();
    void stop();
    int RequestCaptures(StreamCapture requst);
    void dumpPreview(int count);
    void CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result) override;
    void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) override;
    void updataMetaDatas(android::CameraMetadata* meta);
    android::CameraMetadata* getCurrentMeta();
    void setCurrentMeta(android::CameraMetadata* meta);
    typedef void (*ffbmSnapshotCb)(void*, int);
    void setFfbmSnapshotCb(ffbmSnapshotCb);

    ffbmSnapshotCb mFfbmSnapshotCb;
    int PreinitSnapshotStreams();
private:
    int                      mDumpPreviewNum;
    int                      mFrameCount;
    int                      mLastFrameCount;
    nsecs_t                  mLastFpsTime;
    android::CameraMetadata* mMetadataExt;

    pthread_mutex_t          mCaptureSyncMutex;
    pthread_cond_t           mCaptureSyncCond;
    int                      mSnapshotNum;
    int initSnapshotStreams();
    void showPreviewFPS();

};
#endif
