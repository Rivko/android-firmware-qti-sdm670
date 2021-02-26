/*==========================================================================
Description
  It has implementation for logger class

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <hidl/HidlSupport.h>
#include <fcntl.h>
#include <errno.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include "logger.h"
#include "uart_controller.h"

#define LOG_TAG "vendor.qti.bluetooth@1.0-logger"

#define SOC_DUMP_FIRST_PKT 0x0000
#define SOC_DUMP_LAST_PKT 0xFFFF

#define MAX_CRASH_DUMP_COUNT  10
#define SOC_DUMP_PATH "/data/vendor/ramdump/bluetooth/%.02d-bt_fw_crashdump.bin"
#define SOC_DUMP_PATH_BUF_SIZE 60
#define SOC_DUMP_SIGNATURE_SIZE 24

#define IPC_DUMP_PATH "/data/vendor/ramdump/bluetooth/%.02d-uart_ipc_txlogs.txt"
#define IPC_DUMP_PATH_BUF_SIZE 80

#define BT_VND_SPECIFIC_EVENT 0xFF

#define BT_CONTROLLER_LOG        0x01
#define LOG_BT_MESSAGE_TYPE_VSTR     0x02
#define LOG_BT_MESSAGE_TYPE_PACKET   0x05
#define LOG_BT_MESSAGE_TYPE_MEM_DUMP 0x08
#define DIAG_SSR_BT_CMD     0x0007

#define LAST_SEQUENCE_NUM 0xFFFF

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

Logger * Logger :: instance_ = NULL;

Logger * Logger :: Get()
{
  if (!instance_) {
    instance_ = new Logger();
  }
  return instance_;
}

Logger :: Logger()
{
  /*To me initiated dynamically by config input*/
  ibs_log_level_ = MID;
  rx_log_level_ = MID;
  tx_log_level_ = MID;
  gen_log_level_ = MID;
  is_crash_dump_in_progress_ = false;
  crash_dump_status_cb_ = nullptr;
}

bool Logger :: IsSnoopLogEnabled(void)
{
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  property_get("persist.service.bdroid.snooplog", value, "false");
  return (strcmp(value, "true") == 0);
}

bool Logger :: IsHciFwSnoopEnabled(void)
{
#ifdef USER_DEBUG
  return true;
#else
  char value[PROPERTY_VALUE_MAX] = {'\0'};
  property_get("persist.service.bdroid.fwsnoop", value, "false");
  return (strcmp(value, "true") == 0);
#endif
}

bool Logger::IsControllerLogPkt(HciPacketType type, const uint8_t *buff, uint16_t len)
{
  if (len < 3) {
    return false;
  } else if ((HCI_PACKET_TYPE_EVENT == type) &&
             (BT_VND_SPECIFIC_EVENT == buff[0]) &&
             (BT_CONTROLLER_LOG == buff[2])) {
    return true;
  } else {
    return false;
  }
}

bool Logger :: IsCrashDumpStarted(void)
{
  return is_crash_dump_in_progress_;
}

bool Logger:: RegisterCrashDumpCallback(CrashDumpStatusCallback crash_dump_cb)
{
  crash_dump_status_cb_ = crash_dump_cb;
  return true;
}


bool Logger :: ProcessRx(HciPacketType type, const uint8_t *buff, uint16_t len)
{
  bool ret  = true;

  switch (type) {
    case HCI_PACKET_TYPE_ACL_DATA:
#ifdef DUMP_RINGBUF_LOG
      ring_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, (uint8_t*)buff, len);
#endif
      if (IsSnoopLogEnabled()) {
        diag_interface_.SendLogs(buff, len, LOG_BT_HCI_RX_ACL_C);
      }
      break;

    case HCI_PACKET_TYPE_EVENT:
      ret =  HandleHciEvent(type, (uint8_t*)buff, len);
      if (IsSnoopLogEnabled()) {
        diag_interface_.SendLogs(buff, len, LOG_BT_HCI_EV_C);
      }
      break;

    default:
      ret =  false;
      break;
  }
  return ret;
}

bool Logger :: ProcessTx(HciPacketType type, const uint8_t *buff, uint16_t len)
{
#ifdef DUMP_RINGBUF_LOG
  ring_buff_.AddLog(RingBuffer::HOST_TO_SOC, type, (uint8_t*)buff, len);
#endif

  if (IsSnoopLogEnabled()) {
    if (type == HCI_PACKET_TYPE_COMMAND)
      diag_interface_.SendLogs(buff, len, LOG_BT_HCI_CMD_C);
    else if (type == HCI_PACKET_TYPE_ACL_DATA)
      diag_interface_.SendLogs(buff, len, LOG_BT_HCI_TX_ACL_C);
  }
  return true;
}

