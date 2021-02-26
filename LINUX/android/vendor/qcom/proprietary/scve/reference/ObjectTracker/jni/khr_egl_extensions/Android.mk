LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libobjecttrackerref_khr_egl_extensions
LOCAL_C_INCLUDES       := vendor/qcom/proprietary/scve/reference/ObjectTracker/jni/util
LOCAL_SRC_FILES        := entrypoint.cpp \
                          androidpresentationtime.cpp \
                          fencesync.cpp \
                          imagebase.cpp
LOCAL_SHARED_LIBRARIES := libEGL
LOCAL_STATIC_LIBRARIES := libobjecttrackerref_util

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
