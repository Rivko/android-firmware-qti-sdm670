LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/../..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build GTRenderer library
# libEngine_GTRenderer.so

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_C_INCLUDES := $(VAM_TOP_SRCDIR)/vam_utils/JSON/json_apis/include/

LOCAL_SRC_FILES := ../common/VAMUtilities.cc
LOCAL_SRC_FILES += engineGenerator.cc

LOCAL_SHARED_LIBRARIES += libjson_apis

LOCAL_MODULE = libEngine_GTRenderer

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_VAM
