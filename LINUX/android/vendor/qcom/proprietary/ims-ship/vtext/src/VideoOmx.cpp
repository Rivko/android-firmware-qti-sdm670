/*****************************************************************************

============================
Copyright (c)  2010-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoOmx.cpp
 Description: Implementation of VideoOmx

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 27-Dec-11   Rakesh K             First Version
 *****************************************************************************/
#include <VideoOmx.h>
#include <media/omx/1.0/WOmx.h>
int bMediaServiceDied = QP_FALSE;

/* This API is used to register the callback for MediaService.
   If MediaService is terminated for any reason, IMS-Media will know through
   this callback function. Then IMS service process will also forcefully killed
   in response. */
void DeathListener::died()
{
  int pid = 0;
  CRITICAL1("[DeathListener]: Inside the class : Media Server Died Indicate RTP and crash");

  //! Inform RTP about the error in MediaService
  if (!bMediaServiceDied) {
    m_pCallback(VIDEO_MSG_ERROR, NULL, -1, 1, NULL);
  }

  bMediaServiceDied = QP_TRUE;
  pid = getpid();
  CRITICAL2("[DeathListener]: Calling sigkill - %d", pid);
  kill(pid, SIGKILL);
}

//! VideoOmx constructor
VideoOmx::VideoOmx()
{
  m_bOmxNodeAllocated = QP_FALSE;
  isTrebleEnabled = QP_FALSE;
  CRITICAL2("VideoOmx::VideoOmx death : %p", mMPDeathNotifier.get());
}

//! VideoOmx destructor
VideoOmx::~VideoOmx()
{
  CRITICAL1("VideoOmx::~VideoOmx");
};

/******************************************************************************************
 GetIOMXHandle Retrives a handle to the IOMX interface
 *******************************************************************************************/
int VideoOmx::GetIOMXHandle()
{
  if (property_get_bool("persist.media.treble_omx", true)) {
    using namespace ::android::hardware::media::omx::V1_0;
    sp<IOmx> trebleOmx = IOmx::getService("default");

    if (trebleOmx.get() == nullptr) {
      CRITICAL1("Cannot obtain Treble IOmx.");
      return QP_FALSE;
    }

    if (!trebleOmx->isRemote()) {
      CRITICAL1("Treble IOmx is in passthrough mode.");
      return QP_FALSE;
    }

    m_OMX_handle = new utils::LWOmx(trebleOmx);
    CRITICAL1("Treble IOmx obtained");
    isTrebleEnabled = QP_TRUE;
    return QP_TRUE;
  } else {
    sp < IMediaPlayerService > media_service =
      IMediaDeathNotifier::getMediaPlayerService();
    CRITICAL1("Create Omx handle from IMediaDeathNotifier");

    if (media_service != NULL) {
      m_OMX_handle = media_service->getOMX();
      CRITICAL2("GetOMXHandle pointer allocated, ptr %p", m_OMX_handle.get());

      if (m_OMX_handle.get() != NULL) {
        return QP_TRUE;
      }
    }

    ERROR1("GetOMXHandle exit as media service is not UP\n");
    return QP_FALSE;
  }
}



/****************************************************************************************
 Register callback, device info with Video Omx class
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoOmx::RegisterCallBacks(QP_VIDEO_CALLBACK tVideoCallBack,
    void* /*pUserData*/, QPE_VIDEO_DEV eVDev)
{
  CRITICAL2("VideoOmx::RegisterCallBacks API called for Device %d", eVDev);

  // Create required Function Pointers to OMX Shared Object
  if (!GetIOMXHandle()) {
    ERROR2("VideoOmx::Register Failed to Load OMX Shared Object for Device %d",
           eVDev);
    return VIDEO_ERROR_UNKNOWN;
  }

  mMPDeathNotifier = new DeathListener(tVideoCallBack);
  return VIDEO_ERROR_OK;
}



/****************************************************************************************
 Verify the existing components in the OMX core
 *****************************************************************************************/
QPE_VIDEO_ERROR VideoOmx::VerifyComponentName(char* CompName)
{
  List < IOMX::ComponentInfo > comp_list;
  List<IOMX::ComponentInfo>::iterator it_list;
  m_OMX_handle->listNodes(&comp_list);
  EVENT2("CompName = %s", CompName);

  for (it_list = comp_list.begin(); it_list != comp_list.end(); ++it_list) {
    EVENT2("Component list : Name = %s", ((*it_list).mName.string()));

    if (strcmp(((*it_list).mName.string()), CompName) == 0) {
      return VIDEO_ERROR_OK;
    }
  }

  return VIDEO_ERROR_UNKNOWN;
}
