/* Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef IMS_VT_JNI_MEDIA_SURFACE_UTILS_H
#define IMS_VT_JNI_MEDIA_SURFACE_UTILS_H

#include <utils/StrongPointer.h>
#include <gui/Surface.h>

inline jobject android_view_Surface_createFromIGraphicBufferProducer(JNIEnv* env,
        const android::sp<android::IGraphicBufferProducer>& bufferProducer) {
    if (bufferProducer == NULL) {
        return NULL;
    }

    android::sp<android::Surface> surface(new android::Surface(bufferProducer, true));
    if (surface == NULL) {
        return NULL;
    }

    jclass clazz = env->FindClass("android/view/Surface");
    jmethodID miCtor = env->GetMethodID(clazz, "<init>", "(J)V");
    jobject surfaceObj = env->NewObject(clazz, miCtor, (jlong)surface.get());
    if (surfaceObj == NULL) {
        if (env->ExceptionCheck()) {
            ALOGE("Could not create instance of Surface from IGraphicBufferProducer.");
            env->ExceptionClear();
        }
        return NULL;
    }
    surface->incStrong(0);
    return surfaceObj;
}

inline android::sp<android::IGraphicBufferProducer>
android_view_Surface_getIGraphicBufferProducer(JNIEnv* env, jobject jsurface) {

    jclass classSurface = env->FindClass("android/view/Surface");
    jfieldID fiSurface = env->GetFieldID(classSurface, "mNativeObject", "J");
    android::Surface* surface =
            reinterpret_cast<android::Surface*>(env->GetLongField(jsurface,fiSurface));

    android::sp<android::IGraphicBufferProducer> producer;
    if ( surface != nullptr ) {
        producer = surface->getIGraphicBufferProducer();
    }
    return producer;
}


static bool
android_view_Surface_haveSameParent(JNIEnv *e, jobject jSurfaceOne, jobject jSurfaceTwo) {
    android::sp<android::IGraphicBufferProducer> pa =
            android_view_Surface_getIGraphicBufferProducer(e, jSurfaceOne);
    android::sp<android::IGraphicBufferProducer> pb =
            android_view_Surface_getIGraphicBufferProducer(e, jSurfaceTwo);

    if ( pa == nullptr || pb == nullptr) {
        return pa == pb;
    }

    return (android::IInterface::asBinder(pa) == android::IInterface::asBinder(pb));
}

#endif // IMS_VT_JNI_MEDIA_SURFACE_UTILS_H
