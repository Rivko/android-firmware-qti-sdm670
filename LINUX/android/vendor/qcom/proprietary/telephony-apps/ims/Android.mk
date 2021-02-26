#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/
ifneq ($(TARGET_NO_TELEPHONY), true)
src_java_ims := src/org/codeaurora/ims
src_java_ims += src/com/qualcomm/ims/vt
src_java_ims += src/com/qualcomm/ims/utils
src_proto := src

LOCAL_PATH:= $(call my-dir)

# ==========================================================================
# Build the service
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PACKAGE_NAME := ims
LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_JAVA_LIBRARIES := telephony-common ims-common voip-common \
    android.hidl.base-V1.0-java  android.hidl.manager-V1.0-java
LOCAL_STATIC_JAVA_LIBRARIES := ims-ext-common vendor.qti.hardware.radio.ims-V1.0-java-static

LOCAL_JNI_SHARED_LIBRARIES := libimsmedia_jni libimscamera_jni
LOCAL_REQUIRED_MODULES := libimsmedia_jni libimscamera_jni

LOCAL_PROTOC_OPTIMIZE_TYPE := micro

LOCAL_SRC_FILES := $(call all-java-files-under, $(src_java_ims)) \
    $(call all-proto-files-under, $(src_proto))

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
# ==========================================================================
endif # TARGET_NO_TELEPHONY
