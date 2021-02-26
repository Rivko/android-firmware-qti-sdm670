#
# QtiTelephony service
#
ifneq ($(TARGET_NO_TELEPHONY), true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := QtiTelephonyServicelibrary
LOCAL_JAVA_LIBRARIES := qcrilhook android.hidl.base-V1.0-java android.hidl.manager-V1.0-java
LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.radio.am-V1.0-java

LOCAL_SRC_FILES := $(call all-java-files-under, src) \
                   $(call all-proto-files-under, proto)
LOCAL_SRC_FILES += src/com/qualcomm/qti/telephonyservice/IQtiTelephonyService.aidl
LOCAL_PROTOC_OPTIMIZE_TYPE := micro
include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := QtiTelephonyService

LOCAL_JAVA_LIBRARIES := qcrilhook android.hidl.base-V1.0-java android.hidl.manager-V1.0-java
LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.radio.am-V1.0-java

LOCAL_SRC_FILES := $(call all-java-files-under, src) \
                   $(call all-proto-files-under, proto)

LOCAL_SRC_FILES += src/com/qualcomm/qti/telephonyservice/IQtiTelephonyService.aidl

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

LOCAL_PROTOC_OPTIMIZE_TYPE := micro

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)
LOCAL_MODULE := telephonyservice.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

endif # TARGET_NO_TELEPHONY

