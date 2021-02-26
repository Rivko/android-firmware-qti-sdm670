/*****************************************************************************

============================
Copyright (c)  2010-2012,2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoReceiver.h
 Description: Implementation of VideoReceiver

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-Dec-11   Rakesh K             First Version
 26-jun-13  Sandeep P R           modified version
 *****************************************************************************/
#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
#include <VideoReceiver.h>
//#include <qpCircularBuffer.h>
//#include <VideoMgr.h>
#include <media/IMediaPlayerService.h>

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <media/IMediaPlayerService.h>
//#include <media/stagefright/foundation/ADebug.h>
//#include <media/stagefright/foundation/AString.h>
//#include <media/stagefright/MediaDefs.h>
//#include <media/stagefright/OMXClient.h>
//#include <HardwareAPI.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/OMXBuffer.h>
#include <media/IOMX.h>
#include <binder/MemoryDealer.h>
#include <OMX_Component.h>//needed to include this....
#include <semaphore.h>
#include <pthread.h>
#include <gui/Surface.h>
#include <binder/IMemory.h>
#include <OMX_QCOMExtns.h>
//#include <gralloc_priv.h>
#include <cutils/properties.h>
#if defined(MSM8974) || defined(MSM8226)
#include <media/msm_media_info.h>
#endif
#ifdef FAREND_WITH_SURFACETEXTURE
#include <gui/SurfaceTextureClient.h>
#endif
//#include "CVO_rtp_api.h"

#ifdef UNIQUE_FILE_NO
#undef UNIQUE_FILE_NO
#define UNIQUE_FILE_NO 103
#endif
#include <hidlmemory/mapping.h>
#include <android/hidl/allocator/1.0/IAllocator.h>

typedef hidl::allocator::V1_0::IAllocator TAllocator;
typedef ::android::hidl::memory::V1_0::IMemory TMemory;


#define MAX_FRAME_RATE 30
/********************************************************
     Externing the semaphore variable declared in
     imsthinclient to handle the video play frame case
     when the player is in busy state
*********************************************************/


int gpu_running_behind = 0;

namespace android
{

Mutex reciverstateMutex; //syncs change of stae events
Mutex portChangeMutex; //syncs change of port events

boolean reconfiguring = false;
int fillbufferdones = 0;
boolean firstframe = true;

/* Global  Mutex data */
pthread_mutex_t event_resp = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t event_resp_cond = PTHREAD_COND_INITIALIZER;
pthread_t event_handler;

int buffer_id;
boolean surfaceChanged = false;
sem_t buffer_release_semaphore;
sem_t state_semaphore;
sem_t port_settings_semaphore;
sem_t disable_port_semaphore;
int deinit_rx = QP_FALSE;

int Releasing_state = 0;
int Wait_for_Buffer_Release =
  0; // Flag to decide whether VerifyFillBuffersDone should signal the wait semaphore

int tempfillbuffercallbackcount = 0;
int tempemptybuffercallbackcount = 0;
int temprendercount = 0;

extern "C" FILE *fpRawRecv;
extern "C" char storeDecoded[PROPERTY_VALUE_MAX];
FILE *fpEncRecv;
char storeEncoded[PROPERTY_VALUE_MAX];


//DISPLAY_BUFFER m_pBuffer; // used in the fillbufferdone callback function instead of malloc
#ifdef FAREND_WITH_SURFACETEXTURE
static sp<SurfaceTexture> g_surfaceFar = NULL;
#else
sp<IGraphicBufferProducer> g_surfaceFar = NULL;
#endif

#ifndef ANDROID_VT_JB
sp<ANativeWindow> mNativeWindowSurfaceFar;
#endif


static sp<IMemoryHeap> m_pMemInfo; //Smart pointer for Memory
static sp<MemoryDealer> m_mDealer[2];
static sp<IAllocator> mAllocator[2];
static sp<IOMXNode> m_NodeId;
//static IOMX::node_id m_NodeId;
static sp<IBinder> mNodeBinder;
static sp<TMemory> InputHidlmem[MAX_BUFFERS];


static FrameStateMap frameStateSet[FRAME_STATE_SET_SIZE];

uint8_t prev_cvo_info;




static uint32 frameStateIndex = 0;

#define CHECK_EQ(x,y)   if(x != y){ CRITICAL2_STR("Videoreceiver::SOMETHING WENT WRONG AT LINE %d",__LINE__); }

#define CHECK(hr) if(hr != VIDEO_ERROR_OK){ CRITICAL3_STR("Check failed in function %s at line %d",__func__,__LINE__); return VIDEO_ERROR_UNKNOWN;}

static QPE_VIDEO_ERROR sem_timedwait()
{
  struct timespec ts;
  int result;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 3;
  result = sem_timedwait(&state_semaphore, &ts);

  if (result) {
    ERROR4("Failure in [%s:%d] result = %d", __func__, __LINE__, result);
    return VIDEO_ERROR_UNKNOWN;
  }

  return VIDEO_ERROR_OK;
}

/*******************************************************************************
 Constructor for VideoReceiver
 ********************************************************************************/
VideoReceiver::VideoReceiver()
{
  m_pRxVolInfo = NULL;
  m_pRxNalInfo = NULL;
  //m_hRxComponents = NULL;
  m_pFramesFromApp2DPL = NULL;
  m_pFramesFromDpl2UI = NULL;
  m_pFramesFromApp2DPL = NULL;
  m_pFramesFromDpl2UI = NULL;
  mNativeWindow = nullptr;
  m_iMinInputBufCount = 0;
  m_iMinOutputBufCount = 0;
  m_bVolStatus = QP_FALSE;
  m_IOMXState = OMX_StateInvalid;
  m_codecConfigured = QP_FALSE;
  iCodecChanged = QP_FALSE;
  gpu_running_behind = 0;
  iPlayerState = eVideoStateNotReady;
  m_RenderFrameCount = 0;
  Wait_for_Buffer_Release = 0;
  memset(&m_codecConfig, 0, sizeof(QP_VIDEO_CONFIG) );
}

/*******************************************************************************
 Destructor for VideoReceiver
 ********************************************************************************/
VideoReceiver::~VideoReceiver()
{
  MINIMAL1("In VideoReceiver::~VideoReceiver in dtor");
  DecoderFreeHandle();
}


template<class Template> static void InitDecoderOmxParams(Template *omxparams)
{
  omxparams->nVersion.s.nVersionMajor = 1;
  omxparams->nVersion.s.nVersionMinor = 0;
  omxparams->nVersion.s.nRevision = 0;
  omxparams->nVersion.s.nStep = 0;
  omxparams->nSize = sizeof(Template);
}

int VideoReceiver::DropFrameCheck(long long int app_ts, uint8* cvo_info,
                                  int8 *retreive_index)
{
  uint64_t app_tstamp = (uint64_t) app_ts;

  for (int i = 0; i < FRAME_STATE_SET_SIZE; i++) {
    DEBUG5_STR(
      "DropFrameCheck belongs_to_drop_set %d  frameStateIndex %d source_time_video_frame %u, app_tstamp_in_us %llu",
      frameStateSet[i].belongs_to_drop_set, i,
      frameStateSet[i].source_time_video_frame,
      (unsigned long long)frameStateSet[i].app_tstamp_in_us);

    if ((frameStateSet[i].app_tstamp_in_us == app_tstamp) ||
        ((frameStateSet[i].app_tstamp_in_us / 1000) == (app_tstamp / 1000))) {
      QP_VIDEO_PLAYOUT_INFO playoutInfo;
      memset(&playoutInfo, 0, sizeof(playoutInfo));
      playoutInfo.version = 4;
      playoutInfo.source_time_video_frame = frameStateSet[i].source_time_video_frame;
      playoutInfo.source_rtp_timestamp = frameStateSet[i].rtp_tstamp;
      playoutInfo.system_time_decode_video_frame =
        frameStateSet[i].system_time_decode_video_frame;
      playoutInfo.ucFrameNalType = frameStateSet[i].ucFrameNalType;
      playoutInfo.ulFrameSize   = frameStateSet[i].ulFrameSize;
      prev_cvo_info = *cvo_info = frameStateSet[i].cvo_info;
      *retreive_index = i;

      if (frameStateSet[i].belongs_to_drop_set) {
        playoutInfo.system_time_render_video_frame = 0;
      } else {
        timeval tv;
        gettimeofday(&tv, NULL);
        playoutInfo.system_time_render_video_frame = (uint64_t)(getModemTime() +
            ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - getAppsTime());
      }

      LOGPACKET(LOG_VIDEO_PLAYOUT_C, &playoutInfo, sizeof(playoutInfo));
      return frameStateSet[i].belongs_to_drop_set;
    }
  }

  *retreive_index = -1;
  *cvo_info = prev_cvo_info;
  CRITICAL2("DropFrameCheck:: Outside for loop didnt  match any index returning prev_cvo_info - %d",
            prev_cvo_info);
  return QP_FALSE;
}

int VideoReceiver::AddToFrameStateMap(QP_MULTIMEDIA_FRAME* pFrameData,
                                      int belongs_to_drop_set, long long int app_tstamp)
{
  /* Get current time */
  timeval tv;
  gettimeofday(&tv, NULL);
  MEDIA_PACKET_INFO_RX* pRxInfo = &(pFrameData->sMediaPacketInfo.sMediaPktInfoRx);
  int nIdx = frameStateIndex;
  /* Memsetting the array */
  memset(&frameStateSet[frameStateIndex], 0, sizeof(FrameStateMap));
  frameStateSet[nIdx].belongs_to_drop_set  = belongs_to_drop_set;
  frameStateSet[nIdx].ucFrameNalType       = pFrameData->pData[4];
  frameStateSet[nIdx].ulFrameSize          = pFrameData->iDataLen;
  frameStateSet[nIdx].source_time_video_frame = pRxInfo->source_time_video_frame;
  frameStateSet[nIdx].system_time_decode_video_frame = getModemTime() +
      TIMEVAL2MS(tv) -
      getAppsTime();
  frameStateSet[nIdx].app_tstamp_in_us = app_tstamp;
  frameStateSet[nIdx].seqno      = pRxInfo->iSeqNum;
  frameStateSet[nIdx].rtp_tstamp = pRxInfo->source_rtp_timestamp;
  frameStateSet[nIdx].cvo_info   = pFrameData->cvoInfo;
  prev_cvo_info = pFrameData->cvoInfo;
  DEBUG6_STR("AddToFrameStateMap dropset %d seqno %d frameStateIndex %d source_time_video_frame %u, app_tstamp_in_us %llu",
             belongs_to_drop_set, pRxInfo->iSeqNum, frameStateIndex,
             pRxInfo->source_time_video_frame,
             (unsigned long long)frameStateSet[frameStateIndex].app_tstamp_in_us);
  frameStateIndex = (frameStateIndex + 1) % FRAME_STATE_SET_SIZE;
  return frameStateIndex;
}

// Fill a YVU420SemiPlanar buffer with a black used to clear  the surface at call end
void fillYVU420SemiPlanarBlankBuffer(uint8_t* buf, int w, int h, int /*stride*/)
{
  if (buf != NULL) {
    memset((void*)&buf[0],
           0x10, w * h);
    memset((void*)(&buf[0] + w * h), 0x80, w * h / 2);
  }
}



void VideoReceiver::ClearScreen()
{
  ANativeWindowBuffer *buf;
  int err1;
  //LOGE("ClearScreen:: DEQUEUE BUFFER");
  /*err1 = mNativeWindow.get()->dequeueBuffer(mNativeWindow.get(),
      &buf);*/ //deprericated in jb_2.5
  err1 = dequeueBufferFromNativeWindow(mNativeWindow.get(), &buf);
  //ERROR2("VideoReceiver::ClearScreen::HandleEvent inside for loop with err value %d", err1);

  if (err1 != 0) {
    ERROR3("VideoReceiver::ClearScreen::HandleEvent dequeueBuffer failed: %s (%d)",
           strerror(-err1), -err1);
  } else {
    EVENT1("ClearScreen Dequeue sucessful\n");
  }

  if (err1 == 0) {
    //LOGE("RenderingISSUE:: DEQUEUE BUFFER DONE");
    uint8_t i = 0;

    for (; i < m_iMinOutputBufCount; i++) {
      if (m_pDecOutpBufHeader_iomx[i]
          && m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
        DEBUG4_STR(
          "m_pDecOutpBufHeader_iomx handle comparision %p %p %d",
          m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->handle,
          buf->handle, i);

        if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->handle
            == buf->handle) {
          break;
        }
      }
    }

    if (i == m_iMinOutputBufCount) {
      CRITICAL1("VideoReceiver::HandleEvent ERROR quering fillBuffer");
      return;
    }

    if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer == NULL) {
      return;
    }

    // Get the number of buffers needed.
    OMX_PARAM_PORTDEFINITIONTYPE def;
    memset(&def, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    def.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    def.nVersion.s.nVersionMajor = 1;
    def.nVersion.s.nVersionMinor = 0;
    def.nVersion.s.nRevision = 0;
    def.nVersion.s.nStep = 0;
    InitDecoderOmxParams(&def);
    def.nPortIndex = DIR_OUT;
    err1 = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                  &def, sizeof(def));

    if (err1 != OK) {
      ERROR2("ClearScreen OMX_IndexParamPortDefinition getparam error %x", err1);
      return;
    }

    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;

    if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
      // Fill the buffer with the a blank buffer
      uint8_t* img = NULL;
      m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->lock(GRALLOC_USAGE_SW_WRITE_OFTEN,
          (void**)(&img));
      fillYVU420SemiPlanarBlankBuffer(img, def.format.video.nStride,
                                      def.format.video.nSliceHeight,
                                      m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->getStride());
      //*img = 0;
      m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->unlock();
      mNativeWindow.get()->queueBuffer(mNativeWindow.get(),
                                       m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->getNativeBuffer(), -1);
      CRITICAL1("VideoReceiver::ClearScreen sucessful\n");
    }

    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
  }
}

int VideoReceiver::dequeueBufferFromNativeWindow(ANativeWindow * nativeWindow,
    ANativeWindowBuffer** buf)
{
  return native_window_dequeue_buffer_and_wait(nativeWindow, buf);
}

void VideoReceiver::FillNativeBuffer()
{
  ANativeWindowBuffer *buf;
  int err1;
  DEBUG1("RenderingISSUE:: DEQUEUE BUFFER");
  //depricated in from jb2.5
  /*err1 = pRx->mNativeWindow.get()->dequeueBuffer(pRx->mNativeWindow.get(),  &buf);*/
  err1 =  dequeueBufferFromNativeWindow(mNativeWindow.get(), &buf);

  if (err1 != 0) {
    ERROR3("VideoReceiver::HandleEvent dequeueBuffer failed: %s (%d)",
           strerror(-err1), -err1);
  } else {
    DEBUG1("RenderingISSUE:: DEQUEUE BUFFER DONE");
    uint8_t i = 0;

    for (; i < m_iMinOutputBufCount; i++) {
      if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
        DEBUG4_STR("m_pDecOutpBufHeader_iomx handle comparision %p %p %d",
                   m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->handle,
                   buf->handle, i);

        if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->handle == buf->handle) {
          break;
        }
      }
    }

    if (i == m_iMinOutputBufCount
        || m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer == NULL) {
      MINIMAL1("VideoReceiver::HandleEvent ERROR quering fillBuffer");
      return;
    }

    CRITICAL4("VideoReceiver::HandleEvent dequeue buffer calling fillBuffer on buffer %d index %d, fencefd: %d",
              m_pDecOutpBufHeader_iomx[i]->buffer_id, i,
              m_pDecOutpBufHeader_iomx[i]->fencefd);
    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;
    status_t err = m_NodeId->fillBuffer(m_pDecOutpBufHeader_iomx[i]->buffer_id,
                                        OMXBuffer::sPreset, m_pDecOutpBufHeader_iomx[i]->fencefd);
    EVENT2("VideoReceiver::HandleEvent :: calling fillBuffer err = %d", err);

    if (err != 0) {
      m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
      ERROR3("FillNativeBuffer:: Fill Buffer[%d] on Decoder return : %d", i, err);
    }
  }
}

/*******************************************************************************
 Set the Surface for Rendering
 ********************************************************************************/
#ifndef FAREND_WITH_SURFACETEXTURE
void VideoReceiver::SetSurface(const sp<IGraphicBufferProducer> bufferProducer)
{
  g_surfaceFar = bufferProducer;
  CRITICAL3_STR("surfaceFar = %p %d", g_surfaceFar.get(), __LINE__);
  surfaceChanged = true;
}
#else

void VideoReceiver::SetSurface(sp<SurfaceTexture> surfaceFar)
{
  g_surfaceFar = surfaceFar;
  CRITICAL3("surfaceFar = %p %d", g_surfaceFar.get(), __LINE__);
  surfaceChanged = true;
}

#endif

void VideoReceiver::SetNativeWindow(sp<ANativeWindow> surfaceFar)
{
  PlayerMsg info;
  mNativeWindowSurfaceFar = surfaceFar.get();
  CRITICAL2_STR("mNativeWindowSurfaceFar = %p", mNativeWindowSurfaceFar.get());
  surfaceChanged = true;

  /*If we are in INIT and SURFACE pending states, then ensure surface is not NULL*/
  if (VIDEO_INIT == m_ePlayerState && mNativeWindowSurfaceFar.get() ) {
    m_ePlayerState = VIDEO_CODEC_PENDING;
    CRITICAL2("Surface State is Ready, m_ePlayerState %d", m_ePlayerState);
  } else if (VIDEO_SURFACE_PENDING == m_ePlayerState &&
             mNativeWindowSurfaceFar.get()) {
    CRITICAL1("Surface State is Ready, Codec already received");
    /* Do not send indications to Modem, as this configure happening internally*/
    //IsEventInternalTriggered(true);//Commenting this out since we set this in the internal codec set as well. Enabling this here will block the player start request from rtp in race condition.
    //! Move to Surface rcvd state so that Configure API will proceed further
    m_ePlayerState = VIDEO_SURFACE_RECEIVED;
    CRITICAL1("Configure API called as playerStart Request received from Modem");
    QPE_VIDEO_ERROR eStatus = StartPlayer();
  } else if (VIDEO_CODEC_CONFIGURED == m_ePlayerState) {
    if ((mNativeWindowSurfaceFar.get() == NULL) &&
        (m_IOMXState == OMX_StateExecuting)) {
      memset(&info, 0, sizeof(PlayerMsg));
      info.type = MOVE_TO_LOADED;
      postEvent(&info);
      m_ePlayerState = VIDEO_SURFACE_PENDING;
      CRITICAL2("Move to Loaded state, m_ePlayerState %d",
                m_ePlayerState);
      return;
    } else if (mNativeWindowSurfaceFar.get() != NULL) {
      m_ePlayerState = VIDEO_CODEC_PENDING;
      CRITICAL1("Move to configured state");
      memset(&info, 0, sizeof(PlayerMsg));
      info.type = MOVE_TO_EXECUTING;
      postEvent(&info);
    }
  }
}

