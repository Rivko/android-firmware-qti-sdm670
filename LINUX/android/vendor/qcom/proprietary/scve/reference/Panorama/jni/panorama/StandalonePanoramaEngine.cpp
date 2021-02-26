/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <android/log.h>
#include <jni.h>
#include "throw_exception.h"
#include "scvePanorama.hpp"
#include <stdlib.h>
#include <string.h>

#define PACKAGE     "com/qualcomm/qti/panoramaui/hardware/"
#define CLASS       PACKAGE "StandalonePanoramaEngine"
#define LOG_TAG "PanoNative"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

typedef struct {
    SCVE::Context *context;
    SCVE::PanoramaTracking *panoramaTracking;
    SCVE::PanoramaStitching *panoramaStitching;
    int previewNumChannels;
    SCVE::ColorFormat previewColorFormat;
    SCVE::ImageDimension previewDimensions;
    int direction;
} NativeContext;

static jclass class_ByteBuffer;
static jmethodID method_ByteBuffer_allocateDirect_I_ByteBuffer;
static jfieldID field_StandalonePanoramaEngine_nativeContext_J;

static void cleanupNativeContext(NativeContext *nativeContext)
{
    if (nativeContext->panoramaStitching) {
        SCVE::PanoramaStitching::deleteInstance(nativeContext->panoramaStitching);
        nativeContext->panoramaStitching = NULL;
    }
    if (nativeContext->panoramaTracking) {
        SCVE::PanoramaTracking::deleteInstance(nativeContext->panoramaTracking);
        nativeContext->panoramaTracking = NULL;
    }
    if (nativeContext->context) {
        SCVE::Context::deleteInstance(nativeContext->context);
        nativeContext->context = NULL;
    }
}

/*
 * Method:    nClassInit
 * Signature: ()V
 */
static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    field_StandalonePanoramaEngine_nativeContext_J = env->GetFieldID(cls, "nativeContext", "J");
    if (env->ExceptionCheck()) return;

    class_ByteBuffer = env->FindClass("java/nio/ByteBuffer");
    if (env->ExceptionCheck()) return;
    class_ByteBuffer = (jclass)env->NewGlobalRef(class_ByteBuffer); // N.B. must take global ref
    if (env->ExceptionCheck()) return;

    method_ByteBuffer_allocateDirect_I_ByteBuffer = env->GetStaticMethodID(class_ByteBuffer,
            "allocateDirect", "(I)Ljava/nio/ByteBuffer;");
    if (env->ExceptionCheck()) return;
}

/*
 * Method:    nInstanceInit
 * Signature: (IIIIF)Z
 */
static jboolean JNICALL nInstanceInit(JNIEnv *env, jobject obj, jint width, jint height,
        jint stride, jint colorFormat, jint projectionType, jfloat focalLength)
{
    if (env->GetLongField(obj, field_StandalonePanoramaEngine_nativeContext_J) != 0L) {
        throwException(env, RUNTIME_EXCEPTION, "Already initialized");
        return JNI_FALSE;
    }

    NativeContext *nativeContext = new NativeContext();
    SCVE::ImageDimension internalDim;

    SCVE::TrackingConfiguration_t trackingConfig;
    SCVE::StitchingConfiguration_t stitchConfig;

    switch (colorFormat) {
    case SCVE::SCVE_COLORFORMAT_YUV_NV21:
        nativeContext->previewNumChannels = 1;
        break;

    default:
        throwException(env, ILLEGAL_ARGUMENT_EXCEPTION, "Unsupported colorFormat '%d'",
                colorFormat);
        goto error;
    }

    nativeContext->direction  = SCVE::SCVE_PANORAMA_MOTION_UNKNOWN;

    nativeContext->previewDimensions.nWidth = width;
    nativeContext->previewDimensions.nHeight = height;
    nativeContext->previewDimensions.nStride = stride;
    nativeContext->previewColorFormat = (SCVE::ColorFormat)colorFormat;

    nativeContext->context = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
    if (!nativeContext->context) {
        throwException(env, RUNTIME_EXCEPTION, "Failed to create SCVE context");
        goto error;
    }

    internalDim.nWidth = width;
    internalDim.nHeight = height;
    internalDim.nStride = stride;

    trackingConfig.iViewFinderDim = internalDim;
    trackingConfig.cFormat = nativeContext->previewColorFormat;
    trackingConfig.fCameraFOV = 0;

    nativeContext->panoramaTracking = SCVE::PanoramaTracking::newInstance(nativeContext->context,
            trackingConfig);
    if (!nativeContext->panoramaTracking) {
        throwException(env, RUNTIME_EXCEPTION, "Failed to create Panorama tracking instance");
        goto error;
    }

    //SCVE::StitchingConfiguration_t stitchConfig;
    stitchConfig.iViewFinderDim = internalDim;
    // Hard code focalLength to 0 per cv request until they find a better solution.
    stitchConfig.fCameraFocalLength = 0;
    stitchConfig.fCameraPixelSize = 0;
    stitchConfig.iProjectionType = 0;
    if(projectionType == 0 || projectionType == 1) {
        stitchConfig.iProjectionType = projectionType;
    }
    LOGD("init pano stitching, projection type: %d", stitchConfig.iProjectionType);

    nativeContext->panoramaStitching = SCVE::PanoramaStitching::newInstance(nativeContext->context,
            stitchConfig);
    if (!nativeContext->panoramaStitching) {
        throwException(env, RUNTIME_EXCEPTION, "Failed to create Panorama stitching instance");
        goto error;
    }

    env->SetLongField(obj, field_StandalonePanoramaEngine_nativeContext_J, (jlong)nativeContext);

    return JNI_TRUE;

error:
    if (nativeContext) {
        cleanupNativeContext(nativeContext);
        delete nativeContext;
    }
    return JNI_FALSE;
}

