/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_egl_extensions.h"
#include "throw_exception.h"

#define EGL_KHR_IMAGE_BASE_CLASS    KHR_EGL_EXTENSIONS_PACKAGE "EGLKHRImageBase"

static PFNEGLCREATEIMAGEKHRPROC pfnEglCreateImageKHR;
static PFNEGLDESTROYIMAGEKHRPROC pfnEglDestroyImageKHR;

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    pfnEglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    pfnEglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");

    if (pfnEglCreateImageKHR == NULL ||
            pfnEglDestroyImageKHR == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Failed loading EGL_KHR_image_base extension");
        return;
    }
}

static jlong JNICALL nEglCreateImageKHR(JNIEnv *env, jclass cls, jlong dpy, jlong ctx, jint target,
        jint buffer, jintArray attrib_list)
{
    jint *attrib_list_elements = NULL;
    if (attrib_list != NULL) {
        attrib_list_elements = env->GetIntArrayElements(attrib_list, NULL);
    }
    EGLImageKHR image = pfnEglCreateImageKHR((EGLDisplay)dpy, (EGLContext)ctx, (EGLenum)target,
            (EGLClientBuffer)buffer, (const EGLint *)attrib_list_elements);
    if (attrib_list != NULL) {
        env->ReleaseIntArrayElements(attrib_list, attrib_list_elements, JNI_ABORT);
    }
    return (jlong)image;
}

static jboolean JNICALL nEglDestroyImageKHR(JNIEnv *env, jclass cls, jlong dpy, jlong image)
{
    return (jboolean)pfnEglDestroyImageKHR((EGLDisplay)dpy, (EGLImageKHR)image);
}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nEglCreateImageKHR", "(JJII[I)J", (void *)nEglCreateImageKHR },
    { "nEglDestroyImageKHR", "(JJ)Z", (void *)nEglDestroyImageKHR },
};

bool imagebase_RegisterNatives(JNIEnv *env) {
    jclass cls_KHREGLImageBase = env->FindClass(EGL_KHR_IMAGE_BASE_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_KHREGLImageBase, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
