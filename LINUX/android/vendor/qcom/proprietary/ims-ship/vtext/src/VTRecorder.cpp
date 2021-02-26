/*****************************************************************************
============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
 File: VTRecorder.cpp
 Description: Implementation of VTRecorder

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/
#include <VTRecorder.h>
#include <VideoCommon.h>


//! VTRecorder constructor
VTRecorder::VTRecorder()
{
  char sliceMode[PROPERTY_VALUE_MAX];
  char ucQpRange[PROPERTY_VALUE_MAX];
  CRITICAL1("VTRecorder::VTRecorder Constructor");
  property_get("persist.vendor.ims.enableslice", sliceMode, "0");
  m_h265LevelMap[VIDEO_H265_LEVEL1]  = VT_HEVCMainTierLevel1;
  m_h265LevelMap[VIDEO_H265_LEVEL2]  = VT_HEVCMainTierLevel2;
  m_h265LevelMap[VIDEO_H265_LEVEL21] = VT_HEVCMainTierLevel21;
  m_h265LevelMap[VIDEO_H265_LEVEL3]  = VT_HEVCMainTierLevel3;
  m_h265LevelMap[VIDEO_H265_LEVEL31] = VT_HEVCMainTierLevel31;
  m_h265LevelMap[VIDEO_H265_LEVEL4]  = VT_HEVCMainTierLevel4;
  m_h265LevelMap[VIDEO_H265_LEVEL41] = VT_HEVCMainTierLevel41;
  m_h265LevelMap[VIDEO_H265_LEVEL5]  = VT_HEVCMainTierLevel5;
  m_h265LevelMap[VIDEO_H265_LEVEL51] = VT_HEVCMainTierLevel51;
  m_h265LevelMap[VIDEO_H265_LEVEL52] = VT_HEVCMainTierLevel52;
  m_h265LevelMap[VIDEO_H265_LEVEL6]  = VT_HEVCMainTierLevel6;
  m_h265LevelMap[VIDEO_H265_LEVEL61] = VT_HEVCMainTierLevel61;
  m_h265LevelMap[VIDEO_H265_LEVEL62] = VT_HEVCMainTierLevel62;
  m_h264LevelMap[VIDEO_AVC_LEVEL1B] = VT_AVCLevel1b;
  m_h264LevelMap[VIDEO_AVC_LEVEL1]  = VT_AVCLevel1;
  m_h264LevelMap[VIDEO_AVC_LEVEL11] = VT_AVCLevel11;
  m_h264LevelMap[VIDEO_AVC_LEVEL12] = VT_AVCLevel12;
  m_h264LevelMap[VIDEO_AVC_LEVEL13] = VT_AVCLevel13;
  m_h264LevelMap[VIDEO_AVC_LEVEL2]  = VT_AVCLevel2;
  m_h264LevelMap[VIDEO_AVC_LEVEL21] = VT_AVCLevel21;
  m_h264LevelMap[VIDEO_AVC_LEVEL22] = VT_AVCLevel22;
  m_h264LevelMap[VIDEO_AVC_LEVEL3]  = VT_AVCLevel3;
  m_h264LevelMap[VIDEO_AVC_LEVEL31] = VT_AVCLevel31;
  m_h264LevelMap[VIDEO_AVC_LEVEL32] = VT_AVCLevel32;
  m_h264LevelMap[VIDEO_AVC_LEVEL4]  = VT_AVCLevel4;
  m_h264LevelMap[VIDEO_AVC_LEVEL41] = VT_AVCLevel41;
  m_h264LevelMap[VIDEO_AVC_LEVEL42] = VT_AVCLevel42;
  m_h264LevelMap[VIDEO_AVC_LEVEL5]  = VT_AVCLevel5;
  m_h264LevelMap[VIDEO_AVC_LEVEL51] = VT_AVCLevel51;
  m_h263LevelMap[VIDEO_H263_LEVEL10] = VT_H263Level10;
  m_h263LevelMap[VIDEO_H263_LEVEL20] = VT_H263Level20;
  m_h263LevelMap[VIDEO_H263_LEVEL30] = VT_H263Level30;
  m_h263LevelMap[VIDEO_H263_LEVEL40] = VT_H263Level40;
  m_h263LevelMap[VIDEO_H263_LEVEL50] = VT_H263Level50;
  m_h263LevelMap[VIDEO_H263_LEVEL60] = VT_H263Level60;
  m_h263LevelMap[VIDEO_H263_LEVEL70] = VT_H263Level70;
  m_h263LevelMap[VIDEO_H263_LEVEL45] = VT_H263Level45;
  m_SliceMode = atoi(sliceMode);
  m_ullLastIdrTime = 0;
  property_get("persist.vendor.ims.mm_maxqp", ucQpRange, "44");
  m_ulMaxQpValue = atoi(ucQpRange);
  property_get("persist.vendor.ims.mm_minqp", ucQpRange, "16");
  m_ulMinQpValue = atoi(ucQpRange);
  MINIMAL4("configure: sliceMode %d, MinQpValue %d, MaxQpValue %d",
           m_SliceMode, m_ulMinQpValue, m_ulMaxQpValue);
  //TODO : set for H263 and other profiles as well.
}

//! VTRecorder destructor
VTRecorder::~VTRecorder()
{
  CRITICAL1("VTRecorder::VTRecorder Destructor");
}

QPE_VIDEO_ERROR VTRecorder::RegisterRecorderCallBack(QP_VIDEO_CALLBACK
    tVideoCallBack,
    void* pUserData, QPE_VIDEO_DEV eVDev,
    QP_VIDEO_CONFIG* pCodecConfig)
{
  CRITICAL2("VTRecorder::Register API called for Device %d", eVDev);
  m_bInternalEvent = 0;
  //initialize the callbacks and userdata
  m_pCallback = tVideoCallBack;
  m_pUserData = pUserData;
  m_eVidDevType = eVDev;
  pCodecConfig = NULL;
  CRITICAL3("VTRecorder::Register, Device %d, Set callback ptr: %p", eVDev,
            tVideoCallBack);
  return VIDEO_ERROR_OK;
}

int VTRecorder::postEvent(void *info)
{
  OMX_EVENTDATA event;
  event.pData = (void *) info;
  event.iLen = sizeof(RecorderMsg);
  m_pCallback((QPE_VIDEO_MSG)VIDEO_MSG_INTERNAL_EVENT_TRIGGER_RECORDER_HAL1,
              (void*) &event, m_eVidDevType, SUCCESS, m_pUserData);
  return 0;
};
