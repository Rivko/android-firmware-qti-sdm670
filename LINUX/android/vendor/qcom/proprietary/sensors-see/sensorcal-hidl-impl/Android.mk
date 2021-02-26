LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.hardware.sensorscalibrate@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_OWNER := qti
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/sensors/inc
LOCAL_SRC_FILES := \
    SensorsCalibrate.cpp \

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidltransport \
    libutils \
    libdl \
    liblog \
    libcutils \
    libhardware \
    libbase \
    vendor.qti.hardware.sensorscalibrate@1.0 \

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE := vendor.qti.hardware.sensorscalibrate@1.0-service
LOCAL_INIT_RC := vendor.qti.hardware.sensorscalibrate@1.0-service.rc
LOCAL_SRC_FILES := \
        service.cpp \

LOCAL_SHARED_LIBRARIES := \
        liblog \
        libcutils \
        libdl \
        libbase \
        libutils \

LOCAL_SHARED_LIBRARIES += \
        libhidlbase \
        libhidltransport \
        vendor.qti.hardware.sensorscalibrate@1.0 \

include $(BUILD_EXECUTABLE)

# =======================================================
# Install the Library
# =======================================================
include $(CLEAR_VARS)

LOCAL_MODULE := vendor-qti-hardware-sensorscalibrate.xml
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))
