LOCAL_PATH := $(call my-dir)

VAM_TOP_SRCDIR := $(LOCAL_PATH)/../..
VAM_LIB_TOP_SRCDIR := $(LOCAL_PATH)/..

include $(VAM_TOP_SRCDIR)/build.mk

ifneq (,$(BUILD_VAM))

# Build VAManager library
# libVAManager.so

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := VAM
LOCAL_COPY_HEADERS := ../inc/vam_engine_api.h
LOCAL_COPY_HEADERS += ../inc/vaapi_common.h
LOCAL_COPY_HEADERS += ../inc/vaapi_config.h
LOCAL_COPY_HEADERS += ../inc/vaapi_errors.h
LOCAL_COPY_HEADERS += ../inc/VAMUtilities.h
LOCAL_COPY_HEADERS += ../inc/vaapi.h

include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)

include $(VAM_TOP_SRCDIR)/common.mk

LOCAL_C_INCLUDES += $(VAM_LIB_TOP_SRCDIR)/inc

LOCAL_SRC_FILES := VAAPI.cc
LOCAL_SRC_FILES += VAManager.cc
LOCAL_SRC_FILES += VAMUtilities.cc
LOCAL_SRC_FILES += VAMEngineBase.cc
LOCAL_SRC_FILES += VAMEngine_DynEngine.cc

LOCAL_MODULE = libVAManager

include $(BUILD_SHARED_LIBRARY)

endif # BUILD_VAM
