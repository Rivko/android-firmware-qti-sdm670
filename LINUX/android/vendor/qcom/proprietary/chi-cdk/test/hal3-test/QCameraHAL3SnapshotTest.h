////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APPSNAPSHOT_H__
#define __HAL3APPSNAPSHOT_H__

#include "QCameraHAL3VideoTest.h"

namespace qcamera
{
class QCameraHAL3SnapshotTest : public QCameraHAL3Test
{
private:
    hal3_camtest_meminfo_t mCaptureMemInfo;
    native_handle_t *mCaptureHandle;
    const camera_metadata_t *mMetaDataPtr[3];
    camera3_stream_t *mPreviewStream;
    camera3_stream_t *mSnapshotStream;
    camera3_capture_request mRequest;
    camera3_stream_buffer_t mSnapshotStreamBuffs;
    camera3_stream_configuration mSnapshotConfig;
public:
    int mRequestedCapture;
    QCameraHAL3SnapshotTest(int req_cap);
    void constructDefaultRequest(hal3_camera_test_obj_t *, int );
    void configureSnapshotStream(hal3_camera_test_obj_t *, int,
            int w, int h);
    void snapshotProcessCaptureRequest(hal3_camera_test_obj_t *my_test_obj,
            int camid);
    void snapshotAllocateBuffers(int, int );
    void initTest(hal3_camera_lib_test *my_test_obj,
            int testcase, int camid, int w, int h);
    bool snapshotThreadCreate(int testcase_id, void * (*hal3_thread_ops)(void *));
    void snapshotProcessCaptureRequestRepeat(hal3_camera_lib_test *my_hal3test_obj,
            int camid);
    void captureRequestRepeat(hal3_camera_lib_test *my_hal3test_obj, int camid, int testcase);
    void snapshotTestEnd(hal3_camera_lib_test *my_hal3test_obj, int camid);
    virtual ~QCameraHAL3SnapshotTest();
};

    void * hal3appSnapshotProcessBuffers(void *data);
}

#endif
