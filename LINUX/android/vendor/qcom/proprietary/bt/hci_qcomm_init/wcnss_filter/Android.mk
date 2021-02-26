LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Logging Features. Enable only one at any time
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_SYSLOG
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -DDIAG_ENABLED

LOCAL_CFLAGS += -DBT_NV_SUPPORT

# Additional Logging
# Uncomment for VERBOSE level messages
#LOCAL_CFLAGS += -DLOG_NDEBUG=0

# Additional Logging
# Uncomment for DEBUG level messages
#LOCAL_CFLAGS += -DLOG_NDDEBUG=0

# Additional Logging
# Uncomment for INFO level messages
#LOCAL_CFLAGS += -DLOG_NIDEBUG=0

# Ignore HCI_RESET in filter code
# so that HCI_RESET happens only once
# regardless of the client
#LOCAL_CFLAGS += -DIGNORE_HCI_RESET

# MIMIC cmd TOUT at filter
# used to check the behavior of the system
# by mimicing the cmd tout at host
# this is only for debug purpose
# DON'T ENABLE IT FOR NORMAL BT OPERATIONS
#LOCAL_CFLAGS += -DDEBUG_MIMIC_CMD_TOUT

#LOCAL_CFLAGS += -DDEBUG_CRASH_SOC_WHILE_DLD

# disable below flag to disable IBS
# from host side
LOCAL_CFLAGS += -DWCNSS_IBS_ENABLED
# Disable this flag if WAKELOCK HOLDING
# from wcnss_filter is not needed
LOCAL_CFLAGS += -DWAKE_LOCK_ENABLED
#LOCAL_CFLAGS += -DREAD_BT_ADDR_FROM_NV

ifeq ($(UART_BAUDRATE_3_0_MBPS),true)
LOCAL_CFLAGS += -DUART_BAUDRATE_3_0_MBPS
endif

LOCAL_SRC_FILES:= \
           src/main.c \
           src/bt_qxdmlog.c \
           src/hw_cherokee.c \
           src/uart_utils.c \
           src/utils.c

#LOCAL_CFLAGS += -Werror -Wall -Wextra

LOCAL_SRC_FILES += src/wcnss_ibs.c

LOCAL_SRC_FILES += src/bt_vendor_persist.cpp

ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DENABLE_DBG_FLAGS
LOCAL_CFLAGS += -DPANIC_ON_SOC_CRASH
LOCAL_CFLAGS += -DDUMP_IPC_LOG
endif

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/bt/hci_qcomm_init
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_ADDITIONAL_DEPENDENCIES += \
$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libdiag libbtnv

LOCAL_MODULE:= wcnss_filter
LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
