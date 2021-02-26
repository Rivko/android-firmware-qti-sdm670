LOCAL_PATH:= $(call my-dir)

# ------------------------------------------------------------------------------
#       Make the network performance tuner HAL service
# ------------------------------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/hidl
LOCAL_CFLAGS := -Wno-unused-parameter
LOCAL_SRC_FILES := \
    wigignpt.cpp \
    CommandListener.cpp \
    NetworkPerformanceTuner.cpp \
    hidl/NetPerfTuner.cpp
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libutils \
    libsysutils \
    libbase \
    libhwbinder \
    libhidlbase \
    libhidltransport \
    vendor.qti.hardware.wigig.netperftuner@1.0
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := wigignpt

include $(BUILD_EXECUTABLE)
