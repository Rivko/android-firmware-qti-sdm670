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

static jmethodID gDisplay_GetHandle_V_I;
static jfieldID gSync_EGLSyncFieldID;

static inline EGLDisplay getDisplay(JNIEnv *_env, jobject o)
{
    if (!o) return EGL_NO_DISPLAY;
    return (EGLDisplay)_env->CallIntMethod(o, gDisplay_GetHandle_V_I);
}

static inline EGLContext getSync(JNIEnv *_env, jobject o)
{
    if (!o) return EGL_NO_SYNC_KHR;
    return (EGLSyncKHR)_env->GetIntField(o, gSync_EGLSyncFieldID);
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRFenceSync
 * Method:    _nativeClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRFenceSync__1nativeClassInit(
        JNIEnv *_env, jclass clazz)
{
    jclass display_class = _env->FindClass("android/opengl/EGLDisplay");
    gDisplay_GetHandle_V_I = _env->GetMethodID(display_class, "getHandle", "()I");

    jclass sync_class = _env->FindClass("com/qualcomm/qti/qces/khronos/EGLSyncKHR");
    gSync_EGLSyncFieldID = _env->GetFieldID(sync_class, "mEGLSync", "I");

    pfnEglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
    pfnEglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
    pfnEglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
    pfnEglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRFenceSync
 * Method:    _eglCreateSyncKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;I[I)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRFenceSync__1eglCreateSyncKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jint type, jintArray attrib_list)
{
    jint *attribs = NULL;
    jint sync = (jint)EGL_NO_SYNC_KHR;

    EGLDisplay _dpy = getDisplay(_env, dpy);

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

    sync = (jint)(*pfnEglCreateSyncKHR)(_dpy, type, attribs);

cleanup:
    if (attribs != NULL) {
        _env->ReleaseIntArrayElements(attrib_list, attribs, JNI_ABORT);
    }

    return sync;
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRFenceSync
 * Method:    eglDestroySyncKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;Lcom/qualcomm/qti/qces/khronos/EGLSyncKHR;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRFenceSync_eglDestroySyncKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jobject sync)
{
    EGLDisplay _dpy = getDisplay(_env, dpy);
    EGLSyncKHR _sync = getSync(_env, sync);
    return (*pfnEglDestroySyncKHR)(_dpy, _sync);
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRFenceSync
 * Method:    eglClientWaitSyncKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;Lcom/qualcomm/qti/qces/khronos/EGLSyncKHR;IJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRFenceSync_eglClientWaitSyncKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jobject sync, jint flags, jlong timeout)
{
    EGLDisplay _dpy = getDisplay(_env, dpy);
    EGLSyncKHR _sync = getSync(_env, sync);
    return (*pfnEglClientWaitSyncKHR)(_dpy, _sync, flags, timeout);
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRFenceSync
 * Method:    eglGetSyncAttribKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;Lcom/qualcomm/qti/qces/khronos/EGLSyncKHR;I[I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRFenceSync_eglGetSyncAttribKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jobject sync, jint attribute, jintArray value)
{
    if (value == NULL ||
            _env->GetArrayLength(value) != 1) {
        return EGL_FALSE;
    }

    EGLDisplay _dpy = getDisplay(_env, dpy);
    EGLSyncKHR _sync = getSync(_env, sync);
    jint *_value = (jint *)_env->GetPrimitiveArrayCritical(value, (jboolean *)0);
    jboolean result = (*pfnEglGetSyncAttribKHR)(_dpy, _sync, attribute, _value);
    _env->ReleasePrimitiveArrayCritical(value, _value, 0);

    return result;
}

#ifdef __cplusplus
}
#endif
