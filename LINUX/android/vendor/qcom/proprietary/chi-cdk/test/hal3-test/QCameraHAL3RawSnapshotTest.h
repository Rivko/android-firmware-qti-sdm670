////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APPRAWSNAPSHOT_H__
#define __HAL3APPRAWSNAPSHOT_H__

#include "QCameraHAL3SnapshotTest.h"
#include "QCameraHAL3Base.h"
#include "QCameraHAL3Test.h"
#include <vector>
#include <list>


namespace qcamera
{
class QCameraHAL3RawSnapshotTest : public QCameraHAL3Test
{
private:
    hal3_camtest_meminfo_t mRawCaptureMemInfo;
    native_handle_t ** mRawCaptureHandle;
    const camera_metadata_t *mMetaDataPtr[3];
    camera3_stream_t *mPreviewStream;
    camera3_stream_t *mRawSnapshotStream;
    camera3_capture_request mRequest;
    camera3_stream_buffer_t mRawSnapshotStreamBuffs;
    camera3_stream_configuration mRawSnapshotConfig;
public:
    int mRequestedCapture;
    QCameraHAL3RawSnapshotTest(int req_cap);
    void constructDefaultRequest(hal3_camera_test_obj_t *my_test_obj,
            int camid);
    void configureRawSnapshotStream(hal3_camera_test_obj_t *my_test_obj,
            int camid, int, int );
    void rawProcessCaptureRequest(hal3_camera_test_obj_t *my_test_obj,
            int camid);
    void rawProcessCaptureRequestRepeat(hal3_camera_lib_test *my_hal3test_obj,
            int camid);
    void initTest(hal3_camera_lib_test *handle, int testcase, int, int, int);
    bool rawProcessThreadCreate(int testcase_id,
            void *(*hal3_thread_ops)(void *));
    void rawAllocateBuffers(int height, int width, int num);
    void rawTestEnd(hal3_camera_lib_test *my_hal3test_obj, int camid);
    void captureRequestRepeat(hal3_camera_lib_test *my_hal3test_obj, int camid, int testcase);
    virtual ~QCameraHAL3RawSnapshotTest();
};
    void * rawProcessBuffers(void *data);
}
#endif
