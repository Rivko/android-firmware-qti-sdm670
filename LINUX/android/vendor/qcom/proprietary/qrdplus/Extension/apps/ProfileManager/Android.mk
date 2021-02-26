LOCAL_PATH:= $(call my-dir)

#MAKE_JAR
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(call all-java-files-under, src-Wrapper)
LOCAL_JAVA_LIBRARIES := telephony-common
LOCAL_MODULE := profilemanager_wrapper
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
include $(BUILD_STATIC_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := \
    telephony-common \
    profilemanager_wrapper

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := ProfileMgr

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
