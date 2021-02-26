/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <unistd.h>
#include <string.h>

#include <fastcv/fastcv.h>

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
JNIEXPORT jstring JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_getFastCVVersion(JNIEnv * env);

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420toRGB8888down
        (JNIEnv *env, jobject object, jbyteArray img, jint srcWidth, jint srcHeight, jint extraRows, jobject outputBitmap);

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420toRGB8888downscaleByteArrays
        (JNIEnv *env, jobject object, jbyteArray img, jintArray outRGB, jint srcWidth, jint srcHeight, jint srcYStride, jint srcCStride, jint outWidth, jint outHeight);

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420SPscaleDownLetterbox
        (JNIEnv *env, jobject object, jobject src, jint srcWidth, jint srcHeight, jobject dst, jint dstWidth, jint dstHeight, jboolean outputForBitmap);

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420ByteBufferToRGB8888Bitmap
        (JNIEnv *env, jobject object, jobject src, jint srcWidth, jint srcHeight, jint srcYStride, jint srcCStride, jobject outputBitmap);

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_RGB565ByteBufferToYUV420ByteBuffer
        (JNIEnv *env, jobject object, jobject srcRGB, jint srcWidth, jint srcHeight, jint srcStride, jobject dstYUV);

}

JNIEXPORT jstring JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_getFastCVVersion
        (JNIEnv * env)
{
    char sVersion[32];
    fcvGetVersion(sVersion, 32);
    return env->NewStringUTF(sVersion);
}




/*
 * For doing input YUV byte array -> downscaled YUV byte array -> output RGB int array
 */
JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420toRGB8888downscaleByteArrays
        (JNIEnv *env, jobject object, jbyteArray img, jintArray outRGB, jint srcWidth, jint srcHeight, jint srcYStride, jint srcCStride, jint outWidth, jint outHeight) {

    jbyte*           jimgData = NULL;
    jint*            joutRGB = NULL;

    jboolean         isCopy = JNI_FALSE;

    jimgData = env->GetByteArrayElements( img, &isCopy );
    joutRGB = env->GetIntArrayElements( outRGB, &isCopy );

    /////////////////////////////////////
    ////first scale down just the Y plane

    jbyteArray scaledYUVsemiplanar = env->NewByteArray(outWidth * outHeight * 3 / 2);
    jbyte* scaledYUVdata = env->GetByteArrayElements( scaledYUVsemiplanar, &isCopy);

    fcvScaleDownMNu8    (
            (uint8_t*) jimgData,
            srcWidth,
            srcHeight,
            srcYStride,
            (uint8_t*) scaledYUVdata,
            outWidth,
            outHeight,
            outWidth
    );

    ////////////////////////////////////
    ////next scale down the Chroma plane

    fcvScaleDownMNInterleaveu8    (
            (uint8_t*) jimgData + (srcYStride * srcHeight),
            srcWidth/2,
            srcHeight/2,
            srcCStride,
            (uint8_t*) scaledYUVdata + (outWidth * outHeight),
            outWidth/2,
            outHeight/2,
            outWidth
    );


    ///////////////////////////////////
    ////finally do color conversion

    fcvColorYCrCb420PseudoPlanarToRGB8888u8    (
            (uint8_t*)scaledYUVdata,
            outWidth,
            outHeight,
            outWidth,
            outWidth,
            (uint32_t*) joutRGB,
            outWidth * 4
    );

    env->ReleaseByteArrayElements(img, jimgData, 0 );

    env->ReleaseIntArrayElements(outRGB, joutRGB, 0 );

    env->ReleaseByteArrayElements(scaledYUVsemiplanar, scaledYUVdata, 0);

    return JNI_TRUE;

}

