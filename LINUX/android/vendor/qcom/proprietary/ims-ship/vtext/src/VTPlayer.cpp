/*****************************************************************************
============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VTPlayer.cpp
 Description: Implementation of VTPlayer

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 01-dec-2016   Sanjeev Mittal       First Version
 *****************************************************************************/
#include <VTPlayer.h>

//! VTPlayer constructor
VTPlayer::VTPlayer()
{
  m_ePlayerState = VIDEO_INIT;
  m_bPlayerStartReq = false;
  m_ePrevCodec = VIDEO_CODEC_MPEG4_XVID;
  m_pRxVolInfo = NULL;
  m_pRxNalInfo = NULL;
  memset(&m_codecConfig, 0, sizeof(QP_VIDEO_CONFIG) );
  CRITICAL1("VTPlayer::VTPlayer Constructor");
}

//! VTPlayer destructor
VTPlayer::~VTPlayer()
{
  if (m_pRxVolInfo) {
    free(m_pRxVolInfo);
    m_pRxVolInfo = NULL;
  }

  if (m_pRxNalInfo) {
    free(m_pRxNalInfo);
    m_pRxNalInfo = NULL;
  }

  if (m_codecConfig.pVolHeader) {
    free(m_codecConfig.pVolHeader);
  }

  if (m_codecConfig.pNALHeader) {
    free(m_codecConfig.pNALHeader);
  }

  CRITICAL1("VTPlayer::VTPlayer Destructor");
}

QPE_VIDEO_ERROR VTPlayer::RegisterPlayerCallBack(QP_VIDEO_CALLBACK
    tVideoCallBack,
    void* pUserData, QPE_VIDEO_DEV eVDev,
    QP_VIDEO_CONFIG* pCodecConfig)
{
  CRITICAL2("VTPlayer::Register API called for Device %d", eVDev);
  m_bInternalEvent = 0;
  //initialize the callbacks and userdata
  m_pCallback = tVideoCallBack;
  m_pUserData = pUserData;
  m_eVidDevType = eVDev;
  pCodecConfig = NULL;
  m_pRxVolInfo = (VIDEOOMXMP4VOLInfo*) malloc(sizeof(VIDEOOMXMP4VOLInfo));

  if (m_pRxVolInfo == NULL) {
    EVENT1("VTPlayer::RegisterPlayerCallBack malloc failed in allocating for Vol header");
    return VIDEO_ERROR_UNKNOWN;
  }

  memset(m_pRxVolInfo, 0, sizeof(VIDEOOMXMP4VOLInfo));
  m_pRxNalInfo = (VIDEOOMXH264NalInfo*) malloc(sizeof(VIDEOOMXH264NalInfo));

  if (m_pRxNalInfo == NULL) {
    EVENT1("VTPlayer::RegisterPlayerCallBack malloc failed in allocating for nal header");
    return VIDEO_ERROR_UNKNOWN;
  }

  memset(m_pRxNalInfo, 0, sizeof(VIDEOOMXH264NalInfo));
  CRITICAL3("VTPlayer::Register, Device %d, Set callback ptr: %p", eVDev,
            tVideoCallBack);
  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VTPlayer::StartPlayer()
{
  CRITICAL1("Configure API called as playerStart Request received from Modem");
  PlayerMsg eventMsg;
  eventMsg.type = CODEC_CONFIG_REQ;
  eventMsg.data.bPlayerStart = m_bPlayerStartReq;
  postEvent(&eventMsg);
  return VIDEO_ERROR_OK;
}

void VTPlayer::UpdateCodecConfigInfo(QP_VIDEO_CONFIG* pCodecConfig)
{
  if (!pCodecConfig) {
    ERROR1("pCodecConfig is NULL");
    return;
  }

  if (m_codecConfig.pNALHeader) {
    free(m_codecConfig.pNALHeader);
  }

  if (m_codecConfig.pVolHeader) {
    free(m_codecConfig.pVolHeader);
  }

  memcpy(&m_codecConfig, pCodecConfig, sizeof(QP_VIDEO_CONFIG) );
  /* We need to copy pointers explicitly, first reset to NULL */
  m_codecConfig.pNALHeader = m_codecConfig.pVolHeader = NULL;

  if (pCodecConfig->pNALHeader) {
    m_codecConfig.pNALHeader = (char*) malloc(pCodecConfig->iNALHeaderLen);

    if (m_codecConfig.pNALHeader)
      memcpy(m_codecConfig.pNALHeader,
             pCodecConfig->pNALHeader, pCodecConfig->iNALHeaderLen);

    m_codecConfig.iNALHeaderLen = pCodecConfig->iNALHeaderLen;
  }

  if (pCodecConfig->pVolHeader) {
    TRACE2("Vol header length is %d", pCodecConfig->iVolHeaderLen);
    m_codecConfig.pVolHeader = (char*) malloc(pCodecConfig->iVolHeaderLen);

    if (m_codecConfig.pVolHeader)
      memcpy(m_codecConfig.pVolHeader,
             pCodecConfig->pVolHeader, pCodecConfig->iVolHeaderLen);

    m_codecConfig.iVolHeaderLen = pCodecConfig->iVolHeaderLen;
  }

  if (pCodecConfig->pNALHeader != NULL && m_pRxNalInfo) {
    memcpy(m_pRxNalInfo->rawnal, pCodecConfig->pNALHeader,
           pCodecConfig->iNALHeaderLen);
    m_pRxNalInfo->nalLength = (uint32_t) pCodecConfig->iNALHeaderLen;
  }

  return;
}

void VTPlayer::postEvent(void *info)
{
  OMX_EVENTDATA event;
  event.pData = (void *) info;
  event.iLen = sizeof(PlayerMsg);
  m_pCallback((QPE_VIDEO_MSG)VIDEO_MSG_INTERNAL_EVENT_TRIGGER_PLAYER,
              (void*) &event, m_eVidDevType, SUCCESS, m_pUserData);
}
