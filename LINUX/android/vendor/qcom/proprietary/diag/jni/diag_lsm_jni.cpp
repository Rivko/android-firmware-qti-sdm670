/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc
              Diag JNI Support

GENERAL DESCRIPTION

Implementation of Diag JNI Interface

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "comdef.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "../include/diag_lsm.h"
#include "com_qualcomm_qti_diagservice_diag_lsm.h"
#include "signal.h"
#include <JNIHelp.h>
#include <cutils/log.h>
#include "common_log.h"
#define JNI_LOG_TAG "Diag_LSM_JNI"
#define  ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,JNI_LOG_TAG,__VA_ARGS__)


JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_diagservice_diag_1lsm_Diag_1LSM_1Init
			(JNIEnv* jenv, jclass cls, jlong arg1)
{
	jboolean jresult;
	boolean result;
	(void)jenv;
	(void)cls;
	(void)arg1;

	ALOGE("diag:calling LSM Init");
	result = Diag_LSM_Init(NULL);
	jresult = (jboolean)result;
	return result;
}

JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_diagservice_diag_1lsm_Diag_1LSM_1DeInit
				(JNIEnv* jenv, jclass cls)
{
	jboolean jresult;
	boolean result;
	(void)jenv;
	(void)cls;

	result = Diag_LSM_DeInit();
	jresult = (jboolean)result;
	return result;
}
