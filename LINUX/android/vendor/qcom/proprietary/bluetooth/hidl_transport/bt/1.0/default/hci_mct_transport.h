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
#include "hci_transport.h"
#include "wake_lock.h"
#include "uart_utils.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

class HciMctTransport : public HciTransport {
 public:
  HciMctTransport() {};
  virtual bool Init(BluetoothSocType soc_type) override;
  static HciTransport* Get();
  virtual int GetCtrlFd() override;
  virtual int GetDataFd() override;
  virtual bool CleanUp(void) override;
  int Read(unsigned char* buf, size_t len) override;
  int Write(HciPacketType type, const uint8_t *buf, int len) override;

 private:
  int WriteSafely(int fd, const uint8_t *buf, int len);

};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android

