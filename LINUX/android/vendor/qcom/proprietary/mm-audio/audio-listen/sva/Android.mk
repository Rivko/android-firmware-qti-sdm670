LOCAL_PATH := $(call my-dir)

ifeq ($(SVA_RELOCAITON_DISABLED),true)
ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER)),true)

#=============================================
#  Listen App using SoundTrigger
#=============================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := SVA

LOCAL_PROPRIETARY_MODULE := true

LOCAL_JNI_SHARED_LIBRARIES := liblistenjni
LOCAL_MULTILIB := 32

LOCAL_CERTIFICATE := platform

LOCAL_STATIC_JAVA_LIBRARIES := Listen

include $(BUILD_PACKAGE)

endif
endif