void VideoReceiver::MovetoExecutingState()
{
  if (VIDEO_CODEC_PENDING == m_ePlayerState) {
    CRITICAL1("Move to configured state (new API)");
    IsEventInternalTriggered(QP_TRUE);
    QP_VIDEO_CONFIG tCodecConfig;
    memcpy(&tCodecConfig, &m_codecConfig, sizeof(QP_VIDEO_CONFIG));
    enum eVideoState currstate = iPlayerState;
    QPE_VIDEO_ERROR eStatus = ConfigureDecoder(tCodecConfig);

    if (VIDEO_ERROR_OK == eStatus) {
      m_ePlayerState = VIDEO_CODEC_CONFIGURED;
      CRITICAL2("VideoRx::SetMovetoExecuting ConfigureDecoder Success, m_ePlayerState %d",
                m_ePlayerState);
    }

    if (currstate != eVideoStateStopping) {
      Start();

      if (!m_codecConfig.iPortReconfigSet) {
        /* Posting the vt_playframe semaphore to recover the Dequeue thread
           from waiting state during surface state change */
        CRITICAL1("Videoreceiver::SetMovetoExecuting [vt_playframe]sem_post");
        sem_post_videoplay();
      } else {
        //Handling the Deferred Reconfigure Event
        iPlayerState = eVideoStateReConfiguring;
        m_codecConfig.iPortReconfigSet = QP_FALSE;
        CRITICAL1("Videoreceiver::SetMovetoExecuting pOmxData->iPortReconfigSet is set hence proceed to Reconfigure");
        PlayerMsg info;
        memset(&info, 0, sizeof(PlayerMsg));
        info.type = PORT_DISABLE;
        VideoReceiver::HandleEvent(&info);
      }
    }

    IsEventInternalTriggered(QP_FALSE);
  }
}




//Base class methods
/****************************************************************************************
 Init for VideoReceiver
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                    void* pUserData, QPE_VIDEO_DEV eVDev,
                                    QP_VIDEO_CONFIG* pCodecConfig)
{
  //please never remove these register api call code as it is first step of
  //registration of callback and variables
  RegisterPlayerCallBack(tVideoCallBack, pUserData,  eVDev, pCodecConfig);
  RegisterCallBacks(tVideoCallBack, pUserData, eVDev);
  EVENT1("VideoReceiver::Init Initializing the player ");

  for (uint8_t i = 0; i < MAX_BUFFERS; i++) {
    m_pDecOutpBufHeader_iomx[i] = NULL;
    m_pDecInpBufHeader_iomx[i] = NULL;
  }

  sem_init(&buffer_release_semaphore, 0, 0);
  sem_init(&state_semaphore, 0, 0);
  sem_init(&port_settings_semaphore, 0, 0);
  sem_init(&disable_port_semaphore, 0, 0);
  Releasing_state = 0;
  Wait_for_Buffer_Release = 0;
  deinit_rx = QP_FALSE;
  CRITICAL1("VideoReceiver::Init done");
  return VIDEO_ERROR_OK;
}

void VideoReceiver::HandleEvent(void* data)
{
  if (data == NULL) {
    CRITICAL1("VideoReceiver::HandleEvent VideoReceiver is cleaned up");
    return;
  }

  PlayerMsg* internalplayermsg = (PlayerMsg*) data;
  //we can enable  this message if needed by using debug property.
  DEBUG3("Player msg type = %d playerstate=%d", internalplayermsg->type,
         iPlayerState);
  DEBUG3("VideoReceiver::HandleEvent data = %p deinit = %d",
         data, deinit_rx);

  switch (internalplayermsg->type) {
    case MOVE_TO_LOADED: {
        MINIMAL1("Player msg move to loaded");
        MovetoLoadedState();
        return;
      }
      break;

    case MOVE_TO_EXECUTING: {
        MINIMAL1("Player msg move to executing");
        MovetoExecutingState();
        return;
      }
      break;

    default:
      break;
  }

  //Moved down to ensure we dont miss the MoveToLoadedState
  if (VIDEO_SURFACE_PENDING == m_ePlayerState || (mNativeWindowSurfaceFar.get() == NULL)) {
    CRITICAL1("VideoReceiver::HandleEvent Surface is NULL, ignore this API");
    return;
  }

  if (deinit_rx) {
    MINIMAL1("VideoReceiver::HandleEvent: Loaded state free node");

    if (m_bOmxNodeAllocated == QP_TRUE && m_NodeId->freeNode() != OK) {
      ERROR1("VideoReceiver::HandleEvent :Error in de initializing the components");
    } else {
      CRITICAL1("VideoReceiver::HandleEvent :Not freeing the node since rxnode was not already allocated");
    }

    return;
  }

  switch (iPlayerState) {
    case eVideoStateCropping: {
        CropPort();
      }
      break;

    case eVideoStateReConfiguring: {
        if ((internalplayermsg->type != FILLBUFFER_INFO)
            && (internalplayermsg->type != CROP)) {
          m_codecConfig.iPortReconfigSet = QP_FALSE;

          if (Releasing_state) {
            //Call release function
            ReleaseMediaBuffersAfterReconfigure();
            break;;
          }

          CRITICAL1("VideoReceiver::HandleEvent Calling Reconfigure on Port");
          reconfiguring = 1;
          ReConfigurePort();
          //Wait on Port Disable event
          sem_wait(&disable_port_semaphore);
          ERROR1("VideoReceiver::HandleEvent Calling Enable on Port");
          EnablePort(1);
        }
      }
      break;
#if 0 //We have merged the cases

    case eVideoStateReConfigureDone: {
        if (internalplayermsg->type != FILLBUFFER_INFO) {
          MINIMAL1("VideoReceiver::HandleEvent Calling Enable on Port");
          EnablePort(1);
        }
      }
      break;
#endif

    case eVideoStateActive: {
        if (internalplayermsg->type == PORT_DISABLE) {
          iPlayerState = eVideoStateReConfiguring;
          m_codecConfig.iPortReconfigSet = QP_FALSE;

          if (Releasing_state) {
            //Call release function
            ReleaseMediaBuffersAfterReconfigure();
            break;
          }

          CRITICAL1("VideoReceiver::HandleEvent Calling Reconfigure on Port");
          reconfiguring = 1;
          ReConfigurePort();
          //Wait on Port Disable event
          sem_wait(&disable_port_semaphore);
          ERROR1("VideoReceiver::HandleEvent Calling Enable on Port");
          EnablePort(1);
          return;
        } else if (internalplayermsg->type == CROP) {
          iPlayerState = eVideoStateCropping;
          CropPort();
          return;
        }

        RenderFrame(data);
      }
      break;

    case eVideoStateStopping: {
        FillBufferInfo* info = (FillBufferInfo*) (&internalplayermsg->data.fillbuffer);

        if ((info == NULL) || (info->index == (uint8)(-1))
            || (internalplayermsg->type != FILLBUFFER_INFO)
            || (info->index > MAX_BUFFERS - 1)) {
          MINIMAL1("VideoReceiver:: FillBufferInfo is null");
          return;
        }

        uint8_t index = info->index;
        CRITICAL2("VideoReceiver:: HandleEvent eVideoStateStopping FillBufferInfo index = %d",
                  index);

        if (!m_pDecOutpBufHeader_iomx[index]) {
          CRITICAL2("VideoReceiver:: HandleEvent eVideoStateStopping m_pDecOutpBufHeader_iomx[%d] - Null hence returning",
                    index);
          return;
        }

        IOMX::buffer_id bufferptr = m_pDecOutpBufHeader_iomx[index]->buffer_id;
#ifdef ANDROID_VT_JB

        if (g_surfaceFar == NULL) {
          CRITICAL1("VideoReceiver:: HandleEvent surface is NULL");
          return;
        }

#else

        if (mNativeWindowSurfaceFar == NULL) {
          CRITICAL1("VideoReceiver:: HandleEvent surface is NULL");
          return;
        }

#endif

        if (m_IOMXState == OMX_StateExecuting) {
          if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer != NULL) {
            CRITICAL2("Handleevent: eStopping buffer Cancelled %d", index);
            cancelBufferToNativeWindow(index);
            FillNativeBuffer();
          }
        }
      }
      break;

    default:
      break;
  }

  return;
}

/****************************************************************************************
 DeInit for VideoReceiver
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::DeInit()
{
  if (eVideoStateSurfaceReleasing != iPlayerState &&
      eVideoStateReleasing !=  iPlayerState) {
    ERROR2("VideoReceiver::DeInit API called, iPlayerState %d", iPlayerState);
    iPlayerState = eVideoStateReleasing;

    if (TearDown() != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::DeInit::Failed to Teardown decoder- Forcing player to eVideoStateNotReady");
      iPlayerState = eVideoStateNotReady;
      return VIDEO_ERROR_UNKNOWN;
    }
  } else {
    ERROR2("VideoReceiver::DeInit:: Player already released, iPlayerState %d",
           iPlayerState);
  }

  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Start Recording for videoReceiver
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::Start()
{
  EVENT1("In VideoReceiver::Start::starting the VideoReceiver");
  uint8 v_firstframe[20];
#ifdef ANDROID_VT_JB
  char level[1];
#else
  char level[PROPERTY_VALUE_MAX];
#endif
  int result = property_get("VT_PROFILE_LEVEL", level, "");
  EVENT2("VT_PROFILE_LEVEL %c", level[0]);
  CONTROLLOG("PlayerStateTransition:Player Start iPlayerState:%d, OMX_CommandStateSet:%d, m_ePlayerState %d",
             iPlayerState, m_IOMXState, m_ePlayerState);
  enum eVideoState prevState = iPlayerState;

  if (prevState == eVideoStateActive) {
    CRITICAL1("Component is already in Executing state hence returning");
    SendEventsToThinclient(VIDEO_MSG_PLAYER_STARTED);
    return VIDEO_ERROR_OK;
  }
  /* If Codec is not yet configured, then send dummy indication to Upper Layer*/
  else if (VIDEO_INIT == m_ePlayerState ||
           VIDEO_SURFACE_PENDING == m_ePlayerState || (mNativeWindowSurfaceFar.get() == NULL)) {
    m_ePlayerState = VIDEO_SURFACE_PENDING;
    m_bPlayerStartReq = true;
    CRITICAL2("Component not configured, send Dummy VIDEO_MSG_PLAYER_STARTED, m_ePlayerState %d",
              m_ePlayerState);
    SendEventsToThinclient(VIDEO_MSG_PLAYER_STARTED);
    return VIDEO_ERROR_PLAYER_DOWN;
  } else if (VIDEO_SURFACE_RECEIVED == m_ePlayerState) {
    m_bPlayerStartReq = true;
    CRITICAL2("Component not configured, send Dummy VIDEO_MSG_PLAYER_STARTED, m_ePlayerState %d",
              m_ePlayerState);
    CRITICAL1("configure: is already in Q without Start Req, Push another config req into Q with Player start Req");
    SendEventsToThinclient(VIDEO_MSG_PLAYER_STARTED);
    StartPlayer(m_codecConfig);
    return VIDEO_ERROR_PLAYER_DOWN;
  }

  //
  m_bPlayerStartReq = false;

  if (m_IOMXState == OMX_StateExecuting) {
    CRITICAL1("Component is already in Executing state hence returning");
    // Bug here the expectation is the state moves to active when we
    iPlayerState = eVideoStateActive;
    MINIMAL2("PlayerStateTransition:Player:VIDEO_MSG_PLAYER_STARTED:%d",
             iPlayerState);
    SendEventsToThinclient(VIDEO_MSG_PLAYER_STARTED);

    if ((iCodecChanged ) && (m_codecConfig.eCodec == VIDEO_CODEC_H264)) {
      // This log is not needed as we are logging in playvolframe
      //CRITICAL1("Feeding Nal Frame");
      iCodecChanged = QP_FALSE;
      VideoPlayVOLFrame();
    }

    return VIDEO_ERROR_OK;
  } else if (m_IOMXState == OMX_StateIdle) {
    if (m_NodeId->sendCommand(OMX_CommandStateSet,
                              OMX_StateExecuting) != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::Start::::Failure in insrting the command for state EXECUTING\n");
      return VIDEO_ERROR_UNKNOWN;
    }

    CHECK(sem_timedwait());
  }

  if (m_IOMXState != OMX_StateExecuting) {
    CRITICAL2("Component not in Executing state and breaking from the loop %d",
              m_IOMXState);
    return VIDEO_ERROR_UNKNOWN;
  } else {
    CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d",
              m_IOMXState);
  }

  if (prevState == eVideoStateIdle) {
    // Quering the buffers needed for decoding and rendering
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    int err = -1;
    memset(&portdef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion.s.nVersionMajor = 1;
    portdef.nVersion.s.nVersionMinor = 0;
    portdef.nVersion.s.nRevision = 0;
    portdef.nVersion.s.nStep = 0;
    InitDecoderOmxParams(&portdef);
    portdef.nPortIndex = DIR_OUT;
    err = m_NodeId->getParameter( OMX_IndexParamPortDefinition,
                                  &portdef, sizeof(portdef));

    if (err != OK) {
      CRITICAL2("ConfigureDecoder OMX_IndexParamPortDefinition getparam error %x",
                err);
      return VIDEO_ERROR_UNKNOWN;
    }

    //Feed only the minimum amount of buffers to Decoder; other extra buffers allocated are handed back to Native Window
    for (uint8_t i = 0; i < portdef.nBufferCountMin; i++) {
      timeval tv;
      gettimeofday(&tv, NULL);
      CRITICAL3_STR("TIMEDELAY::VideoReceiver::Start call frame = %d, time = %llu",
                    tempfillbuffercallbackcount + 1,
                    (unsigned long long)TIMEVAL2MS(tv));

      if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
        m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;
        CRITICAL4("Start:: FILL BUFFER index %d buffer = %d fencefd : %d",
                  i, m_pDecOutpBufHeader_iomx[i]->buffer_id,
                  m_pDecOutpBufHeader_iomx[i]->fencefd);
        status_t err = m_NodeId->fillBuffer(m_pDecOutpBufHeader_iomx[i]->buffer_id,
                                            OMXBuffer::sPreset,
                                            m_pDecOutpBufHeader_iomx[i]->fencefd);

        if (err != 0) {
          m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
        }
      }
    }
  }

  iPlayerState =
    eVideoStateActive; // Bug here the expectation is the state moves to active when we

  if (((prevState == eVideoStateIdle) || iCodecChanged ) &&
      (m_codecConfig.eCodec == VIDEO_CODEC_H264
       || m_codecConfig.eCodec == VIDEO_CODEC_H265 )) {
    iCodecChanged = QP_FALSE;
    VideoPlayVOLFrame();
  }

  CRITICAL2("PlayerStateTransition:Player:VIDEO_MSG_PLAYER_STARTED:%d",
            iPlayerState);
  SendEventsToThinclient(VIDEO_MSG_PLAYER_STARTED);
  return VIDEO_ERROR_OK;
}

void VideoReceiver::RenderFrame(void* data)
{
  PlayerMsg* internalplayermsg = (PlayerMsg*) data;

  if (eVideoStateActive != iPlayerState || !data) {
    return;
  }

  FillBufferInfo* info = (FillBufferInfo*) (&internalplayermsg->data.fillbuffer);

  if ((info == NULL) || (info->index == (uint8)(-1)) ||
      (internalplayermsg->type != FILLBUFFER_INFO)
      || (info->index > MAX_BUFFERS - 1)) {
    MINIMAL1("VideoReceiver:: FillBufferInfo is null");
    return;
  }

  uint8_t index = info->index;
  IOMX::buffer_id bufferptr = m_pDecOutpBufHeader_iomx[index]->buffer_id;
  //this print can be leveerage from previous updated print which is below
  DEBUG2("RenderingISSUE:: QUEUE BUFFER used %d", gpu_running_behind);
  int err2 =  mNativeWindow.get()->query(mNativeWindow.get(),
                                         NATIVE_WINDOW_CONSUMER_RUNNING_BEHIND,
                                         &gpu_running_behind);

  if (err2 != 0) {
    ERROR3("VideoReceiver::query of native window for buffers failed with error %s (%d)",
           strerror(-err2), -err2);
    return;
  }

  //ALOGE("RenderingISSUE:: QUEUE BUFFER RUNNING_BEHIND updated %d", gpu_running_behind);
  MINIMAL4("RenderingISSUE:: QUEUE BUFFER index = %d drop_set =%d gpu_running_behind =%d",
           index, info->belongs_to_drop_set, gpu_running_behind);

  if (gpu_running_behind == 0 && (!info->belongs_to_drop_set)) {
    CRITICAL2("RenderingISSUE:: QUEUE BUFFER index %d", index);
    int err1 = mNativeWindow.get()->queueBuffer(mNativeWindow.get(),
               m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer->getNativeBuffer(), -1);

    if (err1 != 0) {
      ERROR2("VideoReceiver::IOMXVideoDecFillBufferDone queueBuffer failed w/ error 0x%08x",
             err1);
    } else {
      //if it is okay then we can only print using DEBUG
      DEBUG2("RenderingISSUE:: QUEUE BUFFER DONE index %d", index);
    }
  } else {
    CRITICAL2("RenderingISSUE: QUEUE BUFFER Cancelled %d", index);
    cancelBufferToNativeWindow(index);
  }

#ifdef ANDROID_VT_JB

  if (g_surfaceFar == NULL) {
    ERROR1("VideoReceiver:: HandleEvent surface is NULL");
    return;
  }

#else

  if (mNativeWindowSurfaceFar == NULL) {
    ERROR1("VideoReceiver:: HandleEvent surface is NULL");
    return;
  }

#endif
  m_RenderFrameCount++;
  timeval tv;
  gettimeofday(&tv, NULL);
  DEBUG1("VideoReceiver HandleEvent::IOMXVideoDecFillBufferDone Called Render");

  //fill this buffer callingin the OMX context to fill the buffer in a loop
  if (iPlayerState == eVideoStateActive && !m_codecConfig.iErrorState) {
    timeval tv;
    gettimeofday(&tv, NULL);
    DEBUG4_STR("TIMEDELAY::VideoDecFillBuffer call frame queuebuffer = %d, time = %llu, render frame = %d",
               tempfillbuffercallbackcount + 1, (unsigned long long)TIMEVAL2MS(tv),
               m_RenderFrameCount);

    if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer != NULL) {
      FillNativeBuffer();
    }
  } else {
    CRITICAL1("VideoReceiver::HandleEvent :: player state is not active and returning without filling the buffer");
  }

  return;
}

