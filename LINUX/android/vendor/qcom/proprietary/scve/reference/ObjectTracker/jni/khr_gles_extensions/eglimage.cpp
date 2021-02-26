/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_gles_extensions.h"
#include "throw_exception.h"

#define GLOES2_EGL_IMAGE_CLASS      KHR_GLES_EXTENSIONS_PACKAGE "GLOES2EGLImage"

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC pfnGlEGLImageTargetTexture2DOES;
static PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC pfnGlEGLImageTargetRenderbufferStorageOES;

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    pfnGlEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
            eglGetProcAddress("glEGLImageTargetTexture2DOES");
    pfnGlEGLImageTargetRenderbufferStorageOES = (PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)
            eglGetProcAddress("glEGLImageTargetRenderbufferStorageOES");

    if (pfnGlEGLImageTargetTexture2DOES == NULL ||
            pfnGlEGLImageTargetRenderbufferStorageOES == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Failed loading GL_OES2_egl_image extension");
        return;
    }
}

static void JNICALL nGlEGLImageTargetTexture2DOES(JNIEnv *env, jclass cls, jint target, jlong image)
{
    pfnGlEGLImageTargetTexture2DOES((GLenum)target, (GLeglImageOES)image);
}

static void JNICALL nGlEGLImageTargetRenderbufferStorageOES(JNIEnv *env, jclass cls, jint target,
        jlong image)
{
    pfnGlEGLImageTargetRenderbufferStorageOES((GLenum)target, (GLeglImageOES)image);
}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nGlEGLImageTargetTexture2DOES", "(IJ)V", (void *)nGlEGLImageTargetTexture2DOES },
    { "nGlEGLImageTargetRenderbufferStorageOES", "(IJ)V", (void *)nGlEGLImageTargetRenderbufferStorageOES },
};

bool eglimage_RegisterNatives(JNIEnv *env) {
    jclass cls_GLOES2EglImage = env->FindClass(GLOES2_EGL_IMAGE_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_GLOES2EglImage, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
