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
LOCAL_MODULE := uimremotesimlocklibrary
LOCAL_SRC_FILES := src/com/qualcomm/qti/remoteSimlock/IUimRemoteSimlockServiceCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/remoteSimlock/IUimRemoteSimlockService.aidl

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := RemoteSimlock.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================
# Build the xml package for checking trusted application
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROTOC_OPTIMIZE_TYPE := micro

LOCAL_PACKAGE_NAME := remoteSimLockAuthentication
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

# ==========================================================================
# Build the simlock manager
# which can be used by client

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := remotesimlockmanagerlibrary
LOCAL_JAVA_LIBRARIES := uimremotesimlocklibrary

LOCAL_SRC_FILES := src/com/qualcomm/qti/remoteSimlockManager/RemoteSimlockManager.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/remoteSimlockManager/RemoteSimlockManagerCallback.java

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := RemoteSimlockManager.xml
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

#this will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

endif

endif # TARGET_NO_TELEPHONY

