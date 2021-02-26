ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += $ src/com/qualcomm/qti/qtisystemservice/ICallStateListener.aidl \
    src/com/qualcomm/qti/qtisystemservice/ITelecomCallService.aidl \
    src/org/codeaurora/callenhancement/ICallRecordStateListener.aidl \
    src/org/codeaurora/callenhancement/ICallEnhancement.aidl


LOCAL_PACKAGE_NAME := CallEnhancement

LOCAL_JAVA_LIBRARIES += telephony-common

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

endif
endif # TARGET_NO_TELEPHONY
