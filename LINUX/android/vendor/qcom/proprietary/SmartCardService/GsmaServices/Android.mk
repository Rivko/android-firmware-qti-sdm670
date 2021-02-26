ifeq ($(strip $(TARGET_ENABLE_PROPRIETARY_GSMA_SERVICE)),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PACKAGE_NAME := GsmaNfcService
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_JAVA_LIBRARIES := com.gsma.services.nfc com.nxp.nfc.nq
LOCAL_PROGUARD_ENABLED := disabled
include $(BUILD_PACKAGE)

# ====  permissions ========================
include $(CLEAR_VARS)

LOCAL_MODULE := com.gsma.services.nfc.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
# Install to /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := com.gsma.services.utils.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
# Install to /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif
