LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libfeature_cv_jni
LOCAL_SRC_FILES :=  feature_cv_jni.cpp

LOCAL_SHARED_LIBRARIES := libcvmanager \
                          liblog

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
