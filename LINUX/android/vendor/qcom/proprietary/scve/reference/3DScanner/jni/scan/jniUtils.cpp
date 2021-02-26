/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "jniUtils.h"

JavaVM* gJavaVM = 0;

JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM* javaVM, void* reserved)
{
    gJavaVM = javaVM;
    JNIEnv* env;
    int result = (gJavaVM)->GetEnv((void**) &env, JNI_VERSION_1_6);

    return JNI_VERSION_1_6;
}

//-----------------------------------------------------------------------------
JNIEnv* GetEnv()
//-----------------------------------------------------------------------------
{
	JNIEnv *env;
	gJavaVM->AttachCurrentThread(&env, 0);
	return env;
}

//-----------------------------------------------------------------------------
jobject JNIGetStaticObject(const char* className, const char* fieldName, const char* signature)
//-----------------------------------------------------------------------------
{
    JNIEnv* env = GetEnv();
    jobject obj = 0;
    
    if( env != 0 )
    {
        jclass clazz = env->FindClass(className);
        obj = JNIGetStaticObject(clazz, fieldName, signature);
    }
    
    return obj;
}

//-----------------------------------------------------------------------------
jobject JNIGetStaticObject(jclass clazz, const char* fieldName, const char* signature)
//-----------------------------------------------------------------------------
{
    JNIEnv* env = GetEnv();
    jobject obj = 0;
    
    if( env != 0 )
    {
        if( clazz != 0 )
        {
            jfieldID field = env->GetStaticFieldID(clazz, fieldName, signature);
            if( field != 0 )
            {
                obj = env->GetStaticObjectField(clazz, field);
            }
        }
    }
    
    return obj;
}

//-----------------------------------------------------------------------------
void JNICallStaticVoidMethod(jclass clazz, const char* methodName, const char* signature, jobject param)
//-----------------------------------------------------------------------------
{
    JNIEnv* env = GetEnv();
    
    if( env != 0 )
    {
        if( clazz != 0 )
        {
            jmethodID method = env->GetStaticMethodID(clazz, methodName, signature);

            if( method != 0 )
            {
                env->CallStaticVoidMethod(clazz, method, param);
            }
        }
    }
}

//-----------------------------------------------------------------------------
void JNICallStaticVoidMethod(const char* className, const char* methodName, const char* signature, jobject param)
//-----------------------------------------------------------------------------
{
    JNIEnv* env = GetEnv();
    
    if( env != 0 )
    {
        jclass clazz = env->FindClass(className);
        JNICallStaticVoidMethod(clazz, methodName, signature, param);
    }
}
