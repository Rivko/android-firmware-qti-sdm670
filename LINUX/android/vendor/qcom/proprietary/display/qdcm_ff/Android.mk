#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under,src)

LOCAL_STATIC_JAVA_LIBRARIES := vendor.display.color-V1.1-java-static
LOCAL_JAVA_LIBRARIES := framework android.hidl.base-V1.0-java android.hidl.manager-V1.0-java

LOCAL_PACKAGE_NAME := QdcmFF
LOCAL_CERTIFICATE  := platform
LOCAL_MODULE_OWNER := qti

include $(BUILD_PACKAGE)
