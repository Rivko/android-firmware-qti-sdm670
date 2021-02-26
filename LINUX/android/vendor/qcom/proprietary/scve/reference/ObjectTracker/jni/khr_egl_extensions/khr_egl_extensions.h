/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef KHR_EGL_EXTENSIONS_H
#define KHR_EGL_EXTENSIONS_H

#include <EGL/egl.h>
#include <EGL/eglext.h>

#define KHR_EGL_EXTENSIONS_PACKAGE              "com/qualcomm/qti/khronos/"

extern bool androidpresentationtime_RegisterNatives(JNIEnv *env);
extern bool fencesync_RegisterNatives(JNIEnv *env);
extern bool imagebase_RegisterNatives(JNIEnv *env);

#endif // KHR_EGL_EXTENSIONS_H
