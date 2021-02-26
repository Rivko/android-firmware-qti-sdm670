LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := BatterySaver
LOCAL_CERTIFICATE := platform
#LOCAL_PRIVILEGED_MODULE := true

# This will install the file in /system/vendor/ChinaMobile
LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/ChinaMobile/system/app

include $(BUILD_PACKAGE)
