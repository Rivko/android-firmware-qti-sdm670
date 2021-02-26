/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <android/bitmap.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_qualcomm_qti_khronos_ImageBufferUtils
 * Method:    nPackBuffer
 * Signature: (Ljava/nio/ByteBuffer;IIII)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_ImageBufferUtils_nPackBuffer
  (JNIEnv *env, jclass clazz, jobject buf, jint bytesPerPixel, jint width, jint height, jint stride)
{
    uint8_t *p = (uint8_t *)env->GetDirectBufferAddress(buf);
    int widthBytes = width * bytesPerPixel;
    for (int i = 0; i < height; i++) {
        memmove(p + i * widthBytes, p + i * stride, widthBytes);
    }
}

/*
 * Class:     com_qualcomm_qti_khronos_ImageBufferUtils
 * Method:    nPopulateBitmapFromABGR8888
 * Signature: (Landroid/graphics/Bitmap;Ljava/nio/ByteBuffer;III)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_ImageBufferUtils_nPopulateBitmapFromABGR8888
  (JNIEnv *env, jclass clazz, jobject bmp, jobject buf, jint width, jint height, jint stride)
{
    uint8_t *p = (uint8_t *)env->GetDirectBufferAddress(buf);
    AndroidBitmapInfo bmpInfo;
    uint8_t *bmpP = NULL;

    AndroidBitmap_getInfo(env, bmp, &bmpInfo);
    AndroidBitmap_lockPixels(env, bmp, (void **)&bmpP);
    for (int i = 0; i < height; i++) {
        memcpy(bmpP + i * bmpInfo.stride, p + i * stride, width * 4);
    }
    AndroidBitmap_unlockPixels(env, bmp);
}

/*
 * Class:     com_qualcomm_qti_khronos_ImageBufferUtils
 * Method:    nConvertNV12toNV21
 * Signature: ([BIII)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_ImageBufferUtils_nConvertNV12toNV21
  (JNIEnv *env, jclass clazz, jbyteArray buf, jint width, jint height, jint stride)
{
    jbyte *p = (jbyte *)env->GetByteArrayElements(buf, NULL);
    for (int i = 0; i < (height + 1) / 2; i++) {
        jbyte *row_base = &p[(height + i) * stride];
        for (int j = 0; j < width; j += 2) {
            uint8_t tmp = row_base[j];
            row_base[j] = row_base[j + 1];
            row_base[j + 1] = tmp;
        }
    }
    env->ReleaseByteArrayElements(buf, p, 0); // commit and release
}

#ifdef __cplusplus
}
#endif