/*
 * For doing input YUV byte array -> downscaled YUV byte array -> output Bitmap
 */

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420toRGB8888down
        (JNIEnv *env, jobject object, jbyteArray img, jint srcWidth, jint srcHeight, jint extraRows, jobject outputBitmap)
{
    int *_outputBitmap;

    AndroidBitmapInfo outputBitmapInfo;

    jbyte*            jimgData = NULL;
    jboolean          isCopy = JNI_FALSE;

    jimgData = env->GetByteArrayElements( img, &isCopy );

    AndroidBitmap_getInfo(env, outputBitmap, &outputBitmapInfo);
    AndroidBitmap_lockPixels(env, outputBitmap, (void**)&_outputBitmap);

    int32_t outWidth = outputBitmapInfo.width;
    int32_t outHeight = outputBitmapInfo.height;

    //////////////////////////////////////
    ////first scale down just the Y plane

    jbyteArray scaledYUVsemiplanar = env->NewByteArray(outWidth * outHeight * 3 / 2);
    jbyte* scaledYUVdata = env->GetByteArrayElements( scaledYUVsemiplanar, &isCopy);

    fcvScaleDownMNu8    (
            (uint8_t*) jimgData,
            srcWidth,
            srcHeight,
            srcWidth,
            (uint8_t*) scaledYUVdata,
            outWidth,
            outHeight,
            outWidth
    );

    ////////////////////////////////////
    ////next scale down the Chroma plane

    fcvScaleDownMNInterleaveu8    (
            (uint8_t*) jimgData + (srcWidth * srcHeight) + (extraRows * srcWidth) + 1,
            srcWidth/2,
            srcHeight/2,
            srcWidth,
            (uint8_t*) scaledYUVdata + (outWidth * outHeight),
            outWidth/2,
            outHeight/2,
            outWidth
    );

    ///////////////////////////////////
    ////finally do color conversion

    fcvColorYCrCb420PseudoPlanarToRGB8888u8    (
            (uint8_t*)scaledYUVdata,
            outputBitmapInfo.width,
            outputBitmapInfo.height,
            outputBitmapInfo.width,
            outputBitmapInfo.width,
            (uint32_t*)_outputBitmap,
            outputBitmapInfo.stride
    );

    env->ReleaseByteArrayElements(img, jimgData, 0 );

    env->ReleaseByteArrayElements(scaledYUVsemiplanar, scaledYUVdata, 0);

    AndroidBitmap_unlockPixels(env, outputBitmap);

    return JNI_TRUE;

}

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420SPscaleDownLetterbox
        (JNIEnv *env, jobject object, jobject src, jint srcWidth, jint srcHeight, jobject dst, jint dstWidth, jint dstHeight, jboolean outputForBitmap) {

    enum Orientation {
        PORTRAIT, LANDSCAPE
    };

    //assumes tight packed YUV420SP
    const uint8_t *srcPtr = (uint8_t *) env->GetDirectBufferAddress(src);
    uint8_t *dstPtr = (uint8_t *) env->GetDirectBufferAddress(dst);

    Orientation inputOrientation;
    Orientation outputOrientation;

    int scaleWidth;
    int scaleHeight;
    int letterboxSideWidth = 0;
    int letterboxSideHeight = 0;

    int offsetForCbCrSwitch = 0;

    if (srcWidth > srcHeight)
        inputOrientation = LANDSCAPE;
    else
        inputOrientation = PORTRAIT;

    if (dstWidth > dstHeight)
        outputOrientation = LANDSCAPE;
    else
        outputOrientation = PORTRAIT;

    if (inputOrientation == PORTRAIT && outputOrientation == LANDSCAPE) {
        scaleHeight = dstHeight;
        scaleWidth = (dstHeight * dstHeight) / dstWidth;
        letterboxSideWidth = (dstWidth - scaleWidth) / 2;
    }
    else if (inputOrientation == LANDSCAPE && outputOrientation == PORTRAIT) {
        scaleWidth = dstWidth;
        scaleHeight = (dstWidth * dstWidth) / dstHeight;
        letterboxSideHeight = (dstHeight - scaleHeight) / 2;
    }
    else {
        scaleWidth = dstWidth;
        scaleHeight = dstHeight;
    }

    ////////////////////////
    // first fill the buffer with all black in YCbCr values, Y=16, Cb/Cr = 128
    memset(dstPtr, 16, dstWidth*dstHeight);
    memset(dstPtr + (dstWidth*dstHeight), 128, dstWidth*dstHeight/2);

    //check for even sidewall
    if (((letterboxSideWidth & 1) == 0) && outputForBitmap)
        offsetForCbCrSwitch = 1;

    //////////////////////////////////////
    ////first scale down just the Y plane
    fcvScaleDownMNu8    (
            (uint8_t*) srcPtr + offsetForCbCrSwitch,
            srcWidth,
            srcHeight,
            srcWidth,
            (uint8_t*) dstPtr + (letterboxSideHeight*dstWidth) + letterboxSideWidth,
            scaleWidth,
            scaleHeight,
            dstWidth
    );

    ////////////////////////////////////
    ////next scale down the Chroma plane
    fcvScaleDownMNInterleaveu8    (
            (uint8_t*) srcPtr + (srcWidth * srcHeight) + offsetForCbCrSwitch,
            srcWidth/2,
            srcHeight/2,
            srcWidth,
            (uint8_t*) dstPtr + (dstWidth*dstHeight) + (letterboxSideHeight*dstWidth/2) + letterboxSideWidth,
            scaleWidth/2,
            scaleHeight/2,
            dstWidth
    );


    return JNI_TRUE;
}

/*
 * For doing YUV420SP ByteBuffer src to Android RGB8888 bitmap output color conversion
 */

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_YUV420ByteBufferToRGB8888Bitmap
        (JNIEnv *env, jobject object, jobject src, jint srcWidth, jint srcHeight, jint srcYStride, jint srcCStride, jobject outputBitmap)
{

    int *_outputBitmap;

    AndroidBitmapInfo outputBitmapInfo;

    //assumes tight packed YUV420SP
    const uint8_t *srcPtr = (uint8_t *) env->GetDirectBufferAddress(src);

    AndroidBitmap_getInfo(env, outputBitmap, &outputBitmapInfo);
    AndroidBitmap_lockPixels(env, outputBitmap, (void**)&_outputBitmap);

    fcvColorYCrCb420PseudoPlanarToRGB8888u8    (
            (uint8_t*) srcPtr,
            srcWidth,
            srcHeight,
            srcYStride,
            srcCStride,
            (uint32_t*)_outputBitmap,
            outputBitmapInfo.stride
    );

    AndroidBitmap_unlockPixels(env, outputBitmap);

    return JNI_TRUE;

}


/*
 * For doing RGB565 ByteBuffer src to YUV420SP ByteBuffer output color conversion
 */

JNIEXPORT jboolean JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_fastcv_FastCVJNILib_RGB565ByteBufferToYUV420ByteBuffer
        (JNIEnv *env, jobject object, jobject srcRGB, jint srcWidth, jint srcHeight, jint srcStride, jobject dstYUV)
{

    const uint8_t *srcPtr = (uint8_t *) env->GetDirectBufferAddress(srcRGB);
    const uint8_t *dstPtr = (uint8_t *) env->GetDirectBufferAddress(dstYUV);

    fcvColorRGB565ToYCbCr420PseudoPlanaru8(
            (uint8_t*) srcPtr,
            srcWidth,
            srcHeight,
            srcStride,
            (uint8_t*) dstPtr,
            (uint8_t*) dstPtr + (srcWidth * srcHeight),
            0,
            0
    );

    return JNI_TRUE;

}


