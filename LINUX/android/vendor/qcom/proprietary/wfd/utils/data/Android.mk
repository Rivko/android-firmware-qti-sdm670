LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#            COPY THE WFDCONFIG.XML FILE TO /data
# ---------------------------------------------------------------------------------

LOCAL_MODULE:= wfdconfig.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)
include $(BUILD_PREBUILT)

# ---------------------------------------------------------------------------------
#            COPY THE WFDCONFIGSINK.XML FILE TO /system/etc
# ---------------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE:= wfdconfigsink.xml
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)
include $(BUILD_PREBUILT)

