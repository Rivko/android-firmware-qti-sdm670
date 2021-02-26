LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(call is-board-platform-in-list, msm8998 msmcobalt),true)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc

LOCAL_SRC_FILES:= \
   ccid_uim_daemon.c \
   ccid_uim_parsing.c

LOCAL_SHARED_LIBRARIES := libqmi_cci libqmi_encdec libqmiservices
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

LOCAL_MODULE:= ccid_daemon
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

endif

