/*****************************************************************************
============================
Copyright (c)  2004-2012,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
 File: VideoTransmitter.h
 Description: Implementation class for VideoTransmitter

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-12-11    Rakesh K         First Version
 1-Dec-16   Sanjeev Mittal        Rearchiecture Version
 *****************************************************************************/

#ifndef __VIDEOTRANSMITTER_H__
#define __VIDEOTRANSMITTER_H__
#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
//#include <qpVideoCommon.h>
//#include <qpVideo.h>
#include <VTRecorder.h>
#include <VideoOmx.h>
#include <OMX_VideoExt.h>
#include <OMX_IndexExt.h>
#include <media/IOMX.h>
#include <binder/MemoryDealer.h>
#include <gui/IProducerListener.h>
#include <media/MediaCodecBuffer.h>
//#include "CommandQueue.h"

/* To include the instance for NAtive handle structure */
#include "NativeHandle.h"
#include <gui/IGraphicBufferProducer.h>
#include <gui/BufferQueue.h>
#include <gui/BufferQueue.h>
#include <gui/IGraphicBufferConsumer.h>
#include <gui/IConsumerListener.h>

using namespace android;

//define component names
#ifdef MSM8937
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263sw\0"
#else
#ifndef MSM8909
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263\0"
#else
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263sw\0"
#endif
#endif

#define MPEG4_VIDEO_ENCODER (char*)"OMX.qcom.video.encoder.mpeg4\0"
#define H264_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.avc\0"
#define H265_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.hevc\0"

namespace android
{

struct VideoDecBufferInfo {
  IOMX::buffer_id buffer_id;

  int fencefd;
  void *pBuffer;
  size_t alloclen;
  size_t filllen;
  ssize_t offset;
  boolean usedbyIOMX;
};

class RecordingCameraBuffer;
class VideoTransmitter;

class VideoCameraforHal1: public VtCamera
{
  private:
    VideoTransmitter *mTx;

  public:

    VideoCameraforHal1() {
      mTx = NULL;
    };
    virtual ~VideoCameraforHal1() {};
    VideoTransmitter* getClient() {
      return mTx;
    };
    void registerCameraCallbacks(boolean flag);
    static void engine_custom_params_callback(void* params);
    static void engine_camera_notify_callback(int32_t msgType,
        int32_t ext1,
        int32_t ext2,
        void* user);
    static void engine_camera_handle_callback_timestamp(nsecs_t timestamp,
        native_handle_t *handle);

    void setClient(VideoTransmitter *target) {
      mTx = target;
    }
};

class VideoTransmitter: public VideoOmx, public VTRecorder
{

  public:

    sp<RecordingCameraBuffer> mRecordingBuffer;
    VideoTransmitter();
    virtual ~VideoTransmitter();

