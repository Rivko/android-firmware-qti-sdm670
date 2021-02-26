/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
extern JavaVM* gJavaVM;
JNIEnv* GetEnv();
jobject JNIGetStaticObject(const char* className, const char* fieldName, const char* signature);
jobject JNIGetStaticObject(jclass clazz, const char* fieldName, const char* signature);
void JNICallStaticVoidMethod(jclass clazz, const char* methodName, const char* signature, jobject param);
void JNICallStaticVoidMethod(const char* className, const char* methodName, const char* signature, jobject param);