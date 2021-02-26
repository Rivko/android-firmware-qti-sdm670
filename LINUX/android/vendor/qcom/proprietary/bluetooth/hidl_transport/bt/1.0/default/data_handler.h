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
// Copyright 2016 The Android Open Source Project
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
#include "hci_transport.h"
#include <thread>
#include "controller.h"
#include "uart_controller.h"
#include "mct_controller.h"
#include "wake_lock.h"
#include <mutex>


namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using InitializeCallback = std::function<void(bool success)>;

enum InitStatusType {
  INIT_STATUS_IDLE = 0,
  INIT_STATUS_INITIALIZING = 1,
  INIT_STATUS_FAILED = 2,
  INIT_STATUS_SUCCESS = 3
};

enum HciCommand {
  HCI_RESET_CMD = 1,
  SOC_PRE_SHUTDOWN_CMD,
};

typedef struct {
  ProtocolType type;
  bool is_pending_init_cb;
  InitializeCallback init_cb;
  DataReadCallback data_read_cb;
} ProtocolCallbacksType;

// it is the interface to the all external class like BT and FM and ANT.

class DataHandler {
 public:
  // with this APIs respective modules will register with their dataread cllback.
  // callback implementaiton varies from BT to FM.
  static bool Init(ProtocolType type, InitializeCallback init_cb,
      DataReadCallback data_read_cb);
  static void CleanUp (ProtocolType type);
  static DataHandler* Get();
  BluetoothSocType GetSocType();

  /*Sends command and wait for event*/
  void sendCommandWait(HciCommand cmd);

  // this is used to send the actual packet.
  size_t SendData(ProtocolType type, HciPacketType packet_type, const uint8_t *data, size_t length);

 private:
  DataHandler();
  static void usr1_handler(int s);
  bool Open(ProtocolType type, InitializeCallback init_cb, DataReadCallback data_read_cb);
  bool Close(ProtocolType type);
  BluetoothSocType GetSocTypeInt();
  void OnPacketReady(ProtocolType , HciPacketType, const hidl_vec<uint8_t>*);
  bool isAwaitedEvent(const uint8_t *buff, uint16_t len);
  virtual ~DataHandler() = default;

  Controller *controller_;
  Logger *logger_;
  std::mutex internal_mutex_;
  int init_status_{INIT_STATUS_IDLE};
  std::thread init_thread_;
  BluetoothSocType soc_type_;
  std::map<ProtocolType , ProtocolCallbacksType *> protocol_info_;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
