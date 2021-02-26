////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/


#ifndef __HAL3APP_H__
#define __HAL3APP_H__

#include <hardware/hardware.h>
#include <dlfcn.h>
#include <hardware/camera3.h>
#include <sys/stat.h>
#include <ctype.h>
#include <list>
#include <camera/CameraMetadata.h>
#include <linux/msm_ion.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <poll.h>
extern "C" {
#include "QCameraDBG.h"
}


#define HAL3_DATASPACE_UNKNOWN 0x0
#define HAL3_DATASPACE_ARBITRARY 0x1
#define HAL3_DATASPACE_JFIF 0x101
#define FLAGS_VIDEO_ENCODER 0x00010000

#define PREVIEW_WIDTH 1920
#define PREVIEW_HEIGHT 1080

#define VIDEO_WIDTH 1920
#define VIDEO_HEIGHT 1080

#define SNAPSHOT_CAPTURE_WIDTH 5344
#define SNAPSHOT_CAPTURE_HEIGHT 4008

#define RAWSNAPSHOT_CAPTURE_WIDTH 3864
#define RAWSNAPSHOT_CAPTURE_HEIGHT 2180

namespace qcamera {

#define MAX_NUM_CAMERAS 3
enum HAL3TestStreamType {
  HAL3_TEST_STREAM_TYPE_YUV,
  HAL3_TEST_STREAM_TYPE_RAW,
  HAL3_TEST_STREAM_TYPE_JPEG,
  HAL3_TEST_STREAM_TYPE_IDEAL_RAW,
  HAL3_TEST_STREAM_TYPE_MAX,
};

class HAL3TestSnapshotInfo {
public:
  HAL3TestStreamType  type;
  int32_t             camera_id;
  int                 width;
  int                 height;
  int                 count;

};

class HAL3TestStreamInfo {
public:
  int32_t camera_id;
  int width;
  int height;
  float fps;
  HAL3TestStreamType type;
  bool isvideo;

};

typedef enum {
    HAL3_CAM_OK,
    HAL3_CAM_E_GENERAL,
    HAL3_CAM_E_NO_MEMORY,
    HAL3_CAM_E_NOT_SUPPORTED,
    HAL3_CAM_E_INVALID_INPUT,
    HAL3_CAM_E_INVALID_OPERATION,
    HAL3_CAM_E_ENCODE,
    HAL3_CAM_E_BUFFER_REG,
    HAL3_CAM_E_PMEM_ALLOC,
    HAL3_CAM_E_CAPTURE_FAILED,
    HAL3_CAM_E_CAPTURE_TIMEOUT,
} hal3_camera_status_type_t;

typedef enum {
    PROCESS_BUFFER,
} buffer_thread_msg_type_t;

typedef struct {
    camera3_device_t *device;
    camera3_callback_ops callback_ops;
    struct camera_info cam_info;
    camera_module_callbacks_t module_cb;
} hal3_camera_test_obj_t;

typedef struct {
    int fd;
    int ion_fd;
    ion_user_handle_t ion_handle;
    size_t size;
    void *vaddr;
} hal3_camtest_meminfo_t;

typedef struct {
    buffer_thread_msg_type_t msg;
    bool stop_thread;
    int num;
} buffer_thread_msg_t;

typedef struct {
    void *ptr;
    camera_module_t* halModule_t;
} hal3_interface_lib_t;

typedef struct {
    uint8_t num_cameras;
    vendor_tag_ops_t mVendorTagOps;
    hal3_interface_lib_t hal3_lib;
} hal3_camera_app_t;

typedef struct {
    hal3_camera_app_t app_obj;
    hal3_camera_test_obj_t test_obj;
} hal3_camera_lib_test;

typedef struct {
    pthread_t td;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int32_t readfd;
    int32_t writefd;
    void *data_obj;
    bool is_thread_started;
    int testcase;
    void* camera_obj;
    bool *stop_flag;
    int *queued;
} buffer_thread_t;

extern int32_t pfd[2];
typedef hal3_camera_lib_test hal3_camera_lib_handle;
static void ProcessCaptureResult(
        const camera3_callback_ops *cb,
        const camera3_capture_result *result);

class CameraHAL3Base
{
    friend class MainTestContext;
protected:
    int mCameraIndex;
public:
    CameraHAL3Base();
    CameraHAL3Base(int cameraIndex);
    hal3_camera_lib_test *mLibHandle;
    int mFrameCount;
    int fps;
    int mSecElapsed;
    int mTestCaseSelected;
    int mPreviewRunning;
    int mVideoRunning;
    int mSnapShotRunning;
    bool ir_mode;
    bool svhdr_mode;
    uint8_t binning_mode;
    camera_info camcap_info;
    camera_metadata_entry entry_hal3app;
    android::CameraMetadata hal3app_cam_settings;
    int hal3appCamInitialize(int camid, hal3_camera_test_obj_t *,
            camera3_callback_ops *);
    void hal3appCamCapabilityGet(hal3_camera_lib_test *handle,int camid);
    int hal3appCameraLibOpen(int, camera3_callback_ops *);
    int hal3appCameraLibClose();
    int hal3appTestLoad(hal3_camera_app_t *);
    int hal3appCamOpen(hal3_camera_app_t *,
             int,
             hal3_camera_test_obj_t *);
    int hal3appCamClose(hal3_camera_test_obj_t *);

    int hal3appCameraConfigCaseInit(int, int, int, int);
    int hal3appCameraPreviewInit(int, int, int, int);
    int hal3appCameraVideoInit(int, int camid, int w, int h);
    void display_capability();
    int hal3appCameraCaptureInit(hal3_camera_lib_test *, int, int);
    int hal3appRawCaptureInit(hal3_camera_lib_test *handle, int camid, int );
    int hal3appCameraTestLoad();
    int hal3appCameraTestUnload();
    void hal3appCheckStream(int testcase, int camid);
    void hal3appPreviewEndTest(hal3_camera_lib_test *handle, int camid);
    void hal3appRawEndTest(hal3_camera_lib_test *handle, int camid);

};

}
#endif
