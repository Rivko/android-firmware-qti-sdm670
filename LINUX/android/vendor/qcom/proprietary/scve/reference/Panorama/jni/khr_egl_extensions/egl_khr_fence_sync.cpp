/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * Expose the EGL_KHR_fence_sync extension to Java via JNI
 * This is intended to work with the android.opengl.EGL14 class
 */
#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

// Note: Fence Sync extension not exported from NDK libEGL.so
static PFNEGLCREATESYNCKHRPROC pfnEglCreateSyncKHR;
static PFNEGLDESTROYSYNCKHRPROC pfnEglDestroySyncKHR;
static PFNEGLCLIENTWAITSYNCKHRPROC pfnEglClientWaitSyncKHR;
static PFNEGLGETSYNCATTRIBKHRPROC pfnEglGetSyncAttribKHR;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRFenceSync
 * Method:    _nativeClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_EGLKHRFenceSync__1nativeClassInit(
        JNIEnv *_env, jclass clazz)
{
    pfnEglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
    pfnEglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
    pfnEglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
    pfnEglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRFenceSync
 * Method:    _eglCreateSyncKHR
 * Signature: (JI[I)I
 */
JNIEXPORT jlong JNICALL Java_com_qualcomm_qti_khronos_EGLKHRFenceSync__1eglCreateSyncKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jint type, jintArray attrib_list)
{
    jint *attribs = NULL;
    jint sync = (jint)EGL_NO_SYNC_KHR;

    if (attrib_list != NULL) {
        int num_attribs = _env->GetArrayLength(attrib_list);
        if (num_attribs == 0) {
            goto cleanup;
        }

        attribs = (jint *)_env->GetIntArrayElements(attrib_list, NULL);
        if (attribs[num_attribs - 1] != EGL_NONE) {
            goto cleanup;
        }
    }

    sync = (jint)(*pfnEglCreateSyncKHR)((EGLDisplay)dpy, type, attribs);

cleanup:
    if (attribs != NULL) {
        _env->ReleaseIntArrayElements(attrib_list, attribs, JNI_ABORT);
    }

    return sync;
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRFenceSync
 * Method:    _eglDestroySyncKHR
 * Signature: (JJ)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_khronos_EGLKHRFenceSync__1eglDestroySyncKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jlong sync)
{
    return (*pfnEglDestroySyncKHR)((EGLDisplay)dpy, (EGLSyncKHR)sync);
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRFenceSync
 * Method:    _eglClientWaitSyncKHR
 * Signature: (JJIJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_khronos_EGLKHRFenceSync__1eglClientWaitSyncKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jlong sync, jint flags, jlong timeout)
{
    return (*pfnEglClientWaitSyncKHR)((EGLDisplay)dpy, (EGLSyncKHR)sync, flags, timeout);
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRFenceSync
 * Method:    _eglGetSyncAttribKHR
 * Signature: (JJI[I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_khronos_EGLKHRFenceSync__1eglGetSyncAttribKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jlong sync, jint attribute, jintArray value)
{
    if (value == NULL ||
            _env->GetArrayLength(value) != 1) {
        return EGL_FALSE;
    }

    jint *_value = (jint *)_env->GetPrimitiveArrayCritical(value, (jboolean *)0);
    jboolean result = (*pfnEglGetSyncAttribKHR)((EGLDisplay)dpy, (EGLSyncKHR)sync, attribute, _value);
    _env->ReleasePrimitiveArrayCritical(value, _value, 0);

    return result;
}

#ifdef __cplusplus
}
#endif
