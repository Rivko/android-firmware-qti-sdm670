/*==========================================================================
Description
  It has implementation for logger class

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <hidl/HidlSupport.h>
#include "ring_buffer.h"
#include "uart_ipc.h"
#include "hci_internals.h"
#include "diag_interface.h"

/* Packet types */
#define LOG_BT_CMD_PACKET_TYPE     0x01
#define LOG_BT_ACL_PACKET_TYPE     0x02
#define LOG_BT_SCO_PACKET_TYPE     0x03
#define LOG_BT_EVT_PACKET_TYPE     0x04
#define LOG_ANT_CTL_PACKET_TYPE    0x0c
#define LOG_ANT_DATA_PACKET_TYPE   0x0e
#define LOG_BT_EVT_VENDOR_SPECIFIC 0xFF
#define LOG_BT_EVT_CMD_CMPLT 0x0E

/* Message type of the log from controller */
#define LOG_BT_CONTROLLER_LOG        0x01
#define LOG_BT_MESSAGE_TYPE_VSTR     0x02
#define LOG_BT_MESSAGE_TYPE_PACKET   0x05
#define LOG_BT_MESSAGE_TYPE_MEM_DUMP 0x08

/* Sub log ID for the message type PACKET */
#define LOG_BT_HCI_CMD   0
#define LOG_BT_HCI_EVENT 1

#define LOG_BT_RX_LMP_PDU      18
#define LOG_BT_TX_LMP_PDU      19
#define LOG_BT_RX_LE_CTRL_PDU  20
#define LOG_BT_TX_LE_CTRL_PDU  21
#define LOG_BT_TX_LE_CONN_MNGR 22

#define LOG_BT_LINK_MANAGER_STATE    0x80
#define LOG_BT_CONN_MANAGER_STATE    0x81
#define LOG_BT_SECURITY_STATE        0x82
#define LOG_BT_LE_CONN_MANAGER_STATE 0x83
#define LOG_BT_LE_CHANNEL_MAP_STATE  0x84
#define LOG_BT_LE_ENCRYPTION_STATE   0x85

/* Sub log ID for the message type VSTR */
#define LOG_BT_VSTR_ERROR 0
#define LOG_BT_VSTR_HIGH  1
#define LOG_BT_VSTR_LOW   2

/* Diag ID for LMP packers */
#define LOG_BT_DIAG_LMP_LOG_ID 0x1041
#define LOG_BT_DIAG_LMP_RX_ID  0x1048
#define LOG_BT_DIAG_LMP_TX_ID  0x1049

/* To format LMP logs */
#define LOG_BT_QXDM_PKT_LENGTH_POS    0
#define LOG_BT_QXDM_PKT_LENGTH2_POS   1
#define LOG_BT_QXDM_DEVICE_IDX_POS    2
#define LOG_BT_QXDM_PKT_POS           3

#define LOG_BT_DBG_DEVICE_IDX_POS 0
#define LOG_BT_DBG_PKT_LENGTH_POS 1
#define LOG_BT_DBG_PKT_POS 2

/* Headed size of the log */
#define LOG_BT_HEADER_SIZE (sizeof(bt_log_pkt) - 1)
#define CRASH_SOURCE_FILE_PATH_LEN 50

#define MAX_RING_BUF_SIZE (8*1024)
#define OUT_RING_BUF 0
#define IN_RING_BUF 1

#define LOG_I(tag, fmt, args...) ALOG(LOG_INFO, tag, fmt, ## args)
#define LOG_W(tag, fmt, args...) ALOG(LOG_WARN, tag, fmt, ## args)
#define LOG_E(tag, fmt, args...) ALOG(LOG_ERROR, tag, fmt, ## args)

#define IBS_LOG_INFO(tag, fmt, args...) if(Logger.GetIbsLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define IBS_LOG_WAR(tag, fmt, args...) if(Logger.GetIbsLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define IBS_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define RX_LOG_INFO(tag, fmt, args...) if(Logger.GetRxLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define RX_LOG_WAR(tag, fmt, args...) if(Logger.GetRxLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define RX_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define TX_LOG_INFO(tag, fmt, args...) if(Logger.GetTxLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define TX_LOG_WAR(tag, fmt, args...) if(Logger.GetTxLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define TX_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

#define GEN_LOG_INFO(tag, fmt, args...) if(Logger.GetGenLogLevel() == Logger::log_level::HIGH)LOG_I(tag, fmt, ##args)
#define GEN_LOG_WAR(tag, fmt, args...) if(Logger.GetGenLogLevel() >= Logger::log_level::MID)LOG_W(tag, fmt, ##args)
#define GEN_LOG_ERR(tag, fmt, args...) LOG_E(tag, fmt, ##args)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using CrashDumpStatusCallback = std::function<void(bool)>;

class Logger
{
 private:
  static Logger *instance_;
  static const diagpkt_user_table_entry_type ssr_bt_tbl_[];
  int ssr_level;
  short int ibs_log_level_, rx_log_level_, tx_log_level_, gen_log_level_;
  int16_t inx_scan = -1;
  bool dump_init = false;
#ifdef DUMP_RINGBUF_LOG
  RingBuffer ring_buff_;
#endif
#ifdef DUMP_IPC_LOG
  IpcLogs ipc_log_;
#endif
  DiagInterface diag_interface_;
  bool is_crash_dump_in_progress_;
  CrashDumpStatusCallback crash_dump_status_cb_;

  static void *SsrBtHandler(void *, uint16);
  bool HandleHciEvent(HciPacketType, uint8_t *, uint16_t);
  void SaveSocMemDump(uint8_t *, uint16_t);
  int get_dump_index();
  void delete_dumps(int16_t);

 public:
  Logger();
  enum {
    LOW = 0,
    MID,
    HIGH,
  } log_level;

  static Logger *Get(void);

  void GetCrashDumpFileName(char*);
#ifdef DUMP_RINGBUF_LOG
  void GetRingBufFileName(short int, char*);
  void SaveIbsToRingBuf(short int dir, uint8_t ibs_data) {
    ring_buff_.AddIbsCmd(dir, ibs_data);
  };

  void DumpRingBuffers() {
    ring_buff_.DumpData();
  };
#endif

#ifdef DUMP_IPC_LOG
  void GetUartDumpFilename(short int, char*);
  void DumpUartIpcLogs(void) {
    ipc_log_.DumpUartLogs();
  };
#endif

  bool ProcessRx(HciPacketType type, const uint8_t *buff, uint16_t len);
  bool ProcessTx(HciPacketType, const uint8_t *, uint16_t);
  bool IsSnoopLogEnabled(void);
  bool IsHciFwSnoopEnabled(void);

  void Init(HciTransport *hci_transport);
  void Cleanup(void);

  bool IsControllerLogPkt(HciPacketType type, const uint8_t *buff, uint16_t len);
  bool IsCrashDumpStarted(void);
  bool RegisterCrashDumpCallback (CrashDumpStatusCallback crash_dump_cb);
  bool isSsrTriggered();
  void ResetSsrTriggeredFlag();
  inline short int GetIbsLogLevel() {return ibs_log_level_;};
  inline short int GetRxLogLevel() {return rx_log_level_;};
  inline short int GetTxLogLevel() {return tx_log_level_;};
  inline short int GetGenLogLevel() {return gen_log_level_;};
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