/*
 * Method:    nInstanceRelease
 * Signature: ()V
 */
static void JNICALL nInstanceRelease(JNIEnv *env, jobject obj)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return;
    }

    cleanupNativeContext(nativeContext);
    delete nativeContext;
    env->SetLongField(obj, field_StandalonePanoramaEngine_nativeContext_J, 0L);
}

/*
 * Method:    nProcessPreviewFrame
 * Signature: (Ljava/nio/ByteBuffer;[Z[I[I[I)I
 */
static jint JNICALL nProcessPreviewFrame(JNIEnv *env, jobject obj, jobject frame,
        jbooleanArray isKeyFrame, jintArray framePosition, jintArray movementSpeed,
        jintArray apiDirection)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    SCVE::Image image = SCVE::Image();
    image.sFormat = nativeContext->previewColorFormat;
    image.nChannels = nativeContext->previewNumChannels;
    image.sDim = nativeContext->previewDimensions;
    image.pPtr = (uint8_t *)env->GetDirectBufferAddress(frame);
    if (env->ExceptionCheck()) return 0;

    SCVE::TrackingInput input;
    input.pFrame = &image;
    input.rotation = 0;
    input.flags = 0;
    SCVE::TrackingOutput output;
    SCVE::Status status = nativeContext->panoramaTracking->Run_Sync(input, output);
    if (status == SCVE::SCVE_SUCCESS) {
        int32_t pos[2];
        pos[0] = output.X;
        pos[1] = output.Y;
        env->SetBooleanArrayRegion(isKeyFrame, 0, 1, (jboolean *)&output.isKeyFrame);
        env->SetIntArrayRegion(framePosition, 0, 2, (jint *)pos);
        env->SetIntArrayRegion(movementSpeed, 0, 1, (jint *)&output.speed);
        env->SetIntArrayRegion(apiDirection, 0, 1, (jint *)&output.direction);

        nativeContext->direction = output.direction;
    }

    return status;
}

/*
 * Method:    nProcessKeyFrame
 * Signature: ([BII)I
 */
static jint JNICALL nProcessKeyFrame(JNIEnv *env, jobject obj, jbyteArray keyFrameJpegData,
        jint x, jint y, int exifAngle)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    SCVE::KeyFrameJPEG_t jpeg;
    jpeg.pData = (uint8_t *)env->GetByteArrayElements(keyFrameJpegData, NULL);
    jpeg.size = (uint32_t)env->GetArrayLength(keyFrameJpegData);

    SCVE::KeyFrame_t keyFrame = SCVE::KeyFrame_t();
    keyFrame.type = SCVE::SCVE_PANORAMASTITCHING_KEYFRAME_JPEG;
    keyFrame.u.pKeyFrameJPEG = &jpeg;
    keyFrame.rotation = exifAngle % 360;
    // Extract orientation information using libexif built-in function
    // In case no Exif data available, use orientation

    SCVE::StitchingInput_t input;
    input.pKeyFrame = &keyFrame;
    input.X = x;
    input.Y = y;
    input.flags = 0;
    input.direction = nativeContext->direction;
    SCVE::StitchingOutput_t output;
    SCVE::Status status = nativeContext->panoramaStitching->Run_Sync(input, output);

    env->ReleaseByteArrayElements(keyFrameJpegData, (jbyte *)keyFrame.u.pKeyFrameJPEG->pData,
            JNI_ABORT);
    return status;
}
static jint JNICALL nProcessKeyFrameNV21(JNIEnv *env, jobject obj, jobject frame, jint x, jint y)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }
    SCVE::Image image = SCVE::Image();
    image.sFormat = nativeContext->previewColorFormat;
    image.nChannels = nativeContext->previewNumChannels;
    image.sDim = nativeContext->previewDimensions;
    image.pPtr = (uint8_t *)env->GetDirectBufferAddress(frame);
    if (env->ExceptionCheck()) return 0;
    SCVE::KeyFrame_t keyFrame = SCVE::KeyFrame_t();
    keyFrame.type = SCVE::SCVE_PANORAMASTITCHING_KEYFRAME_RAW;
    keyFrame.u.pKeyFrameRaw = &image;
    SCVE::StitchingInput_t input;
    input.pKeyFrame = &keyFrame;
    input.X = x;
    input.Y = y;
    input.flags = 0;
    input.direction = nativeContext->direction;
    SCVE::StitchingOutput_t output;
    SCVE::Status status = nativeContext->panoramaStitching->Run_Sync(input, output);

    return status;
}

