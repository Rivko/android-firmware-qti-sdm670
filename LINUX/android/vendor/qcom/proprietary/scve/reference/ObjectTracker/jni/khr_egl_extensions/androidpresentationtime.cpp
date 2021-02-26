/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_egl_extensions.h"
#include "throw_exception.h"

#define EGL_ANDROID_PRESENTATION_TIME_CLASS     KHR_EGL_EXTENSIONS_PACKAGE "EGLAndroidPresentationTime"

typedef khronos_stime_nanoseconds_t EGLnsecsANDROID;
typedef EGLBoolean (EGLAPIENTRYP PFNEGLPRESENTATIONTIMEANDROIDPROC) (EGLDisplay dpy, EGLSurface sur, EGLnsecsANDROID time);

static PFNEGLPRESENTATIONTIMEANDROIDPROC pfnEglPresentationTimeANDROID;

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    pfnEglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC)eglGetProcAddress("eglPresentationTimeANDROID");

    if (pfnEglPresentationTimeANDROID == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Failed loading EGL_ANDROID_presentation_time extension");
        return;
    }
}

static jboolean nEglPresentationTimeANDROID(JNIEnv *env, jclass cls, jlong dpy, jlong sur,
        jlong presentationTimeNs) {
    return (jboolean)pfnEglPresentationTimeANDROID((EGLDisplay)dpy, (EGLSurface)sur,
            (EGLnsecsANDROID)presentationTimeNs);
}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nEglPresentationTimeANDROID", "(JJJ)Z", (void *)nEglPresentationTimeANDROID },
};

bool androidpresentationtime_RegisterNatives(JNIEnv *env) {
    jclass cls_EGLAndroidPresentationTime = env->FindClass(EGL_ANDROID_PRESENTATION_TIME_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_EGLAndroidPresentationTime, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
