#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

ifeq ($(BOARD_SUPPORTS_SOUND_TRIGGER),true)

LOCAL_PATH:= $(call my-dir)
#=============================================
#  Generate SoundTrigger JAR
#=============================================
include $(CLEAR_VARS)

#include all original java files
LOCAL_SRC_FILES += $(call all-java-files-under, src)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := SoundTriggerJAR

#replaced ANDROID_COMPILE_WITH_JACK by the local flag and it will only affect this module
LOCAL_JACK_ENABLED := disabled
include $(BUILD_STATIC_JAVA_LIBRARY)

endif

