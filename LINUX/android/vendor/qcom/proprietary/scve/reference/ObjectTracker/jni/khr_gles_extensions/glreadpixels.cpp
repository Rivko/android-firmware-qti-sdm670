/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_gles_extensions.h"
#include "throw_exception.h"

#define GLES30_EXT_CLASS            KHR_GLES_EXTENSIONS_PACKAGE "GLES30Ext"

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
}

static void JNICALL nGlReadPixels(JNIEnv *env, jclass cls, jint x, jint y, jint width, jint height,
        jint format, jint type, jint offset)
{
    GLint buffer;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buffer);
    if (buffer == 0) {
        throwException(env, RUNTIME_EXCEPTION, "This version of glReadPixels can only be used "
                "when a PBO is bound to GL_PIXEL_PACK_BUFFER");
        return;
    }

    glReadPixels((GLint)x, (GLint)y, (GLint)width, (GLint)height, (GLenum)format, (GLenum)type,
            (GLvoid *)offset);
}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nGlReadPixels", "(IIIIIII)V", (void *)nGlReadPixels },
};

bool glreadpixels_RegisterNatives(JNIEnv *env) {
    jclass cls_GLES30Ext = env->FindClass(GLES30_EXT_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_GLES30Ext, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
