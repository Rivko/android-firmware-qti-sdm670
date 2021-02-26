/*****************************************************************************
 ============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VTPlayer.h
 Description: Implementation of VTPlayer

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/

#ifndef __VTPLAYER_H__
#define __VTPLAYER_H__
#include <VideoCodecBase.h>
#include <qplog.h>
#ifndef ANDROIDOPR1
#include <system/window.h>
#endif

using namespace android;


struct FrameStateMap {
  int belongs_to_drop_set;
  uint8_t  cvo_info;
  uint8_t  ucFrameNalType;
  uint32_t ulFrameSize;
  uint32_t seqno;
  uint32_t source_time_video_frame;
  uint32_t system_time_decode_video_frame;
  uint32_t rtp_tstamp;
  long long int app_tstamp;
  uint64_t app_tstamp_in_us;
};
#define FRAME_STATE_SET_SIZE 30

class VTPlayer : public VideoCodecBase
{

  protected:
    eVIDEO_PLAYER_STATE m_ePlayerState;
    QPE_VIDEO_CODEC m_ePrevCodec;
    boolean m_bPlayerStartReq;
    QP_VIDEO_CONFIG m_codecConfig;
    VIDEOOMXMP4VOLInfo* m_pRxVolInfo;
    VIDEOOMXH264NalInfo* m_pRxNalInfo;
    int32_t mTransform;
    void postEvent(void *info);
  public:
    VTPlayer();
    virtual ~VTPlayer();

    virtual QPE_VIDEO_ERROR DeInit() = 0;
    virtual QPE_VIDEO_ERROR Start() = 0;
    virtual QPE_VIDEO_ERROR Stop() = 0;
    virtual QPE_VIDEO_ERROR Pause() = 0;
    virtual QPE_VIDEO_ERROR Resume() = 0;
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig) = 0;
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void* pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG* pCodecConfig) = 0;
    virtual QPE_VIDEO_ERROR RegisterPlayerCallBack(QP_VIDEO_CALLBACK tVideoCallBack,
        void* pUserData, QPE_VIDEO_DEV eVDev,
        QP_VIDEO_CONFIG* pCodecConfig);
    virtual void HandleEvent(void* data) = 0;
    virtual QPE_VIDEO_ERROR MovetoLoadedState() = 0;
    virtual void MovetoExecutingState() = 0;
    virtual QPE_VIDEO_ERROR VideoPlayFrame(QP_MULTIMEDIA_FRAME* pFrame,
                                           int belongs_to_drop_set) = 0;
    virtual void SetNativeWindow(sp<ANativeWindow> surfaceFar) = 0;
    virtual QPE_VIDEO_ERROR UpdateNalHeader(uint8_t* pVolHeader,
                                            uint16_t pVolHeaderLen) = 0;
    virtual QPE_VIDEO_ERROR StartPlayer();
    void UpdateCodecConfigInfo(QP_VIDEO_CONFIG* pCodecConfig);
};

#endif //  __VIDEO_OMX_H_
