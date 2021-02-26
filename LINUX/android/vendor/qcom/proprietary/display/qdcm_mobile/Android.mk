#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_STATIC_JAVA_LIBRARIES := \
    android-support-v4

LOCAL_REQUIRED_MODULES := \
    com.qti.snapdragon.sdk.display

LOCAL_JAVA_LIBRARIES := \
    com.qti.snapdragon.sdk.display

LOCAL_PACKAGE_NAME := QDCMMobileApp
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

