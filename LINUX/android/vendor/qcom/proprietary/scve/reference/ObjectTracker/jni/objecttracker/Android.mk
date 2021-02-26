LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE           := libobjecttrackerref_jni
LOCAL_C_INCLUDES       := vendor/qcom/proprietary/scve/reference/ObjectTracker/jni/util
LOCAL_SRC_FILES        := objecttracker.cpp
LOCAL_SHARED_LIBRARIES := libscveObjectTracker
LOCAL_STATIC_LIBRARIES := libobjecttrackerref_util

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
