/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include <cstdlib>
#include <vector>
#include "scve/scveObjectTracker.hpp"
#include "throw_exception.h"

#define OBJECT_TRACKER_PACKAGE                  "com/qualcomm/qti/objecttracker/tracker/"

#define OBJECT_TRACKER_BINDING_CLASS            OBJECT_TRACKER_PACKAGE "ObjectTrackerBinding"
#define OBJECT_TRACKER_BINDING_RESULT_CLASS     OBJECT_TRACKER_PACKAGE "ObjectTrackerBinding$Result"

#define OBJECT_TRACKER_NOT_RUNNING_EXCEPTION    OBJECT_TRACKER_PACKAGE "ObjectTrackerNotRunningException"
#define INTERNAL_OBJECT_TRACKER_EXCEPTION       OBJECT_TRACKER_PACKAGE "InternalObjectTrackerException"
#define NO_OBJECTS_REGISTERED_EXCEPTION         OBJECT_TRACKER_PACKAGE "NoObjectsRegisteredException"
#define OBJECT_REGISTRATION_FAILED_EXCEPTION    OBJECT_TRACKER_PACKAGE "ObjectRegistrationFailedException"
#define TOO_MANY_OBJECTS_REGISTERED_EXCEPTION   OBJECT_TRACKER_PACKAGE "TooManyObjectsRegisteredException"

#define PENDING_REGISTRATION                    -1L

// These dimensions are for images that the object tracker does not downscale; they need to be
// scaled accordingly (see getObjectTrackerDownscaleFactor)
#define MIN_ROI_DIMENSION                       16
#define MAX_ROI_DIMENSION                       100

typedef struct {
    SCVE::Context *context;
    SCVE::ObjectTracker *tracker;
    SCVE::ImageDimension image_dimension;
    std::vector<uint32_t> tracked_object_ids;
} instance_data_t;

static jfieldID field_ObjectTrackerBinding_nativeInstanceData_J;

static jclass cls_ObjectTrackerBinding$Result;
static jmethodID method_ObjectTrackerBinding$Result_ctor_IIIIII_V;

// N.B. It would be better if SCVE could return this information dynamically, but it's not currently
// available via the SCVE APIs
static int getObjectTrackerDownscaleFactor(int frameHeight) {
    if (frameHeight > 1080) {
        return 8;
    } else if (frameHeight > 960) {
        return 6;
    } else if (frameHeight > 480) {
        return 4;
    } else if (frameHeight > 240) {
        return 2;
    } else {
        return 1;
    }
}

/*
 * Method:    nClassInit
 * Signature: ()V
 */
static void JNICALL nClassInit(JNIEnv *env, jclass cls)
{
    field_ObjectTrackerBinding_nativeInstanceData_J = env->GetFieldID(cls, "nativeInstanceData", "J");
    if (env->ExceptionCheck()) return;
    jclass tmp = env->FindClass(OBJECT_TRACKER_BINDING_RESULT_CLASS);
    if (env->ExceptionCheck()) return;
    cls_ObjectTrackerBinding$Result = (jclass)env->NewGlobalRef(tmp);
    method_ObjectTrackerBinding$Result_ctor_IIIIII_V = env->GetMethodID(cls_ObjectTrackerBinding$Result, "<init>", "(IIIIII)V");
    if (env->ExceptionCheck()) return;
}

/*
 * Method:    nInit
 * Signature: (IIIII)V
 */
static void JNICALL nInit(JNIEnv *env, jobject obj, jint operationMode, jint precision,
        jint width, jint height, jint stride)
{
    if (env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J) != 0L) {
        throwException(env, RUNTIME_EXCEPTION, "instanceData expected to be NULL but isn't");
        return;
    }

    instance_data_t *instance_data = new instance_data_t;
    if (instance_data == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed to allocate instance data");
        goto error;
    }
    instance_data->context = SCVE::Context::newInstance((SCVE::OperationMode)operationMode);
    if (instance_data->context == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed to create SCVE context");
        goto error;
    }
    instance_data->image_dimension.nWidth = width;
    instance_data->image_dimension.nHeight = height;
    instance_data->image_dimension.nStride = stride;
    instance_data->tracker = SCVE::ObjectTracker::newInstance(instance_data->context,
            instance_data->image_dimension, SCVE::SCVE_COLORFORMAT_YUV_NV21, NULL, NULL,
            (SCVE::ObjectTracker_Precision)precision, NULL);
    if (instance_data->tracker == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed to create object tracker");
        goto error;
    }

    env->SetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J, (jlong)instance_data);
    return; // success

error: // cleanup memory on an error
    if (instance_data != NULL) {
        if (instance_data->tracker != NULL) {
            SCVE::ObjectTracker::deleteInstance(instance_data->tracker);
        }

        if (instance_data->context != NULL) {
            SCVE::Context::deleteInstance(instance_data->context);
        }
    }
    delete instance_data;
}

/*
 * Method:    nRelease
 * Signature: ()V
 */
static void JNICALL nRelease(JNIEnv *env, jobject obj)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return;
    }

    env->SetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J, 0L);
    SCVE::ObjectTracker::deleteInstance(instance_data->tracker);
    SCVE::Context::deleteInstance(instance_data->context);
    delete instance_data;
}

/*
 * Method:    nGetMinRoiDimension
 * Signature: ()I
 */
static jint JNICALL nGetMinRoiDimension(JNIEnv *env, jobject obj)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return 0;
    }

    return MIN_ROI_DIMENSION * getObjectTrackerDownscaleFactor(instance_data->image_dimension.nHeight);
}

/*
 * Method:    nGetMaxRoiDimension
 * Signature: ()I
 */
static jint JNICALL nGetMaxRoiDimension(JNIEnv *env, jobject obj)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return 0;
    }

    return MAX_ROI_DIMENSION * getObjectTrackerDownscaleFactor(instance_data->image_dimension.nHeight);
}

/*
 * Method:    nRegisterObjectByRect
 * Signature: ([BIIII)I
 */
static jint JNICALL nRegisterObjectByRect(JNIEnv *env, jobject obj, jbyteArray imageDataNV21,
        jint left, jint top, jint right, jint bottom)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return 0;
    }

    jbyte* imageDataNV21Array = env->GetByteArrayElements(imageDataNV21, NULL);
    if (imageDataNV21Array == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed retrieving image data");
        return 0;
    }

    SCVE::Image image;
    image.sFormat = SCVE::SCVE_COLORFORMAT_YUV_NV21;
    image.sDim = instance_data->image_dimension;
    image.pPtr = (uint8_t *)imageDataNV21Array;

    SCVE::Rect roi;
    roi.nLocX = left;
    roi.nLocY = top;
    roi.nWidth = right - left;
    roi.nHeight = bottom - top;

    uint32_t objectId = 0;

    uint32_t status = instance_data->tracker->RegisterObject_Sync(&image, roi, &objectId,
            SCVE::SCVE_OBJECTTRACKER_REGFLAG_DEFAULT);

    env->ReleaseByteArrayElements(imageDataNV21, imageDataNV21Array, JNI_ABORT);

    switch (status) {
    case SCVE::SCVE_SUCCESS:
        instance_data->tracked_object_ids.push_back(objectId);
        break;

    case SCVE::SCVE_OBJECTTRACKER_ERROR_TOO_MANY_OBJECTS:
        throwException(env, TOO_MANY_OBJECTS_REGISTERED_EXCEPTION, NULL);
        return 0;

    case SCVE::SCVE_OBJECTTRACKER_ERROR_OBJ_REG_FAILED:
        throwException(env, OBJECT_REGISTRATION_FAILED_EXCEPTION, NULL);
        return 0;

    default:
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Unexpected object tracker error occurred. Error=%d", status);
        return 0;
    }

    return objectId;
}

/*
 * Method:    nRegisterObjectByPoint
 * Signature: ([BIIZ)I
 */
static jint JNICALL nRegisterObjectByPoint(JNIEnv *env, jobject obj, jbyteArray imageDataNV21,
        jint x, jint y, jboolean firstTime)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return 0;
    }

    jbyte* imageDataNV21Array = env->GetByteArrayElements(imageDataNV21, NULL);
    if (imageDataNV21Array == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed retrieving image data");
        return 0;
    }

    SCVE::Image image;
    image.sFormat = SCVE::SCVE_COLORFORMAT_YUV_NV21;
    image.sDim = instance_data->image_dimension;
    image.pPtr = (uint8_t *)imageDataNV21Array;

    SCVE::Point point;
    point.nLocX = x;
    point.nLocY = y;

    uint32_t flags = SCVE::SCVE_OBJECTTRACKER_REGFLAG_DEFAULT;
    if (firstTime == JNI_TRUE) {
        flags |= SCVE::SCVE_OBJECTTRACKER_REGFLAG_NEW_POINT;
    }

    uint32_t objectId = 0;

    uint32_t status = instance_data->tracker->RegisterObjectByPoint_Sync(&image, point,
            &objectId, flags);

    env->ReleaseByteArrayElements(imageDataNV21, imageDataNV21Array, JNI_ABORT);

    switch (status) {
    case SCVE::SCVE_SUCCESS:
        instance_data->tracked_object_ids.push_back(objectId);
        break;

    case SCVE::SCVE_PENDING:
        objectId = PENDING_REGISTRATION;
        break;

    case SCVE::SCVE_OBJECTTRACKER_ERROR_TOO_MANY_OBJECTS:
        throwException(env, TOO_MANY_OBJECTS_REGISTERED_EXCEPTION, NULL);
        return 0;

    case SCVE::SCVE_OBJECTTRACKER_ERROR_OBJ_REG_FAILED:
        throwException(env, OBJECT_REGISTRATION_FAILED_EXCEPTION, NULL);
        return 0;

    default:
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Unexpected object tracker error occurred. Error=%d", status);
        return 0;
    }

    return objectId;
}

