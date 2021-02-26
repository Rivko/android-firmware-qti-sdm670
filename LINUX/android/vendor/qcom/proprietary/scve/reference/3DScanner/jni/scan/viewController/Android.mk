LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := viewController

# C Flags
LOCAL_ARM_NEON   := true
LOCAL_ARM_MODE   := arm
LOCAL_CFLAGS     += ${MY_GNU_C_FLAGS}

LOCAL_C_INCLUDES += ${COMMON_C_INCLUDE} \
						  $(LOCAL_PATH)/../..

#wildcard uses current path
LOCAL_EXPORT_C_INCLUDES = ${APP_JNI_ROOT}/${LOCAL_MODULE} $(APP_JNI_ROOT)

LOCAL_SRC_FILES         := $(notdir $(wildcard $(LOCAL_PATH)/*.cpp))

LOCAL_SHARED_LIBRARIES := libfastcvopt \
                          libjpeg

include vendor/qcom/proprietary/scve/reference/common.mk

include $(BUILD_STATIC_LIBRARY)
