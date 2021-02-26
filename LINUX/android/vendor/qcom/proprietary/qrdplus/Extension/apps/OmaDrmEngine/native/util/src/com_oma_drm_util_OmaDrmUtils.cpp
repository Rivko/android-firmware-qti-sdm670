/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "jni/OmaDrmUtils"

#include <stdlib.h>
#include <utils/Log.h>
#include <jni.h>
#include <JNIHelp.h>
#include <drm/DrmManagerClient.h>
#include <drm/drm_framework_common.h>

#include "SessionMap.h"
#include "OmaDrmInputStream.h"

#define INVALID_INPUT_STREAM_SESSION_ID -1

using namespace android;

static SessionMap<OmaDrmInputStream*> mInputStreamSessionMap;

static jint OmaDrmInputStream_Read(JNIEnv* env, jobject clazz, jint uniqueId,
        jbyteArray buffer, jint numBytes, jint offset) {
    if (mInputStreamSessionMap.isCreated(uniqueId)) {
        OmaDrmInputStream* inputStream = mInputStreamSessionMap.getValue(
                uniqueId);

        unsigned char* internalBuffer = (unsigned char*) malloc(numBytes + 1);
        memset(internalBuffer, '\0', numBytes + 1);

        int length = inputStream->read(internalBuffer, numBytes, offset);

        env->SetByteArrayRegion(buffer, 0, length, (jbyte*) internalBuffer);

        free(internalBuffer);

        return length;
    } else {
        ALOGE("Error! Can not read drm file! invalide session id %d", uniqueId);
        return 0;
    }
}

static jlong OmaDrmInputStream_FileSize(JNIEnv* env, jobject clazz,
        jint uniqueId) {
    jlong result = 0;
    if (mInputStreamSessionMap.isCreated(uniqueId)) {
        OmaDrmInputStream* inputStream = mInputStreamSessionMap.getValue(
                uniqueId);
        result = inputStream->getFileSize();
    } else {
        ALOGE("Error! invalide session id %d", uniqueId);
    }
    return result;
}

static jobject OmaDrmInputStream_GetOmaDrmInfo(JNIEnv* env, jobject clazz,
        jint uniqueId) {
    OmaDrmInputStream* inputStream = NULL;
    jobject infoObject = NULL;
    if (mInputStreamSessionMap.isCreated(uniqueId)) {
        inputStream = mInputStreamSessionMap.getValue(uniqueId);
    } else {
        ALOGE("Error! invalide session id %d", uniqueId);
    }

    if (inputStream != NULL) {
        OmaDrmInfo *info = inputStream->getOmaDrmInfo();
        jclass omaDrmPermissionClass = env->FindClass(
                "com/oma/drm/OmaDrmPermission");
        if (omaDrmPermissionClass == NULL) {
            ALOGE("Error! JNI Class Loading OmaDrmPermission failed");
            return NULL;
        }
        jmethodID omaDrmPermissionClass_constructor = env->GetMethodID(
                omaDrmPermissionClass, "<init>", "(IIJJJ)V");
        jobject omaDrmPermissionObject = env->NewObject(omaDrmPermissionClass,
                omaDrmPermissionClass_constructor, info->mPermission->mType,
                info->mPermission->mCount,
                (jlong) info->mPermission->mStartDate,
                (jlong) info->mPermission->mEndDate,
                (jlong) info->mPermission->mInterval);

        jclass omaDrmInfoClass = env->FindClass("com/oma/drm/OmaDrmInfo");
        jmethodID omaDrmInfoClass_constructor =
                env->GetMethodID(omaDrmInfoClass, "<init>",
                        "(Ljava/lang/String;Ljava/lang/String;JLcom/oma/drm/OmaDrmPermission;)V");
        infoObject = env->NewObject(omaDrmInfoClass,
                omaDrmInfoClass_constructor, env->NewStringUTF(info->mMimetype),
                env->NewStringUTF(info->mUid), (jlong) info->mFileSize,
                omaDrmPermissionObject);
    }

    return infoObject;
}

static jint OmaDrmInputStream_Init(JNIEnv* env, jobject clazz, jint uniqueId,
        jobject fileDescriptor, jboolean consumeRights) {
    ALOGV(
            "Before OmaDrmInputStream_Init - addr of mInputStreamSessionMap %zd and size %d  and uniqueId %d",
            &mInputStreamSessionMap, mInputStreamSessionMap.getSize(),
            uniqueId);
    jint fd = jniGetFDFromFileDescriptor(env, fileDescriptor);

    if (!mInputStreamSessionMap.isCreated(uniqueId)) {
        OmaDrmInputStream* inputStream = OmaDrmInputStream::create(uniqueId, fd,
                consumeRights);
        mInputStreamSessionMap.addValue(uniqueId, inputStream);
        ALOGV(
                "OmaDrmInputStream_Init - addr of mInputStreamSessionMap %zd and size %d,  and uniqueId %d",
                &mInputStreamSessionMap, mInputStreamSessionMap.getSize(),
                uniqueId);
        if (inputStream != NULL) {
            if (RightsStatus::RIGHTS_INVALID == inputStream->getStatus()) {
                ALOGE("Missmatch uniqueId in JNI:OmaDrmInputStream!");

                // Return a negative value
                // -1 -> Generic error
                // -2 -> Invalid Drm license
                return ((jint) (-2));
            }
        }
        return uniqueId;
    } else {
        return INVALID_INPUT_STREAM_SESSION_ID;
    }
}

static jint OmaDrmInputStream_Close(JNIEnv* env, jobject clazz, jint uniqueId) {
    OmaDrmInputStream* inputStream = NULL;
    jobject infoObject = NULL;
    if (mInputStreamSessionMap.isCreated(uniqueId)) {
        inputStream = mInputStreamSessionMap.getValue(uniqueId);
    } else {
        ALOGE("Error! invalid uniqueId %d", uniqueId);
    }

    if (inputStream != NULL) {
        inputStream->release();
    }

    if (mInputStreamSessionMap.isCreated(uniqueId)) {
        mInputStreamSessionMap.removeValue(uniqueId);
        return uniqueId;
    } else {
        return INVALID_INPUT_STREAM_SESSION_ID;
    }
}

static JNINativeMethod nativeMethods[] = { { "_readOmaDrmInputStream",
        "(I[BII)I", (void*) OmaDrmInputStream_Read },

{ "_fileSizeOmaDrmInputStream", "(I)J", (void*) OmaDrmInputStream_FileSize }, {
        "_getOmaDrmInfoOmaDrmInputStream", "(I)Ljava/lang/Object;",
        (void*) OmaDrmInputStream_GetOmaDrmInfo }, {
        "_initializeOmaDrmInputStream", "(ILjava/io/FileDescriptor;Z)I",
        (void*) OmaDrmInputStream_Init },

{ "_closeOmaDrmInputStream", "(I)I", (void*) OmaDrmInputStream_Close },

};

static int registerNativeMethods(JNIEnv* env) {
    int result = -1;

    /* look up the class */
    jclass clazz = env->FindClass("com/oma/drm/util/OmaDrmUtils");

    if (NULL != clazz) {
        if (env->RegisterNatives(clazz, nativeMethods,
                sizeof(nativeMethods) / sizeof(nativeMethods[0])) == JNI_OK) {
            result = 0;
        }
    }
    return result;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK) {
        if (NULL != env && registerNativeMethods(env) == 0) {
            result = JNI_VERSION_1_4;
        }
    }
    return result;
}