/*
 * Method:    nCancelPanorama
 * Signature: ()I
 */
static jint JNICALL nCancelPanorama(JNIEnv *env, jobject obj)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    SCVE::TrackingInput trackInput;
        trackInput.pFrame = NULL;
        trackInput.rotation = 0;
        trackInput.flags = SCVE::SCVE_PANORAMA_TASK_CANCEL;
        SCVE::TrackingOutput trackOutput;// N.B. the optional parameters to PanoramaTracker::Run_Sync aren't optional
    SCVE::Status statusTracking = nativeContext->panoramaTracking->Run_Sync(trackInput, trackOutput);

    SCVE::StitchingInput_t stitchInput;
        stitchInput.X = 0;
        stitchInput.Y = 0;
        stitchInput.flags = SCVE::SCVE_PANORAMA_TASK_CANCEL;
        stitchInput.direction = SCVE::SCVE_PANORAMA_MOTION_UNKNOWN;
        SCVE::StitchingOutput_t stitchOutput;
    SCVE::Status statusStitching = nativeContext->panoramaStitching->Run_Sync(stitchInput, stitchOutput);

    if (statusTracking != SCVE::SCVE_SUCCESS) return statusTracking;
    return statusStitching;
}

/*
 * Method:    nCompletePanorama
 * Signature: ([Ljava/nio/ByteBuffer;)I
 */
static jint JNICALL nCompletePanorama(JNIEnv *env, jobject obj, jobjectArray panoramaJpegData)
{
    NativeContext *nativeContext = (NativeContext *)env->GetLongField(obj,
            field_StandalonePanoramaEngine_nativeContext_J);
    if (nativeContext == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Not initialized");
        return 0;
    }

    SCVE::TrackingInput trackInput;
    trackInput.pFrame = NULL;
    trackInput.rotation = 0;
    trackInput.flags = SCVE::SCVE_PANORAMA_FRAME_ENDOFSEQUENCE;
    SCVE::TrackingOutput trackOutput;// N.B. the optional parameters to PanoramaTracker::Run_Sync aren't optional
    SCVE::Status statusTracking = nativeContext->panoramaTracking->Run_Sync(trackInput, trackOutput);

    SCVE::StitchingInput_t stitchInput;
    stitchInput.X = 0;
    stitchInput.Y = 0;
    stitchInput.flags = SCVE::SCVE_PANORAMA_FRAME_ENDOFSEQUENCE;
    stitchInput.direction = SCVE::SCVE_PANORAMA_MOTION_UNKNOWN;
    SCVE::StitchingOutput_t stitchOutput;
    SCVE::Status statusStitching = nativeContext->panoramaStitching->Run_Sync(stitchInput, stitchOutput);

    if (statusTracking != SCVE::SCVE_SUCCESS) return statusTracking;
    if (statusStitching != SCVE::SCVE_SUCCESS) return statusStitching;

    uint32_t size = 0;
    statusStitching = nativeContext->panoramaStitching->GetPanoramaResult(NULL, &size);
    if (statusStitching != SCVE::SCVE_SUCCESS) return statusStitching;

    jobject byteBuffer = env->CallStaticObjectMethod(class_ByteBuffer,
            method_ByteBuffer_allocateDirect_I_ByteBuffer, (jint)size);
    if (env->ExceptionCheck()) return 0;
    if (byteBuffer == NULL) {
        throwException(env, RUNTIME_EXCEPTION, "Failed allocating a direct ByteBuffer");
        return 0;
    }

    uint8_t *jpegData = (uint8_t *)env->GetDirectBufferAddress(byteBuffer);
    statusStitching = nativeContext->panoramaStitching->GetPanoramaResult(jpegData, &size);
    if (statusStitching != SCVE::SCVE_SUCCESS) return statusStitching;

    env->SetObjectArrayElement(panoramaJpegData, 0, byteBuffer);
    if (env->ExceptionCheck()) return 0;

    return SCVE::SCVE_SUCCESS;
}

///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nInstanceInit", "(IIIIIF)Z", (void *)nInstanceInit },
    { "nInstanceRelease", "()V", (void *)nInstanceRelease },
    { "nProcessPreviewFrame", "(Ljava/nio/ByteBuffer;[Z[I[I[I)I", (void *)nProcessPreviewFrame },
    { "nProcessKeyFrame", "([BIII)I", (void *)nProcessKeyFrame },
    { "nProcessKeyFrameNV21", "(Ljava/nio/ByteBuffer;II)I", (void *)nProcessKeyFrameNV21 },
    { "nCancelPanorama", "()I", (void *)nCancelPanorama },
    { "nCompletePanorama", "([Ljava/nio/ByteBuffer;)I", (void *)nCompletePanorama },
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    jclass cls = env->FindClass(CLASS);
    if (env->ExceptionCheck()) return -1;
    env->RegisterNatives(cls, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return -1;

    return JNI_VERSION_1_4;
}
