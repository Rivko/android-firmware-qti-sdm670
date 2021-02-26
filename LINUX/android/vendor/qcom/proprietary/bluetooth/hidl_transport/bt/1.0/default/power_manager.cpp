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

#define LOG_TAG "vendor.qti.bluetooth@1.0-power_manager"

#include <utils/Log.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <fcntl.h>

#include "power_manager.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {


#define RFKILL_STATE_PATH  "/sys/class/rfkill/rfkill%d/state"
#define RFKILL_TYPE_PATH  "/sys/class/rfkill/rfkill%d/type"
#define EXTLDO_PATH       "/sys/class/rfkill/rfkill%d/device/extldo"

PowerManager :: PowerManager()
{
  rfkill_id_ = -1;
  bt_soc_type_ = BT_SOC_RESERVED;
  ext_ldo_ = false;
  pm_state_ = POWER_MANAGER_OFF;
  pwr_control_dev_ = "/dev/btpower";

}

void PowerManager :: Init(BluetoothSocType soc_type)
{
  if (pm_state_ != POWER_MANAGER_OFF)
    return;
  bt_soc_type_ = soc_type;
}

bool PowerManager :: SetPower(bool enable)
{
  bool ret = false;

  ALOGD("%s: enable: %x", __func__, enable);

  if (bt_soc_type_ == BT_SOC_RESERVED) {
    ALOGE("Power Manager not initialized, Returning!");
    return false;
  }

  pm_state_ = enable ? POWER_MANAGER_TURNING_ON : POWER_MANAGER_TURNING_OFF;

  switch (bt_soc_type_) {
    int rfkill_fd;
    /*power up for smd based chip*/
    case BT_SOC_SMD:
      ret = PowerUpSmd();
      break;

      /*power up for uart based chip*/
    case BT_SOC_NAPIER:
    case BT_SOC_ROME:
    case BT_SOC_NAPLES_UART:
    case BT_SOC_AR3K:
      rfkill_fd = GetRfkillFd();
      if (rfkill_fd < 0)
        return false;

      ret = ControlRfkill(rfkill_fd, enable);
      close(rfkill_fd);
      break;

    case BT_SOC_CHEROKEE:
      ret = PowerUpCherokee(enable);
      break;

    default:
      ALOGE("%s: unknown soc type %d", __func__, bt_soc_type_);
      break;
  }

  pm_state_ = enable ^ ret ? POWER_MANAGER_OFF : POWER_MANAGER_ON;

  return ret;
}

void PowerManager :: Cleanup(void)
{
  if (pm_state_ != POWER_MANAGER_OFF)
    return;
  rfkill_id_ = -1;
  ext_ldo_ = false;
}

PowerManagerState PowerManager :: GetState()
{
  return pm_state_;
}

bool PowerManager :: PowerUpSmd()
{
  ALOGE("smd initialization");
  return true;
}

int PowerManager :: GetRfkillFd()
{
  int rfkill_fd;

  rfkill_fd = InitializeRfkill();
  if (rfkill_fd < 0) {
    ALOGE("rfkill initialization failed, returning!");
    Cleanup();
    return -1;
  }

  if (!SetExtLdoStatus()) {
    ALOGE("%s: set ext ldo status fail", __func__);
    Cleanup(rfkill_fd);
    return -1;
  }

  ALOGI("%s: rfkill_fd: %d", __func__, rfkill_fd);

  return rfkill_fd;
}

bool PowerManager :: PowerUpCherokee(bool enable)
{
  int fd_btpower;

  ALOGE("\nbt_power_cherokee enable  (%d)\n", enable);
  fd_btpower = open(pwr_control_dev_, O_RDWR, O_NONBLOCK);
  if (fd_btpower < 0) {
    ALOGE("\nfailed to open bt device error = (%s)\n", strerror(errno));
    Cleanup();
    return false;
  }

  if (Ioctl(fd_btpower, enable ? 1 : 0) < 0) {
    Cleanup();
    return false;
  }

  close(fd_btpower);
  return true;
}

bool PowerManager :: ControlRfkill(int rfkill_fd, bool enable)
{
  char power_on = enable ? '1' : '0';

  ALOGD("%s: rfkill_fd: %d, enable: %x", __func__, rfkill_fd, enable);

  if (write(rfkill_fd, &power_on, 1) < 0) {
    ALOGE("%s: write rfkill failed: %s (%d)", __func__, strerror(errno), errno);
    Cleanup(rfkill_fd);
    return false;
  }

  return true;
}

int PowerManager :: InitializeRfkill()
{
  int fd, read_size;
  char rfkill_type[64] = {'\0'}, rfkill_state[64] = {'\0'}, type[16] = {'\0'};

  for (int i = 0; rfkill_id_ == -1; i++) {
    snprintf(rfkill_type, sizeof(rfkill_type), (char *)RFKILL_TYPE_PATH, i);
    if ((fd = open(rfkill_type, O_RDONLY)) < 0) {
      ALOGE("open(%s) failed: %s (%d)\n", rfkill_type, strerror(errno), errno);
      return -1;
    }

    read_size = read(fd, &type, sizeof(type));
    close(fd);

    if (read_size >= 9 && !memcmp(type, "bluetooth", 9)) {
      rfkill_id_ = i;
      break;
    }
  }

  snprintf(rfkill_state, sizeof(rfkill_state), (char *)RFKILL_STATE_PATH, rfkill_id_);

  fd = open(rfkill_state, O_RDWR);
  if (fd < 0)
    ALOGE("open(%s) for write failed: %s (%d)", rfkill_state, strerror(errno), errno);

  return fd;
}

bool PowerManager :: SetExtLdoStatus()
{
  int read_size, ldo_fd;
  char curr_ldo_path[64] = {'\0'}, enable_ldo[6] = {'\0'};

  snprintf(curr_ldo_path, sizeof(curr_ldo_path), (char *)EXTLDO_PATH, rfkill_id_);
  if ((ldo_fd = open(curr_ldo_path, O_RDWR)) < 0) {
    ALOGE("open(%s) failed: %s (%d)", curr_ldo_path, strerror(errno), errno);
    return false;
  }

  read_size = read(ldo_fd, &enable_ldo, sizeof(enable_ldo));
  close(ldo_fd);

  if (read_size <= 0) {
    ALOGE("read(%s) failed: %s (%d)", curr_ldo_path, strerror(errno), errno);
    return false;
  }

  if (!memcmp(enable_ldo, "true", 4)) {
    ALOGI("External LDO has been configured");
    ext_ldo_ = true;
  }
  return true;
}

int PowerManager :: Ioctl(int fd, int val)
{
  int ret = ioctl(fd, power_ctrl_cmd_, (unsigned long)val);

  if (ret < 0)
    ALOGE(" ioctl failed to power control:%d error =(%s)", ret, strerror(errno)); ;

  return ret;
}

void PowerManager :: Cleanup(int fd)
{
  close(fd);
  Cleanup();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
