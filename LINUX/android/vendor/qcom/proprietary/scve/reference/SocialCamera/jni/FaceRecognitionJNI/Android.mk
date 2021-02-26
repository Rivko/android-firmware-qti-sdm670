LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libFaceRecognitionJNI
LOCAL_SRC_FILES := FaceRecognitionJNI.cpp

LOCAL_SHARED_LIBRARIES := liblog \
                          libdl \
                          libscveCommon \
                          libscveFaceRecognition

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
