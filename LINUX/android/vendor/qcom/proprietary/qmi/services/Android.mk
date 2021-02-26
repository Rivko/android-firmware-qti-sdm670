LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wno-missing-field-initializers

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../platform
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../core/lib/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc

LOCAL_SHARED_LIBRARIES := libidl
LOCAL_WHOLE_STATIC_LIBRARIES := libimsqmiservices
LOCAL_WHOLE_STATIC_LIBRARIES += libdataqmiservices
LOCAL_WHOLE_STATIC_LIBRARIES += librilqmiservices


LOCAL_SRC_FILES := common_v01.c
LOCAL_SRC_FILES += sar_vs_service_v01.c
LOCAL_SRC_FILES += lowi_service_v01.c
LOCAL_SRC_FILES += secure_filesystem_service_v01.c
LOCAL_SRC_FILES += bandwidth_limits_management_v01.c

LOCAL_MODULE:= libqmiservices

LOCAL_MODULE_TAGS := optional

LOCAL_CLANG := true

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
