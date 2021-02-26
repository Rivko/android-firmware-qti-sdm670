/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <cstdlib>
#include <cstdint>
#include <string.h>

#define CLASS_ROTATE_YUV420SP "qces/qualcomm/qti/com/videosummary/videoprocessing/VideoFrameUtils/RotateYUV420SP"

static void JNICALL nRotate(JNIEnv *env, jclass cls, jobject src, jint srcWidth, jint srcHeight,
        jint srcOffsetY, jint srcStrideY, jint srcOffsetUV, jint srcStrideUV,
        jobject dst, jint dstOffsetY, jint dstStrideY, jint dstOffsetUV, jint dstStrideUV,
        jint degrees)
{
    const uint8_t *srcPtr = (uint8_t *)env->GetDirectBufferAddress(src) + srcOffsetY;
    uint8_t *dstPtr = (uint8_t *)env->GetDirectBufferAddress(dst) + dstOffsetY;
    const int srcWidthUV = (srcWidth + 1) / 2;
    const int srcHeightUV = (srcHeight + 1) / 2;
    const uint8_t *srcPtrUV = (srcPtr + srcOffsetUV);
    uint8_t *dstPtrUV = (dstPtr + dstOffsetUV);

    if (degrees == 0) {
        for (int i = 0; i < srcHeight; i++) {
            memcpy(dstPtr + dstStrideY * i, srcPtr + srcStrideY * i, srcWidth);
        }
        for (int i = 0; i < srcHeightUV; i++) {
            memcpy(dstPtrUV + dstStrideUV * i, srcPtrUV + srcStrideUV * i, srcWidthUV * 2);
        }
    } else if (degrees == 90) {
        for (int i = 0; i < srcHeight; i++) {
            for (int j = 0; j < srcWidth; j++) {
                dstPtr[dstStrideY * j + (srcHeight - i - 1)] = srcPtr[srcStrideY * i + j];
            }
        }
        for (int i = 0; i < srcHeightUV; i++) {
            for (int j = 0; j < srcWidthUV; j++) {
                *(uint16_t *)&dstPtrUV[dstStrideUV * j + (srcHeightUV - i - 1) * 2] =
                        *(uint16_t *)&srcPtrUV[srcStrideUV * i + j * 2];
            }
        }
    } else if (degrees == 180) {
        for (int i = 0; i < srcHeight; i++) {
            for (int j = 0; j < srcWidth; j++) {
                dstPtr[dstStrideY * (srcHeight - i - 1) + srcWidth - j - 1] = srcPtr[srcStrideY * i + j];
            }
        }
        for (int i = 0; i < srcHeightUV; i++) {
            for (int j = 0; j < srcWidthUV; j++) {
                *(uint16_t *)&dstPtrUV[dstStrideUV * (srcHeightUV - i - 1) + (srcWidthUV - j - 1) * 2] =
                        *(uint16_t *)&srcPtrUV[srcStrideUV * i + j * 2];
            }
        }
    } else { // degrees == 270
        for (int i = 0; i < srcHeight; i++) {
            for (int j = 0; j < srcWidth; j++) {
                dstPtr[dstStrideY * (srcWidth - j - 1) + i] = srcPtr[srcStrideY * i + j];
            }
        }
        for (int i = 0; i < srcHeightUV; i++) {
            for (int j = 0; j < srcWidthUV; j++) {
                *(uint16_t *)&dstPtrUV[dstStrideUV * (srcWidthUV - j - 1) + i * 2] =
                        *(uint16_t *)&srcPtrUV[srcStrideUV * i + j * 2];
            }
        }
    }
}

///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod methods[] = {
        { "nRotate", "(Ljava/nio/ByteBuffer;IIIIIILjava/nio/ByteBuffer;IIIII)V", (void *)nRotate },
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    jclass cls = env->FindClass(CLASS_ROTATE_YUV420SP);
    if (env->ExceptionCheck()) return -1;
    env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return -1;

    return JNI_VERSION_1_4;
}