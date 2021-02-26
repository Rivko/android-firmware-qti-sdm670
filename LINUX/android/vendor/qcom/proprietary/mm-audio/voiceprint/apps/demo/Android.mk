#############################################################
# VoicePrint Demo
#############################################################

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_JAVA_LIBRARIES := framework
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_STATIC_JAVA_LIBRARIES := VoicePrintSDK

LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_SRC_FILES += $(TOP)/core/java/com/android/internal/widget/ILockSettings.aidl \

LOCAL_PACKAGE_NAME := VoicePrintDemo
LOCAL_SDK_VERSION := current
LOCAL_CERTIFICATE := platform
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PACKAGE)
