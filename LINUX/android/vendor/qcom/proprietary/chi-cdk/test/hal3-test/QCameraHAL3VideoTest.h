////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APPVIDEOTEST_H__
#define __HAL3APPVIDEOTEST_H__

#include "QCameraHAL3Base.h"
#include "QCameraHAL3Test.h"


namespace qcamera {


class QCameraHAL3VideoTest : public QCameraHAL3Test
{

private:
    hal3_camtest_meminfo_t mVideoMeminfo;
    native_handle_t **mVideoHandle;
    hal3_camtest_meminfo_t mCaptureMemInfo;
    native_handle_t *mCaptureHandle;
    const camera_metadata_t *mMetaDataPtr[3];
    camera3_stream_t *mVideoStream;
    camera3_stream_t *mSnapshotStream;
    camera3_capture_request mRequest;
    camera3_stream_buffer_t mVideoStreamBuffs;
    android::CameraMetadata hal3app_video_settings;
    camera3_stream_buffer_t mSnapshotStreamBuffs;
    camera3_stream_configuration mVideoConfig;
public:
    int width;
    int height;
    int nobuffer;
    int mPipelineDepthVideo;
    int mCameraId;
    int32_t binning_mode;

    QCameraHAL3VideoTest(int cameraIndex);

    void configureVideoStream(hal3_camera_test_obj_t *,
            int, int, int);
    void snapshotCaptureRequest(hal3_camera_lib_test *handle,int testcase,
            int camid, int w, int h);

    void constructDefaultRequest(hal3_camera_test_obj_t *,
            int);

    void captureRequestRepeat(hal3_camera_lib_test *, int, int);
    void initTest(hal3_camera_lib_test *handle, int testcase, int camid,
            int w, int h);
    void vidoeAllocateBuffers(int, int, int);

    bool videoProcessThreadCreate(hal3_camera_lib_test *);
    void videoTestEnd(hal3_camera_lib_test *my_hal3test_obj, int camid);
    void snapshotAllocateBuffers(int, int );
    int get_binning_mode(int32_t binning_mode);
    virtual ~QCameraHAL3VideoTest();
};

    void * hal3appVideoProcessBuffers(void *);
}

#endif
