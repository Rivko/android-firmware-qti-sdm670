LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libpanoramaref_jni
LOCAL_SRC_FILES := StandalonePanoramaEngine.cpp

LOCAL_C_INCLUDES := vendor/qcom/proprietary/scve/reference/Panorama/jni/util \
                    vendor/qcom/proprietary/scve/reference/Panorama/jni/exifhelper

LOCAL_SHARED_LIBRARIES := libscvePanorama liblog libdl

LOCAL_STATIC_LIBRARIES := libpanoramaref_util libpanoramaref_exifhelper

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_SHARED_LIBRARY)
