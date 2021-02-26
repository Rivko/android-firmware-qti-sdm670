LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libobjecttrackerref_khr_gles_extensions
LOCAL_C_INCLUDES       := vendor/qcom/proprietary/scve/reference/ObjectTracker/jni/util
LOCAL_SRC_FILES        := entrypoint.cpp \
                          eglimage.cpp \
                          gles30helper.cpp \
                          glreadpixels.cpp
LOCAL_SHARED_LIBRARIES := libEGL libGLESv2 libjnigraphics
LOCAL_STATIC_LIBRARIES := libobjecttrackerref_util

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
