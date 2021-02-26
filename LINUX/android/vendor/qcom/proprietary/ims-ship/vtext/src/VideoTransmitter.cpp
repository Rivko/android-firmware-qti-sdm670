/*****************************************************************************
============================
Copyright (c)  2004-2012,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoTransmitter.cpp
 Description: Implementation of VideoTransmitter

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-Dec-11   Rakesh K             First Version
 *****************************************************************************/
#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
#include "ImsCameraInternalInterface.h"
#include <VideoTransmitter.h>
#include <media/msm_media_info.h>
#if defined(VENUS_OMX)
#include <media/msm_media_info.h>
#endif
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <media/IMediaPlayerService.h>
#include <OMX_QCOMExtns.h>
//#include <media/stagefright/foundation/ADebug.h>
//#include <media/stagefright/MediaDefs.h>
//#include <media/stagefright/OMXClient.h>
#include <media/IOMX.h>
#include <android/IGraphicBufferSource.h>
#include <binder/MemoryDealer.h>
#include <OMX_Component.h>
#include <semaphore.h>
#include <HardwareAPI.h>
#include <qdMetaData.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/OMXBuffer.h>

//below needed for GPU Rotation on HAL1
#include <qdMetaData.h>
#include <gralloc_priv.h>

#include <hidlmemory/mapping.h>
#include <android/hidl/allocator/1.0/IAllocator.h>
typedef hidl::allocator::V1_0::IAllocator TAllocator;
typedef ::android::hidl::memory::V1_0::IMemory TMemory;
#ifdef UNIQUE_FILE_NO
#undef UNIQUE_FILE_NO
#define UNIQUE_FILE_NO 102
#endif



#ifdef DEBUG_ENC
#define DLOG LOGE
#else
#define DLOG
#endif
//#define GPU_ROTATION
#if defined(MSM8974) || defined(MSM8226)
#include <media/msm_media_info.h>
#endif
#define RATE_CONTROL


typedef struct {
  sp<GraphicBuffer> buffer;
  sp<Fence> fence;
  bool skip;
  uint64_t slotid;
  bool used;

} CameraBufferSlot;
#define CameraBufferSize 25

static int Camerahead;
static int Cameratail;
static CameraBufferSlot camerabuffers[CameraBufferSize];
pthread_mutex_t CameraBuffer_mutex;


//shamu
sp<IGraphicBufferProducer> RecordingBufferProducer;
sp<IGraphicBufferConsumer> RecordingBufferConsumer;
extern uint64_t modem_time;
extern uint64_t apps_time;

//extern char useModemTime[1];
//extern bool useSystemTime;


FILE *fpRawSend;
char storeDecoded[PROPERTY_VALUE_MAX];
FILE *fpEncSend;
char storeEncoded[PROPERTY_VALUE_MAX];

//extern int avtimer_dev_node;
//extern uint64_t getCurrentAVTime();



#ifdef ANDROID_VT_JB
extern char videoInjection[1];
#else
char videoInjection[PROPERTY_VALUE_MAX];
#endif


#define CAMERA_TS_MAP_SIZE 30
#define VT_ACTUAL_BUFF_COUNT  10

typedef struct {
  sp<GraphicBuffer> CameraBuffer;
  sp<GraphicBuffer> LocalBuffer;
  uint64_t CameraBufferId;
  uint64_t LocalBufferId;
  bool   UsedByIOMX;
  sp<Fence> CameraFence;
} EncoderBufferSlot;

typedef struct {
  sp<GraphicBuffer> outBuffer;
  native_handle_t *nh;
  bool   UsedByIOMX;
  IOMX::buffer_id bufferid;
  private_handle_t *src;
} EncoderNativeHandleBuffer;


#define EncoderBufferSize 12


static int Encoderhead;
static int Encodertail;
static EncoderBufferSlot RotationBuffer[EncoderBufferSize];
static EncoderNativeHandleBuffer EncoderNativeBuffer[EncoderBufferSize];
pthread_mutex_t EncoderBuffer_mutex;
pthread_mutex_t RecordingConsumer_mutex;
static CameraTStampMap cameraTSMap[CAMERA_TS_MAP_SIZE];
static int cameraTSMapIndex = 0;
static int cameraTSMapCount = 0;
nsecs_t previouststamp = 0;
#define CAMER_BUFFER_COUNT 10
VideoCameraforHal1 mHal1;


//HAL1 APIs

void VideoCameraforHal1::registerCameraCallbacks(boolean flag)
{
  CRITICAL1("registerCameraCallbacks");

  if (flag) {
    registerCallbacks(engine_camera_notify_callback,
                      NULL,
                      NULL,
                      engine_camera_handle_callback_timestamp,
                      engine_custom_params_callback,
                      NULL);
  } else {
    registerCallbacks(NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL);
  }
}

void VideoCameraforHal1::engine_custom_params_callback(void* params)
{
  CustomCallbackParam* callbackParam = (CustomCallbackParam*)params;
  VideoTransmitter *pTransmitter = NULL;
  pTransmitter = mHal1.getClient();

  if (pTransmitter == NULL) {
    // ERROR1("[VideoCameraforHal1]engine_custom_params_callback: Dropping camera custom callback");
    // return;
  }

  CRITICAL2("engine_custom_params_callback callbackParam->type : %d",
            callbackParam->type);

  if (callbackParam->type == CAMERA_ROTATION_CROP_PARAM) {
    CameraParameters *param = (CameraParameters *)
                              callbackParam->cropRotationParam.cameraParam;
    int rotation = 0;
    int portrait = 0;
    rotation = callbackParam->cropRotationParam.cameraMount;
    int cameraFacing = callbackParam->cropRotationParam.cameraFacing;
    int cameraConfigHeight = callbackParam->cropRotationParam.height;
    int cameraConfigWidth = callbackParam->cropRotationParam.width;

    if (pTransmitter != NULL) {
    pTransmitter->cameraHeight = cameraConfigHeight;
    pTransmitter->cameraWidth = cameraConfigWidth;
    }

    if (callbackParam->cropRotationParam.width <
        callbackParam->cropRotationParam.height) {
      portrait = 1;
    }

    CRITICAL4("[VideoCameraforHal1]Camera mount %d, height %d, width=%d", rotation,
              cameraConfigHeight, cameraConfigWidth);
    CRITICAL4("[VideoCameraforHal1]camera mount %d rotation_needed %d camerafacing %d",
              rotation, portrait, callbackParam->cropRotationParam.cameraFacing);
    param->set("avtimer", "enable");
#ifndef GPU_ROTATION
    RecorderMsg info;
    memset(&info, 0, sizeof(RecorderMsg));
    info.type = ROTATION_CROP_PARAM_NOGPU;
    info.rotation = &rotation;
    info.cameraFacing = &cameraFacing;
    info.cameraConfigHeight = &cameraConfigHeight;
    info.cameraConfigWidth = &cameraConfigWidth;
    info.cameraMount = &callbackParam->cropRotationParam.cameraMount;
    info.rotationOffset = callbackParam->cropRotationParam.rotationOffset;

    if (pTransmitter != NULL) {
      pTransmitter->postEvent(&info);
    }

    CRITICAL2("[VideoCameraforHal1]Camera Rotation %d", rotation);

    switch (rotation) {
      case 0:
        param->set("video-rotation", "0");
        break;

      case 90:
        param->set("video-rotation", "90");
        break;

      case 180:
        param->set("video-rotation", "180");
        break;

      case 270:
        param->set("video-rotation", "270");
        break;
    }

    Vector<Size> sizes;
    param->getSupportedPictureSizes(sizes);

    if (sizes.size() != 0) {
      Size pictureSize = sizes.itemAt(0);
      int leastWidth = pictureSize.width;
      int leastHeight = pictureSize.height;
      CRITICAL4("[VideoCameraforHal1]camera picture height %d width %d size %d",
                leastHeight,
                leastWidth, sizes.size());

      for (int i = 1; i < sizes.size(); i++) {
        pictureSize = sizes.itemAt(i);

        if (leastWidth > pictureSize.width) {
          leastWidth = pictureSize.width;
          leastHeight = pictureSize.height;
        }
      }

      CRITICAL3("[VideoCameraforHal1]camera picture height %d width %d", leastHeight,
                leastWidth);
      param->setPictureSize(leastWidth, leastHeight);
    }

    if (!portrait && rotation % 180 != 0) {
      param->setPreviewSize(cameraConfigHeight, cameraConfigWidth);
      param->setVideoSize(cameraConfigHeight, cameraConfigWidth);
      //param->setPictureSize(cameraConfigHeight,cameraConfigWidth);
      CRITICAL1("[VideoCameraforHal1]vt-crop is enabled");
      CRITICAL3("[VideoCameraforHal1]width %d height %d", cameraConfigHeight,
                cameraConfigWidth);
    } else if (portrait && rotation % 180 == 0) {
      param->setPreviewSize(cameraConfigWidth, cameraConfigHeight);
      param->setVideoSize(cameraConfigWidth, cameraConfigHeight);
      //param->setPictureSize(cameraConfigWidth,cameraConfigHeight);
      CRITICAL1("[VideoCameraforHal1]vt-crop is enabled");
      CRITICAL3("[VideoCameraforHal1]width %d height %d", cameraConfigHeight,
                cameraConfigWidth);
    } else if (portrait) {
      param->setPreviewSize(cameraConfigHeight, cameraConfigWidth);
      param->setVideoSize(cameraConfigHeight, cameraConfigWidth);
      //param->setPictureSize(cameraConfigHeight,cameraConfigWidth);
      CRITICAL3("[VideoCameraforHal1]width %d height %d", cameraConfigHeight,
                cameraConfigWidth);
    } else if (!portrait) {
      param->setPreviewSize(cameraConfigWidth, cameraConfigHeight);
      param->setVideoSize(cameraConfigWidth, cameraConfigHeight);
      param->setPictureSize(cameraConfigWidth, cameraConfigHeight);
      CRITICAL3("[VideoCameraforHal1]width %d height %d", cameraConfigWidth,
                cameraConfigHeight);
    }

#else
    CRITICAL4("[VideoCameraforHal1]IMSCamera Requested Mount angle %d  width=%d height=%d",
              rotation, cameraConfigWidth, cameraConfigHeight);
    RecorderMsg info;
    memset(&info, 0, sizeof(RecorderMsg));
    info.type = ROTATION_CROP_PARAM_GPU;
    info.cameraFacing = &cameraFacing;
    info.rotation = &rotation;
    info.cameraConfigHeight = &cameraConfigHeight;
    info.cameraConfigWidth = &cameraConfigWidth;

    if (pTransmitter != NULL) {
      pTransmitter->postEvent(&info);
    }

    CRITICAL4("[VideoCameraforHal1]IMSCamera rotation angle %d  width=%d height=%d",
              rotation, cameraConfigWidth, cameraConfigHeight);
    param->setPreviewSize(cameraConfigWidth, cameraConfigHeight);
    param->setVideoSize(cameraConfigWidth, cameraConfigHeight);
    param->setPictureSize(cameraConfigWidth, cameraConfigHeight);
#endif
    TRACE1("[VideoCameraforHal1]engine_custom_params_callback <");
  } else if (callbackParam->type == CAMERA_STOP_RECORDING_PARAM) {
    RecorderMsg info;
    memset(&info, 0, sizeof(RecorderMsg));
    info.type = STOP_RECORDING;

    if (pTransmitter != NULL) {
      pTransmitter->postEvent(&info);
    }
  } else if (callbackParam->type == CAMERA_WAIT_FOR_BUFFERS) {
    RecorderMsg info;
    memset(&info, 0, sizeof(RecorderMsg));
    info.type = WAIT_FOR_BUFFERS;

    if (pTransmitter != NULL) {
      pTransmitter->postEvent(&info);
    }
  }
}
void VideoCameraforHal1::engine_camera_notify_callback(int32_t msgType,
    int32_t /*ext1*/,
    int32_t /*ext2*/,
    void* /*user*/)
{
  CRITICAL2("[THINCLIENT]ims_camera_notify_callback msgType = %d", msgType);
}

void VideoCameraforHal1::engine_camera_handle_callback_timestamp(
  nsecs_t timestamp,
  native_handle_t *handle)
{
  uint64 padding = 10000;
  uint64 MAX = 0xFFFFFFFFFFFFFFFF / 2000;
  uint32 result;
  VideoTransmitter *pTransmitter = NULL;
  pTransmitter = mHal1.getClient();

  if (pTransmitter == NULL) {
    ERROR1("[THINCLIENT]engine_camera_handle_callback_timestamp: Dropping camera frames pTransmitter not inited");
    return;
  }

  if (eVideoStateActive != pTransmitter->getRecorderState()) {
    ERROR1("[THINCLIENT]engine_camera_handle_callback_timestamp: Dropping camera frames Recorder not started");
    return;
  }

//Need to handle in a different way
#if 1

  /* Added to handle the image send mode */
  if (getImageSendStatus() == IMAGE_SEND_ACTIVE ) {
    ERROR2("[THINCLIENT]engine_camera_handle_callback_timestamp: Dropping camera frames image transfer mode - %d ",
           getImageSendStatus());
    return;
  }

#endif
//Move to Encoded data function in Thinclient
#if 0
#ifdef RCC

  if (rate_adapt_flag == RATE_ADAPTATION_AVPF) {
    if (RccSenderUpdateRate(p_avpf_sender_hdl, 0, 0)) {
      RecorderReconfigurationCmd();
    }
  }

#endif
#endif
  int  error = recorder_mutex_timedlock(20);

  if (error != 0) {
    ERROR2("[THINCLIENT]engine_camera_handle_callback_timestamp:recorder_mutex_timedlock timed out %d",
           error);
    return;
  }

  if ((getModemTimeProperty() == 1 && getModemTime() == 0) /*|| cameraConfigHeight != configuredHeight || cameraConfigWidth != configuredWidth*/
      || (eVideoStateActive != pTransmitter->getRecorderState())
      || !(pTransmitter->isCameraResolutionMatching())) {
    ERROR2("[THINCLIENT]Dropping camera frames umt - %d", getModemTimeProperty());
    recorder_mutex_unlock();
    return;
  }

  if (handle == NULL) {
    ERROR1("[THINCLIENT]engine_camera_handle_callback_timestamp : handle is NULL hence returning\n");
    recorder_mutex_unlock();
    return;
  }

  ERROR5("[THINCLIENT]engine_camera_handle_callback_timestamp : time_in_micro %llu time_in_nano %llu handle - %p size - %d",
         (unsigned long long)timestamp / 1000,
         (unsigned long long)timestamp, handle, handle->data[2]);
#if 0

  if (videoInjection[0] == '1') {
    uint32 size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, configuredWidth,
                                    configuredHeight);
    CRITICAL2("[THINCLIENT]Start of video injection %d", size);

    if (buffer == NULL) {
      buffer = (char*)qpDplMalloc(size);

      if (buffer == NULL) {
        ERROR1("[THINCLIENT]Error while creating buffer");
        fclose(fpVideoInjection);
        return;
      }
    }

    if (buffer != NULL) {
      char *orig = (char *)buffer;
      memset(buffer, 0, sizeof(buffer));
      result = 0;
      CRITICAL2_STR("[THINCLIENT]timestamp = %llu", timestamp);

      if (buffer != NULL) {
        char *yuv = (char *)buffer;
        int i, lscanl, lstride, cscanl, cstride;
        lstride = VENUS_Y_STRIDE(COLOR_FMT_NV12, configuredWidth);
        lscanl = VENUS_Y_SCANLINES(COLOR_FMT_NV12, configuredHeight);
        cstride = VENUS_UV_STRIDE(COLOR_FMT_NV12, configuredWidth);
        cscanl = VENUS_UV_SCANLINES(COLOR_FMT_NV12, configuredHeight);
        CRITICAL5("[THINCLIENT]lscanl=%d,cstride=%d,cscanl=%d,lstride=%d", lstride,
                  lscanl, cstride, cscanl);

        for (i = 0; i < configuredHeight; i++) {
          if (fpVideoInjection != NULL) {
            result += (int)fread(yuv, 1, configuredWidth, fpVideoInjection);
            yuv += lstride;
          } else {
            ERROR1("[THINCLIENT]Could not read videoinjection file");
          }
        }

        yuv = (char *)buffer + (lscanl * lstride);

        for (i = 0; i < ((configuredHeight + 1) >> 1); i++) {
          if (fpVideoInjection != NULL) {
            result += (int)fread(yuv, 1, configuredWidth, fpVideoInjection);
            yuv += cstride;
          } else {
            ERROR1("[THINCLIENT]Could not read videoinjection file");
          }
        }

        //this is if file is finished we should startover
        if (result != ((configuredWidth * configuredHeight * 3) / 2)) {
          if (fpVideoInjection != NULL) {
            fseek(fpVideoInjection, 0, 0);
          } else {
            ERROR1("[THINCLIENT]Could not seek videoinjection file");
          }

          buffer = orig;
          char *yuv = (char *)buffer;
          CRITICAL5("[THINCLIENT]reading again lscanl=%d,cstride=%d,cscanl=%d,lstride=%d",
                    lstride, lscanl, cstride, cscanl);

          for (i = 0; i < configuredHeight; i++) {
            if (fpVideoInjection != NULL) {
              result += (int)fread(yuv, 1, configuredWidth, fpVideoInjection);
              yuv += lstride;
            } else {
              ERROR1("[THINCLIENT]Could not read videoinjection file");
            }
          }

          yuv = (char *)buffer + (lscanl * lstride);

          for (i = 0; i < ((configuredHeight + 1) >> 1); i++) {
            if (fpVideoInjection != NULL) {
              result += (int)fread(yuv, 1, configuredWidth, fpVideoInjection);
              yuv += cstride;
            } else {
              ERROR1("[THINCLIENT]Could not read videoinjection file");
            }
          }
        }

        QpDplImsThinClient *ptr = QpDplImsThinClient::getInstance();
#ifdef HAL1
        ptr->pushFramestoEncoderNativeHandle(handle, timestamp / 1000, handle->data[2]);
#endif
        buffer = orig;
      }

      CRITICAL2("[THINCLIENT]End of video injection =%d", result);
    }

    /* Unlocking the mutex for recorder since its returning */
    recorder_mutex_unlock();
    return;
  }

#endif

  if (pTransmitter->getFrameRate() != 0) {
    if ((0 == previouststamp) ||
        skipFrame(timestamp, previouststamp, pTransmitter->getFrameRate())) {
      previouststamp = timestamp;
      pTransmitter->ProcessRecordingFrame((uint64)timestamp / 1000, handle,
                                          handle->data[2]);
      MINIMAL3_STR("[THINCLIENT]pushed frames timstamp difference timestamp = %llu , prevtstamp = %llu",
                   (unsigned long long)timestamp / 1000,
                   (unsigned long long)previouststamp / 1000);
      recorder_mutex_unlock();
      return;
    } else {
      MINIMAL3_STR("[THINCLIENT]skipping frames timstamp difference timestamp = %llu , prevtstamp = %llu",
                   (unsigned long long)timestamp / 1000,
                   (unsigned long long)previouststamp / 1000);
    }
  } else {
    MINIMAL3_STR("[THINCLIENT]no skipping frames timstamp difference timestamp = %llu , prevtstamp = %llu",
                 (unsigned long long)timestamp / 1000,
                 (unsigned long long)previouststamp / 1000);
    pTransmitter->ProcessRecordingFrame((uint64)timestamp / 1000, handle,
                                        handle->data[2]);
  }

  recorder_mutex_unlock();
}

void RecordingCameraBuffer::createqueuebuffer(uint32_t height, uint32_t width)
{
  status_t mInitCheck;
  BufferQueue::createBufferQueue(&RecordingBufferProducer,
                                 &RecordingBufferConsumer);
  CRITICAL2_STR("producerrecordingsurface %p", RecordingBufferProducer.get());
  CRITICAL2_STR("consumerrecordingsurface %p", RecordingBufferConsumer.get());
  //RecordingBufferConsumer->setConsumerName("VTLib");
  RecordingBufferConsumer->setDefaultBufferSize(width, height);
  String8 name("VTLib");
  RecordingBufferConsumer->setConsumerName(name);
  mInitCheck = RecordingBufferConsumer->setMaxAcquiredBufferCount(
                 CAMER_BUFFER_COUNT);

  if (mInitCheck != NO_ERROR) {
    CRITICAL3_STR("consumerrecordingsurface Error connecting to BufferQueue: %s (%d)",
                  strerror(-mInitCheck), mInitCheck);
    return;
  } else {
    CRITICAL1("consumerrecordingsurface  setbuffer count to BufferQueue");
  }

  RecordingBufferConsumer->setConsumerUsageBits(GRALLOC_USAGE_HW_VIDEO_ENCODER);
  wp<BufferQueue::ConsumerListener> listener =
    static_cast<BufferQueue::ConsumerListener*>(this);
  sp<BufferQueue::ProxyConsumerListener> proxy = new
  BufferQueue::ProxyConsumerListener(listener);
  CRITICAL2_STR("vtlib proxy %p", proxy.get());
  mInitCheck = RecordingBufferConsumer->consumerConnect(proxy, false);

  if (mInitCheck != NO_ERROR) {
    CRITICAL3_STR("consumerrecordingsurface Error connecting to BufferQueue: %s (%d)",
                  strerror(-mInitCheck), mInitCheck);
    return;
  } else {
    CRITICAL2("consumerrecordingsurface  connecting to BufferQueue(%d)",
              mInitCheck);
  }
}

RecordingCameraBuffer::~RecordingCameraBuffer()
{
  CRITICAL1("RecordingCameraBuffer Destroyed");
}
//int CameraBufferEnqueue(CameraBufferSlot *Buffer);
int CameraBufferEnqueue(CameraBufferSlot *Buffer)
{
//if the Queue is full
  if ((Cameratail - CameraBufferSize) == Camerahead) {
    ERROR1("bufferqueue is full");
    return QP_FALSE;
  } else {
    Cameratail++;
    //add data to the camerabuffers
    camerabuffers[Cameratail % CameraBufferSize].buffer = Buffer->buffer;
    camerabuffers[Cameratail % CameraBufferSize].fence  = Buffer->fence;
    camerabuffers[Cameratail % CameraBufferSize].skip = Buffer->skip;
    camerabuffers[Cameratail % CameraBufferSize].slotid = Buffer->slotid;
    camerabuffers[Cameratail % CameraBufferSize].used = Buffer->used;
    DEBUG2_STR("enqueue buffer %#llx to input",
               (long long)Buffer->buffer->getId());
    return QP_TRUE;
  }

  return QP_FALSE;
}
void ReleaseBuffers(const sp<GraphicBuffer> &buffer, const sp<Fence> &fence)
{
  int consumerSlot;
  status_t status = RecordingBufferConsumer->attachBuffer(&consumerSlot, buffer);

  if (status != NO_ERROR) {
    ERROR2("attaching buffer to input failed (%d)", status);
  }

  if (status == NO_ERROR) {
    status = RecordingBufferConsumer->releaseBuffer(consumerSlot,
             0 /* frameNumber */,
             EGL_NO_DISPLAY, EGL_NO_SYNC_KHR, fence);

    if (status != NO_ERROR) {
      CRITICAL2("releasing buffer to input failed (%d)", status);
    }
  }

  CRITICAL2_STR("releasedbuffer %#llx to input",
                (long long)buffer->getId());
}

bool CameraBufferDequeueAll(CameraBufferSlot *Buffer)
{
  if (Cameratail == Camerahead) {
    CRITICAL1("CameraQueue is empty");
    Camerahead = Cameratail = -1; //clear it to start
    return QP_FALSE;  //null if queue is empty
  } else {
    Camerahead++;

    if (camerabuffers[Camerahead % CameraBufferSize].used == QP_TRUE) {
      DEBUG2_STR("dequuebuffer %#llx to input",
                 (long long)camerabuffers[Camerahead % CameraBufferSize].buffer->getId());
      Buffer->buffer = camerabuffers[Camerahead % CameraBufferSize].buffer;
      Buffer->fence = camerabuffers[Camerahead % CameraBufferSize].fence;
      Buffer->skip = camerabuffers[Camerahead % CameraBufferSize].skip;
      Buffer->slotid = camerabuffers[Camerahead % CameraBufferSize].slotid;
      Buffer->used = camerabuffers[Camerahead % CameraBufferSize].used;
      DEBUG2_STR("dequeue Cbuffer %#llx to input",
                 (long long)Buffer->buffer->getId());
      return QP_TRUE;
    }
  }

  return QP_FALSE;
}

void ReleasePendingBuffers()
{
  uint64_t id;
  CameraBufferSlot camerabuffer;
  pthread_mutex_lock(&CameraBuffer_mutex);
  CRITICAL1("releasing pending camera buffers");

  // Attach and release the buffer back to the input.
  //for (index = 0; index < CameraBufferSize; index++) {
  while (CameraBufferDequeueAll(&camerabuffer)) {
    if (camerabuffer.used == QP_TRUE) {
      ReleaseBuffers(camerabuffer.buffer, camerabuffer.fence);
      camerabuffer.used = QP_FALSE;
      camerabuffer.skip = QP_FALSE;
    }
  }

  pthread_mutex_unlock(&CameraBuffer_mutex);
}

