#/******************************************************************************
#*@file Android.mk
#*brief Rules for copiling the source files
#*******************************************************************************/
ifneq ($(TARGET_BUILD_VARIANT),user)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional debug

LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_MODULE_PATH := $(PRODUCT_OUT)/data
LOCAL_JAVA_LIBRARIES := com.qualcomm.qti.imscmservice@1.0-java
LOCAL_JAVA_LIBRARIES += android.hidl.base-V1.0-java
LOCAL_PACKAGE_NAME := ConnectionManagerTestApp
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_OWNER := qti
#LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PACKAGE)
endif
