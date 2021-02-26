/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright 2012 The Android Open Source Project
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
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

#include "hci_transport.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

/*Constants*/
typedef enum {
  POWER_MANAGER_OFF = 0,
  POWER_MANAGER_TURNING_ON,
  POWER_MANAGER_ON,
  POWER_MANAGER_TURNING_OFF,
} PowerManagerState;
  
class PowerManager {
 private:
  /*Variables*/
  int rfkill_id_;
  BluetoothSocType bt_soc_type_;
  PowerManagerState pm_state_;
  bool ext_ldo_;

  /*Constants*/
  static const int power_ctrl_cmd_ = 0xbfad;
  const char *pwr_control_dev_;
  const char *rfkill_state_path_, *rfkill_type_path_, *extldo_path_;

  /*Methods*/
  bool PowerUpSmd(void);
  int GetRfkillFd(void);
  bool ControlRfkill(int rfkill_fd, bool enable);
  bool PowerUpCherokee(bool enable);
  int InitializeRfkill(void);
  bool SetExtLdoStatus(void);
  int Ioctl(int, int);
  void Cleanup(int);

 public:
  /*Methods*/
  void Init(BluetoothSocType soc_type);
  bool SetPower(bool);
  PowerManagerState GetState(void);
  void Cleanup(void);
  PowerManager(void);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
