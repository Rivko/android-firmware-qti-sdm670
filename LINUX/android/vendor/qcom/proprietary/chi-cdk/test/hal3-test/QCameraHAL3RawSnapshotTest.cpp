////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Copyright (c) 2016, The Linux Foundation. All rights reserved.*/

#include "QCameraHAL3RawSnapshotTest.h"
#include "QCameraHAL3MainTestContext.h"

namespace qcamera {
hal3_camera_lib_test *RawSnapshot_CamObj_handle;
extern int fcount_captured;
extern pthread_mutex_t TestAppLock;
extern std::list<uint32_t> RawQueue;
bool stop_flag = false;
int queued = 0;
buffer_thread_t gthread;




QCameraHAL3RawSnapshotTest::QCameraHAL3RawSnapshotTest(int req_cap) :
    QCameraHAL3Test(0),
    mRawCaptureHandle(NULL),
    mRawSnapshotStream(NULL),
    mRequestedCapture(req_cap)
{

}

void QCameraHAL3RawSnapshotTest::initTest(hal3_camera_lib_test *handle,
        int testcase, int camid, int w, int h)
{
    RawSnapshot_CamObj_handle = handle;
    LOGD("\n Raw buffer thread created for testcase : %d", testcase);
    configureRawSnapshotStream(&(handle->test_obj), camid, w, h);
    constructDefaultRequest(&(handle->test_obj), 0);
    mRequest.frame_number = 0;
    LOGD("\nRaw  Snapshot stream configured");
    rawProcessCaptureRequest(&(handle->test_obj), 0);
    LOGD("\nRaw  Snapshot Process Capture Request Sent");
    LOGD("\n Raw Snapshot Default stream setting read");
    rawProcessThreadCreate(MENU_START_RAW_CAPTURE,
            rawProcessBuffers);
}

void QCameraHAL3RawSnapshotTest::constructDefaultRequest(
        hal3_camera_test_obj_t *my_test_obj, int camid)
{
    camera3_device_t *device_handle = my_test_obj->device;
    LOGD("Camera ID : %d",camid);
    mMetaDataPtr[0] = device_handle->ops->construct_default_request_settings(
            my_test_obj->device, CAMERA3_TEMPLATE_PREVIEW);
    mMetaDataPtr[1] = device_handle->ops->construct_default_request_settings(
            my_test_obj->device, CAMERA3_TEMPLATE_STILL_CAPTURE);
}

void QCameraHAL3RawSnapshotTest::configureRawSnapshotStream(hal3_camera_test_obj_t *my_test_obj,
                                    int camid, int w, int h)
{
    camera3_device_t *device_handle = my_test_obj->device;
    LOGD(" configureSnapshotStream testcase dim :%d  X %d", w, h);
    mPreviewStream = new camera3_stream_t;
    mRawSnapshotStream = new camera3_stream_t;

    memset(mPreviewStream, 0, sizeof(camera3_stream_t));
    memset(mRawSnapshotStream, 0, sizeof(camera3_stream_t));

    mPreviewStream = initStream(CAMERA3_STREAM_OUTPUT, camid, PREVIEW_WIDTH, PREVIEW_HEIGHT, 0,
            HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL3_DATASPACE_UNKNOWN);
    mRawSnapshotStream = initStream(CAMERA3_STREAM_OUTPUT, camid, RAWSNAPSHOT_CAPTURE_WIDTH,
            RAWSNAPSHOT_CAPTURE_HEIGHT, 0, HAL_PIXEL_FORMAT_RAW16, HAL3_DATASPACE_ARBITRARY);
    mRawSnapshotConfig = configureStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, 2);

    mRawSnapshotConfig.streams[0] = mPreviewStream;
    mRawSnapshotConfig.streams[1] = mRawSnapshotStream;
    device_handle->ops->configure_streams(my_test_obj->device, &(mRawSnapshotConfig));
}


void QCameraHAL3RawSnapshotTest::rawProcessCaptureRequest(
        hal3_camera_test_obj_t *my_test_obj, int camid)
{
    int width, height, num;

    camera3_device_t *device_handle = my_test_obj->device;
    width = mRawSnapshotStream->width;
    height = mRawSnapshotStream->height;
    mRawCaptureHandle = new native_handle_t *[8];
    for (int i = 0; i < 8; i++)
    {
        rawAllocateBuffers(width, height, i);
        RawQueue.push_back(i);
    }
    mRequest.frame_number = 0;
    mRequest.settings = mMetaDataPtr[1];
    mRequest.input_buffer = NULL;
    mRequest.num_output_buffers = 1;
    mRawSnapshotStreamBuffs.stream = mRawSnapshotStream;
    mRawSnapshotStreamBuffs.status = 0;
	num = RawQueue.front();
    RawQueue.pop_front();
    mRawSnapshotStreamBuffs.buffer = (const native_handle_t**)&mRawCaptureHandle[num];
    mRawSnapshotStreamBuffs.release_fence = -1;
    mRawSnapshotStreamBuffs.acquire_fence = -1;
    mRequest.output_buffers = &(mRawSnapshotStreamBuffs);
    LOGD("Calling HAL3APP capture request for camid : %d", camid);
    device_handle->ops->process_capture_request(my_test_obj->device, &(mRequest));
}

