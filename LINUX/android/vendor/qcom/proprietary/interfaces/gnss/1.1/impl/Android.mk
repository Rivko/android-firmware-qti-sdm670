ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifneq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.qti.gnss@1.1-impl
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    LocHidlAGnss.cpp \
    LocHidlDebugReportService.cpp \
    LocHidlFlpService.cpp \
    LocHidlGeofenceService.cpp \
    LocHidlIzatProvider.cpp \
    LocHidlGnssNi.cpp \
    LocHidlIzatOsNpGlue.cpp \
    LocHidlRilInfoMonitor.cpp \
    LocHidlIzatSubscription.cpp \
    LocHidlGnss.cpp \
    LocHidlXT.cpp \
    LocHidlWiFiDBReceiver.cpp \

LOCAL_SRC_FILES += \
    clients/LocHidlNiClient.cpp \
    clients/LocHidlFlpClient.cpp \
    clients/LocHidlGeofenceClient.cpp \
    clients/LocHidlUtils.cpp \

LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/clients \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/libxtadapter \
    $(TARGET_OUT_HEADERS)/libizat_core \
    $(TARGET_OUT_HEADERS)/libflp \
    $(TARGET_OUT_HEADERS)/liblbs_core \
    $(TARGET_OUT_HEADERS)/liblocationservice/lcp/inc \
    $(TARGET_OUT_HEADERS)/liblocationservice_glue \
    $(TARGET_OUT_HEADERS)/liblocationservice \
    $(TARGET_OUT_HEADERS)/libdataitems \
    $(TARGET_OUT_HEADERS)/liblocationservice_jni

LOCAL_HEADER_LIBRARIES := \
    izat_remote_api_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libhidlbase \
    libhidltransport \
    libhwbinder \
    libutils \
    vendor.qti.gnss@1.0_vendor \
    vendor.qti.gnss@1.1_vendor \
    android.hardware.gnss@1.0 \
    android.hidl.base@1.0

LOCAL_SHARED_LIBRARIES += \
    libloc_core \
    libgps.utils \
    libdl \
    liblocation_api \
    libflp \
    liblocationservice_glue \
    liblbs_core \
    libdataitems \
    liblocationservice \
    libxtadapter \
    libizat_core \
    libizat_client_api \

include $(BUILD_SHARED_LIBRARY)

endif # TARGET_SUPPORTS_ANDROID_WEAR
endif # BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET
endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
