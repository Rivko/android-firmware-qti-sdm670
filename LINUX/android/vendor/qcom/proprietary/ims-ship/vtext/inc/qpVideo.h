/**
============================
Copyright (c)  2004-2012,2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
*/

#ifndef __QPVIDEO_H
#define __QPVIDEO_H

//#include <qpdpl.h>
//#include <qpMultimedia.h>
//#include <VideoCommon.h>
#include <VideoCodecBase.h>
#include <VTPlayer.h>
#include <VTRecorder.h>

using namespace android;
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * \var typedef void* QP_VIDEO_DESC;
 * QP_VIDEO_DESC is the video descriptor passed by the video module.
 */
typedef void* QP_VIDEO_DESC;

/**
 * \var typedef void (*QP_VIDEO_CALLBACK)(QPE_VIDEO_MSG tVideoMsg,
 *                                          void* pParam1,
 *                                          QPUINT32 iParam2,
 *                                          void* pUserData)
 * QP_VIDEO_CALLBACK is function pointer to the video callback function.
 */
//typedef void (*QP_VIDEO_CALLBACK)(QPE_VIDEO_MSG tVideoMsg, void* pFrameData,
//  QPUINT32 eDevType,QPBOOL iStatus, void* pUserData);
/**
 * \fn QP_VIDEO_CALLBACK
 * \brief Video callback function
 *
 * This is the callback function called by Video Recorder and player
 * when events/messages needs to be reported to the application.
 *
 * \param tVideoMsg enum specifying the video message.\n
 *                  See \ref QPE_VIDEO_MSG.
 * \param pParam1 Pointer to the data.\n
 * The data interpretation varies depending on tVideoMsg.\n
 * If tVideoMsg = VIDEO_MSG_ERROR pParam1 will contain error information\n
 * If tVideoMsg = VIDEO_MSG_RECORDED_DATA pParam1 is pointer to
 * \ref QP_MULTIMEDIA_FRAME.\n
 * If tVideoMsg = VIDEO_MSG_DATA_PLAYED or VIDEO_MSG_RESUME_PLAYING
 * pParam1 contains NULL.
 * \param iParam2 Contains the number of encoded frames available in memory
 * pointed by pParam1.\n
 * iParam2 is valid only when tVideoMsg = VIDEO_MSG_RECORDED_DATA in all
 * other cases it will be 0.
 * \param pUserData Pointer to the user data, which is passed in
 * \ref qpVideoRecordStart or \ref qpVideoPlayStart.
 * \return \ref void
 * \retval None
 */

//Init the video resources. Returns the video descriptor. Returns NULL on error
/**
 * \fn QP_IMPORT_C QP_VIDEO_DESC qpVideoInitialize(QP_VIDEO_CALLBACK tVideoCallBack,
 void* pUserData,
 QPE_VIDEO_DEV eVDev,
 QP_VIDEO_CONFIG* pCodecConfig)
 * \brief Video Initialization function
 *
 * This function initializes the video resource.
 *
 * \param tVideoCallBack Function pointer to the video callback function
 * that will be called when video has to deliver encoded frames or any
 * other messages.\n
 * See \ref QP_VIDEO_CALLBACK.
 * \param pUserData A void pointer, which contains the user information.
 * This pointer will be passed back to the user in the callback.\n
 * \param eVDev Video Device Type. \n
 * See \ref QPE_VIDEO_DEV
 * \param pCodecConfig Pointer to configuration structure specified in
 * QP_VIDEO_CONFIG.\n
 * See \ref QP_VIDEO_CONFIG.
 * \return \ref QP_VIDEO_DESC
 * \retval A valid descriptor to the initialized video module otherwise
 * returns NULL.
 * \remarks None
 */
QP_IMPORT_C void *qpVideoInitialize(QP_VIDEO_CALLBACK tVideoCallBack,
                                    void* pUserData, QPE_VIDEO_DEV eVDev, QP_VIDEO_CONFIG* pCodecConfig);

//Cleanup the video resources
/**
 * \fn QP_IMPORT_C void qpVideoUninitialize(QP_VIDEO_DESC tVideoDescriptor,
 QPE_VIDEO_DEV eVDev)
 * \brief Video Uninitialization function
 *
 * This function uninitializes and releases the video resource.
 *
 * \param tVideoDescriptor Video Descriptor.\n
 * \param eVDev Video Device Type. \n
 * See \ref QPE_VIDEO_DEV
 * \return \ref QP_VIDEO_DESC
 * See \ref QP_VIDEO_DESC.
 * \return \ref void
 * \retval None
 * \remarks None
 */
QP_IMPORT_C void qpVideoUninitialize(VideoCodecBase *tVideoDescriptor,
                                     QPE_VIDEO_DEV eVDev);

CAMERA_HAL_VERSION checkForCamera2Support();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
