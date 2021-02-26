ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/ldcwarp
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=              \
    camxchinodeldcwarp.h        \
    image_warping.h

LOCAL_SRC_FILES :=              \
    camxchinodeldcwarp.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES :=                            \
    $(CAMX_C_INCLUDES)                         \
    $(CAMX_CDK_PATH)/node/                     \
    $(CAMX_CDK_PATH)/generated/g_parser/       \
    $(CAMX_CDK_PATH)/generated/g_chromatix/    \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr \

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

LOCAL_SHARED_LIBRARIES :=           \
    libcamera_metadata              \
    libcom.qti.chinodeutils         \
    libcutils                       \

LOCAL_WHOLE_STATIC_LIBRARIES :=   \
    libcamxgenerated              \
    libcamxosutils                \
    libcamxutils

LOCAL_MODULE := com.qti.node.ldcwarp

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
