LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
            $(call all-subdir-java-files) \
            $(call all-Iaidl-files-under, src)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= QPerformance

# This will install the file in /system/framework
LOCAL_MODULE_PATH := $(TARGET_OUT_JAVA_LIBRARIES)

LOCAL_CERTIFICATE := platform
LOCAL_MODULE_OWNER := qti
LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_JAVA_LIBRARY)

# ====  permissions ========================
include $(CLEAR_VARS)

LOCAL_MODULE := com.qualcomm.qti.Performance.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ====  To compile JNI ========================
include $(LOCAL_PATH)/jni/Android.mk
