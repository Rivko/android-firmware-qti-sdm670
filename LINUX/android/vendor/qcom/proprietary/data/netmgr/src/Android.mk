# sources and intermediate files are separated

LOCAL_PATH:= $(call my-dir)

# Load bulid files for all sub-modules
include $(call all-subdir-makefiles)
include $(CLEAR_VARS)

################################################################################

include $(CLEAR_VARS)
include $(LOCAL_PATH)/Android.min
LOCAL_ADDITIONAL_DEPENDENCIES += $(LOCAL_PATH)/Android.min

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_SHARED_LIBRARIES += libhardware_legacy

LOCAL_SRC_FILES := \
	netmgr_client.c \
	netmgr_netlink.c \
	netmgr_unix.c \
	netmgr_util.c

LOCAL_MODULE := libnetmgr
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

################################################################################

include $(CLEAR_VARS)
include $(LOCAL_PATH)/Android.min
LOCAL_ADDITIONAL_DEPENDENCIES += $(LOCAL_PATH)/Android.min
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/dataservices/rmnetctl
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/cne/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../qmi/src
LOCAL_C_INCLUDES += system/core/libnetutils/include/netutils

LOCAL_CFLAGS += -DNETMGR_USE_RMNET_DATA

ifeq ($(PLATFORM_SDK_VERSION), 19)
LOCAL_CFLAGS += -DFEATURE_DATA_KIT_KAT
endif

ifneq ($(TARGET_BUILD_VARIANT),user)
   LOCAL_CFLAGS += -DNETMGR_LOG_TO_FILE
endif

LOCAL_SRC_FILES := netmgr.c
LOCAL_SRC_FILES += netmgr_exec.c
LOCAL_SRC_FILES += netmgr_kif.c
LOCAL_SRC_FILES += netmgr_qmi_dpm.c
LOCAL_SRC_FILES += netmgr_main.c
LOCAL_SRC_FILES += netmgr_platform.c
LOCAL_SRC_FILES += netmgr_qmi.c
LOCAL_SRC_FILES += netmgr_sm.c
LOCAL_SRC_FILES += netmgr_sm_int.c
LOCAL_SRC_FILES += netmgr_tc.c
LOCAL_SRC_FILES += netmgr_test.c
LOCAL_SRC_FILES += netmgr_rmnet.c
LOCAL_SRC_FILES += netmgr_qmi_wda.c
LOCAL_SRC_FILES += netmgr_qmi_dfs.c
LOCAL_SRC_FILES += netmgr_config.c
LOCAL_SRC_FILES += netmgr_iwlan_client.cpp
LOCAL_SRC_FILES += netmgr_netd_client.cpp
LOCAL_SRC_FILES += netmgr_cmdq.c
LOCAL_SRC_FILES += netmgr_qmi_wds.c
LOCAL_SRC_FILES += netmgr_qmi_dsd.c
LOCAL_SRC_FILES += netmgr_qmi_qos.c
LOCAL_SRC_FILES += netmgr_cb.c
LOCAL_SRC_FILES += plugins/link_local/nm_link_local_main.c
LOCAL_SRC_FILES += plugins/link_local/nm_link_local_route_provider_factory.c
LOCAL_SRC_FILES += plugins/link_local/nm_link_local_ndc_route_provider.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_iptables_helper.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_common.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_mark_provider_factory.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_legacy_mark_provider.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_socket_mark_provider.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_mark_provider_common.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_route_provider_factory.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_legacy_route_provider.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_ndc_route_provider.c
LOCAL_SRC_FILES += plugins/modem_proxy/nm_mdmprxy_main.c

LOCAL_MODULE := netmgrd
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += libnetutils
LOCAL_SHARED_LIBRARIES += librmnetctl
LOCAL_SHARED_LIBRARIES += libnetmgr
LOCAL_SHARED_LIBRARIES += libqmi
LOCAL_SHARED_LIBRARIES += libqmiservices
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_SHARED_LIBRARIES += libqmi_client_qmux
LOCAL_SHARED_LIBRARIES += libqmi_client_helper
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libhidlbase
LOCAL_SHARED_LIBRARIES += libhidltransport
LOCAL_SHARED_LIBRARIES += android.system.net.netd@1.0

ifeq ($(BOARD_USES_QCNE),true)
LOCAL_SHARED_LIBRARIES += libcneapiclient
LOCAL_SHARED_LIBRARIES += libbinder
endif

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := netmgr_config.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_ETC)/data
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_OWNER := qti
include $(BUILD_PREBUILT)

