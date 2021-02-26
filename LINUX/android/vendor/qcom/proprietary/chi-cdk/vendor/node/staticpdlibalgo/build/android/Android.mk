ifeq ($(CAMX_CHICDK_PATH),)
LOCAL_PATH := $(abspath $(call my-dir)/../..)
CAMX_CHICDK_PATH := $(abspath $(LOCAL_PATH)/../../..)
else
LOCAL_PATH := $(CAMX_CHICDK_PATH)/vendor/node/staticpdlibalgo
endif

include $(CLEAR_VARS)

# Module supports function call tracing via ENABLE_FUNCTION_CALL_TRACE
# Required before including common.mk
SUPPORT_FUNCTION_CALL_TRACE := 1

# Get definitions common to the CHI-CDK project here
include $(CAMX_CHICDK_PATH)/vendor/common.mk

LOCAL_SRC_FILES :=  \
    src/camxpdlib.cpp

LOCAL_INC_FILES :=

# Put here any libraries that should be linked by CAMX projects
LOCAL_C_LIBS := $(CAMX_C_LIBS)

CHROMATIX_VERSION := 0x0310
# Paths to included headers
LOCAL_C_INCLUDES :=                         \
    $(CAMX_C_INCLUDES)                      \
    $(CAMX_CDK_PATH)/generated/g_chromatix  \
    $(CAMX_CDK_PATH)/generated/g_parser     \
    system/media/camera/include

LOCAL_C_INCLUDES +=                         \
    $(CAMX_CDK_PATH)                        \
    $(CAMX_CDK_PATH)/stats                  \
    $(CAMX_VENDOR_PATH)/node/staticpdlibalgo

# Libraries to link
LOCAL_WHOLE_STATIC_LIBRARIES := \
    libcamxosutils              \
    libcamxutils                \
    libcamxgenerated

LOCAL_SHARED_LIBRARIES :=   \
    libcutils               \
    liblog                  \
    libsync

# Compiler flags
LOCAL_CFLAGS := $(CAMX_CFLAGS)
LOCAL_CPPFLAGS := $(CAMX_CPPFLAGS)

# Binary name
LOCAL_MODULE := com.qtistatic.stats.pdlib

LOCAL_MODULE_RELATIVE_PATH := $(CAMX_LIB_OUTPUT_PATH)

LOCAL_LDFLAGS :=
include $(BUILD_SHARED_LIBRARY)
# WHINER is disabled temporarily until 3A core is rewritten
#-include $(CAMX_CHECK_WHINER)