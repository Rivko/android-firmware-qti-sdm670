ifneq ($(TARGET_NO_TELEPHONY), true)
ifeq ($(TARGET_USES_QCOM_BSP_ATEL), true)

 LOCAL_PATH:= $(call my-dir)
 include $(CLEAR_VARS)
 LOCAL_MODULE_TAGS := optional eng

 LOCAL_SRC_FILES := $(call all-subdir-java-files)
 LOCAL_SRC_FILES += \
        src/com/qualcomm/qti/networksetting/INetworkQueryService.aidl \
        src/com/qualcomm/qti/networksetting/INetworkQueryServiceCallback.aidl
 LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
 LOCAL_JAVA_LIBRARIES := telephony-common qcrilhook ims-common telephony-ext

 LOCAL_PACKAGE_NAME := NetworkSetting

 LOCAL_CERTIFICATE := platform

 include $(BUILD_PACKAGE)
endif
endif # TARGET_NO_TELEPHONY