    //virtual QPE_VIDEO_ERROR Init();
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void* pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG* pCodecConfig);
    virtual QPE_VIDEO_ERROR DeInit();
    virtual QPE_VIDEO_ERROR Start();
    virtual QPE_VIDEO_ERROR Stop();
    virtual QPE_VIDEO_ERROR Pause();
    virtual QPE_VIDEO_ERROR Resume();
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig);
    virtual QPE_VIDEO_ERROR CreateInputRecordingSurface(sp<IGraphicBufferProducer>
        *bufferproducer, uint32_t height, uint32_t width);
    virtual QPE_VIDEO_ERROR TearDown();
    virtual QPE_VIDEO_ERROR AdaptVideoBitRate(uint32_t iBitRate);


    virtual QPE_VIDEO_ERROR AdaptVideoFrameRate(uint32_t iFrameRate);
    virtual QPE_VIDEO_ERROR SetRotationAngle(uint16_t rotation);
    virtual QPE_VIDEO_ERROR SetSourceTime(uint8_t useSystemTime);
    virtual QPE_VIDEO_ERROR GenerateH264IDRFrame();


    virtual void HandleEvent(void* data);
    QPE_VIDEO_ERROR MovetoLoadedState();
    QPE_VIDEO_ERROR Validate_params(QP_VIDEO_CONFIG& tCodecConfig);
    QPE_VIDEO_ERROR SendEncodedFramesToApp(void* pOmxData);
    QPE_VIDEO_ERROR SendEncodedFramesToAppfromIOMX(
      OMX_TICKS timestamp, uint64_t appsTimestamp,
      VideoDecBufferInfo* pVideoBufferInfo,
      OMX_U32 range_offset, OMX_U32 range_length);
    void QPVIDEOOMXFreeBuffers();
    QPE_VIDEO_ERROR EncoderFreeHandle(void);
    //QPE_VIDEO_ERROR EncoderFreeHandleInternal(void);
    int VerifyFillBuffersDone(void);
    // encoder Helper functions
    void IOMXVideoEncEventHandler(sp<IOMXNode> node, OMX_EVENTTYPE eEvent,
                                  OMX_U32 Data1, OMX_U32 Data2);
    void IOMXVideoEncEmptyBufferDone(sp<IOMXNode> node,
                                     IOMX::buffer_id buffer);
    void IOMXVideoEncFillBufferDone(sp<IOMXNode>  node, IOMX::buffer_id buffer,
                                    OMX_U32 range_offset, OMX_U32 range_length, OMX_U32 flags,
                                    OMX_TICKS timestamp//, OMX_PTR platform_private, OMX_PTR data_ptr,OMX_U32 pmem_offset
                                   );

    //callback to get Frames from camera
    QPE_VIDEO_ERROR WaitTillAllBuffersAreConsumed();
    QPE_VIDEO_ERROR ProcessFramesFromCamera();
    QPE_VIDEO_ERROR ProcessRecordingFrame(uint64_t timestamp, void *dataPtr,
                                          size_t size);
    virtual QPE_VIDEO_ERROR AdaptVideoIdrPeriodicity(int32_t iFrameInt);
    void onMessages(const std::list<omx_message> &messages);
    enum eVideoState getRecorderState() {
      return iRecorderState;
    }

    QPE_VIDEO_BUFFER_MODE getBufferMode() {
      return m_codecConfig.iInputVideoBufferMode;
    }

    uint32_t getFrameRate() {
      return m_codecConfig.iFrameRate;
    }

    /* This API will be called to mark LTR frame.*/
    void ConfigLTRMark(int32_t slLtrIdx);
    /* This API will be called to use LTR frame.*/
    void ConfigLTRUse(int32_t slLtrIdx);
    /* Process ExtraData */
    void ProcessExtraData(uint32_t ulRangeOffset, uint32_t ulRangeLen,
                          void* pFrameBuf, uint32_t* pulLtrExtraData);

    QPE_VIDEO_ERROR ProcessImageFrame(uint64_t  timestamp,
                                      void *dataPtr, size_t size);

    int isCameraResolutionMatching();
    int32_t cameraHeight;
    int32_t cameraWidth;

  protected:

  private:
    //member variables

    QP_VIDEO_CONFIG m_codecConfig;
    enum eVideoState iRecorderState;
    //VIDEOCOMPONENTS* m_hTxComponents;
    //void* m_pFramesFromCamera2Encoder;
    //void* m_pFramesFromEncoder2DPL;

    //static VIDEOOMXMP4VOLInfo* m_pMp4VolInfo;
    uint8_t m_iMinInputBufCount;
    uint8_t m_iMinOutputBufCount;
    uint64_t m_iBaseTimeStamp;
    uint64_t m_iTimeStamp;
    //DISPLAY_BUFFER m_pBuffer; //static structure
    QPE_VIDEO_CODEC m_iConfiguredCodec;

    volatile OMX_STATETYPE m_IOMXState;
    //CommandQueue fill_buffer;
    boolean m_codecConfigured;
    uint32 m_PrevTimeStamp;
    uint8_t m_IDRframeinterval;
    bool m_Metamode;

    uint32 m_LastIDRtimestamp;
    bool m_IDRRetransmitOn;

    OMX_U32 m_OmxFrameFlags;
    uint32_t m_ulFrameCount;

    //member functions
    QPE_VIDEO_ERROR CameraInit(void* pOmxData);
    QPE_VIDEO_ERROR CameraShutDown();
    QPE_VIDEO_ERROR ConfigureCommonEncoderInputPort(
      QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureCommonEncoderOutputPort(
      QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR EncodeConfigure(
      QP_VIDEO_CONFIG tCodecConfig);
    QPE_VIDEO_ERROR ConfigureH263EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureMPEG4EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureH264EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR ConfigureH265EncoderPort(
      OMX_U32 iPort, QP_VIDEO_CONFIG sVideoConfig);
    QPE_VIDEO_ERROR LoadIOMXEncoderComponents();
    status_t ConfigureBitrateMode();

    QPE_VIDEO_ERROR ProcessRecordingFrame_HAL3(uint64_t  timestamp,
        void *dataPtr, size_t size);


    // This is a thin wrapper class that lets us listen to
// IProducerListener::onBufferReleased from mOutput.

    class InputBufferListener : public BnProducerListener,
      public IBinder::DeathRecipient
    {
      public:
        virtual ~InputBufferListener();
        // From IProducerListener
        virtual void onBufferReleased();
        // From IBinder::DeathRecipient
        virtual void binderDied(const wp<IBinder> &who);

    };



};

using namespace android;
class RecordingCameraBuffer : public BufferQueue::ConsumerListener
{

  private:
    VideoTransmitter *mTx;
    virtual void onFrameAvailable(const BufferItem& item);
    virtual void onBuffersReleased();
    virtual void onSidebandStreamChanged();
    uint32_t height;
    uint32_t width;

  public:
    virtual ~RecordingCameraBuffer();
    RecordingCameraBuffer(uint32_t height, uint32_t width) {
      this->height = height;
      this->width = width;
      CRITICAL3("RecordingCameraBuffer Height = %d, width =%d", height, width);
    }
    void createqueuebuffer(uint32_t height, uint32_t width);
    void setClient(VideoTransmitter *target) {
      mTx = target;
    }
    void setHeight(uint32_t height ) {
      this->height = height;
      CRITICAL2("RecordingCameraBuffer height =%d", height);
    }
    void setWidth(uint32_t width ) {
      this->width = width;
      CRITICAL2("RecordingCameraBufferwidth =%d", width);
    }

};

class OmxTransmitterObserver : public BnOMXObserver
{
  private:
    VideoTransmitter *mTarget;
  public:

    OmxTransmitterObserver() {
    }

    void setCodec(VideoTransmitter *target) {
      mTarget = target;
    }
    virtual void onMessages(const std::list<omx_message> &messages) {
      if (mTarget != NULL) {
        mTarget->onMessages(messages);
      }
    }
    virtual void registerBuffers(const sp<IMemoryHeap> & /*mem*/) {}

  protected:
    virtual ~OmxTransmitterObserver() {}


};

}
;

#endif //  __VIDEOTRANSMITTER_H__
