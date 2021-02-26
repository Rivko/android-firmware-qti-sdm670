LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libclevercapture
LOCAL_SRC_FILES := clevercapture_engine.cpp
LOCAL_SHARED_LIBRARIES := liblog \
                          libscveCleverCapture

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
