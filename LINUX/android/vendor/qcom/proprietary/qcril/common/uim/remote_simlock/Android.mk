LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

# Copy header file to out directory
LOCAL_COPY_HEADERS_TO := qcril/uim/inc
LOCAL_COPY_HEADERS := remote_simlock.h

LOCAL_MODULE := lib_remote_simlock
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

# Logging Features. Turn any one ON at any time

LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -g

##REMOTE_SIMLOCK##
LOCAL_SRC_FILES := remote_simlock.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include

##Used libraries
LOCAL_SHARED_LIBRARIES = libqmi_cci
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libqmiservices

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_ADDITIONAL_DEPENDENCIES += qcril_features_def.h

include $(BUILD_SHARED_LIBRARY)
