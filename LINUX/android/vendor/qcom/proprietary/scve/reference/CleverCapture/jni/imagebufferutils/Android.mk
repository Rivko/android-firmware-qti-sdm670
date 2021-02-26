LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE            := libimagebufferutils
LOCAL_CFLAGS            := -Werror -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_SHARED_LIBRARIES  := libjnigraphics
LOCAL_SRC_FILES         := imagebufferutils.cpp

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
