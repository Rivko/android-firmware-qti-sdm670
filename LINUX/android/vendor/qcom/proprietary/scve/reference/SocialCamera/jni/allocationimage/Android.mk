LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liballocationimage
LOCAL_SRC_FILES := allocationimage.cpp
LOCAL_SHARED_LIBRARIES := liblog \
                          libjnigraphics

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