void RecordingCameraBuffer::onFrameAvailable(const BufferItem& item)
{
  int msize = 0;
  void *buf;
  int color_format;
  uint64 padding = 10000;
  uint64 MAX = 0xFFFFFFFFFFFFFFFF / 2000;
  status_t status;
  DEBUG2_STR("onFrameAvailable, item %p", &item);
  BufferItem Consumeritem;
  pthread_mutex_lock(&RecordingConsumer_mutex);

  if (NULL == RecordingBufferConsumer.get() ) {
    ERROR1("onFrameAvailable RecordingBufferConsumer is NULL");
    pthread_mutex_unlock(&RecordingConsumer_mutex);
    return;
  }

  status_t err = RecordingBufferConsumer->acquireBuffer(&Consumeritem, 0);

  if (err != NO_ERROR) {
    ERROR2("onFrameAvailable acquireBuffer fails %d", err);
    pthread_mutex_unlock(&RecordingConsumer_mutex);
    return;
  } else {
    DEBUG2("onFrameAvailable acquireBuffer success %p",
           Consumeritem.mGraphicBuffer.get());
  }

  CameraBufferSlot camerabuffer;

  if ( Consumeritem.mGraphicBuffer != NULL) {
    err = RecordingBufferConsumer->detachBuffer(Consumeritem.mSlot);

    if (err != NO_ERROR) {
      CRITICAL2("BUFFERDEBUG::onFrameAvailable detaching buffer from input failed (%d)",
                err);
    }
  } else {
    MINIMAL2("BUFFERDEBUG::releaseBuffer gb null slot %d cameracount %d",
             Consumeritem.mSlot);
    pthread_mutex_unlock(&RecordingConsumer_mutex);
    return;
  }

  if ((mTx->getRecorderState() == eVideoStateStopping)
      || (mTx->getBufferMode() == VIDEO_BUFFER_NON_META_MODE)) {
    CRITICAL1("[THINCLIENT]onFrameAvailable: Dropping camera frames");
    ReleasePendingBuffers();
    ReleaseBuffers(Consumeritem.mGraphicBuffer, Consumeritem.mFence);
    pthread_mutex_unlock(&RecordingConsumer_mutex);
    return;
  }

  //struct timespec mutex_time;
  //memset(&mutex_time, 0, sizeof(mutex_time));
  //clock_gettime(CLOCK_REALTIME, &mutex_time);
  //mutex_time.tv_nsec += 40000000; //Add 40ms timeout
  int  error = recorder_mutex_timedlock(20);

  if ((eVideoStateActive != mTx->getRecorderState()) || (error != 0) ) {
    CRITICAL3("onFrameAvailable release buffer immediately, recordstate %d not active slot=%d",
              mTx->getRecorderState(), Consumeritem.mSlot);

    //Unlock the Recorder mutex only when you have acquired it
    if (error == 0) {
      recorder_mutex_unlock();
    } else {
      ERROR2("onFrameAvailable mutex timed out err %d", error);
    }
    ReleasePendingBuffers();
    ReleaseBuffers(Consumeritem.mGraphicBuffer, Consumeritem.mFence);

    pthread_mutex_unlock(&RecordingConsumer_mutex);
    return;
  }

  if (Consumeritem.mGraphicBuffer != NULL) {
    ANativeWindowBuffer* NatWindBuf =
      Consumeritem.mGraphicBuffer->getNativeBuffer();
    private_handle_t* pHandle = (private_handle_t*)NatWindBuf->handle;
    DEBUG2_STR("buffer handle ptr is  %p", pHandle);
    uint64_t VtTimeStamp = 0;
#ifndef HAL1
    int eStatus = getMetaData(pHandle, GET_VT_TIMESTAMP, &VtTimeStamp);
    MINIMAL3("onFrameAvailable acquireBuffer CameraTS=%llu slot=%d",
             (unsigned long long)VtTimeStamp,
             (long long)Consumeritem.mGraphicBuffer->getId());
#endif

    if ((mTx->getRecorderState() == eVideoStateActive)
        && (Consumeritem.mGraphicBuffer->getWidth() == width)
        && (Consumeritem.mGraphicBuffer->getHeight() == height)
       ) {
      if (mTx->getFrameRate() != 0) {
        if ((0 == previouststamp) ||
            skipFrame(VtTimeStamp, previouststamp, mTx->getFrameRate())) {
          pthread_mutex_lock(&CameraBuffer_mutex);
          CameraBufferSlot camerabuffer;
          previouststamp = VtTimeStamp;
          camerabuffer.buffer = Consumeritem.mGraphicBuffer;
          camerabuffer.fence = Consumeritem.mFence;
          camerabuffer.used = QP_TRUE;
          camerabuffer.skip = QP_FALSE;
          camerabuffer.slotid = Consumeritem.mGraphicBuffer->getId();
          /*tried to add complexity to check if Queue is full and then we should
            not push the buffers to OMX and then we cannot even release it because
            there might be pending buffers - very less likely to hit but if hit
            then it is something wrong at OMX to either delay or no response*/
          CameraBufferEnqueue(&camerabuffer);
          MINIMAL2_STR("onFrameAvailable pushing Camera frame =%#llx",
                       (long long)Consumeritem.mGraphicBuffer->getId());
          pthread_mutex_unlock(
            &CameraBuffer_mutex); //make sure we don't block pushframestoEncoder
          //for this mutex
          mTx->ProcessRecordingFrame(VtTimeStamp, &Consumeritem, msize);
        } else  {
          pthread_mutex_lock(&CameraBuffer_mutex);
          CameraBufferSlot camerabuffer;
          MINIMAL2_STR("onFrameAvailable skipping Camera frame %#llx",
                       (long long)Consumeritem.mGraphicBuffer->getId());

          //if queue is empty then just release the buffer
          //very less likely we will hit this condition
          if (Camerahead == Cameratail) {
            ReleaseBuffers(Consumeritem.mGraphicBuffer, Consumeritem.mFence);
          } else {
            camerabuffer.buffer = Consumeritem.mGraphicBuffer;
            camerabuffer.fence = Consumeritem.mFence;
            camerabuffer.used = QP_TRUE;
            camerabuffer.skip = QP_TRUE;
            camerabuffer.slotid = Consumeritem.mGraphicBuffer->getId();
            CameraBufferEnqueue(&camerabuffer);
          }

          pthread_mutex_unlock(&CameraBuffer_mutex);
        }
      } else {
        pthread_mutex_lock(&CameraBuffer_mutex);
        CameraBufferSlot camerabuffer;
        camerabuffer.buffer = Consumeritem.mGraphicBuffer;
        camerabuffer.fence = Consumeritem.mFence;
        camerabuffer.used = QP_TRUE;
        camerabuffer.skip = QP_FALSE;
        camerabuffer.slotid = Consumeritem.mGraphicBuffer->getId();
        /*tried to add complexity to check if Queue is full and then we should
          not push the buffers to OMX and then we cannot even release it because
          there might be pending buffers - very less likely to hit but if hit
          then it is something wrong at OMX to either delay or no response*/
        CameraBufferEnqueue(&camerabuffer);
        pthread_mutex_unlock(
          &CameraBuffer_mutex);//make sure we don't block pushframestoEncoder
        //for this mutex
        mTx->ProcessRecordingFrame(VtTimeStamp, &Consumeritem, msize);
      }

      /* Unlocking the mutex for recorder since its returning */
      recorder_mutex_unlock();
      pthread_mutex_unlock(&RecordingConsumer_mutex);
      return;
    } else {
      MINIMAL3("onFrameAvailable releaseBuffer configuredheight=%d graphicbufferheight=%d",
               height, Consumeritem.mGraphicBuffer->getHeight());
      status =      RecordingBufferConsumer->releaseBuffer(Consumeritem.mSlot,
                    Consumeritem.mFrameNumber,
                    EGL_NO_DISPLAY, EGL_NO_SYNC_KHR, Consumeritem.mFence);

      if (status != NO_ERROR) {
        CRITICAL2("releaseBuffer to input failed (%d)", status);
      }

      // TODO: do we need to try to return this buffer later?
      CRITICAL2("onFrameAvailable releaseBuffer to input (%d)", status);
      /* Unlocking the mutex for recorder since its returning */
      recorder_mutex_unlock();
    }
  } else {
    MINIMAL2("BUFFERDEBUG::releaseBuffer gb null slot %d",
             Consumeritem.mSlot);
  }

  /* Unlocking the mutex for recorder since its returning */
  recorder_mutex_unlock();
  pthread_mutex_unlock(&RecordingConsumer_mutex);
  return;
}

void RecordingCameraBuffer::onBuffersReleased()
{
}

void RecordingCameraBuffer::onSidebandStreamChanged()
{
}

namespace android
{





void CameraBufferInitandClearQueue()
{
  MINIMAL1("Camera Internal Bufferqueue is cleaning up");

  for (int i = 0; i < CameraBufferSize; i++) {
    camerabuffers[i].buffer = NULL;
    camerabuffers[i].fence = NULL;
    memset(camerabuffers + i, 0, sizeof(CameraBufferSlot));
  }

  Camerahead = Cameratail = -1; //clear it to start
}



bool CameraBufferDequeue(CameraBufferSlot *Buffer, bool skip)
{
  if (Cameratail == Camerahead) {
    return QP_FALSE;  //null if queue is empty
  } else {
    Camerahead++;
    DEBUG2_STR("dequuebuffer %#llx to input",
               (long long)camerabuffers[Camerahead % CameraBufferSize].buffer->getId());

    if (skip == camerabuffers[Camerahead % CameraBufferSize].skip) {
      Buffer->buffer = camerabuffers[Camerahead % CameraBufferSize].buffer;
      Buffer->fence = camerabuffers[Camerahead % CameraBufferSize].fence;
      Buffer->skip = camerabuffers[Camerahead % CameraBufferSize].skip;
      Buffer->slotid = camerabuffers[Camerahead % CameraBufferSize].slotid;
      Buffer->used = camerabuffers[Camerahead % CameraBufferSize].used;
      DEBUG2_STR("dequeue buffer %#llx to input", (long long)Buffer->buffer->getId());
      return QP_TRUE;
    } else {
      Camerahead--;
      DEBUG2_STR("previous released buffer %#llx to input",
                 (long long)camerabuffers[Camerahead % CameraBufferSize].buffer->getId());
      return QP_FALSE;
    }
  }
}

void ReleaseToBufferToCamera(const sp<GraphicBuffer> &buffer,
                             const sp<Fence>& /*fence*/)
{
  uint64_t id;
  CameraBufferSlot camerabuffer;
  pthread_mutex_lock(&CameraBuffer_mutex);

  // Attach and release the buffer back to the input.
  //for (index = 0; index < CameraBufferSize; index++) {
  if (CameraBufferDequeue(&camerabuffer, QP_FALSE)) {
    if (camerabuffer.used == QP_TRUE
        && camerabuffer.slotid == (uint64_t)buffer->getId()) {
      ReleaseBuffers(camerabuffer.buffer, camerabuffer.fence);
      camerabuffer.used = QP_FALSE;
      camerabuffer.skip = QP_FALSE;

      //this logic is added to make sure any skipped frame is returned in order of recieving.
      // the trigger would be once x frame is released from encoder then x+1 is skipped from ims for frame
      //rate adaptatino then we should release those as well before next buffer which will be released from encoder with x+2.
      while ( CameraBufferDequeue(&camerabuffer, QP_TRUE)) {
        ReleaseBuffers(camerabuffer.buffer, camerabuffer.fence);
        camerabuffer.used = QP_FALSE;
        camerabuffer.skip = QP_FALSE;
      }
    }
  }

  pthread_mutex_unlock(&CameraBuffer_mutex);
}

void ReleasePendingBuffers()
{
  uint64_t id;
  CameraBufferSlot camerabuffer;
  pthread_mutex_lock(&CameraBuffer_mutex);
  CRITICAL1("releasing pending camera buffers");

  // Attach and release the buffer back to the input.
  //for (index = 0; index < CameraBufferSize; index++) {
  while (CameraBufferDequeueAll(&camerabuffer)) {
    if (camerabuffer.used == QP_TRUE) {
      ReleaseBuffers(camerabuffer.buffer, camerabuffer.fence);
      camerabuffer.used = QP_FALSE;
      camerabuffer.skip = QP_FALSE;
    }
  }

  pthread_mutex_unlock(&CameraBuffer_mutex);
}



Mutex transmitterstateMutex; //syncs change of stae events
#define __DEBUG_PAYLOAD__

/* Global  Mutex data */
pthread_mutex_t event_resp_tx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t event_resp_tx_cond = PTHREAD_COND_INITIALIZER;
pthread_t event_handler_tx;

sem_t tx_state_semaphore;

#define SEM_WAIT() CRITICAL3_STR("Sem wait %s Function, line number: %d\n",\
 __FUNCTION__,__LINE__); sem_wait(&tx_state_semaphore);

#define SEM_POST() CRITICAL3_STR("Sem post %s Function, line number: %d\n",\
 __FUNCTION__,__LINE__); sem_post(&tx_state_semaphore);

#define CHECK(hr) if(hr != VIDEO_ERROR_OK){ ERROR3("Check failed in function %s at line %d",__func__,__LINE__); return VIDEO_ERROR_UNKNOWN;}



static sp<IMemoryHeap> m_pMemInfo; //Smart pointer for Memory
static sp<MemoryDealer> m_mDealer[2];
static sp<IAllocator> mAllocator[2];
static sp<IOMXNode> m_NodeId;
static sp<IBinder> mNodeBinder;
static sp<IGraphicBufferProducer> InputbufferProducer;
static sp<IGraphicBufferSource> mGraphicBufferSource;
static VideoDecBufferInfo* m_pEncInpBufHeader_iomx[MAX_BUFFERS];
static VideoDecBufferInfo* m_pEncOutpBufHeader_iomx[MAX_BUFFERS];
static sp<IMemory> Inputmem[MAX_BUFFERS];
static sp<IMemory> Outputmem[MAX_BUFFERS];
static sp<TMemory> InputHidlmem[MAX_BUFFERS];
static sp<TMemory> OutputHidlmem[MAX_BUFFERS];
//#define GRALLOC_USAGE_PRIVATE_ALLOC_UBWC 0x20000000
int tempfillbuffercallbackcountenc = 0;
uint32 OutputPortBufferSize;
int tempemptybuffercallbackcountenc = 0;
bool deinit_tx = QP_FALSE;
//QP_VIDEO_ENCODER_INFO encoderOutputInfo;
OMX_TICKS nTimeStamp = 1048883179904;
int frameRate = 30;

static QPE_VIDEO_ERROR sem_timedwait()
{
  struct timespec ts;
  int result;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 3;
  result = sem_timedwait(&tx_state_semaphore, &ts);

  if (result) {
    ERROR4("Failure in [%s:%d] result = %d", __func__, __LINE__, result);
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Constructor for videoTransmitter
 *****************************************************************************************/
VideoTransmitter::VideoTransmitter()
{
  EVENT1("Constructor of VideoTransmitter");
  m_iMinInputBufCount = 0;
  m_iMinOutputBufCount = 0;
  m_iBaseTimeStamp = 0;
  m_iTimeStamp = 0;
  m_IOMXState = OMX_StateInvalid;
  m_codecConfigured = QP_FALSE;
  m_PrevTimeStamp = 0;
  m_IDRframeinterval = 2;
  iRecorderState = eVideoStateNotReady;
  memset(&m_codecConfig, 0, sizeof(QP_VIDEO_CONFIG) );
  pthread_mutex_init(&EncoderBuffer_mutex, NULL);
  pthread_mutex_init(&CameraBuffer_mutex, NULL);
  pthread_mutex_init(&RecordingConsumer_mutex, NULL);
  memset(&m_codecConfig, 0, sizeof(QP_VIDEO_CONFIG) );
  CameraBufferInitandClearQueue();
}

/****************************************************************************************
 Destructor for videoTransmitter
 *****************************************************************************************/
VideoTransmitter::~VideoTransmitter()
{
  CRITICAL1("VideoTransmitter::~VideoTransmitter:: In dtor of video transmitter");
  //EncoderFreeHandle();
  pthread_mutex_destroy(&EncoderBuffer_mutex);
  pthread_mutex_destroy(&CameraBuffer_mutex);
  pthread_mutex_destroy(&RecordingConsumer_mutex);
}

template<class Template> static void InitEncoderOMXParams(Template *params)
{
  params->nVersion.s.nVersionMajor = 1;
  params->nVersion.s.nVersionMinor = 0;
  params->nVersion.s.nRevision = 0;
  params->nSize = sizeof(Template);
  params->nVersion.s.nStep = 0;
}


/****************************************************************************************
 Init for videoTransmitter
 *****************************************************************************************/

QPE_VIDEO_ERROR VideoTransmitter::CreateInputRecordingSurface(
  sp<IGraphicBufferProducer> *bufferproducer, uint32_t height, uint32_t width)
{
  if (NULL == mRecordingBuffer.get() ) {
    mRecordingBuffer = new RecordingCameraBuffer(height, width);
    mRecordingBuffer->setClient(this);
    //smittal we have to make changes to always configure it in Landscape for 8998
    // it will be controlled by VTlibrary itself as that is entity knows about what resolution we need to
    // configure bufferqueue
    mRecordingBuffer->createqueuebuffer(height, width);
    *bufferproducer = RecordingBufferProducer.get();
  } else{
	  mRecordingBuffer->setHeight(height);
      mRecordingBuffer->setWidth(width);
      RecordingBufferConsumer->setDefaultBufferSize(width,height);
  }

  return VIDEO_ERROR_OK;
}
QPE_VIDEO_ERROR VideoTransmitter::Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                       void* pUserData, QPE_VIDEO_DEV eVDev,
                                       QP_VIDEO_CONFIG* pCodecConfig)
{
  //please never remove these register api call code as it is first step of
  //registration of callback and variables
  RegisterRecorderCallBack(tVideoCallBack, pUserData,  eVDev, pCodecConfig);
  RegisterCallBacks(tVideoCallBack, pUserData, eVDev);
  previouststamp = 0;
  TRACE1("VideoTransmitter::Init");
  ;
  int32_t size = (m_codecConfig.iWidth
                  * m_codecConfig.iHeight * 3) / 2;
  MINIMAL2("VideoTransmitter::Init size = %d", size);

  for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
    m_pEncOutpBufHeader_iomx[i] = NULL;
    m_pEncInpBufHeader_iomx[i] = NULL;
    Inputmem[i] = NULL;
    Outputmem[i] = NULL;
  }

  sem_init(&tx_state_semaphore, 0, 0);
  MINIMAL1("VideoTransmitter::Init done");
  tempfillbuffercallbackcountenc = 0;
  tempemptybuffercallbackcountenc = 0;

  //QSmart_init (QSmartRA **sr, const char *path_config_file, const char *path_log_file)
  if (pCodecConfig->eHALVersion == CAMERA_HAL1) {
    mHal1.setClient(this);
    mHal1.registerCameraCallbacks(TRUE);
  }

  return VIDEO_ERROR_OK;
}

void VideoTransmitter::HandleEvent(void* data)
{
  EVENT2("VideoTransmitter::HandleEvent : %p", data);

  if (deinit_tx) {
    MINIMAL1("VideoTransmitter::HandleEvent: Loaded state free node");

    if (m_bOmxNodeAllocated && m_NodeId->freeNode() != OK) {
      ERROR1("VideoTransmitter::HandleEvent :Error in de initializing the components");
    } else {
      CRITICAL1("VideoTransmitter::HandleEvent :Unable to free the node when the node was not allocated");
    }
  }
}

void VideoTransmitter::InputBufferListener::onBufferReleased()
{
  DEBUG1("onBufferReleased");
  sp<GraphicBuffer> buffer;
  sp<Fence> fence;
  status_t status = InputbufferProducer->detachNextBuffer(&buffer, &fence);

  if (status != NO_ERROR) {
    ERROR2("detaching buffer from output failed (%d)", status);
  }

  if (status == NO_INIT) {
    // If the output has been abandoned, we can't do anything else,
    // since buffer is invalid.
    //onAbandoned_l(false /* isInput */);
    ERROR1("onBufferReleased detaching buffer NO_INIT");
    buffer = NULL;

    for (uint8_t i = 0; i < EncoderBufferSize; i++) {
      RotationBuffer[i].UsedByIOMX = QP_FALSE;

      if (RotationBuffer[i].LocalBuffer != NULL) {
        CRITICAL2("VideoTransmitter::OnbufferReleased:: RotationBuffer Cleaning index %d",
                  i);
        //smittal one change to made
        RotationBuffer[i].LocalBuffer = NULL;
        RotationBuffer[i].CameraBuffer = NULL;
        RotationBuffer[i].CameraFence = NULL;
      }
    }

#if 0 //We are explictly doing the disconnect before CreateInputSurface

    //we are calling disconnect to make sure connect API will not fail
    // if we are calling multiple connect API on recorderconfiguration.
    if (InputbufferProducer != NULL) {
      InputbufferProducer->disconnect(NATIVE_WINDOW_API_MEDIA);
      InputbufferProducer = NULL;
    }

#endif
    return;
  } else if (NO_MEMORY == status) {
    //! If this issue is happening, we need to debug with OMX team.
    //! Ideally this error should not occur.
    ERROR1("onBufferReleased detaching buffer failed with NO_MEMORY error");
    return;
  }

  MINIMAL2_STR("onBufferReleased detachnextbuffer  %llx to input",
               (long long)buffer->getId());
#ifdef GPU_ROTATION
  DEBUG1("Waiting for onBufferReleased encoderbuffer_mutex");
  pthread_mutex_lock(&EncoderBuffer_mutex);
  DEBUG1("got the lock onBufferReleased encoderbuffer_mutex");
  int countencoder;

  for (countencoder = 0; countencoder < EncoderBufferSize; countencoder++ ) {
    DEBUG2(" onBufferReleased  %d buffer", countencoder);

    if (RotationBuffer[countencoder].UsedByIOMX == QP_TRUE) {
      DEBUG2(" onBufferReleased setUsedByIndex is true counter=%d buffer",
             countencoder);

      if (RotationBuffer[countencoder].LocalBufferId == buffer->getId()) {
        RotationBuffer[countencoder].UsedByIOMX = QP_FALSE;
        DEBUG2(" onBufferReleased setUsedByIndex changed to false  %#llx to input",
               (long long)buffer->getId());
        MINIMAL2_STR("onBufferReleased localbuffer entry found  %#llx",
                     (long long)buffer->getId());
        break;
      }
    }
  }

  if (countencoder < EncoderBufferSize) {
    ReleaseToBufferToCamera(RotationBuffer[countencoder].CameraBuffer, fence);
  }

  pthread_mutex_unlock(&EncoderBuffer_mutex);
#else
  MINIMAL2_STR("onBufferReleased cameraBuffer %#llx to input",
               (long long)buffer->getId());
  ReleaseToBufferToCamera(buffer, fence);
#endif
}

void VideoTransmitter::InputBufferListener::binderDied(const wp<IBinder>
    &/* who */)
{
}

VideoTransmitter::InputBufferListener::~InputBufferListener() {}

QPE_VIDEO_ERROR VideoTransmitter::GenerateH264IDRFrame()
{
  QPE_VIDEO_ERROR result = VIDEO_ERROR_OK;
  status_t omxresult;

  do {
    struct timeval currtime;
    gettimeofday(&currtime, NULL);
    uint64_t ullCurTime = TIMEVAL2MS(currtime);

    if (iRecorderState != eVideoStateActive ) {
      CRITICAL2("VideoTransmitter::GenerateH264IDRFrame: Video state is %d which is not active",
                iRecorderState);
      break;
    }

    MINIMAL3_STR("VideoTransmitter::GenerateH264IDRFrame IDR=%llu,Curr=%llu",
                 (unsigned long long)m_ullLastIdrTime,
                 (unsigned long long)ullCurTime);

    /* If last IDR generation time is less than 400ms, then ignore it. Too many
       close by IDR requests cannot be given to encoder. */
    if ((ullCurTime - m_ullLastIdrTime < 400) && (!m_bLTRFeatureOn)) {
      CRITICAL1("VideoTransmitter::GenerateH264IDRFrame: frequent IDR requests, ignore this req");
      result = VIDEO_ERROR_OK;
      break;
    }

    m_ullLastIdrTime = ullCurTime;
    /** Request an I-FRAME */
    OMX_CONFIG_INTRAREFRESHVOPTYPE vop_refresh;
    memset(&vop_refresh, 0, sizeof(OMX_CONFIG_INTRAREFRESHVOPTYPE));
    InitEncoderOMXParams(&vop_refresh);
    vop_refresh.nPortIndex = OMX_DirOutput;
    vop_refresh.IntraRefreshVOP = OMX_TRUE;
    omxresult = m_NodeId->setConfig(
                  OMX_IndexConfigVideoIntraVOPRefresh,
                  &vop_refresh, sizeof(vop_refresh));

    if (omxresult != OK) {
      ERROR2("VideoTransmitter::GenerateH264IDRFrame::setConfig OMX_IndexConfigVideoIntraVOPRefresh with return value %x",
             omxresult);
      result = VIDEO_ERROR_UNKNOWN;
      break;
    }
  } while (0);

  return result;
}


