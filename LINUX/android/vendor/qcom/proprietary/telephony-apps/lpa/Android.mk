#/******************************************************************************
#*@file Android.mk
#*brief Rules for copiling the source files
#*******************************************************************************/

ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

LOCAL_PATH:= $(call my-dir)

# Build the auto generated files into a library to be used by both the
# app and the service
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := uimlpalibrary
LOCAL_SRC_FILES := src/com/qualcomm/qti/lpa/IUimLpaService.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/lpa/IUimLpaServiceCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/lpa/UimLpaDownloadProgress.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/lpa/UimLpaProfile.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/lpa/UimLpaConstants.java


include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := lpa.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

endif
endif # TARGET_NO_TELEPHONY
