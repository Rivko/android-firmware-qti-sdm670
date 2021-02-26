/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2016-2017 by Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Diag JNI Support

GENERAL DESCRIPTION

Implementation of Diag JNI Interface

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "comdef.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "../include/diag_lsm_dci.h"
#include "../include/diag_lsm.h"
#include "com_qualcomm_qti_diagservice_diag_dci.h"
#include "signal.h"
#include <JNIHelp.h>
#include <cutils/log.h>
#include "common_log.h"

#define JNI_LOG_TAG "Diag_JNI_Lib"
#define  ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,JNI_LOG_TAG,__VA_ARGS__)

static JavaVM* gJavaVM;
static jobject cb_class_object;
static jclass cb_class;
static void diag_process_dci_log_cb_handler(unsigned char* ptr, int len);
static void diag_process_dci_event_cb_handler(unsigned char* ptr, int len);
static void* rsp_param = NULL;
static jbyteArray rsp_buf = NULL;


static void diag_process_pkt_rsp_cb_handler(unsigned char* ptr, int len, void* data_ptr)
{
	int status;
	JNIEnv* env = NULL;
	jbyteArray retArray;
	boolean isAttached = FALSE;

	status = gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	if (status < 0) {
		status = gJavaVM->AttachCurrentThread(&env, NULL);
		if (status < 0) {
			ALOGE("In %s failed to attach "
				"current thread",__func__);
		return;
		}
		isAttached = TRUE;
	}

	jint jlen = (jint)len;
	jlong data = (jlong)(*(long*)data_ptr);
	jmethodID method = env->GetMethodID(
				cb_class, "diag_dci_pkt_rsp_cb", "([BIJ)V");
	if (!method) {
		LOGE("In %s: failed to get method ID",__func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	retArray = env->NewByteArray(len);
	if (!retArray) {
		LOGE("In %s: Failed to allocate memory", __func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	void* temp = env->GetPrimitiveArrayCritical((jarray)retArray, 0);
	memcpy(temp, ptr, len);
	env->ReleasePrimitiveArrayCritical(retArray, temp, 0);

	env->CallVoidMethod(cb_class_object, method, retArray, jlen);
	if (isAttached)
		gJavaVM->DetachCurrentThread();

}

static void diag_process_dci_log_cb_handler(unsigned char* ptr, int len)
{
	int status;
	JNIEnv* env = NULL;
	jbyteArray retArray;
	boolean isAttached = FALSE;

	status = gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	if(status < 0) {
		status = gJavaVM->AttachCurrentThread(&env, NULL);
		if (status < 0) {
		ALOGE("In %s failed to attach "
			"current thread", __func__);
		return;
		}
		isAttached = TRUE;
	}

	jint jlen = (jint)len;

	jmethodID method = env->GetMethodID(
				cb_class, "diag_dci_log_cb", "([BI)V");
	if (!method) {
		LOGE("In %s failed to get method ID", __func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	retArray = env->NewByteArray(len);
	if (!retArray) {
		LOGE("In %s Failed to allocate memory", __func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	void* temp = env->GetPrimitiveArrayCritical((jarray)retArray, 0);
	memcpy(temp, ptr, len);
	env->ReleasePrimitiveArrayCritical(retArray, temp, 0);
	env->CallVoidMethod(cb_class_object, method, retArray, jlen);
	if (isAttached)
		gJavaVM->DetachCurrentThread();
}

static void diag_process_dci_event_cb_handler(unsigned char* ptr, int len)
{
	int status;
	JNIEnv* env = NULL;
	jbyteArray retArray;
	boolean isAttached = FALSE;

	status = gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	if (status < 0) {
		status = gJavaVM->AttachCurrentThread(&env, NULL);
		if (status < 0) {
			ALOGE("In %s failed to attach "
				"current thread", __func__);
			return;
		}
		isAttached = TRUE;
	}

	jint jlen = (jint)len;
	jmethodID method = env->GetMethodID(
				cb_class, "diag_dci_event_cb", "([BI)V");
	if(!method) {
		LOGE("In %s failed to get method ID", __func__);
		if(isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	retArray = env->NewByteArray(len);
	if (!retArray) {
		LOGE("In %s Failed to allocate memory", __func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}
	void* temp = env->GetPrimitiveArrayCritical((jarray)retArray, 0);
	memcpy(temp, ptr, len);
	env->ReleasePrimitiveArrayCritical(retArray, temp, 0);
	env->CallVoidMethod(cb_class_object, method, retArray, jlen);
	if(isAttached)
		gJavaVM->DetachCurrentThread();
}

static void diag_process_dci_signal_cb_handler(int signal) {
	int status;
	JNIEnv* env = NULL;
	jbyteArray retArray;
	boolean isAttached = FALSE;

	status = gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
	if(status < 0) {
		status = gJavaVM->AttachCurrentThread(&env, NULL);
		if(status < 0) {
			ALOGE("In %s failed to attach "
				"current thread", __func__);
			return;
		}
	isAttached = TRUE;
	}

	jint dci_signal = (jint)signal;

	jmethodID method = env->GetMethodID(
				cb_class, "diag_dci_signal_cb_handler", "([I)V");
	if (!method) {
		LOGE("In %s failed to get method ID", __func__);
		if (isAttached)
			gJavaVM->DetachCurrentThread();
		return;
	}

	env->CallVoidMethod(cb_class_object, method, dci_signal);
	if(isAttached)
		gJavaVM->DetachCurrentThread();

}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1register_1dci_1client
			(JNIEnv* jenv, jclass jcls, jlong jarg1,
			jint jarg2, jint jarg3)
{
	jint jresult;
	int arg1;
	diag_dci_peripherals* arg2 = NULL;
	int arg3 = 0;
	void* arg4 = (void*)0;
	int result;
	int client_id = -1;
	(void)jenv;
	(void)jcls;
	arg2 = (diag_dci_peripherals*)&jarg1;
	arg3 = (int)jarg2;
	arg4 = (void*)&jarg3;
	result = (int)diag_register_dci_client(&arg1, arg2, arg3, arg4);
	if (result != DIAG_DCI_NO_ERROR) {
		ALOGE("In %s failed with err:%d", __func__, result);
		return client_id;
	}
	client_id = arg1;
	jresult = (jint)client_id;

	return jresult;
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1get_1dci_1support_1list_1proc
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	diag_dci_peripherals arg2;
	int result;

	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;

	result = (int)diag_get_dci_support_list_proc(arg1, &arg2);
	if (result != DIAG_DCI_NO_ERROR)
		ALOGE("In %s failed with err:%d", __func__, result);
	jresult = (jint)arg2;

	return jresult;
}
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1dci_1get_1version
			(JNIEnv* jenv, jclass jcls)
{
	jint jresult = 0;
	int result;

	(void)jenv;
	(void)jcls;
	result = (int)diag_dci_get_version();
	jresult = (jint)result;
	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1dci_1set_1version
			(JNIEnv* jenv, jclass jcls, jint jarg1, jint jarg2)
{
	jint jresult = 0;
	int arg1;
	int arg2;
	int result;
	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	arg2 = (int)jarg2;
	result = (int)diag_dci_set_version(arg1, arg2);
	jresult = (jint)result;

	return jresult;
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1send_1dci_1async_1req
			(JNIEnv* jenv, jclass jcls, jint jarg1,
			jbyteArray jarg2, jint jarg3, jlong jarg4)
{
	jint jresult = 0;
	int arg1;
	unsigned char* arg2 = NULL;
	int arg3;
	unsigned char* arg4 = NULL;
	int arg5;
	void (* arg6)(unsigned char*, int, void*) = (void (* )(unsigned char*, int, void*))0;
	void* arg7 = (void*)0;
	int result;

	(void)jenv;
	(void)jcls;

	unsigned char* buffer = (unsigned char*)jenv->GetByteArrayElements(jarg2, NULL);
	arg1 = (int)jarg1;
	arg2 = buffer;
	arg3 = (int)jarg3;

	arg4 = (unsigned char*)rsp_buf;
	arg5 = IN_BUF_SIZE;
	arg6 = diag_process_pkt_rsp_cb_handler;
	rsp_param = malloc(sizeof(long));
	if (!rsp_param) {
		ALOGE("In %s malloc failed for rsp_param\n", __func__);
		return jresult;
	}
	*(long *)(rsp_param) = jarg4;
	arg7 = rsp_param;
	result = (int)diag_send_dci_async_req(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	jresult = (jint)result;
	jenv->ReleaseByteArrayElements(jarg2, (jbyte*)buffer, JNI_ABORT);
	return jresult;
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1release_1dci_1client
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int* arg1 = (int*)0;
	int result;

	(void)jenv;
	(void)jcls;
	arg1 = (int*)&jarg1;
	result = (int)diag_release_dci_client(arg1);
	jresult = (jint)result;

	return jresult;
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1log_1stream_1config
			(JNIEnv* jenv, jclass jcls,
			jint jarg1, jint jarg2,
			jintArray jarg3, jint jarg4)
{
	jint jresult = 0;
	int arg1;
	int arg2;
	uint16* arg3;
	int arg4;
	int result;
	jint* data;
	jboolean isCopy = JNI_FALSE;
	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;
	arg2 = (int)jarg2;
	data = jenv->GetIntArrayElements(jarg3, &isCopy);
	jenv->ReleaseIntArrayElements(jarg3, data, JNI_ABORT);
	arg3 = (uint16*)data;
	arg4 = (int)jarg4;
	result = (int)diag_log_stream_config(arg1, arg2, arg3, arg4);
	jresult = (jint)result;

	return jresult;
}
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1event_1stream_1config
			(JNIEnv* jenv, jclass jcls, jint jarg1, jint jarg2, jintArray jarg3, jint jarg4)
{
	jint jresult = 0;
	int arg1;
	int arg2;
	int* arg3;
	int arg4;
	int result;
	jint* data;
	jboolean isCopy = JNI_FALSE;
	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;
	arg2 = (int)jarg2;
	data = jenv->GetIntArrayElements(jarg3, &isCopy);
	jenv->ReleaseIntArrayElements(jarg3, data, JNI_ABORT);
	arg3 = (int*)data;
	arg4 = (int)jarg4;
	result = (int)diag_event_stream_config(arg1, arg2, arg3, arg4);
	jresult = (jint)result;

	return jresult;
}
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1register_1dci_1stream_1proc
			(JNIEnv* jenv, jclass jcls, jobject obj, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	void (* arg2)(unsigned char*, int) = (void (* )(unsigned char*, int))0;
	void (* arg3)(unsigned char*, int) = (void (* )(unsigned char*, int))0;
	int result;

	(void)jenv;
	jclass clazz;
	clazz = jenv->GetObjectClass(obj);
	cb_class = (jclass)jenv->NewGlobalRef(jcls);
	cb_class_object = (jobject)jenv->NewGlobalRef(obj);
	arg1 = (int)jarg1;
	arg2 = diag_process_dci_log_cb_handler;
	arg3 = diag_process_dci_event_cb_handler;
	result = (int)diag_register_dci_stream_proc(arg1, arg2, arg3);
	jresult = (jint)result;
	return jresult;
}

JNIEXPORT jobject JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1get_1health_1stats_1proc
				(JNIEnv* jenv, jclass jcls, jint jarg1, jint jarg2)
{
	jint jresult = 0;
	int arg1;
	struct diag_dci_health_stats arg2 = {0, 0, 0, 0, 0};
	int arg3;
	int result;

	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	arg3 = (int)jarg2;
	jclass health_stats_cls = jenv->FindClass("com/qti/diagservice/diag_dci_health_stats");
	jmethodID health_stats_id = jenv->GetMethodID(health_stats_cls, "<init>", "(IIIIII)V");

	result = (int)diag_get_health_stats_proc(arg1, &arg2, arg3);

	return jenv->NewObject(health_stats_cls, health_stats_id,
		(jint)result,(jint)arg2.dropped_logs,
		(jint)arg2.dropped_events, (jint)arg2.received_logs,
		(jint) arg2.received_events,(jint)arg2.reset_status);
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1get_1log_1status
		(JNIEnv* jenv, jclass jcls, jint jarg1, jlong jarg2)
{
	jint jresult = 0;
	int arg1;
	uint16 arg2;
	boolean arg3;
	int result;
	const char* ret_val;
	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;
	arg2 = (uint16)jarg2;
	result = (int)diag_get_log_status(arg1, arg2, &arg3);
	if (result != DIAG_DCI_NO_ERROR)
		ALOGE("In %s failed with err:%d", __func__, result);
	jresult = (jint)arg3;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1get_1event_1status
			(JNIEnv* jenv, jclass jcls, jint jarg1, jlong jarg2)
{
	jint jresult = 0;
	int arg1;
	uint16 arg2;
	boolean arg3;
	int result;
	const char* ret_val;
	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;
	arg2 = jarg2;

	result = (int)diag_get_event_status(arg1, arg2, &arg3);
	if (result != DIAG_DCI_NO_ERROR)
		ALOGE("In %s failed with err:%d", __func__, result);
	jresult = (jint)arg3;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1disable_1all_1logs
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	int result;

	(void)jenv;
	(void)jcls;
	arg1 = (int)jarg1;
	result = (int)diag_disable_all_logs(arg1);
	jresult = (jint)result;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1disable_1all_1events
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	int result;

	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	result = (int)diag_disable_all_events(arg1);
	jresult = (jint)result;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1dci_1vote_1real_1time
			(JNIEnv* jenv, jclass jcls, jint jarg1, jint jarg2)
{
	jint jresult = 0;
	int arg1;
	int arg2;
	int result;

	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	arg2 = (int)jarg2;
	result = (int)diag_dci_vote_real_time(arg1, arg2);
	jresult = (jint)result;

	return jresult;
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1dci_1get_1real_1time_1status_1proc
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	int arg2 = 0;
	int result;

	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	result = (int)diag_dci_get_real_time_status_proc(arg1, &arg2);
	if (result != DIAG_DCI_NO_ERROR)
		ALOGE("In %s failed with err:%d", __func__, result);
	jresult = (jint)arg2;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1register_1dci_1signal_1data
			(JNIEnv* jenv, jclass jcls, jint jarg1, jint jarg2)
{
	jint jresult = 0;
	int arg1;
	int arg2;
	int result;
	struct sigaction dci_sig_action;

	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	arg2 = (int)jarg2;
	sigemptyset(&dci_sig_action.sa_mask);
	dci_sig_action.sa_handler = diag_process_dci_signal_cb_handler;
	dci_sig_action.sa_flags = 0;
	sigaction(arg2, &dci_sig_action, NULL);
	result = (int)diag_register_dci_signal_data(arg1, arg2);
	jresult = (jint)result;

	return jresult;
}


JNIEXPORT jint JNICALL Java_com_qualcomm_qti_diagservice_diag_1dci_diag_1deregister_1dci_1signal_1data
			(JNIEnv* jenv, jclass jcls, jint jarg1)
{
	jint jresult = 0;
	int arg1;
	int result;
	(void)jenv;
	(void)jcls;

	arg1 = (int)jarg1;
	result = (int)diag_deregister_dci_signal_data(arg1);
	jresult = (jint)result;

	return jresult;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env;
	(void)reserved;
	gJavaVM = vm;
	ALOGE("diag: calling JNI on load");
	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Failed to get the environment using GetEnv()");
		return -1;
	}
	rsp_buf = (jbyteArray)malloc(IN_BUF_SIZE);
	return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
	JNIEnv* env;
	(void)reserved;
	ALOGE("diag: calling JNI on unload");
	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("Failed to get the environment using GetEnv()");
		return;
	}
	env->DeleteGlobalRef(cb_class);
	env->DeleteGlobalRef(cb_class_object);
	free(rsp_buf);
	rsp_buf = NULL;
	free(rsp_param);
	rsp_param = NULL;
	return;
}
