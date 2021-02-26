LOCAL_PATH:= $(call my-dir)
ifeq ($(TARGET_HAS_SPLIT_A2DP_FEATURE),true)
include $(CLEAR_VARS)
LOCAL_MODULE := com.qualcomm.qti.bluetooth_audio@1.0-impl
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    BluetoothAudio.cpp

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidltransport \
    libutils \
    liblog \
    libhwbinder \
    libbase \
    libcutils \
    libhardware \
    android.hidl.base@1.0 \
    com.qualcomm.qti.bluetooth_audio@1.0_vendor \

include $(BUILD_SHARED_LIBRARY)
endif # TARGET_HAS_SPLIT_A2DP_FEATURE
