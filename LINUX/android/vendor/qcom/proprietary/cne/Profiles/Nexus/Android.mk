LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ROW_profiles.xml
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_CLASS := ETC
#This will install the file in /system/etc/cne/Nexus/ROW
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/cne/Nexus/ROW
LOCAL_SRC_FILES := ROW/profiles.xml
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := VZW_profiles.xml
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_CLASS := ETC
#This will install the file in /system/etc/cne/Nexus/VZW
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/cne/Nexus/VZW
LOCAL_SRC_FILES := VZW/profiles.xml
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ATT_profiles.xml
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_CLASS := ETC
#This will install the file in /system/etc/cne/Nexus/ATT
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/cne/Nexus/ATT
LOCAL_SRC_FILES := ATT/profiles.xml
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)