ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += $ src/com/qualcomm/qti/qtisystemservice/ISystemService.aidl

LOCAL_PACKAGE_NAME := DeviceInfo

LOCAL_JAVA_LIBRARIES += telephony-common telephony-ext

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

endif
endif # TARGET_NO_TELEPHONY
