#/******************************************************************************
#*@file Android.mk
#* brief Rules to create an package which exposes the location api
#*******************************************************************************/
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= izat.lib.glue

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_SRC_FILES += \
    java/com/qti/izat/IIzatService.aidl \
    java/com/qti/flp/ILocationCallback.aidl \
    java/com/qti/flp/IMaxPowerAllocatedCallback.aidl \
    java/com/qti/flp/IFlpService.aidl \
    java/com/qti/flp/ISessionStatusCallback.aidl \
    java/com/qti/flp/ITestService.aidl \
    java/com/qti/geofence/IGeofenceService.aidl \
    java/com/qti/geofence/IGeofenceCallback.aidl \
    java/com/qti/debugreport/IDebugReportService.aidl \
    java/com/qti/debugreport/IDebugReportCallback.aidl \
    java/com/qti/wifidbreceiver/IWiFiDBReceiver.aidl \
    java/com/qti/wifidbreceiver/IWiFiDBReceiverResponseListener.aidl \
    java/com/qti/wwandbreceiver/IWWANDBReceiver.aidl \
    java/com/qti/wwandbreceiver/IWWANDBReceiverResponseListener.aidl

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_MODULE_OWNER := qti

include $(BUILD_STATIC_JAVA_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