QPE_VIDEO_ERROR VideoReceiver::ReleaseOutputPort()
{
  EVENT1("VideoReceiver::ReleaseOutputPort");
  status_t omxresult;
  iPlayerState = eVideoStatePortDisabling;
  m_NodeId->sendCommand(OMX_CommandFlush, 1);
  reconfiguring = true;
  EVENT1("VideoReceiver::ReleaseOutputPort Wait on a signal");

  if (!VerifyFillBuffersDone()) {
    sem_wait(&buffer_release_semaphore);
    EVENT1("VideoReceiver::ReleaseOutputPort Received a signal");
  }

  m_NodeId->sendCommand(OMX_CommandPortDisable, 1);

  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
#ifdef VDEC_EXTRA_DATA

    if (m_pDecOutpBufHeader_iomx[i]->pBuffer) {
      CRITICAL2("UNMAP Memory pBuffer = %d", i);
      munmap(m_pDecOutpBufHeader_iomx[i]->pBuffer,
             m_pDecOutpBufHeader_iomx[i]->pBufferSize);
    }

#endif
    omxresult = m_NodeId->freeBuffer(1,
                                     m_pDecOutpBufHeader_iomx[i]->buffer_id);
    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;

    if (omxresult != OMX_ErrorNone) {
      CRITICAL2(
        "VideoReceiver::ReleaseOutputPort:: Failed to free the output buffers. error %x",
        omxresult);
      return VIDEO_ERROR_UNKNOWN;
    }

    if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
      EVENT4(
        "VideoReceiver::ReleaseOutputPort line no %d setobserver for %p to null index %d",
        __LINE__, m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer.get(), i);

      if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != 0) {
        cancelBufferToNativeWindow(i);
      } else {
        ERROR1("Graphics buffer is null");
      }

      DEBUG1("VideoReceiver::ReleaseOutputPort release media buffer");
      m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer = NULL;
    }
  }

  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoReceiver::EnableOutputPort()
{
  EVENT1("VideoReceiver::EnableOutputPort");
  EVENT2("VideoReceiver::EnableOutputPort surfaceFar = %p", g_surfaceFar.get());
#ifndef ANDROID_VT_JB
  mNativeWindow = mNativeWindowSurfaceFar.get();
#else

  if (g_surfaceFar != 0) {
#ifdef FAREND_WITH_SURFACETEXTURE
    mNativeWindow = new SurfaceTextureClient(g_surfaceFar);
#else
    mNativeWindow = new Surface(g_surfaceFar);
#endif
  }

#endif
  return EnablePort(1);
}

/****************************************************************************************
 Stop Recording for videoReceiver
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::Stop()
{
  CRITICAL2("PlayerStateTransition:PlayerStop API iPlayerState %d", iPlayerState);

  if (iPlayerState == eVideoStateStopping) {
    ERROR1("Component is already in Stopping state hence returning");
    return VIDEO_ERROR_OK;
  }

  /* If Decoder is not yet configured, then nothing to do with decoder. Stop immediately*/
  if (VIDEO_CODEC_CONFIGURED != m_ePlayerState) {
    CRITICAL2("PlayerStateTransition:Player:Stop: m_ePlayerState %d Codec not yet configured",
              m_ePlayerState);
    SendEventsToThinclient(VIDEO_MSG_PLAYER_STOPPED);
    return VIDEO_ERROR_OK;
  }

  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);
#if 0

  if (m_NodeId->sendCommand(OMX_CommandStateSet, OMX_StatePause)
      != VIDEO_ERROR_OK) {
    DEBUG1(
      "VideoReceiver::Start::::Failure in insrting the command for state EXECUTING\n");
    return VIDEO_ERROR_UNKNOWN;
  }

  CHECK(sem_timedwait());
  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d",
            m_IOMXState);

  if (m_IOMXState != OMX_StatePause) {
    DEBUG1("Component in Executing state and breaking from the loop");
    return VIDEO_ERROR_UNKNOWN;
  }

#endif
  iPlayerState = eVideoStateStopping;
  /*If surface is set to NULL, intermittently then this flag will help to call
    start() API*/
  m_bPlayerStartReq = true;
  CRITICAL2("PlayerStateTransition:Player:VIDEO_MSG_PLAYER_STOPPED:%d",
            iPlayerState);
  SendEventsToThinclient(VIDEO_MSG_PLAYER_STOPPED);
  return VIDEO_ERROR_OK;
}

int VideoReceiver::VerifyFillBuffersDone()
{
  TRACE1("VideoReceiver::VerifyFillBuffersDone");
  uint8_t i = 0;

  for (i = 0; i < m_iMinOutputBufCount; i++) {
    if (m_pDecOutpBufHeader_iomx[i]
        && m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL
        && m_pDecOutpBufHeader_iomx[i]->usedbyIOMX == QP_TRUE) {
      CRITICAL3("VideoReceiver::VerifyFillBuffersDone  returning QP_FALSE used by %d buffer_id = %d",
                i, m_pDecOutpBufHeader_iomx[i]->buffer_id);
      return QP_FALSE;
    }
  }

  if (i == m_iMinOutputBufCount) {
    MINIMAL1("VideoReceiver::VerifyFillBuffersDone returning QP_TRUE ");
    return QP_TRUE;
  } else {
    MINIMAL1(
      "VideoReceiver::VerifyFillBuffersDone returning QP_FALSE here 2  ");
    return QP_FALSE;
  }
}

QPE_VIDEO_ERROR VideoReceiver::UpdateNalHeader(
  uint8_t* pVolHeader,
  uint16_t pVolHeaderLen)
{
  CRITICAL3("VideoReceiver::UpdateNalHeader pVolHeader : %p pVolHeaderLen : %d",
            pVolHeader, pVolHeaderLen);

  if (pVolHeader != NULL && pVolHeaderLen > 0) {
    memset(m_pRxNalInfo->rawnal, 0, VIDEOOMX_MAX_NAL_LENGTH - 1);
    memcpy(m_pRxNalInfo->rawnal, pVolHeader,
           pVolHeaderLen);
    m_pRxNalInfo->nalLength = (uint32_t) pVolHeaderLen;
  }

  return VIDEO_ERROR_OK;
}

/****************************************************************************************
 Used for configuring of the IOMX components and ports of videoReceiver
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::Configure(QP_VIDEO_CONFIG tCodecConfig)
{
  EVENT1("VideoReceiver::Configure");
  static OMX_CALLBACKTYPE VideoDecCallback;
  status_t err = OK;
  int isReconfiguring = QP_FALSE;
  uint8_t nal_local[] = { 0, 0, 0, 1, 103, 66, 192, 13, 233, 2, 131, 242, 0, 0, 0, 1, 104, 206, 6, 226 };
  enum eVideoState prevState;
  QPE_VIDEO_ERROR eStatus;
  UpdateCodecConfigInfo(&tCodecConfig);

  if (VIDEO_INIT == m_ePlayerState ||
      VIDEO_SURFACE_PENDING == m_ePlayerState) {
    m_ePlayerState = VIDEO_SURFACE_PENDING;
    CRITICAL2("VideoRx: Waiting for Surface, m_ePlayerState %d", m_ePlayerState);
    //! Send Dummy response to Modem
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
    return VIDEO_ERROR_OK;
  }

  prevState = iPlayerState;
  MINIMAL2("PlayerStateTransition:Player:CodecSet:%d:VIDEO_MSG_CODEC_CHANGED",
           iPlayerState);

  if ((iPlayerState == eVideoStateActive)
      || (iPlayerState == eVideoStateStopping)
      || (iPlayerState == eVideoStateIdle)
     ) {
    //Transition the codec to Loaded state and perform the codec set all over again
    if (tCodecConfig.eCodec != m_ePrevCodec) {
      MovetoLoadedState();
      iPlayerState = eVideoStateNotReady;
    }
  }

  if (iPlayerState == eVideoStateIdle) {
    MINIMAL2("VideoReceiver::Configure MovetoLoadedState current state : %d",
             iPlayerState);
    MovetoLoadedState();
    MINIMAL2("VideoReceiver::Configure ReConfigureDecoder current state : %d",
             iPlayerState);
    eStatus = ConfigureDecoder(tCodecConfig);

    if (VIDEO_ERROR_OK == eStatus) {
      m_ePlayerState = VIDEO_CODEC_CONFIGURED;
      CRITICAL2("VideoReceiver::Configure ReConfigureDecoder m_ePlayerState %d",
                m_ePlayerState);
    } else {
      return VIDEO_ERROR_UNKNOWN;
    }

    MINIMAL2("PlayerStateTransition:Player:VIDEO_MSG_CODEC_CHANGED:%d",
             iPlayerState);
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
    return VIDEO_ERROR_OK;
  }

  MINIMAL2("PlayerStateTransition:Player:VIDEO_MSG_CODEC_CHANGED:%d",
           iPlayerState);

  //We feed in the new SPS/PPS information for a port reconfigure
  if (iPlayerState == eVideoStateActive) {
    VideoPlayVOLFrame();
    m_codecConfig.iErrorState = 0;
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
    return VIDEO_ERROR_OK;
  } else if ((iPlayerState == eVideoStateStopping)
             || (iPlayerState == eVideoStateReConfiguring)
             || (iPlayerState == eVideoStateReConfigureDone)
             || (iPlayerState == eVideoStatePortDisabling)
             || (iPlayerState == eVideoStatePortDisabled)
             || (iPlayerState == eVideoStateCropping)) {
    //flag is used for signalling to initiate the feeding of new SPS/PPS when codec
    // moves to active state
    iCodecChanged = QP_TRUE;
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
    return VIDEO_ERROR_OK;
  }

  //check for the component availability
  EVENT3("VideoReceiver::Configure:: codec type is %d, frameRate %d",
         tCodecConfig.eCodec, tCodecConfig.iFrameRate);
  /* This variable used to check whether component needs to be Loaded state or
     not, before calling ConfigureDecoder API */
  m_ePrevCodec = tCodecConfig.eCodec;

  if (tCodecConfig.eCodec == VIDEO_CODEC_H263) {
    if (VerifyComponentName(H263_VIDEO_DECODER) != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::Configure:: H263_VIDEO_DECODER component is not available");
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
      || tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
    if (VerifyComponentName(MPEG4_VIDEO_DECODER) != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::Configure:: MPEG4_VIDEO_DECODER component is not available");
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_H264) {
    DEBUG1("VIDEO_CODEC_H264");

    if (VerifyComponentName(H264_VIDEO_DECODER) != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::Configure:: H264_VIDEO_DECODER component is not available");
      return VIDEO_ERROR_UNKNOWN;
    }
  } else if (tCodecConfig.eCodec == VIDEO_CODEC_H265) {
    CRITICAL1("VIDEO_CODEC_HEVC");

    if (VerifyComponentName(HEVA_VIDEO_DECODER) != VIDEO_ERROR_OK) {
      ERROR1("VideoReceiver::Configure:: HEVA_VIDEO_DECODER component is not available");
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  iPlayerState = eVideoStateNotReady;

  if (!isReconfiguring) {
    sp<OmxReceiverObserver> observer = new OmxReceiverObserver;
    observer.get()->setCodec(this);

    if (tCodecConfig.eCodec == VIDEO_CODEC_H263) {
      err = m_OMX_handle->allocateNode(H263_VIDEO_DECODER, observer,
                                       &m_NodeId);

      if (err != OK) {
        ERROR2("VideoReceiver::Configure:: Error getting the decoder component handle for H263 codec Err Nor %d\n",
               err);
        iPlayerState = eVideoStateNotReady;
        ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d",
               iPlayerState);
        SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
        return VIDEO_ERROR_UNKNOWN;
      }
    } else if (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
               || tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
      err = m_OMX_handle->allocateNode(MPEG4_VIDEO_DECODER, observer,
                                       &m_NodeId);

      if (err != OMX_ErrorNone) {
        ERROR2("VideoReceiver::Configure:: Error getting the decoder component handle for MPEG4 codec Err Nor %d\n",
               err);
        iPlayerState = eVideoStateNotReady;
        ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d",
               iPlayerState);
        SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
        return VIDEO_ERROR_UNKNOWN;
      }
    } else if (tCodecConfig.eCodec == VIDEO_CODEC_H264) {
      MINIMAL1("Alloctae node Component :VIDEO_CODEC_H264  \n");
      err = m_OMX_handle->allocateNode(H264_VIDEO_DECODER, observer,
                                       &m_NodeId);

      if (err != OMX_ErrorNone) {
        ERROR2("VideoReceiver::Configure:: Error getting the decoder component handle for H264 codec Err Nor %d\n",
               err);
        iPlayerState = eVideoStateNotReady;
        ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d",
               iPlayerState);
        SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
        return VIDEO_ERROR_UNKNOWN;
      }

      OMX_VIDEO_PARAM_PORTFORMATTYPE def;
      memset(&def, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
      InitDecoderOmxParams(&def);
      def.nPortIndex = DIR_OUT;
      status_t err = m_NodeId->getParameter(OMX_IndexParamVideoPortFormat,
                                            &def, sizeof(def));

      if (err != OK) {
        ERROR2("VideoReceiver::ConfigureDecoder - setup video Renderer: Could not get port definition color type; defaulting. error %x",
               err);
        //def.eColorFormat =
        //(OMX_COLOR_FORMATTYPE) QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka;
      }

      EVENT2("VideoReceiver::ConfigureDecoder - VIDEO RENDERER COLOR FORMAT IS 0x%x",
             def.eColorFormat);

      //CRITICAL2("surfaceFar = %p", g_surfaceFar.get());
      if (g_surfaceFar != NULL) {
        surfaceChanged = false;
      }
    } else if (tCodecConfig.eCodec == VIDEO_CODEC_H265) {
      CRITICAL1("Alloctae node Component :VIDEO_CODEC_HEVC  \n");
      err = m_OMX_handle->allocateNode(HEVA_VIDEO_DECODER, observer,
                                       &m_NodeId);
//      CRITICAL2("aFTER Alloctae node Component :VIDEO_CODEC_HEVC -%d \n", m_NodeId);

      if (err != OMX_ErrorNone) {
        ERROR2("VideoReceiver::Configure:: Error getting the decoder component handle for H265 codec Err Nor %d\n",
               err);
        iPlayerState = eVideoStateNotReady;
        ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d ",
               iPlayerState);
        SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
        return VIDEO_ERROR_UNKNOWN;
      }
    }
  }

  // Updating the class boolean variable to true once the allocate node is successful
  m_bOmxNodeAllocated = QP_TRUE;

  //Configure the decoder
  if (ConfigureDecoder(tCodecConfig) != VIDEO_ERROR_OK) {
    EVENT1("VideoReceiver::Configure:: Failed in decoder configuration\n");
    //Send the error message to the RTP
    iPlayerState = eVideoStateNotReady;
    ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d",
           iPlayerState);
    SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
    return VIDEO_ERROR_UNKNOWN;
  }

  if (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO
      || tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID) {
    if (m_pRxVolInfo) {
      if ((m_pRxVolInfo->volLength <= VIDEOOMX_MAX_VOL_LENGTH)
          && (VideoPlayVOLFrame(m_pRxVolInfo->rawVol, m_pRxVolInfo->volLength)
              != VIDEO_ERROR_OK)) {
        EVENT1("VideoReceiver::Configure::Error in playing the VOL frame\n");
        //Send the error message to the RTP
        iPlayerState = eVideoStateNotReady;
        ERROR2("PlayerStateTransition:Player:VIDEO_MSG_CONFIGURE_ERROR:%d",
               iPlayerState);
        SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
        return VIDEO_ERROR_UNKNOWN;
      }
    } else {
      ERROR1("VideoReceiver::Configure:: VOL frame is NULL and is not played out in first empty buffer\n");
    }
  }

  m_codecConfigured = QP_TRUE;
  m_ePlayerState = VIDEO_CODEC_CONFIGURED;
  MINIMAL3("PlayerStateTransition:Player:VIDEO_MSG_CODEC_CHANGED:%d, m_ePlayerState %d",
           iPlayerState, m_ePlayerState);
  SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);

  if ((prevState != eVideoStateNotReady) && (prevState != eVideoStateStopping)) {
    CRITICAL1("Move to configured state");
    IsEventInternalTriggered(QP_TRUE);
    Start();
    IsEventInternalTriggered(QP_FALSE);
  }

  return VIDEO_ERROR_OK;
}




