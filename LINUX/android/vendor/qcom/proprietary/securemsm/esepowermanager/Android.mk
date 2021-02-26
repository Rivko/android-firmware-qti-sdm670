LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.esepowermanager@1.0-impl
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    EsePowerManager.cpp

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc


LOCAL_SHARED_LIBRARIES := \
    libbase \
    libhwbinder \
    libcutils \
    libhidlbase \
    libhidltransport \
    liblog \
    libutils \
    vendor.qti.esepowermanager@1.0_vendor \
    android.hidl.base@1.0

include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)

LOCAL_MODULE := esepmhidlclient
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    EsePMClient.cpp

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc


LOCAL_SHARED_LIBRARIES := \
        libhidlbase \
        libhidltransport \
        liblog \
        libutils \
        vendor.qti.esepowermanager@1.0_vendor

include $(BUILD_EXECUTABLE)
include $(CLEAR_VARS)

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_VENDOR_MODULE := true

LOCAL_MODULE := vendor.qti.esepowermanager@1.0-service
LOCAL_INIT_RC := vendor.qti.esepowermanager@1.0-service.rc
LOCAL_SRC_FILES := \
        service.cpp \

LOCAL_C_INCLUDES := $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \

LOCAL_SHARED_LIBRARIES := \
       libcutils \
       libdl \
       libbase \
       libutils \
       libhardware_legacy \
       libhardware \
       libQSEEComAPI \
       liblog


LOCAL_SHARED_LIBRARIES += \
       libhidlbase \
       libhidltransport \
       vendor.qti.esepowermanager@1.0_vendor

include $(BUILD_EXECUTABLE)
