/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <unistd.h>

#define  LOG_TAG    "Applog"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
{

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_segmentation_Alpha8JNILib_FillAlphaBitmapWithByteArray
(JNIEnv * env, jobject object, jbyteArray inputByteArray, jobject outputBitmap);

JNIEXPORT void Java_com_qualcomm_qti_qces_imageedit_segmentation_Alpha8JNILib_CopyAlpha8Pixels
(JNIEnv *env, jobject object, jobject inputBitmap, jobject outputBitmap);

}

JNIEXPORT void Java_com_qualcomm_qti_qces_imageedit_segmentation_Alpha8JNILib_FillAlphaBitmapWithByteArray
(JNIEnv *env, jobject object, jbyteArray inputByteArray, jobject outputBitmap)
{

    uint32_t *_outputBitmap;

    jbyte* array = (env)->GetByteArrayElements(inputByteArray,NULL);

    AndroidBitmapInfo outputInfo;
    AndroidBitmap_getInfo(env, outputBitmap, &outputInfo);
    AndroidBitmap_lockPixels(env, outputBitmap, (void**)&_outputBitmap);

    int byteCnt = outputInfo.stride * outputInfo.height;

    //populate the bitmap's pixels with a single
    memcpy(&_outputBitmap[0], &array[0], byteCnt);

    (env)->ReleaseByteArrayElements(inputByteArray,array,0);

    AndroidBitmap_unlockPixels(env, outputBitmap);

}


JNIEXPORT void Java_com_qualcomm_qti_qces_imageedit_segmentation_Alpha8JNILib_CopyAlpha8Pixels
(JNIEnv *env, jobject object, jobject inputBitmap, jobject outputBitmap)
{

     uint32_t *_inputBitmap, *_outputBitmap;

     AndroidBitmapInfo inputInfo;
     AndroidBitmapInfo outputInfo;

     AndroidBitmap_getInfo(env, inputBitmap, &inputInfo);
     AndroidBitmap_lockPixels(env, inputBitmap, (void**)&_inputBitmap);
     AndroidBitmap_lockPixels(env, outputBitmap, (void**)&_outputBitmap);

     int byteCnt = inputInfo.stride * inputInfo.height;
     memcpy(&_outputBitmap[0], &_inputBitmap[0], byteCnt);

     AndroidBitmap_unlockPixels(env, inputBitmap);
     AndroidBitmap_unlockPixels(env, outputBitmap);

}
