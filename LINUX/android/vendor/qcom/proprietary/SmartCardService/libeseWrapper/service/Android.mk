LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_SRC_FILES := src/com/qualcomm/qti/libesewrapper/eSEWrapper.java \

LOCAL_MODULE:= com.qualcomm.qti.libesewrapper

include $(BUILD_JAVA_LIBRARY)

# ====  permissions ========================
include $(CLEAR_VARS)

LOCAL_MODULE := com.qualcomm.qti.libesewrapper.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# Install to /vendor/etc/permissions
LOCAL_MODULE_PATH :=  $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(LOCAL_PATH)/jni/Android.mk
