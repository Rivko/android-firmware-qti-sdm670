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

#include "patch_dl_manager.h"
#include "nvm_tags_manager.h"
#include "logger.h"

#include "bluetooth_address.h"

#define LOG_TAG "vendor.qti.bluetooth@1.0-nvm_tags_manager"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

NvmTagsManager::NvmTagsManager(HciMctTransport* transport) :
  mct_transport_(transport)
{
  fd_transport_ = mct_transport_->GetCtrlFd();
}

int NvmTagsManager::SocInit()
{
  int err = 0;
  BluetoothAddress::GetLocalAddress(vnd_local_bd_addr_);
  /* NVM download */
  err = DownloadNvmTags(vnd_local_bd_addr_);
  if ( err < 0 ) {
      ALOGE("Downloading NVM Failed !!");
      return err;
  }
  err = HciReset();
  if (err < 0) {
      ALOGE("HCI Reset Failed !!");
      return err;
  }
  return err;
}

int NvmTagsManager::DownloadNvmTags(uint8_t *bdaddr)
{
  int i, size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  unsigned char cmds[MCT_MAX_TAG_CMD][HCI_MAX_CMD_SIZE] =
  {
    /* Tag 2 */ /* BD Address */
    { /* Opcode */       0x0b,0xfc,
      /* Total Len */    9,
      /* NVM CMD */      NVM_ACCESS_SET,
      /* Tag Num */      2,
      /* Tag Len */      6,
      /* Tag Value */    0x77, 0x78, 0x23, 0x01, 0x56, 0x22
    },
    /* Tag 95 */ /* Bluetooth Local Features Control */
    { /* Opcode */       0x0b,0xfc,
      /* Total Len */    0x05,
      /* NVM CMD */      NVM_ACCESS_SET,
      /* Tag Num */      0x5F,
      /* Tag Len */      0x02,
      /* Tag Value */    0x00, 0x0B
    },
    /* Tag 83 */ /* LE Tx Power Configuration */
    { /* Opcode */       0x0b,0xfc,
      /* Total Len */    0x06,
      /* NVM CMD */      NVM_ACCESS_SET,
      /* Tag Num */      0x53,
      /* Tag Len */      0x03,
      /* Tag Value */    0x07, 0x07, 0x07
    },
    /* Tag 36 */ /* External Power Configuration */
    { /* Opcode */       0x0b,0xfc,
      /* Total Len */    0x0F,
      /* NVM CMD */      NVM_ACCESS_SET,
      /* Tag Num */      0x24,
      /* Tag Len */      0x0C,
      /* Tag Value */    0xFF, 0x03, 0x07, 0x09, 0x09, 0x09, 0x00, 0x00,
                         0x09, 0x09, 0x04, 0x00
    },
    {TAG_END}
  };

  for (i = 0; (i < MCT_MAX_TAG_CMD) && (cmds[i][0] != TAG_END); i++) {
    /* Write BD Address */
    ALOGD("%s: Downloading NVM TAG %d", __func__, cmds[i][MCT_TAG_NUM_OFFSET]);
    if (cmds[i][MCT_TAG_NUM_OFFSET] == TAG_NUM_2) {
      memcpy(&cmds[i][MCT_TAG_BDADDR_OFFSET], bdaddr, 6);
      ALOGI("BD Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
            cmds[i][MCT_TAG_BDADDR_OFFSET ], cmds[i][MCT_TAG_BDADDR_OFFSET + 1],
            cmds[i][MCT_TAG_BDADDR_OFFSET + 2], cmds[i][MCT_TAG_BDADDR_OFFSET + 3],
            cmds[i][MCT_TAG_BDADDR_OFFSET + 4], cmds[i][MCT_TAG_BDADDR_OFFSET + 5]);
    }
    size = cmds[i][2] + HCI_COMMAND_HDR_SIZE;
    /* Send HCI Command packet to Controller */
    err = HciSendVsCmd((unsigned char*)&cmds[i][0], rsp, size);
    if ( err != size) {
      ALOGE("Failed to Send NVM tag to Controller!");
      return err;
    }
    /* Read Command Complete Event */
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to receive event", __func__);
      break;
    } else if (rsp[0] != EVT_CMD_COMPLETE) {
      ALOGE("%s: Unexpected event %d", __func__, rsp[0]);
      err = -1;
      break;
    } else {
      ALOGD("%s: Downloaded NVM TAG %d", __func__,
                    cmds[i][MCT_TAG_NUM_OFFSET]);
    }
  }
  return err;
}

int NvmTagsManager::HciReset()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  ALOGI("%s: HCI RESET ", __func__);

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd);
  cmd_hdr->opcode = HCI_RESET;
  cmd_hdr->plen   = 0;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_COMMAND_HDR_SIZE);

  err = mct_transport_->Write(HCI_PACKET_TYPE_COMMAND, cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    err = -1;
    goto error;
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to receive event", __func__);
  } else if (rsp[0] != EVT_CMD_COMPLETE) {
    ALOGE("%s: Unexpected event %d", __func__, rsp[0]);
    err = -1;
  } else {
    ALOGI("%s: HCI RESET Done", __func__);
  }
error:
  return err;
}

int NvmTagsManager::ReadHciEvent(unsigned char* buf, int size)
{
  int retval;
  unsigned char hdr[BT_EVT_HDR_SIZE];
  unsigned char packet_len;
  unsigned short tot_len;

  if (size < 0) {
    ALOGE("%s: Invalid size: %d\n", __func__, size);
    return -1;
  }

  retval = mct_transport_->Read(hdr, BT_EVT_HDR_SIZE);
  if (retval < 0) {
    ALOGE("%s:error in reading hdr: %d", __func__, retval);
    return -1;
  }
  packet_len = hdr[BT_EVT_HDR_LEN_OFFSET];

  memcpy(buf, hdr, BT_EVT_HDR_SIZE);
  retval = mct_transport_->Read(buf + BT_EVT_HDR_SIZE, packet_len);
  if (retval < 0) {
    ALOGE("%s:error in reading buf: %d", __func__, retval);
    retval = -1;
    return retval;
  }
  tot_len = packet_len + BT_EVT_HDR_SIZE;
  ALOGV("read scucesssssfully payload: tot_len: %d", tot_len);
  return tot_len;
}

int NvmTagsManager::ReadVsHciEvent(unsigned char* buf, int size)
{
  int tot_len = -1;

  tot_len = ReadHciEvent(buf, size);
  if (tot_len < 0) {
    ALOGE("%s: Error while reading the hci event", __func__);
  } else if (buf[0] != LOG_BT_EVT_VENDOR_SPECIFIC) {
    ALOGE("%s: Unexpected event %d", __func__, buf[0]);
    tot_len = -1;
  } else if (buf[2] != NVM_ACCESS_CODE) {
    ALOGE("%s: Invalid NVM access code", __func__);
    tot_len = -1;
  }
  return tot_len;
}

int NvmTagsManager::HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;
  /* Send the HCI command packet to UART for transmission */
  ret = mct_transport_->Write(HCI_PACKET_TYPE_COMMAND, cmd, size);
  if (ret != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
  } else if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
    ret = -1;
    ALOGE("%s: Failed to get HCI-VS Event from SOC", __func__);
  }
  return ret;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
