LOCAL_PATH:= $(call my-dir)

ifeq ($(TARGET_USES_STATSPOLL_MANAGER),true)
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := StatsPollManager
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-subdir-java-files)
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_OWNER := qti
LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
endif