/****************************************************************************************
 DeInit for videoTransmitter
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::DeInit()
{
  CRITICAL2("VideoTransmitter::DeInit:: rec state is %d and un initialize components",
            iRecorderState);

  if (eVideoStateReleasing ==  iRecorderState) {
    ERROR1("VideoTransmitter::DeInit:: Recorder already released");
    return VIDEO_ERROR_OK;
  }

  iRecorderState = eVideoStateReleasing;
  mHal1.setClient(NULL);

  if (TearDown() != VIDEO_ERROR_OK) {
    ERROR1("VideoTransmitter::DeInit::Failed to TearDown encoder components");
    return VIDEO_ERROR_UNKNOWN;
  }

#ifdef GPU_ROTATION
  destroyEGL();

  //QSmart_clean (QSmartRA **sr);
  if (m_codecConfig.eHALVersion != CAMERA_HAL1) {
    for (uint8_t i = 0; i < EncoderBufferSize ; i++) {
      RotationBuffer[i].UsedByIOMX = QP_FALSE;

      if (RotationBuffer[i].LocalBuffer != NULL) {
        CRITICAL2("VideoTransmitter::DeInit:: RotationBuffer index %d", i);
        RotationBuffer[i].LocalBuffer = NULL;
        RotationBuffer[i].CameraBuffer = NULL;
        RotationBuffer[i].CameraFence = NULL;
      }
    }
  } else {
    DEBUG1("Waiting for encoderbuffer_mutex");
    pthread_mutex_lock(&EncoderBuffer_mutex);
    DEBUG1("got the lock encoderbuffer_mutex");

    for (uint8_t i = 0; i < EncoderBufferSize ; i++) {
      EncoderNativeBuffer[i].UsedByIOMX = QP_FALSE;

      if (EncoderNativeBuffer[i].outBuffer != NULL) {
        CRITICAL2("VideoTransmitter::DeInit:: RotationBuffer index %d", i);
        EncoderNativeBuffer[i].outBuffer = NULL;
      }

      if (EncoderNativeBuffer[i].nh != NULL) {
        native_handle_close(EncoderNativeBuffer[i].nh);
        native_handle_delete(EncoderNativeBuffer[i].nh);
        CRITICAL3("VideoTransmitter::EncoderNativeBuffer[%d].nh %p", i,
                  EncoderNativeBuffer[i].nh);
        EncoderNativeBuffer[i].nh = NULL;
      }
    }

    pthread_mutex_unlock(&EncoderBuffer_mutex);
  }

#endif

  if (m_codecConfig.eHALVersion == CAMERA_HAL3) {
    if (InputbufferProducer != NULL) {
      InputbufferProducer->disconnect(NATIVE_WINDOW_API_MEDIA);
      InputbufferProducer = NULL;
    }
  }

  pthread_mutex_lock(&RecordingConsumer_mutex);

  if (RecordingBufferConsumer != NULL) {
    RecordingBufferConsumer->consumerDisconnect();
    RecordingBufferConsumer = NULL;
  }

  if (RecordingBufferProducer != NULL) {
    RecordingBufferProducer = NULL;
  }

  pthread_mutex_unlock(&RecordingConsumer_mutex);
  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Start Recording for videoTransmitter
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::Start()
{
  CRITICAL3("RecorderStateTransition:Recorder:Start:%d:VIDEO_MSG_RECORDER_STARTED OMX_CommandStateSet:%d",
            iRecorderState, m_IOMXState);

  if (iRecorderState == eVideoStateNotReady) {
    MINIMAL1("VideoTransmitter::Video State is not ready hence returning");
    return VIDEO_ERROR_UNKNOWN;
  }

  if (iRecorderState == eVideoStateActive) {
    MINIMAL1("VideoTransmitter::Start already active hence returning");
    SendEventsToThinclient(VIDEO_MSG_RECORDER_STARTED);
    return VIDEO_ERROR_OK;
  } else if (m_IOMXState == OMX_StateExecuting) {
    MINIMAL1("VideoTransmitter::Start already active hence returning");
    iRecorderState = eVideoStateActive;
    CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_RECORDER_STARTED:%d",
              iRecorderState);
    GenerateH264IDRFrame();
    CRITICAL1("Trigger IDRframe ");
    SendEventsToThinclient(VIDEO_MSG_RECORDER_STARTED);
    return VIDEO_ERROR_OK;
  }

  if (m_NodeId->sendCommand(OMX_CommandStateSet,
                            OMX_StateExecuting) != OK) {
    ERROR1(
      "VideoTransmitter::Start::Failure in insrting the command for stateExecuting");
    return VIDEO_ERROR_UNKNOWN;
  }

  CHECK(sem_timedwait());
  m_IDRRetransmitOn = true;
  m_LastIDRtimestamp = 0;
  m_IDRframeinterval = 3;

  /* If IDR interval is between 1 and 6sec, then do not retransmit IDR frames*/
  if (m_codecConfig.iFrameInterval < IDR_RETRANS_INTERVAL &&
      m_codecConfig.iFrameInterval > 0) {
    m_IDRRetransmitOn = false;
  }

  /** Request an I-FRAME */
  /*OMX_CONFIG_INTRAREFRESHVOPTYPE vop_refresh;
  memset(&vop_refresh,0,sizeof(OMX_CONFIG_INTRAREFRESHVOPTYPE));
  InitEncoderOMXParams(&vop_refresh);
  vop_refresh.nPortIndex = OMX_DirOutput;
  vop_refresh.IntraRefreshVOP = OMX_TRUE;
  int res = m_OMX_handle->setConfig(m_NodeId,
        OMX_IndexConfigVideoIntraVOPRefresh,
        &vop_refresh,sizeof(vop_refresh));
                                */
  iRecorderState = eVideoStateActive;
  CRITICAL3("RecorderStateTransition:Recorder:VIDEO_MSG_RECORDER_STARTED:%d OMX_CommandStateSet:%d",
            iRecorderState, m_IOMXState);

  for (uint i = 0; i < m_iMinOutputBufCount ; i++) {
    if (m_pEncOutpBufHeader_iomx[i] && !m_pEncOutpBufHeader_iomx[i]->usedbyIOMX ) {
      m_pEncOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;
      CRITICAL3_STR("Calling fill buffer on %p i = %d",
                    m_pEncOutpBufHeader_iomx[i], i);

      if (m_NodeId->fillBuffer(m_pEncOutpBufHeader_iomx[i]->buffer_id,
                               OMXBuffer::sPreset, m_pEncOutpBufHeader_iomx[i]->fencefd) != OK) {
        ERROR1("VideoTransmitter::Start Failed to fill the buffer");
        return VIDEO_ERROR_UNKNOWN;
      }
    }
  }

  SendEventsToThinclient(VIDEO_MSG_RECORDER_STARTED);
  //initialize the timestamp for first time
  MINIMAL1("In VideoTransmitter::Start::: done");
  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Stop Recording for videoTransmitter
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::Stop()
{
  CRITICAL2("RecorderStateTransition:Recorder:Stop:%d:VIDEO_MSG_RECORDER_STOPPED",
            iRecorderState);

  if (iRecorderState == eVideoStateStopping) {
    ERROR1("RecorderStateTransition::Stop already Stopped hence returning");
    return VIDEO_ERROR_OK;
  }

  iRecorderState = eVideoStateStopping;
  CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_RECORDER_STOPPED:%d",
            iRecorderState);
  SendEventsToThinclient(VIDEO_MSG_RECORDER_STOPPED);
  return VIDEO_ERROR_OK;
}
void AddCameraTSMap(uint64_t cameraTs, uint64_t modemTS, uint64_t appsTS)
{
  if (cameraTSMapCount == 0) {
    cameraTSMapIndex = 0;
  }

  DEBUG3("cameraTSMapCount %d cameraTSMapIndex %d", cameraTSMapCount,
         cameraTSMapIndex);
  cameraTSMap[cameraTSMapIndex].camerats = cameraTs;
  cameraTSMap[cameraTSMapIndex].modemts = modemTS;
  cameraTSMap[cameraTSMapIndex].appsts = appsTS;
  cameraTSMapIndex = (cameraTSMapIndex + 1) % CAMERA_TS_MAP_SIZE;
  cameraTSMapCount++;
}

void GetModemTSForCameraTS(uint64_t cameraTS, uint64_t* modemTS,
                           uint64_t* appsTS)
{
  for (int i = 0; i < CAMERA_TS_MAP_SIZE; i++) {
    if (cameraTS != 0 && cameraTSMap[i].camerats == cameraTS) {
      *modemTS = cameraTSMap[i].modemts;
      *appsTS = cameraTSMap[i].appsts;
      DEBUG3("cameraTSMapCount %d cameraTSMapIndex %d dec", cameraTSMapCount,
             cameraTSMapIndex);
      cameraTSMapCount--;
      return;
    }
  }

  *modemTS = 0;
  *appsTS = 0;
  return;
}

/****************************************************************************************
 Pause for videoTransmitter
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::Pause()
{
  TRACE1("VideoTransmitter::Pause");

  if (iRecorderState == eVideoStateActive) {
    iRecorderState == eVideoStatePaused;
  }

  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Resume for videoTransmitter
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::Resume()
{
  TRACE1("VideoTransmitter::Resume");

  if (iRecorderState == eVideoStatePaused) {
    EVENT1("VideoTransmitter::Resume:: TX video resumed");
    iRecorderState == eVideoStateActive;
  }

  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Used for configuring of the OMX components and ports of videoTransmitter

 *****************************************************************************************/

QPE_VIDEO_ERROR VideoTransmitter::Configure(QP_VIDEO_CONFIG tCodecConfig)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  bool isReconfiguring = QP_FALSE;
  CRITICAL2("RecorderStateTransition:Recorder:Configure:%d:VIDEO_MSG_CODEC_CHANGED",
            iRecorderState);
  enum eVideoState prevState = iRecorderState;
  previouststamp = 0;
  EVENT3("VideoTransmitter::Configure:: prev codec is %d and new codec received is %d",
         m_codecConfig.eCodec, tCodecConfig.eCodec);
  /* We do not need to copy pointers, reset to NULL */
  memcpy(&m_codecConfig, &tCodecConfig, sizeof(QP_VIDEO_CONFIG) );
  m_codecConfig.pNALHeader = m_codecConfig.pVolHeader = nullptr;
  CRITICAL4("Videotransmitter:Configure: ePktnMode - %d Bit rate : %d rc - %d",
            m_codecConfig.ePktnMode, m_codecConfig.iBitRate,
            m_codecConfig.eBitRateMode);
  CRITICAL3("Videotransmitter:Configure: eH265ProfileLevel - %d eH265ProfileType - %d",
            tCodecConfig.eH265ProfileLevel, tCodecConfig.eH265ProfileType);
  CRITICAL5("configure: rotation is %d cameraheight=%d camerawidth=%d eHalVersion %d",
            tCodecConfig.rotation, m_codecConfig.cameraConfigHeight,
            m_codecConfig.cameraConfigWidth, m_codecConfig.eHALVersion);

  if (Validate_params(tCodecConfig) != VIDEO_ERROR_OK) {
    LOGE("VideoTransmitter::Configure:: Validating Params failed");
    ERROR2("RecorderStateTransition:Recorder:VIDEO_MSG_CONFIGURE_ERROR:%d",
           iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
    return VIDEO_ERROR_UNKNOWN;
  }

  if (m_codecConfig.eHALVersion == CAMERA_HAL3) {
    if (NULL != mRecordingBuffer.get() ) {
      //mRecordingBuffer = new RecordingCameraBuffer();
      //mRecordingBuffer->setClient(this);
      //smittal we have to make changes to always configure it in Landscape for 8998
      //mRecordingBuffer->createqueuebuffer(tCodecConfig.iHeight,tCodecConfig.iWidth);
#ifdef GPU_ROTATION
      mRecordingBuffer->setHeight(tCodecConfig.cameraConfigHeight);
      mRecordingBuffer->setWidth(tCodecConfig.cameraConfigWidth);
      RecordingBufferConsumer->setDefaultBufferSize(tCodecConfig.cameraConfigWidth,
          tCodecConfig.cameraConfigHeight);
#else
      mRecordingBuffer->setHeight(tCodecConfig.iHeight);
      mRecordingBuffer->setWidth(tCodecConfig.iWidth);
      RecordingBufferConsumer->setDefaultBufferSize(tCodecConfig.iWidth,
          tCodecConfig.iHeight);
#endif
    }
  }

  CRITICAL5("width %d height %d bit rate %d frame rate %d", tCodecConfig.iWidth,
            tCodecConfig.iHeight, tCodecConfig.iBitRate, tCodecConfig.iFrameRate);
#ifdef GPU_ROTATION
  EGLBoolean result;
  destroyEGL();
  setupEGL();
#endif

  if (m_codecConfig.iInternalCodecConfig == QP_FALSE) {
    OutputPortBufferSize = 0;
  }

  if ((iRecorderState == eVideoStateActive)
      || (iRecorderState == eVideoStateStopping )
      || (iRecorderState == eVideoStateIdle)
     ) {
    //Transition the codec to Loaded state and perform the codec set all over again
    iRecorderState = eVideoStateNotReady;
    MovetoLoadedState();
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
      || tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
    TRACE1("VideoTransmitter::Configure:: Inside MP4...");

    if (tCodecConfig.iVolHeaderLen > 0) {
      TRACE1(
        "VideoTransmitter::Configure:: Inside MP4: Vol header length is greater than 0");
      m_codecConfig.pVolHeader = (char*) malloc(
                                   tCodecConfig.iVolHeaderLen);

      if (m_codecConfig.pVolHeader)
        memcpy(m_codecConfig.pVolHeader,
               tCodecConfig.pVolHeader, tCodecConfig.iVolHeaderLen);

      m_codecConfig.iVolHeaderLen = tCodecConfig.iVolHeaderLen;
    } else {
      if (tCodecConfig.pVolHeader == NULL) {
        TRACE1(
          "VideoTransmitter::Configure:: Inside MP4: Vol header is NULL... Generating");
        m_codecConfig.iVolHeaderLen = 100;
        m_codecConfig.pVolHeader = (char*) malloc(
                                     m_codecConfig.iVolHeaderLen);
      }
    }
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_H264
      || tCodecConfig.eCodec == VIDEO_CODEC_H265 ) {
    if (tCodecConfig.iNALHeaderLen > 0) {
      m_codecConfig.pNALHeader = (char*) malloc(
                                   tCodecConfig.iNALHeaderLen);

      if (m_codecConfig.pNALHeader)
        memcpy(m_codecConfig.pNALHeader,
               tCodecConfig.pNALHeader, tCodecConfig.iNALHeaderLen);

      m_codecConfig.iNALHeaderLen = tCodecConfig.iNALHeaderLen;
    } else {
      if (tCodecConfig.pNALHeader == NULL) {
        TRACE1(
          "VideoTransmitter::Configure:: Inside MP4: Vol header is NULL... Generating");
        m_codecConfig.iNALHeaderLen = 100;
        m_codecConfig.pNALHeader = (char*) malloc(
                                     m_codecConfig.iNALHeaderLen);
      }
    }
  }

  if (!isReconfiguring) {
    if (LoadIOMXEncoderComponents() != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Init  Failed to initialize the encoder components");
      iRecorderState = eVideoStateNotReady;
      ERROR2("RecorderStateTransition:Recorder:VIDEO_MSG_CONFIGURE_ERROR:%d",
             iRecorderState);
      SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
      return VIDEO_ERROR_UNKNOWN;
    }
  }
  if (VIDEO_BUFFER_NON_META_MODE == m_codecConfig.iInputVideoBufferMode) {
   m_codecConfig.iFrameInterval =0;
  }
  if (EncodeConfigure(m_codecConfig)
      != VIDEO_ERROR_OK) {
    LOGE(
      "VideoTransmitter::Configure:: Failure in configuring common parameters");
    iRecorderState = eVideoStateNotReady;
    ERROR2("RecorderStateTransition:Recorder:VIDEO_MSG_CONFIGURE_ERROR:%d",
           iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
    return VIDEO_ERROR_UNKNOWN;
  }

  if (prevState == eVideoStateNotReady) {
    CRITICAL1("VideoTransmitter :: Post VIDEO_MSG_DEV_INITIALISED and CODEC_CHANGED");
    SendEventsToThinclient(VIDEO_MSG_DEV_INITIALISED);
    CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_DEV_INITIALISED:%d",
              iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
  } else {
    if (m_codecConfig.iInternalCodecConfig == QP_FALSE) {
      EVENT1("VideoTransmitter :: Post VIDEO_MSG_CODEC_CHANGED");
      SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
    }

    CONTROLLOG("RecorderStateTransition:Recorder:VIDEO_MSG_CODEC_CHANGED:%d supress_transmitter_notification - %d OMX_CommandStateSet:%d",
               iRecorderState, m_bInternalEvent, m_IOMXState);

    if (m_NodeId->sendCommand(OMX_CommandStateSet, OMX_StateExecuting) != OK) {
      ERROR1(
        "VideoTransmitter::Configure::Failure in insrting the command for stateExecuting");
      return VIDEO_ERROR_UNKNOWN;
    }

    CHECK(sem_timedwait());
    CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
              m_IOMXState);
#ifdef GPU_ROTATION

    if (m_codecConfig.eHALVersion != CAMERA_HAL1) {
      for (uint8_t i = 0; i < EncoderBufferSize; i++) {
        RotationBuffer[i].UsedByIOMX = QP_FALSE;

        if (RotationBuffer[i].LocalBuffer != NULL) {
          CRITICAL2("VideoTransmitter::DeInit:: RotationBuffer index %d", i);
          RotationBuffer[i].LocalBuffer = NULL;
          RotationBuffer[i].CameraBuffer = NULL;
          RotationBuffer[i].CameraFence = NULL;
        }
      }
    } else {
      DEBUG1("Waiting for encoderbuffer_mutex");
      pthread_mutex_lock(&EncoderBuffer_mutex);
      DEBUG1("got the lock encoderbuffer_mutex");

      for (uint8_t i = 0; i < EncoderBufferSize; i++) {
        EncoderNativeBuffer[i].UsedByIOMX = QP_FALSE;

        if (EncoderNativeBuffer[i].outBuffer != NULL) {
          CRITICAL2("VideoTransmitter::DeInit:: RotationBuffer index %d", i);
          EncoderNativeBuffer[i].outBuffer = NULL;
        }

        if (EncoderNativeBuffer[i].nh != NULL) {
          native_handle_close(EncoderNativeBuffer[i].nh);
          native_handle_delete(EncoderNativeBuffer[i].nh);
          CRITICAL3_STR("VideoTransmitter::EncoderNativeBuffer[%d].nh %p", i,
                        EncoderNativeBuffer[i].nh);
          EncoderNativeBuffer[i].nh = NULL;
        }
      }

      pthread_mutex_unlock(&EncoderBuffer_mutex);
    }

#endif

    if (prevState == eVideoStateStopping) {
      //Transition to Paused with new configuration
      iRecorderState = eVideoStateStopping;
      CRITICAL1(" VideoTransmitter::Stop::   move to Pause command ");
    } else {
      //We stay at Executing state
      iRecorderState = eVideoStateActive;
    }
  }

  CRITICAL1("VideoTransmitter:: Configured");
  m_codecConfigured = QP_TRUE;

  /*we have to release all the pending buffers if any after codec reconfigured*/
  while (Cameratail != Camerahead) {
    CRITICAL3("[THINCLIENT]Buffers are not released to Camera, camerahead %d, cameratail %d",
              Cameratail, Camerahead);
    ReleasePendingBuffers();
  }

  return VIDEO_ERROR_OK;
}
QPE_VIDEO_ERROR VideoTransmitter::Validate_params(QP_VIDEO_CONFIG& tCodecConfig)
{
  if ((tCodecConfig.iWidth == 0) || (tCodecConfig.iHeight == 0)
      || (tCodecConfig.iBitRate == 0) || (tCodecConfig.iFrameRate == 0)) {
    CRITICAL1("Validate_params failed. Invalid Params");
    return VIDEO_ERROR_UNKNOWN;
  }

  if ((tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID)
      || (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO)) {
    CRITICAL1("Validate_params failed. Invalid or Unsupported codec");
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoTransmitter::AdaptVideoBitRate(uint32_t iBitRate)
{
  OMX_VIDEO_CONFIG_BITRATETYPE ConfigBitRate;
  QPE_VIDEO_ERROR result = VIDEO_ERROR_UNKNOWN;
  CRITICAL2("VideoTransmitter::AdaptVideoBitRate iBitRate = %d", iBitRate);
  memset(&ConfigBitRate, 0, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));
  InitEncoderOMXParams(&ConfigBitRate);
  ConfigBitRate.nPortIndex = OMX_DirOutput;

  do {
    if (iRecorderState != eVideoStateActive ) {
      CRITICAL2("VideoTransmitter::AdaptVideoBitRate: Video state is %d which is not active",
                iRecorderState);
      return VIDEO_ERROR_OK;
    }

    //configuration settings
    int res = m_NodeId->getConfig(OMX_IndexConfigVideoBitrate,
                                  &ConfigBitRate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

    if (res != OK) {
      break;
    }

    ConfigBitRate.nEncodeBitrate = iBitRate;
    res = m_NodeId->setConfig(OMX_IndexConfigVideoBitrate,
                              &ConfigBitRate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

    if (res != OK) {
      break;
    } else {
      result = VIDEO_ERROR_OK;
    }
  } while (0);

  return result;
}

status_t VideoTransmitter::ConfigureBitrateMode()
{
  status_t result = OMX_ErrorNone;
#ifdef RATE_CONTROL
  OMX_VIDEO_PARAM_BITRATETYPE bitrate;
  memset(&bitrate, 0, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));
  InitEncoderOMXParams(&bitrate);
  bitrate.nPortIndex = 1;
  result = m_NodeId->getParameter( OMX_IndexParamVideoBitrate,
                                   &bitrate, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));

  if (OK != result) {
    CRITICAL2("ConfigureBitrateMode:: OMX_IndexParamVideoBitrate %d getparam failed",
              result);
  }

  switch (m_codecConfig.eBitRateMode) {
    case BITRATE_MODE_VBR_CFR:
      bitrate.eControlRate = OMX_Video_ControlRateVariable;
      break;

    case BITRATE_MODE_CBR_CFR:
      bitrate.eControlRate = OMX_Video_ControlRateConstant;
      break;

    case BITRATE_MODE_VBR_VFR:
      bitrate.eControlRate = OMX_Video_ControlRateVariableSkipFrames;
      break;

    case BITRATE_MODE_CBR_VFR:
    default:
      bitrate.eControlRate = OMX_Video_ControlRateConstantSkipFrames;
      break;
  } //! switch (m_codecConfig.eBitRateMode)

  bitrate.nTargetBitrate = m_codecConfig.iBitRate;
  result = m_NodeId->setParameter( OMX_IndexParamVideoBitrate,
                                   &bitrate,
                                   sizeof(OMX_VIDEO_PARAM_BITRATETYPE));

  if (OK != result) {
    CRITICAL2("ConfigureBitrateMode:: OMX_IndexParamVideoBitrate %d setparam failed",
              result);
  } else {
    CRITICAL3("ConfigureBitrateMode:: OMX_IndexParamVideoBitrate bitrate - %d RC - %d",
              m_codecConfig.iBitRate, m_codecConfig.eBitRateMode);
  }

#endif
  return result;
}

QPE_VIDEO_ERROR VideoTransmitter::SetSourceTime(uint8_t useSystemTime)
{
  MINIMAL2("setting source time %d", useSystemTime);
  m_codecConfig.useSystemTime = useSystemTime;
  return VIDEO_ERROR_OK;
}


QPE_VIDEO_ERROR VideoTransmitter::SetRotationAngle(uint16_t rotation)
{
  QPE_VIDEO_ERROR result = VIDEO_ERROR_UNKNOWN;
  CRITICAL3("VideoTransmitter::SetRotationAngle rotation = %d, rec state %d",
            rotation, iRecorderState);
  /** We have already passed the ThinClient Check that Recorder is Started
    * So we post a Internal configure to CmdQ in any current state of Encoder
  */
  CRITICAL2("VideoTransmitter::SetRotationAngle: Video state is %d which is active",
            iRecorderState);
  m_codecConfig.rotation = rotation;
  OMX_EVENTDATA event;
  event.pData = (void *) &rotation;
  event.iLen = sizeof(uint16_t);

  if (m_pCallback)
    m_pCallback((QPE_VIDEO_MSG)VIDEO_MSG_INTERNAL_EVENT_TRIGGER_RECORDER,
                (void*) (&event), m_eVidDevType,
                SUCCESS, NULL);

  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoTransmitter::AdaptVideoFrameRate(uint32_t iFrameRate)
{
  OMX_CONFIG_FRAMERATETYPE  ConfigFrameRate;
  OMX_VIDEO_CONFIG_AVCINTRAPERIOD idr_period;
  QPE_VIDEO_ERROR result = VIDEO_ERROR_UNKNOWN;
  int res = VIDEO_ERROR_OK;
  CRITICAL2("VideoTransmitter::AdaptVideoFrameRate iFrameRate = %d ", iFrameRate);
  memset(&ConfigFrameRate, 0, sizeof(OMX_CONFIG_FRAMERATETYPE));
  InitEncoderOMXParams(&ConfigFrameRate);
  InitEncoderOMXParams(&idr_period);
  ConfigFrameRate.nPortIndex = OMX_DirOutput;

  do {
    if (iRecorderState != eVideoStateActive ) {
      CRITICAL2("VideoTransmitter::AdaptVideoFrameRate: Video state is %d which is not active",
                iRecorderState);
      result = VIDEO_ERROR_OK;
      break;
    }

    //configuration settings
    res = m_NodeId->getConfig(OMX_IndexConfigVideoFramerate,
                              &ConfigFrameRate, sizeof(OMX_CONFIG_FRAMERATETYPE));

    if (res != OK) {
      break;
    }

    m_codecConfig.iFrameRate = iFrameRate;
    ConfigFrameRate.xEncodeFramerate = iFrameRate << 16;
    res = m_NodeId->setConfig(OMX_IndexConfigVideoFramerate,
                              &ConfigFrameRate, sizeof(OMX_CONFIG_FRAMERATETYPE));

    if (res != OK) {
      break;
    }

    result = AdaptVideoIdrPeriodicity(m_codecConfig.iFrameInterval);
  } while (0);

  return result;
}


QPE_VIDEO_ERROR VideoTransmitter::AdaptVideoIdrPeriodicity(int32_t iFrameInt)
{
  OMX_VIDEO_CONFIG_AVCINTRAPERIOD idr_period;
  QOMX_VIDEO_INTRAPERIODTYPE intraperiod;
  QPE_VIDEO_ERROR result = VIDEO_ERROR_UNKNOWN;
  int res = VIDEO_ERROR_OK;
  CRITICAL2("VideoTransmitter::AdaptVideoIdrPeriodicity iFrameInt = %d",
            iFrameInt);
  InitEncoderOMXParams(&idr_period);
  m_codecConfig.iFrameInterval = iFrameInt;
  //calculate nPFrames and IDRPeriod
  int nPFrames = (int) (m_codecConfig.iFrameRate * iFrameInt) - 1;
  int nIDRPeriod = (int) 1;

  if (iRecorderState != eVideoStateActive ) {
    CRITICAL2("VideoTransmitter::AdaptVideoIdrPeriodicity: Video state is %d which is not active",
              iRecorderState);
    return VIDEO_ERROR_OK;
  }
  /*If all the frames need to be marked as IDR frames*/
  else if (0 == iFrameInt) {
    nPFrames = 0;
    nIDRPeriod = 1;
  } else if (m_bLTRFeatureOn || MAX_IDR_INTERVAL == iFrameInt) {
    /* If LTR is enabled, then set IDR frame interval to Max value*/
    nPFrames = MAX_IDR_PERIOD;
    nIDRPeriod = 0;
  }

  if (m_codecConfig.eCodec == VIDEO_CODEC_H265) {
    intraperiod.nPortIndex = (OMX_U32) DIR_OUT;
    res = m_NodeId->getConfig((OMX_INDEXTYPE) QOMX_IndexConfigVideoIntraperiod,
                              (OMX_PTR *) &intraperiod,
                              sizeof(QOMX_VIDEO_INTRAPERIODTYPE));

    if (res == OMX_ErrorNone) {
      intraperiod.nPFrames = nPFrames;
      CRITICAL3("VideoTransmitter:: iframeinterval.nPFrames %lu nIDRPeriod %d",
                nPFrames, nIDRPeriod);
      intraperiod.nIDRPeriod = nIDRPeriod;
      res = m_NodeId->setConfig((OMX_INDEXTYPE) QOMX_IndexConfigVideoIntraperiod,
                                (OMX_PTR *) &intraperiod,
                                sizeof(QOMX_VIDEO_INTRAPERIODTYPE));

      if (res != OK) {
        ERROR2("VideoTransmitter:: QOMX_IndexConfigVideoIntraperiod err %d", res);
      } else {
        result = VIDEO_ERROR_OK;
      }
    }
  } else {
    idr_period.nPortIndex = (OMX_U32) DIR_OUT;
    res = m_NodeId->getConfig((OMX_INDEXTYPE) OMX_IndexConfigVideoAVCIntraPeriod,
                              (OMX_PTR *) &idr_period,
                              sizeof(OMX_VIDEO_CONFIG_AVCINTRAPERIOD));

    if (res == OMX_ErrorNone) {
      idr_period.nPFrames = nPFrames;
      CRITICAL3("VideoTransmitter:: iframeinterval.nPFrames %lu nIDRPeriod %d",
                nPFrames, nIDRPeriod);
      idr_period.nIDRPeriod = nIDRPeriod;
      res = m_NodeId->setConfig((OMX_INDEXTYPE) OMX_IndexConfigVideoAVCIntraPeriod,
                                (OMX_PTR *) &idr_period,
                                sizeof(OMX_VIDEO_CONFIG_AVCINTRAPERIOD));

      if (res != OK) {
        ERROR2("VideoTransmitter:: OMX_IndexConfigVideoAVCIntraPeriod err %d", res);
      } else {
        result = VIDEO_ERROR_OK;
      }
    }
  }

  return result;
}


/************************************************************************************************
 Video Encoder EventHandler callback function
 ************************************************************************************************/
void VideoTransmitter::IOMXVideoEncEventHandler(sp<IOMXNode> /*node*/,
    OMX_EVENTTYPE eEvent, OMX_U32 Data1, OMX_U32 Data2)
{
  CRITICAL3(
    "VideoTransmitter::IOMXVideoEncEventHandler:: event handler callback command/index %d, m_iomxState %d",
    Data1, Data2);

  if (eEvent == OMX_EventCmdComplete) {
    OMX_COMMANDTYPE iCommandReceived = (OMX_COMMANDTYPE) Data1;
    OMX_STATETYPE iStateReceived = (OMX_STATETYPE) Data2;

    if (iCommandReceived == OMX_CommandStateSet) {
      m_IOMXState = iStateReceived;

      if (iStateReceived == OMX_StateIdle) {
        EVENT2(
          "VideoTransmitter::IOMXVideoEncEventHandler:: component is in the state IDLE and recorder state is %d",
          iRecorderState);

        if (iRecorderState != eVideoStateReleasing) {
          iRecorderState = eVideoStateIdle;
        }

        SEM_POST();
      } else if (iStateReceived == OMX_StateLoaded) {
        EVENT2("VideoTransmitter::IOMXVideoEncEventHandler:: component is in the state LOADED and recorder state is %d",
               iRecorderState);

        if (iRecorderState == eVideoStateReleasing) {
        }

        SEM_POST();
      } else if (iStateReceived == OMX_StateExecuting) {
        EVENT2("VideoTransmitter::IOMXVideoEncEventHandler:: component is in the state Executing encoding is in progress and recorder state is %d",
               iRecorderState);
        SEM_POST();
        CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
                  m_IOMXState);
      } else if (iStateReceived == OMX_StatePause) {
        EVENT1("VideoReceiver::IOMXVideoEncEventHandler:: component is in the  Paused State");
        SEM_POST();
      } else if (iStateReceived == OMX_StateInvalid) {
        ERROR1(" VideoTransmitter::IOMXVideoEncEventHandler:: Component is in wrong state and returning");
        return;
      }
    } else if (iCommandReceived == OMX_CommandPortDisable) {
      EVENT1(
        "VideoTransmitter::IOMXVideoEncEventHandler:: Received OMX_CommandPortDisable event");
      EVENT1(
        "VideoTransmitter::IOMXVideoEncEventHandler:: not updated any state here");
      SEM_POST();
    } else if (iCommandReceived == OMX_CommandPortEnable) {
      EVENT1(
        "VideoTransmitter::IOMXVideoEncEventHandler:: Received OMX_CommandPortEnable event");
      EVENT1(
        "VideoTransmitter::IOMXVideoEncEventHandler:: not updated any state here");
      SEM_POST();
    }
  } else if (eEvent == OMX_EventPortSettingsChanged) {
    OMX_INDEXTYPE iIndexReceived = (OMX_INDEXTYPE) Data1;
    OMX_STATETYPE iStateReceived = (OMX_STATETYPE) Data2;
  } else if (eEvent == OMX_EventPortFormatDetected) {
    //nothing to be done here
  } else if (eEvent == OMX_EventError) {
    CRITICAL2(
      " VideoTransmitter::IOMXVideoEncEventHandler::OMX error received with error type 0x%x",
      (OMX_ERRORTYPE) Data1);
    //Notify the error to RTP to trigger Cleanup
    SendEventsToThinclient(VIDEO_MSG_ERROR);
  }

  return;
}

/************************************************************************************************
 Video Encoder EmptyBufferDone callback function
 ************************************************************************************************/
void VideoTransmitter::IOMXVideoEncEmptyBufferDone(sp<IOMXNode> node,
    IOMX::buffer_id buffer)
{
  uint8_t i = 0;
  EVENT1(
    "VideoTransmitter::VideoEncEmptyBufferDone:: In Encoder EmptyBufferDone callback");
  tempemptybuffercallbackcountenc++;
  timeval tv;
  gettimeofday(&tv, NULL);

  if (node == NULL) {
    ERROR2("TIMEDELAY::IOMXVideoEncEmptyBufferDone omx node null, frame = %d",
           tempemptybuffercallbackcountenc);
    return;
  }

  for (i = 0; i < m_iMinInputBufCount; i++) {
    if (m_pEncInpBufHeader_iomx[i]->buffer_id == buffer) {
      m_pEncInpBufHeader_iomx[i]->filllen = 0;
      break;
    }
  }

#ifdef GPU_ROTATION
  int countencoder;
  pthread_mutex_lock(&EncoderBuffer_mutex);
  DEBUG1("got the lock encoderbuffer_mutex");

  for (countencoder = 0; countencoder < EncoderBufferSize; countencoder++) {
    if (EncoderNativeBuffer[countencoder].UsedByIOMX == QP_TRUE) {
      if (EncoderNativeBuffer[countencoder].bufferid ==
          m_pEncInpBufHeader_iomx[i]->buffer_id) {
        //allocate buffer if this is new slot we are using.
        EncoderNativeBuffer[countencoder].UsedByIOMX = QP_FALSE;
      }
    }
  }

  pthread_mutex_unlock(&EncoderBuffer_mutex);
#endif
  CRITICAL5_STR(
    "Profiling ENCODER->IMS EmptyBufferDone frame = %d, time = %llu buffid = %d index %d",
    tempemptybuffercallbackcountenc,
    (unsigned long long)TIMEVAL2MS(tv), buffer, i);
  return;
}

/************************************************************************************************
 Video Encoder FillBufferDone callback function
 ************************************************************************************************/
void VideoTransmitter::IOMXVideoEncFillBufferDone(sp<IOMXNode> node,
    IOMX::buffer_id buffer, OMX_U32 range_offset, OMX_U32 range_length,
    OMX_U32 flags, OMX_TICKS
    timestamp//, OMX_PTR platform_private,OMX_PTR data_ptr, OMX_U32 pmem_offset
                                                 )
{
  EVENT1(
    "VideoTransmitter::VideoEncFillBufferDone:: In Encoder FillBufferDone callback");

  if (node == NULL) {
    MINIMAL1("VideoTransmitter::IOMXVideoEncFillBufferDone:: NULL\n");
    return;
  }

  tempfillbuffercallbackcountenc++;
  timeval tv;
  uint64_t  appsTS;
  uint64_t  modemTS;
  gettimeofday(&tv, NULL);
  CRITICAL6_STR(
    "Profiling ENCODER->IMS Fillbufferdone frame = %d, appstime = %llu buffer = %d frametime=%llu length=%d",
    tempfillbuffercallbackcountenc, (unsigned long long)TIMEVAL2MS(tv),
    buffer, (unsigned long long)timestamp, (int)range_length);
  uint8_t i = 0;

  while (i < m_iMinOutputBufCount
         && m_pEncOutpBufHeader_iomx[i]->buffer_id != buffer) {
    ++i;
  }

  m_pEncOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
  DEBUG3(
    "VideoTransmitter::IOMXVideoEncFillBufferDone   m_pEncOutpBufHeader_iomx[i]->buffer_id = %d range offset %d",
    m_pEncOutpBufHeader_iomx[i]->buffer_id, range_offset);
  GetModemTSForCameraTS(timestamp, &modemTS, &appsTS);
  /*if(timestamp == 0)
    {
      timestamp = modem_time +((tv.tv_sec * 1000) + (tv.tv_usec / 1000))-apps_time - 5; //this is to handle the sps pps case where the timestamp output is coming as zero.
      LOGE(
      "TIMEDELAY::IOMXVideoEncFillBufferDone frame = %d, time = %lu buffer = %p frametime=%llu",
      tempfillbuffercallbackcountenc,
      ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)), buffer,timestamp);
    }
  else {
    timestamp  = timestamp / 1000;
  }*/
  char *data = (char *) m_pEncOutpBufHeader_iomx[i]->pBuffer;

  //! IDR retransmit is enabled if IDR interval is more than 6sec or if no
  //! periodic IDR frame is being generated
  if ((m_IDRRetransmitOn) && (iRecorderState == eVideoStateActive) ) {
    if ((flags & OMX_BUFFERFLAG_SYNCFRAME) && !m_LastIDRtimestamp) {
      m_LastIDRtimestamp = (uint32)tv.tv_sec;;
    }

    DEBUG4("m_IDRRetransmitOn %d tv.tv_sec %lu m_LastIDRtimestamp : %u",
           m_IDRRetransmitOn, tv.tv_sec, m_LastIDRtimestamp);

    if (m_LastIDRtimestamp
        && (((uint32)tv.tv_sec - (uint32)m_LastIDRtimestamp) > m_IDRframeinterval)) {
      /** Request an I-FRAME */
      OMX_CONFIG_INTRAREFRESHVOPTYPE vop_refresh;
      memset(&vop_refresh, 0, sizeof(OMX_CONFIG_INTRAREFRESHVOPTYPE));
      InitEncoderOMXParams(&vop_refresh);
      vop_refresh.nPortIndex = OMX_DirOutput;
      vop_refresh.IntraRefreshVOP = OMX_TRUE;
      int res = m_NodeId->setConfig(
                  OMX_IndexConfigVideoIntraVOPRefresh,
                  &vop_refresh, sizeof(vop_refresh));

      if (res != OK) {
        ERROR2("VideoTransmitter::setParam OMX_IndexConfigVideoIntraVOPRefresh with return value %x",
               res);
      }

      if (m_IDRframeinterval == 3) {
        m_IDRframeinterval = 6;
      } else {
        m_IDRRetransmitOn = 0;
      }

      CRITICAL1("FTB Trigger IDRframe ");
    }
  }

  timestamp = modemTS;
  CRITICAL5("FTB_DONE : data[4] : 0x%x m_IDRRetransmitOn : %d m_LastIDRtimestamp : %u tv.tv_sec : %lu",
            data[4], m_IDRRetransmitOn, m_LastIDRtimestamp, tv.tv_sec);
  m_OmxFrameFlags = flags;
  SendEncodedFramesToAppfromIOMX(timestamp, appsTS,
                                 m_pEncOutpBufHeader_iomx[i],
                                 range_offset, range_length);

  if (flags & OMX_BUFFERFLAG_SYNCFRAME) {
    DEBUG2("IDR frame received seq no : %d", tempfillbuffercallbackcountenc);
  }

  uint8_t j = 0;

  for (j = 0; j < 6; j++) {
    DEBUG3("Encoder Header[%d] = %d", j, data[j]);
  }

#ifdef __DEBUG_PAYLOAD__

  //Print the first 20 bytes for first frame
  if (range_length <= 80) {
    uint8_t i;

    for (i = 0; i < range_length; i++) {
      CRITICAL3("Encoder SPS/PPS Data [%d] = %d", i, data[i]);
    }
  }

#endif
  return;
}

int VideoTransmitter::VerifyFillBuffersDone()
{
  TRACE1("VideoTransmitter::VerifyFillBuffersDone");
  uint8_t i = 0;

  for (i = 0; i < m_iMinOutputBufCount; i++) {
    if (m_pEncOutpBufHeader_iomx[i]->usedbyIOMX == QP_TRUE) {
      EVENT1(
        "VideoTransmitter::VerifyFillBuffersDone returning QP_FALSE ");
      return QP_FALSE;
    }
  }

  if (i == m_iMinOutputBufCount) {
    EVENT1("VideoTransmitter::VerifyFillBuffersDone returning QP_TRUE ");
    return QP_TRUE;
  } else {
    return QP_FALSE;
  }
}

/************************************************************************************************
 Parametsrs which are Common to all the codecs are set here
 ************************************************************************************************/


QPE_VIDEO_ERROR VideoTransmitter::ConfigureCommonEncoderInputPort(
  QP_VIDEO_CONFIG sVideoConfig)
{
  TRACE1("VideoTransmitter::ConfigureCommonEncoderPort");
  OMX_PARAM_PORTDEFINITIONTYPE pParamDef;
  OMX_VIDEO_PARAM_PORTFORMATTYPE pParamPort;
  int err = OK;
  memset(&pParamDef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  memset(&pParamPort, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
  InitEncoderOMXParams(&pParamDef);
  InitEncoderOMXParams(&pParamPort);
  pParamDef.nPortIndex = DIR_INP;
  pParamDef.eDir = (OMX_DIRTYPE)DIR_INP;

  if (m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                             (OMX_PTR*) &pParamDef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE))
      != OK) {
    ERROR1("VideoTransmitter::ConfigureCommonEncoderPort :: Failed to get the parameter for port definition");
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT1("VideoTransmitter::ConfigureCommonEncoderPort ::Parameters : \n");
  CRITICAL5("iWidth:%d iHeight:%d iBitRate:%d iFrameRate:%d\n",
            sVideoConfig.iWidth, sVideoConfig.iHeight, sVideoConfig.iBitRate,
            sVideoConfig.iFrameRate);
  pParamDef.format.video.nFrameWidth = sVideoConfig.iWidth;
  pParamDef.format.video.nFrameHeight = sVideoConfig.iHeight;

//#ifdef MSM8909
  if (sVideoConfig.eCodec == VIDEO_CODEC_H263
      || sVideoConfig.eCodec == VIDEO_CODEC_H265) {
    pParamDef.format.video.nSliceHeight = pParamDef.format.video.nFrameHeight;
  }

  //pParamDef.format.video.nBitrate = sVideoConfig.iBitRate;
  pParamDef.format.video.xFramerate = (sVideoConfig.iFrameRate << 16);
  //pParamDef.nBufferCountActual = pParamDef.nBufferCountActual;

  if (m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                             (OMX_PTR*) &pParamDef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE)) != OK) {
    ERROR1("VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for port definition");
    return VIDEO_ERROR_UNKNOWN;
  }

  pParamPort.nIndex = 0;
  pParamPort.nPortIndex = DIR_INP;

  if (m_NodeId->getParameter(OMX_IndexParamVideoPortFormat,
                             (OMX_PTR*) &pParamPort, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))
      != OK) {
    ERROR1("VideoTransmitter::ConfigureCommonEncoderPort :: Failed to get the parameter for port format");
    return VIDEO_ERROR_UNKNOWN;
  }

  pParamPort.nIndex = 0;
  pParamPort.xFramerate = (sVideoConfig.iFrameRate << 16);
  CRITICAL3("m_codecConfig.eHALVersion : %d m_codecConfig.iInputVideoBufferMode %d",
            m_codecConfig.eHALVersion, m_codecConfig.iInputVideoBufferMode);

  if (m_codecConfig.eHALVersion == CAMERA_HAL3) {
    if (sVideoConfig.iInputVideoBufferMode == VIDEO_BUFFER_NON_META_MODE) {
      pParamPort.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
    } else {
      //this is needed for Shamu camera2 API create input surface
      pParamPort.eColorFormat = OMX_COLOR_FormatAndroidOpaque;
    }
  } else {
//camera1
#ifndef COLOR_FORMAT_NV21
    pParamPort.eColorFormat =
      OMX_COLOR_FormatYUV420SemiPlanar; //Only supported color format
#else

    /*
       Changing the colour format to YVUSemiPlanar as suggested by Camera and OMX team
       to avoid mismatching colours of the objects focussed which is reflected in
       peer end far end video.
               Change here is the format changed from YUV420Semiplanar to YVU420Semiplanar
           */
    if (m_codecConfig.iInputVideoBufferMode == VIDEO_BUFFER_NON_META_MODE) {
      CRITICAL1("VideoTransmitter::ConfigureCommonEncoderPort::Setting color format as OMX_COLOR_FormatYUV420SemiPlanar");
      pParamPort.eColorFormat =
        OMX_COLOR_FormatYUV420SemiPlanar; //Only supported color format
    } else {
      CRITICAL1("VideoTransmitter::ConfigureCommonEncoderPort::Setting color format as YVU420SemiPlanar");
      pParamPort.eColorFormat =
        OMX_QCOM_COLOR_FormatYVU420SemiPlanar; //Only supported color format
    }

#endif
  }

