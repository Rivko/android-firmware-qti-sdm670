$(info "Building native nfc app...")

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := thumb

LOCAL_MODULE_TAGS:= optional

LOCAL_SRC_FILES := src/main.c

LOCAL_CFLAGS := -Wall -Wextra -Werror

LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

# include
LOCAL_C_INCLUDES := -I $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_SHARED_LIBRARIES += libdl libcutils liblog

LOCAL_MODULE:= nqnfcinfo
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_EXECUTABLE)

