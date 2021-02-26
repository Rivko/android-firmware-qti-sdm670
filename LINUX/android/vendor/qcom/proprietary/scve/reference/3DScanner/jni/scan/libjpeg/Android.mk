LOCAL_PATH:= $(call my-dir)

####################################################
# libjpgIO.a
####################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libjpgIO

# C Flags
LOCAL_ARM_NEON   := true
LOCAL_CFLAGS     += ${MY_GNU_C_FLAGS}

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../..

LOCAL_SRC_FILES         := jpgIO.cpp

LOCAL_SHARED_LIBRARIES += libjpeg

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_STATIC_LIBRARY)