#ifdef MSM8937

  if (sVideoConfig.eCodec == VIDEO_CODEC_H263) {
    pParamPort.eColorFormat = OMX_QCOM_COLOR_FormatYVU420SemiPlanar;
  }

#endif
  CRITICAL2("VideoTransmitter::ConfigureCommonEncoderPort eColorFormat 0x%x",
            pParamPort.eColorFormat);

  if (m_NodeId->setParameter(OMX_IndexParamVideoPortFormat,
                             (OMX_PTR*) &pParamPort, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))
      != OK) {
    ERROR1(
      "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for port format");
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}


QPE_VIDEO_ERROR VideoTransmitter::ConfigureCommonEncoderOutputPort(
  QP_VIDEO_CONFIG sVideoConfig)
{
  TRACE1("VideoTransmitter::ConfigureCommonEncoderPort");
  OMX_PARAM_PORTDEFINITIONTYPE pParamDef;
  OMX_VIDEO_PARAM_PORTFORMATTYPE pParamPort;
  OMX_QCOM_VIDEO_PARAM_QPRANGETYPE pQPRange;
  memset(&pParamDef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  memset(&pParamPort, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
  memset(&pQPRange, 0, sizeof(OMX_QCOM_VIDEO_PARAM_QPRANGETYPE));
  InitEncoderOMXParams(&pParamDef);
  InitEncoderOMXParams(&pParamPort);
  InitEncoderOMXParams(&pQPRange);
  pParamDef.nPortIndex = DIR_OUT;
  pParamDef.eDir = (OMX_DIRTYPE)DIR_OUT;

  if (m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                             (OMX_PTR*) &pParamDef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE))
      != OK) {
    ERROR1(
      "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to get the parameter for port definition");
    return VIDEO_ERROR_UNKNOWN;
  }

  DEBUG1("VideoTransmitter::ConfigureCommonEncoderPort ::Parameters : \n");
  CRITICAL2("Output Port iWidth :  %d\n", sVideoConfig.iOut_Width);
  CRITICAL2("Output Port iHeight : %d\n", sVideoConfig.iOut_Height);
  DEBUG2("iBitRate : %d\n", sVideoConfig.iBitRate);
  DEBUG2("iFrameRate : %d\n", sVideoConfig.iFrameRate);
  pParamDef.format.video.nFrameWidth = sVideoConfig.iOut_Width;
  pParamDef.format.video.nFrameHeight = sVideoConfig.iOut_Height;
  pParamDef.format.video.nBitrate = sVideoConfig.iBitRate;
  pParamDef.format.video.xFramerate = (sVideoConfig.iFrameRate << 16);
#ifdef MSM8909

  if (sVideoConfig.eCodec == VIDEO_CODEC_H263 ) {
    pParamDef.format.video.nSliceHeight = pParamDef.format.video.nFrameHeight;
  }

#endif

  switch (sVideoConfig.eCodec) {
    case VIDEO_CODEC_H263:
      pParamDef.format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
      break;

    case VIDEO_CODEC_MPEG4_XVID:
    case VIDEO_CODEC_MPEG4_ISO:
      pParamDef.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
      pParamDef.format.video.eColorFormat =
        OMX_COLOR_FormatYUV420SemiPlanar; //need for both inp/out
      break;

    case VIDEO_CODEC_H264:
      pParamDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
      break;

    case VIDEO_CODEC_H265:
      pParamDef.format.video.eCompressionFormat = OMX_VIDEO_CodingHEVC;
      break;

    default:
      break;
  }

  if (m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                             (OMX_PTR*) &pParamDef, sizeof(OMX_PARAM_PORTDEFINITIONTYPE)) != OK) {
    ERROR1(
      "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for port definition");
    return VIDEO_ERROR_UNKNOWN;
  }

  pParamPort.nIndex = 1;
  //Initialize the parameter ports
  pParamPort.nPortIndex = DIR_OUT;

  if (m_NodeId->getParameter(OMX_IndexParamVideoPortFormat,
                             (OMX_PTR*) &pParamPort, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))
      != OK) {
    ERROR1(
      "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to get the parameter for port format");
    return VIDEO_ERROR_UNKNOWN;
  }

  pParamPort.nIndex = 1;
  pParamPort.xFramerate = (sVideoConfig.iFrameRate << 16);

  switch (sVideoConfig.eCodec) {
    case VIDEO_CODEC_H263:
      pParamPort.eCompressionFormat = OMX_VIDEO_CodingH263;
      break;

    case VIDEO_CODEC_MPEG4_XVID:
    case VIDEO_CODEC_MPEG4_ISO:
      pParamPort.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
      break;

    case VIDEO_CODEC_H264:
      pParamPort.eCompressionFormat = OMX_VIDEO_CodingAVC;
      break;

    case VIDEO_CODEC_H265:
      pParamPort.eCompressionFormat = OMX_VIDEO_CodingHEVC;
      break;

    default:
      //Assigning H263 as default
      pParamPort.eCompressionFormat = OMX_VIDEO_CodingH263;
      break;
  }

  if (m_NodeId->setParameter(OMX_IndexParamVideoPortFormat,
                             (OMX_PTR*) &pParamPort, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE))
      != OK) {
    ERROR1(
      "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for port format");
    return VIDEO_ERROR_UNKNOWN;
  }

  /***********************************************************************************************
   Set the OMX Encoder Output Port QPRange through IMS,also provide a property to set by user.
   ***********************************************************************************************/
  pQPRange.nPortIndex = DIR_OUT;

  if (m_NodeId->getParameter((OMX_INDEXTYPE)OMX_QcomIndexParamVideoQPRange,
                             (OMX_PTR*) &pQPRange, sizeof(OMX_QCOM_VIDEO_PARAM_QPRANGETYPE)) != OK) {
    ERROR1("VideoTransmitter::ConfigureCommonEncoderOutputPort :: Failed to get the parameter for output port QPRange!");
    return VIDEO_ERROR_UNKNOWN;
  }

  pQPRange.minQP = m_ulMinQpValue;
  pQPRange.maxQP = m_ulMaxQpValue;

  if (m_NodeId->setParameter((OMX_INDEXTYPE)OMX_QcomIndexParamVideoQPRange,
                             (OMX_PTR*) &pQPRange, sizeof(OMX_QCOM_VIDEO_PARAM_QPRANGETYPE )) != OK) {
    ERROR1("VideoTransmitter::ConfigureCommonEncoderOutputPort :: Failed to set the parameter for output port QPRange!");
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}

void create_camera_yuv_dump(int width, int height, void *buf)
{
  if (buf) {
    //this logic is only for without UBWC mode VENUS_BUFFER_SIZE
    int k;
    int stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, width);
    int scanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, height);
    unsigned char *pvirt, *ptemp;

    if (fpRawSend == NULL) {
      fpRawSend = fopen("/sdcard/RawSend.yuv", "wb");
    }

    if (fpRawSend != NULL) {
      CRITICAL3("buffer height=%d width =%d", height, width);
      char *temp = (char *)buf;
      int msize = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, width, height);
      CRITICAL4("buffer height=%d width =%d size=%d", height, width, msize);

      for (k = 0; k <  height; k++) {
        fwrite(temp, width, 1, fpRawSend);
        temp += stride;
      }

      temp = (char *)buf + (stride * scanlines);

      for (k = 0; k <  height / 2; k++) {
        fwrite(temp, width, 1, fpRawSend);
        temp += stride;
      }
    }
  } else {
    ERROR1("create_camera_yuv_dump buf is null");
  }
}