// encoder Helper functions
void VideoReceiver::IOMXVideoDecEventHandler(sp<IOMXNode> /*node*/,
    OMX_EVENTTYPE eEvent, OMX_U32 Data1, OMX_U32 Data2)
{
  CRITICAL2("VideoReceiver::IOMXVideoDecEventHandler:: In decoder event handler callback eEvent = %d \n",
            eEvent);
  int data;

  if (eEvent == OMX_EventCmdComplete) {
    OMX_COMMANDTYPE iCommandReceived = (OMX_COMMANDTYPE) Data1;
    OMX_STATETYPE iStateReceived = (OMX_STATETYPE) Data2;
    CRITICAL3(
      "VideoReceiver::IOMXVideoDecEventHandler:: In decoder event handler callback cmd = %d  state =%d\n",
      iCommandReceived, iStateReceived);

    if (iCommandReceived == OMX_CommandStateSet) {
      m_IOMXState = iStateReceived;

      if (iStateReceived == OMX_StateIdle) {
        EVENT2(
          "VideoReceiver::IOMXVideoDecEventHandler:: component is in the state IDLE and the player state is %d\n",
          iPlayerState);

        if (iPlayerState == eVideoStateReleasing) {
          EVENT1("VideoReceiver::IOMXVideoDecEventHandler:: component is in the state eVideoStateReleasing and notifying the VIDEO_MSG_DEV_UNINITIALISED event\n");
        } else if (iPlayerState != eVideoStateReleasing) {
          iPlayerState = eVideoStateIdle;
        }

        sem_post(&state_semaphore);
      } else if (iStateReceived == OMX_StateLoaded) {
        EVENT2("VideoReceiver::IOMXVideoDecEventHandler:: component is in the LOADED state and player state is %d\n",
               iPlayerState);

        if (iPlayerState == eVideoStateReleasing) {
          EVENT1("VideoReceiver::IOMXVideoDecEventHandler:: setting the state as eVideoStateNotReady and resining the VIDEO_MSG_DEV_UNINITIALISED event\n");
          iPlayerState = eVideoStateNotReady;
        }

        sem_post(&state_semaphore);
      } else if (iStateReceived == OMX_StateExecuting) {
        EVENT2("VideoReceiver::IOMXVideoDecEventHandler:: component is in the state Executing and player state is %d\n",
               iPlayerState);
#if 0  // already done in Start
        iPlayerState = eVideoStateActive;
#endif
        sem_post(&state_semaphore);
      } else if (iStateReceived == OMX_StatePause) {
        EVENT1(
          "VideoReceiver::IOMXVideoDecEventHandler:: component is in the  Paused State");
        sem_post(&state_semaphore);
      } else if (iStateReceived == OMX_StateInvalid) {
        EVENT1(
          "VideoReceiver::IOMXVideoDecEventHandler:: Component is in wrong state and returning\n");
        return;
      }
    }

    if (iCommandReceived == OMX_CommandPortDisable) {
      EVENT2("VideoReceiver::IOMXVideoDecEventHandler:: Received OMX_CommandPortDisable event and the player state is %d\n",
             iPlayerState);
      MINIMAL2("OMX_CommandPortDisable:%d", m_IOMXState);

      if (iPlayerState != eVideoStateReleasing
          && iPlayerState == eVideoStateReConfiguring) {
#if 0       //we dont need this intermediate state
        iPlayerState = eVideoStateReConfigureDone;
#endif
        MINIMAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandPortDisable:%d",
                 m_IOMXState);
        MINIMAL2("PlayerStateTransition:Player:VIDEO_INTERNAL_EVENT_STATUS_MSG:%d",
                 iPlayerState);
#if 0 // We are handling player reconfigure in one context itself without a CmdQ break so no need to post an event we signal the semaphore
        PlayerMsg info;
        memset(&info, 0, sizeof(PlayerMsg));
        info.type = PORT_ENABLE;
        CONTROLLOG("PlayerStateTransition:OMX_Decoder:OMX_CommandPortDisable:%d",
                   m_IOMXState, 0, 0);
        CONTROLLOG("PlayerStateTransition:Player:VIDEO_INTERNAL_EVENT_STATUS_MSG:%d",
                   iPlayerState, 0, 0);
        postEvent(&info);
#endif
        sem_post(&disable_port_semaphore);
      } else {
        sem_post(&port_settings_semaphore);
      }
    }

    if (iCommandReceived == OMX_CommandPortEnable) {
      EVENT1(
        "VideoReceiver::IOMXVideoDecEventHandler:: Received OMX_CommandPortEnable event\n");
      sem_post(&port_settings_semaphore);
      MINIMAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandPortEnable:%d",
               m_IOMXState);
#if 0 // We will perform this transition in enable Port
      iPlayerState = eVideoStateActive;
#endif
      MINIMAL2("PlayerStateTransition:Player:VIDEO_INTERNAL_EVENT_STATUS_MSG:%d",
               iPlayerState);
      //No need to send any event here and it is just the decoder reconfiguration
    }
  } else if (eEvent == OMX_EventPortSettingsChanged) {
    EVENT2("VideoReceiver::IOMXVideoDecEventHandler:: OMX_EventPortSettingsChanged event received and dynamic port reconfiguration required and player state is %d\n",
           iPlayerState);
    OMX_INDEXTYPE iIndexReceived = (OMX_INDEXTYPE) Data1;
    OMX_STATETYPE iStateReceived = (OMX_STATETYPE) Data2;
    MINIMAL2("VideoReceiver::IOMXVideoDecEventHandler iStateReceived : %x",
             iStateReceived);

    if (iStateReceived == (OMX_STATETYPE)OMX_IndexConfigCommonOutputCrop) {
      CRITICAL1("VideoReceiver::IOMXVideoDecEventHandler Cropping ");

      if (iPlayerState == eVideoStateActive) {
        iPlayerState = eVideoStateCropping;
        m_codecConfig.iCropSet =
          QP_TRUE; //To avoid race condition when crop event is received during enable port
        PlayerMsg info;
        memset(&info, 0, sizeof(PlayerMsg));
        info.data.fillbuffer.index = (uint8)(-1);
        info.type = CROP;
        postEvent(&info);
      } else {
        CRITICAL2("Ignoring crop as player is in state %d but updating CropSe",
                  iPlayerState);
        m_codecConfig.iCropSet =
          QP_TRUE;
      }
    } else if (iStateReceived == (OMX_STATETYPE)OMX_IndexParamPortDefinition) {
      CRITICAL1("VideoReceiver::IOMXVideoDecEventHandler Reconfiguring updating m_codecConfig.iPortReconfigSet ");
      iPlayerState = eVideoStateReConfiguring;
      m_codecConfig.iPortReconfigSet = QP_TRUE;
      PlayerMsg info;
      memset(&info, 0, sizeof(PlayerMsg));
      info.data.fillbuffer.index = (uint8)(-1);
      info.type = PORT_DISABLE;
      postEvent(&info);
    } else {
      MINIMAL2("VideoReceiver::IOMXVideoDecEventHandler got status %d from OMX which is not handled",
               iStateReceived);
    }
  } else if (eEvent == OMX_EventPortFormatDetected) {
    //nothing to be done here
  } else if (eEvent == OMX_EventError) {
    MINIMAL2("VideoReceiver::IOMXVideoDecEventHandler::OMX error received with error type %x\n",
             (OMX_ERRORTYPE) Data1);
    m_codecConfig.iErrorState = 1;
    SendEventsToThinclient(VIDEO_MSG_ERROR);
  }

  return;
}


void VideoReceiver::ReleaseMediaBuffersAfterReconfigure()
{
  timeval tv;
  status_t omxresult;

  if (Releasing_state == 0) {
    return;
  }

  Releasing_state = 0;

  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
    if (m_pDecOutpBufHeader_iomx[i]) {
#ifdef VDEC_EXTRA_DATA

      if (m_pDecOutpBufHeader_iomx[i]->pBuffer) {
        CRITICAL2("UNMAP Memory pBuffer = %d", i);
        munmap(m_pDecOutpBufHeader_iomx[i]->pBuffer,
               m_pDecOutpBufHeader_iomx[i]->pBufferSize);
      }

#endif
      omxresult = m_NodeId->freeBuffer(1,
                                       m_pDecOutpBufHeader_iomx[i]->buffer_id);
      m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;

      if (omxresult != OMX_ErrorNone) {
        ERROR2("VideoReceiver::ReConfigurePort:: Failed to free the output buffers. error %x",
               omxresult);
        Releasing_state = 0;
        return;
      }

      if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
        EVENT3(
          "VideoReceiver::ReConfigurePort line no %d setobserver to null index %d",
          __LINE__, i);

        if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
          cancelBufferToNativeWindow(i);
        } else {
          ERROR1("Graphics buffer is null");
        }

        EVENT1("media buffer release called from reconfigure port");
        m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer = NULL;
      }
    }
  }

  gettimeofday(&tv, NULL);
  CRITICAL2("TIMEDELAY::eVideoStateReConfiguring end time = %lu",
            ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));

  /** Free the Descriptors alocated for IOMX */
  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
    if (m_pDecOutpBufHeader_iomx[i] != NULL) {
      free(m_pDecOutpBufHeader_iomx[i]);
      m_pDecOutpBufHeader_iomx[i] = NULL;
    }
  }
}
void VideoReceiver::IOMXVideoDecEmptyBufferDone(sp<IOMXNode> node,
    IOMX::buffer_id buffer)
{
  uint8_t i = 0;
  EVENT1(
    "VideoReceiver::IOMXVideoDecEmptyBufferDone:: In Decoder DecEmptyBufferDone callback\n");

  if (buffer == 0 || node.get() == NULL) {
    return;
  }

  tempemptybuffercallbackcount++;
  timeval tv;
  gettimeofday(&tv, NULL);
  CRITICAL4_STR("Profiling Decoder->IMS EmptyBufferDone frame = %d, appstime = %lu buffer=%d",
                tempemptybuffercallbackcount,
                ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)), buffer);

  for (i = 0; i < m_iMinInputBufCount; i++) {
    if (m_pDecInpBufHeader_iomx[i]->buffer_id == buffer) {
      m_pDecInpBufHeader_iomx[i]->filllen = 0;
      break;
    }
  }

  DEBUG2("VideoReceiver :: IOMXVideoDecEmptyBufferDone id  = %d", buffer);
  return;
}
void VideoReceiver::IOMXVideoDecFillBufferDone(sp<IOMXNode> /*node*/,
    IOMX::buffer_id buffer, OMX_U32 /*range_offset*/, OMX_U32 range_length,
    OMX_U32 /*flags*/, OMX_TICKS timestamp
    /*, OMX_PTR platform_private,OMX_PTR data_ptr, OMX_U32 pmem_offset*/)
{
  DEBUG1(
    "VideoReceiver::IOMXVideoDecFillBufferDone:: In Decoder VideoDecFillBufferDone callback");
  PlayerMsg info;
  OMX_U8* data = NULL;
  tempfillbuffercallbackcount++;
  timeval tv;
  gettimeofday(&tv, NULL);
  CRITICAL5_STR("Profiling Decoder->IMS FillBufferDone frame = %d, appstime = %lu buffer=%d FBD Time - %llu",
                tempfillbuffercallbackcount,
                ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)), buffer, timestamp);
  //CRITICAL2("DEBUGGING OMX ISSUE FBD time %lu",timestamp);
  DEBUG3("VideoReceiver::IOMXVideoDecFillBufferDone:: Received the decoded frame with frame len= %u and player state is %d",
         range_length, iPlayerState);
  //TO avoid calling showframeondisplay in case of decoded frae is invalid
  uint8_t i = 0;

  while (i < m_iMinOutputBufCount
         && m_pDecOutpBufHeader_iomx[i]->buffer_id != buffer) {
    ++i;
  }

  if (i == m_iMinOutputBufCount) {
    CRITICAL1("VideoReceiver::no buffers available");
    return;
  }

  if (m_pMemInfo != NULL) {
    OMX_U8* base = (OMX_U8*) m_pMemInfo->getBase();
    data = base + 0;//pmem_offset;
  }

  EVENT2("range_length = %u", range_length);
  char *data1 = (char *) m_pDecOutpBufHeader_iomx[i]->pBuffer;
  buffer_id = i;
  DEBUG4("VideoReceiver::IOMXVideoDecFillBufferDone:: buffer id : %d, buffer = %d, m_iMinOutputBufCount : %d",
         buffer_id, buffer, m_iMinOutputBufCount);
  m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;

  if ((iPlayerState == eVideoStatePortDisabling)
      || (iPlayerState == eVideoStateReConfiguring)) {
    DEBUG2(
      "IOMXVideoDecFillBufferDone in Reconfiguring | eVideoStatePortDisabling state fillbufferdones : %d",
      fillbufferdones);

    if (VerifyFillBuffersDone()) {
      DEBUG1(" IOMXVideoDecFillBufferDone Post a signal");
      reconfiguring = 0;
#if 0 // We no longer need to repost DISABLE as we are waiting on Fill buffer done Semaphore
      PlayerMsg info;
      memset(&info, 0, sizeof(PlayerMsg));
      info.data.fillbuffer.index = (uint8)(-1);
      info.type = PORT_DISABLE;
      postEvent(&info);
#endif

      if (Wait_for_Buffer_Release) {
        CRITICAL1("VideoReceiver::VerifyFillBuffersDone Posting buffer_release_semaphore");
        Wait_for_Buffer_Release = 0;
        sem_post(&buffer_release_semaphore);
      }
    }
  } else {
    if (iPlayerState != eVideoStateActive && iPlayerState != eVideoStateCropping) {
      CRITICAL1("Cancelling buffer and Returning as we are not going to process");

      if (iPlayerState != eVideoStateStopping) {
        cancelBufferToNativeWindow(i);
      }

      //If Decoder is in active state then we need to pass the FillBuffer
      if ((iPlayerState == eVideoStateStopping)
          && ( m_IOMXState == OMX_StateExecuting)) {
        uint8 cvo_info;   //cvo info has to be used for rotation and flip
        int8 retrieve_index = -1;
        memset(&info, 0, sizeof(PlayerMsg));
        info.type = FILLBUFFER_INFO;
        info.data.fillbuffer.index = i;
        info.data.fillbuffer.belongs_to_drop_set = DropFrameCheck(timestamp, &cvo_info,
            &retrieve_index );
        postEvent(&info);
        CRITICAL3("Posting the FillBufferInfo in Decoder state : %d index : %d",
                  iPlayerState, i);
      }

      return;
    }

    if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer == NULL
        || m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer == 0) {
      CRITICAL1("VideoReceiver::media buffers are null");
      return;
    }

    //FillBufferInfo info;
    memset(&info, 0, sizeof(PlayerMsg));
    info.type = FILLBUFFER_INFO;
    //info.data.fillbuffer = (FillBufferInfo *) info.data;
    uint8 cvo_info;   //cvo info has to be used for rotation and flip
    int8 retrieve_index = -1;
    info.data.fillbuffer.index = i;
    info.data.fillbuffer.belongs_to_drop_set = DropFrameCheck(timestamp, &cvo_info,
        &retrieve_index);

    if ( iPlayerState == eVideoStateCropping ) {
      info.data.fillbuffer.belongs_to_drop_set = 1;
    }

    CRITICAL5("VideoReceiver CVO info %x drop_set=%d and index=%d frame_state_map index = %d",
              cvo_info, info.data.fillbuffer.belongs_to_drop_set, info.data.fillbuffer.index,
              retrieve_index);

    if (storeDecoded[0] == '1' || storeDecoded[0] == '3') {
      /*code for YUV Dumps*/
      if (fpRawRecv != NULL) {
#if defined(MSM8974) || defined(MSM8226)
        int k, msize;
        int stride = VENUS_Y_STRIDE(COLOR_FMT_NV12, m_codecConfig.iWidth);
        int scanlines = VENUS_Y_SCANLINES(COLOR_FMT_NV12, m_codecConfig.iHeight);
        unsigned char* pvirt, *ptemp;
        char* temp = (char*)m_pDecOutpBufHeader_iomx[i]->pBuffer;
        msize = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, m_codecConfig.iWidth,
                                  m_codecConfig.iHeight);

        for (k = 0; k <  m_codecConfig.iHeight; k++) {
          fwrite(temp, m_codecConfig.iWidth, 1, fpRawRecv);
          temp += stride;
        }

        temp = (char*)m_pDecOutpBufHeader_iomx[i]->pBuffer + (stride * scanlines);

        for (k = 0; k <  m_codecConfig.iHeight / 2; k++) {
          fwrite(temp, m_codecConfig.iWidth, 1, fpRawRecv);
          temp += stride;
        }

#endif
      }
    }

#ifdef VDEC_EXTRA_DATA

    /*extract conceal MB information from the extradata buffer*/
    if (flags & OMX_BUFFERFLAG_EXTRADATA) {
      OMX_OTHER_EXTRADATATYPE *p_extra = NULL;
      OMX_U8 *pBuffer = (OMX_U8 *)(m_pDecOutpBufHeader_iomx[i]->pBuffer) +
                        range_offset;

      if (pBuffer != NULL && range_length != 0) {
        MINIMAL3(" range_offset=%lu and range_length =%lu", range_offset, range_length);
        p_extra = (OMX_OTHER_EXTRADATATYPE *)
                  ((unsigned)(pBuffer + range_offset + range_length + 3) & (~3));
        MINIMAL3(" pBuffer : %p p_extra : %p", pBuffer, p_extra);

        while (p_extra &&  p_extra->eType != OMX_ExtraDataNone) {
          switch ((OMX_EXTRADATATYPE)p_extra->eType) {
            case OMX_ExtraDataConcealMB: {
                OMX_U8 *data_ptr = (OMX_U8 *)p_extra->data;
              }
              break;

            case OMX_ExtraDataFrameInfo: {
                OMX_QCOM_EXTRADATA_FRAMEINFO *frame_info = (OMX_QCOM_EXTRADATA_FRAMEINFO *)
                    p_extra->data;
                CRITICAL2("OMX_ExtraDataFrameInfo nConcealedMacroblocks %d",
                          frame_info->nConcealedMacroblocks);
                info.data.fillbuffer.concealmb = frame_info->nConcealedMacroblocks;
              }
              break;

            default:
              break;
          }

          p_extra = (OMX_OTHER_EXTRADATATYPE *)(((OMX_U8 *)p_extra) + p_extra->nSize);
        }
      }
    }

#endif

    if (!info.data.fillbuffer.belongs_to_drop_set) {
      int transform;

      //Match the right
      switch (cvo_info & 0x03) {
        case CVO_ZERO_ROTATION:
          transform = 0;
          break;

        case CVO_90_ROTATION:
          transform = HAL_TRANSFORM_ROT_90;
          break;

        case CVO_180_ROTATION:
          transform = HAL_TRANSFORM_ROT_180;
          break;

        case CVO_270_ROTATION:
          transform = HAL_TRANSFORM_ROT_270;
          break;

        default:
          transform = 0;
          break;
      }

      switch (cvo_info & 0x04) {
        case CVO_FLIP_ENABLED:

          /* since flip is applied before the rotation
          we need to change the angles to compensate it*/
          if (transform == HAL_TRANSFORM_ROT_90) {
            transform = HAL_TRANSFORM_ROT_270;
          } else if (transform == HAL_TRANSFORM_ROT_270) {
            transform = HAL_TRANSFORM_ROT_90;
          }

          transform ^= HAL_TRANSFORM_FLIP_H ;
          break; //Flip horizontal

        default:
          break;
      }

      CRITICAL2("VideoReceiver transform info %x", transform);
      status_t err = OK;
//        if (transform) {
      err = native_window_set_buffers_transform(
              mNativeWindow.get(), transform);

      if (err != OK) {
        CRITICAL3_STR("native_window_set_buffers_transform result: %s (%d)",
                      strerror(-err), -err);
      }

//        }
    }

    EVENT2("info is %p", &info);
    postEvent(&info);
  }

  return;
}

