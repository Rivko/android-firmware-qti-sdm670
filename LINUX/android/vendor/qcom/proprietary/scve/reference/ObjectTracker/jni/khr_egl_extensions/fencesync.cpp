/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_egl_extensions.h"
#include "throw_exception.h"

#define EGL_KHR_FENCE_SYNC_CLASS    KHR_EGL_EXTENSIONS_PACKAGE "EGLKHRFenceSync"

static PFNEGLCREATESYNCKHRPROC pfnEglCreateSyncKHR;
static PFNEGLDESTROYSYNCKHRPROC pfnEglDestroySyncKHR;
static PFNEGLCLIENTWAITSYNCKHRPROC pfnEglClientWaitSyncKHR;
static PFNEGLGETSYNCATTRIBKHRPROC pfnEglGetSyncAttribKHR;

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    pfnEglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
    pfnEglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
    pfnEglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
    pfnEglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");

    if (pfnEglCreateSyncKHR == NULL ||
            pfnEglDestroySyncKHR == NULL ||
            pfnEglClientWaitSyncKHR == NULL ||
            pfnEglGetSyncAttribKHR == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Failed loading EGL_KHR_fence_sync extension");
        return;
    }
}

static jlong JNICALL nEglCreateSyncKHR(JNIEnv *env, jclass cls, jlong dpy, jint type,
        jintArray attrib_list)
{
    jint *attrib_list_elements = NULL;
    if (attrib_list != NULL) {
        attrib_list_elements = env->GetIntArrayElements(attrib_list, NULL);
    }
    EGLSyncKHR sync = pfnEglCreateSyncKHR((EGLDisplay)dpy, (EGLenum)type,
            (const EGLint *)attrib_list_elements);
    if (attrib_list != NULL) {
        env->ReleaseIntArrayElements(attrib_list, attrib_list_elements, JNI_ABORT);
    }
    return (jlong)sync;
}

static jboolean JNICALL nEglDestroySyncKHR(JNIEnv *env, jclass cls, jlong dpy, jlong sync)
{
    return (jboolean)pfnEglDestroySyncKHR((EGLDisplay)dpy, (EGLSyncKHR)sync);
}

static jint JNICALL nEglClientWaitSyncKHR(JNIEnv *env, jclass cls, jlong dpy, jlong sync,
        jint flags, jlong timeout)
{
    return (jint)pfnEglClientWaitSyncKHR((EGLDisplay)dpy, (EGLSyncKHR)sync, (EGLint)flags,
            (EGLTimeKHR)timeout);
}

static jboolean JNICALL nEglGetSyncAttribKHR(JNIEnv *env, jclass cls, jlong dpy, jlong sync,
        jint attribute, jintArray value)
{
    EGLint outValue = 0;
    EGLBoolean result = pfnEglGetSyncAttribKHR((EGLDisplay)dpy, (EGLSyncKHR)sync, (EGLint)attribute,
            &outValue);
    env->SetIntArrayRegion(value, 0, 1, (const jint *)&outValue);
    return (jboolean)result;
}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nEglCreateSyncKHR", "(JI[I)J", (void *)nEglCreateSyncKHR },
    { "nEglDestroySyncKHR", "(JJ)Z", (void *)nEglDestroySyncKHR },
    { "nEglClientWaitSyncKHR", "(JJIJ)I", (void *)nEglClientWaitSyncKHR },
    { "nEglGetSyncAttribKHR", "(JJI[I)Z", (void *)nEglGetSyncAttribKHR },
};

bool fencesync_RegisterNatives(JNIEnv *env) {
    jclass cls_KHREGLFenceSync = env->FindClass(EGL_KHR_FENCE_SYNC_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_KHREGLFenceSync, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
