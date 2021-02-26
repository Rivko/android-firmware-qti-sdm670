LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build CustomEngineTemplate library
# libEngine_CustomEngineTemplate.s0

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_SRC_FILES := vam_engine_wrapper_template.cc

LOCAL_SHARED_LIBRARIES += libjson_apis

LOCAL_MODULE = libEngine_CustomEngineTemplate

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_VAM
