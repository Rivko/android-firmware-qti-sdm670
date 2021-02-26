LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libImageRemoval
LOCAL_SRC_FILES := ImageRemoval.cpp
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/include/scve
LOCAL_SHARED_LIBRARIES += liblog libdl libjnigraphics libscveImageRemoval

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)

