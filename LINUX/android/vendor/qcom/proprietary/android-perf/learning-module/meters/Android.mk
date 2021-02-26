# LM_PATH points to learning-module folder and comes from main Android.mk
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := reference/ReferenceMeter.cpp

LOCAL_SHARED_LIBRARIES := liblog libc libcutils liblearningmodule
LOCAL_CFLAGS := -Wno-unused-parameter
LOCAL_C_INCLUDES := $(LM_INC_PATH)/utils \
    $(LM_INC_PATH)/metameters \
    $(MPCTL_PATH)
LOCAL_MULTILIB := first
LOCAL_MODULE := libmeters
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
