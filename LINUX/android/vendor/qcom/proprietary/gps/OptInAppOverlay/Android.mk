#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling OptInAPP overlay Application
#*  ******************************************************************************/

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/overlay

#include files in src directory
LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PROPRIETARY_MODULE := true

#include files in res diretory
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_SDK_VERSION = current

#the name of target apk
LOCAL_PACKAGE_NAME := OptInAppOverlay
include $(BUILD_PACKAGE)