bool Logger :: HandleHciEvent(HciPacketType type, uint8_t *buff, uint16_t len)
{
  bool ret = true;

  if ((buff[0] == BT_VND_SPECIFIC_EVENT) &&
      (buff[2] == BT_CONTROLLER_LOG)) {
    switch (buff[3]) {
      case LOG_BT_MESSAGE_TYPE_VSTR:
      case LOG_BT_MESSAGE_TYPE_PACKET:
#ifdef DUMP_RINGBUF_LOG
        ring_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
        break;

      case LOG_BT_MESSAGE_TYPE_MEM_DUMP:
        SaveSocMemDump(buff, len);
        break;

      default:
        ret = false;
        break;
    }
  } else {
#ifdef DUMP_RINGBUF_LOG
    ring_buff_.AddLog(RingBuffer::SOC_TO_HOST, type, buff, len);
#endif
  }
  return ret;
}

const diagpkt_user_table_entry_type Logger :: ssr_bt_tbl_[] = {
  { DIAG_SSR_BT_CMD, DIAG_SSR_BT_CMD, SsrBtHandler },
};

void * Logger :: SsrBtHandler(void *req_pkt, uint16_t pkt_len)
{
  Logger *logger = Logger :: Get();

  if (logger) {
    return logger->diag_interface_.SsrBtHandler(req_pkt, pkt_len);
  } else {
    return nullptr;
  }
}