status_t VideoReceiver::cancelBufferToNativeWindow(int index)
{
  DEBUG3("VideoReceiver--- in function %s line no %d", __func__, __LINE__);

  //EVENT2("Calling cancelBuffer on buffer %p", info->buffer_id);
  if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer == NULL) {
    CRITICAL1("VideoReceiver: media buffer is null");
    return -1;
  }

  int err = mNativeWindow.get()->cancelBuffer(mNativeWindow.get(),
            m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer.get(), -1);

  if (err != 0) {
    EVENT2("cancelBuffer failed w/ error 0x%08x", err);
    return err;
  }

  return 1;
}

/**************************************************************************************************************************************
 VideoReceiver::VideoPlayFrame::Receives the frames from the Upper layer and gives it to the decoder to decode
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoReceiver::VideoPlayFrame(QP_MULTIMEDIA_FRAME* pFrame,
    int belongs_to_drop_set)
{
  DEBUG3("VideoPlayFrame Player State : %d error state : %d",
         iPlayerState, m_codecConfig.iErrorState);

  if (pFrame == NULL) {
    return VIDEO_ERROR_UNKNOWN;
  }

  if (m_codecConfig.iErrorState) {
    CRITICAL2("VideoReceiver::VideoPlayFrame Returning VIDEO_ERROR_UNKNOWN bcs of OMX Error State %d",
              m_codecConfig.iErrorState );
    return VIDEO_ERROR_UNKNOWN;
  }

  if (eVideoStateActive == iPlayerState) {
    uint8_t i = 0;
    int index = -1;
    long long int etbtime = 0;

    for (; i < m_iMinInputBufCount; i++) {
      if (m_pDecInpBufHeader_iomx[i]) {
        if (m_pDecInpBufHeader_iomx[i]->filllen == 0) {
          timeval tv;
          gettimeofday(&tv, NULL);
          EVENT2("TIMEDELAY::emptyBuffer start time = %lu",
                 ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));
          memcpy(m_pDecInpBufHeader_iomx[i]->pBuffer,
                 pFrame->pData, pFrame->iDataLen);
          m_pDecInpBufHeader_iomx[i]->filllen = pFrame->iDataLen;
          EVENT2(
            "VideoReceiver::VideoPlayFrameEx:; emptying the buffer with buffer len = %lu",
            (unsigned long)m_pDecInpBufHeader_iomx[i]->filllen);
          gettimeofday(&tv, NULL);
          CRITICAL5(
            "IMS->Decoder EmpytBuffer call frame = %d, appstime = %lu emptyBuffer = %d len=%lu",
            tempemptybuffercallbackcount + 1,
            ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)),
            m_pDecInpBufHeader_iomx[i]->buffer_id,
            (unsigned long)m_pDecInpBufHeader_iomx[i]->filllen);
          etbtime = ((long long int)(((long long int)tv.tv_sec) * 1000000) + tv.tv_usec);
          CRITICAL4("VideoReceiver :: emptyBuffer = %u tv.tv_sec = %d tv.tv_usec %d",
                    m_pDecInpBufHeader_iomx[i]->buffer_id, tv.tv_sec, tv.tv_usec);
          index = AddToFrameStateMap(pFrame, belongs_to_drop_set,
                                     etbtime);
          CRITICAL4_STR("DEBUGGING OMX ISSUE ETB time %llu add_index : %d cvoinfo of this frame : %d",
                        etbtime,
                        index,
                        pFrame->cvoInfo);
          DEBUG1("RenderingISSUE:: EMPTY BUFFER");
          OMXBuffer InputBuff(0, pFrame->iDataLen);
          int error = m_NodeId->emptyBuffer(
                        m_pDecInpBufHeader_iomx[i]->buffer_id,
                        InputBuff,
                        0,
                        etbtime, -1);

          if (error != OK) {
            ERROR2(
              "VideoReceiver::VideoPlayFrameEx:: Failed to empty the buffer 1 %d", error);
            return VIDEO_ERROR_UNKNOWN;
          }

          gettimeofday(&tv, NULL);
          EVENT2("TIMEDELAY::emptyBuffer end time = %lu",
                 ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));
          return VIDEO_ERROR_OK;
        }
      }
    }

    if (i == m_iMinInputBufCount) {
      CRITICAL1("VideoReceiver :: No empty buffers left in iomx input port");
      return VIDEO_ERROR_PLAYER_BUSY;
    }
  } else {
    CRITICAL2(
      "VideoReceiver::VideoPlayFrame:: player state is %d not active and dropping the frame",
      iPlayerState);
    return VIDEO_ERROR_PLAYER_BUSY;
  }

  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoReceiver::ConfigureH263Decoder :: Port configuration for the component H263 video decoder
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoReceiver::ConfigureDecoder(QP_VIDEO_CONFIG tCodecConfig)
{
  TRACE1("VideoReceiver::ConfigureDecoder");
  status_t omxresult = OK;
  OMX_PORT_PARAM_TYPE portFmt;
  OMX_STATETYPE state = OMX_StateInvalid;
  memset(&portFmt, 0, sizeof(OMX_PORT_PARAM_TYPE));
  InitDecoderOmxParams(&portFmt);
  //CONFIG_VERSION_SIZE(portFmt);
#ifdef VDEC_EXTRA_DATA
  //Extra data
  QOMX_ENABLETYPE extradata_enable;
  OMX_INDEXTYPE index_extra = (OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData;
  InitDecoderOmxParams(&extradata_enable);
  extradata_enable.bEnable = OMX_TRUE;
  omxresult = m_NodeId->setParameter(index_extra,
                                     &extradata_enable, sizeof(extradata_enable));

  if (omxresult != OK) {
    ERROR2("VideoReceiver::ConfigureDecoder::ERROR - Failed to set extra data Param %d",
           omxresult);
    //return VIDEO_ERROR_UNKNOWN;
  }

#endif
  QOMX_VIDEO_DECODER_PICTURE_ORDER picture_order;
  memset(&picture_order, 0, sizeof(QOMX_VIDEO_DECODER_PICTURE_ORDER));
  InitDecoderOmxParams(&picture_order);
  picture_order.nVersion.nVersion = 0x00000101;
  picture_order.nSize = sizeof(picture_order);
  picture_order.nPortIndex = 1;
  picture_order.eOutputPictureOrder = QOMX_VIDEO_DECODE_ORDER;
  omxresult = m_NodeId->setParameter((OMX_INDEXTYPE)
                                     OMX_QcomIndexParamVideoDecoderPictureOrder,
                                     (OMX_PTR*) & picture_order,
                                     sizeof(QOMX_VIDEO_DECODER_PICTURE_ORDER));

  if (omxresult != OK) {
    ERROR2("VideoReceiver::ConfigureDecoder::ERROR - Failed to set Picture order Param %d",
           omxresult);
    //return VIDEO_ERROR_UNKNOWN;
  }

  OMX_QCOM_PARAM_PORTDEFINITIONTYPE inputPortFmt;
  memset(&inputPortFmt, 0, sizeof(OMX_QCOM_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&inputPortFmt);
  //CONFIG_VERSION_SIZE(inputPortFmt);
  inputPortFmt.nPortIndex = 0; // input port
  inputPortFmt.nFramePackingFormat =
    OMX_QCOM_FramePacking_OnlyOneCompleteFrame;
  omxresult = m_NodeId->setParameter((OMX_INDEXTYPE) OMX_QcomIndexPortDefn,
                                     (OMX_PTR*) & inputPortFmt,
                                     sizeof(OMX_QCOM_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2("VideoReceiver::ConfigureDecoder :: get OMX_QcomIndexPortDefn IN = %d",
           omxresult);
  }

  /* Query the decoder outport's min buf requirements */
  OMX_PARAM_PORTDEFINITIONTYPE portInpFmt;
  memset(&portInpFmt, 0, sizeof(OMX_QCOM_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&portInpFmt);
  //CONFIG_VERSION_SIZE(portInpFmt);
  portInpFmt.nPortIndex = DIR_INP;
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2("VideoReceiver::ConfigureDecoder :: get OMX_IndexParamPortDefinition IN = %d",
           omxresult);
  }

  portInpFmt.format.video.nFrameHeight = tCodecConfig.iHeight;
  portInpFmt.format.video.nFrameWidth = tCodecConfig.iWidth;
  portInpFmt.format.video.xFramerate = tCodecConfig.iFrameRate;
  //portInpFmt.format.video.nStride = tCodecConfig.iWidth;
  //portInpFmt.format.video.nSliceHeight = tCodecConfig.iHeight;
  omxresult = m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult)
    ERROR2("VideoReceiver::ConfigureDecoder :: set OMX_IndexParamPortDefinition IN = %d",
           omxresult);

  memset(&portInpFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  /* Query the decoder outport's min buf requirements */
  InitDecoderOmxParams(&portInpFmt);
  //CONFIG_VERSION_SIZE(portInpFmt);
  portInpFmt.nPortIndex = DIR_OUT;
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2("VideoReceiver::ConfigureDecoder :: get OMX_IndexParamPortDefinition OUT = %d",
           omxresult);
  }

  portInpFmt.format.video.nFrameHeight = tCodecConfig.iHeight;
  portInpFmt.format.video.nFrameWidth = tCodecConfig.iWidth;
  portInpFmt.format.video.xFramerate = tCodecConfig.iFrameRate;
  //portInpFmt.format.video.nStride = tCodecConfig.iWidth;
  //portInpFmt.format.video.nSliceHeight = tCodecConfig.iHeight;
  omxresult = m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2("VideoReceiver::ConfigureDecoder :: set OMX_IndexParamPortDefinition OUT = %d",
           omxresult);
  }

  OMX_PARAM_COMPONENTROLETYPE params;
  memset(&params, 0, sizeof(OMX_PARAM_COMPONENTROLETYPE));
  InitDecoderOmxParams(&params);
  params.nSize = sizeof(params);
  params.nVersion.s.nVersionMajor = 1;
  params.nVersion.s.nVersionMinor = 0;
  params.nVersion.s.nRevision = 0;
  params.nVersion.s.nStep = 0;

  if (tCodecConfig.eCodec == VIDEO_CODEC_H264)
    strlcpy((char *) params.cRole, "video_decoder.avc",
            OMX_MAX_STRINGNAME_SIZE - 1);
  else if (tCodecConfig.eCodec == VIDEO_CODEC_H263)
    strlcpy((char *) params.cRole, "video_decoder.h263",
            OMX_MAX_STRINGNAME_SIZE - 1);
  else if (tCodecConfig.eCodec == VIDEO_CODEC_H265)
    strlcpy((char *) params.cRole, "video_decoder.hevc",
            OMX_MAX_STRINGNAME_SIZE - 1);
  else if ((tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_ISO)
           || (tCodecConfig.eCodec == VIDEO_CODEC_MPEG4_XVID))
    strlcpy((char *) params.cRole, "video_decoder.mpeg4",
            OMX_MAX_STRINGNAME_SIZE - 1);

  params.cRole[OMX_MAX_STRINGNAME_SIZE - 1] = '\0';
  omxresult = m_NodeId->setParameter(OMX_IndexParamStandardComponentRole,
                                     &params, sizeof(params));

  if (OK != omxresult) {
    ERROR2("VideoReceiver::ConfigureDecoder::::Failure in insrting the command for stateIDLE, %d",
           omxresult);
  }

  IOMX::PortMode mode;
  mode = IOMX::kPortModePresetANWBuffer;//kPortModeDynamicANWBuffer
  omxresult = m_NodeId->setPortMode(DIR_OUT, mode);

  if (omxresult != OK) {
    ERROR2("VideoReceiver:: setPortMode w/ err %d", omxresult);
    //return err;
  } else {
    CRITICAL1("VideoReceiver::setPortMode setport mode is successful with kPortModePresetANWBuffer");
  }

  //move component  to IDLE state
  DEBUG1("VideoReceiver::ConfigureDecoder::acquired first lock moving to OMX_StateIdle\n");
  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);

  if (m_NodeId->sendCommand(OMX_CommandStateSet, OMX_StateIdle)
      != VIDEO_ERROR_OK) {
    ERROR1("VideoReceiver::ConfigureDecoder::::Failure in insrting the command for stateIDLE\n");
    return VIDEO_ERROR_UNKNOWN;
  }

  /* To configure the frame rate always as 30 */
  CRITICAL1("VideoReceiver::ConfigureDecoder:: Setting framerate as 30");
  OMX_VENDOR_VIDEOFRAMERATE data;
  InitDecoderOmxParams(&data);
  data.nPortIndex = DIR_INP;// INPUT port index;
  data.nFps = ( MAX_FRAME_RATE << 16 );
  data.bEnabled = OMX_TRUE;
  omxresult = m_NodeId->setConfig((OMX_INDEXTYPE)OMX_IndexVendorVideoFrameRate,
                                  (OMX_PTR*)&data, sizeof(OMX_VENDOR_VIDEOFRAMERATE));

  if (omxresult != OK) {
    ERROR2("VideoReceiver::ConfigureDecoder:: error in set config of FrameRate %d",
           omxresult);
  }

  /* Query the decoder outport's min buf requirements */
//  OMX_PARAM_PORTDEFINITIONTYPE portInpFmt;
  //CONFIG_VERSION_SIZE(portInpFmt);
  memset(&portInpFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&portInpFmt);
  portInpFmt.nPortIndex = DIR_INP;
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2(
      "VideoReceiver::ConfigureDecoder :: get OMX_IndexParamPortDefinition IN = %x",
      omxresult);
  }

  if (OMX_DirInput != portInpFmt.eDir) {
    CRITICAL1("VideoReceiver::ConfigureDecoder:: Dec: Expect Input Port");
    return VIDEO_ERROR_UNKNOWN;
  }

#ifdef MSM8909

  // For H263 in 8909 target we dont need have this input buffer count
  if (tCodecConfig.eCodec == VIDEO_CODEC_H264) {
    portInpFmt.nBufferCountActual =
      8;    //Increasing the iomx input buffer size to avoid macro blocks
  }

#else
  portInpFmt.nBufferCountActual = 8;
#endif
  omxresult = m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2(
      "VideoReceiver::ConfigureDecoder :: set OMX_IndexParamPortDefinition IN = %x",
      omxresult);
  }

  if (OMX_DirInput != portInpFmt.eDir) {
    CRITICAL1("VideoReceiver::ConfigureDecoder:: Dec: Expect Input Port");
    return VIDEO_ERROR_UNKNOWN;
  }

  memset(&portInpFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&portInpFmt);
  portInpFmt.nPortIndex = DIR_INP;
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     (OMX_PTR*) & portInpFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (OK != omxresult) {
    ERROR2(
      "VideoReceiver::ConfigureDecoder :: get OMX_IndexParamPortDefinition IN = %x",
      omxresult);
  }

  if (OMX_DirInput != portInpFmt.eDir) {
    ERROR1("VideoReceiver::ConfigureDecoder:: Dec: Expect Input Port");
    return VIDEO_ERROR_UNKNOWN;
  }

  MINIMAL2("Requested BufferActual to %lu", portInpFmt.nBufferCountActual);
  //Store the inp buffer count for future use
  m_iMinInputBufCount = portInpFmt.nBufferCountActual;

  if (!isTrebleEnabled) {
    sp < IMemory > mem;
    size_t totalSize = portInpFmt.nBufferCountActual * portInpFmt.nBufferSize;
    m_mDealer[portInpFmt.nPortIndex] = new MemoryDealer(totalSize,
        "VideoReceiver");
    CRITICAL2("VideoReceiver::ConfigureDecoder::nMin Buffer Count=%d\n",
              portInpFmt.nBufferCountActual);
    CRITICAL2("VideoReceiver::ConfigureDecoder:: nBuffer Size=%d\n",
              portInpFmt.nBufferSize);
    CRITICAL2("pbuffer VideoReceiver::ConfigureDecoder:: nBuffer Size=%d\n",
              portInpFmt.nBufferSize);
    CRITICAL2("VideoReceiver::ConfigureDecoder:: m_iMinInputBufCount = %d\n",
              m_iMinInputBufCount);

    for (uint8_t i = 0; i < portInpFmt.nBufferCountActual; i++) {
      m_pDecInpBufHeader_iomx[i] = (VideoEncBufferInfo*) malloc(sizeof(
                                     VideoEncBufferInfo));
      CRITICAL1(
        "VideoReceiver::ConfigureDecoder::Before allocating the buffer for inp port\n");
      mem = m_mDealer[portInpFmt.nPortIndex]->allocate(
              portInpFmt.nBufferSize);

      if (mem.get() == NULL) {
        return VIDEO_ERROR_UNKNOWN;
      }

      omxresult = m_NodeId->useBuffer(
                    portInpFmt.nPortIndex, mem,
                    &(m_pDecInpBufHeader_iomx[i]->buffer_id));

      if (omxresult != OK) {
        ERROR2(
          "VideoReceiver::ConfigureDecoder:: error in Allocate Buffer for inp ports. error %x\n ",
          omxresult);
        return VIDEO_ERROR_UNKNOWN;
      } else {
        DEBUG2(
          "VideoReceiver::ConfigureDecoder OMX_AllocateBuffer Input buffer %d success",
          i);
      }

      if (m_pDecInpBufHeader_iomx[i]) {
        m_pDecInpBufHeader_iomx[i]->filllen = 0;
        m_pDecInpBufHeader_iomx[i]->alloclen = portInpFmt.nBufferSize;
      } else {
        ERROR1(
          "VideoReceiver::ConfigureDecoder:: m_pEncInpBufHeader is NULL and not initialized");
      }

      //uses as use buffer
      if (mem != NULL) {
        m_pDecInpBufHeader_iomx[i]->pBuffer = mem->pointer();
        m_pDecInpBufHeader_iomx[i]->offset = mem->offset();
        DEBUG2(
          " VideoReceiver::ConfigureDecoder  m_pDecInpBufHeader_iomx[i]->pBuffer %p\n",
          m_pDecInpBufHeader_iomx[i]->pBuffer);
      }
    }
  } else {
    mAllocator[portInpFmt.nPortIndex] = IAllocator::getService("ashmem");

    if (mAllocator[portInpFmt.nPortIndex] == NULL) {
      ERROR2("VideoReceiver::ConfigureDecoder hidl allocator on port %d is null",
             (int)portInpFmt.nPortIndex);
      return VIDEO_ERROR_UNKNOWN;
    }

    for (uint8_t i = 0; i < portInpFmt.nBufferCountActual; i++) {
      hidl_memory hidlMemoryToken;
      sp<TMemory> hidlMemory;
      bool success;
      m_pDecInpBufHeader_iomx[i] = (VideoEncBufferInfo *)malloc(sizeof(
                                     VideoEncBufferInfo));
      auto transStatus = mAllocator[portInpFmt.nPortIndex]->allocate(
                           portInpFmt.nBufferSize,
                           [&success, &hidlMemoryToken](
                             bool s,
      hidl_memory const & m) {
        success = s;
        hidlMemoryToken = m;
      });

      if (!transStatus.isOk()) {
        ERROR2("VideoReceiver::ConfigureDecoder hidl's AshmemAllocator failed at the "
               "transport: %s",
               transStatus.description().c_str());
        return VIDEO_ERROR_UNKNOWN;
      }

      if (!success) {
        return VIDEO_ERROR_UNKNOWN;
      }

      hidlMemory = mapMemory(hidlMemoryToken);
      OMXBuffer Buff(hidlMemoryToken);
      InputHidlmem[i] = hidlMemory;
      omxresult = m_NodeId->useBuffer(
                    portInpFmt.nPortIndex, Buff, &(m_pDecInpBufHeader_iomx[i]->buffer_id));

      if (omxresult != OK) {
        CRITICAL1(
          "VideoReceiver::ConfigureDecoder: error in Allocate Buffer for inp ports ");
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_pDecInpBufHeader_iomx[i]) {
        m_pDecInpBufHeader_iomx[i]->filllen = 0;
        m_pDecInpBufHeader_iomx[i]->alloclen = portInpFmt.nBufferSize;
      } else {
        CRITICAL1(
          "VideoReceiver::ConfigureDecoder m_pEncInpBufHeader is NULL and not initialized");
      }

      //uses as use buffer
      if (hidlMemory != NULL) {
        m_pDecInpBufHeader_iomx[i]->pBuffer = (void*)InputHidlmem[i]->getPointer();
        m_pDecInpBufHeader_iomx[i]->offset = 0;
        DEBUG3_STR("m_pDecInpBufHeader_iomx[%d]->pBuffer %p", i,
                   m_pDecInpBufHeader_iomx[i]->pBuffer);
      }
    }
  }

  status_t err = OK;
