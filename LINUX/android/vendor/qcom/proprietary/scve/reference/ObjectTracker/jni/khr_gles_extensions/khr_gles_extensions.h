/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef KHR_EGL_EXTENSIONS_H
#define KHR_EGL_EXTENSIONS_H

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#define __gl2_h_
#include <GLES2/gl2ext.h>

#define KHR_GLES_EXTENSIONS_PACKAGE              "com/qualcomm/qti/khronos/"

extern bool eglimage_RegisterNatives(JNIEnv *env);
extern bool glreadpixels_RegisterNatives(JNIEnv *env);
extern bool gles30helper_RegisterNatives(JNIEnv *env);

#endif // KHR_EGL_EXTENSIONS_H
