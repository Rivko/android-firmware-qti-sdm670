TOP_LOCAL_PATH := $(call my-dir)
#
# Build JNI library
#
include $(TOP_LOCAL_PATH)/jni/Android.mk

#include HIDL
include $(TOP_LOCAL_PATH)/hidl/Android.mk

#include Daemon
include $(TOP_LOCAL_PATH)/daemon/Android.mk

#
# Build sample application package
#
LOCAL_PATH:= $(TOP_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-subdir-java-files)
LOCAL_PACKAGE_NAME := Qmmi
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_JAVA_LIBRARIES := android.hidl.base-V1.0-java \
                        android.hardware.light-V2.0-java \
                        vendor.qti.hardware.factory-V1.0-java \
                        qcom.fmradio \

#static linking required
LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.fingerprint-V1.0-java-static

LOCAL_JNI_SHARED_LIBRARIES := libmmi_jni

LOCAL_SHARED_LIBRARIES := mmi_sensor

LOCAL_MODULE_TAGS := debug optional

LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_PACKAGE)

# ==========================================================================
# Install the Library
# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := android.hardware.light.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := vendor.qti.hardware.factory.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
