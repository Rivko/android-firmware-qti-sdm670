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
#include "nvm_tags_manager.h"
#include "ibs_handler.h"
#include "hci_internals.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {


// singleton class
class MctController : public Controller {
 public:
  // used to get the instance_ controller class
  virtual bool Init(PacketReadCallback packet_read_cb) override;
  // send packet is used to send the data. protocol type, actual packet types are expected.
  virtual size_t SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length) override;
  virtual bool Cleanup(void) override;

  void OnEventDataReady(int fd);

  void OnAclDataReady(int fd);

  void OnEventPacketReady(hidl_vec<uint8_t> *data);

  void OnAclPacketReady(hidl_vec<uint8_t> *data);

  MctController(BluetoothSocType soc_type);

 private:
  bool DevInReset(int);
  BluetoothSocType soc_type_;
  HciPacketizer hci_event_packetizer_;
  HciPacketizer hci_acl_packetizer_;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
