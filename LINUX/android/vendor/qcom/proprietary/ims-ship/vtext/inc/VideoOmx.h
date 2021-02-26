/*****************************************************************************
============================
Copyright (c)  2010-2011,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoOmx.h
 Description: Implementation of VideoOmx

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-Dec-11   Rakesh K             First Version
 *****************************************************************************/

#ifndef __VIDEOOMX_H__
#define __VIDEOOMX_H__
#define OMX_ANDROID_COMPILE_AS_32BIT_ON_64BIT_PLATFORMS
#include <VideoCodecBase.h>
//New files needed to interface with IOMX
#include <android/hidl/allocator/1.0/IAllocator.h>
#include <android/hidl/memory/1.0/IMemory.h>
#include <hidlmemory/mapping.h>
#include <media/IOMX.h>
#include <binder/IServiceManager.h>
#include <media/IMediaDeathNotifier.h>
#include <media/IMediaPlayerService.h>
#include <hidl/HidlSupport.h>
#include <media/MediaCodecBuffer.h>
//#include <qpVideoCommon.h>
#include <qplog.h>
//#include <qpdpl.h>
//#include <qpVideo.h>
using namespace android;
using hardware::hidl_memory;
using hidl::allocator::V1_0::IAllocator;
typedef hardware::hidl_memory hidl_memory;
typedef hidl::memory::V1_0::IMemory TMemory;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hidl::allocator::V1_0::IAllocator;
using ::android::hidl::base::V1_0::IBase;
using ::android::hidl::memory::V1_0::IMemory;//

class VideoOmx
{

  public:
    VideoOmx();
    virtual ~VideoOmx();
    QPE_VIDEO_ERROR RegisterCallBacks(QP_VIDEO_CALLBACK tVideoCallBack,
                                      void* pUserData, QPE_VIDEO_DEV eVDev);
    QPE_VIDEO_ERROR VerifyComponentName(char* CompName);
  protected:

    bool m_bOmxNodeAllocated;
    android::sp<android::IOMX> m_OMX_handle;
    android::sp<android::IMediaDeathNotifier> mMPDeathNotifier;
    int GetIOMXHandle();
    bool isTrebleEnabled;

};

class DeathListener : public android::IMediaDeathNotifier
{
  public :
    QP_VIDEO_CALLBACK m_pCallback;
    DeathListener(QP_VIDEO_CALLBACK pCallback) {
      m_pCallback = pCallback;
    }
    virtual void died();
};
#endif //  __VIDEO_OMX_H_
