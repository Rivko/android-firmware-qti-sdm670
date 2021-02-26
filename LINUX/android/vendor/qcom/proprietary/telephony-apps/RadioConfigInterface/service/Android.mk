#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)

# Build Library with nano-pb generated .c/.h files from proto
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/radio_config_interface.proto

LOCAL_C_INCLUDES += external/nanopb-c/

LOCAL_C_FILES :=

LOCAL_PROTOC_OPTIMIZE_TYPE := nanopb-c-enable_malloc

LOCAL_MODULE:= libril-qc-radioconfig

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

radioconfig_generated_dir := $(call generated-sources-dir-for,SHARED_LIBRARIES,$(LOCAL_MODULE),,)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(radioconfig_generated_dir)/proto/$(LOCAL_PATH)/src

include $(BUILD_SHARED_LIBRARY)
