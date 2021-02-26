LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PACKAGE_NAME := ZeroBalanceHelper
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
TARGET_OUT_VENDOR_APPS_PRIVILEGED := $(TARGET_OUT)/vendor/priv-app
LOCAL_PROPRIETARY_MODULE := true
LOCAL_DEX_PREOPT := nostripping
include $(BUILD_PACKAGE)