void QCameraHAL3RawSnapshotTest::rawProcessCaptureRequestRepeat(
        hal3_camera_lib_test *my_hal3test_obj, int camid)
{
    hal3_camera_test_obj_t *my_test_obj = &(my_hal3test_obj->test_obj);
	int num1, num2;
    LOGD("\nRaw Requested Capture : %d and Received Capture : %d",
            mRequestedCapture, fcount_captured);
    if (0) {
        LOGD("\n Raw Snapshot is running successfully Ending test");
        fflush(stdout);
        LOGD("\n Capture Done , Recieved Frame : %d", fcount_captured);
        rawTestEnd(my_hal3test_obj, camid);
    }
    else {
        camera3_device_t *device_handle = my_test_obj->device;
        (mRequest.frame_number)++;
        mRequest.settings = mMetaDataPtr[1];
        mRequest.input_buffer = NULL;
        mRequest.num_output_buffers = 1;
        mRawSnapshotStreamBuffs.stream = mRawSnapshotStream;
        num2 = RawQueue.front();
        RawQueue.pop_front();
        mRawSnapshotStreamBuffs.buffer = (const native_handle_t**)&mRawCaptureHandle[num2];
        mRawSnapshotStreamBuffs.release_fence = -1;
        mRawSnapshotStreamBuffs.acquire_fence = -1;
        mRequest.output_buffers = &(mRawSnapshotStreamBuffs);
        LOGD("Calling HAL3APP repeat capture request repeat ");
        device_handle->ops->process_capture_request(my_test_obj->device, &(mRequest));
    }
}

void QCameraHAL3RawSnapshotTest::rawTestEnd(
        hal3_camera_lib_test *my_hal3test_obj, int camid)
{
    buffer_thread_msg_t msg;
    extern pthread_mutex_t gCamLock;
    hal3_camera_test_obj_t *my_test_obj = &(my_hal3test_obj->test_obj);
    camera3_device_t *device_handle = my_test_obj->device;
    device_handle->ops->flush(my_test_obj->device);
    LOGD("%s Closing Camera %d", __func__, camid);
    /* Free the Allocated ION Memory */
    ioctl(mRawCaptureMemInfo.ion_fd, ION_IOC_FREE, &mRawCaptureMemInfo.ion_handle);
    close(mRawCaptureMemInfo.ion_fd);
    mRawCaptureMemInfo.ion_fd = -1;
	stop_flag = 1;
    /* Close the Thread */
    memset(&msg, 0, sizeof(buffer_thread_msg_t));
    msg.stop_thread = 1;
    write(pfd[1], &msg, sizeof(buffer_thread_msg_t));
}


void QCameraHAL3RawSnapshotTest::rawAllocateBuffers(int width, int height, int num)
{
    mRawCaptureHandle[num] = allocateBuffers(width, height, &mRawCaptureMemInfo);
}

bool QCameraHAL3RawSnapshotTest::rawProcessThreadCreate(int testcase_id,
        void *(*hal3_thread_ops)(void *))
{
    int32_t ret = 0;

    pthread_attr_t attr;
    if (pipe(pfd) < 0) {
        LOGE("%s:Test:%d Error in creating the pipe", __func__, testcase_id);
    }
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&gthread.mutex, NULL);
    pthread_cond_init(&gthread.cond, NULL);
    gthread.is_thread_started = 0;
    gthread.readfd = pfd[0];
    gthread.writefd = pfd[1];
    gthread.data_obj = this;
    gthread.stop_flag = &stop_flag;
    gthread.queued  = &queued;
    pthread_mutex_lock(&gthread.mutex);
    ret = pthread_create(&gthread.td, &attr, hal3_thread_ops, &gthread );
    pthread_setname_np(gthread.td, "TestApp_Thread");
    if (ret < 0) {
        LOGE("Failed to create status thread");
        return 0;
    }
    while(gthread.is_thread_started == 0) {
        pthread_cond_wait(&gthread.cond, &gthread.mutex);
    }
    pthread_mutex_unlock(&gthread.mutex);
    return 1;
}

void QCameraHAL3RawSnapshotTest::captureRequestRepeat(
        hal3_camera_lib_test *my_hal3test_obj, int camid, int testcase)
{
    if(my_hal3test_obj == NULL) {
        LOGD("camid :%d and testcase : %d handle is NULL", camid, testcase);
    }
}

void * rawProcessBuffers(void *data) {
    buffer_thread_t *thread = (buffer_thread_t*)data;
    int32_t readfd, writefd;
    hal3_camera_lib_test *hal3_test_handle;
    pthread_mutex_lock(&thread->mutex);
    struct timespec ts1;
    thread->is_thread_started = 1;
    QCameraHAL3RawSnapshotTest *obj;
    hal3_test_handle = RawSnapshot_CamObj_handle;
    obj = (QCameraHAL3RawSnapshotTest *)thread->data_obj;
    pthread_cond_signal(&thread->cond);
    pthread_mutex_unlock(&thread->mutex);
    bool rthread_exit = 0;
    int32_t ready = 0;
    while(!rthread_exit) {
        pthread_mutex_lock(&thread->mutex);
        if (*thread->stop_flag == true) {
            pthread_mutex_unlock(&thread->mutex);
            break;
        }
        if (*thread->queued > 8*2) {
            clock_gettime(CLOCK_REALTIME, &ts1);
            ts1.tv_sec += 10L;
            pthread_cond_timedwait(&thread->cond, &thread->mutex, &ts1);
        }
        pthread_mutex_unlock(&thread->mutex);
        obj->rawProcessCaptureRequestRepeat(hal3_test_handle, 0);
    }
    LOGD("Sensor thread is exiting");
    pthread_mutex_unlock(&TestAppLock);
    pthread_exit(0);
    return NULL;
}

QCameraHAL3RawSnapshotTest::~QCameraHAL3RawSnapshotTest()
{

}

}
