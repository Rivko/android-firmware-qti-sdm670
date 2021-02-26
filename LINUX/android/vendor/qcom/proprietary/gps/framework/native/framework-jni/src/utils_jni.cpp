/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "LocSvc_HIDL_utils_jni"
#define LOG_NDEBUG 0

#include "utils_jni.h"
#include <android_runtime/AndroidRuntime.h>
#include "utils/Log.h"

using namespace android;

class VMAssociator {

private:
    JNIEnv* mJNIEnv = nullptr;

public:
    VMAssociator() {

        JavaVM* javaVm = AndroidRuntime::getJavaVM();
        JavaVMAttachArgs args = {
             JNI_VERSION_1_6,
             "IzatSvc Callback Thread",
             NULL
        };
        jint result = javaVm->AttachCurrentThread(&mJNIEnv, &args);
        if (result != 0) {
            ALOGE("Callback thread attach error %d", result);
        } else {
            ALOGV("Callback thread attached to VM, jniEnv %p", mJNIEnv);
        }
    }

    ~VMAssociator() {

        JavaVM* javaVm = AndroidRuntime::getJavaVM();
        jint result = javaVm->DetachCurrentThread();
        if (result != 0) {
             ALOGE("Callback thread detach error %d", result);
        }
    }

    JNIEnv* getJNIEnv() {
        if (AndroidRuntime::getJNIEnv() != mJNIEnv) {
            ALOGE("Environment mismatch !");
            return NULL;
        }
        return mJNIEnv;
    }
};

thread_local std::unique_ptr<VMAssociator> sVMAssociator;

JNIEnv* getCallbackEnv() {

    JNIEnv* env = AndroidRuntime::getJNIEnv();

    if (env == nullptr || sVMAssociator == nullptr) {
        sVMAssociator.reset(new VMAssociator());
        env = sVMAssociator->getJNIEnv();
    }
    return env;
}

void checkAndClearExceptionsFromCallback(JNIEnv *env, const char* methodName)
{
    if (env->ExceptionCheck()) {
        ALOGE("An exception was thrown from callback '%s'", methodName);
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}
