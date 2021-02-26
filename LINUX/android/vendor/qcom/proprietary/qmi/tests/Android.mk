LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        qmi_client_utils.c \
        qmi_simple_ril_core.c    \
        qmi_simple_ril_voice.c    \
	qmi_simple_ril_misc.c \
	qmi_simple_ril_nas.c \
	qmi_simple_ril_sms.c \
	qmi_simple_ril_ss.c \
        qmi_simple_ril_suite.c \
        qmi_test_console.c    \
        fota.c    \
        fota_cdma.c    \
        wmsts/wmsts.c    \
        wmsts/wmstscdma.c    \
        wmsts/bit.c    \

LOCAL_SHARED_LIBRARIES := \
        libqmi_cci \
        libqmi \
        libqmi_client_qmux \
	libqcci_legacy \
        libidl \
        libqmiservices \
        libcutils \
        liblog

LOCAL_CFLAGS := -DFEATURE_DSS_LINUX_ANDROID
LOCAL_CFLAGS += -DFEATURE_QMI_ANDROID
LOCAL_CFLAGS += -Wno-format -Wno-format-extra-args -Wno-unused-parameter -Wno-sign-compare
LOCAL_CFLAGS += -Wno-incompatible-pointer-types -Wno-pointer-sign -Wno-macro-redefined

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/wmsts
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi-framework/inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../core/lib/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../platform
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc

LOCAL_CLANG := true

LOCAL_MODULE:= qmi_simple_ril_test
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional debug
include $(BUILD_EXECUTABLE)