QPE_VIDEO_ERROR VideoTransmitter::WaitTillAllBuffersAreConsumed()
{
  if (eVideoStateActive == iRecorderState) {
    uint8_t i = 0;
    uint8_t waitCount = 0;
    bool loopExit = QP_TRUE;

    do {
      loopExit = QP_TRUE;

      for (i = 0; i < m_iMinInputBufCount; i++) {
        CRITICAL2("VideoTransmitter::WaitTillAllBuffersAreConsumed:; For Loop %d",  i);

        if (m_pEncInpBufHeader_iomx[i] == NULL) {
          CRITICAL3_STR("m_pEncInpBufHeader_iomx[i] %p is NULL i : %d",
                        m_pEncInpBufHeader_iomx[i], i);
          return VIDEO_ERROR_UNKNOWN;
        }

        if (m_pEncInpBufHeader_iomx[i]->filllen != 0) {
          CRITICAL3_STR("Waiting for BufRelease %p and index : %d",
                        m_pEncInpBufHeader_iomx[i], i);
          loopExit = QP_FALSE;
          break;
        }
      }

      if (loopExit == QP_FALSE) {
        waitCount++;
        usleep(10000);
      }
    } while ((!loopExit) && (waitCount < 10));

    if (loopExit == QP_TRUE) {
      return VIDEO_ERROR_OK;
    } else {
      ERROR1("WaitTillAllBuffersAreConsumed timed out!!!!");
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  return VIDEO_ERROR_OK;
}


int VideoTransmitter::isCameraResolutionMatching()
{
  return (cameraHeight==m_codecConfig.iHeight && cameraWidth==m_codecConfig.iWidth);
}

/* This API will process extra data provided at the end of frame.*/
void VideoTransmitter::ProcessExtraData(uint32_t ulRangeOffset,
                                        uint32_t ulRangeLen,
                                        void* pFrameBuf, uint32_t* pulLtrExtraData)
{
  if (!pFrameBuf || !pulLtrExtraData) {
    return;
  }

  //! Initialize with default value 0xFFFF
  *pulLtrExtraData = 0xFFFF;

  if (!m_bLTRFeatureOn) {
    return;
  }

  EVENT4("Tx-LTR: OMX flags 0x%x, and offset 0x%x, range len %d",
         m_OmxFrameFlags, ulRangeOffset, ulRangeLen);
  OMX_U32 utemp = (m_OmxFrameFlags & OMX_BUFFERFLAG_EXTRADATA);

  if (m_OmxFrameFlags & OMX_BUFFERFLAG_EXTRADATA) {
    uint32_t ulExtraDataOffset = ulRangeOffset + ulRangeLen;
    //! Extra data pointer needs to be 32bi aligned
    char *pExtraDataBuf = (char *) pFrameBuf;
    pExtraDataBuf = (char *)((uint64_t)(pExtraDataBuf + ulExtraDataOffset + 3) &
                             (~3));
    OMX_OTHER_EXTRADATATYPE* pExtraDataPtr = (OMX_OTHER_EXTRADATATYPE *)
        pExtraDataBuf;
    int nCount = 0;

    /* Check for Terminator for ExtraData type */
    while (pExtraDataPtr && OMX_ExtraDataNone !=  pExtraDataPtr->eType) {
      nCount++;
      EVENT3("Tx-LTR: Extra Data enabled in the given frame, etype 0x%x, frameno count %d",
             pExtraDataPtr->eType, m_ulFrameCount);

      if (OMX_ExtraDataVideoLTRInfo == pExtraDataPtr->eType) {
        OMX_U8 *pDataPtr = (OMX_U8 *)pExtraDataPtr->data;
        uint32_t ulDataVal = *((uint32_t*)pDataPtr);

        if (0xFFFF != ulDataVal) {
          *pulLtrExtraData = ulDataVal;
          CRITICAL5("Tx-LTR: ExtraData 0x%x, frame len %d, frameno count %d, pBuffer %p",
                    ulDataVal, ulRangeLen, m_ulFrameCount, pExtraDataPtr);
        }

        pExtraDataPtr += pExtraDataPtr->nDataSize;
        /* If more than one ExtraData support is needed, then remove break */
        break;
      } //!if (OMX_ExtraDataVideoLTRInfo == pExtraDataPtr->eType)

      /* This is a defensive check, in case above while condition fails for any reason*/
      if (nCount > 5) {
        break;
      }
    } //! while (pExtraDataPtr && ...)

    if (pExtraDataPtr) {
      DEBUG3("Tx-LTR: Break the loop, nCount %d, eType %d",
             nCount, pExtraDataPtr->eType);
    }
  } //! if(flags & OMX_BUFFERFLAG_EXTRADATA)

  return;
}

/* This API will be called to generate LTR-MARK frame.*/
void VideoTransmitter::ConfigLTRMark(int32_t slLtrIdx)
{
  QOMX_VIDEO_CONFIG_LTRMARK_TYPE ltrMark;
  InitEncoderOMXParams(&ltrMark);
  ltrMark.nPortIndex = 0;
  ltrMark.nID = slLtrIdx;
  int res = m_NodeId->setConfig((OMX_INDEXTYPE)QOMX_IndexConfigVideoLTRMark,
                                &ltrMark, sizeof(QOMX_VIDEO_CONFIG_LTRMARK_TYPE));

  if (OK != res) {
    ERROR3("Tx-LTR: Request to mark ltr-index %d failed with response %d",
           slLtrIdx, res);
  } else {
    CRITICAL2("Tx-LTR: LTR MARK request with index %d", slLtrIdx);
  }
}





/* This API will be called to generate LTR-USE frame.*/
void VideoTransmitter::ConfigLTRUse(int32_t slLtrIdx)
{
  QOMX_VIDEO_CONFIG_LTRUSE_TYPE ltrUse;
  InitEncoderOMXParams(&ltrUse);
  ltrUse.nPortIndex = 0;
  ltrUse.nID = 1 << slLtrIdx;
  ltrUse.nFrames = 0x01;
  int res = m_NodeId->setConfig((OMX_INDEXTYPE) QOMX_IndexConfigVideoLTRUse,
                                &ltrUse, sizeof(QOMX_VIDEO_CONFIG_LTRUSE_TYPE));

  if (OK != res) {
    ERROR3("Tx-LTR: Request to Use (eror recovery frame) ltr-index %d failed with response %d",
           slLtrIdx, res);
  } else {
    CRITICAL3("Tx-LTR: LTR USE request with index %d, Id requested to encoder 0x%x",
              slLtrIdx, ltrUse.nID);
  }
}

QPE_VIDEO_ERROR VideoTransmitter::ProcessRecordingFrame_HAL3(
  uint64_t  timestamp,
  void *dataPtr, size_t /*size*/)

{
  status_t status = NO_ERROR;
  timeval tv;
  uint64_t  currentAppsTS;
  gettimeofday(&tv, NULL);

  for (uint j = 0; j < m_iMinOutputBufCount; j++) {
    if (m_pEncOutpBufHeader_iomx[j] && !m_pEncOutpBufHeader_iomx[j]->usedbyIOMX) {
      m_pEncOutpBufHeader_iomx[j]->usedbyIOMX = QP_TRUE;
      CRITICAL4_STR("Profiling IMS->Encoder FillBuffer on %p j = %d appstime=%lu",
                    m_pEncOutpBufHeader_iomx[j], j, ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));

      if (m_NodeId->fillBuffer(m_pEncOutpBufHeader_iomx[j]->buffer_id,
                               OMXBuffer::sPreset, m_pEncOutpBufHeader_iomx[j]->fencefd) != OK) {
        ERROR1("VideoTransmitter::pushFramesFromCameraCB Failed to fill the buffer");
        return VIDEO_ERROR_UNKNOWN;
      }
    }
  }

  uint64_t  camerats = timestamp / 1000;
  currentAppsTS = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
  uint64_t  currentAVTime = 0;

  if (getModemTimeProperty() == 1 || m_codecConfig.useSystemTime) {
    AddCameraTSMap(camerats, getModemTime() + (currentAppsTS - getAppsTime()),
                   currentAppsTS);
  } else if (getAVTimerNode() < 0) {
    AddCameraTSMap(camerats, camerats / 1000, currentAppsTS);
  } else {
    currentAVTime = getCurrentAVTime();
    AddCameraTSMap(camerats, camerats / 1000,
                   currentAppsTS - ((currentAVTime - camerats) / 1000));
  }

  int slot = 0;
  BufferItem *Consumeritem = (BufferItem *)dataPtr;

  if (Consumeritem == NULL || Consumeritem->mGraphicBuffer.get() == NULL) {
    ERROR1("Transmitter mGraphicBuffer is null hence returning");
    return VIDEO_ERROR_UNKNOWN;
  }

  //changes because of security fixes google pushes to maintain order of IOMX api
  //checking for inputbufferproducer if null to return frame back to camera in case it is
  if (InputbufferProducer != NULL) {
#ifdef GPU_ROTATION
    DEBUG1("Waiting for encoderbuffer_mutex");
    pthread_mutex_lock(&EncoderBuffer_mutex);
    DEBUG1("got the lock encoderbuffer_mutex");
    int countencoder;

    for (countencoder = 0; countencoder < EncoderBufferSize; countencoder++) {
      if (RotationBuffer[countencoder].UsedByIOMX == QP_FALSE) {
        if (RotationBuffer[countencoder].LocalBuffer == NULL) {
          //allocate buffer if this is new slot we are using.
          //normally we should be end us using max 4-5 at one time together
          CRITICAL2(" RotationBuffer Allocation %d Firsttime", countencoder);
          RotationBuffer[countencoder].LocalBuffer  = new GraphicBuffer(
            m_codecConfig.iWidth,
            m_codecConfig.iHeight,
            Consumeritem->mGraphicBuffer->getPixelFormat(),
            Consumeritem->mGraphicBuffer->getUsage());
        }

        if (RotationBuffer[countencoder].LocalBuffer == NULL) {
          CRITICAL2(" Allocation failed for Rotationbuffer =%d", countencoder);
        } else {
          DEBUG2("Allocation set for Rotationbuffer =%d", countencoder);
          DEBUG2(" GraphicBuffer Allocation success %d", countencoder);
          RotationBuffer[countencoder].UsedByIOMX = QP_TRUE;
          RotationBuffer[countencoder].CameraBuffer = Consumeritem->mGraphicBuffer;
          RotationBuffer[countencoder].CameraBufferId =
            Consumeritem->mGraphicBuffer->getId();
          RotationBuffer[countencoder].LocalBufferId =
            RotationBuffer[countencoder].LocalBuffer->getId();
          //RotationBuffer[countencoder].CameraFence = Consumeritem->mFence;
        }

        break;
      }
    }

    pthread_mutex_unlock(&EncoderBuffer_mutex);

    if (countencoder < EncoderBufferSize) {
      if (RotationBuffer[countencoder].LocalBuffer != NULL) {
        rotateFrame(Consumeritem->mGraphicBuffer->getWidth(),
                    Consumeritem->mGraphicBuffer->getHeight(),
                    m_codecConfig.iHeight,
                    m_codecConfig.iWidth,
                    m_codecConfig.rotation,
                    Consumeritem->mGraphicBuffer->getNativeBuffer(),
                    RotationBuffer[countencoder].LocalBuffer->getNativeBuffer());
      }

      status = InputbufferProducer->attachBuffer(&slot,
               RotationBuffer[countencoder].LocalBuffer);

      if (status == NO_ERROR) {
        MINIMAL5_STR("IMS->Encoder queuebuffer CameraTS= %llu localid=%llx,cameraid=%llx currentavtime=%llu",
                     (unsigned long long)timestamp,
                     (long long)RotationBuffer[countencoder].LocalBuffer->getId(),
                     (long long)Consumeritem->mGraphicBuffer->getId(),
                     (unsigned long long)currentAVTime);
      }
    }

#else
    status = InputbufferProducer->attachBuffer(&slot, Consumeritem->mGraphicBuffer);

    if (status == NO_ERROR) {
      MINIMAL4_STR("IMS->Encoder queuebuffer CameraTS= %llu slotid=%llx currentavtime=%llu",
                   timestamp, (long long)Consumeritem->mGraphicBuffer->getId(), currentAVTime);
    }

#endif

    if (status == NO_ERROR) {
      IGraphicBufferProducer::QueueBufferOutput queueOutput;
      IGraphicBufferProducer::QueueBufferInput queueInput(
        timestamp,
        Consumeritem->mIsAutoTimestamp,
        Consumeritem->mDataSpace,
        Consumeritem->mCrop,
        static_cast<int32_t>(Consumeritem->mScalingMode),
        Consumeritem->mTransform,
        // Consumeritem->mIsDroppable,
        Consumeritem->mFence);
      status = InputbufferProducer->queueBuffer(slot, queueInput, &queueOutput);

      if (status != 0) {
        ERROR2("ProcessRecordingFrame queueing buffer to output (%d)", status);
      }
    }
  } else {
    ERROR1("Transmitter IGraphicBufferProducer is null");
    //QpDplImsThinClient *ptr = QpDplImsThinClient::getInstance();
    ReleaseToBufferToCamera(Consumeritem->mGraphicBuffer, Consumeritem->mFence);
  }

  return VIDEO_ERROR_OK;
}


QPE_VIDEO_ERROR VideoTransmitter::ProcessImageFrame(uint64_t  timestamp,
    void *dataPtr, size_t size)
{
  status_t status;
  timeval tv;
  uint64_t  currentAppsTS;
  gettimeofday(&tv, NULL);

  if (eVideoStateActive == iRecorderState) {
    uint8_t i = 0;
    native_handle_t *native_hdl = (native_handle_t *)dataPtr;

    if (native_hdl == NULL) {
      ERROR1("ProcessImageFrame: NATIVE_HDL is NULL hence returning");
      return VIDEO_ERROR_UNKNOWN;
    }

    for (; i < m_iMinInputBufCount; i++) {
      DEBUG2("VideoTransmitter::ProcessImageFrame:; For Loop %d", i);

      /*
           if( size > m_pEncInpBufHeader_iomx[i]->alloclen )
          {
        ERROR3("VideoTransmitter::ProcessRecordingFrame:size %d exceeds allocated length - %d hence returning error_unknown",size,m_pEncInpBufHeader_iomx[i]->alloclen);
        return VIDEO_ERROR_UNKNOWN;
      }
      */
      if (m_pEncInpBufHeader_iomx[i] == NULL) {
        MINIMAL3("m_pEncInpBufHeader_iomx[i] is NULL i : %d",
                 m_pEncOutpBufHeader_iomx[i], i);
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_pEncOutpBufHeader_iomx[i] == NULL) {
        MINIMAL2("m_pEncOutpBufHeader_iomx[i] is NULL i : %d", i);
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_pEncInpBufHeader_iomx[i]->filllen == 0) { //reuse the buffer which is free
        DEBUG3_STR(
          "VideoTransmitter::ProcessImageFrame alloclen: %d m_pEncInpBufHeader_iomx[i]->pBuffer = %p",
          (int)m_pEncInpBufHeader_iomx[i]->alloclen,
          m_pEncInpBufHeader_iomx[i]->pBuffer);

        if (m_pEncInpBufHeader_iomx[i]->pBuffer) {
          char *destbuffer =
            (char *)m_pEncInpBufHeader_iomx[i]->pBuffer;
          char *srcbuffer = (char *)dataPtr;
          DEBUG3("VideoTransmitter::Copying to destbuffer of size - %d, src size %d",
                 (int)m_pEncInpBufHeader_iomx[i]->alloclen, (int)size);

          if ( srcbuffer && destbuffer ) {
            memcpy(destbuffer, srcbuffer, size);
          } else {
            CRITICAL5_STR("VideoTransmitter::destbuffer - %p srcbuffer - %p dstSize %d srcSize %d",
                          destbuffer, srcbuffer,
                          (int)m_pEncInpBufHeader_iomx[i]->alloclen, (int)size);
            return VIDEO_ERROR_OK;
          }

          timeval tv;
          gettimeofday(&tv, NULL);
          uint64_t currentAppsTS = 0;
          uint64_t currentAVTime = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));

          for (uint j = 0; j < m_iMinOutputBufCount; j++) {
            if (m_pEncOutpBufHeader_iomx[j] && !m_pEncOutpBufHeader_iomx[j]->usedbyIOMX) {
              m_pEncOutpBufHeader_iomx[j]->usedbyIOMX = QP_TRUE;
              CRITICAL4_STR("Profiling IMS->Encoder FillBuffer on %p j = %d appstime=%llu",
                            m_pEncOutpBufHeader_iomx[j], j, (unsigned long long)currentAVTime);

              if (m_NodeId->fillBuffer(m_pEncOutpBufHeader_iomx[j]->buffer_id,
                                       OMXBuffer::sPreset, m_pEncOutpBufHeader_iomx[j]->fencefd) != OK) {
                ERROR1(
                  "VideoTransmitter::ProcessImageFrame Failed to fill the buffer");
                return VIDEO_ERROR_UNKNOWN;
              }
            }
          }

          DEBUG3_STR(
            "VideoTransmitter::ProcessImageFrame:: Before emptying the buffer with bufflen = %d and pointer = %p",
            (int)m_pEncInpBufHeader_iomx[i]->alloclen,
            m_pEncInpBufHeader_iomx[i]->pBuffer);
          CRITICAL6_STR("Profiling IMS-> ENCODER EmptyBuffer = %d,localtime= %llu buffid = %d index = %d,len=%d",
                        tempemptybuffercallbackcountenc + 1,
                        (unsigned long long)currentAppsTS,
                        m_pEncInpBufHeader_iomx[i]->buffer_id, i,
                        (int)m_pEncInpBufHeader_iomx[i]->alloclen);

          //timestamp = timestamp * 1000;
          if (getModemTimeProperty() == 1 || m_codecConfig.useSystemTime) {
            AddCameraTSMap(timestamp, getModemTime() + (currentAppsTS - getAppsTime()),
                           currentAppsTS);
          } else if (getAVTimerNode() < 0) {
            AddCameraTSMap(timestamp, timestamp / 1000, currentAppsTS);
          } else {
            currentAVTime = getCurrentAVTime();
            AddCameraTSMap(timestamp, timestamp / 1000,
                           currentAppsTS - ((currentAVTime - timestamp) / 1000));
          }

          CRITICAL4_STR("Profiling IMS->ENCODER ETB ts : %llu appstime = %llu currentAVTime %llu",
                        (unsigned long long)timestamp,
                        (unsigned long long)currentAppsTS,
                        (unsigned long long)currentAVTime);
          m_pEncInpBufHeader_iomx[i]->filllen = m_pEncInpBufHeader_iomx[i]->alloclen;
// To be ported for HAL1 the API here is wrong
#if 0

          if (m_OMX_handle->emptyBuffer(
                m_NodeId,
                m_pEncInpBufHeader_iomx[i]->buffer_id,
                0,
                m_pEncInpBufHeader_iomx[i]->alloclen,
                0,
                timestamp) != OK) {
            ERROR1(
              "VideoTransmitter::ProcessImageFrame Failed to empty the buffer");
            return VIDEO_ERROR_UNKNOWN;
          }

#else
          OMXBuffer InputBuff(0, m_pEncInpBufHeader_iomx[i]->alloclen);
          int error = m_NodeId->emptyBuffer(
                        m_pEncInpBufHeader_iomx[i]->buffer_id,
                        InputBuff,
                        0,
                        timestamp, -1);

          if (error != OK) {
            ERROR2(
              "VideoTransmitter::ProcessImageFrame Failed to empty the buffer %d", error);
            return VIDEO_ERROR_UNKNOWN;
          }

#endif
          DEBUG1(
            "Return from VideoTransmitter::ProcessImageFrame\n");
          return VIDEO_ERROR_OK;
        } else {
          DEBUG3_STR(
            "VideoTransmitter::ProcessImageFrame :: input buffer is NULL and can not empty the buffer %d  and %p",
            (int)m_pEncInpBufHeader_iomx[i]->alloclen,
            m_pEncInpBufHeader_iomx[i]->pBuffer);
        }
      }
    }
  } else {
    MINIMAL1("VideoTransmitter::ProcessImageFrame ignored as Stream not active");
  }

  return VIDEO_ERROR_OK;
}



QPE_VIDEO_ERROR VideoTransmitter::ProcessRecordingFrame(uint64_t  timestamp,
    void *dataPtr, size_t size)
{
  status_t status;
  timeval tv;
  uint64_t  currentAppsTS;

  if (eVideoStateActive == iRecorderState) {
    if (m_codecConfig.iInputVideoBufferMode == VIDEO_BUFFER_NON_META_MODE) {
      return ProcessImageFrame(timestamp, dataPtr, size);
    }

    if (m_codecConfig.eHALVersion == CAMERA_HAL3) {
      return ProcessRecordingFrame_HAL3(timestamp, dataPtr, size);
    } else {
      uint8_t i = 0;
      native_handle_t *native_hdl = (native_handle_t *)dataPtr;

      if (native_hdl == NULL) {
        ERROR1("ProcessRecordingFrame: NATIVE_HDL is NULL hence returning");
        return VIDEO_ERROR_UNKNOWN;
      }

#ifdef GPU_ROTATION
      int fd;
      int camerasize = -1;
      unsigned int format;
      uint32_t usage;
      OMX_U8 *pbuffer = NULL;
      int countencoder = -1;

      /* Printing the size and fd when needed for debugging */
      if (native_hdl != NULL) {
        fd = native_hdl->data[0];
        camerasize = native_hdl->data[2];
        CRITICAL6_STR("ProcessrecordingFrame handle-> %p fd - %d,size - %d usage=%d format=%d",
                      native_hdl, fd, camerasize, native_hdl->data[3], native_hdl->data[5]);
        //CRITICAL2("usage is %p", usage);
      }

      int src_stride;
      int src_scanlines;
      int src_size;
      uint32_t dstusage;
      format = native_hdl->data[5];
      usage = GRALLOC_USAGE_HW_TEXTURE | native_hdl->data[3];
      dstusage = GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_VIDEO_ENCODER |
                 GRALLOC_USAGE_HW_RENDER;

      if (private_handle_t::PRIV_FLAGS_UBWC_ALIGNED & native_hdl->data[3]) {
        /*if Buffer is ubwc aligned or not*/
        src_stride = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC,
                                    m_codecConfig.cameraConfigWidth);
        src_scanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC,
                                          m_codecConfig.cameraConfigHeight);
        src_size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC,
                                     m_codecConfig.cameraConfigWidth, m_codecConfig.cameraConfigHeight);
        DEBUG1("ubwc is enabled");
      } else {
        //SW_ flags cannot be used with UBWC mode as per display team
        usage = usage | GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN |
                GRALLOC_USAGE_HW_RENDER;// | GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
        dstusage = dstusage | GRALLOC_USAGE_SW_READ_OFTEN |
                   GRALLOC_USAGE_SW_WRITE_OFTEN;
        src_stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_codecConfig.cameraConfigWidth);
        src_scanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12,
                                          m_codecConfig.cameraConfigHeight);
        src_size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, m_codecConfig.cameraConfigWidth,
                                     m_codecConfig.cameraConfigHeight);
        format = HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS;
        DEBUG1("ubwc is disabled");
      }

      CRITICAL4("Inputbuffer Src stride %d and scanline is %d and src_size=%d",
                src_stride, src_scanlines, src_size);

      if (src_size != camerasize) {
        ERROR5("Dropping Camera frame size mismatch wxh=%dx%d camerasize %d expectedsize %d",
               m_codecConfig.iWidth, m_codecConfig.iHeight, camerasize, src_size);
        return VIDEO_ERROR_OK;
      }

      DEBUG3("Inputbuffer usage %d and dst usage is %d", usage, dstusage);
      pbuffer = (OMX_U8 *)mmap(0, src_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                               0);
      int errnum = errno;

      if (pbuffer == MAP_FAILED) {
        ERROR4("Failed to mmap pmem with fd = %d, size = %d errno=%d",
               fd, (int)size, errnum);
      } else {
        MINIMAL3("Success mmap of buffer fd=%d size= %d", fd, src_size);
        private_handle_t *privateHandle = new private_handle_t(fd, size, usage,
            BUFFER_TYPE_UI,  format,
            src_stride,  src_scanlines);

        if (privateHandle != NULL) {
          DEBUG3("privateHandle buffer fd=%d size= %d", privateHandle->fd,
                 privateHandle->size);
        }

        sp<GraphicBuffer> InputBuffer = new GraphicBuffer(
          m_codecConfig.cameraConfigWidth,
          m_codecConfig.cameraConfigHeight,
          format,
          1,/*layer count*/
          usage,
          src_stride,
          (native_handle_t *)privateHandle,
          false);
        DEBUG1("Waiting for encoderbuffer_mutex");
        pthread_mutex_lock(&EncoderBuffer_mutex);
        DEBUG1("got the lock encoderbuffer_mutex");

        for (countencoder = 0; countencoder < EncoderBufferSize; countencoder++) {
          if (EncoderNativeBuffer[countencoder].UsedByIOMX == QP_FALSE) {
            if (EncoderNativeBuffer[countencoder].outBuffer == NULL) {
              //allocate buffer if this is new slot we are using.
              //normally we should be end us using max 4-5 at one time together
              CRITICAL2("ImsCamera GPU outBuffer Allocation %d Firsttime", countencoder);
              EncoderNativeBuffer[countencoder].outBuffer  = new GraphicBuffer(
                m_codecConfig.iWidth,
                m_codecConfig.iHeight,
                format,
                dstusage);

              if (EncoderNativeBuffer[countencoder].outBuffer.get() != NULL) {
                EncoderNativeBuffer[countencoder].nh = native_handle_create(1, 6);
                EncoderNativeBuffer[countencoder].src = (private_handle_t *)(
                    EncoderNativeBuffer[countencoder].outBuffer->getNativeBuffer()->handle);

                if (!EncoderNativeBuffer[countencoder].nh) {
                  ERROR1("Error in getting video native handle");
                } else {
                  int msize;

                  if (private_handle_t::PRIV_FLAGS_UBWC_ALIGNED & native_hdl->data[3]) {
                    msize = VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC,
                                              m_codecConfig.iWidth,
                                              m_codecConfig.iHeight);
                    DEBUG2("outBuffer ubwc is enabled and size is %d", msize);
                  } else {
                    msize = VENUS_BUFFER_SIZE(COLOR_FMT_NV12,
                                              EncoderNativeBuffer[countencoder].outBuffer->getWidth(),
                                              EncoderNativeBuffer[countencoder].outBuffer->getHeight());
                    DEBUG2("outBuffer ubwc is disabled and size is %d", msize);
                  }

                  EncoderNativeBuffer[countencoder].nh->data[0] =
                    EncoderNativeBuffer[countencoder].src->fd;
                  EncoderNativeBuffer[countencoder].nh->data[1] = 0;
                  EncoderNativeBuffer[countencoder].nh->data[2] = msize;
                  EncoderNativeBuffer[countencoder].nh->data[3] = native_hdl->data[3];
                  EncoderNativeBuffer[countencoder].nh->data[4] =
                    0; //dummy value for timestamp in non-batch mode
                  EncoderNativeBuffer[countencoder].nh->data[5] = native_hdl->data[5];
                  EncoderNativeBuffer[countencoder].nh->data[6] = countencoder;
                }
              } else {
                ERROR1("GPU Rotation outbuffer is null");
              }
            }

            if (EncoderNativeBuffer[countencoder].outBuffer.get() == NULL) {
              CRITICAL2(" Allocation failed for outBuffer =%d", countencoder);
            } else {
              DEBUG2("Allocation set for outBuffer =%d", countencoder);
              EncoderNativeBuffer[countencoder].UsedByIOMX = QP_TRUE;
              DEBUG2("private_handle_t fd %d", EncoderNativeBuffer[countencoder].src->fd);
              //RotationBuffer[countencoder].CameraFence = Consumeritem->mFence;
            }

            break;
          }
        }

        pthread_mutex_unlock(&EncoderBuffer_mutex);

        if (InputBuffer.get() != NULL
            && EncoderNativeBuffer[countencoder].outBuffer.get() != NULL) {
          rotateFrame(InputBuffer->getWidth(), InputBuffer->getHeight(),
                      m_codecConfig.iHeight,
                      m_codecConfig.iWidth,
                      m_codecConfig.rotation,
                      InputBuffer->getNativeBuffer(),
                      EncoderNativeBuffer[countencoder].outBuffer->getNativeBuffer());
        } else {
          ERROR1("Cannot do GPU Rotation because of Graphic Buffer failed")
        }

        /*if (pbuffer != NULL) {
             void *buf;

          if (EncoderNativeBuffer[countencoder].outBuffer->lock((GRALLOC_USAGE_SW_READ_MASK | GRALLOC_USAGE_SW_WRITE_MASK), &buf) == 0) {
             //smittal yuv
            CRITICAL1("buffer inside lockgraphicbuffer");
             if (buf) {
              int width = EncoderNativeBuffer[countencoder].outBuffer->getWidth();
              int height = EncoderNativeBuffer[countencoder].outBuffer->getHeight();
              //create_camera_yuv_dump(width, height, buf);
             }
             buf = NULL;
            EncoderNativeBuffer[countencoder].outBuffer->unlock();
          }
        }*/
        if (privateHandle) {
          delete privateHandle;
        }

        if (pbuffer != NULL) {
          MINIMAL3("UNMAP Memory pBuffer %p size %d", pbuffer, native_hdl->data[2]);
          munmap(pbuffer, native_hdl->data[2]);
        }

        if (InputBuffer != NULL) {
          InputBuffer = NULL;
        }
      }

