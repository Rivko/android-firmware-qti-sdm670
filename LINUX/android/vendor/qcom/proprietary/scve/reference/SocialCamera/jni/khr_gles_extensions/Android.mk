LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := libsocialcamera_khr_gles_extensions
LOCAL_C_INCLUDES := vendor/qcom/proprietary/scve/reference/SocialCamera/jni/util
LOCAL_CFLAGS     := -Werror -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_SHARED_LIBRARIES := libEGL libGLESv2 libsocialcamera_util
LOCAL_SRC_FILES  := gl_read_pixels.cpp

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
