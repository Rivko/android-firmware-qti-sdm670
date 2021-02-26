/*****************************************************************************
============================
Copyright (c)  2010-2012,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
 File: VideoReceiver.h
 Description: Implementation class for VideoReceiver

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-Dec-11   Rakesh K       First Version
 1-Dec-16   Sanjeev Mittal      Rearchiecture Version
 *****************************************************************************/

#ifndef __VIDEORECEIVER_H__
#define __VIDEORECEIVER_H__

#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
//#include <qpVideoCommon.h>
#include <VideoOmx.h>
#include <VTPlayer.h>
//#include <qpVideo.h>
#include <media/IOMX.h>
#include <binder/MemoryDealer.h>
//#include "CommandQueue.h"
//#include <media/stagefright/MediaBuffer.h>
#include <media/MediaCodecBuffer.h>

#ifdef FAREND_WITH_SURFACETEXTURE
#include <gui/SurfaceTextureClient.h>
#endif

#include <gui/Surface.h>

//define component names
#ifdef MSM8937
#define H263_VIDEO_DECODER    (char*)"OMX.qti.video.decoder.h263sw\0"
#else
#ifndef MSM8909
#define H263_VIDEO_DECODER      (char*)"OMX.qcom.video.decoder.h263\0"
#else
#define H263_VIDEO_DECODER      (char*)"OMX.ittiam.video.decoder.h263\0"
#endif
#endif

#define MPEG4_VIDEO_DECODER   (char*)"OMX.qcom.video.decoder.mpeg4\0"
#define H264_VIDEO_DECODER      (char*)"OMX.qcom.video.decoder.avc\0"
#define HEVA_VIDEO_DECODER      (char*)"OMX.qcom.video.decoder.hevc\0"

namespace android
{

struct VideoEncBufferInfo {

  IOMX::buffer_id buffer_id;
  void *pBuffer;
  sp<IMemory> mMemory;
  size_t alloclen;
  size_t filllen;
  ssize_t offset;
  uint32_t  pBufferSize;
  boolean usedbyIOMX;
  sp<GraphicBuffer> mGraphicBuffer;
  int fencefd;

  //MediaBuffer *mMediaBuffer;
};


class VideoReceiver: public VTPlayer, public VideoOmx
{


  public:
    VideoReceiver();
    virtual ~VideoReceiver();
    //virtual QPE_VIDEO_ERROR Init();
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void* pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG* pCodecConfig);
    virtual QPE_VIDEO_ERROR DeInit();
    virtual QPE_VIDEO_ERROR Start();
    virtual QPE_VIDEO_ERROR Stop();
    virtual QPE_VIDEO_ERROR Pause()   {
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR Resume()  {
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig);
    virtual QPE_VIDEO_ERROR TearDown();

    virtual void ClearScreen();
    QPE_VIDEO_ERROR MovetoLoadedState();
    void MovetoExecutingState();

    QPE_VIDEO_ERROR VideoPlayFrame(QP_MULTIMEDIA_FRAME* pFrame,
                                   int belongs_to_drop_set);
    QPE_VIDEO_ERROR ReConfigurePort(); //make it as private
    void ReleaseMediaBuffersAfterReconfigure();
    QPE_VIDEO_ERROR CropPort() ;
    void QPVIDEOOMXFreeBuffers();
    virtual QPE_VIDEO_ERROR DecoderFreeHandle(void);

#ifdef FAREND_WITH_SURFACETEXTURE
    static void SetSurface(sp<SurfaceTexture> surfaceFar);
#else
    static void SetSurface(const sp<IGraphicBufferProducer> bufferProducer);
#endif
    QPE_VIDEO_ERROR UpdateNalHeader(
      uint8_t* pVolHeader,
      uint16_t pVolHeaderLen);

#ifndef ANDROID_VT_JB
    void SetNativeWindow(sp<ANativeWindow> surfaceFar);
#endif
    status_t cancelBufferToNativeWindow(int index);
    void HandleEvent(void* data);
    QPE_VIDEO_ERROR ReleaseOutputPort();
    QPE_VIDEO_ERROR EnableOutputPort();

    // encoder Helper functions
    void IOMXVideoDecEventHandler(sp<IOMXNode> node, OMX_EVENTTYPE eEvent,
                                  OMX_U32 Data1, OMX_U32 Data2);
    void IOMXVideoDecEmptyBufferDone(sp<IOMXNode> node,
                                     IOMX::buffer_id buffer);
    void IOMXVideoDecFillBufferDone(sp<IOMXNode>  node, IOMX::buffer_id buffer,
                                    OMX_U32 range_offset, OMX_U32 range_length, OMX_U32 flags,
                                    OMX_TICKS timestamp //,OMX_PTR platform_private, OMX_PTR data_ptr,OMX_U32 pmem_offset
                                   );

    //Added for IOMX
    void onMessages(const std::list<omx_message> &messages);

    int dequeueBufferFromNativeWindow(ANativeWindow *anw,
                                      ANativeWindowBuffer** anb);
    int VerifyFillBuffersDone();
    void FillNativeBuffer();

  protected:
    // declaration here

  private:
    //Member functions
    //VIDEOCOMPONENTS* m_hRxComponents;
    void* m_pFramesFromApp2DPL;
    void* m_pFramesFromDpl2UI;
    uint8_t m_iMinInputBufCount;
    uint8_t m_iMinOutputBufCount;
    uint8_t m_iVideoDisplayBufferCount;
    int m_bVolStatus;
    volatile OMX_STATETYPE m_IOMXState;
    //CommandQueue fill_buffer;
    boolean iCodecChanged;
    boolean m_codecConfigured;
    sp<ANativeWindow> mNativeWindow;
    uint32_t m_RenderFrameCount;

    VideoEncBufferInfo* m_pDecInpBufHeader_iomx[MAX_BUFFERS];
    VideoEncBufferInfo* m_pDecOutpBufHeader_iomx[MAX_BUFFERS];
    enum eVideoState iPlayerState;

    //private member functions
    QPE_VIDEO_ERROR VideoPlayFrameEx();
    QPE_VIDEO_ERROR VideoPlayVOLFrame();
    QPE_VIDEO_ERROR ConfigureDecoder(QP_VIDEO_CONFIG tCodecConfig);
    void RenderFrame(void* data);

    QPE_VIDEO_ERROR EnablePort(uint8_t iPort);
    QPE_VIDEO_ERROR VideoPlayVOLFrame(uint8_t* pVolFrame, uint16_t iVolLen);

    int AddToFrameStateMap(QP_MULTIMEDIA_FRAME* pFrameData,
                           int belongs_to_drop_set, long long int app_tstamp);
    int DropFrameCheck(long long int app_tstamp, uint8* cvo_info,
                       int8 *retreive_index);
};

class OmxReceiverObserver : public BnOMXObserver
{
  private:
    VideoReceiver *mTarget;
  public:

    OmxReceiverObserver() {
    }

    void setCodec(VideoReceiver *target) {
      mTarget = target;
    }
    virtual void onMessages(const std::list<omx_message> &messages) {
      if (mTarget != NULL) {
        mTarget->onMessages(messages);
      }
    }
    virtual void registerBuffers(const sp<IMemoryHeap> &/*mem*/) {}

  protected:
    virtual ~OmxReceiverObserver() {}


};


}
;

#endif //  __VIDEORECEIVER_H_