#endif

      for (; i < m_iMinInputBufCount; i++) {
        DEBUG2("VideoTransmitter::ProcessRecordingFrame:; For Loop %d", i);

        /*
                     if( size > m_pEncInpBufHeader_iomx[i]->alloclen )
                    {
          ERROR3("VideoTransmitter::ProcessRecordingFrame:size %d exceeds allocated length - %d hence returning error_unknown",size,m_pEncInpBufHeader_iomx[i]->alloclen);
          return VIDEO_ERROR_UNKNOWN;
        }
        */
        if (m_pEncInpBufHeader_iomx[i] == NULL) {
          MINIMAL3("m_pEncInpBufHeader_iomx[i] is NULL i : %d",
                   m_pEncOutpBufHeader_iomx[i], i);
          return VIDEO_ERROR_UNKNOWN;
        }

        if (m_pEncOutpBufHeader_iomx[i] == NULL) {
          MINIMAL2("m_pEncOutpBufHeader_iomx[i] is NULL i : %d", i);
          return VIDEO_ERROR_UNKNOWN;
        }

        if (m_pEncInpBufHeader_iomx[i]->filllen == 0) { //reuse the buffer which is free
          DEBUG3_STR(
            "VideoTransmitter::ProcessRecordingFrame alloclen: %d m_pEncInpBufHeader_iomx[i]->pBuffer = %p",
            (int)m_pEncInpBufHeader_iomx[i]->alloclen,
            m_pEncInpBufHeader_iomx[i]->pBuffer);

          if (m_pEncInpBufHeader_iomx[i]->pBuffer) {
            char *destbuffer =
              (char *)m_pEncInpBufHeader_iomx[i]->pBuffer;
            char *srcbuffer = (char *)dataPtr;

            if (m_codecConfig.iInputVideoBufferMode == VIDEO_BUFFER_META_MODE) {
#ifdef GPU_ROTATION
              EncoderNativeBuffer[countencoder].bufferid =
                m_pEncInpBufHeader_iomx[i]->buffer_id;
#endif
#if 0 // API is deprecated the buffer needs to be passed in emptyBuffer

              /* API to update the native handle to OMX component for every input buffer index */
              if (m_OMX_handle->updateNativeHandleInMeta(
                    m_NodeId, i,
#ifdef GPU_ROTATION
                    (EncoderNativeBuffer[countencoder].nh != NULL) ? NativeHandle::create(
                      EncoderNativeBuffer[countencoder].nh, false /* ownsHandle */) :
                    NativeHandle::create(native_hdl, false /* ownsHandle */), //native_hdl,
#else
                    NativeHandle::create(native_hdl, false /* ownsHandle */), //native_hdl,
#endif
                    m_pEncInpBufHeader_iomx[i]->buffer_id
                  ) != OK) {
                CRITICAL1("VideoTransmitter::updateNativeHandleInMeta returned Non-OK hence  returning");
#ifdef GPU_ROTATION
                pthread_mutex_lock(&EncoderBuffer_mutex);
                DEBUG1("got the lock encoderbuffer_mutex");
                EncoderNativeBuffer[countencoder].UsedByIOMX = QP_FALSE;
                pthread_mutex_unlock(&EncoderBuffer_mutex);
#endif
                return VIDEO_ERROR_OK;
              }

#endif
            } else if ( m_codecConfig.iInputVideoBufferMode ==
                        VIDEO_BUFFER_NON_META_MODE ) {
              DEBUG2("VideoTransmitter::Copying to destbuffer of size - %d", (int)size);

              if ( srcbuffer && destbuffer ) {
                memcpy(destbuffer, srcbuffer, size);
              } else {
                CRITICAL3_STR("VideoTransmitter::destbuffer - %p srcbuffer - %p ",
                              destbuffer, srcbuffer);
                return VIDEO_ERROR_OK;
              }
            } else {
              CRITICAL2("VideoTransmitter::Invalid mode %d",
                        m_codecConfig.iInputVideoBufferMode);
              return VIDEO_ERROR_OK;
            }

            timeval tv;
            gettimeofday(&tv, NULL);
            uint64_t currentAppsTS = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
            uint64_t currentAVTime = 0;

            for (uint j = 0; j < m_iMinOutputBufCount; j++) {
              if (m_pEncOutpBufHeader_iomx[j] && !m_pEncOutpBufHeader_iomx[j]->usedbyIOMX) {
                m_pEncOutpBufHeader_iomx[j]->usedbyIOMX = QP_TRUE;
                CRITICAL4_STR("Profiling IMS->Encoder FillBuffer on %p j = %d appstime=%llu",
                              m_pEncOutpBufHeader_iomx[j], j,
                              (unsigned long long)currentAppsTS);

                if (m_NodeId->fillBuffer(m_pEncOutpBufHeader_iomx[j]->buffer_id,
                                         OMXBuffer::sPreset, m_pEncOutpBufHeader_iomx[j]->fencefd) != OK) {
                  ERROR1(
                    "VideoTransmitter::pushFramesFromCameraCB Failed to fill the buffer");
                  return VIDEO_ERROR_UNKNOWN;
                }
              }
            }

            DEBUG3_STR(
              "VideoTransmitter::ProcessRecordingFrame:: Before emptying the buffer with bufflen = %lu and pointer = %p",
              (unsigned long)m_pEncInpBufHeader_iomx[i]->alloclen,
              m_pEncInpBufHeader_iomx[i]->pBuffer);
            CRITICAL6_STR("Profiling IMS-> ENCODER EmptyBuffer = %d,localtime= %llu buffid = %d index = %d,len=%lu",
                          tempemptybuffercallbackcountenc + 1,
                          (unsigned long long)currentAppsTS,
                          m_pEncInpBufHeader_iomx[i]->buffer_id, i,
                          (unsigned long)m_pEncInpBufHeader_iomx[i]->alloclen);

            //timestamp = timestamp * 1000;
            if (getModemTimeProperty() == 1 || m_codecConfig.useSystemTime) {
              AddCameraTSMap(timestamp, getModemTime() + (currentAppsTS - getAppsTime()),
                             currentAppsTS);
            } else if (getAVTimerNode() < 0) {
              AddCameraTSMap(timestamp, timestamp / 1000, currentAppsTS);
            } else {
              currentAVTime = getCurrentAVTime();
              AddCameraTSMap(timestamp, timestamp / 1000,
                             currentAppsTS - ((currentAVTime - timestamp) / 1000));
            }

            CRITICAL4_STR("Profiling IMS->ENCODER ETB ts : %llu appstime = %llu currentAVTime %llu",
                          (unsigned long long)timestamp,
                          (unsigned long long)currentAppsTS,
                          (unsigned long long)currentAVTime);
            m_pEncInpBufHeader_iomx[i]->filllen = m_pEncInpBufHeader_iomx[i]->alloclen;
#ifdef GPU_ROTATION

            if (EncoderNativeBuffer[countencoder].nh != NULL) {
              OMXBuffer InputBuff(NativeHandle::create(
                                    EncoderNativeBuffer[countencoder].nh, false /* ownsHandle */));

              if (m_NodeId->emptyBuffer(
                    m_pEncInpBufHeader_iomx[i]->buffer_id,
                    InputBuff,
                    0,
                    timestamp, -1) != OK) {
                ERROR1(
                  "VideoTransmitter::ProcessRecordingFrame Failed to empty the buffer");
                return VIDEO_ERROR_UNKNOWN;
              }
            } else {
              OMXBuffer InputBuff(NativeHandle::create(native_hdl,
                                  false /* ownsHandle */)); //native_hdl,

              if (m_NodeId->emptyBuffer(
                    m_pEncInpBufHeader_iomx[i]->buffer_id,
                    InputBuff,
                    0,
                    timestamp, -1) != OK) {
                ERROR1(
                  "VideoTransmitter::ProcessRecordingFrame Failed to empty the buffer");
                return VIDEO_ERROR_UNKNOWN;
              }
            }

#else
            OMXBuffer InputBuff(NativeHandle::create(native_hdl,
                                false /* ownsHandle */)); //native_hdl,

            if (m_NodeId->emptyBuffer(
                  m_pEncInpBufHeader_iomx[i]->buffer_id,
                  InputBuff,
                  0,
                  timestamp, -1) != OK) {
              ERROR1(
                "VideoTransmitter::ProcessRecordingFrame Failed to empty the buffer");
              return VIDEO_ERROR_UNKNOWN;
            }

#endif
            DEBUG1("Return from VideoTransmitter::ProcessRecordingFrame");
            return VIDEO_ERROR_OK;
          } else {
            DEBUG3_STR(
              "VideoTransmitter::ProcessRecordingFrame :: input buffer is NULL and can not empty the buffer %lu and %p",
              (unsigned long)m_pEncInpBufHeader_iomx[i]->alloclen,
              m_pEncInpBufHeader_iomx[i]->pBuffer);
          }
        }
      }
    }//HAL1
  } else {
    MINIMAL1("VideoTransmitter::ProcessRecordingFrame ignored as Stream not active");
  }

  return VIDEO_ERROR_OK;
}


