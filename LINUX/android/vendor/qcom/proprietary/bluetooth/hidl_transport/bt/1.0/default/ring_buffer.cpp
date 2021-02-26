/*==========================================================================
Description
  It has implementation for ring buffer.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#define LOG_TAG "vendor.qti.bluetooth@1.0-ring_buffer"

#include <string.h>
#include <stdlib.h>
#include <utils/Log.h>

#include "ring_buffer.h"
#include "logger.h"
#include <private/android_filesystem_config.h>

#define TIME_STR_SIZE 12

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef DUMP_RINGBUF_LOG

void RingBuffer::AddLog(short int dir, HciPacketType type, uint8_t *pkt, uint16_t len)
{
  if (dir == HOST_TO_SOC) {
    std::unique_lock<std::mutex> guard(ring_buffer_mutex_);
    tx_.AddBuffNode(type, len, pkt);
  }
  else
    rx_.AddBuffNode(type, len, pkt);
}

void RingBuffer::DumpData()
{
  ALOGD("%s", __func__);
  {
    std::unique_lock<std::mutex> guard(ring_buffer_mutex_);
    tx_.DumpData(HOST_TO_SOC);
  }
  rx_.DumpData(SOC_TO_HOST);
}

void RingBuffer::AddIbsCmd(short int dir, uint8_t ibs_data)
{
  if (dir == HOST_TO_SOC) {
    std::unique_lock<std::mutex> guard(ring_buffer_mutex_);
    tx_.AddIbsData(ibs_data);
  }
  else
    rx_.AddIbsData(ibs_data);
}

PacketBuff :: PacketBuff()
{
  memset(buff_index_, 0, sizeof(buff_index_));
  active_buff_ = 0;
}

PacketBuff::~PacketBuff()
{
  memset(buff_index_, 0, sizeof(buff_index_));
  active_buff_ = 0;
}

void PacketBuff::AddIbsData(uint8_t ibs_data)
{
  uint16_t log_len = TIME_STR_SIZE;

  if (log_len + 2 > max_buff_size_ - buff_index_[active_buff_]) {
    //switch active buffer if current active buffer is full.
    active_buff_ = (active_buff_ + 1) % 2;
    buff_index_[active_buff_] = 0;
    memset(&buff_[active_buff_][0], 0x00, max_buff_size_);
  }
  //Add time string to current active ring buffer.
  add_time_str((char*)&buff_[active_buff_][buff_index_[active_buff_]]);
  buff_[active_buff_][TIME_STR_SIZE + buff_index_[active_buff_]] = ibs_data;
  buff_index_[active_buff_] += (TIME_STR_SIZE + 1);  //1 is for the IBS byte
}

void PacketBuff::AddBuffNode(HciPacketType type, uint16_t len, uint8_t *pkt)
{
  uint16_t log_len = TIME_STR_SIZE + len;

  if (log_len + 2 > max_buff_size_ - buff_index_[active_buff_]) {
    //switch active buffer if current active buffer is full.
    active_buff_ = (active_buff_ + 1) % 2;
    buff_index_[active_buff_] = 0;
    memset(&buff_[active_buff_][0], 0x00, max_buff_size_);
  }
  //Add time string to current active ring buffer.
  add_time_str((char*)&buff_[active_buff_][buff_index_[active_buff_]]);
  buff_[active_buff_][TIME_STR_SIZE + buff_index_[active_buff_]] = (uint8_t)type;
  buff_index_[active_buff_] += (TIME_STR_SIZE + 1);  //1 is for the type byte

  memcpy(&buff_[active_buff_][buff_index_[active_buff_]], pkt, len);
  buff_index_[active_buff_] += len;
}

void PacketBuff::DumpData(short int dir)
{
  char file_name[RING_BUFF_FILE_NAME_LEN];
  int fd;
  int index = 0;

  ALOGE("%s: Dumping data from static buffer", __func__);
  logger_ = static_cast<void*>(Logger::Get());
  static_cast<Logger*>(logger_)->GetRingBufFileName(dir,file_name);

  fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  if (fd < 0)
    return;

  if ((index = buff_index_[(active_buff_ + 1) % 2]) > 0) {
    write(fd, &buff_[(active_buff_ + 1) % 2][0], index);
  }

  if ((index = buff_index_[active_buff_]) > 0) {
    write(fd, &buff_[active_buff_][0], index);
  }

  fsync(fd);
  close(fd);
}

void add_time_str(char *src)
{
  struct timeval tval;
  unsigned short int t_hh, t_mm, t_ss;

  gettimeofday(&tval, NULL);
  t_hh = tval.tv_sec / 3600;
  tval.tv_sec = tval.tv_sec % 3600;
  t_mm = tval.tv_sec / 60;
  t_ss = tval.tv_sec % 60;

  snprintf(src, TIME_STR_SIZE + 1, "%.2d:%.2d:%.2d:%.3d", t_hh, t_mm, t_ss, (int)(tval.tv_usec / 1000));
}

#endif
}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
