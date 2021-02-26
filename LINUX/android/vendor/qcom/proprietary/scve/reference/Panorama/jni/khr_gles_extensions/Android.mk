LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libpanoramaref_khr_gles_extensions
LOCAL_CFLAGS           := -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_C_INCLUDES       := vendor/qcom/proprietary/scve/reference/Panorama/jni/util
LOCAL_SRC_FILES        := gl_oes_egl_image.cpp \
                          gl_read_pixels.cpp
LOCAL_SHARED_LIBRARIES := libEGL libGLESv2 libjnigraphics
LOCAL_STATIC_LIBRARIES := libpanoramaref_util

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
