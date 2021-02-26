LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_JAVA_LIBRARIES := telephony-common  ims-common
LOCAL_JAVA_LIBRARIES += org.apache.http.legacy.boot
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := ExtSettings

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
