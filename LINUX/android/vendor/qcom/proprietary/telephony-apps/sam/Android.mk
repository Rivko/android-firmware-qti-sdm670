#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/
ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

ifeq (0,1)
ifeq ($(TARGET_USES_QCOM_BSP_ATEL), true) #needs CommandException.INVALID_PARAMETER

LOCAL_PATH:= $(call my-dir)

#  Build the SAM interface files into a library
#  which can be used by both service and client.

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := saminterfacelibrary

LOCAL_SRC_FILES := src/com/qualcomm/qti/sam/interfaces/CardState.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/CardATR.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/SlotMap.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/SAMErrors.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/ISAMResponseCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/ISAMUnsolicitedCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/interfaces/ISAM.aidl

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
# Build the sam manager interface to a library
# which can be used by client

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := sammanagerlibrary
LOCAL_JAVA_LIBRARIES := saminterfacelibrary

LOCAL_SRC_FILES := src/com/qualcomm/qti/sam/manager/SAMManager.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/manager/SAMResponseCallback.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/sam/manager/SAMUnsolicitedCallback.java

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
# Build the service
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
LOCAL_SRC_FILES := src/com/qualcomm/qti/sam/service/SAMService.java

LOCAL_PACKAGE_NAME := sam
LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_JAVA_LIBRARIES := saminterfacelibrary
LOCAL_JAVA_LIBRARIES += qcrilhook
LOCAL_JAVA_LIBRARIES += telephony-common

include $(BUILD_PACKAGE)

# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := sammanager.xml
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_CLASS := ETC

#this will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := saminterface.xml
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_CLASS := ETC

#this will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

endif
endif

endif

endif # TARGET_NO_TELEPHONY
