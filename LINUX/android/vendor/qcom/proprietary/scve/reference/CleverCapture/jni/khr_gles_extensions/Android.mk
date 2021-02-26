LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := libclevercapture_khr_gles_extensions
LOCAL_CFLAGS     := -Werror -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_SHARED_LIBRARIES := libEGL libGLESv2 libclevercapture_util

LOCAL_C_INCLUDES := vendor/qcom/proprietary/scve/reference/CleverCapture/jni/util
LOCAL_SRC_FILES  := gl_oes_egl_image.cpp \
                    gl_read_pixels.cpp

include vendor/qcom/proprietary/scve/reference/common.mk
include $(BUILD_SHARED_LIBRARY)
