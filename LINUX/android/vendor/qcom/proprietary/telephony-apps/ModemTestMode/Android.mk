ifneq ($(TARGET_NO_TELEPHONY), true)
ifneq ($(TARGET_HAS_LOW_RAM),true)

ifeq ($(TARGET_USES_QCOM_BSP_ATEL), true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional eng
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := ModemTestMode
LOCAL_CERTIFICATE := platform
LOCAL_JAVA_LIBRARIES := qcrilhook telephony-common

include $(BUILD_PACKAGE)
#$(shell cp -r $(LOCAL_PATH)/labtest-apns-conf.xml $(TARGET_OUT)/etc/)
endif
endif
endif # TARGET_NO_TELEPHONY

