ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/defog
endif

include $(CLEAR_VARS)

# Get definitions common to the CAMX project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_INC_FILES :=              \
    camxchinodedefog.h

LOCAL_SRC_FILES :=              \
    camxchinodedefog.cpp

LOCAL_C_LIBS := $(CAMX_C_LIBS)

LOCAL_C_INCLUDES :=                            \
    $(CAMX_C_INCLUDES)                         \
    $(CAMX_CHICDK_PATH)/../camx/src/hwl/iqinterpolation/  \
    $(CAMX_CHICDK_PATH)/../camx/src/hwl/iqsetting/        \
    $(CAMX_CDK_PATH)/node/                     \
    $(CAMX_CDK_PATH)/generated/g_parser/       \
    $(CAMX_CDK_PATH)/generated/g_chromatix/    \
    $(CAMX_CDK_PATH)/../../camx/src/core/      \
    $(CAMX_CDK_PATH)/../../camx/src/core/hal/  \
    $(CAMX_CDK_PATH)/../../camx/src/core/chi/  \
    $(CAMX_CDK_PATH)/../../camx/src/csl/       \
    $(LOCAL_PATH)/../../../../camx-lib/system/defog/inc/  \
    $(CAMX_CDK_PATH)/../vendor/node/yuvshdr/              \
    system/media/camera/include

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

LOCAL_SHARED_LIBRARIES :=           \
    libcamera_metadata              \
    libcom.qti.chinodeutils         \

LOCAL_WHOLE_STATIC_LIBRARIES := \
    libcamxgenerated            \
    libcamxosutils              \
    libcamxutils

LOCAL_MODULE := com.qti.node.defog

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

include $(BUILD_SHARED_LIBRARY)