/************************************************************************************************
 Configure common codec parameters for OMX encoder
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::EncodeConfigure(QP_VIDEO_CONFIG tCodecConfig)
{
  TRACE1("VideoTransmitter::EncodeConfigure");
  status_t err = OMX_ErrorNone;
  frameRate = tCodecConfig.iFrameRate;
  m_bLTRFeatureOn = tCodecConfig.iLTRSupportEnabled;
  m_ulFrameCount = 0;
  CRITICAL4("VideoTransmitter::EncodeConfigure:: bitrate = %d, iFrameRate = %d, Tx-LTR support flag %d",
            tCodecConfig.iBitRate, tCodecConfig.iFrameRate, m_bLTRFeatureOn);

  //! If LTR Support is diabled by RTP, then mark the flag to FALSE
  if (!tCodecConfig.iLTRSupportEnabled) {
    CRITICAL1("Tx-LTR: LTR support disabled from RTP");
    m_bLTRFeatureOn = QP_FALSE;
  }

  //Set the meta data mode enable
  /*if (videoInjection[0] != '1') {
     StoreMetaDataInBuffersParams sMetadataMode;
     //OMX_INIT_STRUCT(&sMetadataMode, StoreMetaDataInBuffersParams);
     memset(&sMetadataMode, 0, sizeof(StoreMetaDataInBuffersParams));
     InitEncoderOMXParams(&sMetadataMode);

     sMetadataMode.nPortIndex = (OMX_U32)DIR_INP;
     sMetadataMode.bStoreMetaData = OMX_TRUE;

     CRITICAL1("VideoTransmitter::EncodeConfigure setting meta mode");
     err = m_OMX_handle->setParameter(m_NodeId, (OMX_INDEXTYPE)OMX_QcomIndexParamVideoMetaBufferMode,
                                    (OMX_PTR *)&sMetadataMode, sizeof(StoreMetaDataInBuffersParams));
     if (err != OK) {
        ERROR2(
           "VideoTransmitter::EncodeConfigure :: Failed to set the parameter for meta data mode %d",err);
        //return VIDEO_ERROR_UNKNOWN;
     }
     m_Metamode = QP_TRUE;
  }*/

  if ( (m_codecConfig.eHALVersion == CAMERA_HAL1)  &&
       m_codecConfig.iInputVideoBufferMode != VIDEO_BUFFER_NON_META_MODE ) {
    MetadataBufferType mInputMetadataType;
    CRITICAL1("VideoTransmitter::EncodeConfigure setting meta mode using nativehandlesource");
    /* This API is used  to invoke the meta data mode to enable the native handle */
    mInputMetadataType =   kMetadataBufferTypeNativeHandleSource;
#if 0 // API deprecated in O

    if (m_OMX_handle->storeMetaDataInBuffers(m_NodeId, DIR_INP, OMX_TRUE,
        &mInputMetadataType) != OK ) {
      ERROR1("VideoTransmitter :: EncodeConfigure : Failed for api : storeMetaDataInBuffers");
      return VIDEO_ERROR_UNKNOWN;
    }

#endif
  }

  /*
   Configure encoder Output port params
   */

  if (ConfigureCommonEncoderInputPort(tCodecConfig) != VIDEO_ERROR_OK) {
    ERROR1(
      "VideoTransmitter::EncodeConfigure::Failure in Configuring the INPORT parameter");
    return VIDEO_ERROR_UNKNOWN;
  }

  /*
   Configure encoder Output port params
   */
  if (ConfigureCommonEncoderOutputPort(tCodecConfig) != VIDEO_ERROR_OK) {
    ERROR1(
      "VideoTransmitter::EncodeConfigure::Failure in Configuring the OUT PORT parameter");
    return VIDEO_ERROR_UNKNOWN;
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_H263) {
    if (ConfigureH263EncoderPort( OMX_DirInput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring H263 codec specific inp parameters");
      return VIDEO_ERROR_UNKNOWN;
    }

    // Configure codec specificparams for Out ports
    if (ConfigureH263EncoderPort( OMX_DirOutput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring H263 codec specific out parameters");
      return VIDEO_ERROR_UNKNOWN;
    }
  } else if (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
             || tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
    if (ConfigureMPEG4EncoderPort( OMX_DirInput,
                                   tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring MPEG4 codec specific inp parameters");
      return VIDEO_ERROR_UNKNOWN;
    }

    // Configure codec specificparams for Out ports
    if (ConfigureMPEG4EncoderPort(OMX_DirOutput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring MPEG4 codec specific out parameters");
      return VIDEO_ERROR_UNKNOWN;
    }
  } else if (tCodecConfig.eCodec == VIDEO_CODEC_H264) {
    if (ConfigureH264EncoderPort( OMX_DirInput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring H264 codec specific inp parameters");
      return VIDEO_ERROR_UNKNOWN;
    }

    // Configure codec specificparams for Out ports
    if (ConfigureH264EncoderPort( OMX_DirOutput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring H264 codec specific out parameters");
      return VIDEO_ERROR_UNKNOWN;
    }
  } else if (tCodecConfig.eCodec == VIDEO_CODEC_H265) {
    if (ConfigureH265EncoderPort( OMX_DirInput,
                                  tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1("VideoTransmitter::Configure:: Failure in configuring H264 codec specific inp parameters");
      return VIDEO_ERROR_UNKNOWN;;
    }

    // Configure codec specificparams for Out ports
    if (ConfigureH265EncoderPort(OMX_DirOutput,
                                 tCodecConfig) != VIDEO_ERROR_OK) {
      ERROR1(
        "VideoTransmitter::Configure:: Failure in configuring H264 codec specific out parameters");
      return VIDEO_ERROR_UNKNOWN;;
    }
  }

  OMX_PARAM_COMPONENTROLETYPE params;
  memset(&params, 0, sizeof(OMX_PARAM_COMPONENTROLETYPE));
  params.nSize = sizeof(params);
  params.nVersion.s.nVersionMajor = 1;
  params.nVersion.s.nVersionMinor = 0;
  params.nVersion.s.nRevision = 0;
  params.nVersion.s.nStep = 0;
  InitEncoderOMXParams(&params);

  if (tCodecConfig.eCodec == VIDEO_CODEC_H264) strlcpy((char *)params.cRole,
        "video_encoder.avc",
        OMX_MAX_STRINGNAME_SIZE - 1);
  else if (tCodecConfig.eCodec == VIDEO_CODEC_H263) strlcpy((char *)params.cRole,
        "video_encoder.h263",
        OMX_MAX_STRINGNAME_SIZE - 1);
  else if ((tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO)
           || (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID)) strlcpy((
                   char *)params.cRole, "video_encoder.mpeg4",
                 OMX_MAX_STRINGNAME_SIZE - 1);
  else if (tCodecConfig.eCodec == VIDEO_CODEC_H265) strlcpy((char *)params.cRole,
        "video_encoder.hevc",
        OMX_MAX_STRINGNAME_SIZE - 1);

  params.cRole[OMX_MAX_STRINGNAME_SIZE - 1] = '\0';
  err = m_NodeId->setParameter(OMX_IndexParamStandardComponentRole,
                               &params, sizeof(params));

  if (err != OK) {
    ERROR2("VideoTransmitter::setParam OMX_IndexParamStandardComponentRole with return value %x",
           err);
  }

  //security fixes this has to be moved before sending command to idle
  //separated input and output port as parameter has to be set before sending command idle
  OMX_PARAM_PORTDEFINITIONTYPE InputportFmt;
  OMX_PARAM_PORTDEFINITIONTYPE portFmt;
  memset(&InputportFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitEncoderOMXParams(&InputportFmt);
  InputportFmt.nPortIndex = DIR_INP;
  memset(&portFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitEncoderOMXParams(&portFmt);
  portFmt.nPortIndex = DIR_OUT;
  //security fixes this has to be moved before sending command to idle
  PrependSPSPPSToIDRFramesParams param;
  InitEncoderOMXParams(&param);
  memset(&param, 0, sizeof(PrependSPSPPSToIDRFramesParams));
  param.nSize = sizeof(PrependSPSPPSToIDRFramesParams);
  //param.bEnable = pConfig->bInsertInbandVideoHeaders;
  param.bEnable = OMX_TRUE;
  CRITICAL2("Set SPS/PPS headers: %d", param.bEnable);
  status_t result = m_NodeId->setParameter(
                      (OMX_INDEXTYPE)OMX_QcomIndexParamSequenceHeaderWithIDR,
                      (OMX_PTR *)&param, sizeof(PrependSPSPPSToIDRFramesParams));

  if (result != OK) {
    ERROR2("VideoTransmitter::setParam OMX_QcomIndexParamSequenceHeaderWithIDR with return value %x",
           result);
  }

  if (m_codecConfig.iInputVideoBufferMode != VIDEO_BUFFER_NON_META_MODE) {
    if (m_codecConfig.eHALVersion == CAMERA_HAL3) {
      IOMX::PortMode mode;
      mode = IOMX::kPortModeDynamicANWBuffer;
      err = m_NodeId->setPortMode(InputportFmt.nPortIndex, mode);

      if (err != OK) {
        ERROR2("VideoTransmitter::setPortMode w/ err %d", err);
      } else {
        MINIMAL1("VideoTransmitter::setPortMode setport mode kPortModeDynamicANWBuffer is successful");
      }
    }

    if (m_codecConfig.eHALVersion == CAMERA_HAL1) {
      IOMX::PortMode mode;
      mode = IOMX::kPortModeDynamicNativeHandle;
      err = m_NodeId->setPortMode(InputportFmt.nPortIndex, mode);

      if (err != OK) {
        ERROR2("VideoTransmitter::setPortMode w/ err %d", err);
      } else {
        MINIMAL1("VideoTransmitter::setPortMode setport mode kPortModeDynamicNativeHandle is successful");
      }
    }
  }

#if 0 //depracated
  err = m_NodeId->setQuirks(1);

  if (err != OK) {
    ERROR2("VideoTransmitter::setQuirks w/ err %d", err);
    //return err;
  } else {
    MINIMAL1("VideoTransmitter::setQuirks is successful");
  }

#endif

//if (m_codecConfig.eHALVersion == CAMERA_HAL1) {
  //security fixes this has to be moved before sending command to idle
  //and this is failing as they handle it internally, we need to check if
  //this is needed for handling meta buffer mode in HAL1 implementation
  //Set the meta data mode enable
  if (m_codecConfig.iInputVideoBufferMode != VIDEO_BUFFER_NON_META_MODE) {
    StoreMetaDataInBuffersParams sMetadataMode;
    //OMX_INIT_STRUCT(&sMetadataMode, StoreMetaDataInBuffersParams);
    memset(&sMetadataMode, 0, sizeof(StoreMetaDataInBuffersParams));
    InitEncoderOMXParams(&sMetadataMode);
    sMetadataMode.nPortIndex = (OMX_U32)DIR_INP;
    sMetadataMode.bStoreMetaData = OMX_TRUE;
    CRITICAL1("VideoTransmitter::EncodeConfigure setting meta mode");
    err = m_NodeId->setParameter((OMX_INDEXTYPE)
                                 OMX_QcomIndexParamVideoMetaBufferMode,
                                 (OMX_PTR *)&sMetadataMode, sizeof(StoreMetaDataInBuffersParams));

    if (err != OK) {
      ERROR2(
        "VideoTransmitter::EncodeConfigure :: Failed to set the parameter for meta data mode %d",
        err);
      //return VIDEO_ERROR_UNKNOWN;
    }

    m_Metamode = QP_TRUE;
  }

//}
  //security fixes this has to be moved before sending command to idle

  if ( (m_codecConfig.eHALVersion != CAMERA_HAL1) &&
       ( m_codecConfig.iInputVideoBufferMode != VIDEO_BUFFER_NON_META_MODE ) ) {
    android_dataspace dataSpace = HAL_DATASPACE_UNKNOWN;

    //Cleanup the Old InputBufferProducer
    if (InputbufferProducer != NULL) {
      InputbufferProducer->disconnect(NATIVE_WINDOW_API_MEDIA);
      InputbufferProducer = NULL;
    }

    err = m_OMX_handle->createInputSurface( &InputbufferProducer,
                                            &mGraphicBufferSource);
    MINIMAL2("VideoTransmitter::EncodeConfigure createInputSurface is successful %d",
             err);

    if (err != OK) {
      ERROR2(
        "VideoTransmitter::createInputSurface failed: %d\n",
        err);
      return VIDEO_ERROR_UNKNOWN;
    }

    mGraphicBufferSource->configure(m_NodeId, dataSpace);
    mGraphicBufferSource->setMaxFps(30);

    if (InputbufferProducer != NULL) {
      CRITICAL1("InputbufferProducer created succesfully");
      //InputbufferProducer->setBufferCount(11);
      IGraphicBufferProducer::QueueBufferOutput queueBufferOutput;
      sp<InputBufferListener> listener(new InputBufferListener());
      InputbufferProducer->connect(listener, NATIVE_WINDOW_API_MEDIA, true,
                                   &queueBufferOutput);
    }
  }

  //Extra data
  if (m_bLTRFeatureOn) {
    QOMX_INDEXEXTRADATATYPE sExtraData;
    OMX_INDEXTYPE eExtaDataIndex = (OMX_INDEXTYPE)
                                   OMX_QcomIndexParamIndexExtraDataType;
    InitEncoderOMXParams(&sExtraData);
    sExtraData.bEnabled = OMX_TRUE;
    sExtraData.nIndex = (OMX_INDEXTYPE)OMX_ExtraDataVideoLTRInfo;
    sExtraData.nPortIndex = (OMX_U32)OMX_DirOutput;
    CRITICAL1("Tx-LTR: Extra Data enable for Encoder");
    status_t omxresult = m_NodeId->setParameter(eExtaDataIndex,
                         &sExtraData,
                         sizeof(sExtraData));

    if (omxresult != OK) {
      ERROR3("TX-LTR::ConfigureEncoder::ERROR - Failed to set extra data Param %d, err code %s",
             omxresult, strerror(omxresult));
      // m_bLTRFeatureOn = false;
    }
  } //!if (m_bLTRFeatureOn)

  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);

  if (m_NodeId->sendCommand(OMX_CommandStateSet, OMX_StateIdle)
      != VIDEO_ERROR_OK) {
    ERROR1(
      "VideoReceiver::ConfigureDecoder::::Failure in insrting the command for state IDLE\n");
    return VIDEO_ERROR_UNKNOWN;
  }

  DLOG("VideoTransmitter::getParameter OMX_IndexParamPortDefinition nPortIndex %d",
       InputportFmt.nPortIndex);
  //security fixes this has to be after sendcommand idle as port definition has to be retrived before
  //allocation of buffers
  err = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                               (OMX_PTR *)&InputportFmt, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OMX_DirInput != InputportFmt.eDir) {
    ERROR1("VideoTransmitter::Configure::Dec: Expect Input Port");
    return VIDEO_ERROR_UNKNOWN;
  }

  if (InputportFmt.nBufferCountMin < VT_ACTUAL_BUFF_COUNT) {
    InputportFmt.nBufferCountActual = VT_ACTUAL_BUFF_COUNT;
  }

  m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                         (OMX_PTR *)&InputportFmt, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OMX_DirInput != InputportFmt.eDir) {
    ERROR1("VideoTransmitter::Configure::Dec: Expect Input Port");
    return VIDEO_ERROR_UNKNOWN;
  }

  //sp < IMemory > mem;
  size_t totalSize = 0;
  m_iMinInputBufCount = InputportFmt.nBufferCountActual;

  if (!isTrebleEnabled) {
    if (tCodecConfig.iInternalCodecConfig == QP_FALSE) {
      totalSize = InputportFmt.nBufferCountActual * InputportFmt.nBufferSize;
      m_mDealer[InputportFmt.nPortIndex] = new MemoryDealer(totalSize,
          "VideoTransmitter");
      CRITICAL2(
        "VideoTransmitter::Configure::allocating input Memory Dealer totalsize - %lu",
        (unsigned long)totalSize);
    } else {
      CRITICAL2(
        "VideoTransmitter::Configure::already input allocated Memory Dealer totalsize - %lu",
        (unsigned long)totalSize);
    }

    for (uint8_t i = 0; i < InputportFmt.nBufferCountActual; i++) {
      if (Inputmem[i].get() == NULL
          || tCodecConfig.iInternalCodecConfig == QP_FALSE) {
        m_pEncInpBufHeader_iomx[i] = (VideoDecBufferInfo *)malloc(sizeof(
                                       VideoDecBufferInfo));
        TRACE1("VideoTransmitter::Configure::Before allocating the buffer for inp port\n");
        Inputmem[i] = m_mDealer[InputportFmt.nPortIndex]->allocate(
                        InputportFmt.nBufferSize);

        if (Inputmem[i].get() == NULL) {
          return VIDEO_ERROR_UNKNOWN;
        }
      } else {
        CRITICAL1(
          "VideoTransmitter::Configure::reusing allocating the buffer for inp port\n");
      }

//    if (videoInjection[0] == '1') {
      err = m_NodeId->useBuffer(InputportFmt.nPortIndex, Inputmem[i],
                                &(m_pEncInpBufHeader_iomx[i]->buffer_id));
      /*     } else {
            err = m_OMX_handle->allocateBufferWithBackup(m_NodeId,
                  InputportFmt.nPortIndex, Inputmem[i],
                  &(m_pEncInpBufHeader_iomx[i]->buffer_id), Inputmem[i]->size());
          }*/

      if (err != OK) {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: error in Allocate Buffer for inp ports ");
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_pEncInpBufHeader_iomx[i]) {
        m_pEncInpBufHeader_iomx[i]->filllen = 0;
        m_pEncInpBufHeader_iomx[i]->alloclen = InputportFmt.nBufferSize;
      } else {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: m_pEncInpBufHeader is NULL and not initialized");
      }

      //uses as use buffer
      if (Inputmem[i] != NULL) {
        m_pEncInpBufHeader_iomx[i]->pBuffer = Inputmem[i]->pointer();
        m_pEncInpBufHeader_iomx[i]->offset = Inputmem[i]->offset();
        DEBUG2("m_pEncOutpBufHeader_iomx[i]->pBuffer %p\n",
               m_pEncInpBufHeader_iomx[i]->pBuffer);
      }
    }
  } else {
    hidl_memory hidlMemoryToken;
    sp<TMemory> hidlMemory;
    bool success;
    mAllocator[InputportFmt.nPortIndex] = IAllocator::getService("ashmem");

    if (mAllocator[InputportFmt.nPortIndex] == NULL) {
      ERROR2("VideoTransmitter::EncodeConfigure hidl allocator on port %d is null",
             (int)InputportFmt.nPortIndex);
      return VIDEO_ERROR_UNKNOWN;
    }

    for (uint8_t i = 0; i < InputportFmt.nBufferCountActual; i++) {
      m_pEncInpBufHeader_iomx[i] = (VideoDecBufferInfo *)malloc(sizeof(
                                     VideoDecBufferInfo));
      auto transStatus = mAllocator[InputportFmt.nPortIndex]->allocate(
                           InputportFmt.nBufferSize,
                           [&success, &hidlMemoryToken](
                             bool s,
      hidl_memory const & m) {
        success = s;
        hidlMemoryToken = m;
      });

      if (!transStatus.isOk()) {
        ERROR2("VideoTransmitter::EncodeConfigure AshmemAllocator failed at the "
               "transport: %s",
               transStatus.description().c_str());
        return VIDEO_ERROR_UNKNOWN;
      }

      if (!success) {
        return VIDEO_ERROR_UNKNOWN;
      }

      //InputHidlmem[i] = hidlMem;
      hidlMemory = mapMemory(hidlMemoryToken);
      InputHidlmem[i] = hidlMemory;
      err = m_NodeId->useBuffer(
              InputportFmt.nPortIndex, hidlMemoryToken,
              &(m_pEncInpBufHeader_iomx[i]->buffer_id));

      if (err != OK) {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: error in Allocate Buffer for inp ports ");
        return VIDEO_ERROR_UNKNOWN;
      }

      CRITICAL2("useBuffer Done Creating SharedMemoryBuffer nBufferSize %d",
                InputportFmt.nBufferSize);

      if (m_pEncInpBufHeader_iomx[i]) {
        m_pEncInpBufHeader_iomx[i]->filllen = 0;
        m_pEncInpBufHeader_iomx[i]->alloclen = InputportFmt.nBufferSize;
      } else {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: m_pEncInpBufHeader is NULL and not initialized");
      }

      //uses as use buffer
      if (InputHidlmem[i] != NULL) {
        m_pEncInpBufHeader_iomx[i]->pBuffer = (void*)InputHidlmem[i]->getPointer();
        m_pEncInpBufHeader_iomx[i]->offset = 0;
        DEBUG3("m_pEncOutpBufHeader_iomx[%d]->pBuffer %p\n", i,
               m_pEncInpBufHeader_iomx[i]->pBuffer);
      }
    }
  }

  //output buffer parameter started
  portFmt.nPortIndex = DIR_OUT;
  DLOG("VideoTransmitter::getParameter OMX_IndexParamPortDefinition nPortIndex %d",
       portFmt.nPortIndex);
  err = m_NodeId->getParameter(OMX_IndexParamPortDefinition, \
                               (OMX_PTR *)&portFmt, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  CRITICAL5("VideoTransmitter::getParam OMX_IndexParamPortDefinition with return value %x eDir %d nBufferCountActual %d nBufferSize %d",
            err, portFmt.eDir, portFmt.nBufferCountActual, portFmt.nBufferSize);

  if (OMX_DirOutput != portFmt.eDir) {
    ERROR1("VideoTransmitter::Configure::Dec: Expect Output Port retuning VIDEO_ERROR_UNKNOWN");
    return VIDEO_ERROR_UNKNOWN;
  }

#ifndef MSM8909

  if (portFmt.nBufferCountMin < VT_ACTUAL_BUFF_COUNT) {
    portFmt.nBufferCountActual = VT_ACTUAL_BUFF_COUNT;
  }

#endif

  if (!isTrebleEnabled) {
    if (tCodecConfig.iInternalCodecConfig != QP_FALSE) {
      portFmt.nBufferSize = OutputPortBufferSize;
    }
  }

  m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                         (OMX_PTR *)&portFmt, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OMX_DirOutput != portFmt.eDir) {
    ERROR1("VideoTransmitter::Configure::Dec: Expect Output Port returning VIDEO_ERROR_UNKNOWN");
    return VIDEO_ERROR_UNKNOWN;
  }

  m_iMinOutputBufCount = portFmt.nBufferCountActual;
  CRITICAL3(" VideoTransmitter :: configure m_iMinOutputBufCount : %d m_iMinInputBufCount : %d",
            m_iMinOutputBufCount, m_iMinInputBufCount);

  //output port buffer size
  /*  if (m_mDealer[portFmt.nPortIndex] == NULL
        || tCodecConfig.iInternalCodecConfig == QP_FALSE) {*/
  if (!isTrebleEnabled) {
    totalSize = portFmt.nBufferCountActual * portFmt.nBufferSize;
    OutputPortBufferSize = portFmt.nBufferSize;
    m_mDealer[portFmt.nPortIndex] = new MemoryDealer(totalSize,
        "VideoTransmitter");
    CRITICAL1(
      "VideoTransmitter::Configure::allocating output Memory Dealer\n");
    /*  } else {
        CRITICAL1(
          "VideoTransmitter::Configure::already output allocated Memory Dealer\n");
      }*/

    for (uint8_t i = 0; i < portFmt.nBufferCountActual; i++) {
      if (Outputmem[i].get() == NULL
          || tCodecConfig.iInternalCodecConfig == QP_FALSE) {
        m_pEncOutpBufHeader_iomx[i] = (VideoDecBufferInfo *)malloc(sizeof(
                                        VideoDecBufferInfo));
        TRACE1(
          "VideoTransmitter::Configure::allocating the buffer for output port\n");
        Outputmem[i] = m_mDealer[portFmt.nPortIndex]->allocate(portFmt.nBufferSize);
        m_pEncOutpBufHeader_iomx[i]->fencefd = -1;

        if (Outputmem[i].get() == NULL) {
          return VIDEO_ERROR_UNKNOWN;
        }
      } else {
        CRITICAL1(
          "VideoTransmitter::Configure::reusing allocating the buffer for output port\n");
      }

      err = m_NodeId->useBuffer(
              portFmt.nPortIndex, Outputmem[i],
              &(m_pEncOutpBufHeader_iomx[i]->buffer_id));

      if (err != OK) {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: error  in Allocate Buffer for out ports");
        return VIDEO_ERROR_UNKNOWN;
      }

      //uses as use buffer
      if (Outputmem[i] != NULL) {
        m_pEncOutpBufHeader_iomx[i]->pBuffer = Outputmem[i]->pointer();
        m_pEncOutpBufHeader_iomx[i]->offset = Outputmem[i]->offset();
        m_pEncOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
        DEBUG2("m_pEncOutpBufHeader_iomx[i]->pBuffer %p\n",
               m_pEncOutpBufHeader_iomx[i]->pBuffer);
      }
    }
  } else {
    mAllocator[portFmt.nPortIndex] = TAllocator::getService("ashmem");

    if (mAllocator[portFmt.nPortIndex] == nullptr) {
      ERROR2("VideoTransmitter::EncodeConfigure hidl allocator on port %d is null",
             (int)portFmt.nPortIndex);
      return VIDEO_ERROR_UNKNOWN;
    }

    for (uint8_t i = 0; i < portFmt.nBufferCountActual; i++) {
      hidl_memory hidlMemToken;
      sp<TMemory> hidlMem;
      bool success;
      m_pEncOutpBufHeader_iomx[i] = (VideoDecBufferInfo *)malloc(sizeof(
                                      VideoDecBufferInfo));
      m_pEncOutpBufHeader_iomx[i]->fencefd = -1;
      auto transStatus = mAllocator[portFmt.nPortIndex]->allocate(
                           portFmt.nBufferSize,
                           [&success, &hidlMemToken](
                             bool s,
      hidl_memory const & m) {
        success = s;
        hidlMemToken = m;
      });

      if (!transStatus.isOk()) {
        ERROR2("hidl's AshmemAllocator failed at the "
               "transport: %s",
               transStatus.description().c_str());
        return VIDEO_ERROR_UNKNOWN;
      }

      if (!success) {
        return VIDEO_ERROR_UNKNOWN;
      }

      hidlMem = mapMemory(hidlMemToken);
      OutputHidlmem[i] = hidlMem;
      err = m_NodeId->useBuffer(
              portFmt.nPortIndex, hidlMemToken, &(m_pEncOutpBufHeader_iomx[i]->buffer_id));

      if (err != OK) {
        CRITICAL2(
          "VideoTransmitter::EncodeConfigure:: error in Allocate Buffer for output ports err : %d ",
          err);
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_pEncOutpBufHeader_iomx[i]) {
        m_pEncOutpBufHeader_iomx[i]->filllen = 0;
        m_pEncOutpBufHeader_iomx[i]->alloclen = portFmt.nBufferSize;
      } else {
        CRITICAL1(
          "VideoTransmitter::EncodeConfigure:: m_pEncInpBufHeader is NULL and not initialized");
      }

      if (OutputHidlmem[i] != NULL) {
        m_pEncOutpBufHeader_iomx[i]->pBuffer = (void*)OutputHidlmem[i]->getPointer();
        m_pEncOutpBufHeader_iomx[i]->offset = 0;
        m_pEncOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
        DEBUG2("m_pEncOutpBufHeader_iomx[i]->pBuffer %p\n",
               m_pEncOutpBufHeader_iomx[i]->pBuffer);
      }
    }
  }

  CHECK(sem_timedwait());
  CRITICAL2("RecorderStateTransition::Configure::Encoder Configure done OMX_CommandStateSet:%d",
            m_IOMXState);
  return VIDEO_ERROR_OK;
}

/************************************************************************************************
 Configure H263 codec parameters for OMX encoder
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::ConfigureH263EncoderPort(OMX_U32 iPort,
    QP_VIDEO_CONFIG sVideoConfig)
{
  TRACE1("VideoTransmitter::ConfigureH263EncoderPort :: configuring H263 port parameters");
  int result = OK;

  if (iPort == OMX_DirOutput) {
    OMX_VIDEO_PARAM_H263TYPE h263;
    memset(&h263, 0, sizeof(OMX_VIDEO_PARAM_H263TYPE));
    InitEncoderOMXParams(&h263);
    h263.nPortIndex = (OMX_U32) iPort;
    result = m_NodeId->getParameter(OMX_IndexParamVideoH263,
                                    (OMX_PTR *)&h263, sizeof(OMX_VIDEO_PARAM_H263TYPE));

    if (result != OK) {
      ERROR2("VideoTransmitter::getParam OMX_IndexParamVideoH263 with return value %x",
             result);
      return VIDEO_ERROR_UNKNOWN;
    }

    h263.nPortIndex = (OMX_U32) OMX_DirOutput;
    h263.nPFrames = sVideoConfig.iFrameRate;
    EVENT2(
      "VideoTransmitter::ConfigureH263EncoderPort:: Setting Max Frame Rate = %d",
      h263.nPFrames);

    switch (sVideoConfig.eH263ProfileLevel) {
      case VIDEO_H263_LEVEL10:
        h263.eLevel = OMX_VIDEO_H263Level10;
        break;

      case VIDEO_H263_LEVEL20:
        h263.eLevel = OMX_VIDEO_H263Level20;
        break;

      case VIDEO_H263_LEVEL30:
        h263.eLevel = OMX_VIDEO_H263Level30;
        break;

      case VIDEO_H263_LEVEL40:
        h263.eLevel = OMX_VIDEO_H263Level40;
        break;

      case VIDEO_H263_LEVEL50:
        h263.eLevel = OMX_VIDEO_H263Level50;
        break;

      case VIDEO_H263_LEVEL60:
        h263.eLevel = OMX_VIDEO_H263Level60;
        break;

      case VIDEO_H263_LEVEL70:
        h263.eLevel = OMX_VIDEO_H263Level70;
        break;

      case VIDEO_H263_LEVEL45:
        h263.eLevel = OMX_VIDEO_H263Level45;
        break;

      default:
        h263.eLevel = OMX_VIDEO_H263Level10;
        break;
    }

    switch (sVideoConfig.eH263ProfileType) {
      case VIDEO_H263_PROFILE_BASELINE:
        h263.eProfile = OMX_VIDEO_H263ProfileBaseline; //H263 Profile 0
        break;

      case VIDEO_H263_PROFILE_H320_CODING:
        break;

      case VIDEO_H263_PROFILE_BACKWARD_COMPATIBLE:
        break;

      case VIDEO_H263_PROFILE_ISWV2:
        break;

      case VIDEO_H263_PROFILE_ISWV3:
        break;

      case VIDEO_H263_PROFILE_HIGH_COMPRESSION:
        break;

      case VIDEO_H263_PROFILE_INTERNET:
        break;

      case VIDEO_H263_PROFILE_INTERLACE:
        break;

      case VIDEO_H263_PROFILE_HIGH_LATENCY:
        break;

      default:
        h263.eProfile = OMX_VIDEO_H263ProfileBaseline;
        break;
    }

    h263.bPLUSPTYPEAllowed = OMX_FALSE;
    h263.nAllowedPictureTypes = 2;
    h263.bForceRoundingTypeToZero = OMX_TRUE; ///@todo determine what this should be
    h263.nPictureHeaderRepetition = 0; ///@todo determine what this should be
    // TBD determine more elegant way to communicate h263 error resilience tools w/o packetization
    h263.nGOBHeaderInterval = 1;
    result = m_NodeId->setParameter(OMX_IndexParamVideoH263,
                                    (OMX_PTR *)&h263, sizeof(OMX_VIDEO_PARAM_H263TYPE));

    if (result != OK) {
      LOGE("VideoTransmitter::setParam OMX_IndexParamVideoH263 with return value %x",
           result);
      return VIDEO_ERROR_UNKNOWN;
    }

    ConfigureBitrateMode();
  }

  return VIDEO_ERROR_OK;
}

/************************************************************************************************
 Configure MPEG4 codec parameters for OMX encoder
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::ConfigureMPEG4EncoderPort(
  OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig)
{
  OMX_CONFIG_FRAMERATETYPE framerate;
  OMX_VIDEO_CONFIG_BITRATETYPE ConfigBitRate;
  OMX_CONFIG_FRAMERATETYPE ConfigFramerate;
  OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE ResyncMarker;
  status_t result = OMX_ErrorNone;
  OMX_VIDEO_PARAM_MPEG4TYPE mpeg4;
  memset(&framerate, 0, sizeof(OMX_CONFIG_FRAMERATETYPE));
  memset(&ConfigBitRate, 0, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));
  memset(&ConfigFramerate, 0, sizeof(OMX_CONFIG_FRAMERATETYPE));
  memset(&ResyncMarker, 0, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE));
  memset(&mpeg4, 0, sizeof(OMX_VIDEO_PARAM_MPEG4TYPE));
  InitEncoderOMXParams(&framerate);
  InitEncoderOMXParams(&ConfigBitRate);
  InitEncoderOMXParams(&ConfigFramerate);
  InitEncoderOMXParams(&ResyncMarker);
  InitEncoderOMXParams(&mpeg4);
  TRACE1("VideoTransmitter::ConfigureMPEG4EncoderPort:: Configuring the MPEG4 port parameteres");
  result = m_NodeId->getParameter(OMX_IndexParamVideoMpeg4,
                                  &mpeg4, sizeof(OMX_VIDEO_PARAM_MPEG4TYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::getParam OMX_IndexParamVideoMpeg4 with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  mpeg4.nPortIndex = (OMX_U32) iPort;
  mpeg4.eProfile = OMX_VIDEO_MPEG4ProfileSimple;
  mpeg4.eLevel = OMX_VIDEO_MPEG4Level0;
  mpeg4.nPFrames = sVideoConfig.iFrameRate;
  mpeg4.bSVH = OMX_FALSE;
  mpeg4.nIDCVLCThreshold = 0;
  mpeg4.bACPred = OMX_TRUE;
  mpeg4.bReversibleVLC = OMX_FALSE;
  mpeg4.nHeaderExtension = 0; // check this value
  result = m_NodeId->setParameter(OMX_IndexParamVideoMpeg4,
                                  &mpeg4, sizeof(OMX_VIDEO_PARAM_MPEG4TYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::setParam OMX_IndexParamVideoMpeg4 with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  result = ConfigureBitrateMode();
  //configuration settings
  result = m_NodeId->getConfig(OMX_IndexConfigVideoFramerate,
                               &ConfigFramerate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::getConfig OMX_IndexConfigVideoFramerate with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  ConfigFramerate.xEncodeFramerate = sVideoConfig.iFrameRate << 16;
  result = m_NodeId->setConfig(OMX_IndexConfigVideoFramerate,
                               &ConfigFramerate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::setConfig OMX_IndexConfigVideoFramerate with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  //configuration settings
  result = m_NodeId->getConfig(OMX_IndexConfigVideoBitrate,
                               &ConfigBitRate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::getConfig OMX_IndexConfigVideoBitrate with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  ConfigBitRate.nEncodeBitrate = sVideoConfig.iBitRate * 1000;
  result = m_NodeId->setConfig(OMX_IndexConfigVideoBitrate,
                               &ConfigBitRate, sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));

  if (result != OK) {
    ERROR2("VideoTransmitter::setConfig OMX_IndexConfigVideoBitrate with return value %x",
           result);
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}

/************************************************************************************************
 Configure H264 codec parameters for OMX encoder
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::ConfigureH264EncoderPort(
  OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig)
{
  TRACE1("VideoTransmitter::ConfigureH264EncoderPort");
  OMX_VIDEO_PARAM_AVCTYPE h264;
  OMX_VIDEO_CONFIG_AVCINTRAPERIOD idr_period;
  status_t result = OMX_ErrorNone;
  OMX_CONFIG_ROTATIONTYPE rotateframe;
  int iFrameInt = sVideoConfig.iFrameInterval;
  int nIDRPeriod = 1;
  int nPFrames = 0;

  if (iPort == 0) {
    return VIDEO_ERROR_OK;
  }

  memset(&h264, 0, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
  InitEncoderOMXParams(&h264);
  InitEncoderOMXParams(&idr_period);
  InitEncoderOMXParams(&rotateframe);
  h264.nPortIndex = DIR_OUT;
  result = m_NodeId->getParameter(OMX_IndexParamVideoAvc,
                                  (OMX_PTR *)&h264,
                                  sizeof(OMX_VIDEO_PARAM_AVCTYPE));

  if (result != OK) {
    ERROR2(
      "VideoTransmitter::ConfigureH264EncoderPort:: error getting h264 videoAVC err %d",
      result);
    return VIDEO_ERROR_UNKNOWN;
  }

  h264.nPortIndex = DIR_OUT;

  if(0 == iFrameInt) {
    nPFrames = 0;
  } else if (m_bLTRFeatureOn || MAX_IDR_INTERVAL == sVideoConfig.iFrameInterval) {
    nPFrames = MAX_IDR_PERIOD;
    nIDRPeriod = 0;
  } else {
    nPFrames = (iFrameInt * sVideoConfig.iFrameRate) - 1;
  }

  h264.nPFrames = nPFrames;
  h264.nBFrames = 0; //check this
  h264.bUseHadamard = OMX_FALSE;
  h264.nRefFrames = 1;
  h264.nRefIdx10ActiveMinus1 = 1;
  h264.nRefIdx11ActiveMinus1 = 0;
  h264.bEnableUEP = OMX_FALSE;
  h264.bEnableFMO = OMX_FALSE;
  h264.bEnableASO = OMX_FALSE;
  h264.bEnableRS = OMX_FALSE;
  h264.nAllowedPictureTypes = 2;
  h264.bFrameMBsOnly = OMX_FALSE;
  h264.bMBAFF = OMX_FALSE;
  h264.bWeightedPPrediction = OMX_FALSE;
  h264.nWeightedBipredicitonMode = 0;
  h264.bconstIpred = OMX_FALSE;
  h264.bDirect8x8Inference = OMX_FALSE;
  h264.bDirectSpatialTemporal = OMX_FALSE;
  h264.bEntropyCodingCABAC = OMX_FALSE;
  h264.nCabacInitIdc = 0;
  h264.eLoopFilterMode = OMX_VIDEO_AVCLoopFilterEnable;
  EVENT2(" sVideoConfig.eH264ProfileType is : %d",
         sVideoConfig.eH264ProfileType);
  EVENT2(" sVideoConfig.eH264ProfileLevel is : %d",
         sVideoConfig.eH264ProfileLevel);

  if (sVideoConfig.eH264ProfileType == VIDEO_AVC_PROFILE_BASELINE) {
    h264.eProfile = (OMX_VIDEO_AVCPROFILETYPE)
                    QOMX_VIDEO_AVCProfileConstrainedBaseline;//baseline profile
    CRITICAL2("VideoTransmitter::ConfigureH264EncoderPort:: h264.eProfile is : %d",
              h264.eProfile);
  }

  if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL1B) {
    h264.eLevel = OMX_VIDEO_AVCLevel1b;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL1) {
    h264.eLevel = OMX_VIDEO_AVCLevel1;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL11) {
    h264.eLevel = OMX_VIDEO_AVCLevel11;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL12) {
    h264.eLevel = OMX_VIDEO_AVCLevel12;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL13) {
    h264.eLevel = OMX_VIDEO_AVCLevel13;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL2) {
    h264.eLevel = OMX_VIDEO_AVCLevel2;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL21) {
    h264.eLevel = OMX_VIDEO_AVCLevel21;  // level 1b is used as per AT & T
  } else if (sVideoConfig.eH264ProfileLevel == VIDEO_AVC_LEVEL22) {
    h264.eLevel = OMX_VIDEO_AVCLevel22;  // level 1b is used as per AT & T
  }

  EVENT2(" Profile Level set is : %d", h264.eLevel);
  EVENT2(" Profile type set is : %d", h264.eProfile);
  result = m_NodeId->setParameter(OMX_IndexParamVideoAvc,
                                  (OMX_PTR *)&h264,
                                  sizeof(OMX_VIDEO_PARAM_AVCTYPE));

  if (result != OK) {
    ERROR2(
      "VideoTransmitter::ConfigureH264EncoderPort:: error setting h264 videoAVC, %d",
      result);
    return VIDEO_ERROR_UNKNOWN;
  }

  result = ConfigureBitrateMode();
  idr_period.nPortIndex = (OMX_U32) DIR_OUT;
  result = m_NodeId->getConfig(
             (OMX_INDEXTYPE) OMX_IndexConfigVideoAVCIntraPeriod,
             (OMX_PTR *) &idr_period,
             sizeof(OMX_VIDEO_CONFIG_AVCINTRAPERIOD));

  if (result == OMX_ErrorNone) {
    idr_period.nPFrames = nPFrames;
    idr_period.nIDRPeriod = nIDRPeriod;
    CRITICAL4("Tx-LTR:: Configure:nPFrames %d, nIDRPeriod %d, iframeInt %d",
              nPFrames, nIDRPeriod, iFrameInt);
    result = m_NodeId->setConfig(
               (OMX_INDEXTYPE) OMX_IndexConfigVideoAVCIntraPeriod,
               (OMX_PTR *) &idr_period,
               sizeof(OMX_VIDEO_CONFIG_AVCINTRAPERIOD));

    if (result != OK) {
      ERROR2("VideoTransmitter:: OMX_IndexConfigVideoAVCIntraPeriod err %d", result);
    }
  }

  CRITICAL2("VideoTransmitter::ConfigureCommonEncoderPort :: ePktnMode - %d",
            sVideoConfig.ePktnMode);

  if (m_SliceMode == 1
      || (sVideoConfig.ePktnMode) == VIDEO_CODEC_H264_SINGLE_NALU  ) {
    OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE errCorrection;
    memset(&errCorrection, 0, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE));
    InitEncoderOMXParams(&errCorrection);
    errCorrection.nPortIndex = DIR_OUT;

    if (m_NodeId->getParameter(OMX_IndexParamVideoErrorCorrection,
                               (OMX_PTR*) &errCorrection, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE))
        != OK) {
      CRITICAL1("VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for OMX_IndexParamVideoErrorCorrection format");
    }

    errCorrection.bEnableDataPartitioning = OMX_FALSE;
    errCorrection.bEnableRVLC = OMX_FALSE;
    CRITICAL2("VideoTransmitter::ConfigureCommonEncoderPort :: configuring slice mode for =%d bits",
              (sVideoConfig.iRtpMTUSize) * 8);
    errCorrection.nResynchMarkerSpacing = (sVideoConfig.iRtpMTUSize) * 8;
    errCorrection.bEnableResync = OMX_TRUE;

    if (m_NodeId->setParameter(OMX_IndexParamVideoErrorCorrection,
                               (OMX_PTR*) &errCorrection, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE))
        != OK) {
      CRITICAL1(
        "VideoTransmitter::ConfigureCommonEncoderPort :: Failed to set the parameter for OMX_IndexParamVideoErrorCorrection format");
    }
  }

  //! Encoder is configured to generate LTR frames of count "cLtrPicNum"
  if (m_bLTRFeatureOn) {
    QOMX_VIDEO_PARAM_LTRCOUNT_TYPE ltrCount;
    InitEncoderOMXParams(&ltrCount);
    ltrCount.nPortIndex = 1;
    ltrCount.nCount = NUM_LTR_MARK_FRAMES;
    m_NodeId->setParameter(
      (OMX_INDEXTYPE)QOMX_IndexParamVideoLTRCount,
      (OMX_PTR*) &ltrCount, sizeof(QOMX_VIDEO_PARAM_LTRCOUNT_TYPE));
    CRITICAL2("Tx-LTR: setParameters to use %d Ltrs", ltrCount.nCount);
  }

  return VIDEO_ERROR_OK;
}


/************************************************************************************************
Configure H265 codec parameters for OMX encoder
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::ConfigureH265EncoderPort(OMX_U32 /*iPort*/,
    QP_VIDEO_CONFIG sVideoConfig)
{
  TRACE1("VideoTransmitter::ConfigureH265EncoderPort");
  status_t result = OMX_ErrorNone;
  OMX_VIDEO_PARAM_HEVCTYPE h265;
  int iFrameInt = sVideoConfig.iFrameInterval;
  memset(&h265, 0, sizeof(OMX_VIDEO_PARAM_HEVCTYPE));
  InitEncoderOMXParams(&h265);
  h265.nPortIndex = 1;
  result = m_NodeId->getParameter(
             (OMX_INDEXTYPE)OMX_IndexParamVideoHevc,
             (OMX_PTR *)&h265, sizeof(OMX_VIDEO_PARAM_HEVCTYPE));

  if (OK != result) {
    ERROR1("VideoTransmitter::ConfigureH265EncoderPort:: error getting h265 videoHevc");
    return VIDEO_ERROR_UNKNOWN;
  }

  h265.nKeyFrameInterval = (iFrameInt * sVideoConfig.iFrameRate) - 1;
  CRITICAL4("VideoTransmitter:: eH265ProfileType - %d, eH265ProfileLevel - %d, iframeinterval %d",
            sVideoConfig.eH265ProfileType, sVideoConfig.eH265ProfileLevel,
            h265.nKeyFrameInterval);

  if (sVideoConfig.eH265ProfileType == VIDEO_H265_PROFILE_MAIN) {
    h265.eProfile = OMX_VIDEO_HEVCProfileMain;  //main profile
  }

  if (sVideoConfig.eH265ProfileType == VIDEO_H265_PROFILE_MAIN10) {
    h265.eProfile = OMX_VIDEO_HEVCProfileMain10;  //main10 profile
  }

  if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL1) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel1 ;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL2) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel2;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL21) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel21;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL3) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel3;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL31) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel31;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL4) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel4;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL41) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel41;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL5) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel5;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL51) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel51;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL52) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel52;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL6) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel6;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL61) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel61;  // H265 level
  } else if (sVideoConfig.eH265ProfileLevel == VIDEO_H265_LEVEL62) {
    h265.eLevel = OMX_VIDEO_HEVCMainTierLevel62;  // H265 level
  }

  result = m_NodeId->setParameter(
             (OMX_INDEXTYPE)OMX_IndexParamVideoHevc,
             (OMX_PTR *)&h265, sizeof(OMX_VIDEO_PARAM_HEVCTYPE));

  if (OK != result) {
    ERROR1("VideoTransmitter::ConfigureH265EncoderPort:: error in setting h265 videoHevc");
    return VIDEO_ERROR_UNKNOWN;
  }

  ConfigureBitrateMode();
  return VIDEO_ERROR_OK;
}