#if 0 //disabled for O Deprecated
  status_t err = m_OMX_handle->enableNativeBuffers(m_NodeId, DIR_OUT, OMX_TRUE,
                 OMX_TRUE);

  if (err != OK) {
    return VIDEO_ERROR_UNKNOWN;
  }

#endif
  //LOGE("VideoReceiver::ConfigureDecoder surfaceFar = %p", mNativeWindowSurfaceFar.get());
#ifdef ANDROID_VT_JB

  if (g_surfaceFar != NULL) {
#ifdef FAREND_WITH_SURFACETEXTURE
    mNativeWindow = new SurfaceTextureClient(g_surfaceFar);
#else
    mNativeWindow = new Surface(g_surfaceFar);
#endif
  }

#else

  /*lets disconnect the mnativewindow before connecting it again to
    make sure we are not connecting it and leaving leaks*/
  if (mNativeWindow.get() != NULL) {
    status_t error = OK;
    error = native_window_api_disconnect(mNativeWindow.get(),
                                         NATIVE_WINDOW_API_MEDIA);

    if (error != OK) {
      CRITICAL3_STR("VideoReceiver:: configuredecoder  native_window_api_disconnect returned an error: %s (%d)",
                    strerror(-error), error);
    } else {
      CRITICAL1("configuredecoder native_window_api_disconnect");
    }
  }

  mNativeWindow = mNativeWindowSurfaceFar.get();
  CRITICAL3_STR("VideoReceiver:: mNativeWindow.get() %p m_ePlayerState - %d",
                mNativeWindow.get(), m_ePlayerState);
