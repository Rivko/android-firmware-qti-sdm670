#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional samples

LOCAL_SRC_FILES := $(call all-subdir-java-files,java)
LOCAL_PACKAGE_NAME := seccamservice
LOCAL_CERTIFICATE := platform
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_JNI_SHARED_LIBRARIES := libseccamservice
LOCAL_PRIVILEGED_MODULE := true

include $(BUILD_PACKAGE)

include $(LOCAL_PATH)/jni/Android.mk
