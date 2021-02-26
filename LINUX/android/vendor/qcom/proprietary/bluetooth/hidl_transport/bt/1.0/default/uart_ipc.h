/*==========================================================================
Description
  It has implementation for IPC logging mechanism.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#pragma once

#include <hidl/HidlSupport.h>

#define UART_IPC_TX_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_ipc_txlogs.txt"
#define UART_IPC_RX_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_ipc_rxlogs.txt"
#define UART_IPC_STATE_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_ipc_statelogs.txt"
#define UART_IPC_PWR_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_ipc_pwrlogs.txt"

#define UART_UART_TX_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_txlogs.txt"
#define UART_UART_RX_LOGS_DEST_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_rxlogs.txt"

#define UART_IPC_PATH_BUF_SIZE   (255)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef DUMP_IPC_LOG

class IpcLogs
{
 private:
  bool DumpIpcLogs(const char *, const char *, int, int);
  void *logger_;

 public:
  enum uart_crash_type{
    TX_LOG,
    RX_LOG,
    STATE_LOG,
    POWER_LOG,
    TX_LOGS_DEST,
    RX_LOGS_DEST,
  };
  void DumpUartLogs();
  IpcLogs() {};
};

#endif

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