void Logger :: SaveSocMemDump(uint8_t *eventBuf, uint16_t packet_len)
{
  static unsigned int dump_size = 0, total_size = 0;
  unsigned short seq_num = 0;
  static unsigned short seq_num_cnt = 0;
  uint8_t *dump_ptr = NULL;
  static char *temp_buf, *p, path[SOC_DUMP_PATH_BUF_SIZE + 1];
  static int dump_fd = -1;
  char nullBuff[255] = { 0 };

  dump_ptr = &eventBuf[7];
  seq_num = eventBuf[4] | (eventBuf[5] << 8);
  packet_len -= 7;

  ALOGV("%s: LOG_BT_MESSAGE_TYPE_MEM_DUMP (%d) ", __func__, seq_num);

  if ((seq_num != seq_num_cnt) && seq_num != LAST_SEQUENCE_NUM) {
    ALOGE("%s: current sequence number: %d, expected seq num: %d ", __func__,
          seq_num, seq_num_cnt);
  }

  if (seq_num == 0x0000) {
#ifdef DUMP_RINGBUF_LOG
    ring_buff_.DumpData();
#endif

#ifdef DUMP_IPC_LOG
    ipc_log_.DumpUartLogs();
#endif

    dump_size = (unsigned int)
                (eventBuf[7] | (eventBuf[8] << 8) | (eventBuf[9] << 16) | (eventBuf[10] << 24));
    dump_ptr = &eventBuf[11];
    total_size = seq_num_cnt = 0;
    packet_len -= 4;

    //memset(path, 0, SOC_DUMP_PATH_BUF_SIZE);
    /* first pack has total ram dump size (4 bytes) */
    ALOGD("%s: Crash Dump Start - total Size: %d ", __func__, dump_size);
    is_crash_dump_in_progress_ = true;
    p = temp_buf = (char*)malloc(dump_size);
    if (p != NULL) {
      memset(p, 0, dump_size);
    } else {
      ALOGE("Failed to allocate mem for the crash dump size: %d", dump_size);
    }

    GetCrashDumpFileName(path);
    dump_fd = open(path, O_CREAT | O_SYNC | O_WRONLY,  S_IRUSR | S_IWUSR | S_IRGRP );
    if (dump_fd < 0) {
      ALOGE("%s: File open (%s) failed: errno: %d", __func__, path, errno);
      seq_num_cnt++;
      return;
    }
    ALOGV("%s: File Open (%s) successfully ", __func__, path);
  }

  if (dump_fd >= 0) {
    for (; (seq_num > seq_num_cnt) && (seq_num != LAST_SEQUENCE_NUM); seq_num_cnt++) {
      ALOGE("%s: controller missed packet : %d, write null (%d) into file ",
            __func__, seq_num_cnt, packet_len);

      if (p != NULL) {
        memcpy(temp_buf, nullBuff, packet_len);
        temp_buf = temp_buf + packet_len;
      }
    }

    if (p != NULL) {
      memcpy(temp_buf, dump_ptr, packet_len);
      temp_buf = temp_buf + packet_len;
    }
    total_size += packet_len;
  }

  seq_num_cnt++;
  if (seq_num == LAST_SEQUENCE_NUM && p != NULL) {
    ALOGE("Writing crash dump of size %d bytes", total_size);
    write(dump_fd, p, total_size);
    free(p);
    temp_buf = NULL;
    p = NULL;
    seq_num_cnt = 0;
  }

 skip_crashdump:
  if (seq_num == LAST_SEQUENCE_NUM) {
    if (dump_fd >= 0) {
      if ( fsync(dump_fd) < 0 ) {
        ALOGE("%s: File flush (%s) failed: %s, errno: %d", __func__,
              path, strerror(errno), errno);
      }
      close(dump_fd);
      dump_fd = -1;
      ALOGI("%s: Write crashdump successfully : \n"
            "\t\tFile: %s\n\t\tdump_size: %d\n\t\twritten_size: %d",
            __func__, path, dump_size, total_size );
      int cntr = 0;
      size_t readsize;
      unsigned char *dumpinfo, *tempptr;
      uint32_t ucFilename;
      uint32_t uLinenum;
      uint32_t uPCAddr;
      uint32_t uResetReason;
      uint32_t uBuildVersion;
      uint32_t uReserved;
      dump_fd = -1;
      int i = 0;
      char filenameBuf [SOC_DUMP_PATH_BUF_SIZE];
      memset(filenameBuf, 0, SOC_DUMP_PATH_BUF_SIZE);
      if (-1 != (dump_fd = open(path, O_RDONLY))) {
        if (NULL != (dumpinfo = (unsigned char*)malloc(SOC_DUMP_SIGNATURE_SIZE))) {
          tempptr = dumpinfo;
          lseek(dump_fd, 0xFEE0, SEEK_SET);
          readsize = SOC_DUMP_SIGNATURE_SIZE;
          while (readsize) {
            cntr = read(dump_fd, (void*)tempptr, readsize);
            tempptr += cntr;
            readsize -= cntr;
          }

          tempptr = dumpinfo;
          memcpy(&ucFilename, tempptr, 4); tempptr += 4;
          memcpy(&uLinenum, tempptr, 4); tempptr += 4;
          memcpy(&uPCAddr, tempptr, 4); tempptr += 4;
          memcpy(&uResetReason, tempptr, 4); tempptr += 4;
          memcpy(&uBuildVersion, tempptr, 4); tempptr += 4;

          if (0 != ucFilename) {
            lseek(dump_fd, (off_t)ucFilename, SEEK_SET);
            cntr = read(dump_fd, (void*)filenameBuf, SOC_DUMP_PATH_BUF_SIZE);
            while (i < SOC_DUMP_PATH_BUF_SIZE && filenameBuf[i++] != '\0' ) ;
            if (i < SOC_DUMP_PATH_BUF_SIZE) {
              ALOGE("Filename::%s\n", filenameBuf);
            }
          }
          ALOGE("Linenum::%d\n", uLinenum);
          ALOGE("PC Addr::0x%x\n", uPCAddr);
          //ALOGE("Reset reason::%s\n", get_reset_reason_str(uResetReason));
          ALOGE("Build Version::0x%x\n", uBuildVersion);
        }
        if (NULL != dumpinfo) free(dumpinfo);
        close(dump_fd);
      }
    }

    is_crash_dump_in_progress_ = false;
  }

  if ( (0 == seq_num || LAST_SEQUENCE_NUM == seq_num) && crash_dump_status_cb_) {
    crash_dump_status_cb_(is_crash_dump_in_progress_);
  }
}

void Logger :: Init(HciTransport *hci_transport)
{
  diag_interface_.Init(hci_transport);
  DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_BT, ssr_bt_tbl_);
}

void Logger :: Cleanup(void)
{
  diag_interface_.CleanUp();
  dump_init = false;
}

