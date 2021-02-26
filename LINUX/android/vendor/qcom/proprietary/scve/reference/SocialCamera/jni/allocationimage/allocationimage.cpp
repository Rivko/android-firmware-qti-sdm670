/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#define CLASS_IMAGE_ALLOCATION_YUV_420_888_PLANE "android/media/ImageAllocationYUV_420_888$Plane"
#define TAG "allocationimage"

static jobject JNICALL nLockBitmap(JNIEnv *env, jclass cls, jobject bitmap)
{
    if (bitmap == NULL) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "bitmap is null");
        return NULL;
    }

    AndroidBitmapInfo info;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &info)) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "Failed retrieving bitmap info");
        return NULL;
    }

    jobject buf = NULL;
    void *addr = NULL;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &addr)) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "Failed locking bitmap pixels");
        return NULL;
    }

    return env->NewDirectByteBuffer(addr, info.stride * info.height);
}

static void JNICALL nUnlockBitmap(JNIEnv *env, jclass cls, jobject bitmap)
{
    if (bitmap == NULL) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "bitmap is null");
        return;
    }

    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_unlockPixels(env, bitmap)) {
        __android_log_write(ANDROID_LOG_ERROR, TAG, "Failed unlocking bitmap pixels");
        return;
    }
}

///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod methods[] = {
    { "nLockBitmap", "(Landroid/graphics/Bitmap;)Ljava/nio/ByteBuffer;", (void *)nLockBitmap },
    { "nUnlockBitmap", "(Landroid/graphics/Bitmap;)V", (void *)nUnlockBitmap },
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    jclass cls = env->FindClass(CLASS_IMAGE_ALLOCATION_YUV_420_888_PLANE);
    if (env->ExceptionCheck()) return -1;
    env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return -1;

    return JNI_VERSION_1_4;
}
