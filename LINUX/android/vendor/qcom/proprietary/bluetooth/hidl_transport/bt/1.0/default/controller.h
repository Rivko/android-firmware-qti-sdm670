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
#include "power_manager.h"
#include "hci_transport.h"
#include "async_fd_watcher.h"
#include "hci_internals.h"
#include "hci_packetizer.h"
#include "logger.h"


enum ProtocolType {
  TYPE_BT,
  TYPE_FM,
  TYPE_ANT
};

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using PacketReadCallback = std::function<void(ProtocolType, HciPacketType, const hidl_vec<uint8_t>*)>;

// singleton class
class Controller {
 public:
  // used to get the instance_ controller class
  // open expects the packet reader callback and when the data is available it will call this callback with
  // protocol and pacekt type, and the actual packet.
  virtual bool Init(PacketReadCallback packet_read_cb) = 0;
  // send packet is used to send the data. protocol type, actual packet types are expected.
  virtual size_t SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length) = 0;
  virtual bool Cleanup(void) = 0;
  virtual ~Controller() {};
  virtual void WaitforCrashdumpFinish() {};
  virtual void SignalCrashDumpFinish() {};
  virtual bool Disconnect() { return true; };

 protected:
  // used to poll fd for reading the soc data.
  AsyncFdWatcher fd_watcher_;
  PacketReadCallback read_cb_;
  HciTransport *hci_transport_;
  PowerManager power_manager_;
  bool init_done_;
  PacketReadCallback packet_read_cb_;
  Logger* logger_;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
