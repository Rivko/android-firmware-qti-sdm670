LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_JNI_SHARED_LIBRARIES := libSampleExtAuthJNI
LOCAL_PACKAGE_NAME := SampleExtAuthService

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += $(call all-Iaidl-files-under, src)

LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)
include $(LOCAL_PATH)/jni/Android.mk
