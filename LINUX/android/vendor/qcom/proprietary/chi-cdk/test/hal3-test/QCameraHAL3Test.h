////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APPTESTINIT_H__
#define __HAL3APPTESTINIT_H__

#include "QCameraHAL3MainTestContext.h"
#include "QCameraHAL3Base.h"

namespace qcamera {

class QCameraHAL3Test
{
    int  mCamId;
    int  mIonFd;
    bool mCaseStopFlag;
    int  mQueued;
public:
    QCameraHAL3Test(int cameraIndex);
    camera3_stream_t *requested_stream;
    camera3_stream_t *initStream(int streamtype,
            int camid, int w, int h, int format,int usage,int dataspace);

    camera3_stream_configuration configureStream(
            int opmode, int num_streams);
    virtual void captureRequestRepeat(hal3_camera_lib_test *, int, int);
    camera_metadata_t* hal3appGetDefaultRequest(int type);

    camera3_capture_request hal3appGetRequestSettings(
            camera3_stream_buffer_t *stream_buffs, int num_buffer);
    camera3_stream_buffer_t hal3appGetStreamBuffs(camera3_stream_t *req_stream);

    native_handle_t *allocateBuffers(int width, int height,
            hal3_camtest_meminfo_t *req_meminfo);
    void freeBuffers(native_handle_t *handle, hal3_camtest_meminfo_t *req_meminfo);
    bool processThreadCreate(void *obj, int testcase, hal3_camera_lib_test *);
    virtual ~QCameraHAL3Test();
};

    void * processBuffers(void *data);
}
#endif
