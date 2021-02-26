/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2017 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "hci_packetizer.h"

#include <android-base/logging.h>
#include <utils/Log.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <thread>

#define LOG_TAG "vendor.qti.bluetooth@1.0-hci_packetizer"

namespace {

const size_t GetPreambleSizeForType(HciPacketType type)
{
  size_t header_len = 0;

  switch (type) {
    case HCI_PACKET_TYPE_COMMAND:
      header_len = HCI_COMMAND_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_ACL_DATA:
      header_len = HCI_ACL_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_SCO_DATA:
      header_len = HCI_SCO_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_EVENT:
      header_len = HCI_EVENT_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_ANT_CTRL:
      header_len = HCI_ANT_CMD_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_ANT_DATA:
      header_len = HCI_ANT_CMD_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_FM_CMD:
      header_len = HCI_FM_CMD_PREAMBLE_SIZE;
      break;
    case HCI_PACKET_TYPE_FM_EVENT:
      header_len = HCI_FM_EVENT_PREAMBLE_SIZE;
      break;
    default:
      break;
  }
  return header_len;
};

const size_t GetPacketLenOffsetForType(HciPacketType type)
{
  size_t offset = 0;

  switch (type) {
    case HCI_PACKET_TYPE_COMMAND:
      offset = HCI_LENGTH_OFFSET_CMD;
      break;
    case HCI_PACKET_TYPE_ACL_DATA:
      offset = HCI_LENGTH_OFFSET_ACL;
      break;
    case HCI_PACKET_TYPE_SCO_DATA:
      offset = HCI_LENGTH_OFFSET_SCO;
      break;
    case HCI_PACKET_TYPE_EVENT:
      offset = HCI_LENGTH_OFFSET_EVT;
      break;
    case HCI_PACKET_TYPE_ANT_CTRL:
      offset = HCI_LENGTH_OFFSET_ANT_CMD;
      break;
    case HCI_PACKET_TYPE_ANT_DATA:
      offset = HCI_LENGTH_OFFSET_ANT_CMD;
      break;
    case HCI_PACKET_TYPE_FM_CMD:
      offset = HCI_LENGTH_OFFSET_FM_CMD;
      break;
    case HCI_PACKET_TYPE_FM_EVENT:
      offset = HCI_LENGTH_OFFSET_FM_EVT;
      break;
    default:
      break;
  }
  return offset;
}

size_t HciGetPacketLengthForType(HciPacketType type, const uint8_t* preamble)
{
  size_t offset = GetPacketLenOffsetForType(type);

  if (type != HCI_PACKET_TYPE_ACL_DATA) return preamble[offset];
  return (((preamble[offset + 1]) << 8) | preamble[offset]);
}

}  // namespace

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {


void HciPacketizer::OnDataReady(int fd, HciPacketType packet_type)
{
  switch (state_) {
    case HCI_PREAMBLE: {
      ssize_t bytes_read = TEMP_FAILURE_RETRY(
        read(fd, preamble_ + bytes_read_,
             GetPreambleSizeForType(packet_type) - bytes_read_));

      if(bytes_read < 0) {
        ALOGE("preamble read returned %zd error %s", bytes_read, strerror(errno));
        if((-1 == bytes_read ) && (( EAGAIN == errno)||(EWOULDBLOCK == errno)||( EIO == errno)))
          break;
      } else if(bytes_read == 0) {
        if(++retry_read_ < 5) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }else if(retry_read_ == 5) {
          packet_ready_cb_(nullptr);
        } else {
          ALOGE("read returned %zd retry_read_ %zd", bytes_read, retry_read_);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return;
      }
      CHECK(bytes_read > 0);
      retry_read_ = 0;
      bytes_read_ += bytes_read;
      if (bytes_read_ == GetPreambleSizeForType(packet_type)) {
        packet_ = new hidl_vec<uint8_t>;
        size_t packet_length =
          HciGetPacketLengthForType(packet_type, preamble_);
        packet_->resize(GetPreambleSizeForType(packet_type) + packet_length);
        memcpy(packet_->data(), preamble_, GetPreambleSizeForType(packet_type));
        bytes_remaining_ = packet_length;
        if (packet_length == 0) {
          ALOGE("ACL packet with 0 length ");
          packet_ready_cb_(packet_);
        } else {
          state_ = HCI_PAYLOAD;
        }
        bytes_read_ = 0;
      }
      break;
    }

    case HCI_PAYLOAD: {
      ssize_t bytes_read = TEMP_FAILURE_RETRY(read(
                           fd, packet_->data() + GetPreambleSizeForType(packet_type)
                           + bytes_read_, bytes_remaining_));
      if(bytes_read < 0) {
        ALOGE("payload read returned %zd error %s", bytes_read, strerror(errno));
        if((-1 == bytes_read ) && (( EAGAIN == errno)||(EWOULDBLOCK == errno)||( EIO == errno)))
          break;
      } else if(bytes_read == 0) {
        if(++retry_read_ < 5) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          return;
        }
        packet_ready_cb_(nullptr);
        return;
      }
      CHECK(bytes_read > 0);
      retry_read_ = 0;
      bytes_remaining_ -= bytes_read;
      bytes_read_ += bytes_read;
      if (bytes_remaining_ == 0) {
        packet_ready_cb_(packet_);
        state_ = HCI_PREAMBLE;
        bytes_read_ = 0;
      }
      break;
    }
  }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
