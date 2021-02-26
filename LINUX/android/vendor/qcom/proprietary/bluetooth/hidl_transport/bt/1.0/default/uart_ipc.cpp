/*==========================================================================
Description
  It has implementation for IPC logging mechanism.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#define LOG_TAG "vendor.qti.bluetooth@1.0-uart_ipc"

#include <fcntl.h>
#include <errno.h>
#include <utils/Log.h>
#include "logger.h"
#include <sys/types.h>
#include <dirent.h>

#define UART_IPC_LOG_COLLECTION_OLD_PATH  "/sys/kernel/debug/ipc_logging/msm_serial_hs/log"
#define UART_IPC_LOG_COLLECTION_DIR  "/sys/kernel/debug/ipc_logging/"

#define UART_TX_LOG_COLLECTION_PATH        "/sys/kernel/debug/msm_serial_hs/tx_data.0"
#define UART_RX_LOG_COLLECTION_PATH        "/sys/kernel/debug/msm_serial_hs/rx_data.0"

#define UART_IPC_LOG_TX_FILE_NAME    "uart_tx/log"
#define UART_IPC_LOG_RX_FILE_NAME    "uart_rx/log"
#define UART_IPC_LOG_PWR_FILE_NAME    "uart_pwr/log"
#define UART_IPC_LOG_STATE_FILE_NAME    "uart_state/log"
#define UART_IPC_LOG_STATE_FILE_NAME_845    "uart_misc/log"

#define BT_SOC_CHEROKEE 1

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef DUMP_IPC_LOG

void IpcLogs :: DumpUartLogs()
{
  int index, fd;
  bool ret;
  char dpath[UART_IPC_PATH_BUF_SIZE];
  char spath[UART_IPC_PATH_BUF_SIZE];
  char ipc_log_dir[UART_IPC_PATH_BUF_SIZE];
  const char *TX_path = "uart_tx";
  struct DIR* p_dir;
  struct dirent* p_dirent;

  ALOGD("%s: -->", __func__);

  logger_ = static_cast<void*>(Logger::Get());

  static_cast<Logger*>(logger_)->GetUartDumpFilename(STATE_LOG,dpath);

  strlcpy(ipc_log_dir, UART_IPC_LOG_COLLECTION_DIR, sizeof(ipc_log_dir));

  /* Finding the UART IPC log source location */
  p_dir = opendir(UART_IPC_LOG_COLLECTION_DIR);
  if (p_dir == NULL) {
    ALOGE("Unable to open the Dir %s", UART_IPC_LOG_COLLECTION_DIR);
    return;
  }

  while ((p_dirent = readdir(p_dir)) != NULL) {
    char *end;
    if ( end = strstr(p_dirent->d_name, TX_path) ) {
      *end = '\0';
      strlcat(ipc_log_dir, p_dirent->d_name, sizeof(ipc_log_dir));
      break;
    }
  }
  closedir(p_dir);

  if(p_dirent == NULL) {
    ALOGE("%s: Did not find UART IPC source log files", __func__);
    return;
  }

  strlcpy(spath, ipc_log_dir, sizeof(spath));
  ALOGI("%s: found IPC source log location: %s", __func__, spath);
  strlcat(spath, UART_IPC_LOG_STATE_FILE_NAME, sizeof(spath));

  if ((fd = open(spath, O_RDONLY | O_NONBLOCK)) < 0) {
      strlcpy(spath, ipc_log_dir, sizeof(spath));
      strlcat(spath, UART_IPC_LOG_STATE_FILE_NAME_845, sizeof(spath));
      fd = open(spath, O_RDONLY | O_NONBLOCK);
  }

  if (fd >= 0) {
    close(fd);
    ret = DumpIpcLogs(spath, dpath, 1024 * 30, 1024 * 12);

    static_cast<Logger*>(logger_)->GetUartDumpFilename(RX_LOG,dpath);
    strlcpy(spath, ipc_log_dir, sizeof(spath));
    strlcat(spath, UART_IPC_LOG_RX_FILE_NAME, sizeof(spath));
    ret = DumpIpcLogs(spath, dpath, 1024 * 30, 1024 * 12);

    static_cast<Logger*>(logger_)->GetUartDumpFilename(TX_LOG,dpath);
    strlcpy(spath, ipc_log_dir, sizeof(spath));
    strlcat(spath, UART_IPC_LOG_TX_FILE_NAME, sizeof(spath));
    ret = DumpIpcLogs(spath, dpath, 1024 * 30, 1024 * 8);

    static_cast<Logger*>(logger_)->GetUartDumpFilename(POWER_LOG,dpath);
    strlcpy(spath, ipc_log_dir, sizeof(spath));
    strlcat(spath, UART_IPC_LOG_PWR_FILE_NAME, sizeof(spath));
    ret = DumpIpcLogs(spath, dpath, 1024 * 30, 1024 * 8);
  } else {
    ALOGE("%s: Error opening source file (%s) error (%s)", __func__, spath, strerror(errno) );
    DumpIpcLogs(UART_IPC_LOG_COLLECTION_OLD_PATH, dpath, 1024 * 50, 1024 * 22);
  }

  static_cast<Logger*>(logger_)->GetUartDumpFilename(TX_LOGS_DEST,dpath);
  DumpIpcLogs(UART_TX_LOG_COLLECTION_PATH, dpath, 4000, 4000);

  static_cast<Logger*>(logger_)->GetUartDumpFilename(RX_LOGS_DEST,dpath);
  DumpIpcLogs(UART_RX_LOG_COLLECTION_PATH, dpath, 4000, 4000);
}

bool IpcLogs :: DumpIpcLogs(const char *spath, const char *dpath, int buff_size, int log_limit)
{
  int src_fd, dest_fd, logs_written = 0;
  int ret;
  char *buff;

  ALOGV("%s: dump to %s", __func__, dpath);

  buff = new (std::nothrow)char[buff_size];
  if (buff == NULL) {
    ALOGE("%s: Error allocating memory %d bytes for log buffer", __func__, buff_size);
    return false;
  }

  src_fd = open(spath, O_RDONLY | O_NONBLOCK);
  if (src_fd < 0) {
    ALOGE("%s: Error opening source file (%s) error (%s)", __func__, spath, strerror(errno) );
    delete[] buff;
    return false;
  }

  dest_fd = open(dpath, O_CREAT | O_SYNC | O_WRONLY,  S_IRUSR | S_IWUSR | S_IRGRP );
  if (dest_fd < 0) {
    ALOGE("%s: Error opening destination file: %s (%s)", __func__, dpath, strerror(errno) );
    delete[] buff;
    close(src_fd);
    return false;
  }

  do {
    ret = read(src_fd, buff, buff_size);
    if (ret <= 0) {
      ALOGE("%s: Finish reading src file: %d (%s)", __func__, ret, strerror(errno) );
      break;
    }

    ret = write(dest_fd, buff, ret);
    if (ret <= 0) {
      ALOGE("%s: Error writing to dest file: %d (%s)", __func__, ret, strerror(errno) );
      break;
    }

    ALOGE("%s: Read/Written size = %d", __func__, ret);

    logs_written += ret;
    if (logs_written >= log_limit) {
      ALOGE("%s: Have pulled enough UART IPC logs", __func__);
      break;
    }
  } while (1);

  delete[] buff;
  close(src_fd);
  fsync(dest_fd);
  close(dest_fd);

  return true;
}

#endif

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
