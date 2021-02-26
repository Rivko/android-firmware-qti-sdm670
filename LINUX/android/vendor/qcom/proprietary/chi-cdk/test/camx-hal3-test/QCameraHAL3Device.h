////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Device.h
/// @brief camera devices layer to contrl camera hardware ,camera3_device implementation
///        provide camera device to QCameraHAL3Test
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERADEVICE_H
#define _CAMERADEVICE_H
#include <camera/CameraMetadata.h>
#include <hardware/camera_common.h>
#include <hardware/camera3.h>
#include <hardware/gralloc.h>
#include <map>
#include <vector>
#include <pthread.h>
#include <list>
#include <utils/KeyedVector.h>
#include <utils/Timers.h>
#include <log/log.h>
#include <inttypes.h>
#include "BufferManager.h"

#define REQUEST_NUMBER_UMLIMIT -1
#define MAXSTREAM 3
#define PUT_BUFFER_EXTERNAL 1
#define PUT_BUFFER_INTERNAL 0
struct AvailableStream {
    int32_t width;
    int32_t height;
    int32_t format;
};
// Request and Result Pending
class RequestPending {
public:
    RequestPending();
    camera3_capture_request_t mRequest;
    int mNumOutputbuffer;
    int mNumMetadata;
};

// Callback for QCameraHAL3Device to upper layer
class DeviceCallback {
public:
    virtual void CapturePostProcess(DeviceCallback* cb, camera3_capture_result *result) = 0;
    virtual void HandleMetaData(DeviceCallback* cb, camera3_capture_result *result) = 0;
    virtual ~DeviceCallback() {}
};

class QCameraHAL3Device;
// Message data for PostProcess thread
typedef struct _CameraPostProcessMsg {
    camera3_capture_result result;
    std::vector<camera3_stream_buffer_t> streamBuffers;
    int stop;
}CameraPostProcessMsg;

typedef enum {
    START_STOP,
    REQUEST_CHANGE,
}MsgType;
// Message data for Request Thread
typedef struct _CameraRequestMsg {
    int msgType;
    int mask; //0x1 for stream0,0x10 for stream 1,0x11 for stream0 and 1
    int requestNumber[MAXSTREAM];
    int stop;
}CameraRequestMsg;

// Thread Data for camera Request and Result thread
typedef struct _CameraThreadData {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    std::list<void*> msgQueue;
    int requestNumber[MAXSTREAM];
    int skipPattern[MAXSTREAM];
    int frameNumber;
    int stopped;
    QCameraHAL3Device* device;
    void* priv;
    _CameraThreadData() {
        priv = NULL;
        stopped = 0;
        frameNumber = 0;
        for (int i = 0;i < MAXSTREAM;i++) {
            requestNumber[i] = 0;
            skipPattern[i] = 1;
        }
    }

}CameraThreadData;

// Stream info of CameraDevice
typedef struct _Stream {
    //Buffer
    BufferManager* bufferManager;
    //Metadata
    camera_metadata* metadata;
    //stream
    int streamId;
    int streamType;
    //RunTime
    camera3_stream_buffer_t streamBuffer;
}CameraStream;

class QCameraHAL3Device {
public:
    QCameraHAL3Device(camera_module_t* module, int cameraId,int mode = 0);
    ~QCameraHAL3Device();
    void openCamera();
    void closeCamera();
    void PreAllocateStreams(std::vector<camera3_stream_t*> streams);
    void configureStreams(std::vector<camera3_stream_t*> streams, int opMode = CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE);
    void constructDefaultRequestSettings(int index,camera3_request_template_t type,bool isCurrent = false);
    int processCaptureRequestOn(CameraThreadData* requestThread,CameraThreadData* resultThread);
    void flush();
    void setCallBack(DeviceCallback* mCallback);

    int getAvailableOutputStreams(std::vector<AvailableStream> &outputStreams,
        const AvailableStream *threshold);
    int findStream(camera3_stream_t* stream);
    int getPutBufferExt(){return mMode;}
    void setPutBufferExt(bool putbuf){ mMode = putbuf; }
    void setFpsRange(int min, int max)
    {
        mFpsRange[0] = min;
        mFpsRange[1] = max;
    };
    int updateMetadataForNextRequest(android::CameraMetadata* meta);
    int ProcessOneCaptureRequest(int* requestNumberOfEachStream,int* frameNumber);
    void stopStreams();
    void setCurrentMeta(android::CameraMetadata* meta);

    camera_metadata_t*             mCharacteristics;
    camera3_stream_configuration_t mStreamConfig;
    camera3_device_t*              mDevice;
    //Threads
    CameraThreadData*              mRequestThread;
    CameraThreadData*              mResultThread;

    CameraStream*                  mCameraStreams[MAXSTREAM];
    BufferManager*                 mBufferManager[MAXSTREAM];
    std::vector<camera3_stream_t*> mStreams;
    //capture result and notify to upper layer
    DeviceCallback*                mCallback;

    android::CameraMetadata        mCurrentMeta;
    int                            mLivingRequestExtAppend;
private:
    camera_module_t*               mModule;
    int                            mCameraId;
    int                            mMode;
    bool                           mHFRMode;
    int32_t                        mFpsRange[2];
    pthread_mutex_t                mSettingMetaLock;
    std::list<android::CameraMetadata> mSettingMetaQ;
    pthread_mutex_t                mPendingLock;
    pthread_cond_t                 mPendingCond;
    android::KeyedVector<int,RequestPending*>     mPendingVector;


    int getJpegBufferSize(uint32_t width, uint32_t height);

    class CallbackOps : public camera3_callback_ops {
    public:
        CallbackOps(QCameraHAL3Device *parent) :
            camera3_callback_ops({&ProcessCaptureResult, &Notify}),
            mParent(parent)
        {
        }
        static void ProcessCaptureResult(const camera3_callback_ops *cb,const camera3_capture_result *hal_result);
        static void Notify(const struct camera3_callback_ops *cb, const camera3_notify_msg_t *msg);

    private:
        QCameraHAL3Device*         mParent;
    };
    CallbackOps* mCBOps;
};
#endif

