/*****************************************************************************

============================
Copyright (c)  2004-2012,2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: qpVideo.cpp
 Description: Dpl implementation of video module

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================

 *******************************************************************************************/

/*-------------------------------------------------------------------

 -------------------------------------------------------------------*/

#include <qplog.h>
#include <qpVideo.h>
//#include <VideoOmx.h>
#ifndef MEDIA_CODEC
#include <VideoTransmitter.h>
#include <VideoReceiver.h>
//To check support for Camera HAL Version
#include <binder/IServiceManager.h>
#include <android/hardware/ICameraService.h>
#endif
#include <VideoCodecDecoder.h>
#include <VideoCodecEncoder.h>
#include <VTPlayer.h>
#include <VTRecorder.h>




/*===========================================================================

 FUNCTION checkForCamera2Support


 DESCRIPTION
 This api is used to check the supported camera HAL version.

 DEPENDENCIES
 None

 ARGUEMENTS IN
 None

 RETURN VALUE
 None

 SIDE EFFECTS
 None

 ===========================================================================*/

CAMERA_HAL_VERSION checkForCamera2Support()
{
  CAMERA_HAL_VERSION eHALVersion;
  //default mode is HAL3
  eHALVersion = CAMERA_HAL3;
#ifndef MEDIA_CODEC
  char cameraInterfaceType[PROPERTY_VALUE_MAX];
  property_get("persist.vendor.qti.telephony.vt_cam_interface",
               cameraInterfaceType, "2");
  sp<IServiceManager> serviceman = defaultServiceManager();
  sp<IBinder> binderObject = serviceman->getService(String16("media.camera"));
  sp<hardware::ICameraService> serviceObject =
    interface_cast<hardware::ICameraService>(binderObject);
  binder::Status result;
  int32_t numCameras = 0;
  bool isCamera2Supported = false;

  if (serviceObject.get() != NULL) {
    result = serviceObject->getNumberOfCameras(
               hardware::ICameraService::CAMERA_TYPE_ALL, &numCameras);

    if (result.isOk()) {
      for (int32_t i = 0; i < numCameras; i++) {
        String16 cameraid = String16(String8::format("%d", i));
        result = serviceObject->supportsCameraApi(cameraid,
                 hardware::ICameraService::API_VERSION_2, &isCamera2Supported);

        if (!isCamera2Supported) {
          CRITICAL1("checkForCamera2Support Camera2 not supported");
          eHALVersion = CAMERA_HAL1;
        }
      }
    } else {
      CRITICAL1("checkForCamera2Support getNumberOfCameras failed");
    }
  } else {
    CRITICAL1("checkForCamera2Support getService failed");
  }

  if (eHALVersion == CAMERA_HAL3 && cameraInterfaceType[0] == '2') {
    CRITICAL1("checkForCamera2Support Camera2 enabled");
  } else {
    eHALVersion = CAMERA_HAL1;
    CRITICAL1("checkForCamera2Support Camera1 enabled");
  }

#endif
  return eHALVersion;
}


/**
 * \fn QP_IMPORT_C QPBOOL IsSavingStarted(void* clientData);
 *
 * \brief Internally used to check the state of the Video Saving.
 *
 * This function will return TRUE if the saving is already started else FALSE.
 *
 * \param clientData Video Data.\n
 * See \ref qpVideoData.
 *
 * \return \ref QPBOOL
 * \retval QP_TRUE
 * \retval QP_FALSE
 */

/********************************************************************************
 qpVideoInitialize
 *********************************************************************************/
