LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE            := libpanoramaref_exifhelper
LOCAL_SRC_FILES := exifhelper.cpp
LOCAL_SHARED_LIBRARIES  := libjpeg
include vendor/qcom/proprietary/scve/reference/common.mk
include $(BUILD_STATIC_LIBRARY)
