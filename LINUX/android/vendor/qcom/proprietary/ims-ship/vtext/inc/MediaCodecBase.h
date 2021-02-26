/*****************************************************************************

============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: MediaCodecBase.h
 Description: header of MediaCodecBase

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal			First Version
 *****************************************************************************/

#ifndef __MEDIACODECBASE_H
#define __MEDIACODECBASE_H
#ifdef ANDROIDOPR1
#include "ndk/NdkMediaCodec.h"
#include "ndk/NdkMediaExtractor.h"
#else
#include <system/window.h>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#endif
#include <cutils/log.h>
#include <gui/IGraphicBufferProducer.h>
#include <qplog.h>
#include<VideoCodecBase.h>
#include<android/native_window.h>

using namespace android;

class MediaCodecBase
{
  protected:
   //ANativeWindow *AMediaCodecnativeWindow;
    public:
     QPE_VIDEO_ERROR RegisterCallBacks(QP_VIDEO_CALLBACK tVideoCallBack,
                                   void* pUserData, QPE_VIDEO_DEV eVDev);
    virtual ~MediaCodecBase();
  AMediaFormat *m_format;
  sp<ANativeWindow> AMediaCodecnativeWindow;
  AMediaCodec *mCodec;
};
#endif