void *qpVideoInitialize(QP_VIDEO_CALLBACK tVideoCallBack,
                        void* pUserData, QPE_VIDEO_DEV eVDev,
                        QP_VIDEO_CONFIG* pCodecConfig)
{
  ERROR1("qpVideo::qpVideoinitialize");
  VTRecorder* pVideoTx = NULL;
  VTPlayer* pVideoRx = NULL;
  //VideoCodecDecoder* videoDecoder = NULL;
#if 0
  QPE_VIDEO_ERROR eStatus;

  if (VIDEO_PLAYER == eVDev) {
#ifdef MEDIA_CODEC
    IMS_NEW(VideoCodecDecoder, MEM_IMS_FW, pVideoRx, ());
#else
    IMS_NEW(VideoReceiver, MEM_IMS_FW, pVideoRx, ());
#endif

    if (pVideoRx) {
      eStatus = pVideoRx->Register(tVideoCallBack, pUserData,  eVDev, pCodecConfig);

      if (VIDEO_ERROR_OK == eStatus) {
        eStatus = pVideoRx->Init();
      }

      if (VIDEO_ERROR_OK != eStatus) {
        ERROR1("qpVideoInitialize: Player Init/Register Failed");
        IMS_DELETE(pVideoRx, MEM_IMS_FW);
        pVideoRx = NULL;
      }
    }

    return pVideoRx;
  } else if (VIDEO_RECORDER == eVDev) {
#ifdef MEDIA_CODEC
    IMS_NEW(VideoCodecEncoder, MEM_IMS_FW, pVideoTx, ());
#else
    IMS_NEW(VideoTransmitter, MEM_IMS_FW, pVideoTx, ());
#endif

    if (pVideoTx) {
      eStatus = pVideoTx->Register(tVideoCallBack, pUserData,  eVDev, pCodecConfig);

      if (VIDEO_ERROR_OK == eStatus) {
        eStatus = pVideoTx->Init();
      }

      if (VIDEO_ERROR_OK != eStatus) {
        ERROR1("qpVideoInitialize: Recorder Init/Register Failed");
        IMS_DELETE(pVideoTx, MEM_IMS_FW);
        pVideoTx = NULL;
      }
    }

    return pVideoTx;
  }

#else
  QPE_VIDEO_ERROR eStatus;

  if (VIDEO_PLAYER == eVDev) {
#ifdef MEDIA_CODEC
    ERROR1("qpVideo::qpVideoinitialize for player");
    pVideoRx = new VideoCodecDecoder();
#else
    ERROR1("qpVideo::qpVideoinitialize for player");
    pVideoRx = new VideoReceiver();
#endif

    if (pVideoRx) {
      eStatus = pVideoRx->Init(tVideoCallBack, pUserData,  eVDev, pCodecConfig);

      if (VIDEO_ERROR_OK != eStatus) {
        ERROR1("qpVideoInitialize: Player Init/Register Failed");
        delete pVideoRx;
        pVideoRx = NULL;
      }
    }

    return pVideoRx;
  } else if (VIDEO_RECORDER == eVDev) {
#ifdef MEDIA_CODEC
    ERROR1("qpVideo::qpVideoinitialize for recorder");

    switch (pCodecConfig->eHALVersion) {
      default:
      case CAMERA_HAL3:
        pVideoTx = new VideoCodecEncoder();
        break;
        // case CAMERA_HAL1:
        // default:
        //  pVideoTx = new VideoTransmitter();
    }

#else
    ERROR1("qpVideo::qpVideoinitialize for recorder");
    pVideoTx = new VideoTransmitter();
#endif

    if (pVideoTx) {
      eStatus = pVideoTx->Init(tVideoCallBack, pUserData,  eVDev, pCodecConfig);

      if (VIDEO_ERROR_OK != eStatus) {
        ERROR1("qpVideoInitialize: Recorder Init/Register Failed");
        delete pVideoTx;
        pVideoTx = NULL;
      }
    }

    return pVideoTx;
  }

#endif
  return NULL;
}

void qpVideoUninitialize(VideoCodecBase *tVideoDescriptor, QPE_VIDEO_DEV eVDev)
{
  ERROR1("qpVideo::qpVideoUninitialize");

  if (tVideoDescriptor == NULL) {
    return;
  }

  if (VIDEO_PLAYER == eVDev) {
#ifdef MEDIA_CODEC
    VideoCodecDecoder* pVideoRx = (VideoCodecDecoder*)tVideoDescriptor;
#else
    VideoReceiver* pVideoRx = (VideoReceiver*)tVideoDescriptor;
#endif
    pVideoRx->DeInit();
    delete pVideoRx;
  } else if (VIDEO_RECORDER == eVDev) {
#ifdef MEDIA_CODEC
    VideoCodecEncoder* pVideoTx = (VideoCodecEncoder*)tVideoDescriptor;
#else
    VideoTransmitter* pVideoTx = (VideoTransmitter*)tVideoDescriptor;
#endif
    pVideoTx->DeInit();
    delete pVideoTx;
  }

  return;
}

