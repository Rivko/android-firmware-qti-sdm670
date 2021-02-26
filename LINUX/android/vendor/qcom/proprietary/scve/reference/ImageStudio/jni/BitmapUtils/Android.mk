LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libBitmapUtils
LOCAL_SRC_FILES := BitmapHelper.cpp BitmapBorderReplication.cpp BitmapAlpha8Utils.cpp
LOCAL_CXXFLAGS  := -std=gnu++11
LOCAL_SHARED_LIBRARIES    := libjnigraphics

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)

