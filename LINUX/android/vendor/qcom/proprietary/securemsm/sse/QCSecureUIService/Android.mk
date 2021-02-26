ifeq ($(call is-board-platform-in-list, msm8937 msm8953 msm8998 apq8098_latv sdm660 sdm845 sdm710),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := com.qualcomm.qti.services.secureui

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PROGUARD_FLAG_FILES := proguard.flags
#LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES := WfdCommon telephony-common

include $(BUILD_PACKAGE)

endif
