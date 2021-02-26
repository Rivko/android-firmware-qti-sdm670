##############################################################################
# @file vendor/qcom/proprietary/qrdplus/Extension/apps/QuickSettingsService/Android.mk
# @brief Makefile for building the QSServices library on Android.
##############################################################################

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_JAVA_LIBRARIES := telephony-common telephony-ext
LOCAL_PACKAGE_NAME := QSService
LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
