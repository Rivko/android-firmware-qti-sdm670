################################################################################
# @file  Android.mk
# @brief Rules for compiling the source files
################################################################################
ifneq ($(TARGET_NO_TELEPHONY), true)

LOCAL_PATH:= $(call my-dir)
# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional debug
LOCAL_JAVA_LIBRARIES := telephony-common

LOCAL_MODULE := qcrilhook
LOCAL_SRC_FILES := $(call all-java-files-under,src/com/qualcomm/qcrilhook)
LOCAL_SRC_FILES += src/com/qualcomm/qcrilhook/IOemHookCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qcrilmsgtunnel/IQcrilMsgTunnel.aidl

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional debug
LOCAL_JAVA_LIBRARIES := telephony-common

LOCAL_MODULE := qcrilhook-static
LOCAL_SRC_FILES := $(call all-java-files-under,src/com/qualcomm/qcrilhook)
LOCAL_SRC_FILES += src/com/qualcomm/qcrilhook/IOemHookCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qcrilmsgtunnel/IQcrilMsgTunnel.aidl


include $(BUILD_STATIC_JAVA_LIBRARY)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := qcrilhook.xml

LOCAL_MODULE_TAGS := optional debug

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional debug
LOCAL_JAVA_LIBRARIES := telephony-common android.hidl.base-V1.0-java android.hidl.manager-V1.0-java
LOCAL_STATIC_JAVA_LIBRARIES := vendor.qti.hardware.radio.qcrilhook-V1.0-java-static

LOCAL_PRIVILEGED_MODULE := true

LOCAL_SRC_FILES := $(call all-java-files-under,src/com/qualcomm/qcrilmsgtunnel)
LOCAL_SRC_FILES += src/com/qualcomm/qcrilhook/IOemHookCallback.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qcrilmsgtunnel/IQcrilMsgTunnel.aidl

LOCAL_PACKAGE_NAME := qcrilmsgtunnel
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif # TARGET_NO_TELEPHONY
