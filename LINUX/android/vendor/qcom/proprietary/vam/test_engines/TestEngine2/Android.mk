LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/../..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build TestEngine2 library
# libEngine_Test2.so

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_SRC_FILES := ../common/VAMUtilities.cc
LOCAL_SRC_FILES += testEngine2.cc

LOCAL_MODULE = libEngine_Test2

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_VAM
