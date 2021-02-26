/*****************************************************************************

 ============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoCodecBase.h
 Description: Implementation of VideoCodecBase

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal			First Version
 *****************************************************************************/

#ifndef __VIDEOCODECBASE_H
#define __VIDEOCODECBASE_H
//#include "ndk/NdkMediaCodec.h"
//#include "ndk/NdkMediaExtractor.h"

#include <cutils/log.h>
#include <VideoCommon.h>
#include <gui/IGraphicBufferProducer.h>
#include <qplog.h>

#define CHECK_MEDIA_ERROR { if(m_bMediaCodecError) return VIDEO_ERROR_UNKNOWN;}

using namespace android;

class VideoCodecBase
{
  protected:
   //ANativeWindow *AMediaCodecnativeWindow;
   //AMediaCodec *codec;
    QP_VIDEO_CALLBACK m_pCallback;
  void* m_pUserData;
  //sVIDEOOMXDATA* m_pVideoOmx;
  QPE_VIDEO_DEV m_eVidDevType;
  //! This variable is used to differentiate between external and internal Video events
  //! If it is an internval event, then no need to indicate about it in Callback.
  bool m_bInternalEvent;

    bool m_bMediaCodecError;

    void MediaServerDied();

  //! This variable is used to check whether OMX node allocation is success or not.
  //! While freeing the handle, this variable will be checked.
  public:
    virtual void IsEventInternalTriggered(bool bType) {
    CRITICAL2("VideoCodecBase::IsEventInternalTriggered %d", bType);
    m_bInternalEvent = bType;};
    //virtual QPE_VIDEO_ERROR Init() = 0;
    virtual QPE_VIDEO_ERROR DeInit() = 0;
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG tCodecConfig)= 0;
    virtual QPE_VIDEO_ERROR Start() = 0;
    virtual QPE_VIDEO_ERROR Stop() = 0;
    virtual QPE_VIDEO_ERROR Pause() = 0;
    virtual QPE_VIDEO_ERROR Resume() = 0;
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                           void* pUserData, QPE_VIDEO_DEV eVDev,
                           QP_VIDEO_CONFIG* pCodecConfig) = 0;
    virtual void HandleEvent(void* data) = 0;

    //base class functions
    void SendEventsToThinclient(int32_t event);
    virtual ~VideoCodecBase();
    VideoCodecBase();
};
#endif
