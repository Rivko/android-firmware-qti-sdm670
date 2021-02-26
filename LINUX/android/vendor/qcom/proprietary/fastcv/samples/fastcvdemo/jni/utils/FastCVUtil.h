/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Utility.
 *
 */

#ifndef FASTCVUTIL_H_
#define FASTCVUTIL_H_

#include <jni.h>
#include <android/log.h>
#include "FPSCounter.h"
#include "fastcv/fastcv.h"

#define LOG_TAG    "FastCVDemoJNI"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

class FastCVUtil{
private:
    FPSCounter camFPSCounter;
    float timeFilteredMs;

public:
    float getProcessTime();
    void setProcessTime(float);
    float getCameraFPS();
    void CameraTick();
    uint64_t getTimeMicroSeconds();
};

extern FastCVUtil util;

#ifdef __cplusplus
extern "C"
{
#endif

//---------------------------------------------------------------------------
/// @brief
///    Initializes the renderer
//---------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_init
(
   JNIEnv* env,
   jobject obj
);

//------------------------------------------------------------------------------
/// @brief Increments the camera frame tick count
///
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_cameraFrameTick
(
   JNIEnv*  env,
   jobject  obj
);

//------------------------------------------------------------------------------
/// @brief Gets the camera frame rate
///
/// @return Camera FPS (frames per second)
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_getCameraFPS
(
   JNIEnv*  env,
   jobject  obj
);

//------------------------------------------------------------------------------
/// @brief Retrieves FastCV processing timing from native layer.
///
/// @return FastCV processing timing, filtered, in ms.
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcvdemo_utils_FastCVUtil_getFastCVProcessTime
(
   JNIEnv*  env,
   jobject  obj
);

#ifdef __cplusplus
}
#endif

#endif /* FASTCVUTIL_H_ */
