////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/

#include "QCameraHAL3MainTestContext.h"
#include "QCameraHAL3SnapshotTest.h"
#include "QCameraHAL3RawSnapshotTest.h"
#include "QCameraHAL3PreviewTest.h"
#include <sys/time.h>

extern "C" {
extern int set_camera_metadata_vendor_ops(const vendor_tag_ops_t *query_ops);
}

namespace qcamera {

QCameraHAL3PreviewTest *mPreviewtestCase = NULL;
QCameraHAL3VideoTest *mVideotestCase = NULL;
QCameraHAL3SnapshotTest *mSnapshottestCase = NULL;
QCameraHAL3RawSnapshotTest *mRawSnapshottestCase = NULL;

#define QCAMERA3_IR_AVAILABLE_MODES 1

struct timeval start_time;
int capture_received;
int pfd[2];
extern int test_case_end;
extern int snapshot_buffer;
struct timeval prevtv;
int32_t count;
pthread_cond_t mRequestAppCond;
std::list<uint32_t> PreviewQueue;
std::list<uint32_t> VideoQueue;
std::list<uint32_t> RawQueue;

pthread_mutex_t TestAppLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mCaptureRequestLock = PTHREAD_MUTEX_INITIALIZER;

#define FPS_TIME_INTERVAL 3000000

static void camera_device_status_change(
        const struct camera_module_callbacks* callbacks,
        int camera_id, int new_status)
{
    /* Stub function */
    if (callbacks == NULL) {
        LOGD("Parameters are NULL %d %d", camera_id, new_status);
    }
}

static void torch_mode_status_change(
        const struct camera_module_callbacks* callbacks,
        const char* camera_id, int new_status)
{
    /* Stub function */
    if((callbacks == NULL) || (camera_id == NULL)) {
        LOGD("Parameters are NULL %d", new_status);
    }
}

static void Notify(
        const camera3_callback_ops *cb,
        const camera3_notify_msg *msg)
{
    /* Stub function */
    if((cb == NULL) || (msg == NULL)) {
        LOGD("Parameters are NULL ");
    }
}

static void ProcessCaptureResult(
        const camera3_callback_ops *cb,
        const camera3_capture_result *result)
{
    buffer_thread_msg_t msg;
    extern CameraHAL3Base *mCamHal3Base;
    int frame_num;
    extern int req_sent;
    extern int preview_buffer_allocated;
    extern int video_buffer_allocated;
    int num;
    if(cb == NULL) {
        LOGD("callback returned is NULL");
    }
    LOGD("Cam Capture Result Callback %d and %d",
            result->num_output_buffers, mCamHal3Base->mFrameCount);
    if (mCamHal3Base->mTestCaseSelected == MENU_START_PREVIEW ||
            mCamHal3Base->mTestCaseSelected == MENU_START_VIDEO) {
        if (result->num_output_buffers == 1) {
            frame_num = result->frame_number;
            LOGD("Frame width:%d and height:%d and format:%d",
                    result->output_buffers->stream->width,
                    result->output_buffers->stream->height,
                    result->output_buffers->stream->format);
            (mCamHal3Base->mFrameCount)++;
            LOGD("Preview/Video Capture Result %d and fcount: %d and req_Sent:%d and %d ",
            result->num_output_buffers, mCamHal3Base->mFrameCount, req_sent, result->frame_number);
            if (test_case_end == 0) {
                if (mCamHal3Base->mTestCaseSelected == MENU_START_PREVIEW) {
                    num = (result->frame_number)%preview_buffer_allocated;
                    PreviewQueue.push_back(num);
                }
                else {
                    num = (result->frame_number)%video_buffer_allocated;
                           VideoQueue.push_back(num);
                }
                pthread_cond_signal(&mRequestAppCond);
                memset(&msg, 0, sizeof(buffer_thread_msg_t));
            }
        }
    }
    else {
        extern int fcount_captured;
        if (result->num_output_buffers == 1) {
            LOGD("snapshot/Raw Capture1 Result Callback %d and %d",
                    result->num_output_buffers, fcount_captured);
            (mCamHal3Base->mFrameCount)++;
            fcount_captured++;
            num = (result->frame_number) % 8;
            RawQueue.push_back(num);
            LOGD("\n Capture %d done preparing for capture ", fcount_captured);
            if (1) {
                struct timeval tv;
                gettimeofday(&tv, NULL);
                uint64_t time_diff = (uint64_t)((tv.tv_sec * 1000000 + tv.tv_usec) -
                                     (prevtv.tv_sec * 1000000 + prevtv.tv_usec));

                 count++;
                 if (time_diff >= FPS_TIME_INTERVAL) {
                     float framerate = (count * 1000000) / (float)time_diff;
                     LOGD(" %s:stream fps: = %0.2f",
                      __func__,framerate);
                     prevtv = tv;
                     count = 0;
                 }
            }
        }
    }
}

CameraHAL3Base::CameraHAL3Base(int cameraIndex) :
    mCameraIndex(cameraIndex),
    mLibHandle(NULL),
    mFrameCount(0),
    mSecElapsed(1),
    mTestCaseSelected(0),
    mPreviewRunning(0),
    mVideoRunning(0),
    mSnapShotRunning(0),
    binning_mode(0)
{

}


int CameraHAL3Base::hal3appCameraTestLoad()
{
    int rc = HAL3_CAM_OK;
    int numCam = 0;
    int32_t res = 0;
    hal3_camera_test_obj_t *my_test_obj;
    mLibHandle = new hal3_camera_lib_test;
    memset(mLibHandle, 0, sizeof(hal3_camera_lib_handle));
    rc = hal3appTestLoad(&mLibHandle->app_obj);
    camera_module_t *my_if_handle = mLibHandle->app_obj.hal3_lib.halModule_t;
    if (HAL3_CAM_OK != rc) {
        LOGE("hal3 err\n");
        goto EXIT;
    }

    numCam = my_if_handle->get_number_of_cameras();
    printf("\n Number of Cameras are : %d ", numCam);
    if (my_if_handle->get_vendor_tag_ops) {
        mLibHandle->app_obj.mVendorTagOps = vendor_tag_ops_t();
        my_if_handle->get_vendor_tag_ops(&(mLibHandle->app_obj.mVendorTagOps));

        res = set_camera_metadata_vendor_ops(&(mLibHandle->app_obj.mVendorTagOps));
        if (0 != res) {
            printf("%s: Could not set vendor tag descriptor, "
                    "received error %s (%d). \n", __func__,
                    strerror(-res), res);
            goto EXIT;
        }
    }
    my_test_obj = &(mLibHandle->test_obj);
    my_test_obj->module_cb.torch_mode_status_change = &torch_mode_status_change;
    my_test_obj->module_cb.camera_device_status_change = &camera_device_status_change;
    EXIT:
    return numCam;

}

void CameraHAL3Base::display_capability()
{
    ALOGE("Camera Here");
    int i;
    int *available_ir_modes = NULL;
    if(hal3app_cam_settings.exists(QCAMERA3_IR_AVAILABLE_MODES)) {
        ALOGE("\n mrad check1 ");
        entry_hal3app = hal3app_cam_settings.find(QCAMERA3_IR_AVAILABLE_MODES);
        available_ir_modes = (int *) malloc((entry_hal3app.count)*sizeof(int ));
        for(i =0;i < (int)entry_hal3app.count; i++){
                    available_ir_modes[i] = entry_hal3app.data.i32[i];
            ALOGE("\n mrad cap %d ", available_ir_modes[i]);

        }
    }
}

int CameraHAL3Base::hal3appCameraLibOpen(int camid, camera3_callback_ops *cb)
{
    int rc;
    rc = hal3appCamOpen(&mLibHandle->app_obj, (int)camid, &(mLibHandle->test_obj));
    if (rc != HAL3_CAM_OK) {
        LOGE("hal3appCamOpen() camidx=%d, err=%d\n",
                camid, rc);
        goto EXIT;
    }
    rc = hal3appCamInitialize((int)camid, &mLibHandle->test_obj, cb);
    EXIT:
    return rc;
}

int CameraHAL3Base::hal3appCameraLibClose()
{
    int rc;
    rc = hal3appCamClose(&(mLibHandle->test_obj));
    return rc;
}

int CameraHAL3Base::hal3appTestLoad(hal3_camera_app_t *my_hal3_app)
{
    hw_get_module(CAMERA_HARDWARE_MODULE_ID,
            (const hw_module_t **)&my_hal3_app->hal3_lib.halModule_t);
    return HAL3_CAM_OK;
}


int CameraHAL3Base::hal3appCameraTestUnload()
{
  int rc;
  return HAL3_CAM_OK;
}

int CameraHAL3Base::hal3appCamOpen(
        hal3_camera_app_t *my_hal3_app,
        int camid,
        hal3_camera_test_obj_t *my_test_obj)
{
    const char *camIds[] = {"0", "1", "2"};
    if (camid < 0 || camid >= sizeof(camIds)/sizeof(camIds[0])) {
      printf("\nInvalid Camera ID %d\n", camid);
      return HAL3_CAM_E_GENERAL;
    }

    camera_module_t *my_if_handle = my_hal3_app->hal3_lib.halModule_t;
    my_if_handle->set_callbacks(&(my_test_obj->module_cb));
    my_if_handle->get_camera_info(camid, &(my_test_obj->cam_info));
    camcap_info = my_test_obj->cam_info;
    hal3app_cam_settings = (camcap_info.static_camera_characteristics);
    display_capability();
    my_if_handle->common.methods->open(&(my_if_handle->common), camIds[camid],
            reinterpret_cast<hw_device_t**>(&(my_test_obj->device)));
    printf("\n Camera ID %s Opened \n", camIds[camid]);
    return HAL3_CAM_OK;
}

int CameraHAL3Base::hal3appCamClose(
        hal3_camera_test_obj_t *my_test_obj)
{
    my_test_obj->device->common.close(&my_test_obj->device->common);
    printf("\n Camera ID %d Closed \n", mCameraIndex);
    return HAL3_CAM_OK;
}

int CameraHAL3Base::hal3appCamInitialize(int camid, hal3_camera_test_obj_t *my_test_obj,
    camera3_callback_ops *cb)
{
    int rc = 0;
    camera3_device_t *device_handle = my_test_obj->device;
    camera3_callback_ops *cbOps = cb;
    if(!cbOps) {
        my_test_obj->callback_ops.notify = &Notify;
        my_test_obj->callback_ops.process_capture_result = &ProcessCaptureResult;
        cbOps = &(my_test_obj->callback_ops);
    }
    rc = device_handle->ops->initialize(my_test_obj->device, cbOps);
    if (rc != HAL3_CAM_OK) {
        LOGE("hal3appCamInitialize() camidx=%d, err=%d\n",
                camid, rc);
        goto EXIT;
    }
    EXIT:
    return rc;
}


void CameraHAL3Base::hal3appCheckStream(int testcase, int camid)
{
    if (testcase != MENU_START_PREVIEW) {
        if (mPreviewtestCase != NULL) {
            mPreviewtestCase->previewTestEnd(mLibHandle, camid);
            delete mPreviewtestCase;
            mPreviewtestCase = NULL;
        }
    }
    if (testcase != MENU_START_VIDEO){
        if (mVideotestCase != NULL) {
            mVideotestCase->videoTestEnd(mLibHandle, camid);
            delete mVideotestCase;
            mVideotestCase = NULL;
        }
    }

    if (testcase != MENU_START_CAPTURE){
        if (mSnapshottestCase != NULL) {
            delete mSnapshottestCase;
            mSnapshottestCase = NULL;
        }
    }

    if (testcase != MENU_START_RAW_CAPTURE) {
        if (mRawSnapshottestCase != NULL) {
            delete mRawSnapshottestCase;
            mRawSnapshottestCase = NULL;
        }
    }
}


int CameraHAL3Base::hal3appCameraConfigCaseInit(int testcase, int camid, int w, int h)
{

  return HAL3_CAM_OK;
}

int CameraHAL3Base::hal3appCameraPreviewInit(int testcase, int camid, int w, int h)
{
    extern int req_sent;
    int testCaseEndComplete = 0;
    if (w == 0 || h == 0) {
        printf("\n Frame dimension is wrong");
        return -1;
    }
    if ( mPreviewtestCase != NULL) {
        if(testcase == MENU_TOGGLE_IR_MODE) {
            ALOGE("\n IR mode requested is :%d", ir_mode);
            mPreviewtestCase->ir_mode = ir_mode;
        }
        if(testcase == MENU_TOGGLE_SVHDR_MODE) {
            ALOGE("\n SVHDR mode requested is :%d", svhdr_mode);
            mPreviewtestCase->svhdr_mode = svhdr_mode;
        }
        return 0;
    }
    else {
        testCaseEndComplete = 0;
        do {
            if (mVideoRunning == 1) {
                hal3appCheckStream(MENU_START_PREVIEW, camid);
            }
            pthread_mutex_lock(&TestAppLock);
            mTestCaseSelected = MENU_START_PREVIEW;
            if (mVideoRunning != 1) {
                hal3appCheckStream(MENU_START_PREVIEW, camid);
            }
            mPreviewtestCase = new QCameraHAL3PreviewTest(0);
            printf("\n\n Testing the Resolution : %d X %d", w, h);
            req_sent = 0;
            PreviewQueue.clear();
            capture_received = 0; mSecElapsed = 1;
            snapshot_buffer = -1; mFrameCount = 0;
            mPreviewtestCase->width = w; mPreviewtestCase->height = h;
            mPreviewtestCase->ir_mode = 0; mPreviewtestCase->svhdr_mode = 0;
            mPreviewtestCase->initTest(mLibHandle,
                    (int) MENU_START_PREVIEW, camid, w, h);
            testCaseEndComplete = 1;
        }while(testCaseEndComplete != 1);
    }
    return 0;
}

void CameraHAL3Base::hal3appPreviewEndTest(hal3_camera_lib_test *handle, int camid)
{
    mPreviewtestCase->previewTestEnd(mLibHandle, camid);
    delete mPreviewtestCase;
}

int CameraHAL3Base::hal3appCameraVideoInit(int testcase, int camid, int w, int h)
{
    extern int req_sent;
    int testCaseEndComplete = 0;
    if (w == 0 || h == 0) {
        printf("\n Frame dimension is wrong");
        return -1;
    }

    if (mVideotestCase != NULL) {
        if(testcase == MENU_TOGGLE_BINNING_CORRECTION) {
            ALOGE("\n Binning Mode Requested is :%d", binning_mode);
            mVideotestCase->binning_mode = binning_mode;
        }
        return 0;
    }
    else {
        testCaseEndComplete = 0;
        do {
            if (mPreviewRunning == 1) {
                hal3appCheckStream(MENU_START_VIDEO, camid);
            }
            pthread_mutex_lock(&TestAppLock);
            mTestCaseSelected = MENU_START_VIDEO;
            if (mPreviewRunning != 1) {
                hal3appCheckStream(MENU_START_VIDEO, camid);
            }
            mVideotestCase = new QCameraHAL3VideoTest(0);
            VideoQueue.clear();
            printf("\n\nTesting the Resolution : %d X %d", w, h);
            req_sent = 0;
            capture_received =0; mSecElapsed = 1; test_case_end = 0;
            mVideotestCase->width = w; mVideotestCase->height = h;
            snapshot_buffer = -1; mFrameCount = 0;
            mVideotestCase->initTest(mLibHandle,
                    (int) MENU_START_VIDEO, camid, w, h);
            testCaseEndComplete = 1;
        }while(testCaseEndComplete !=1);
    }
    return 0;
}


int CameraHAL3Base::hal3appRawCaptureInit(hal3_camera_lib_test *handle, int camid, int req_cap)
{
    int testCaseEndComplete = 0;
    if(handle == NULL) {
        LOGE("Camera Handle is NULL");
    }
    if (mSnapShotRunning != 1) {
        hal3appCheckStream(MENU_START_RAW_CAPTURE, camid);
    }
    testCaseEndComplete = 0;
    do {
        pthread_mutex_lock(&TestAppLock);
        if (mSnapShotRunning == 1) {
            hal3appCheckStream(MENU_START_RAW_CAPTURE, camid);
        }
        printf("\n capture:%d", req_cap);
        mTestCaseSelected = MENU_START_RAW_CAPTURE;
        mRawSnapshottestCase = new QCameraHAL3RawSnapshotTest(req_cap);
        mRawSnapshottestCase->mRequestedCapture = req_cap;
        mRawSnapshottestCase->initTest(mLibHandle,
        (int) MENU_START_RAW_CAPTURE, camid, RAWSNAPSHOT_CAPTURE_WIDTH,
                RAWSNAPSHOT_CAPTURE_HEIGHT);
        testCaseEndComplete = 1;
    }while(testCaseEndComplete !=1);
    return 0;
}

void CameraHAL3Base::hal3appRawEndTest(hal3_camera_lib_test *handle, int camid)
{
    mRawSnapshottestCase->rawTestEnd(mLibHandle, camid);
}

int CameraHAL3Base::hal3appCameraCaptureInit(hal3_camera_lib_test *handle,
        int camid, int req_cap)
{
    int testCaseEndComplete = 0;
    if(handle == NULL) {
        LOGE("Camera Handle is NULL");
    }
    if (mSnapShotRunning != 1) {
        hal3appCheckStream(MENU_START_CAPTURE, camid);
    }
    testCaseEndComplete = 0;
    do {
        pthread_mutex_lock(&TestAppLock);
        if (mSnapShotRunning == 1) {
            hal3appCheckStream(MENU_START_CAPTURE, camid);
        }
        printf("\n capture:%d", req_cap);
        mTestCaseSelected = MENU_START_CAPTURE;
        mSnapshottestCase = new QCameraHAL3SnapshotTest(req_cap);
        mSnapshottestCase->mRequestedCapture = req_cap;
        mSnapshottestCase->initTest(mLibHandle,
            (int) MENU_START_CAPTURE, camid, SNAPSHOT_CAPTURE_WIDTH, SNAPSHOT_CAPTURE_HEIGHT);
        testCaseEndComplete = 1;
    }while(testCaseEndComplete != 1);
    return 0;
}

}