int Logger :: get_dump_index()
{
  int dumpFd = -1;
  char path[IPC_DUMP_PATH_BUF_SIZE];
  /* Scan for present crash dump file */

  if (inx_scan == -1) {
    /*
     inx_scan is -1 for first crash after boot, scan through
     dump folder for a vacant spot to determine the dump index
    */
    for (inx_scan= 0; inx_scan<MAX_CRASH_DUMP_COUNT;inx_scan++) {
      snprintf(path, IPC_DUMP_PATH_BUF_SIZE, IPC_DUMP_PATH,inx_scan);
      ALOGV("%s: try to open file : %s", __func__, path);
      dumpFd = open(path, O_RDONLY);
      if(dumpFd < 0 && (errno == ENOENT) ) {
        ALOGV("%s: file( %s) can be used", __func__, path);
        break;
      }
    close(dumpFd);
    }

    inx_scan = inx_scan % MAX_CRASH_DUMP_COUNT;
    delete_dumps(inx_scan);
    dump_init = true;
  }
  else if (dump_init == false) {
    /*
     Dump index already known from previous crash,
     use it to calculate current index for filenames
    */
    inx_scan = (inx_scan + 1) % MAX_CRASH_DUMP_COUNT;
    delete_dumps(inx_scan);
    dump_init = true;
  }

  return inx_scan;
}

void Logger :: GetCrashDumpFileName(char* path)
{
  int16_t index;
  index =  get_dump_index();
  snprintf(path, SOC_DUMP_PATH_BUF_SIZE, SOC_DUMP_PATH,index);
}

#ifdef DUMP_IPC_LOG
void Logger :: GetUartDumpFilename(short int uart_dump, char* path)
{
  int16_t index;
  index =  get_dump_index();

  switch (uart_dump){
  case IpcLogs::TX_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_TX_LOGS_DEST_PATH, index);
    break;
  case IpcLogs::RX_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_RX_LOGS_DEST_PATH, index);
    break;
  case IpcLogs::STATE_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_STATE_LOGS_DEST_PATH, index);
    break;
  case IpcLogs::POWER_LOG:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_IPC_PWR_LOGS_DEST_PATH, index);
    break;
  case IpcLogs::TX_LOGS_DEST:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_UART_TX_LOGS_DEST_PATH, index);
    break;
  case IpcLogs::RX_LOGS_DEST:
    snprintf(path, UART_IPC_PATH_BUF_SIZE, UART_UART_RX_LOGS_DEST_PATH, index);
  }
}
#endif

#ifdef DUMP_RINGBUF_LOG
void Logger :: GetRingBufFileName(short int dir, char* path)
{
  uint16_t index;
  index =  get_dump_index();
  switch (dir) {
  case  RingBuffer::HOST_TO_SOC:
    snprintf(path, RING_BUFF_FILE_NAME_LEN, RING_BUFF_LOG_TX_FILE_NAME, index); //HOST_TO_SOC
    break;
  case RingBuffer::SOC_TO_HOST:
    snprintf(path, RING_BUFF_FILE_NAME_LEN, RING_BUFF_LOG_RX_FILE_NAME, index); //SOC_TO_HOST
    break;
  }
}
#endif

void Logger :: delete_dumps(int16_t index)
{
  int16_t index_oldest;
  char crashdump_path[SOC_DUMP_PATH_BUF_SIZE + 1];
  char uart_path[UART_IPC_PATH_BUF_SIZE];
  char ringbuff_path[RING_BUFF_FILE_NAME_LEN];

  index_oldest = (index + 1) % MAX_CRASH_DUMP_COUNT;

  snprintf(crashdump_path, SOC_DUMP_PATH_BUF_SIZE, SOC_DUMP_PATH,index_oldest);
  remove(crashdump_path);

  snprintf(uart_path, UART_IPC_PATH_BUF_SIZE, UART_IPC_TX_LOGS_DEST_PATH, index_oldest);
  remove(uart_path);
  snprintf(uart_path, UART_IPC_PATH_BUF_SIZE, UART_IPC_RX_LOGS_DEST_PATH, index_oldest);
  remove(uart_path);
  snprintf(uart_path, UART_IPC_PATH_BUF_SIZE, UART_IPC_STATE_LOGS_DEST_PATH, index_oldest);
  remove(uart_path);
  snprintf(uart_path, UART_IPC_PATH_BUF_SIZE, UART_IPC_PWR_LOGS_DEST_PATH, index_oldest);
  remove(uart_path);

  snprintf(ringbuff_path, RING_BUFF_FILE_NAME_LEN, RING_BUFF_LOG_TX_FILE_NAME, index_oldest);
  remove(ringbuff_path);
  snprintf(ringbuff_path, RING_BUFF_FILE_NAME_LEN, RING_BUFF_LOG_RX_FILE_NAME, index_oldest);
  remove(ringbuff_path);
}

bool Logger :: isSsrTriggered()
{
  return diag_interface_.isSsrTriggered();
}

void Logger :: ResetSsrTriggeredFlag()
{
    return diag_interface_.ResetSsrTriggeredFlag();
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