#endif
  err = native_window_api_connect(mNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);

  if (err != NO_ERROR) {
    ERROR3("VideoReceiver  native_window_api_connect failed: %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  CRITICAL3("Configure Height : %u width: %u",
            portInpFmt.format.video.nFrameHeight, portInpFmt.format.video.nFrameWidth);
  // Quering the buffers needed for decoding and rendering
  OMX_PARAM_PORTDEFINITIONTYPE portdef;
  memset(&portdef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
  portdef.nVersion.s.nVersionMajor = 1;
  portdef.nVersion.s.nVersionMinor = 0;
  portdef.nVersion.s.nRevision = 0;
  portdef.nVersion.s.nStep = 0;
  InitDecoderOmxParams(&portdef);
  portdef.nPortIndex = DIR_OUT;
  err = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                               &portdef, sizeof(portdef));

  if (err != OK) {
    CRITICAL2("ConfigureDecoder OMX_IndexParamPortDefinition getparam error %x",
              err);
    return VIDEO_ERROR_UNKNOWN;
  }

  DEBUG1(
    "VideoReceiver::ConfigureDecoder native_window:: native_window_set_scaling_mode");
  err = native_window_set_scaling_mode(mNativeWindow.get(),
                                       NATIVE_WINDOW_SCALING_MODE_SCALE_CROP);

  if (err != OK) {
    CRITICAL2("ConfigureDecoder native_window_set_scaling_mode faield %x", err);
    return VIDEO_ERROR_UNKNOWN;
  }

  int format = portdef.format.video.eColorFormat;
#if 0

  if (portdef.format.video.eColorFormat ==
      OMX_QCOM_COLOR_FormatYVU420SemiPlanar) {
    format = HAL_PIXEL_FORMAT_YCrCb_420_SP;
  }

  DEBUG1(
    "VideoReceiver::ConfigureDecoder native_window:: native_window_set_buffers_geometry");
#endif
  CRITICAL3("VideoReceiver::ConfigureDecoder native_window_set_buffers_geometry width: %u hegith: %u",
            portdef.format.video.nFrameWidth,
            portdef.format.video.nFrameHeight);
  err = native_window_set_buffers_geometry(mNativeWindow.get(),
        portdef.format.video.nFrameWidth, portdef.format.video.nFrameHeight, format);

  if (err != 0) {
    ERROR3(
      "VideoReceiver::failed in native_window_set_buffers_geometry with error %s (%d)",
      strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  android_native_rect_t crop;
  crop.left = 0;
  crop.top = 0;
  crop.right = tCodecConfig.iWidth;
  crop.bottom = tCodecConfig.iHeight;
  CRITICAL3("VideoReceiver:: configure width  = %d height =  %d", crop.right,
            crop.bottom);
#ifdef FAREND_WITH_SURFACETEXTURE
  err = mNativeWindow.get()->perform(mNativeWindow.get(),
                                     NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY,
                                     portdef.format.video.nFrameWidth, portdef.format.video.nFrameHeight,
                                     format);
#endif
  err = native_window_set_crop(mNativeWindow.get(), &crop);

  if (err != 0) {
    ERROR3("VideoReceiver::failed in native_window_set_crop with error %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  //native window setup
  OMX_U32 usage = 0;
  err = m_NodeId->getGraphicBufferUsage(DIR_OUT, &usage);

  if (err != 0) {
    ERROR2(
      "VideoReceiver::failed in getGraphicBufferUsage: %d",
      err);
    usage = 0;
  }

  EVENT2("VideoReceiver::native_window_set_usage usage=0x%x", usage);
  EVENT1(
    "VideoReceiver::ConfigureDecoder native_window:: native_window_set_usage");
  err = native_window_set_usage(mNativeWindow.get(),
                                usage | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP);

  if (err != 0) {
    ERROR3("VideoReceiver::failed in setting native_window_set_usage with error: %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  int minUnDequeuedBufs = 0;
  err = mNativeWindow.get()->query(mNativeWindow.get(),
                                   NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &minUnDequeuedBufs);
  CRITICAL2("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS buffer %d", minUnDequeuedBufs);

  if (err != 0) {
    ERROR3(
      "VideoReceiver::query of native window for buffers failed with error %s (%d)",
      strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  OMX_U32 newBufferCount = portdef.nBufferCountMin + minUnDequeuedBufs + 3;
  portdef.nBufferCountActual = newBufferCount;
  err = m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                               &portdef, sizeof(portdef));

  if (err != OK) {
    ERROR3(
      "VideoReceiver::failed while setting nBufferCountActual to %u error no = %d",
      newBufferCount, err);
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT1("native_window:: native_window_set_buffer_count successful");
  err = native_window_set_buffer_count(mNativeWindow.get(),
                                       portdef.nBufferCountActual);

  if (err != 0) {
    ERROR3("VideoReceiver::failed in setting native_window_set_buffer_count with error = %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  m_iMinOutputBufCount = portdef.nBufferCountActual;
  /* IMS_ARCH_64 */
  //#if !defined(__aarch64__)
  //err = mNativeWindow.get()->perform(mNativeWindow.get(), NATIVE_WINDOW_SET_BUFFERS_SIZE, portdef.nBufferSize);
  //#endif
  CRITICAL3("NATIVE_WINDOW_SET_BUFFERS_SIZE : %d err : %d", portdef.nBufferSize,
            err);
  CRITICAL4("VideoReceiver:: allocated buffer count %u buffer size = %u minimum buffer count = %u",
            portdef.nBufferCountActual, portdef.nBufferSize, portdef.nBufferCountMin);
  OMX_U32 index = 0;

  // Dequeue buffers from native window and send it to decoder
  for (; index < portdef.nBufferCountActual; index++) {
    ANativeWindowBuffer* buf;
    /*err = mNativeWindow.get()->dequeueBuffer(mNativeWindow.get(), &buf);*/ //depricated in jb_2.5
    err = dequeueBufferFromNativeWindow(mNativeWindow.get(), &buf);

    if (err != 0) {
      ERROR3("VideoReceiver::failed in dequeueBuffer of native window with error= %s (%d)",
             strerror(-err), -err);
      break;
    }

    OMX_U8 *pbuffer = NULL;
#ifdef VDEC_EXTRA_DATA
    private_handle_t *handle = (private_handle_t *) buf->handle;

    if (!handle) {
      ERROR1("VideoReceiver::native buffer handle is NULL");
      break;
    }

    if (handle) {
      //  CRITICAL3("handle fd =%d size = %d", handle->fd, portdef.nBufferSize);
      pbuffer = (OMX_U8 *)mmap(0, portdef.nBufferSize,
                               PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
      int errnum = errno;

      if (pbuffer == MAP_FAILED) {
        CRITICAL4("Failed to mmap pmem with fd = %d, size = %d errno=%d", handle->fd,
                  handle->size, errnum);
      } else {
        CRITICAL3(" success mmap of buffer fd=%d size= %d", handle->fd,
                  portdef.nBufferSize);
      }
    } else {
      CRITICAL1("Failed to get native handle\n");
    }

#endif
    m_pDecOutpBufHeader_iomx[index] = (VideoEncBufferInfo*) malloc(
                                        sizeof(VideoEncBufferInfo));
    memset(m_pDecOutpBufHeader_iomx[index], 0, sizeof(VideoEncBufferInfo));

    if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer  != NULL) {
      ERROR1("VideoReceiver::GraphicBuffer malloc already clearing it to null before allocating again ");
      m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer = NULL;
    }

    sp<GraphicBuffer> graphicBuffer(GraphicBuffer::from(buf));
    m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer = graphicBuffer;
    m_pDecOutpBufHeader_iomx[index]->pBuffer = NULL;
    m_pDecOutpBufHeader_iomx[index]->pBufferSize = 0;

    if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer  == NULL) {
      // m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer = graphicBuffer.get();
      //}else{
      ERROR1("VideoReceiver::GraphicBuffer malloc failed ");
      return VIDEO_ERROR_UNKNOWN;
    }

#ifdef VDEC_EXTRA_DATA
    m_pDecOutpBufHeader_iomx[index]->pBuffer = pbuffer;
    m_pDecOutpBufHeader_iomx[index]->pBufferSize = portdef.nBufferSize;
#endif
    IOMX::buffer_id bufferId;
    err = m_NodeId->useBuffer(DIR_OUT,
                              m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer,
                              &bufferId);

    if (err != 0) {
      ERROR2("VideoReceiver::registering GraphicBuffer with OMX IL component failed with error %d",
             err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_pDecOutpBufHeader_iomx[index]->buffer_id = bufferId;
    m_pDecOutpBufHeader_iomx[index]->usedbyIOMX = QP_FALSE;
    m_pDecOutpBufHeader_iomx[index]->fencefd = -1;
    DEBUG3_STR("registered graphic buffer with ID %d (pointer = %p)", bufferId,
               m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer.get());
  }

  OMX_U32 cancelStart;
  OMX_U32 cancelEnd;

  if (err != 0) {
    //During occurance of a dequeue error we need to cancel the remaining buffers
    cancelStart = 0;
    cancelEnd = index;
  } else {
    // Return the last two buffers to the native window.
    cancelStart = portdef.nBufferCountActual - minUnDequeuedBufs - 3;
    cancelEnd = portdef.nBufferCountActual;
  }

  CRITICAL3("VideoReceiver:: ConfigureDecoder : Cancelling Buffer -> Cancel Start - %d Cancel End - %d",
            cancelStart, cancelEnd);

  for (OMX_U32 i = cancelStart; i < cancelEnd; i++) {
    if (cancelBufferToNativeWindow(i) < 0 ) {
      ERROR2("VideReceiver""ConfigureDecoder: cancelBufferToNativeWindow failed i- %d",
             i);
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  CHECK(sem_timedwait());
  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d",
            m_IOMXState);

  if (m_IOMXState != OMX_StateIdle) {
    ERROR1("Component not in Idle state");
    return VIDEO_ERROR_UNKNOWN;
  }

  CRITICAL1("VideoReceiver Codec in Idle state fully configured \n");
  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoReceiver::CropPort()
{
  OMX_PARAM_PORTDEFINITIONTYPE portFmt;
  memset(&portFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&portFmt);
  portFmt.nPortIndex = DIR_OUT;
  status_t omxresult = m_NodeId->getParameter(
                         OMX_IndexParamPortDefinition, &portFmt,
                         sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (omxresult != OK) {
    ERROR2("VideoReceiver::CropPort OMX_IndexParamPortDefinition error %x",
           omxresult);
  }

  OMX_CONFIG_RECTTYPE rect;
  memset(&rect, 0, sizeof(OMX_CONFIG_RECTTYPE));
  InitDecoderOmxParams(&rect);
  rect.nPortIndex = DIR_OUT;
  omxresult = m_NodeId->getConfig(OMX_IndexConfigCommonOutputCrop,
                                  &rect,
                                  sizeof(OMX_CONFIG_RECTTYPE));

  if (omxresult != OK) {
    ERROR2("VideoReceiver::CropPort OMX_IndexParamPortDefinition error %x",
           omxresult);
  }

  android_native_rect_t crop;
  crop.left = rect.nLeft;
  crop.top = rect.nTop;
  crop.right = rect.nWidth;
  crop.bottom = rect.nHeight;
  CRITICAL5("VideoReceiver:: OMX_IndexConfigCommonOutputCrop config common crop: width = %d height =  %d left = %d right = %d",
            crop.right, crop.bottom, crop.left, crop.top);
  //CR 697039
  PlayerMsg info;
  //FillBufferInfo info;
  memset(&info, 0, sizeof(PlayerMsg));
  info.type = RESOLUTION_UPDATE;
  info.data.resolution.height = rect.nHeight;
  info.data.resolution.width  = rect.nWidth;
  DEBUG2_STR("Resolution information in VideoReceiver::CropPort is %p", &info);
  CRITICAL3("VideoReceiver::CropPort: posting event RESOLUTION_UPDATE height = %d width =  %d",
            info.data.resolution.height, info.data.resolution.width);
  postEvent(&info);
  int err;
#ifdef FAREND_WITH_SURFACETEXTURE
  err = mNativeWindow.get()->perform(mNativeWindow.get(),
                                     NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY,
                                     rect.nWidth, rect.nHeight,
                                     portFmt.format.video.eColorFormat);
#endif
  err = native_window_set_crop(mNativeWindow.get(), &crop);

  if (err != 0) {
    ERROR3("VideoReceiver::Enable Port failed in native_window_set_crop with error %s (%d)",
           strerror(-err), -err);
  }

  CRITICAL2("VideoReceiver::Cropport::iCodecChanged = %d", iCodecChanged);

  if ((iCodecChanged == QP_TRUE) && (m_codecConfig.eCodec == VIDEO_CODEC_H264)) {
    CRITICAL1("VideoReceiver::Cropport:: feeding SPS/PPS");
    iCodecChanged = QP_FALSE;
    VideoPlayVOLFrame();
  }

  //Allow the transition to Active state only from Cropping and Reconfiguring state
  if ((iPlayerState == eVideoStateCropping)
      || (iPlayerState == eVideoStateReConfiguring)) {
    iPlayerState = eVideoStateActive;
  }

  m_codecConfig.iCropSet = QP_FALSE;
  /* Posting the vt_playframe semaphore */
  CRITICAL1("Videoreceiver::CropPort:: [vt_playframe]sem_post");
  //sem_post(&vt_playframe);
  sem_post_videoplay();
  return VIDEO_ERROR_OK;
}


/**************************************************************************************************************************************
 VideoReceiver::ReConfigurePort :: Port ReConfiguration for the decoder
 ***************************************************************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::ReConfigurePort()
{
  status_t omxresult;
  CRITICAL2("PlayerStateTransition:Player:ReConfigurePort:%d:VIDEO_INTERNAL_EVENT_STATUS_MSG",
            iPlayerState);
  EVENT2("VideoReceiver::ReConfigurePort :: Playerstate is %d", iPlayerState);

  //reconfiguration needed for 8k decoder
  if (iPlayerState == eVideoStateReConfiguring) {
    timeval tv;
    gettimeofday(&tv, NULL);
    EVENT2("TIMEDELAY::eVideoStateReConfiguring start time = %lu",
           ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));
    CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandPortDisable:%d:OMX_CommandPortDisable",
              m_IOMXState);
    reconfiguring = true;
    Releasing_state = 1;

    if (VerifyFillBuffersDone()) {
      CRITICAL1("Port disable and release <");
      //will verify if all buffers are with us and then release
      m_NodeId->sendCommand(OMX_CommandPortDisable, 1);
      ReleaseMediaBuffersAfterReconfigure();
      CRITICAL1("Port disable and release >");
    } else {
      CRITICAL1("Port disable and and wait for fill buffer done to release <");
      Wait_for_Buffer_Release = 1;
      m_NodeId->sendCommand(OMX_CommandPortDisable, 1);
      CRITICAL1("Port disable and and wait for fill buffer done to release > wait for buffers to be released");
      sem_wait(&buffer_release_semaphore);
      CRITICAL1("ReleaseMediaBuffersAfterReconfigure");
      ReleaseMediaBuffersAfterReconfigure();
    }
  }

#if 0 // We have merged the cases

  if (iPlayerState == eVideoStateReConfigureDone) {
    timeval tv;
    gettimeofday(&tv, NULL);
    ERROR2("TIMEDELAY::eVideoStateReConfigureDone  time = %d",
           ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));
    EnablePort(1);
  }

#endif
  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoReceiver::EnablePort :: Port Enable for thed decoder
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoReceiver::EnablePort(uint8_t iPort)
{
  EVENT1("VideoReceiver::EnablePort :: Sending the command for OMX_CommandPortEnable");
  OMX_PARAM_PORTDEFINITIONTYPE portFmt;
  OMX_PARAM_PORTDEFINITIONTYPE InpportFmt;
  memset(&portFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  memset(&InpportFmt, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  InitDecoderOmxParams(&portFmt);
  InitDecoderOmxParams(&InpportFmt);
  status_t omxresult;
  CRITICAL2("PlayerStateTransition:Player:EnablePort:%d:VIDEO_INTERNAL_EVENT_STATUS_MSG",
            iPlayerState);
  portFmt.nPortIndex = iPort;
  /* Port for which the Client needs to obtain info */
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     &portFmt,
                                     sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (omxresult != OK) {
    ERROR2("EnablePort OMX_IndexParamPortDefinition getparam error %x", omxresult);
    return VIDEO_ERROR_UNKNOWN;
  }

  //update the member variable with this updated out buffers
  m_iMinOutputBufCount = portFmt.nBufferCountActual;
  InpportFmt.nPortIndex = 0;
  /* Port for which the Client needs to obtain info */
  omxresult = m_NodeId->getParameter(OMX_IndexParamPortDefinition,
                                     &InpportFmt, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

  if (omxresult != OK) {
    ERROR2("EnablePort OMX_IndexParamPortDefinition getparam error %x", omxresult);
    return VIDEO_ERROR_UNKNOWN;
  }

  //update the member variable with this updated out buffers
  m_iMinInputBufCount =
    InpportFmt.nBufferCountActual;    //This was to handle the macro block issue
  CRITICAL3("VideoReceiver::EnablePort:: m_iMinInputBufCount = %d Actual = %u",
            m_iMinInputBufCount, InpportFmt.nBufferCountActual);
  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandPortEnable:%d:OMX_CommandPortEnable",
            m_IOMXState);
  m_NodeId->sendCommand(OMX_CommandPortEnable, iPort);
  EVENT3("VideoReceiver::EnablePort:: Min Buffer Count=%u and Buffer Size=%u",
         (unsigned int) portFmt.nBufferCountActual,
         (unsigned int) portFmt.nBufferSize);

  if (OMX_DirOutput != portFmt.eDir) {
    ERROR1("VideoReceiver::EnablePort::Error - Expect Output Port\n");
    return VIDEO_ERROR_UNKNOWN;
  }

  DEBUG2_STR("VideoReceiver:: EnablePort window=%p\n", mNativeWindow.get());
  CRITICAL3("VideoReceiver:: EnablePort  Reconfigure Height : %u width: %u",
            portFmt.format.video.nFrameHeight, portFmt.format.video.nFrameWidth);
  status_t error = OK;

  if (mNativeWindow != NULL) {
    error = native_window_api_disconnect(mNativeWindow.get(),
                                         NATIVE_WINDOW_API_MEDIA);

    if (error != OK) {
      ERROR3("VideoReceiver:: EnablePort  native_window_api_disconnect returned an error: %s (%d)",
             strerror(-error), error);
    }
  }

  error = native_window_api_connect(mNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);

  if (error != NO_ERROR) {
    ERROR3("VideoReceiver::EnablePort   native_window_api_connect failed: %s (%d)",
           strerror(-error), -error);
    //(void)native_window_api_connect(mNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
    return VIDEO_ERROR_UNKNOWN;
  }

  // Quering the buffers needed for decoding and rendering
  OMX_PARAM_PORTDEFINITIONTYPE portdef;
  memset(&portdef, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
  portdef.nVersion.s.nVersionMajor = 1;
  portdef.nVersion.s.nVersionMinor = 0;
  portdef.nVersion.s.nRevision = 0;
  portdef.nVersion.s.nStep = 0;
  InitDecoderOmxParams(&portdef);
  portdef.nPortIndex = DIR_OUT;
  status_t err = m_NodeId->getParameter(
                   OMX_IndexParamPortDefinition, &portdef, sizeof(portdef));

  if (err != OK) {
    ERROR2("EnablePort OMX_IndexParamPortDefinition getparam error %x", err);
    return VIDEO_ERROR_UNKNOWN;
  }

  CRITICAL3("Reconfigure portdef slice height: %u stride: %u",
            portdef.format.video.nSliceHeight,
            portdef.format.video.nStride);
  DEBUG1(
    "VideoReceiver:: EnablePort native_window:: native_window_set_scaling_mode");
  err = native_window_set_scaling_mode(mNativeWindow.get(),
                                       NATIVE_WINDOW_SCALING_MODE_SCALE_CROP);

  if (err != OK) {
    return VIDEO_ERROR_UNKNOWN;
  }

  int format = portdef.format.video.eColorFormat;
#if 0

  if (portdef.format.video.eColorFormat ==
      OMX_QCOM_COLOR_FormatYVU420SemiPlanar) {
    format = HAL_PIXEL_FORMAT_YCrCb_420_SP;
  }

  DEBUG1(
    "VideoReceiver:: EnablePort native_window:: native_window_set_buffers_geometry");
#endif
  CRITICAL3("VideoReceiver:: EnablePort  native_window_set_buffers_geometry width: %u hegith: %u",
            portdef.format.video.nFrameWidth,
            portdef.format.video.nFrameHeight);
#if 0
  PlayerMsg info;
  //FillBufferInfo info;
  memset(&info, 0, sizeof(PlayerMsg));
  info.type = RESOLUTION_UPDATE;
  info.data.resolution.height = portdef.format.video.nFrameHeight;
  info.data.resolution.width  = portdef.format.video.nFrameWidth;
  EVENT2("Resolution info is %p", &info);
  postEvent(&info);
#endif
  err = native_window_set_buffers_geometry(mNativeWindow.get(),
        portdef.format.video.nFrameWidth, portdef.format.video.nFrameHeight, format);

  if (err != 0) {
    ERROR3(
      "VideoReceiver:: EnablePort failed in native_window_set_buffers_geometry with error %s (%d)",
      strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  android_native_rect_t crop;
  crop.left = 0;
  crop.top = 0;
  crop.right = portFmt.format.video.nFrameWidth;
  crop.bottom = portFmt.format.video.nFrameHeight;
  CRITICAL3("VideoReceiver::EnablePort width = %d height =  %d", crop.right,
            crop.bottom);
#ifdef FAREND_WITH_SURFACETEXTURE
  err = mNativeWindow.get()->perform(mNativeWindow.get(),
                                     NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY,
                                     portdef.format.video.nFrameWidth, portdef.format.video.nFrameHeight,
                                     format);
#endif
  err = native_window_set_crop(mNativeWindow.get(), &crop);

  if (err != 0) {
    ERROR3("VideoReceiver::Enable Port failed in native_window_set_crop with error %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT1("VideoReceiver crop called");
  //native window setup
  OMX_U32 usage = 0;
  err = m_NodeId->getGraphicBufferUsage(DIR_OUT, &usage);

  if (err != 0) {
    ERROR2("VideoReceiver::EnablePort:failed in getGraphicBufferUsage: %d", err);
    usage = 0;
  }

  EVENT2("VideoReceiver:: EnablePort native_window_set_usage usage=0x%x",
         usage);
  EVENT1("VideoReceiver:: EnablePort native_window:: native_window_set_usage");
  err = native_window_set_usage(mNativeWindow.get(),
                                usage | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP);

  if (err != 0) {
    ERROR3("VideoReceiver::EnablePort:failed in setting native_window_set_usage with error: %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  int minUnDequeuedBufs = 0;
  err = mNativeWindow.get()->query(mNativeWindow.get(),
                                   NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &minUnDequeuedBufs);
  CRITICAL2("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS buffer %d", minUnDequeuedBufs);

  if (err != 0) {
    ERROR3("VideoReceiver::Enable Port: query of native window for buffers failed with error %s (%d)",
           strerror(-err), -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  OMX_U32 newBufferCount = portdef.nBufferCountMin + minUnDequeuedBufs + 3;
  portdef.nBufferCountActual = newBufferCount;
  err = m_NodeId->setParameter(OMX_IndexParamPortDefinition,
                               &portdef, sizeof(portdef));

  if (err != OK) {
    ERROR3("VideoReceiver::EnablePort:failed while setting nBufferCountActual to %u error no = %x",
           newBufferCount, err);
    m_iMinOutputBufCount = 0;
    return VIDEO_ERROR_UNKNOWN;
  }

  EVENT1(
    "VideoReceiver:: EnablePort native_window:: native_window_set_buffer_count successful");
  err = native_window_set_buffer_count(mNativeWindow.get(),
                                       portdef.nBufferCountActual);

  if (err != 0) {
    ERROR3("VideoReceiver::EnablePort: failed in setting native_window_set_buffer_count with error = %s (%d)",
           strerror(-err),
           -err);
    return VIDEO_ERROR_UNKNOWN;
  }

  m_iMinOutputBufCount = portdef.nBufferCountActual;
  //#if !defined(__aarch64__)
  //err = mNativeWindow.get()->perform(mNativeWindow.get(), NATIVE_WINDOW_SET_BUFFERS_SIZE, portdef.nBufferSize);
  //#endif
  CRITICAL3("NATIVE_WINDOW_SET_BUFFERS_SIZE : %d err : %d", portdef.nBufferSize,
            err);
  CRITICAL4("VideoReceiver:: allocated buffer count %u buffer size = %u minimum buffer count = %u",
            portdef.nBufferCountActual, portdef.nBufferSize, portdef.nBufferCountMin);
  OMX_U32 index = 0;

  // Dequeue buffers from native window and send it to decoder
  for (; index < portdef.nBufferCountActual; index++) {
    ANativeWindowBuffer* buf;
    EVENT2("VideoReceiver::EnablePort inside for loop %d\n", index);
    /*err = mNativeWindow.get()->dequeueBuffer(mNativeWindow.get(), &buf);*/ //depricated in jb_2.5
    err = dequeueBufferFromNativeWindow(mNativeWindow.get(), &buf);
    EVENT2("VideoReceiver::EnablePort inside for loop with err value %d\n",
           err);

    if (err != 0) {
      ERROR3("VideoReceiver::EnablePort failed in dequeueBuffer of native window with error= %s (%d)",
             strerror(-err), -err);
      break;
    }

#ifdef VDEC_EXTRA_DATA
    private_handle_t *handle = (private_handle_t *)buf->handle;

    if (!handle) {
      ERROR1("VideoReceiver::native buffer handle is NULL");
      break;
    }

    CRITICAL5(" handle fd =%d and handle size= %d width=%d height= %d", handle->fd,
              portdef.nBufferSize, buf->width, buf->height);
    OMX_U8 *pbuffer = NULL;

    if (handle) {
      pbuffer = (OMX_U8 *)mmap(0, portdef.nBufferSize,
                               PROT_READ | PROT_WRITE, MAP_SHARED, handle->fd, 0);
      int errnum = errno;

      if (pbuffer == MAP_FAILED) {
        ERROR4(" Failed to mmap pmem with fd = %d, size = %d errno=%d", handle->fd,
               handle->size, errnum);
      } else {
        CRITICAL2(" success mmap of buffer fd=%d", handle->fd);
      }
    } else {
      CRITICAL1("Failed to get native handle\n");
    }

#endif
    m_pDecOutpBufHeader_iomx[index] = (VideoEncBufferInfo*) malloc(sizeof(
                                        VideoEncBufferInfo));
    memset(m_pDecOutpBufHeader_iomx[index], 0, sizeof(VideoEncBufferInfo));

    if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer != NULL) {
      ERROR1("VideoReceiver::GraphicBuffer malloc cleared ");
      m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer = NULL;
    }

    sp<GraphicBuffer> graphicBuffer(GraphicBuffer::from(buf));
    m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer = graphicBuffer;
    m_pDecOutpBufHeader_iomx[index]->pBuffer = NULL;
    m_pDecOutpBufHeader_iomx[index]->pBufferSize = 0;

    if (m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer == NULL) {
      ERROR1("VideoReceiver::GraphicBuffer malloc failed ");
      return VIDEO_ERROR_UNKNOWN;
    }

    //m_pDecOutpBufHeader_iomx[index]->mMediaBuffer->setObserver(this);
#ifdef VDEC_EXTRA_DATA
    m_pDecOutpBufHeader_iomx[index]->pBuffer = pbuffer;
    m_pDecOutpBufHeader_iomx[index]->pBufferSize = portdef.nBufferSize;
    EVENT5("VideoReceiver--- in function %s line no %d setobserver for %x index %d",
           __func__, __LINE__,
           m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer.get(), index);
#endif
    IOMX::buffer_id bufferId;
    err = m_NodeId->useBuffer(DIR_OUT,
                              m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer,
                              &bufferId);

    if (err != 0) {
      ERROR2("VideoReceiver::EnablePort registering GraphicBuffer with OMX IL component failed with error %x",
             err);
      return VIDEO_ERROR_UNKNOWN;
    }

    m_pDecOutpBufHeader_iomx[index]->buffer_id = bufferId;
    m_pDecOutpBufHeader_iomx[index]->usedbyIOMX = QP_FALSE;
    m_pDecOutpBufHeader_iomx[index]->fencefd = -1;
    DEBUG3_STR("VideoReceiver::registered graphic buffer with ID %d (pointer = %p)",
               bufferId, m_pDecOutpBufHeader_iomx[index]->mGraphicBuffer.get());
  }

  OMX_U32 cancelStart;
  OMX_U32 cancelEnd;

  if (err != 0) {
    //During occurance of a dequeue error we need to cancel the remaining buffers
    cancelStart = 0;
    cancelEnd = index;
  } else {
    // Return the last two buffers to the native window.
    cancelStart = portdef.nBufferCountActual - minUnDequeuedBufs - 3;
    cancelEnd = portdef.nBufferCountActual;
  }

  for (OMX_U32 i = cancelStart; i < cancelEnd; i++) {
    cancelBufferToNativeWindow(i);
    CRITICAL2("VideoReceiver::EnablePort Cancelling buffer %d", i);
  }

  EVENT1("VideoReceiver::EnablePort end of for");
  EVENT2("VideoReceiver:: EnablePort portFmt.nBufferCountMin = %u",
         (unsigned int) portFmt.nBufferCountMin);
  uint8_t i;

  for (i = 0; i < portFmt.nBufferCountMin; i++) {
    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;
    timeval tv;
    gettimeofday(&tv, NULL);
    CRITICAL4("Profiling IMS->Decoder EnablePort FillBuffer call frame = %d, appstime = %lu buffer=%d ",
              tempfillbuffercallbackcount + 1,
              ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)),
              m_pDecOutpBufHeader_iomx[i]->buffer_id);
    //sp < GraphicBuffer > graphicBuffer =
    //  m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer;
    /*if (graphicBuffer != 0) {
      // When using a native buffer we need to lock the buffer before
      // giving it to OMX.
      EVENT2("Calling lockBuffer on %p",
          m_pDecOutpBufHeader_iomx[i]->buffer_id);
      int err1 = mNativeWindow.get()->lockBuffer(mNativeWindow.get(),
          graphicBuffer.get());
      if (err1 != 0) {
        ERROR2("lockBuffer failed w/ error 0x%08x", err1);
        return VIDEO_ERROR_UNKNOWN;
      }
    }*/ //not needed from jb_2.5
    m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_TRUE;
    EVENT2("RenderingISSUE:: FILL BUFFER index %d", i);
    status_t err = m_NodeId->fillBuffer(m_pDecOutpBufHeader_iomx[i]->buffer_id,
                                        OMXBuffer::sPreset,
                                        m_pDecOutpBufHeader_iomx[i]->fencefd);

    if (err != 0) {
      ERROR2("VideoReceiver:: fillBuffer failed with error %d", err);
      m_pDecOutpBufHeader_iomx[i]->usedbyIOMX = QP_FALSE;
    }
  }

    CropPort();

  MINIMAL2("VideoReceiver::EnablePort:: got port enabled signal %d", i);
  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoReceiver::QPVIDEOOMXFreeBuffers ::Conversion of ascii to the hex
 ***************************************************************************************************************************************/
void VideoReceiver::QPVIDEOOMXFreeBuffers()
{
  return;
}

QPE_VIDEO_ERROR VideoReceiver::MovetoLoadedState()
{
  CRITICAL3_STR("VideoReceiver--- in function %s line no %d", __func__, __LINE__);
  EVENT1("VideoReceiver::MovetoLoadedState - in");
  status_t err;
  iPlayerState = eVideoStateSurfaceReleasing;

  if ( m_IOMXState == OMX_StateExecuting ) {
    CRITICAL2("VideoReceiver::MovetoLoadedState - Set OMX_StateIdle state = %d",
              m_IOMXState);
    err = m_NodeId->sendCommand(OMX_CommandStateSet,
                                OMX_StateIdle);

    if (err != OK) {
      ERROR2(
        "VideoReceiver::MovetoLoadedState Error Sending Idle State: %d\n",
        err);
      return VIDEO_ERROR_UNKNOWN;
    }

    CRITICAL1("VideoReceiver::MovetoLoadedState - WAIT FOR IDLE STATE");
    CHECK(sem_timedwait());
  }

  CRITICAL2("VideoReceiver::MovetoLoadedState - Set OMX_StateLoaded state = %d",
            m_IOMXState);
  err = m_NodeId->sendCommand(OMX_CommandStateSet,
                              OMX_StateLoaded);

  if (err != OK) {
    ERROR2(
      "VideoReceiver::MovetoLoadedState - Error Sending Loaded State: %d\n",
      err);
    return VIDEO_ERROR_UNKNOWN;
  }

  CRITICAL2("VideoReceiver::MovetoLoadedState:: m_iMinOutputBufCount = %d",
            m_iMinOutputBufCount);

  //Free out put buffer
  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
    CRITICAL2("VideoReceiver::MovetoLoadedState:: freeing the out buffer %d",
              i);

    if (NULL != m_pDecOutpBufHeader_iomx[i]) {
#ifdef VDEC_EXTRA_DATA

      if (m_pDecOutpBufHeader_iomx[i]->pBuffer) {
        CRITICAL2("UNMAP Memory pBuffer =%d", i);
        munmap(m_pDecOutpBufHeader_iomx[i]->pBuffer,
               m_pDecOutpBufHeader_iomx[i]->pBufferSize);
      }

#endif

      if (m_NodeId->freeBuffer(1,
                               m_pDecOutpBufHeader_iomx[i]->buffer_id) != OK) {
        CRITICAL2(
          "VideoReceiver::MovetoLoadedState:: Failed to free the output buffer %d",
          i);
      }

      if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
        CRITICAL4_STR(
          "VideoReceiver--- in function %s line no %d setobserver for %p to null",
          __func__, __LINE__,
          m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer.get());
        //m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->setObserver(NULL);

        // Make sure nobody but us owns this buffer at this point.
        //CHECK_EQ(m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->refcount(),
        //0);

        // Cancel the buffer if it belongs to an ANativeWindow.

        if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != 0) {
          cancelBufferToNativeWindow(i);
        }

        CRITICAL1("media buffer release called from reconfigure port");
        //m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->release();
        m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer = NULL;
      }
    }
  }

  /** Free the Descriptors alocated for IOMX ouput port */
  for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
    if (m_pDecOutpBufHeader_iomx[i] != NULL) {
      free(m_pDecOutpBufHeader_iomx[i]);
      m_pDecOutpBufHeader_iomx[i] = NULL;
    }
  }

  CRITICAL2("VideoReceiver::MovetoLoadedState:: m_iMinInputBufCount = %d",
            m_iMinInputBufCount);

  //Free input buffer
  for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
    CRITICAL2("VideoReceiver::MovetoLoadedState:: freeing the in buffer %d",
              i);

    if (m_pDecInpBufHeader_iomx[i] != NULL) {
      if (m_NodeId->freeBuffer(0,
                               m_pDecInpBufHeader_iomx[i]->buffer_id) != OK) {
        ERROR2("VideoReceiver::MovetoLoadedState:: Failed to free the input buffer %d",
               i);
      }
    }
  }

  if (!isTrebleEnabled) {
    /** Free the Descriptors alocated for IOMX input port */
    for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      if (m_pDecInpBufHeader_iomx[i] != NULL) {
        m_mDealer[DIR_INP]->deallocate(m_pDecInpBufHeader_iomx[i]->offset);
        free(m_pDecInpBufHeader_iomx[i]);
        m_pDecInpBufHeader_iomx[i] = NULL;
      }
    }
  } else {
    /** Free the Descriptors alocated for IOMX input port */
    for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      if (m_pDecInpBufHeader_iomx[i] != NULL) {
        free(m_pDecInpBufHeader_iomx[i]);
        m_pDecInpBufHeader_iomx[i] = NULL;
      }

      if (InputHidlmem[i].get() != NULL) {
        InputHidlmem[i] = NULL;
      }
    }

    mAllocator[DIR_INP].clear();
  }

  CRITICAL2(
    "VideoReceiver::MovetoLoadedState - DEACTIVATE state = %d port disabled wait for Loaded state",
    m_IOMXState);
  CHECK(sem_timedwait());
  iPlayerState = eVideoStateSurfaceReleased;
  /* It needs to be in Codec Pending state so that, once surface is provided
     internal codecConfig will be called.*/
  m_ePlayerState = VIDEO_CODEC_PENDING;
  CRITICAL2("Surface State is not Ready, m_ePlayerState %d", m_ePlayerState);
  CRITICAL3(
    "VideoReceiver::MovetoLoadedState - COMPLETE state = %d player state = %d returning",
    m_IOMXState, iPlayerState);
  return VIDEO_ERROR_OK;
}



/**************************************************************************************************************************************
 VideoReceiver::TearDown ::De Initialize the decoder components
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoReceiver::TearDown()
{
  EVENT1("VideoReceiver::TearDown - in");
  status_t err;
  CRITICAL2("PlayerStateTransition:Player:TearDown:%d:VIDEO_MSG_DEV_UNINITIALISED",
            iPlayerState);
  EVENT1("VideoReceiver::TearDown - Set  OMX_CommandPortDisable");
  EVENT2("VideoReceiver::TearDown - Set OMX_StateIdle state = %d",
         m_IOMXState);
  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d:OMX_CommandStateSet",
            m_IOMXState);

  if (m_IOMXState == OMX_StatePause || m_IOMXState == OMX_StateExecuting) {
    CRITICAL1("Trying to move to idle state by calling m_OMX_handle->sendCommand");
    err = m_NodeId->sendCommand(OMX_CommandStateSet,
                                OMX_StateIdle);
    CRITICAL1("Returned from m_OMX_handle->sendCommand");

    if (err != OK) {
      ERROR2("VideoReceiver::TearDown Error Sending Idle State: %d\n", err);
      return VIDEO_ERROR_UNKNOWN;
    }

    EVENT1("VideoReceiver::TearDown - WAIT FOR IDLE STATE");

    if (sem_timedwait() != VIDEO_ERROR_OK) {
      iPlayerState = eVideoStateNotReady;
      m_codecConfig.iErrorState = 0;
      EVENT1("VideoReceiver :: Post VIDEO_MSG_DEV_UNINITIALISED");
      CRITICAL2("PlayerStateTransition:Player:VIDEO_MSG_DEV_UNINITIALISED:%d",
                iPlayerState);
      SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d",
            m_IOMXState);
  EVENT2("VideoReceiver::TearDown - Set OMX_StateLoaded state = %d",
         m_IOMXState);

  if (m_IOMXState == OMX_StateIdle) {
    err = m_NodeId->sendCommand(OMX_CommandStateSet,
                                OMX_StateLoaded);

    if (err != OK) {
      ERROR2("VideoReceiver::TearDown - Error Sending Loaded State: %d\n", err);
      return VIDEO_ERROR_UNKNOWN;
    }

    //We dont need to verify the Fillbuffer done here as transition to Idle guarantees the buffers are returned
    EVENT2("VideoReceiver::TearDown:: m_iMinOutputBufCount = %d",
           m_iMinOutputBufCount);

    //ClearScreen();

    //Free out put buffer
    for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
      EVENT2("VideoReceiver::TearDown:: freeing the out buffer %d", i);

      if (NULL != m_pDecOutpBufHeader_iomx[i]) {
#ifdef VDEC_EXTRA_DATA

        if (m_pDecOutpBufHeader_iomx[i]->pBuffer != NULL) {
          CRITICAL2("UNMAP Memory aBuffer =%d", i);
          munmap(m_pDecOutpBufHeader_iomx[i]->pBuffer,
                 m_pDecOutpBufHeader_iomx[i]->pBufferSize);
        }

#endif

        if (m_NodeId->freeBuffer(1,
                                 m_pDecOutpBufHeader_iomx[i]->buffer_id) != OK) {
          ERROR2(
            "VideoReceiver::TearDown:: Failed to free the output buffer %d",
            i);
        }

        if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != NULL) {
          //EVENT4(
          //"VideoReceiver--- in function %s line no %d setobserver for %x to null",
          //__func__, __LINE__,
          // m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer.get());
          //m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->setObserver(NULL);

          // Make sure nobody but us owns this buffer at this point.
          //CHECK_EQ(m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->refcount(),
          //0);

          // Cancel the buffer if it belongs to an ANativeWindow.
          if (m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer != 0) {
            cancelBufferToNativeWindow(i);
          }

          //m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer->release();
          m_pDecOutpBufHeader_iomx[i]->mGraphicBuffer = NULL;
        }
      }
    }

    MINIMAL1("VideoReceiver::TearDown: Free the output Descriptors");

    /** Free the Descriptors alocated for IOMX ouput port */
    for (uint8_t i = 0; i < m_iMinOutputBufCount; i++) {
      if (m_pDecOutpBufHeader_iomx[i] != NULL) {
        free(m_pDecOutpBufHeader_iomx[i]);
        m_pDecOutpBufHeader_iomx[i] = NULL;
      }
    }

    EVENT2("VideoReceiver::TearDown:: m_iMinInputBufCount = %d",
           m_iMinInputBufCount);

    //Free input buffer
    for (uint8_t i = 0; i <= m_iMinInputBufCount; i++) {
      EVENT2("VideoReceiver::TearDown:: freeing the in buffer %d", i);

      if (m_pDecInpBufHeader_iomx[i] != NULL) {
        if (m_NodeId->freeBuffer(0,
                                 m_pDecInpBufHeader_iomx[i]->buffer_id) != OK) {
          ERROR2(
            "VideoReceiver::TearDown:: Failed to free the input buffer %d",
            i);
        }
      }
    }

    MINIMAL1("VideoReceiver::TearDown: Free the input Descriptors");

    if (!isTrebleEnabled) {
      /** Free the Descriptors alocated for IOMX input port */
      for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
        if (m_pDecInpBufHeader_iomx[i] != NULL) {
          m_mDealer[DIR_INP]->deallocate(m_pDecInpBufHeader_iomx[i]->offset);
          free(m_pDecInpBufHeader_iomx[i]);
          m_pDecInpBufHeader_iomx[i] = NULL;
        }
      }
    } else {
      /** Free the Descriptors alocated for IOMX input port */
      for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
        if (m_pDecInpBufHeader_iomx[i] != NULL) {
          free(m_pDecInpBufHeader_iomx[i]);
          m_pDecInpBufHeader_iomx[i] = NULL;
        }

        if (InputHidlmem[i].get() != NULL) {
          InputHidlmem[i] = NULL;
        }
      }

      mAllocator[DIR_INP].clear();
    }

    EVENT2(
      "VideoReceiver::TearDown - DEACTIVATE state = %d port disabled wait for Loaded state",
      m_IOMXState);

    if (sem_timedwait() != VIDEO_ERROR_OK) {
      iPlayerState = eVideoStateNotReady;
      m_codecConfig.iErrorState = 0;
      EVENT1("VideoReceiver :: Post VIDEO_MSG_DEV_UNINITIALISED");
      CRITICAL2("PlayerStateTransition:Player:VIDEO_MSG_DEV_UNINITIALISED:%d",
                iPlayerState);
      SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
      return VIDEO_ERROR_UNKNOWN;
    }
  }

  CRITICAL2("PlayerStateTransition:OMX_Decoder:OMX_CommandStateSet:%d",
            m_IOMXState);
  iPlayerState = eVideoStateNotReady;
  m_codecConfig.iErrorState = 0;
  CRITICAL2("PlayerStateTransition:Player:VIDEO_MSG_DEV_UNINITIALISED:%d",
            iPlayerState);
  SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);

  if (m_bOmxNodeAllocated && m_NodeId->freeNode() != OK) {
    ERROR1("VideoReceiver::HandleEvent :Error in de initializing the components");
  } else {
    CRITICAL1("VideoReceiver::HandleEvent :Not freeing the node since node was not allocated");
  }

  status_t error = OK;

  if (mNativeWindow.get() != NULL) {
    error = native_window_api_disconnect(mNativeWindow.get(),
                                         NATIVE_WINDOW_API_MEDIA);

    if (error != OK) {
      ERROR3("VideoReceiver:: TearDown  native_window_api_disconnect returned an error: %s (%d)",
             strerror(-error), error);
    } else {
      CRITICAL1("Teardown Disconnected the surface");
    }
  }

  MINIMAL1("VideoReceiver::TearDown - COMPLETE return");
  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoReceiver::DecoderFreeHandle ::Free the component handle
 ***************************************************************************************************************************************/
QPE_VIDEO_ERROR VideoReceiver::DecoderFreeHandle(void)
{
  OMX_STATETYPE state = OMX_StateInvalid;
  state = m_IOMXState;
  EVENT2("VideoReceiver::DecoderFreeHandle: state %d", state);

  if (state == OMX_StateLoaded) {
    sem_destroy(&buffer_release_semaphore);
    sem_destroy(&state_semaphore);
    sem_destroy(&port_settings_semaphore);
    sem_destroy(&disable_port_semaphore);
  }

  EVENT2("VideoReceiver::DecoderFreeHandle: return %d", state);
  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoReceiver::VideoPlayVOLFrame()
{
  //check the validity of SPs/PPS info before feeding it
  if (m_pRxNalInfo && ((m_pRxNalInfo->rawnal[4] & 0x0F) != 0x07)) {
    CRITICAL2("SPS/PPS is wrong discarding it as we got %d",
              m_pRxNalInfo->rawnal[4]);
    return VIDEO_ERROR_OK; // Not raising an error as we will recover in band
  }

  if (m_pRxNalInfo && m_pRxNalInfo->nalLength > 0) {
    OMX_VIDEO_CONFIG_NALSIZE naluSize;
    InitDecoderOmxParams(&naluSize);
    naluSize.nNaluBytes = m_pRxNalInfo->nalLength;
    int err = m_NodeId->setConfig(OMX_IndexConfigVideoNalSize,
                                  (OMX_PTR *) & naluSize, sizeof(OMX_VIDEO_CONFIG_NALSIZE));

    if (err != OK) {
      CRITICAL1("VideoReceiver::Configure:: error in set config of NalSize\n");
      /*return VIDEO_ERROR_UNKNOWN;*/
    }

    CRITICAL2("VideoReceiver::Configure:: the NAL unit size is set to %d\n",
              naluSize.nNaluBytes);

    if ((m_pRxNalInfo->nalLength <= VIDEOOMX_MAX_NAL_LENGTH)
        && (VideoPlayVOLFrame(m_pRxNalInfo->rawnal, m_pRxNalInfo->nalLength)
            != VIDEO_ERROR_OK)) {
      CRITICAL1(
        "VideoReceiver::Configure::Error in playing the NAL frame\n");
      //Send the error message to the RTP
      iPlayerState = eVideoStateNotReady;
      SendEventsToThinclient(VIDEO_MSG_CONFIGURE_ERROR);
      return VIDEO_ERROR_UNKNOWN;
    }
  } else {
    CRITICAL1(
      "VideoReceiver::Configure:: NAL frame is NULL and is not played out in first empty buffer\n");
  }

  return VIDEO_ERROR_OK;
}

/**************************************************************************************************************************************
 VideoReceiver::VideoPlayVOLFrame ::Play the VOL frame
 ***************************************************************************************************************************************/

QPE_VIDEO_ERROR VideoReceiver::VideoPlayVOLFrame(uint8_t* pVolFrame,
    uint16_t iVolLen)
{
  TRACE1("VideoReceiver::VideoPlayVOLFrame");

  if (iVolLen > 0 && iVolLen < VIDEOOMX_MAX_VOL_LENGTH  && pVolFrame != NULL) {
    for (uint8_t i = 0; i < m_iMinInputBufCount; i++) {
      if (m_pDecInpBufHeader_iomx[i]->filllen == 0) {
        memcpy(m_pDecInpBufHeader_iomx[i]->pBuffer, pVolFrame,
               iVolLen);
        m_pDecInpBufHeader_iomx[i]->filllen = iVolLen;

        for (uint8_t i = 0; i < iVolLen; i++) {
          CRITICAL3("Decoder OOB SPS/PPS Data [%d] = %d", i,
                    pVolFrame[i]);
        }

        DEBUG2(
          "VideoReceiver::VideoPlayVOLFrame:; emptying the buffer with buffer len = %lu",
          (unsigned long)m_pDecInpBufHeader_iomx[i]->filllen);
        timeval tv;
        gettimeofday(&tv, NULL);
        CRITICAL3(
          "TIMEDELAY::IOMXVideoDecEmptyBuffer call frame = %d, time = %lu",
          tempemptybuffercallbackcount + 1,
          ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)));

        //Store the sps/pps fed out of band to allow proper decoding of stream
        if (storeEncoded[0] == '1' || storeEncoded[0] == '3') {
          if (fpEncRecv != NULL) {
            fwrite(pVolFrame, 1, iVolLen, (FILE *)fpEncRecv);
          } else {
            MINIMAL1("Couldnt create a file");
          }
        }

        OMXBuffer InputBuff(0, iVolLen);
        int error = m_NodeId->emptyBuffer(
                      m_pDecInpBufHeader_iomx[i]->buffer_id,
                      InputBuff,
                      0,
                      ((uint64_t)(tv.tv_sec * 1000000) + tv.tv_usec), -1);

        if (error != OK) {
          ERROR2(
            "VideoReceiver::VideoPlayFrameEx:: Failed to empty the buffer 1 %d", error);
          return VIDEO_ERROR_UNKNOWN;
        }

        return VIDEO_ERROR_OK;
      }
    }
  }

  return VIDEO_ERROR_OK;
}

void VideoReceiver::onMessages(const std::list<omx_message> &messages)
{
  std::list<omx_message>::const_iterator msg;

  for (msg = messages.begin(); msg != messages.end(); ++msg) {
    switch (msg->type) {
      case omx_message::EVENT:
        EVENT1("VideoReciever:: onmessage EVENT");
        IOMXVideoDecEventHandler(m_NodeId, msg->u.event_data.event,
                                 msg->u.event_data.data1, msg->u.event_data.data2);
        break;

      case omx_message::EMPTY_BUFFER_DONE:
        DEBUG1("VideoReciever:: onmessage EMPTY_BUFFER_DONE");
        IOMXVideoDecEmptyBufferDone(m_NodeId, msg->u.buffer_data.buffer);
        break;

      case omx_message::FILL_BUFFER_DONE:
        DEBUG1("VideoReciever:: onmessage FILL_BUFFER_DONE");
        IOMXVideoDecFillBufferDone(m_NodeId, msg->u.extended_buffer_data.buffer,
                                   msg->u.extended_buffer_data.range_offset,
                                   msg->u.extended_buffer_data.range_length,
                                   msg->u.extended_buffer_data.flags,
                                   msg->u.extended_buffer_data.timestamp//,msg.u.extended_buffer_data.platform_private,msg.u.extended_buffer_data.data_ptr, 0
                                  );
        break;

      default:
        DEBUG2("VideoReciever:: default event %d", msg->type);
    }
  }
}


}