/*
 * Method:    nUnregisterObject
 * Signature: (I)V
 */
static void JNICALL nUnregisterObject(JNIEnv * env, jobject obj, jint id)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return;
    }

    SCVE::Status status = instance_data->tracker->UnregisterObject(id);

    if (status != SCVE::SCVE_SUCCESS) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Unexpected object tracker error occurred. Error=%d", status);
        return;
    }

    for (std::vector<uint32_t>::iterator it = instance_data->tracked_object_ids.begin();
            it != instance_data->tracked_object_ids.end(); ++it) {
        if ((int)*it == id) {
            instance_data->tracked_object_ids.erase(it);
            break;
        }
    }
}

/*
 * Method:    nTrackObjects
 * Signature: ([B)[Lcom/qualcomm/qti/objecttracker/tracker/ObjectTrackerBinding$Result;
 */
static jobjectArray JNICALL nTrackObjects(JNIEnv *env, jobject obj, jbyteArray imageDataNV21)
{
    instance_data_t *instance_data = (instance_data_t *)env->GetLongField(obj, field_ObjectTrackerBinding_nativeInstanceData_J);
    if (instance_data == NULL) {
        throwException(env, OBJECT_TRACKER_NOT_RUNNING_EXCEPTION, NULL);
        return NULL;
    }

    int num_tracked_objects = instance_data->tracked_object_ids.size();

    jbyte* imageDataNV21Array = env->GetByteArrayElements(imageDataNV21, NULL);
    if (imageDataNV21Array == NULL) {
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Failed retrieving image data");
        return NULL;
    }

    SCVE::Image image;
    image.sFormat = SCVE::SCVE_COLORFORMAT_YUV_NV21;
    image.sDim = instance_data->image_dimension;
    image.pPtr = (uint8_t *)imageDataNV21Array;

    SCVE::ObjectTrackerResult *results = new SCVE::ObjectTrackerResult[num_tracked_objects];
    jobjectArray resultObjs = NULL;

    uint32_t status = instance_data->tracker->TrackObjects_Sync(&image, results, num_tracked_objects);

    env->ReleaseByteArrayElements(imageDataNV21, imageDataNV21Array, JNI_ABORT);

    switch (status) {
    case SCVE::SCVE_SUCCESS:
        // Do nothing
        break;

    case SCVE::SCVE_OBJECTTRACKER_ERROR_NO_OBJECTS:
        throwException(env, NO_OBJECTS_REGISTERED_EXCEPTION, NULL);
        goto cleanup;

    default:
        throwException(env, INTERNAL_OBJECT_TRACKER_EXCEPTION, "Unexpected object tracker error occurred. Error=%d", status);
        goto cleanup;
    }

    resultObjs = env->NewObjectArray(num_tracked_objects, cls_ObjectTrackerBinding$Result, NULL);
    if (env->ExceptionCheck()) goto cleanup;

    for (int i = 0; i < num_tracked_objects; i++) {
        jobject o = env->NewObject(cls_ObjectTrackerBinding$Result, method_ObjectTrackerBinding$Result_ctor_IIIIII_V,
                instance_data->tracked_object_ids[i], results[i].nConfidence, results[i].sROI.nLocX, results[i].sROI.nLocY,
                results[i].sROI.nLocX + results[i].sROI.nWidth, results[i].sROI.nLocY + results[i].sROI.nHeight);
        if (env->ExceptionCheck()) goto cleanup;
        env->SetObjectArrayElement(resultObjs, i, o);
        if (env->ExceptionCheck()) goto cleanup;
    }

cleanup:
    delete[] results;

    return resultObjs;
}

///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod methods[] = {
    { "nClassInit", "()V", (void *)nClassInit },
    { "nInit", "(IIIII)V", (void *)nInit },
    { "nRelease", "()V", (void *)nRelease },
    { "nGetMinRoiDimension", "()I", (void *)nGetMinRoiDimension },
    { "nGetMaxRoiDimension", "()I", (void *)nGetMaxRoiDimension },
    { "nRegisterObjectByRect", "([BIIII)I", (void *)nRegisterObjectByRect },
    { "nRegisterObjectByPoint", "([BIIZ)I", (void *)nRegisterObjectByPoint },
    { "nUnregisterObject", "(I)V", (void *)nUnregisterObject },
    { "nTrackObjects", "([B)[L" OBJECT_TRACKER_BINDING_RESULT_CLASS ";", (void *)nTrackObjects },
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    jclass cls_ObjectTrackerBinding = env->FindClass(OBJECT_TRACKER_BINDING_CLASS);
    if (env->ExceptionCheck()) return -1;
    env->RegisterNatives(cls_ObjectTrackerBinding, methods, sizeof(methods) / sizeof(methods[0]));
    if (env->ExceptionCheck()) return -1;

    return JNI_VERSION_1_4;
}
