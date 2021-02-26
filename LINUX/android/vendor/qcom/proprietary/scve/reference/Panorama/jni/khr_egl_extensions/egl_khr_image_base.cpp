/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * Expose the EGL_KHR_image_base extension to Java via JNI
 * This is intended to work with the android.opengl.EGL14 class
 */

#include <jni.h>
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRImageBase
 * Method:    _nativeClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_EGLKHRImageBase__1nativeClassInit(
        JNIEnv *_env, jclass clazz)
{
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRImageBase
 * Method:    _eglCreateImageKHR
 * Signature: (JJII[I)I
 */
JNIEXPORT jlong JNICALL Java_com_qualcomm_qti_khronos_EGLKHRImageBase__1eglCreateImageKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jlong ctx, jint target, jint buffer, jintArray attrib_list)
{
    jint *attribs = NULL;
    jint image = (jlong)EGL_NO_IMAGE_KHR;

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

    image = (jint)eglCreateImageKHR((EGLDisplay)dpy, (EGLContext)ctx, target,
            (EGLClientBuffer)buffer, attribs);

cleanup:
    if (attribs != NULL) {
        _env->ReleaseIntArrayElements(attrib_list, attribs, JNI_ABORT);
    }

    return image;
}

/*
 * Class:     com_qualcomm_qti_khronos_EGLKHRImageBase
 * Method:    _eglDestroyImageKHR
 * Signature: (JJ)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_khronos_EGLKHRImageBase__1eglDestroyImageKHR(
        JNIEnv *_env, jclass clazz, jlong dpy, jlong image)
{
    return eglDestroyImageKHR((EGLDisplay)dpy, (EGLImageKHR)image);
}

#ifdef __cplusplus
}
#endif
