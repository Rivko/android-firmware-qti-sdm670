LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libgarden_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH) \
    $(LOCAL_PATH)/gnss_case \
    $(LOCAL_PATH)/gnss_multiclient_case \
    $(LOCAL_PATH)/flp_case \
    $(LOCAL_PATH)/geofence_case \
    $(LOCAL_PATH)/locationapi_api \
    $(LOCAL_PATH)/android

include $(BUILD_HEADER_LIBRARY)

ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
    LOCAL_CFLAGS := -DLOCHIDL
else ifeq ($(LW_FEATURE_SET),true)
    LOCAL_CFLAGS := -DLOCHIDL
endif

# Build libgarden
include $(CLEAR_VARS)

LOCAL_MODULE := libgarden
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES += \
    GardenFramework.cpp \
    GardenUtil.cpp \
    GnssCbBase.cpp \
    gnss_case/GnssCase.cpp \
    gnss_case/GnssCb.cpp \
    gnss_multiclient_case/GnssMultiClientCase.cpp \
    gnss_multiclient_case/GnssMultiClientCb.cpp \
    gnss_multiclient_case/GnssMultiClientLocationAPIClient.cpp \
    gnss_multiclient_case/GnssMultiClientCaseUtils.cpp \
    geofence_case/GeofenceCb.cpp \
    geofence_case/GeofenceCase.cpp \
    flp_case/FlpCase.cpp \
    flp_case/FlpCb.cpp \
    locationapi_api/GnssLocationAPI.cpp \
    locationapi_api/GnssLocationAPIClient.cpp \
    locationapi_api/FlpLocationAPIClient.cpp \
    locationapi_api/GeofenceLocationAPIClient.cpp \
    PowerEvtHandler.cpp \
    android/HidlAPI.cpp \

LOCAL_CFLAGS += \
    -DDEBUG \
    -D_ANDROID_ \
    -fno-short-enums \
    -fexceptions

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocationservice_headers \
    libevent_observer_headers \
    liblocation_api_headers

LOCAL_SHARED_LIBRARIES := \
    libdl \
    libgps.utils \
    libloc_core \
    liblocation_api \
    liblog \
    libutils \
    android.hardware.gnss@1.0 \
    libevent_observer \
    vendor.qti.gnss@1.0 \
    libhidlbase \
    libhidltransport

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


# Build garden_app
include $(CLEAR_VARS)
LOCAL_MODULE := garden_app
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += \
    $(GNSS_CFLAGS)

LOCAL_SRC_FILES := \
    main.cpp

LOCAL_HEADER_LIBRARIES := \
    libgarden_headers \
    libgps.utils_headers \
    libloc_pla_headers \
    liblocation_api_headers

LOCAL_SHARED_LIBRARIES := \
    libgarden \
    libdl \
    libgps.utils \
    libloc_core \
    liblog

include $(BUILD_EXECUTABLE)

include $(LOCAL_PATH)/*/Android.mk
