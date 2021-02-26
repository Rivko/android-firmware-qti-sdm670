/*
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Implementation of About native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <stdlib.h>
#include <android/log.h>
#include <fastcv/fastcv.h>
#include "About.h"

//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "FastCVSample"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

//==============================================================================
// Function Definitions
//==============================================================================

JNIEXPORT jstring JNICALL Java_com_qualcomm_fastcorner_About_getFastCVVersion
(
   JNIEnv* env,
   jobject obj
)
{
   char sVersion[32];
   fcvGetVersion(sVersion, 32);
   DPRINTF("FastCV version %s", sVersion);
   return env->NewStringUTF(sVersion);
}

