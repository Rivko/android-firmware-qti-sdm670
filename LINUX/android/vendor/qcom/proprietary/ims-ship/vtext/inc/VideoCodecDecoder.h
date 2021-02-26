/*****************************************************************************

 ============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoCodecDecoder.h
 Description: Implementation of VideoCodecDecoder

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/

#include<MediaCodecBase.h>
#include <gui/Surface.h>
#include <utils/StrongPointer.h>
#include<VTPlayer.h>
#include <android/native_window.h>
class VideoCodecDecoder : public MediaCodecBase, public VTPlayer
{
  public:
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
    QPE_VIDEO_ERROR UpdateNalHeader(
      uint8_t* pVolHeader,
      uint16_t pVolHeaderLen);
    virtual void HandleEvent(void* data) {
      if (data != NULL) {
        ERROR1("HandleEvent NOT IMPLEMENTED NA for MediaCodec");
      }
    };
    //player specific functions
    virtual QPE_VIDEO_ERROR MovetoLoadedState() {
      ERROR1("MoveToLoadedState NOT IMPLEMENTED NA for MediaCodec");
      return VIDEO_ERROR_UNKNOWN;
    };
    virtual void MovetoExecutingState() {
      ERROR1("MovetoExecutingState NOT IMPLEMENTED NA for MediaCodec");
      return;
    };
    virtual QPE_VIDEO_ERROR VideoPlayFrame(QP_MULTIMEDIA_FRAME* pFrame,
                                           int belongs_to_drop_set);
    virtual void SetNativeWindow(sp<ANativeWindow> surfaceFar);
    virtual ~VideoCodecDecoder();
    VideoCodecDecoder();
    /** Returns true if the thread started */
    bool StartPlayerThread() {
      CRITICAL1("Start PlayerLoop Thread ");
      return (pthread_create(&m_PlayerThread, NULL, PlayerThreadFunction,
                             this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitforPlayerThread() {
      CRITICAL1("Waiting for PlayerLoop Thread to exit");
      (void) pthread_join(m_PlayerThread, NULL);
      CRITICAL1("PlayerLoop Thread exited");
    }

  private:
    int AddToFrameStateMap(QP_MULTIMEDIA_FRAME* pFrameData,
                           int belongs_to_drop_set,
                           long long int app_tstamp);
    int DropFrameCheck(long long int app_tstamp, uint8_t& cvo_info,
                       int8_t& retreive_index,
                       uint64_t ulCurTime);
    static void * PlayerThreadFunction(void * This) {
      ((VideoCodecDecoder *)This)->videoPlayFrameLoop();
      return NULL;
    }
    void videoPlayFrameLoop();
    int32_t CalcTransform(uint8_t ucCvoInfo);
    pthread_t m_PlayerThread;
    enum eVideoState iPlayerState;
    uint8_t m_PlayerThreadCreated;
    FrameStateMap m_frameStateSet[FRAME_STATE_SET_SIZE];
    uint32_t m_frameStateIndex = 0;
    uint8_t  m_ucPrevCvoInfo;
};
