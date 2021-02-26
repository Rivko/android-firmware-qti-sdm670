LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := wbc_hal.default
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_PATH_32 := $(TARGET_OUT)/lib
LOCAL_MODULE_PATH_64 := $(TARGET_OUT)/lib64
else
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
endif

LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/common/inc
LOCAL_SRC_FILES += file_access.c wbc_uevent.c wbc_hal_interface.c

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        liblog

LOCAL_CFLAGS := -Wall
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
