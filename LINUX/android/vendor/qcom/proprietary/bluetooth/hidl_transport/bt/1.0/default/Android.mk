LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.bluetooth@1.0-impl-qti
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

ifeq ($(UART_BAUDRATE_3_0_MBPS),true)
LOCAL_CPPFLAGS := -DUART_BAUDRATE_3_0_MBPS
endif

LOCAL_SRC_FILES := \
    bluetooth_hci.cpp \
    bluetooth_address.cpp \
    async_fd_watcher.cpp \
    hci_packetizer.cpp \
    data_handler.cpp \
    uart_controller.cpp \
    patch_dl_manager.cpp \
    hci_uart_transport.cpp \
    power_manager.cpp \
    ibs_handler.cpp \
    logger.cpp \
    uart_ipc.cpp \
    ring_buffer.cpp \
    wake_lock.cpp \
    diag_interface.cpp \
    mct_controller.cpp \
    hci_mct_transport.cpp \
    nvm_tags_manager.cpp

LOCAL_CFLAGS += -DDIAG_ENABLED
# disable below flag to disable IBS
LOCAL_CFLAGS += -DWCNSS_IBS_ENABLED
# Disable this flag for disabling wakelocks
LOCAL_CFLAGS += -DWAKE_LOCK_ENABLED

ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DDUMP_IPC_LOG -DDUMP_RINGBUF_LOG
LOCAL_CFLAGS += -DENABLE_FW_CRASH_DUMP -DUSER_DEBUG
endif

LOCAL_C_INCLUDES += vendor/qcom/proprietary/bt/hci_qcomm_init
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi/platform
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qmi-framework/inc
LOCAL_C_INCLUDES += vendor/qcom/proprietary/qcril-qmi-services/
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    libhidlbase \
    libhidltransport \
    liblog \
    libutils \
    libbtnv \
    libdiag \
    libqmiservices \
    libqmi_cci \
    android.hardware.bluetooth@1.0

include $(BUILD_SHARED_LIBRARY)

#service

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := android.hardware.bluetooth@1.0-service-qti
LOCAL_INIT_RC := android.hardware.bluetooth@1.0-service-qti.rc
LOCAL_SRC_FILES := \
  service.cpp

LOCAL_SHARED_LIBRARIES := \
  liblog \
  libcutils \
  libutils \
  libhwbinder \
  libhidlbase \
  libhidltransport \
  android.hardware.bluetooth@1.0 \

ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_SHARED_LIBRARIES += vendor.qti.hardware.fm@1.0_vendor
LOCAL_CPPFLAGS += -DQCOM_FM_SUPPORTED
endif #BOARD_HAVE_QCOM_FM

ifneq ($(BOARD_ANT_WIRELESS_DEVICE),)
LOCAL_SHARED_LIBRARIES += com.qualcomm.qti.ant@1.0_vendor
LOCAL_CPPFLAGS += -DQCOM_ANT_SUPPORTED
endif

include $(BUILD_EXECUTABLE)
