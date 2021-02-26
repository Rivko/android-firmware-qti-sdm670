/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <unistd.h>

#include <fastcv.h>

#define  LOG_TAG    "FastCVJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


extern "C"
{
/*
 * Class:     com_qualcomm_qti_qces_imageedit_fastcv_FastCVJNILib
 * Method:    getFastCVVersion
 * Signature:
 */
JNIEXPORT jstring JNICALL Java_com_qualcomm_qti_qces_imageedit_fastcv_FastCVJNILib_getFastCVVersion(JNIEnv * env);

JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_qces_imageedit_fastcv_FastCVJNILib_dilateNxN
(JNIEnv *env, jobject object, jobject inputMaskBitmap, jint N, jobject outputMaskBitmap);

}

JNIEXPORT jstring JNICALL Java_com_qualcomm_qti_qces_imageedit_fastcv_FastCVJNILib_getFastCVVersion
(JNIEnv * env)
{
    char sVersion[32];
    fcvGetVersion(sVersion, 32);
    return env->NewStringUTF(sVersion);
}


JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_qces_imageedit_fastcv_FastCVJNILib_dilateNxN
(JNIEnv *env, jobject object, jobject inputMaskBitmap, jint N, jobject outputMaskBitmap)
{

    int *_inputMaskBitmap, *_outputMaskBitmap;

    AndroidBitmapInfo inputMaskInfo;
    AndroidBitmapInfo outputMaskInfo;

    AndroidBitmap_getInfo(env, inputMaskBitmap, &inputMaskInfo);
    AndroidBitmap_lockPixels(env, inputMaskBitmap, (void**)&_inputMaskBitmap);
    AndroidBitmap_getInfo(env, outputMaskBitmap, &outputMaskInfo);
    AndroidBitmap_lockPixels(env, outputMaskBitmap, (void**)&_outputMaskBitmap);

    //N must be odd number
   // if(N%2 == 0){
   //     return JNI_FALSE;
   // }

    fcvFilterDilateNxNu8( (uint8_t*)_inputMaskBitmap, inputMaskInfo.width, inputMaskInfo.height, inputMaskInfo.stride, N,
                          (uint8_t*)_outputMaskBitmap, outputMaskInfo.stride);

    AndroidBitmap_unlockPixels(env, inputMaskBitmap);
    AndroidBitmap_unlockPixels(env, outputMaskBitmap);

    return JNI_TRUE;

}






