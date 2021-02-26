/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Utility.
 *
 */

#include <time.h>
#include "FastCVUtil.h"

FastCVUtil util;

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_init
(
   JNIEnv* env,
   jobject obj
)
{
   char sVersion[32];

   fcvSetOperationMode( (fcvOperationMode) FASTCV_OP_PERFORMANCE );

   fcvGetVersion(sVersion, 32);
   DPRINTF( "Using FastCV version %s \n", sVersion );

   return;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_cameraFrameTick
(
   JNIEnv*  env,
   jobject  obj
)
{
   util.CameraTick();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_getCameraFPS
(
   JNIEnv*  env,
   jobject  obj
)
{
   return util.getCameraFPS();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_getFastCVProcessTime
(
   JNIEnv*  env,
   jobject  obj
)
{
   return (jfloat) util.getProcessTime();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

float FastCVUtil::getProcessTime(){
    return timeFilteredMs;
}

void FastCVUtil::setProcessTime(float f){
    timeFilteredMs = f;
}

float FastCVUtil::getCameraFPS(){
    return camFPSCounter.GetFilteredFPS();
}

void FastCVUtil::CameraTick(){
    camFPSCounter.FrameTick();
}

//------------------------------------------------------------------------------
/// @brief Returns current time in microseconds
///
/// @return Time in microseconds
//------------------------------------------------------------------------------
uint64_t FastCVUtil::getTimeMicroSeconds()
{
   struct timespec t;
   t.tv_sec = t.tv_nsec = 0;

   clock_gettime(CLOCK_REALTIME, &t);
   return (uint64_t)t.tv_sec * 1000000ULL + t.tv_nsec / 1000L;
}
