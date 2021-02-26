LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libSoftSegmentation
LOCAL_SRC_FILES := SoftSegmentation.cpp
LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/include/scve
LOCAL_SHARED_LIBRARIES := \
   liblog \
   libdl \
   libjnigraphics \
   libscveObjectSegmentation \
   libscveObjectMatting \
   libfastcvopt

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)

