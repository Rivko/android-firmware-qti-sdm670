
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libImageCloningSample
LOCAL_SRC_FILES := ImageCloningSample.cpp
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/include/scve
LOCAL_SHARED_LIBRARIES += libscveImageCloning liblog libdl libjnigraphics

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)

