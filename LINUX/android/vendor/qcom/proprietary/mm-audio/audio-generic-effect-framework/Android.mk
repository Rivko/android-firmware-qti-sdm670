LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES := src/qti_gef_api.c

LOCAL_C_INCLUDES  += $(LOCAL_PATH)/api \
                     $(LOCAL_PATH)/inc \
                     system/core/include/utils

LOCAL_CFLAGS += -Werror -Wall

LOCAL_CFLAGS += -DGEF_PLATFORM_NAME=$(TARGET_BOARD_PLATFORM)

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libcutils \
        libdl

LOCAL_MODULE := libqtigef
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
