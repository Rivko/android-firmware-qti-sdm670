/*****************************************************************************

============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoCodecBase.cpp
 Description: Implementation of VideoCodecBase

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/
#include<VideoCodecBase.h>

bool m_bMediaServiceDied = false;

VideoCodecBase::VideoCodecBase()
{
  m_bMediaServiceDied = false;
}

VideoCodecBase::~VideoCodecBase()
{
  CRITICAL1("VideoCodecBase destructor");
}

/*********************************************************************************
 Send the DPL events to the RTP
 *********************************************************************************/
void VideoCodecBase::SendEventsToThinclient(int32_t event)
{
  QPE_VIDEO_MSG iVideoMsg = (QPE_VIDEO_MSG) event;

  if (m_bInternalEvent || NULL == m_pCallback) {
    CRITICAL5("VideoCodecBase::SendEventsToThinclient ignore due to m_bInternalEvent %d or pCallback %p for dev %d and event %d",
              m_bInternalEvent, m_pCallback, m_eVidDevType, iVideoMsg);
    return;
  }

  EVENT3("VideoCodecBase::SendEventsToThinclient:: Sending the events to RTP with event %d and the dev type is %d",
         iVideoMsg, m_eVidDevType);

  //! Only Device un-initialization only needs special attention depdening on
  //! whether it is player or recorder.
  if (VIDEO_MSG_DEV_UNINITIALISED == iVideoMsg) {
    CRITICAL2("SendEventsToThinclient: Sending VIDEO_MSG_DEV_UNINITIALISED event for dev %d",
              m_eVidDevType);
    m_pCallback(iVideoMsg, NULL, m_eVidDevType, SUCCESS, m_pUserData);
    m_pCallback = NULL;
    m_pUserData = NULL;
  } else {
    CRITICAL3("VideoCodecBase::SendEventsToThinclient: Sending event for msg=%d dev %d",
              iVideoMsg,
              m_eVidDevType);
    m_pCallback(iVideoMsg, NULL, m_eVidDevType, SUCCESS, m_pUserData);
  }

  return;
}
/* This API is used to register the callback for MediaService.
   If MediaService is terminated for any reason, IMS-Media will know through
   this callback function. Then IMS service process will also forcefully killed
   in response. */
void VideoCodecBase::MediaServerDied()
{
  int pid = 0;
  CRITICAL1("[DeathListener]: Inside the class : Media Server Died Indicate RTP and crash");

  //! Inform RTP about the error in MediaService
  if (!m_bMediaServiceDied) {
    m_pCallback(VIDEO_MSG_ERROR, NULL, -1, 1, NULL);
  }

  m_bMediaServiceDied = true;
  pid = getpid();
  CRITICAL2("[DeathListener]: Calling sigkill - %d", pid);
  kill(pid, SIGKILL);
}
