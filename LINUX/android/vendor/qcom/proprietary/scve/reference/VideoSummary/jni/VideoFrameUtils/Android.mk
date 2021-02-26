LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libVideoFrameUtils
LOCAL_SRC_FILES := VideoFrameUtils.cpp
include vendor/qcom/proprietary/scve/reference/common.mk
include $(BUILD_SHARED_LIBRARY)
