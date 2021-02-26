/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include <string.h>
#include <android/bitmap.h>
#include "khr_gles_extensions.h"
#include "throw_exception.h"

#define GLES30_HELPER_CLASS         KHR_GLES_EXTENSIONS_PACKAGE "GLES30Helper"

static int getBytesPerPixel(AndroidBitmapInfo &info)
{
    switch (info.format) {
    case ANDROID_BITMAP_FORMAT_RGBA_8888:
        return 4;

    case ANDROID_BITMAP_FORMAT_RGB_565:
    case ANDROID_BITMAP_FORMAT_RGBA_4444:
        return 2;

    default:
        return 1;
    }
}

static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
}

static void JNICALL nCopyBitmapToPbo(JNIEnv *env, jclass cls, jobject bitmap, jint pbo) {
    AndroidBitmapInfo info;
    uint8_t *bitmapData = NULL;
    uint8_t *buffer = NULL;
    int rowBytes = 0;

    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &info)) {
        throwException(env, RUNTIME_EXCEPTION, "Error getting bitmap info");
        return;
    }
    rowBytes = info.width * getBytesPerPixel(info);

    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap,
            (void **)&bitmapData)) {
        throwException(env, RUNTIME_EXCEPTION, "Failed locking bitmap pixels");
        goto done;
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    buffer = (uint8_t *)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, rowBytes * info.height,
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!buffer) {
        throwException(env, RUNTIME_EXCEPTION, "Failed mapping pixel unpack buffer for write");
        goto done;
    }

    for (unsigned i = 0; i < info.height; i++) {
        memcpy(buffer + rowBytes * i, bitmapData + info.stride * (info.height - i - 1), rowBytes);
    }

done:
    if (bitmapData != NULL) {
        AndroidBitmap_unlockPixels(env, bitmap);
    }
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

static void JNICALL nCopyPboToBitmap(JNIEnv *env, jclass cls, jint pbo, jobject bitmap) {
    AndroidBitmapInfo info;
    uint8_t *bitmapData = NULL;
    uint8_t *buffer = NULL;
    int rowBytes = 0;

    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &info)) {
        throwException(env, RUNTIME_EXCEPTION, "Error getting bitmap info");
        return;
    }
    rowBytes = info.width * getBytesPerPixel(info);

    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap,
            (void **)&bitmapData)) {
        throwException(env, RUNTIME_EXCEPTION, "Failed locking bitmap pixels");
        goto done;
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
    buffer = (uint8_t *)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, rowBytes * info.height,
            GL_MAP_READ_BIT);
    if (!buffer) {
        throwException(env, RUNTIME_EXCEPTION, "Failed mapping pixel pack buffer for read");
        goto done;
    }

    for (unsigned i = 0; i < info.height; i++) {
        memcpy(bitmapData + info.stride * i, buffer + rowBytes * (info.height - i - 1), rowBytes);
    }

done:
    if (bitmapData != NULL) {
        AndroidBitmap_unlockPixels(env, bitmap);
    }
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

}

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nCopyBitmapToPbo", "(Landroid/graphics/Bitmap;I)V", (void *)nCopyBitmapToPbo },
    { "nCopyPboToBitmap", "(ILandroid/graphics/Bitmap;)V", (void *)nCopyPboToBitmap },
};

bool gles30helper_RegisterNatives(JNIEnv *env) {
    jclass cls_GLES30Helper = env->FindClass(GLES30_HELPER_CLASS);
    if (env->ExceptionCheck()) return false;
    env->RegisterNatives(cls_GLES30Helper, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return false;

    return true;
}
