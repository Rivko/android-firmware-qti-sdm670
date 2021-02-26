##############################################################################
# @file vendor/qcom-proprietary/time_services/Android.mk
# @brief Makefile for building the time services library on Android.
##############################################################################

#################### Time services Shared library ############################

ifneq ($(call is-board-platform, msm7627a msm8625 msm7630_surf msm8660),true)
######################### Build for QMI version ############################
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

commonIncludes := $(TARGET_OUT_HEADERS)/common/inc

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/time-services

LOCAL_SRC_FILES := time_genoff_qmi.c

LOCAL_SHARED_LIBRARIES := libc
LOCAL_SHARED_LIBRARIES += libcutils liblog

LOCAL_MODULE := libtime_genoff
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

###################### STATIC LIBRARY #######################################
include $(CLEAR_VARS)

commonIncludes := $(TARGET_OUT_HEADERS)/common/inc

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/time-services

LOCAL_SRC_FILES := time_genoff_qmi.c

LOCAL_SHARED_LIBRARIES := libc
LOCAL_SHARED_LIBRARIES += libcutils

LOCAL_MODULE := libtime_genoff
LOCAL_MODULE_TAGS := optional debug
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_STATIC_LIBRARY)

###################### Time services daemon  #################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= time_daemon_qmi.c time_service_v01.c wlan_time_service_v01.c

commonIncludes := $(TARGET_OUT_HEADERS)/common/inc

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/qcci/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(commonIncludes)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/time-services

LOCAL_SHARED_LIBRARIES := libc
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_common_so

LOCAL_MODULE := time_daemon
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := TimeService
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif #legacy targets
