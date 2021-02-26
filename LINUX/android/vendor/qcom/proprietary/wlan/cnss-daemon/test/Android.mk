LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cnss-test-server
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	$(TARGET_OUT_HEADERS)/qmi-framework/inc

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../

LOCAL_SRC_FILES := \
	cnss_test_server.c

LOCAL_SRC_FILES += ../wlan_firmware_service_v01.c

LOCAL_SHARED_LIBRARIES := \
	libqmi_csi \
	libcutils \
	libqmi_common_so

include $(BUILD_EXECUTABLE)
