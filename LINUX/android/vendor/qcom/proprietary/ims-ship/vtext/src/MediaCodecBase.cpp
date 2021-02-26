/*****************************************************************************

============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: MediaCodecBase.cpp
 Description: Implementation of MediaCodecBase

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/
#include<MediaCodecBase.h>



MediaCodecBase::~MediaCodecBase()
{
  CRITICAL1("MediaCodecBase destructor called");
}


/****************************************************************************************
Register callback, device info with MediaCodec class
*****************************************************************************************/
QPE_VIDEO_ERROR MediaCodecBase::RegisterCallBacks(QP_VIDEO_CALLBACK
    tVideoCallBack,
    void* pUserData, QPE_VIDEO_DEV eVDev)
{
  CRITICAL2("MediaCodecBase::Register API called for Device %d", eVDev);
  CRITICAL3("MediaCodecBase::Register, Device %d, Set callback ptr: %p", eVDev,
            tVideoCallBack);
  if (pUserData != NULL) {
     MINIMAL1("Userdata is not null");
  }
  return VIDEO_ERROR_OK;
};
