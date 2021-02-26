LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := org.simalliance.openmobileapi.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += \ $(call all-java-files-under, ../common/src)
LOCAL_SRC_FILES += \ $(call all-Iaidl-files-under, ../common/src)

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/../common/src/

LOCAL_MODULE:= org.simalliance.openmobileapi
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES:= org.simalliance.openmobileapi.xml

include $(BUILD_JAVA_LIBRARY)
