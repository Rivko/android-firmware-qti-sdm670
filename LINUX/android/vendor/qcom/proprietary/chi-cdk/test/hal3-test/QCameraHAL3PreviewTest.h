////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APPPREVIEWTEST_H__
#define __HAL3APPPREVIEWTEST_H__

#include "QCameraHAL3Base.h"
#include "QCameraHAL3Test.h"
#include <vector>
#include <list>

namespace qcamera {

class HAL3TestCaseStreamInfo {
public:
    unsigned int              mWidth;
    unsigned int              mHeight;
    hal3_camtest_meminfo_t   *mMeminfo;
    native_handle_t         **mHandle;
    camera3_stream_t         *mStream;
    std::list<uint32_t>       mQueue;
    bool                      isVideo;
    const camera_metadata_t  *mMetaDataPtr;
    int                       count;
    struct timeval            prevtv;

};

class QCameraHAL3ConfigTest:public QCameraHAL3Test,
                            public camera3_callback_ops
{
private:
    camera3_capture_request               mRequest;
    android::CameraMetadata               mStreamSettings;
    camera3_stream_configuration          mConfig;
    buffer_thread_t                       mThread;
    camera3_stream_buffer_t               *mBuffs;
    CameraHAL3Base                        *mHAL3Base;
    int                                   mCamId;
    pthread_mutex_t                       mLock;
    bool                                  mCaseStopFlag;
    int                                   mQueued;
    int                                   mRequestedSnapshots;
    float                                 mFPS;
public:
    int                                   mPipelineDepth;
    std::vector<HAL3TestCaseStreamInfo*>  vTestStreams;
    HAL3TestCaseStreamInfo*               mTestSnapshotStream;
    int                                   mNumSnapshotBuffers;
    int                                   mRuntimeSnapshot;
    QCameraHAL3ConfigTest(int cameraId, CameraHAL3Base* HAL3Base);

    ~QCameraHAL3ConfigTest();
    void Initialize(int cameraId, const std::vector<HAL3TestStreamInfo>& infos,
      const HAL3TestSnapshotInfo *snapshotInfo);
    void Deinitialize();
    void testStart();
    void testMakeSnapshot();
    void testEnd();
    int testThreadCreate();
    void testProcessBuffers();
    void captureRequestRepeat(hal3_camera_lib_test *, int, int);

    static void Notify(const camera3_callback_ops *, const camera3_notify_msg *);
    static void processCaptureResult(const camera3_callback_ops *,
        const camera3_capture_result *);
    void handleNotify(const camera3_callback_ops *cb,
        const camera3_notify_msg *msg);
    void handleCaptureResult(const camera3_callback_ops *cb,
        const camera3_capture_result *result);
};

class QCameraHAL3PreviewTest : public QCameraHAL3Test
{
private:
    hal3_camtest_meminfo_t mPreviewMeminfo;
    native_handle_t **mPreviewHandle;
    hal3_camtest_meminfo_t mCaptureMemInfo;
    native_handle_t *mCaptureHandle;
    const camera_metadata_t *mMetaDataPtr[3];
    camera3_stream_t *mPreviewStream;
    camera3_stream_t *mSnapshotStream;
    camera3_capture_request mRequest;
    android::CameraMetadata hal3app_preview_settings;
    camera3_stream_buffer_t mPreviewStreamBuffs;
    camera3_stream_buffer_t mSnapshotStreamBuffs;
    camera3_stream_configuration mPreviewConfig;
public:
    int width;
    int height;
    int nobuffer;
    int mPipelineDepthPreview;
    int mCamId;
    bool ir_mode;
    bool svhdr_mode;
    bool mCaseStopFlag;
    buffer_thread_t  mThread;
    int  mQueued;
    QCameraHAL3PreviewTest(int cameraIndex);
    void configurePreviewStream(hal3_camera_test_obj_t *my_test_obj, int camid,
            int w, int h);

    void snapshotCaptureRequest(hal3_camera_lib_test *handle,
            int testcase, int camid, int w, int h);
    void constructDefaultRequest(hal3_camera_test_obj_t *, int);
    void initTest(hal3_camera_lib_test *handle,
            int testcase, int camid, int w, int h);

    void captureRequestRepeat(hal3_camera_lib_test *, int, int);
    void previewAllocateBuffers(int,
            int, int);
    bool previewProcessThreadCreate(hal3_camera_lib_test *);
    void previewTestEnd(hal3_camera_lib_test *my_hal3test_obj, int camid);
    void snapshotAllocateBuffers(int, int );
    virtual ~QCameraHAL3PreviewTest();
    int get_ir_mode(int ir_mode);
    int get_svhdr_mode(int hdr_mode);
};

    void * hal3appPreviewProcessBuffers(void *);
}
#endif
