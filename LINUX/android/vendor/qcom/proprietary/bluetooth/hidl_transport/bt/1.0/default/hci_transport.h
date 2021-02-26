/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <hidl/HidlSupport.h>
#include "hci_internals.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

typedef enum {
  BT_SOC_DEFAULT = 0,
  BT_SOC_SMD = BT_SOC_DEFAULT,
  BT_SOC_AR3K,
  BT_SOC_ROME,
  BT_SOC_CHEROKEE,
  BT_SOC_NAPIER,
  BT_SOC_NAPLES_UART,
  /* Add chipset type here */
  BT_SOC_RESERVED
} BluetoothSocType;

class HciTransport {
 public:
  virtual bool Init(BluetoothSocType soc_type) = 0;
  virtual int GetCtrlFd() = 0;
  virtual int GetDataFd() = 0;
  virtual bool CleanUp(void) = 0;
  virtual int Read(unsigned char* buf, size_t len) = 0;
  virtual int Write(HciPacketType type, const uint8_t *buf, int len) = 0;
  virtual ~HciTransport() {};
  virtual void Disconnect() {};

 protected:
  BluetoothSocType soc_type_;
  std::mutex internal_mutex_;
  int ctrl_fd_;
  int data_fd_;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
