ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/hvx/binning
endif

include $(CLEAR_VARS)
LOCAL_MODULE                := libdsp_streamer_binning
LOCAL_MODULE_SUFFIX         := .so
LOCAL_MODULE_CLASS          := ETC
LOCAL_SRC_FILES             := libdsp_streamer_binning.so
LOCAL_MODULE_TAGS           := optional
LOCAL_MODULE_OWNER          := qti
LOCAL_PROPRIETARY_MODULE    := true
LOCAL_MODULE_PATH           := $(TARGET_OUT_VENDOR)/lib/rfsa/adsp
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=

LOCAL_SRC_FILES :=              \
    camxchihvxbinning.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES +=             \
    $(CAMX_C_INCLUDES)          \
    system/media/camera/include

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Binary Name
LOCAL_MODULE := com.qti.hvx.binning

LOCAL_MODULE_RELATIVE_PATH:= $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
-include $(CAMX_CHECK_WHINER)
