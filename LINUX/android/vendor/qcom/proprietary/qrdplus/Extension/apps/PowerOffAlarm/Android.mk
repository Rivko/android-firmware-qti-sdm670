ifneq ($(TARGET_USES_IOTCC_HEADLESS),true)

LOCAL_PATH:= $(call my-dir)

# ==========================================================================
# Build Package
# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

LOCAL_PACKAGE_NAME := PowerOffAlarm
LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES := \
    android.hidl.base-V1.0-java \
    vendor.qti.hardware.alarm-V1.0-java \

include $(BUILD_PACKAGE)

# ==========================================================================
# Install the Library
# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := vendor-qti-hardware-alarm.xml
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
