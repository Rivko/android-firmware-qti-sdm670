EA_BOARD_PLATFORM_LIST += msm8996

ifeq ($(call is-board-platform-in-list,$(EA_BOARD_PLATFORM_LIST)),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_SHARED_LIBRARIES += libutils

LOCAL_MODULE := cpu-voltage-matcher

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS += \
    -DUSE_ANDROID_LOG

LOCAL_SRC_FILES := ./cpu-voltage-matcher.c

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

endif
