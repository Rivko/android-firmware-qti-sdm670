LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO   := data/inc
LOCAL_COPY_HEADERS      := ../inc/dsi_netctrl.h
LOCAL_COPY_HEADERS      += ../inc/dsi_netctrl_qos.h

LOCAL_SRC_FILES += dsi_netctrl.c
LOCAL_SRC_FILES += dsi_netctrl_init.c
LOCAL_SRC_FILES += dsi_netctrli.c
LOCAL_SRC_FILES += dsi_netctrl_mni_cb.c
LOCAL_SRC_FILES += dsi_netctrl_mni.c
LOCAL_SRC_FILES += dsi_netctrl_multimodem.c
LOCAL_SRC_FILES += dsi_netctrl_netmgr.c
LOCAL_SRC_FILES += dsi_netctrl_platform.c
LOCAL_SRC_FILES += dsi_netctrl_cb_thrd.c
LOCAL_SRC_FILES += dsi_netctrl_qos.c
LOCAL_SRC_FILES += dsi_netctrl_embms.c
LOCAL_SRC_FILES += dsi_netctrl_qmi.c
LOCAL_SRC_FILES += dsi_netctrl_qmi_wds.c
LOCAL_SRC_FILES += dsi_netctrl_qmi_qos.c
LOCAL_SRC_FILES += dsi_config.c

#LOCAL_CFLAGS := -DFEATURE_DSI_TEST

LOCAL_CFLAGS := -DFEATURE_DS_LINUX_ANDROID

LOCAL_SHARED_LIBRARIES := libqmi
LOCAL_SHARED_LIBRARIES += libqdi
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libnetmgr
LOCAL_SHARED_LIBRARIES += libdiag
LOCAL_SHARED_LIBRARIES += libdsutils
LOCAL_SHARED_LIBRARIES += libconfigdb
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libqmi_client_helper

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qmi-framework/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qmi-framework/qcci/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qmi-framework/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../dsutils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../configdb/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qmi/src
#LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/oncrpc/inc/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../netmgr/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../qdi/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc

ifeq ($(call is-board-platform,msm7630_fusion),true)
LOCAL_CFLAGS += -DFEATURE_DSI_FUSION
endif
LOCAL_CFLAGS += -DFEATURE_QCRIL_USE_QDP
LOCAL_CFLAGS += -DFEATURE_DSI_MULTIMODEM_ROUTELOOKUP
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB

LOCAL_MODULE := libdsi_netctrl

LOCAL_MODULE_TAGS := optional

LOCAL_CLANG := true

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dsi_config.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/data
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

