/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_gles_extensions.h"

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    if (!eglimage_RegisterNatives(env) ||
            !glreadpixels_RegisterNatives(env) ||
            !gles30helper_RegisterNatives(env)) {
        return -1;
    }

    return JNI_VERSION_1_4;
}
