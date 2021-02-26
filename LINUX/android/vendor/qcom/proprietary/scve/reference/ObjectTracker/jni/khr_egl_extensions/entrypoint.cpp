/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include "khr_egl_extensions.h"

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    if (!androidpresentationtime_RegisterNatives(env) ||
            !fencesync_RegisterNatives(env) ||
            !imagebase_RegisterNatives(env)) {
        return -1;
    }

    return JNI_VERSION_1_4;
}
