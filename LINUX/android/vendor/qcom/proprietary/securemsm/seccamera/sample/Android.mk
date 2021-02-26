#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# This part should be added to use the Secure Camera API (Module: seccamapi)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := seccamapi
LOCAL_SRC_FILES := $(call all-java-files-under,../client/src)

include $(BUILD_STATIC_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional debug

LOCAL_STATIC_JAVA_LIBRARIES = \
    android-support-v4 \
    seccamapi

LOCAL_SRC_FILES := $(call all-subdir-java-files,java)
LOCAL_PACKAGE_NAME := seccamsample
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := false

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/java/res
LOCAL_RESOURCE_DIR += prebuilts/sdk/current/support/v7/appcompat/res
LOCAL_AAPT_FLAGS := --auto-add-overlay
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.appcompat

include $(BUILD_PACKAGE)

