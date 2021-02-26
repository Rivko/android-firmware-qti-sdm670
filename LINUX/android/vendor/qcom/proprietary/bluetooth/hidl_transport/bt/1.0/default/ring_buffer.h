/*==========================================================================
Description
  It has implementation for ring buffer.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <fcntl.h>
#include <errno.h>
#include <mutex>
#include "hci_internals.h"

#define RING_BUFF_FILE_NAME_LEN 60
#define RING_BUFF_LOG_TX_FILE_NAME    "/data/vendor/ramdump/bluetooth/%.02d-ring_buff_tx.txt"
#define RING_BUFF_LOG_RX_FILE_NAME    "/data/vendor/ramdump/bluetooth/%.02d-ring_buff_rx.txt"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef DUMP_RINGBUF_LOG
class PacketBuff
{
  const static long int max_buff_size_ = 1024*50;

  unsigned int active_buff_;
  uint8_t buff_[2][max_buff_size_];
  long int buff_index_[2];  
  void *logger_;

 public:
  PacketBuff(void);
  ~PacketBuff();
  void AddBuffNode(HciPacketType, uint16_t, uint8_t *);
  void AddIbsData(uint8_t ibs_data);
  void DumpData(short int dir);
};

class RingBuffer
{
 private:
  PacketBuff tx_;
  PacketBuff rx_;
  std::mutex ring_buffer_mutex_;

 public:
  enum {
    HOST_TO_SOC,
    SOC_TO_HOST,
  } pkt_direction;

  RingBuffer(void) {};
  void AddLog(short int, HciPacketType, uint8_t *, uint16_t);
  void AddIbsCmd(short int dir, uint8_t ibs_data);
  void DumpData();
};

void add_time_str(char *src);
#endif
}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