/************************************************************************************************
 Function to send frames to the application SendEncodedFramesToApp
 ************************************************************************************************/
QPE_VIDEO_ERROR VideoTransmitter::SendEncodedFramesToAppfromIOMX(
  OMX_TICKS timestamp,
  uint64_t appsTimestamp,
  VideoDecBufferInfo* pVideoBufferInfo,
  OMX_U32 range_offset,
  OMX_U32 range_length)
{
  DEBUG2_STR("VideoTransmitter::SendEncodedFramesToAppfromIOMX %llu",
             (unsigned long long)timestamp);

  if (NULL == pVideoBufferInfo->pBuffer) {
    ERROR1("VideoTransmitter::SendEncodedFramesToAppfromIOMX NULL pointer received for sending to RTP");
    return VIDEO_ERROR_UNKNOWN;
  }

  if (pVideoBufferInfo->pBuffer  && range_length > 0) {
    QP_MULTIMEDIA_FRAME iQpFrame;
    MEDIA_PACKET_INFO_TX* pMediaPktInfoTx =
      &(iQpFrame.sMediaPacketInfo.sMediaPktInfoTx);
    uint64_t iTimeStamp = 0;
    uint32_t iTimeStampIncrement = 0;
    timeval tv;
    memset(&iQpFrame, 0, sizeof(QP_MULTIMEDIA_FRAME));
    DEBUG3_STR(
      "VideoTransmitter::SendEncodedFramesToAppfromIOMX:: timestamp before sending it to RTP %llu and base time stamp is %llu",
      (unsigned long long)iTimeStamp, (unsigned long long)m_iBaseTimeStamp);
    iQpFrame.pData = (uint8_t*) pVideoBufferInfo->pBuffer;
    iQpFrame.iDataLen = range_length;
    pMediaPktInfoTx->bMarkerBit = QP_TRUE;
    pMediaPktInfoTx->iTimeStamp = timestamp;
    pMediaPktInfoTx->appTimeStamp = appsTimestamp;
    pMediaPktInfoTx->ullSysVidFrameOPts =
      timestamp + TIMEVAL2MS(tv) - appsTimestamp;
    /* Update LTR info if extra data is available*/
    pMediaPktInfoTx->ulLtrExtraData = 0xFFFF;
    ProcessExtraData(range_offset, range_length, pVideoBufferInfo->pBuffer,
                     &(pMediaPktInfoTx->ulLtrExtraData));
    CRITICAL3_STR("VideoTx::SendEncodedFramesToAppfromIOMX :: Sending frame to RTP with frame number %d frametime = %llu",
                  m_ulFrameCount, (unsigned long long)pMediaPktInfoTx->iTimeStamp);
    m_pCallback(VIDEO_MSG_RECORDED_DATA, (void*) (&iQpFrame), m_eVidDevType,
                SUCCESS, m_pUserData);
    m_ulFrameCount++;
  } else {
    ERROR1("VideoTransmitter::SendEncodedFramesToAppfromIOMX - Invalid frame");
  }

  return VIDEO_ERROR_OK;
}

/*********************************************************************************
 Free the OMX buffers
 *********************************************************************************/
void VideoTransmitter::QPVIDEOOMXFreeBuffers()
{
  return;
}

/************************************************************************************************
 Initializing the IOMX codec components
 ************************************************************************************************/

QPE_VIDEO_ERROR VideoTransmitter::LoadIOMXEncoderComponents()
{
  TRACE1("VideoTransmitter::LoadEncoderComponents");
  status_t err = OK;
  //Initialize the recorder states
  iRecorderState = eVideoStateNotReady;
  sp<OmxTransmitterObserver> observer = new OmxTransmitterObserver;
  observer.get()->setCodec(this);

  if (m_codecConfig.eCodec == VIDEO_CODEC_H263) {
    VerifyComponentName(H263_VIDEO_ENCODER);
    err = m_OMX_handle->allocateNode(H263_VIDEO_ENCODER, observer,
                                     &m_NodeId);

    if (err != OK) {
      ERROR3(
        "VideoTransmitter::LoadEncoderComponents::Error getting the encoder component handle for  %s codec Err No 0X%x",
        H263_VIDEO_ENCODER, err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_iConfiguredCodec = VIDEO_CODEC_H263;
  } else if (m_codecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
             || m_codecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
    VerifyComponentName(MPEG4_VIDEO_ENCODER);
    err = m_OMX_handle->allocateNode(MPEG4_VIDEO_ENCODER, observer,
                                     &m_NodeId);

    if (err != OK) {
      ERROR3(
        "VideoTransmitter::LoadEncoderComponents::Error getting the encoder component handle for  %s codec Err No 0X%x",
        MPEG4_VIDEO_ENCODER, err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_iConfiguredCodec = VIDEO_CODEC_MPEG4_ISO;
  } else if (m_codecConfig.eCodec == VIDEO_CODEC_H264) {
    VerifyComponentName(H264_VIDEO_ENCODER);
    err = m_OMX_handle->allocateNode(H264_VIDEO_ENCODER, observer,
                                     &m_NodeId);

    if (err != OK) {
      ERROR3(
        "VideoTransmitter::LoadEncoderComponents::Error getting the encoder component handle for  %s codec Err No 0X%x",
        H264_VIDEO_ENCODER, err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_iConfiguredCodec = VIDEO_CODEC_H264;
  } else if (m_codecConfig.eCodec == VIDEO_CODEC_H265) {
    VerifyComponentName(H265_VIDEO_ENCODER);
    err = m_OMX_handle->allocateNode(H265_VIDEO_ENCODER, observer,
                                     &m_NodeId);

    if (err != OK) {
      ERROR3(
        "VideoTransmitter::LoadEncoderComponents::Error getting the encoder component handle for  %s codec Err No 0X%x",
        H265_VIDEO_ENCODER, err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_iConfiguredCodec = VIDEO_CODEC_H265;
  } else {
    ERROR1(
      "VideoTransmitter::LoadEncoderComponents:: in valid video codec and returning");
    return VIDEO_ERROR_UNKNOWN;
  }

  m_bOmxNodeAllocated = QP_TRUE;
  return VIDEO_ERROR_OK;
}
QPE_VIDEO_ERROR VideoTransmitter::MovetoLoadedState()
{
  EVENT3("VideoTransmitter::MovetoLoadedState in recorder state = %d omx state = %d",
         iRecorderState, m_IOMXState);
  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);
  status_t err = m_NodeId->sendCommand(OMX_CommandStateSet,
                                       OMX_StateIdle);

  if (err != OK) {
    ERROR2("VideoTransmitter::MovetoLoadedState Error Sending Idle State: %d\n",
           err);
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT3("VideoTransmitter::MovetoLoadedState - WAIT FOR IDLE STATE recorder state = %d omx state = %d",
         iRecorderState, m_IOMXState);
  CHECK(sem_timedwait());
  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
            m_IOMXState);
  DEBUG1("VideoTransmitter::MovetoLoadedState - moved to IDLE STATE");
  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);
  err = m_NodeId->sendCommand(OMX_CommandStateSet,
                              OMX_StateLoaded);

  if (err != OK) {
    ERROR2(
      "VideoTransmitter::MovetoLoadedState - Error Sending Loaded State: %d\n",
      err);
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT2("VideoTransmitter::MovetoLoadedState:: m_iMinOutputBufCount = %d",
         m_iMinOutputBufCount);

  //Free out put buffer
  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
    EVENT2("VideoTransmitter::MovetoLoadedState:: freeing the out buffer %d",
           i);

    if (NULL != m_pEncOutpBufHeader_iomx[i]) {
      if (m_NodeId->freeBuffer(DIR_OUT,
                               m_pEncOutpBufHeader_iomx[i]->buffer_id) != OK) {
        ERROR2(
          "VideoTransmitter::MovetoLoadedState:: Failed to free the output buffer %d",
          i);
      }
    }
  }

  if (!isTrebleEnabled) {
    if (m_codecConfig.iInternalCodecConfig == QP_FALSE) {
      /** Free the Descriptors alocated for IOMX ouput port */
      for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
        if (m_pEncOutpBufHeader_iomx[i] != NULL) {
          m_mDealer[DIR_OUT]->deallocate(m_pEncOutpBufHeader_iomx[i]->offset);

          if (m_pEncOutpBufHeader_iomx[i] != NULL) {
            free(m_pEncOutpBufHeader_iomx[i]);
            m_pEncOutpBufHeader_iomx[i] = NULL;
          }
        }
      }
    }
  } else {
    for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
      if (m_pEncOutpBufHeader_iomx[i] != NULL) {
        if (m_pEncOutpBufHeader_iomx[i] != NULL) {
          free(m_pEncOutpBufHeader_iomx[i]);
          m_pEncOutpBufHeader_iomx[i] = NULL;
        }

        if (OutputHidlmem[i].get() != NULL) {
          OutputHidlmem[i] = NULL;
        }
      }
    }

    mAllocator[DIR_OUT].clear();
  }

  EVENT2("VideoTransmitter::MovetoLoadedState:: m_iMinInputBufCount = %d",
         m_iMinInputBufCount);

  //Free input buffer
  for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
    EVENT2("VideoTransmitter::MovetoLoadedState:: freeing the in buffer %d",
           i);

    if (m_pEncInpBufHeader_iomx[i] != NULL) {
      if (m_NodeId->freeBuffer(DIR_INP,
                               m_pEncInpBufHeader_iomx[i]->buffer_id) != OK) {
        ERROR2("VideoTransmitter::MovetoLoadedState:: Failed to free the input buffer %d",
               i);
      }
    }
  }

  if (!isTrebleEnabled) {
    if (m_codecConfig.iInternalCodecConfig == QP_FALSE) {
      /** Free the Descriptors alocated for IOMX input port */
      for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
        if (m_pEncInpBufHeader_iomx[i] != NULL) {
          m_mDealer[DIR_INP]->deallocate(m_pEncInpBufHeader_iomx[i]->offset);

          if (m_pEncInpBufHeader_iomx[i] != NULL) {
            free(m_pEncInpBufHeader_iomx[i]);
            m_pEncInpBufHeader_iomx[i] = NULL;
          }
        }
      }
    }
  } else {
    for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      if (m_pEncInpBufHeader_iomx[i] != NULL) {
        if (m_pEncInpBufHeader_iomx[i] != NULL) {
          free(m_pEncInpBufHeader_iomx[i]);
          m_pEncInpBufHeader_iomx[i] = NULL;
        }
      }

      if (InputHidlmem[i].get() != NULL) {
        InputHidlmem[i] = NULL;
      }
    }

    mAllocator[DIR_INP].clear();
  }

  EVENT3("VideoTransmitter::MovetoLoadedState - DEACTIVATE recorder state = %d omx state = %d",
         iRecorderState, m_IOMXState);
  CHECK(sem_timedwait());
  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
            m_IOMXState);
  EVENT1("VideoTransmitter::MovetoLoadedState::: setting the state as eVideoStateNotReady and raising the VIDEO_MSG_DEV_UNINITIALISED event");

  if (m_bOmxNodeAllocated && m_NodeId->freeNode() != OK) {
    ERROR1("VideoTransmitter::HandleEvent :Error in de initializing the components");
  } else {
    CRITICAL1("VideoTransmitter::HandleEvent: able to free the node ");
  }

  iRecorderState = eVideoStateNotReady;
  MINIMAL1("VideoTransmitter::MovetoLoadedState - COMPLETE");
  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoTransmitter::EncoderFreeHandle ::De Initialize the decoder components
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoTransmitter::TearDown()
{
  status_t err;
  CRITICAL3("RecorderStateTransition:Recorder:TearDown:%d:VIDEO_MSG_DEV_UNINITIALISED, OMX_CommandStateSet:%d",
            iRecorderState, m_IOMXState);

  if (m_IOMXState == OMX_StateIdle) {
    EVENT1("VideoTransmitter::TearDown Move to Executing State");

    if (m_NodeId->sendCommand(OMX_CommandStateSet,
                              OMX_StateExecuting) != OK) {
      ERROR1(
        "VideoTransmitter::TearDown::Failure in insrting the command for stateExecuting");
      return VIDEO_ERROR_UNKNOWN;
    }

    if (sem_timedwait() != VIDEO_ERROR_OK) {
      iRecorderState = eVideoStateNotReady;
      EVENT1("VideoTransmitter :: Post VIDEO_MSG_DEV_UNINITIALISED");
      CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_DEV_UNINITIALISED:%d",
                iRecorderState);
      SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
    }

    CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
              m_IOMXState);
  }

  if (m_IOMXState == OMX_StatePause || m_IOMXState == OMX_StateExecuting ) {
    err = m_NodeId->sendCommand(OMX_CommandStateSet, OMX_StateIdle);

    if (err != OK) {
      ERROR2("VideoTransmitter::TearDown Error Sending Idle State: %d\n", err);
      return VIDEO_ERROR_UNKNOWN;
    }

    EVENT3(
      "VideoTransmitter::TearDown - WAIT FOR IDLE STATE recorder state = %d omx state = %d",
      iRecorderState, m_IOMXState);

    if (sem_timedwait() != VIDEO_ERROR_OK) {
      iRecorderState = eVideoStateNotReady;
      EVENT1("VideoTransmitter :: Post VIDEO_MSG_DEV_UNINITIALISED");
      CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_DEV_UNINITIALISED:%d",
                iRecorderState);
      SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
    }
  }

  EVENT1("VideoTransmitter::TearDown - moved to IDLE STATE");
  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);

  if (m_IOMXState == OMX_StateIdle) {
    err = m_NodeId->sendCommand(OMX_CommandStateSet,
                                OMX_StateLoaded);

    if (err != OK) {
      ERROR2("VideoTransmitter::TearDown - Error Sending Loaded State: %d\n",
             err);
      return VIDEO_ERROR_UNKNOWN;
    }

    EVENT2("VideoTransmitter::TearDown:: m_iMinOutputBufCount = %d",
           m_iMinOutputBufCount);

    //Free out put buffer
    for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
      EVENT2("VideoTransmitter::TearDown:: freeing the out buffer %d", i);

      if (NULL != m_pEncOutpBufHeader_iomx[i]) {
        if (m_NodeId->freeBuffer(DIR_OUT,
                                 m_pEncOutpBufHeader_iomx[i]->buffer_id) != OK) {
          ERROR2(
            "VideoTransmitter::TearDown:: Failed to free the output buffer %d",
            i);
        }
      }
    }

    EVENT1("VideoTransmitter::TearDown: Free the Ouput Descriptors");

    if (!isTrebleEnabled) {
      /** Free the Descriptors alocated for IOMX ouput port */
      //for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
      for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        if (m_pEncOutpBufHeader_iomx[i] != NULL) {
          m_mDealer[DIR_OUT]->deallocate(m_pEncOutpBufHeader_iomx[i]->offset);

          if (m_pEncOutpBufHeader_iomx[i] != NULL) {
            free(m_pEncOutpBufHeader_iomx[i]);
            m_pEncOutpBufHeader_iomx[i] = NULL;
          }
        }

        if (Outputmem[i].get() != NULL) {
          Outputmem[i] = NULL;
        }
      }
    } else {
      for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        if (m_pEncOutpBufHeader_iomx[i] != NULL) {
          if (m_pEncOutpBufHeader_iomx[i] != NULL) {
            free(m_pEncOutpBufHeader_iomx[i]);
            m_pEncOutpBufHeader_iomx[i] = NULL;
          }
        }

        if (OutputHidlmem[i].get() != NULL) {
          OutputHidlmem[i] = NULL;
        }
      }

      mAllocator[DIR_OUT].clear();
    }

    EVENT2("VideoTransmitter::TearDown:: m_iMinInputBufCount = %d",
           m_iMinInputBufCount);

    //Free input buffer
    for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      MINIMAL2("VideoTransmitter::TearDown:: freeing the in buffer %d", i);

      if (m_pEncInpBufHeader_iomx[i] != NULL) {
        if (m_NodeId->freeBuffer(DIR_INP,
                                 m_pEncInpBufHeader_iomx[i]->buffer_id) != OK) {
          ERROR2(
            "VideoTransmitter::TearDown:: Failed to free the input buffer %d",
            i);
        }
      }
    }

    DEBUG1("VideoTransmitter::TearDown: Free the Input Descriptors");

    if (!isTrebleEnabled) {
      /** Free the Descriptors alocated for IOMX input port */
      //for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        if (m_pEncInpBufHeader_iomx[i] != NULL) {
          m_mDealer[DIR_INP]->deallocate(m_pEncInpBufHeader_iomx[i]->offset);

          if (m_pEncInpBufHeader_iomx[i] != NULL) {
            free(m_pEncInpBufHeader_iomx[i]);
            m_pEncInpBufHeader_iomx[i] = NULL;
          }
        }

        if (Inputmem[i].get() != NULL) {
          Inputmem[i] = NULL;
        }
      }
    } else {
      for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
        if (m_pEncInpBufHeader_iomx[i] != NULL) {
          if (m_pEncInpBufHeader_iomx[i] != NULL) {
            free(m_pEncInpBufHeader_iomx[i]);
            m_pEncInpBufHeader_iomx[i] = NULL;
          }
        }

        if (InputHidlmem[i].get() != NULL) {
          InputHidlmem[i] = NULL;
        }
      }

      mAllocator[DIR_INP].clear();
    }

    EVENT3(
      "VideoTransmitter::TearDown - DEACTIVATE recorder state = %d omx state = %d",
      iRecorderState, m_IOMXState);

    if (sem_timedwait() != VIDEO_ERROR_OK) {
      iRecorderState = eVideoStateNotReady;
      EVENT1("VideoTransmitter :: Post VIDEO_MSG_DEV_UNINITIALISED");
      CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_DEV_UNINITIALISED:%d",
                iRecorderState);
      SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
    }
  }

  CRITICAL2("RecorderStateTransition:OMX_Encoder:OMX_CommandStateSet:%d",
            m_IOMXState);
  EVENT1(
    "VideoTransmitter::TearDown::: setting the state as eVideoStateNotReady and raising the VIDEO_MSG_DEV_UNINITIALISED event");
  iRecorderState = eVideoStateNotReady;
  EVENT1("VideoTransmitter :: Post VIDEO_MSG_DEV_UNINITIALISED");
  CRITICAL2("RecorderStateTransition:Recorder:VIDEO_MSG_DEV_UNINITIALISED:%d",
            iRecorderState);
  SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
  MINIMAL1("VideoTransmitter::TearDown: Loaded state free node");

  if ( m_bOmxNodeAllocated && m_NodeId->freeNode() != OK) {
    ERROR1("VideoTransmitter::HandleEvent :Error in de initializing the components");
  } else {
    CRITICAL1("VideoTransmitter::HandleEvent : able to free the node ");
  }

  MINIMAL1("VideoTransmitter::TearDown - COMPLETE");
  return VIDEO_ERROR_OK;
}





void VideoTransmitter::onMessages(const std::list<omx_message> &messages)
{
  std::list<omx_message>::const_iterator msg;

  for (msg = messages.begin(); msg != messages.end(); ++msg) {
    switch (msg->type) {
      case omx_message::EVENT:
        EVENT1("VideoTransmitter::onMessage EVENT\n");
        IOMXVideoEncEventHandler(m_NodeId, msg->u.event_data.event,
                                 msg->u.event_data.data1, msg->u.event_data.data2);
        break;

      case omx_message::EMPTY_BUFFER_DONE:
        DEBUG1("VideoTransmitter::onMessage EMPTY_BUFFER_DONE\n");
        IOMXVideoEncEmptyBufferDone(m_NodeId, msg->u.buffer_data.buffer);
        break;

      case omx_message::FILL_BUFFER_DONE:
        DEBUG1("VideoTransmitter::onMessage FILL_BUFFER_DONE\n");
        IOMXVideoEncFillBufferDone(m_NodeId, msg->u.extended_buffer_data.buffer,
                                   msg->u.extended_buffer_data.range_offset,
                                   msg->u.extended_buffer_data.range_length,
                                   msg->u.extended_buffer_data.flags,
                                   msg->u.extended_buffer_data.timestamp//,msg.u.extended_buffer_data.platform_private,msg.u.extended_buffer_data.data_ptr, 0
                                  );
        break;

      default:
        DEBUG2("VideoTransmitter::onMessage %d default case", msg->type);
        break;
    }
  }
}

}
