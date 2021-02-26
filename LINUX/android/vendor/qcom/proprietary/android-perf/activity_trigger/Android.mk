LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    qti-activity_trigger.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libutils \
    liblog \
    libxml2 \
    libqti-util_system \
    libqti-perfd-client_system

LOCAL_CFLAGS += \
    -Wall \
    -DWHITE_LISTS_PATH=\"/system/etc\" \
    -DQC_DEBUG=0

LOCAL_C_INCLUDES := external/connectivity/stlport/stlport \
                    external/libxml2/include \
                    external/icu/icu4c/source/common \
                    vendor/qcom/proprietary/android-perf/mp-ctl \
                    vendor/qcom/proprietary/android-perf/perf-util

LOCAL_MODULE := libqti-at
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)
