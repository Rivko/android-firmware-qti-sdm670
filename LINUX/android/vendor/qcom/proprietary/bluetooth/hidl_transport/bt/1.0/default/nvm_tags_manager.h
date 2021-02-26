/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *
 *  Copyright 2012 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you
 *  may not use this file except in compliance with the License. You may
 *  obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

#pragma once

#include <hidl/HidlSupport.h>
#include <hci_mct_transport.h>
#include <logger.h>

/* Vendor Specific command codes */
#define BT_QSOC_EDL_CMD_OPCODE             (0xFC00)
#define BT_QSOC_NVM_ACCESS_OPCODE          (0xFC0B)

/* NVM */
#define MCT_MAX_TAG_CMD                      10
#define MCT_TAG_NUM_OFFSET                   4
#define MCT_TAG_BDADDR_OFFSET                6

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

/******************************************************************************
**  Local type definitions
******************************************************************************/

class NvmTagsManager {
 public:
  NvmTagsManager(HciMctTransport* transport);
  int SocInit();

 protected:
  int DownloadNvmTags(uint8_t *bdaddr);
  int HciReset(void);
  int HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size);
  int ReadVsHciEvent(unsigned char* buf, int size);
  int ReadHciEvent(unsigned char* buf, int size);

 private:
  HciMctTransport* mct_transport_;
  int fd_transport_;
  uint8_t vnd_local_bd_addr_[6];
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
