LOCAL_PATH := $(call my-dir)

# -----------------------------------------------------------
#       Make the Limits HAL service
# -----------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := vendor.qti.hardware.limits@1.0-service
LOCAL_MODULE_TAGS := optional
LOCAL_INIT_RC := vendor.qti.hardware.limits@1.0-service.rc
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_CFLAGS := -Wno-unused-parameter

LOCAL_SRC_FILES := \
    service.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libutils \
    libbase \
    libhwbinder \
    libhidlbase \
    libhidltransport \
    vendor.qti.hardware.limits@1.0_vendor \
    android.hardware.thermal@1.0

include $(BUILD_EXECUTABLE)
