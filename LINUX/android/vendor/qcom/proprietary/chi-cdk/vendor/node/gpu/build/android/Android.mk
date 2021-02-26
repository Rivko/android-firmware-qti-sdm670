ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/gpu/
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=              \
    camxchinodegpu.h

LOCAL_SRC_FILES :=              \
    camxchinodegpu.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES :=             \
    $(CAMX_C_INCLUDES)          \
    frameworks/native/libs/arect/include \
    frameworks/native/libs/nativewindow/include \
    $(TARGET_OUT_HEADERS)/adreno \
    system/media/camera/include

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

LOCAL_WHOLE_STATIC_LIBRARIES := \
    libcamxosutils              \
    libcamxutils


LOCAL_SHARED_LIBRARIES :=   \
    libcamera_metadata      \
    libcom.qti.chinodeutils \
    libcutils               \
    liblog                  \
    libsync

LOCAL_MODULE := com.qti.node.gpu

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
#-include $(CAMX_CHECK_WHINER)
