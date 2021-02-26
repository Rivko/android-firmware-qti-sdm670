/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __IZAT_MANAGER_UTILS_JNI_H__
#define __IZAT_MANAGER_UTILS_JNI_H__

#ifdef __ANDROID__
#include <jni.h>
#endif
#include "utils/Log.h"

#define GNSS_VENDOR_SERVICE_INSTANCE "gnss_vendor"

#define TO_HIDL_SERVICE()   ALOGV("[%s][%d] [HC] =>> [HS]", __func__, __LINE__)
#define FROM_HIDL_SERVICE() ALOGV("[%s][%d] [HC] <<= [HS]", __func__, __LINE__)

JNIEnv* getCallbackEnv();
void checkAndClearExceptionsFromCallback(JNIEnv *env, const char* methodName);

#endif // #ifndef __IZAT_MANAGER_UTILS_JNI_H__
