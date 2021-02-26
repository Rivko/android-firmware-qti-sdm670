LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := athdiag
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += -Wall -Werror
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../uapi/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../target/inc
LOCAL_SHARED_LIBRARIES := libc libcutils
LOCAL_SRC_FILES := athdiag.c
include $(BUILD_EXECUTABLE)
