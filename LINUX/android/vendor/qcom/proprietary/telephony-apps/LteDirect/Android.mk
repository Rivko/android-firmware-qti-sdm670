#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/
ifneq ($(TARGET_NO_TELEPHONY), true)

LOCAL_PATH:= $(call my-dir)

ifneq ($(TARGET_HAS_LOW_RAM),true)

# ==========================================================================
# Build the auto generated files into a library to be used by both the
# app and the service
include $(CLEAR_VARS)

src_proto := src

LOCAL_MODULE_TAGS := debug

LOCAL_PROTOC_OPTIMIZE_TYPE := micro

LOCAL_MODULE := LteDirectDiscoveryLibrary

LOCAL_SRC_FILES += src/com/qualcomm/qti/ltedirect/internal/ILteDirectDiscoveryService.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/ltedirect/internal/ILteDirectDiscovery.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/ltedirect/internal/ILteDirectDiscoveryStatusListener.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/ltedirect/internal/LteDirectDiscoveryCapability.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/ltedirect/LteDirectDiscovery.java
LOCAL_SRC_FILES += $(call all-proto-files-under, $(src_proto))

include $(BUILD_JAVA_LIBRARY)

endif #TARGET_HAS_LOW_RAM

# ==========================================================================
# Build Library with nano-pb generated .c/.h files from proto
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/LteDirectDiscovery.proto

LOCAL_C_INCLUDES += external/nanopb-c/

LOCAL_C_FILES :=

LOCAL_PROTOC_OPTIMIZE_TYPE := nanopb-c-enable_malloc

LOCAL_MODULE:= libril-qc-ltedirectdisc

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

ltedirectdisc_generated_dir := $(call generated-sources-dir-for,SHARED_LIBRARIES,$(LOCAL_MODULE),,)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(ltedirectdisc_generated_dir)/proto/$(LOCAL_PATH)/src

include $(BUILD_SHARED_LIBRARY)

ifneq ($(TARGET_HAS_LOW_RAM),true)

# ==========================================================================
# Install xml to /system/etc/permissions
include $(CLEAR_VARS)

LOCAL_MODULE := LteDirectDiscovery.xml

LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================

endif

endif # TARGET_NO_TELEPHONY
