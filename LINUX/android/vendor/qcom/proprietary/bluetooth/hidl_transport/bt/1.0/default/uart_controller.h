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

#pragma once

#include <hidl/HidlSupport.h>
#include "controller.h"
#include "patch_dl_manager.h"
#include "ibs_handler.h"
#include "hci_internals.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {


enum CrashTimerState {
  TIMER_NOT_CREATED = 0x00,
  TIMER_CREATED = 0x01,
  TIMER_ACTIVE = 0x02
};

// singleton class
class UartController : public Controller {
 public:
  // used to get the instance_ controller class
  // open expects the packet reader callback and when the data is available it will call this callback with
  // protocol and pacekt type, and the actual packet.
  virtual bool Init(PacketReadCallback packet_read_cb) override;
  // send packet is used to send the data. protocol type, actual packet types are expected.
  virtual size_t SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length) override;
  virtual bool Cleanup(void) override;

  void OnDataReady(int fd);

  void OnPacketReady(hidl_vec<uint8_t> *data);
  void bt_kernel_panic(void);
  UartController(BluetoothSocType soc_type);
  static void SocCrashWaitTimeout(union sigval sig);
  virtual void WaitforCrashdumpFinish();
  virtual void SignalCrashDumpFinish();
  virtual bool Disconnect();
 private:
  void ReportSocFailure(bool dumped_uart_log);
  void StartSocCrashWaitTimer(int ssrtimeout);
  bool StopSocCrashWaitTimer();
  void CleanupSocCrashWaitTimer();
  void SendSpecialBuffer();
  void SsrCleanup(uint8_t reason);
  bool command_is_get_dbg_info(const unsigned char* buf, int len);
  bool IsHciPacketValid(HciPacketType type);

 private:
  int soc_crash_wait_timer_state_;
  timer_t soc_crash_wait_timer_;
  bool force_special_byte_enabled_;
  bool sibs_enabled_;
  bool soc_crashed;
  BluetoothSocType soc_type_;
  HciPacketType hci_packet_type_{HCI_PACKET_TYPE_UNKNOWN};
  HciPacketizer hci_packetizer_;
  std::condition_variable cv;
  std::mutex cv_m;
  bool is_cmd_timeout;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
