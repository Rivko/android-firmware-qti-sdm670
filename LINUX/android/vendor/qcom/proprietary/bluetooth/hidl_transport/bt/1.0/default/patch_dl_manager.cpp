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

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <string.h>
#include <asm-generic/ioctls.h>
#include <hidl/HidlSupport.h>
#include <patch_dl_manager.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <termios.h>
#include "logger.h"
#include "ibs_handler.h"

#include "bluetooth_address.h"
#define UNUSED(x) (void)(x)
#undef LOG_TAG
#define LOG_TAG "vendor.qti.bluetooth@1.0-patch_dl_manager"

#define RESERVED(p)  if (p) ALOGI( "%s: reserved param", __func__);

#define BT_VERSION_FILEPATH "/data/vendor/bluetooth/bt_fw_version.txt"
#define MSB_NIBBLE_MASK 0xF0
#define LSB_NIBBLE_MASK 0x0F
#define MAX_RETRY_COUNT 5
namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

PatchDLManager::PatchDLManager(BluetoothSocType soc_type, HciUartTransport* transport) :
  soc_type_(soc_type), uart_transport_(transport), dnld_fd_in_progress_(-1)
{
  ALOGD("%s", __func__);
  bt_logger_ = Logger::Get();
  chipset_ver_ = 0;
  fd_transport_ = uart_transport_->GetCtrlFd(),
  wait_vsc_evt_ = true;
  patch_dnld_pending_ = false;
  unified_hci = false;
}

PatchDLManager::~PatchDLManager()
{
  patch_dnld_pending_ = false;
  unified_hci = false;
}

bool PatchDLManager::GetBdaddrFromProperty(const char *property, uint8_t res_bd_addr[6], bool is_reverse)
{
  bool is_valid = false;
  int i = 0;
  static char bd_addr[PROPERTY_VALUE_MAX];
  char* tok = NULL;
  char *tmp_token = NULL;

  if (property_get(property, bd_addr, NULL)) {
    ALOGV("BD address read from Boot property: %s\n", bd_addr);
    tok = strtok_r(bd_addr, ":", &tmp_token);
    while (tok != NULL) {
      ALOGV("bd add [%d]: %ld ", i, strtol(tok, NULL, 16));
      if (i >= 6) {
        ALOGE("bd property of invalid length");
        is_valid = false;
        break;
      }
      if (!ValidateToken(tok)) {
        ALOGE("Invalid token in BD address");
        is_valid = false;
        break;
      }
      if (is_reverse) {
        res_bd_addr[5 - i] = strtol(tok, NULL, 16);
      } else {
        res_bd_addr[i] = strtol(tok, NULL, 16);
      }
      tok = strtok_r(NULL, ":", &tmp_token);
      i++;
    }
  }
  if (i == 6)
    is_valid = true;

  return is_valid;
}

bool PatchDLManager::ValidateToken(char* bdaddr_tok)
{
  int i = 0;
  bool ret;

  if (strlen(bdaddr_tok) != 2) {
    ret = false;
    ALOGE("Invalid token length");
  } else {
    ret = true;
    for (i = 0; i < 2; i++) {
      if ((bdaddr_tok[i] >= '0' && bdaddr_tok[i] <= '9') ||
          (bdaddr_tok[i] >= 'A' && bdaddr_tok[i] <= 'F') ||
          (bdaddr_tok[i] >= 'a' && bdaddr_tok[i] <= 'f')) {
        ret = true;
        ALOGV("%s: tok %s @ %d is good", __func__, bdaddr_tok, i);
      } else {
        ret = false;
        ALOGE("invalid character in tok: %s at ind: %d", bdaddr_tok, i);
        break;
      }
    }
  }
  return ret;
}

int PatchDLManager::CheckEmbeddedMode()
{
  int err = 0;

  wipower_flag_ = 0;
  /* Get current wipower charging status */
  if ((err = WipowerCurrentChargingStatusReq()) < 0) {
    ALOGI("%s: Wipower status req failed (0x%x)", __func__, err);
  }
  usleep(500);

  ALOGE("%s: wipower_flag_: %d", __func__, wipower_flag_);

  return wipower_flag_;
}

int PatchDLManager::PerformChipInit()
{
  int ret = 0, err = -1, flags;

  char value[PROPERTY_VALUE_MAX] = { '\0' };
  char emb_wp_mode[PROPERTY_VALUE_MAX];
  bool is_emb_wp_mode = false;
  char wipower_status[PROPERTY_VALUE_MAX];
  uint8_t bd_addr[6], nv_bd_addr[6];
  bool ignore_boot_prop = true;

  property_get("ro.bluetooth.emb_wp_mode", emb_wp_mode, "false");
  if (strcmp(emb_wp_mode, "true") == 0) {
    ALOGV("%s: ro.bluetooth.emb_wp_mode is true", __func__);
    is_emb_wp_mode = true;
  }

  BluetoothAddress::GetLocalAddress(vnd_local_bd_addr_);

  /* Workaround UART issue: Make sure RTS is flowed ON in case it was not flowed on during
cleanup due to UART issue */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    return err;
  }

  if (is_emb_wp_mode) {
    property_get("ro.bluetooth.wipower", wipower_status, "false");
    if (strcmp(wipower_status, "true") == 0) {
      CheckEmbeddedMode();
    } else {
      ALOGI("Wipower not enabled");
    }
  }

  ret = SocInit(vnd_local_bd_addr_, is_emb_wp_mode);
  if (ret < 0) {
    ALOGE("%s: SoC initialization failed: %d\n", __func__, ret);
  }

  return ret;
}

/*
 * For Hand-Off related Wipower commands, Command complete arrives first and
 * the followd with VS event
 *
 */
int PatchDLManager::HciSendWipowerVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;
  int err = 0;

  /* Send the HCI command packet to UART for transmission */
  ret = uart_transport_->UartWrite(cmd, size);
  if (ret != size) {
    ALOGE("%s: WP Send failed with ret value: %d", __func__, ret);
    goto failed;
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to charging status cmd on Controller", __func__);
    goto failed;
  }
  ALOGI("%s: WP Received HCI command complete Event from SOC", __func__);
 failed:
  return ret;
}


int PatchDLManager::WipowerCurrentChargingStatusReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
  cmd_hdr->plen     = EDL_WIP_QUERY_CHARGING_STATUS_LEN;
  cmd[4]  = EDL_WIP_QUERY_CHARGING_STATUS_CMD;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_QUERY_CHARGING_STATUS_LEN);

  ALOGD("%s: Sending EDL_WIP_QUERY_CHARGING_STATUS_CMD", __func__);
  ALOGV("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

  err = HciSendWipowerVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_WIP_QUERY_CHARGING_STATUS_CMD command!");
    goto error;
  }

  /* Check for response from the Controller */
  if (!unified_hci) {
    if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      err = -ETIMEDOUT;
      ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __func__);
      goto error;
    }

    /* Read Command Complete Event */
    if (rsp[4] > NON_WIPOWER_MODE) {
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if (err < 0) {
        ALOGE("%s: Failed to get charging status", __func__);
        goto error;
      }
    }
   }
 error:
  return err;
}

int PatchDLManager::WipowerForwardHandoffReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
  cmd_hdr->plen     = EDL_WIP_START_HANDOFF_TO_HOST_LEN;
  cmd[4]  = EDL_WIP_START_HANDOFF_TO_HOST_CMD;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_START_HANDOFF_TO_HOST_LEN);

  ALOGD("%s: Sending EDL_WIP_START_HANDOFF_TO_HOST_CMD", __func__);
  ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
  err = HciSendWipowerVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_WIP_START_HANDOFF_TO_HOST_CMD command!");
    goto error;
  }

  /* Check for response from the Controller */
  if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
    err = -ETIMEDOUT;
    ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __func__);
    goto error;
  }

 error:
  return err;
}

int PatchDLManager::PatchVerReq()
{
  int size, err = 0, i = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_PATCH_VER_REQ_CMD, 0, -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    goto error;
  } 

  while( i++ < MAX_RETRY_COUNT) {
    if((wait_vsc_evt_) && ((rsp[CMD_RSP_OFFSET] != EDL_CMD_REQ_RES_EVT) ||
        (rsp[RSP_TYPE_OFFSET] != EDL_APP_VER_RES_EVT))) {
      /* This is a workaround for an issue where an unexpected event was received */
      ALOGE("%s: Unexpected event received! Reading the next event", __func__);
      ALOGE("%s: rsp[CMD_RSP_OFFSET] = %x ,  rsp[RSP_TYPE_OFFSET] = %x", __func__,
                        rsp[CMD_RSP_OFFSET], rsp[RSP_TYPE_OFFSET]);
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    } else {
      break;
    }
  }

  /* Read Command Complete Event*/
  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to get patch version(s)", __func__);
      goto error;
    }
  }
 error:
  return err;

}

int PatchDLManager::GetBuildInfoReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_GET_BUILD_INFO, 0,
                -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send get build info cmd to the SoC!");
    goto error;
  }

  if (!unified_hci) {
    err = ReadHciEvent( rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to get build info", __func__);
      goto error;
    }
  }
 error:
  return err;

}

int PatchDLManager::GetTlvFile(const char *file_path, const char* alt_file_path)
{
  FILE * pFile = NULL;
  long fileSize;
  int readSize, err = 0, total_segment, remain_size, nvm_length, nvm_tot_len, nvm_index, i;
  unsigned short nvm_tag_len;
  tlv_patch_info *ptlv_header;
  tlv_nvm_hdr *nvm_ptr;
  unsigned char data_buf[PRINT_BUF_SIZE] = { 0, };
  unsigned char *nvm_byte_ptr;

  ALOGI("File Open (%s)", file_path);
  if (!(file_path && (pFile = fopen( file_path, "r" )))) {
      ALOGE("%s File Open Fail : %s", file_path, strerror(errno));
      //Try opening from alternate path
      if (!(alt_file_path && (pFile = fopen(alt_file_path, "r")))) {
        ALOGE("%s File Opening from alternate path Fail : %s", alt_file_path, strerror(errno));
        return -1;
      }
      ALOGI("fopen %s okay", alt_file_path);
    } else {
      ALOGI("fopen %s okay", file_path);
    }

  /* Get File Size */
  fseek(pFile, 0, SEEK_END);
  fileSize = ftell(pFile);
  rewind(pFile);

  pdata_buffer_ = (unsigned char*)new char[fileSize];
  if (pdata_buffer_ == NULL) {
    ALOGE("Allocated Memory failed");
    fclose(pFile);
    return -1;
  }

  /* Copy file into allocated buffer */
  readSize = fread(pdata_buffer_, 1, fileSize, pFile);

  /* File Close */
  fclose(pFile);

  if (readSize != fileSize) {
    ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)", readSize, fileSize);
    return -1;
  }

  ptlv_header = (tlv_patch_info*)pdata_buffer_;

  /* To handle different event between rampatch and NVM */
  tlv_type_ = ptlv_header->tlv_type;
  tlv_dwn_cfg_ = ptlv_header->tlv.patch.dwnd_cfg;

  if (ptlv_header->tlv_type == TLV_TYPE_PATCH) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    ALOGI("Length\t\t\t : %d bytes", (ptlv_header->tlv_length1) |
          (ptlv_header->tlv_length2 << 8) |
          (ptlv_header->tlv_length3 << 16));
    ALOGI("Total Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_data_len);
    ALOGI("Patch Data Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_patch_data_len);
    ALOGI("Signing Format Version\t : 0x%x", ptlv_header->tlv.patch.sign_ver);
    ALOGI("Signature Algorithm\t\t : 0x%x", ptlv_header->tlv.patch.sign_algorithm);
    ALOGI("Event Handling\t\t\t : 0x%x", ptlv_header->tlv.patch.dwnd_cfg);
    ALOGI("Reserved\t\t\t : 0x%x", ptlv_header->tlv.patch.reserved1);
    ALOGI("Product ID\t\t\t : 0x%04x\n", ptlv_header->tlv.patch.prod_id);
    ALOGI("Rom Build Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.build_ver);
    ALOGI("Patch Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.patch_ver);
    ALOGI("Reserved\t\t\t : 0x%x\n", ptlv_header->tlv.patch.reserved2);
    ALOGI("Patch Entry Address\t\t : 0x%x\n", (ptlv_header->tlv.patch.patch_entry_addr));
    ALOGI("====================================================");

  } else if ( (ptlv_header->tlv_type >= TLV_TYPE_BT_NVM) &&
              ((ptlv_header->tlv_type <= TLV_TYPE_BT_FM_NVM)) ) {
    ALOGI("====================================================");
    ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
    nvm_tot_len = nvm_length  = (ptlv_header->tlv_length1) |
                                (ptlv_header->tlv_length2 << 8) |
                                (ptlv_header->tlv_length3 << 16);
    ALOGI("Length\t\t\t : %d bytes",  nvm_tot_len);
    ALOGI("====================================================");

    if (nvm_tot_len <= 0)
      return readSize;

    if (ptlv_header->tlv_type == TLV_TYPE_BT_FM_NVM) {
      nvm_byte_ptr = (unsigned char*)ptlv_header;
      nvm_byte_ptr += 4;
      ptlv_header = (tlv_patch_info*)nvm_byte_ptr;
      nvm_tot_len -= 4;
      ALOGI("====================================================");
      ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
      nvm_length = (ptlv_header->tlv_length1) |
                   (ptlv_header->tlv_length2 << 8) |
                   (ptlv_header->tlv_length3 << 16);
      ALOGI("Length\t\t\t : %d bytes",  nvm_length);
      ALOGI("====================================================");
    }

 multi_nvm:
    for (nvm_byte_ptr = (unsigned char*)(nvm_ptr = &(ptlv_header->tlv.nvm)), nvm_index = 0;
         nvm_index < nvm_length; nvm_ptr = (tlv_nvm_hdr*)nvm_byte_ptr) {
      ALOGV("TAG ID\t\t\t : %d", nvm_ptr->tag_id);
      nvm_tag_len = nvm_ptr->tag_len;
      ALOGV("TAG Length\t\t : %d", nvm_tag_len);
      ALOGV("TAG Pointer\t\t : %d", nvm_ptr->tag_ptr);
      ALOGV("TAG Extended Flag\t : %d", nvm_ptr->tag_ex_flag);

      /* Increase nvm_index to NVM data */
      nvm_index += sizeof(tlv_nvm_hdr);
      nvm_byte_ptr += sizeof(tlv_nvm_hdr);
      if (ptlv_header->tlv_type == TLV_TYPE_BT_NVM) {
        /* Write BD Address */
        if (nvm_ptr->tag_id == TAG_NUM_2) {
          memcpy(nvm_byte_ptr, vnd_local_bd_addr_, 6);
          ALOGI("BD Address: %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",
                *nvm_byte_ptr, *(nvm_byte_ptr + 1), *(nvm_byte_ptr + 2),
                *(nvm_byte_ptr + 3), *(nvm_byte_ptr + 4), *(nvm_byte_ptr + 5));
        }

        if (IS_NAPIER(chipset_ver_)) {
          UpdateNapierNvm(nvm_ptr);
        } else {
          /* Change SIBS setting */
          if (!IsSibsEnabled()) {
            if (nvm_ptr->tag_id == 17) {
              *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x80));
            }

            if (nvm_ptr->tag_id == 27) {
              *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x01));
            }
          }

          /* Change Baudrate setting based on Host setting */
          if (nvm_ptr->tag_id == 17) {
            *(nvm_byte_ptr + 2) = uart_transport_->GetMaxBaudrate();
          }
        }
      }
      for (i = 0; (i < nvm_ptr->tag_len && (i * 3 + 2) < PRINT_BUF_SIZE); i++)
        snprintf((char*)data_buf, PRINT_BUF_SIZE, "%s%.02x ", (char*)data_buf, *(nvm_byte_ptr + i));

      ALOGV("TAG Data\t\t\t : %s", data_buf);

      /* Clear buffer */
      memset(data_buf, 0x0, PRINT_BUF_SIZE);

      /* increased by tag_len */
      nvm_index += nvm_ptr->tag_len;
      nvm_byte_ptr += nvm_ptr->tag_len;
    }

    nvm_tot_len -= nvm_index;

    if (nvm_tot_len > 4 ) {
      nvm_byte_ptr = (unsigned char*)ptlv_header;
      nvm_byte_ptr += (4 + nvm_index);
      ptlv_header = (tlv_patch_info*)nvm_byte_ptr;
      nvm_tot_len -= 4;
      ALOGI("====================================================");
      ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
      nvm_length = (ptlv_header->tlv_length1) |
                   (ptlv_header->tlv_length2 << 8) |
                   (ptlv_header->tlv_length3 << 16);
      ALOGI("Length\t\t\t : %d bytes",  nvm_length);
      ALOGI("====================================================");
      goto multi_nvm;
    }

    ALOGI("====================================================");

  } else {
    ALOGI("TLV Header type is unknown (%d) ", ptlv_header->tlv_type);
  }

  return readSize;
}

int PatchDLManager::TlvDnldSegment(int index, int seg_size, unsigned char wait_cc_evt)
{
  int size = 0, err = -1;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  ALOGV("%s: Downloading TLV Patch segment no.%d, size:%d", __func__, index, seg_size);

  /* Frame the HCI CMD PKT to be sent to Controller*/
  FrameHciPkt(cmd, EDL_PATCH_TLV_REQ_CMD, 0, index, seg_size);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

  /* Initialize the RSP packet everytime to 0 */
  memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);

  if ( err != size) {
    ALOGE("Failed to send the patch payload to the Controller! 0x%x", err);
    return err;
  }

  if (!unified_hci) {
    if (wait_cc_evt) {
      err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if ( err < 0) {
        ALOGE("%s: Failed to downlaod patch segment: %d!",  __func__, index);
        return err;
      }
    }
  }
  ALOGV("%s: Successfully downloaded patch segment: %d", __func__, index);
  return err;
}

int PatchDLManager::TlvDnldReq(int tlv_size)
{
  int total_segment, remain_size, i, err = -1;
  unsigned char wait_cc_evt = true;

  total_segment = tlv_size / MAX_SIZE_PER_TLV_SEGMENT;
  remain_size = (tlv_size < MAX_SIZE_PER_TLV_SEGMENT) ? \
                tlv_size : (tlv_size % MAX_SIZE_PER_TLV_SEGMENT);

  ALOGI("%s: TLV size: %d, Total Seg num: %d, remain size: %d",
        __func__, tlv_size, total_segment, remain_size);

  if (tlv_type_ == TLV_TYPE_PATCH) {
    /* Prior to Rome version 3.2(including inital few rampatch release of Rome 3.2), the event
     * handling mechanism is SKIP_EVT_NONE. After few release of rampatch for Rome 3.2, the
     * mechamism is changed to SKIP_EVT_VSE_CC. Rest of the mechanism is not used for now
     */
    switch (tlv_dwn_cfg_) {
      case SKIP_EVT_NONE:
        wait_vsc_evt_ = true;
        wait_cc_evt = true;
        ALOGI("Event handling type: SKIP_EVT_NONE");
        break;
      case SKIP_EVT_VSE_CC:
        wait_vsc_evt_ = false;
        wait_cc_evt = false;
        ALOGI("Event handling type: SKIP_EVT_VSE_CC");
        break;
      /* Not handled for now */
      case SKIP_EVT_VSE:
      case SKIP_EVT_CC:
      default:
        ALOGE("Unsupported Event handling: %d", tlv_dwn_cfg_);
        break;
    }
  } else {
    wait_vsc_evt_ = true;
    wait_cc_evt = true;
  }

  for (i = 0; i < total_segment; i++) {
    if ((i + 1) == total_segment) {
      if (tlv_type_ == TLV_TYPE_PATCH) {
        /*
         * 1. None of the command segments receive CCE
         * 2. No command segments receive VSE except the last one
         * 3. If tlv_dwn_cfg_ is SKIP_EVT_NONE then wait for everything
         */
        if (tlv_dwn_cfg_ == SKIP_EVT_NONE) {
          wait_cc_evt = !remain_size ? false : true;
        } else if (tlv_dwn_cfg_ == SKIP_EVT_VSE_CC) {
          wait_vsc_evt_ = !remain_size ? true : false;
        }
      }
    }

    patch_dnld_pending_ = true;
    if ((err = TlvDnldSegment(i, MAX_SIZE_PER_TLV_SEGMENT, wait_cc_evt )) < 0)
      goto error;
    patch_dnld_pending_ = false;
  }

  if (tlv_type_ == TLV_TYPE_PATCH) {
    /*
     * 1. None of the command segments receive CCE
     * 2. No command segments receive VSE except the last one
     * 3. If tlv_dwn_cfg_ is SKIP_EVT_NONE then the logic is
     *    same as Rome 2.1, 2.2, 3.0
     */
    if (tlv_dwn_cfg_ == SKIP_EVT_NONE) {
      wait_cc_evt = remain_size ? false : true;
    } else if (tlv_dwn_cfg_ == SKIP_EVT_VSE_CC) {
      wait_vsc_evt_ = remain_size ? true : false;
    }
  }

  patch_dnld_pending_ = true;
  if (remain_size) err = TlvDnldSegment( i, remain_size, wait_cc_evt);
  patch_dnld_pending_ = false;
 error:
  if (patch_dnld_pending_)
    patch_dnld_pending_ = false;
  return err;
}

void PatchDLManager::RomeExtractPatchHeaderInfo(unsigned char *buf)
{
  int index;

  /* Extract patch id */
  for (index = 0; index < 4; index++)
    rampatch_patch_info_.patch_id |=
      (LSH(buf[index + P_ID_OFFSET], (index * 8)));

  /* Extract (ROM and BUILD) version information */
  for (index = 0; index < 2; index++)
    rampatch_patch_info_.patch_ver.rom_version |=
      (LSH(buf[index + P_CHIP_VER_OFFSET], (index * 8)));

  for (index = 0; index < 2; index++)
    rampatch_patch_info_.patch_ver.build_version |=
      (LSH(buf[index + P_BUILD_VER_OFFSET], (index * 8)));

  /* Extract patch base and entry addresses */
  for (index = 0; index < 4; index++)
    rampatch_patch_info_.patch_base_addr |=
      (LSH(buf[index + P_BASE_ADDR_OFFSET], (index * 8)));

  /* Patch BASE & ENTRY addresses are same */
  rampatch_patch_info_.patch_entry_addr = rampatch_patch_info_.patch_base_addr;

  /* Extract total length of the patch payload */
  for (index = 0; index < 4; index++)
    rampatch_patch_info_.patch_length |=
      (LSH(buf[index + P_LEN_OFFSET], (index * 8)));

  /* Extract the CRC checksum of the patch payload */
  for (index = 0; index < 4; index++)
    rampatch_patch_info_.patch_crc |=
      (LSH(buf[index + P_CRC_OFFSET], (index * 8)));

  /* Extract patch control value */
  for (index = 0; index < 4; index++)
    rampatch_patch_info_.patch_ctrl |=
      (LSH(buf[index + P_CONTROL_OFFSET], (index * 8)));

  ALOGI("PATCH_ID\t : 0x%x", rampatch_patch_info_.patch_id);
  ALOGI("ROM_VERSION\t : 0x%x", rampatch_patch_info_.patch_ver.rom_version);
  ALOGI("BUILD_VERSION\t : 0x%x", rampatch_patch_info_.patch_ver.build_version);
  ALOGI("PATCH_LENGTH\t : 0x%x", rampatch_patch_info_.patch_length);
  ALOGI("PATCH_CRC\t : 0x%x", rampatch_patch_info_.patch_crc);
  ALOGI("PATCH_CONTROL\t : 0x%x\n", rampatch_patch_info_.patch_ctrl);
  ALOGI("PATCH_BASE_ADDR\t : 0x%x\n", rampatch_patch_info_.patch_base_addr);

}

int PatchDLManager::RomeEdlSetPatchRequest(int fd)
{
  int size, err;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  //UNUSED(fd);

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciCmdPkt(cmd, EDL_PATCH_SET_REQ_CMD, 0,
                    -1, PATCH_HDR_LEN + 1);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd( (unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to set the patch info to the Controller!");
    goto error;
  }

  err = ReadHciEvent( rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to set patch info on Controller", __func__);
    goto error;
  }
  ALOGI("%s: Successfully set patch info on the Controller", __func__);
 error:
  return err;
}

int PatchDLManager::RomeEdlPatchDownloadRequest()
{
  int no_of_patch_segment;
  int index = 1, err = 0, size = 0;
  unsigned int p_base_addr;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  no_of_patch_segment = (rampatch_patch_info_.patch_length /
                         MAX_DATA_PER_SEGMENT);
  ALOGI("%s: %d patch segments to be d'loaded from patch base addr: 0x%x",
        __func__, no_of_patch_segment,
        rampatch_patch_info_.patch_base_addr);

  /* Initialize the patch base address from the one read from bin file */
  p_base_addr = rampatch_patch_info_.patch_base_addr;

  /*
   * Depending upon size of the patch payload, download the patches in
   * segments with a max. size of 239 bytes
   */
  for (index = 1; index <= no_of_patch_segment; index++) {

    ALOGI("%s: Downloading patch segment: %d", __func__, index);

    /* Frame the HCI CMD PKT to be sent to Controller*/
    FrameHciPkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr,
                  index, MAX_DATA_PER_SEGMENT);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Initialize the RSP packet everytime to 0 */
    memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

    /* Send HCI Command packet to Controller */
    err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
    if ( err != size) {
      ALOGE("Failed to send the patch payload to the Controller!");
      goto error;
    }

    /* Read Command Complete Event */
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to downlaod patch segment: %d!", __func__, index);
      goto error;
    }
    ALOGI("%s: Successfully downloaded patch segment: %d", __func__, index);
  }

  /* Check if any pending patch data to be sent */
  size = (rampatch_patch_info_.patch_length < MAX_DATA_PER_SEGMENT) ?
         rampatch_patch_info_.patch_length :
         (rampatch_patch_info_.patch_length  % MAX_DATA_PER_SEGMENT);

  if (size) {
    /* Frame the HCI CMD PKT to be sent to Controller*/
    FrameHciPkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr, index, size);

    /* Initialize the RSP packet everytime to 0 */
    memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Send HCI Command packet to Controller */
    err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
    if ( err != size) {
      ALOGE("Failed to send the patch payload to the Controller!");
      goto error;
    }

    /* Read Command Complete Event */
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to downlaod patch segment: %d!",
            __func__, index);
      goto error;
    }

    ALOGI("%s: Successfully downloaded patch segment: %d",
          __func__, index);
  }

 error:
  return err;
}

int PatchDLManager::RomeDownloadRampatch()
{
  int c, tmp, size, index, ret = -1;

  /* Get handle to the RAMPATCH binary file */
  ALOGI("%s: Getting handle to the RAMPATCH binary file from %s", __func__, ROME_FW_PATH);
  FILE* file = fopen(ROME_FW_PATH, "r");
  if (file == NULL) {
    ALOGE("%s: Failed to get handle to the RAMPATCH bin file!",
          __func__);
    return -ENFILE;
  }

  /* Allocate memory for the patch headder info */
  ALOGI("%s: Allocating memory for the patch header", __func__);
  phdr_buffer_ = new unsigned char[PATCH_HDR_LEN + 1];
  if (phdr_buffer_ == NULL) {
    ALOGE("%s: Failed to allocate memory for patch header",
          __func__);
    goto phdr_alloc_failed;
  }
  for (index = 0; index < PATCH_HDR_LEN + 1; index++)
    phdr_buffer_[index] = 0x0;

  /* Read 28 bytes of patch header information */
  ALOGI("%s: Reading patch header info", __func__);
  index = 0;
  do {
    c = fgetc(file);
    phdr_buffer_[index++] = (unsigned char)c;
  } while (index != PATCH_HDR_LEN);

  /* Save the patch header info into local structure */
  ALOGI("%s: Saving patch hdr. info", __func__);
  RomeExtractPatchHeaderInfo((unsigned char*)phdr_buffer_);

  /* Set the patch header info onto the Controller */
  ret = RomeEdlSetPatchRequest(fd_transport_);
  if (ret < 0) {
    ALOGE("%s: Error setting the patchheader info!", __func__);
    goto pdata_alloc_failed;
  }

  /* Allocate memory for the patch payload */
  ALOGI("%s: Allocating memory for patch payload", __func__);
  size = rampatch_patch_info_.patch_length;
  pdata_buffer_ = new unsigned char[size + 1];
  if (pdata_buffer_ == NULL) {
    ALOGE("%s: Failed to allocate memory for patch payload",
          __func__);
    goto pdata_alloc_failed;
  }
  for (index = 0; index < size + 1; index++)
    pdata_buffer_[index] = 0x0;

  /* Read the patch data from Rampatch binary image */
  ALOGI("%s: Reading patch payload from RAMPATCH file", __func__);
  index = 0;
  do {
    c = fgetc(file);
    pdata_buffer_[index++] = (unsigned char)c;
  } while (c != EOF);

  /* Downloading patches in segments to controller */
  ret = RomeEdlPatchDownloadRequest();
  if (ret < 0) {
    ALOGE("%s: Error downloading patch segments!", __func__);
  }

  delete []pdata_buffer_;
 pdata_alloc_failed:
  delete []phdr_buffer_;
 phdr_alloc_failed:
  fclose(file);
 error:
  return ret;
}

int PatchDLManager::RomeAttachRampatch()
{
  int size, err;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciPkt(cmd, EDL_PATCH_ATCH_REQ_CMD, 0,
                -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

  /* Send HCI Command packet to Controller */
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to attach the patch payload to the Controller!");
    goto error;
  }

  /* Read Command Complete Event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to attach the patch segment(s)", __func__);
    goto error;
  }
 error:
  return err;
}

int PatchDLManager::RomeRampatchReset()
{
  int size, err = 0, flags;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  struct timespec tm = { 0, 100 * 1000 * 1000 }; /* 100 ms */

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciCmdPkt(cmd, EDL_PATCH_RST_REQ_CMD, 0,
                    -1, EDL_PATCH_CMD_LEN);

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  /* Send HCI Command packet to Controller */
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    goto error;
  }

  /*
   * Controller doesn't sends any response for the patch reset
   * command. HOST has to wait for 100ms before proceeding.
   */
  nanosleep(&tm, NULL);

 error:
  return err;
}
int PatchDLManager::RomeHciResetReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  ALOGV("%s: HCI RESET ", __func__);

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = HCI_RESET;
  cmd_hdr->plen   = 0;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);

  /* Flow off during baudrate change */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-off error: 0x%x\n", __func__, err);
    return err;
  }

  /* Send the HCI command packet to UART for transmission */
  ALOGI("%s: HCI CMD: 0x%x 0x%x 0x%x 0x%x\n", __func__, cmd[0], cmd[1], cmd[2], cmd[3]);
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    return err;;
  }

  /* Change Local UART baudrate to high speed UART */
  uart_transport_->SetBaudRate(USERIAL_BAUD_3M);

  /* Flow on after changing local uart baudrate */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    return err;
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to set patch info on Controller", __func__);
  }

  return err;

}

int PatchDLManager::Rome10NvmTagDnld(uint8_t *bdaddr)
{
  int i, size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

#if (NVM_VERSION >= ROME_1_0_100019)
  unsigned char cmds[MAX_TAG_CMD][HCI_MAX_CMD_SIZE] =
  {
    /* Tag 2 */ /* BD Address */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     9,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     2,
      /* Tag Len */      6,
      /* Tag Value */   0x77,0x78,0x23,0x01,0x56,0x22
    },
    /* Tag 6 */ /* Bluetooth Support Features */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     11,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     6,
      /* Tag Len */      8,
      /* Tag Value */   0xFF,0xFE,0x8B,0xFE,0xD8,0x3F,0x5B,0x8B
    },
    /* Tag 17 */ /* HCI Transport Layer Setting */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     11,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     17,
      /* Tag Len */      8,
      /* Tag Value */   0x82,0x01,0x0E,0x08,0x04,0x32,0x0A,0x00
    },
    /* Tag 35 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     58,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     35,
      /* Tag Len */      55,
      /* Tag Value */   0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x58, 0x59,
      0x0E, 0x0E, 0x16, 0x16, 0x16, 0x1E, 0x26, 0x5F, 0x2F, 0x5F,
      0x0E, 0x0E, 0x16, 0x16, 0x16, 0x1E, 0x26, 0x5F, 0x2F, 0x5F,
      0x0C, 0x18, 0x14, 0x24, 0x40, 0x4C, 0x70, 0x80, 0x80, 0x80,
      0x0C, 0x18, 0x14, 0x24, 0x40, 0x4C, 0x70, 0x80, 0x80, 0x80,
      0x1B, 0x14, 0x01, 0x04, 0x48
    },
    /* Tag 36 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     15,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     36,
      /* Tag Len */      12,
      /* Tag Value */   0x0F,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x04,0x00
    },
    /* Tag 39 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     7,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     39,
      /* Tag Len */      4,
      /* Tag Value */   0x12,0x00,0x00,0x00
    },
    /* Tag 41 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     91,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     41,
      /* Tag Len */      88,
      /* Tag Value */   0x15, 0x00, 0x00, 0x00, 0xF6, 0x02, 0x00, 0x00, 0x76, 0x00,
      0x1E, 0x00, 0x29, 0x02, 0x1F, 0x00, 0x61, 0x00, 0x1A, 0x00,
      0x76, 0x00, 0x1E, 0x00, 0x7D, 0x00, 0x40, 0x00, 0x91, 0x00,
      0x06, 0x00, 0x92, 0x00, 0x03, 0x00, 0xA6, 0x01, 0x50, 0x00,
      0xAA, 0x01, 0x15, 0x00, 0xAB, 0x01, 0x0A, 0x00, 0xAC, 0x01,
      0x00, 0x00, 0xB0, 0x01, 0xC5, 0x00, 0xB3, 0x01, 0x03, 0x00,
      0xB4, 0x01, 0x13, 0x00, 0xB5, 0x01, 0x0C, 0x00, 0xC5, 0x01,
      0x0D, 0x00, 0xC6, 0x01, 0x10, 0x00, 0xCA, 0x01, 0x2B, 0x00,
      0xCB, 0x01, 0x5F, 0x00, 0xCC, 0x01, 0x48, 0x00
    },
    /* Tag 42 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     63,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     42,
      /* Tag Len */      60,
      /* Tag Value */   0xD7, 0xC0, 0x00, 0x00, 0x8F, 0x5C, 0x02, 0x00, 0x80, 0x47,
      0x60, 0x0C, 0x70, 0x4C, 0x00, 0x00, 0x00, 0x01, 0x1F, 0x01,
      0x42, 0x01, 0x69, 0x01, 0x95, 0x01, 0xC7, 0x01, 0xFE, 0x01,
      0x3D, 0x02, 0x83, 0x02, 0xD1, 0x02, 0x29, 0x03, 0x00, 0x0A,
      0x10, 0x00, 0x1F, 0x00, 0x3F, 0x00, 0x7F, 0x00, 0xFD, 0x00,
      0xF9, 0x01, 0xF1, 0x03, 0xDE, 0x07, 0x00, 0x00, 0x9A, 0x01
    },
    /* Tag 84 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     153,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     84,
      /* Tag Len */      150,
      /* Tag Value */   0x7C, 0x6A, 0x59, 0x47, 0x19, 0x36, 0x35, 0x25, 0x25, 0x28,
      0x2C, 0x2B, 0x2B, 0x28, 0x2C, 0x28, 0x29, 0x28, 0x29, 0x28,
      0x29, 0x29, 0x2C, 0x29, 0x2C, 0x29, 0x2C, 0x28, 0x29, 0x28,
      0x29, 0x28, 0x29, 0x2A, 0x00, 0x00, 0x2C, 0x2A, 0x2C, 0x18,
      0x98, 0x98, 0x98, 0x98, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
      0x1E, 0x13, 0x1E, 0x1E, 0x1E, 0x1E, 0x13, 0x13, 0x11, 0x13,
      0x1E, 0x1E, 0x13, 0x12, 0x12, 0x12, 0x11, 0x12, 0x1F, 0x12,
      0x12, 0x12, 0x10, 0x0C, 0x18, 0x0D, 0x01, 0x01, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x0C, 0x01, 0x01, 0x01, 0x01, 0x0D, 0x0D,
      0x0E, 0x0D, 0x01, 0x01, 0x0D, 0x0D, 0x0D, 0x0D, 0x0F, 0x0D,
      0x10, 0x0D, 0x0D, 0x0D, 0x0D, 0x10, 0x05, 0x10, 0x03, 0x00,
      0x7E, 0x7B, 0x7B, 0x72, 0x71, 0x50, 0x50, 0x50, 0x00, 0x40,
      0x60, 0x60, 0x30, 0x08, 0x02, 0x0F, 0x00, 0x01, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x08, 0x16, 0x16, 0x08, 0x08, 0x00,
      0x00, 0x00, 0x1E, 0x34, 0x2B, 0x1B, 0x23, 0x2B, 0x15, 0x0D
    },
    /* Tag 85 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     119,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     85,
      /* Tag Len */      116,
      /* Tag Value */   0x03, 0x00, 0x38, 0x00, 0x45, 0x77, 0x00, 0xE8, 0x00, 0x59,
      0x01, 0xCA, 0x01, 0x3B, 0x02, 0xAC, 0x02, 0x1D, 0x03, 0x8E,
      0x03, 0x00, 0x89, 0x01, 0x0E, 0x02, 0x5C, 0x02, 0xD7, 0x02,
      0xF8, 0x08, 0x01, 0x00, 0x1F, 0x00, 0x0A, 0x02, 0x55, 0x02,
      0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x2A, 0xD7, 0x00, 0x00,
      0x00, 0x1E, 0xDE, 0x00, 0x00, 0x00, 0x14, 0x0F, 0x0A, 0x0F,
      0x0A, 0x0C, 0x0C, 0x0C, 0x0C, 0x04, 0x04, 0x04, 0x0C, 0x0C,
      0x0C, 0x0C, 0x06, 0x06, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02,
      0x01, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00,
      0x06, 0x0F, 0x14, 0x05, 0x47, 0xCF, 0x77, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xAC, 0x7C, 0xFF, 0x40, 0x00, 0x00, 0x00,
      0x12, 0x04, 0x04, 0x01, 0x04, 0x03
    },
    {TAG_END}
  };
#elif (NVM_VERSION == ROME_1_0_6002)
  unsigned char cmds[MAX_TAG_CMD][HCI_MAX_CMD_SIZE] =
  {
    /* Tag 2 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     9,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     2,
      /* Tag Len */      6,
      /* Tag Value */   0x77,0x78,0x23,0x01,0x56,0x22 /* BD Address */
    },
    /* Tag 6 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     11,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     6,
      /* Tag Len */      8,
      /* Tag Value */   0xFF,0xFE,0x8B,0xFE,0xD8,0x3F,0x5B,0x8B
    },
    /* Tag 17 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     11,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     17,
      /* Tag Len */      8,
      /* Tag Value */   0x82,0x01,0x0E,0x08,0x04,0x32,0x0A,0x00
    },
    /* Tag 36 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     15,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     36,
      /* Tag Len */      12,
      /* Tag Value */   0x0F,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x03,0x03,0x04,0x00
    },

    /* Tag 39 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     7,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     39,
      /* Tag Len */      4,
      /* Tag Value */   0x12,0x00,0x00,0x00
    },

    /* Tag 41 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     199,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     41,
      /* Tag Len */      196,
      /* Tag Value */   0x30,0x00,0x00,0x00,0xD5,0x00,0x0E,0x00,0xD6,0x00,0x0E,0x00,
      0xD7,0x00,0x16,0x00,0xD8,0x00,0x16,0x00,0xD9,0x00,0x16,0x00,
      0xDA,0x00,0x1E,0x00,0xDB,0x00,0x26,0x00,0xDC,0x00,0x5F,0x00,
      0xDD,0x00,0x2F,0x00,0xDE,0x00,0x5F,0x00,0xE0,0x00,0x0E,0x00,
      0xE1,0x00,0x0E,0x00,0xE2,0x00,0x16,0x00,0xE3,0x00,0x16,0x00,
      0xE4,0x00,0x16,0x00,0xE5,0x00,0x1E,0x00,0xE6,0x00,0x26,0x00,
      0xE7,0x00,0x5F,0x00,0xE8,0x00,0x2F,0x00,0xE9,0x00,0x5F,0x00,
      0xEC,0x00,0x0C,0x00,0xED,0x00,0x08,0x00,0xEE,0x00,0x14,0x00,
      0xEF,0x00,0x24,0x00,0xF0,0x00,0x40,0x00,0xF1,0x00,0x4C,0x00,
      0xF2,0x00,0x70,0x00,0xF3,0x00,0x80,0x00,0xF4,0x00,0x80,0x00,
      0xF5,0x00,0x80,0x00,0xF8,0x00,0x0C,0x00,0xF9,0x00,0x18,0x00,
      0xFA,0x00,0x14,0x00,0xFB,0x00,0x24,0x00,0xFC,0x00,0x40,0x00,
      0xFD,0x00,0x4C,0x00,0xFE,0x00,0x70,0x00,0xFF,0x00,0x80,0x00,
      0x00,0x01,0x80,0x00,0x01,0x01,0x80,0x00,0x04,0x01,0x1B,0x00,
      0x05,0x01,0x14,0x00,0x06,0x01,0x01,0x00,0x07,0x01,0x04,0x00,
      0x08,0x01,0x00,0x00,0x09,0x01,0x00,0x00,0x0A,0x01,0x03,0x00,
      0x0B,0x01,0x03,0x00
    },

    /* Tag 44 */
    {  /* Packet Type */HCI_COMMAND_PKT,
      /* Opcode */       0x0b,0xfc,
      /* Total Len */     44,
      /* NVM CMD */    NVM_ACCESS_SET,
      /* Tag Num */     44,
      /* Tag Len */      41,
      /* Tag Value */   0x6F,0x0A,0x00,0x00,0x00,0x00,0x00,0x50,0xFF,0x10,0x02,0x02,
      0x01,0x00,0x14,0x01,0x06,0x28,0xA0,0x62,0x03,0x64,0x01,0x01,
      0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0xA0,0xFF,0x10,0x02,0x01,
      0x00,0x14,0x01,0x02,0x03
    },
    {TAG_END}
  };
#endif

  ALOGI("%s: Start sending NVM Tags (ver: 0x%x)", __func__, (unsigned int)NVM_VERSION);

  for (i = 0; (i < MAX_TAG_CMD) && (cmds[i][0] != TAG_END); i++) {
    /* Write BD Address */
    if (cmds[i][TAG_NUM_OFFSET] == TAG_NUM_2) {
      memcpy(&cmds[i][TAG_BDADDR_OFFSET], bdaddr, 6);
      ALOGI("BD Address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
            cmds[i][TAG_BDADDR_OFFSET ], cmds[i][TAG_BDADDR_OFFSET + 1],
            cmds[i][TAG_BDADDR_OFFSET + 2], cmds[i][TAG_BDADDR_OFFSET + 3],
            cmds[i][TAG_BDADDR_OFFSET + 4], cmds[i][TAG_BDADDR_OFFSET + 5]);
    }
    size = cmds[i][3] + HCI_COMMAND_HDR_SIZE + 1;
    /* Send HCI Command packet to Controller */
    err = HciSendVsCmd((unsigned char*)&cmds[i][0], rsp, size);
    if ( err != size) {
      ALOGE("Failed to attach the patch payload to the Controller!");
      return err;
    }

    /* Read Command Complete Event - This is extra routine for ROME 1.0. From ROM 2.0, it should be removed. */
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
      ALOGE("%s: Failed to get patch version(s)", __func__);
      break;
    }
  }

  return err;
}

void PatchDLManager::FrameHciCmdPkt(
  unsigned char *cmd,
  int edl_cmd, unsigned int p_base_addr,
  int segtNo, int size
)
{
  int offset = 0;
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);

  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
  cmd_hdr->plen   = size;
  cmd[4]      = edl_cmd;

  switch (edl_cmd) {
    case EDL_PATCH_SET_REQ_CMD:
      /* Copy the patch header info as CMD params */
      memcpy(&cmd[5], phdr_buffer_, PATCH_HDR_LEN);
      ALOGD("%s: Sending EDL_PATCH_SET_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_DLD_REQ_CMD:
      offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
      p_base_addr += offset;
      cmd_hdr->plen   = (size + 6);
      cmd[5]  = (size + 4);
      cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
      cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
      cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
      cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
      memcpy(&cmd[10], (pdata_buffer_ + offset), size);

      ALOGD("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
            __func__, size);
      ALOGD("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
            "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
            cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
      break;
    case EDL_PATCH_ATCH_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_RST_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_RESET_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_VER_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_VER_REQ_CMD", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_TLV_REQ_CMD:
      ALOGD("%s: Sending EDL_PATCH_TLV_REQ_CMD", __func__);
      /* Parameter Total Length */
      cmd[3] = size + 2;

      /* TLV Segment Length */
      cmd[5] = size;
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
      offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
      memcpy(&cmd[6], (pdata_buffer_ + offset), size);
      break;
    case EDL_GET_BUILD_INFO:
      ALOGD("%s: Sending EDL_GET_BUILD_INFO", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_GET_BOARD_ID:
      ALOGD("%s: Sending EDL_GET_BOARD_ID", __func__);
      ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    default:
      ALOGE("%s: Unknown EDL CMD !!!", __func__);
  }
}

int PatchDLManager::GetBoardIdReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  bool cmd_supported = true;

  /* Frame the HCI CMD to be sent to the Controller */
  FrameHciCmdPkt(cmd, EDL_GET_BOARD_ID, 0,
                    -1, EDL_PATCH_CMD_LEN);
  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

  ALOGI("%s: Sending EDL_GET_BOARD_ID", __func__);
  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send EDL_GET_BOARD_ID command!");
    cmd_supported = false;
  }

  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
      ALOGE("%s: Failed to get feature request", __func__);
    }
  }
  return (cmd_supported == true ? err : -1);
}

int PatchDLManager::DownloadTlvFile(const char *rampatch_file_path,
  const char *nvm_file_path, const char *rampatch_alt_file_path, const char *nvm_alt_file_path)
{
  int tlv_size = -1;
  int err = -1;
  char nvm_file_path_bid[256] = { 0, };
  char nvm_alt_file_path_bid[256] = { 0, };

  if (nvm_file_path)
    memcpy(nvm_file_path_bid, nvm_file_path, strlen(nvm_file_path) - 2);
  if (nvm_alt_file_path)
    memcpy(nvm_alt_file_path_bid, nvm_alt_file_path, strlen(nvm_alt_file_path) - 2);

  /* Rampatch TLV file Downloading */
  if ((tlv_size = GetTlvFile(rampatch_file_path, rampatch_alt_file_path)) > 0) {
    err = TlvDnldReq(tlv_size);
    delete []pdata_buffer_;
    if (err < 0) {
      return err;
    }
  } else {
    ALOGI("%s: rampatch file is not available", __func__);
  }

ALOGI("%s: Download RAMPATCH file successfully ", __func__);

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
  {
    char trial_prop[PROPERTY_VALUE_MAX];
    int value = 0;
    property_get("persist.bluetooth.csoc.cnt", trial_prop, "0");
    value = atoi(trial_prop);
    ALOGV("%s: value : %d\n", __func__, value);
    if (value > 0) {
      sprintf(trial_prop, "%d", (value = value - 1));
      property_set("persist.bluetooth.csoc.cnt", trial_prop);
      SendCrashCommand();
    }
  }
#endif      //DEBUG_CRASH_SOC_WHILE_DLD

  /* NVM TLV file Downloading */
  tlv_size = -1;
  if (soc_type_ != BT_SOC_NAPLES_UART) {
      if (GetBoardIdReq() < 0) {
        ALOGE("%s: failed to get board id(0x%x)", __func__, err);
      } else {
        strlcat(nvm_file_path_bid, (char*)board_id_, sizeof(nvm_file_path_bid));
        strlcat(nvm_alt_file_path_bid, (char*)board_id_, sizeof(nvm_alt_file_path_bid));
        if ((tlv_size = GetTlvFile(nvm_file_path_bid,nvm_alt_file_path_bid)) < 0) {
          ALOGI("%s: %s: file doesn't exist, falling back to default file", __func__, nvm_file_path_bid);
        }
      }
  }

  if (tlv_size < 0) {
    if ((tlv_size = GetTlvFile(nvm_file_path, nvm_alt_file_path)) < 0) {
      delete []pdata_buffer_;
      return err;
    }
  }

  err = TlvDnldReq(tlv_size);

  delete []pdata_buffer_;
  return err;
}

/* Get addon features that are supported by FW */
int PatchDLManager::GetAddOnFeatureList()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_VS_GET_ADDON_FEATURES_SUPPORT);
  cmd_hdr->plen     = 0x00;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);

  ALOGD("%s: Sending HCI_VS_GET_ADDON_FEATURES_SUPPORT", __func__);
  ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3]);

  err = HciSendVsCmd((unsigned char*)cmd, rsp, size);
  if ( err != size) {
    ALOGE("Failed to send HCI_VS_GET_ADDON_FEATURES_SUPPORT command!");
    goto error;
  }

  if (!unified_hci) {
    err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
      ALOGE("%s: Failed to get feature request", __func__);
      goto error;
    }
  }
 error:
  return err;
}

void PatchDLManager::EnableControllerLog()
{
  int ret = 0;
  /* VS command to enable controller logging to the HOST. By default it is disabled */
  unsigned char cmd[6] = { 0x01, 0x17, 0xFC, 0x02, 0x00, 0x00 };
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char value[PROPERTY_VALUE_MAX] = { '\0' };

#ifdef USER_DEBUG
  // value at cmd[5]: 1 - to enable, 0 - to disable
  cmd[5] = 0x01;
#else
  property_get("persist.service.bdroid.soclog", value, "false");

  // value at cmd[5]: 1 - to enable, 0 - to disable
  ret = (strcmp(value, "true") == 0) ? cmd[5] = 0x01 : 0;
  ALOGI("%s: %d", __func__, ret);
#endif

  ret = HciSendVsCmd((unsigned char*)cmd, rsp, 6);
  if (ret != 6) {
    ALOGE("%s: command failed", __func__);
  }
  if (!unified_hci) {
    ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);

    if (ret < 0) {
      ALOGE("%s: Failed to get CC for enable SoC log", __func__);
    }
  }
  return;
}

void PatchDLManager::EnableEnhControllerLog()
{
  int ret = 0;
  /* VS command to enable/disable enhanced controller logging to the HOST */
  unsigned char cmd[6] = { 0x01, 0x17, 0xFC, 0x02, 0x14, 0x00 };
  unsigned char temp_wait_vsc_evt;
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  char value[PROPERTY_VALUE_MAX] = { '\0' };

  property_get("persist.service.bdroid.soclog", value, "false");

  // value at cmd[5]: 1 - to enable, 0 - to disable
  ret = (strcmp(value, "true") == 0) ? cmd[5] = 0x01 : 0;
  ALOGI("%s: %d", __func__, ret);

  // Command Status not expected for enhanced logging
  temp_wait_vsc_evt = wait_vsc_evt_;
  wait_vsc_evt_ = false;

  ret = HciSendVsCmd((unsigned char*)cmd, rsp, 6);
  if (ret != 6) {
    ALOGE("%s: command failed", __func__);
  }
  wait_vsc_evt_ = temp_wait_vsc_evt;

  if (!unified_hci) {
    ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);

    if (ret < 0) {
     ALOGE("%s: Failed to get CC for enable enh SoC log", __func__);
    }
  }
  return;
}

int PatchDLManager::SetBaudRateReq()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;
  uint8_t bt_baud_rate = uart_transport_->GetMaxBaudrate();

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_SET_BAUDRATE_CMD_OCF);
  cmd_hdr->plen     = VSC_SET_BAUDRATE_REQ_LEN;
  cmd[4]  = bt_baud_rate;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + VSC_SET_BAUDRATE_REQ_LEN);

  /* Flow off during baudrate change */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_OFF, &flags)) < 0) {
    ALOGE("%s: HW Flow-off error: 0x%x\n", __func__, err);
    goto error;
  }

  /* Send the HCI command packet to UART for transmission */
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    goto error;
  }

  usleep(100 * 1000);

  tcdrain(fd_transport_);
  /* Change Local UART baudrate to high speed UART */
  uart_transport_->SetBaudRate(bt_baud_rate);

  /* Check current Baudrate */
  uart_transport_->GetBaudRate();

  /* Flow on after changing local uart baudrate */
  if ((err = uart_transport_->Ioctl(USERIAL_OP_FLOW_ON, &flags)) < 0) {
    ALOGE("%s: HW Flow-on error: 0x%x \n", __func__, err);
    return err;
  }

  /* Check for response from the Controller */
  if (!unified_hci) {
    if ((err = ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE)) < 0) {
      ALOGE("%s: Failed to get HCI-VS Event from SOC", __func__);
      goto error;
    }
    ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to set patch info on Controller", __func__);
    goto error;
  }

 error:
  return err;

}

int PatchDLManager::SocInit(uint8_t *bdaddr, bool is_emb_wp_mode)
{
  int err = -1, size = 0;
  const char *rampatch_file_path = NULL;
  const char *nvm_file_path = NULL;

  const char *rampatch_alt_file_path = NULL;
  const char *nvm_alt_file_path = NULL;
  ALOGI(" %s ", __func__);
  RESERVED(bdaddr);

  dnld_fd_in_progress_ = fd_transport_;


  /* If wipower charging is going on in embedded mode then start hand off req */
  if (wipower_flag_ == WIPOWER_IN_EMBEDDED_MODE && wipower_handoff_ready_ != NON_WIPOWER_MODE) {
    wipower_flag_ = 0;
    wipower_handoff_ready_ = 0;
    if ((err = WipowerForwardHandoffReq()) < 0) {
      ALOGI("%s: Wipower handoff failed (0x%x)", __func__, err);
    }
  }

  /* Get Chipset version information */
  if ((err = PatchVerReq()) < 0) {
    ALOGI("%s: Fail to get chipset Version (0x%x)", __func__, err);
    dnld_fd_in_progress_ = -1;
    return err;
  }

  ALOGI("%s: Chipset Version (0x%08x)", __func__, chipset_ver_);

  switch (chipset_ver_) {
    case CHEROKEE_VER_0_0:
    case CHEROKEE_VER_0_1:
    case CHEROKEE_VER_1_0:
      rampatch_file_path = CHEROKEE_RAMPATCH_TLV_1_0_PATH;
      nvm_file_path = CHEROKEE_NVM_TLV_1_0_PATH;

      rampatch_alt_file_path = CHEROKEE_RAMPATCH_TLV_1_0_PATH_ALT;
      nvm_alt_file_path = CHEROKEE_NVM_TLV_1_0_PATH_ALT;
      break;
    case CHEROKEE_VER_1_1:
      rampatch_file_path = CHEROKEE_RAMPATCH_TLV_1_1_PATH;
      nvm_file_path = CHEROKEE_NVM_TLV_1_1_PATH;

      rampatch_alt_file_path = CHEROKEE_RAMPATCH_TLV_1_1_PATH_ALT;
      nvm_alt_file_path = CHEROKEE_NVM_TLV_1_1_PATH_ALT;
      break;
    case CHEROKEE_VER_2_0:
    case CHEROKEE_VER_2_0_1:
      rampatch_file_path = CHEROKEE_RAMPATCH_TLV_2_0_PATH;
      nvm_file_path = CHEROKEE_NVM_TLV_2_0_PATH;

      rampatch_alt_file_path = CHEROKEE_RAMPATCH_TLV_2_0_PATH_ALT;
      nvm_alt_file_path = CHEROKEE_NVM_TLV_2_0_PATH_ALT;
      break;
    case CHEROKEE_VER_2_1:
      rampatch_file_path = CHEROKEE_RAMPATCH_TLV_2_1_PATH;
      nvm_file_path = CHEROKEE_NVM_TLV_2_1_PATH;

      rampatch_alt_file_path = CHEROKEE_RAMPATCH_TLV_2_1_PATH_ALT;
      nvm_alt_file_path = CHEROKEE_NVM_TLV_2_1_PATH_ALT;
      break;
    case CHEROKEE_VER_3_0:
      rampatch_file_path = CHEROKEE_RAMPATCH_TLV_3_0_PATH;
      nvm_file_path = CHEROKEE_NVM_TLV_3_0_PATH;

      rampatch_alt_file_path = CHEROKEE_RAMPATCH_TLV_3_0_PATH_ALT;
      nvm_alt_file_path = CHEROKEE_NVM_TLV_3_0_PATH_ALT;
      break;
    case NAPIER_VER_1_0_0:
    case NAPIER_VER_1_0_1:
      rampatch_file_path = NAPIER_RAMPATCH_TLV_1_0_PATH;
      nvm_file_path = NAPIER_NVM_TLV_1_0_PATH;

      rampatch_alt_file_path = NAPIER_RAMPATCH_TLV_1_0_PATH_ALT;
      nvm_alt_file_path = NAPIER_NVM_TLV_1_0_PATH_ALT;
      break;
    case NAPIER_VER_2_0:
      rampatch_file_path = NAPIER_RAMPATCH_TLV_2_0_PATH;
      nvm_file_path = NAPIER_NVM_TLV_2_0_PATH;

      rampatch_alt_file_path = NAPIER_RAMPATCH_TLV_2_0_PATH_ALT;
      nvm_alt_file_path = NAPIER_NVM_TLV_2_0_PATH_ALT;
      break;
    case ROME_VER_1_0:
      /* Set and Download the RAMPATCH */
      ALOGI("%s: Setting Patch Header & Downloading Patches", __func__);
      err = RomeDownloadRampatch();
      if (err < 0) {
        ALOGE("%s: DOWNLOAD RAMPATCH failed!", __func__);
        dnld_fd_in_progress_ = -1;
        return err;
      }

      ALOGI("%s: DOWNLOAD RAMPTACH complete", __func__);

      /* Attach the RAMPATCH */
      ALOGI("%s: Attaching the patches", __func__);
      err = RomeAttachRampatch();
      if (err < 0) {
        ALOGE("%s: ATTACH RAMPATCH failed!", __func__);
        dnld_fd_in_progress_ = -1;
        return err;
      }
      ALOGI("%s: ATTACH RAMPTACH complete", __func__);

      /* Send Reset */
      size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);
      err = RomeRampatchReset();
      if ( err < 0 ) {
        ALOGE("Failed to RESET after RAMPATCH upgrade!");
        dnld_fd_in_progress_ = -1;
        return err;
      }

      /* NVM download */
      ALOGI("%s: Downloading NVM", __func__);
      err = Rome10NvmTagDnld(bdaddr);
      if ( err < 0 ) {
        ALOGE("Downloading NVM Failed !!");
        dnld_fd_in_progress_ = -1;
        return err;
      }

      /* Change baud rate 115.2 kbps to 3Mbps*/
      err = RomeHciResetReq();
      if (err < 0) {
        ALOGE("HCI Reset Failed !!");
        dnld_fd_in_progress_ = -1;
        return err;
      }

      ALOGV("HCI Reset is done\n");
      dnld_fd_in_progress_ = -1;
      return err;
    case ROME_VER_1_1:
      rampatch_file_path = ROME_RAMPATCH_TLV_PATH;
      nvm_file_path = ROME_NVM_TLV_PATH;
      break;
    case ROME_VER_1_3:
      rampatch_file_path = ROME_RAMPATCH_TLV_1_0_3_PATH;
      nvm_file_path = ROME_NVM_TLV_1_0_3_PATH;
      break;
    case ROME_VER_2_1:
      rampatch_file_path = ROME_RAMPATCH_TLV_2_0_1_PATH;
      nvm_file_path = ROME_NVM_TLV_2_0_1_PATH;
      break;
    case ROME_VER_3_0:
      rampatch_file_path = ROME_RAMPATCH_TLV_3_0_0_PATH;
      nvm_file_path = ROME_NVM_TLV_3_0_0_PATH;
      fw_su_info_ = ROME_3_1_FW_SU;
      fw_su_offset_ = ROME_3_1_FW_SW_OFFSET;


      rampatch_alt_file_path = ROME_RAMPATCH_TLV_3_0_0_PATH_ALT;
      nvm_alt_file_path = ROME_NVM_TLV_3_0_0_PATH_ALT;
      break;
    case ROME_VER_3_2:
      rampatch_file_path = ROME_RAMPATCH_TLV_3_0_2_PATH;
      nvm_file_path = ROME_NVM_TLV_3_0_2_PATH;
      fw_su_info_ = ROME_3_2_FW_SU;
      fw_su_offset_ =    ROME_3_2_FW_SW_OFFSET;

      rampatch_alt_file_path = ROME_RAMPATCH_TLV_3_0_2_PATH_ALT;
      nvm_alt_file_path = ROME_NVM_TLV_3_0_2_PATH_ALT;
      break;
    case NAPLES_VER_1_0:
      rampatch_file_path = NAPLES_RAMPATCH_TLV_UART_1_0_PATH;
      nvm_file_path = NAPLES_NVM_TLV_UART_1_0_PATH;

      rampatch_alt_file_path = NAPLES_RAMPATCH_TLV_UART_1_0_PATH_ALT;
      nvm_alt_file_path = NAPLES_NVM_TLV_UART_1_0_PATH_ALT;
      break;
    case APACHE_VER_0_0_5:
      rampatch_file_path = APACHE_RAMPATCH_TLV_0_0_5_PATH;
      nvm_file_path = APACHE_NVM_TLV_0_0_5_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_0_0_5_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_0_0_5_PATH_ALT;
      break;
    case APACHE_VER_0_0_6:
      rampatch_file_path = APACHE_RAMPATCH_TLV_0_0_6_PATH;
      nvm_file_path = APACHE_NVM_TLV_0_0_6_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_0_0_6_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_0_0_6_PATH_ALT;
      break;
    case APACHE_VER_1_0_0:
      rampatch_file_path = APACHE_RAMPATCH_TLV_1_0_0_PATH;
      nvm_file_path = APACHE_NVM_TLV_1_0_0_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_1_0_0_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_1_0_0_PATH_ALT;
      break;
    case APACHE_VER_1_0_1:
      rampatch_file_path = APACHE_RAMPATCH_TLV_1_0_1_PATH;
      nvm_file_path = APACHE_NVM_TLV_1_0_1_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_1_0_1_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_1_0_1_PATH_ALT;
      break;
    case APACHE_VER_1_0_2:
      rampatch_file_path = APACHE_RAMPATCH_TLV_1_0_2_PATH;
      nvm_file_path = APACHE_NVM_TLV_1_0_2_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_1_0_2_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_1_0_2_PATH_ALT;
      break;
    case APACHE_VER_1_0_3:
      rampatch_file_path = APACHE_RAMPATCH_TLV_1_0_3_PATH;
      nvm_file_path = APACHE_NVM_TLV_1_0_3_PATH;

      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_1_0_3_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_1_0_3_PATH_ALT;
      break;
    case APACHE_VER_1_1_0:
    case APACHE_VER_1_1_1:
      rampatch_file_path = APACHE_RAMPATCH_TLV_1_1_0_PATH;
      nvm_file_path = APACHE_NVM_TLV_1_1_0_PATH;


      rampatch_alt_file_path = APACHE_RAMPATCH_TLV_1_1_0_PATH_ALT;
      nvm_alt_file_path = APACHE_NVM_TLV_1_1_0_PATH_ALT;
      break;
    case COMANCHE_VER_1_0:
      rampatch_file_path = COMANCHE_RAMPATCH_TLV_1_0_PATH;
      nvm_file_path = COMANCHE_NVM_TLV_1_0_PATH;

      rampatch_alt_file_path = COMANCHE_RAMPATCH_TLV_1_0_PATH_ALT;
      nvm_alt_file_path = COMANCHE_NVM_TLV_1_0_PATH_ALT;
      break;
    case COMANCHE_VER_1_0_1:
      rampatch_file_path = COMANCHE_RAMPATCH_TLV_1_0_1_PATH;
      nvm_file_path = COMANCHE_NVM_TLV_1_0_1_PATH;

      rampatch_alt_file_path = COMANCHE_RAMPATCH_TLV_1_0_1_PATH_ALT;
      nvm_alt_file_path = COMANCHE_NVM_TLV_1_0_1_PATH_ALT;
      break;
    case COMANCHE_VER_1_1:
      rampatch_file_path = COMANCHE_RAMPATCH_TLV_1_1_PATH;
      nvm_file_path = COMANCHE_NVM_TLV_1_1_PATH;

      rampatch_alt_file_path = COMANCHE_RAMPATCH_TLV_1_1_PATH_ALT;
      nvm_alt_file_path = COMANCHE_NVM_TLV_1_1_PATH_ALT;
      break;
    default:
      ALOGE("%s: Detected unknown SoC version: 0x%08x", __func__, chipset_ver_);
      rampatch_file_path = TEMP_RAMPATCH_TLV_0_0_0_PATH;
      nvm_file_path = TEMP_NVM_TLV_0_0_0_PATH;

      rampatch_alt_file_path = TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT;
      nvm_alt_file_path = TEMP_NVM_TLV_0_0_0_PATH_ALT;
      break;
  }

  /* Change baud rate 115.2 kbps to 3Mbps*/
  err = SetBaudRateReq();
  if (err < 0) {
    ALOGE("%s: Baud rate change failed!", __func__);
    dnld_fd_in_progress_ = -1;
    return err;
  }

  ALOGI("%s: Baud rate changed successfully ", __func__);
  /* Donwload TLV files (rampatch, NVM) */
  err = DownloadTlvFile(rampatch_file_path, nvm_file_path, rampatch_alt_file_path, nvm_alt_file_path);
  if (err < 0) {
    ALOGE("%s: Download TLV file failed!", __func__);
    dnld_fd_in_progress_ = -1;
    return err;
  }
  ALOGI("%s: Download TLV file successfully ", __func__);

  /* Get SU FM label information */
  if ((err = GetBuildInfoReq()) < 0) {
    ALOGI("%s: Fail to get FW SU Build info (0x%x)", __func__, err);
    //Ignore the failure of FW SU label information
    err = 0;
  }

  /* Disable internal LDO to use external LDO instead*/
  err = DisableInternalLdo();
  if (is_emb_wp_mode) {
    /*  get chipset supported feature request */
    ALOGV("%s: chipset_ver_: %x08 Calling get addon feature", __func__, chipset_ver_);
    GetAddOnFeatureList();
  }

  /* send enhanced controller logging cmd only for non rome
     chipsets as this command is supported only for them
  */
  if((soc_type_ == BT_SOC_ROME) || (soc_type_ == BT_SOC_NAPLES_UART)) {
    EnableControllerLog();
  } else {
    EnableEnhControllerLog();
  }

  /* Send HCI Reset */
  err = HciReset();
  if ( err < 0 ) {
    ALOGE("HCI Reset Failed !!");
  } else {
    ALOGV("HCI Reset is done\n");
  }

  dnld_fd_in_progress_ = -1;
  return err;
}

int PatchDLManager::HciReset()
{
  int size, err = 0;
  unsigned char cmd[HCI_MAX_CMD_SIZE];
  unsigned char rsp[HCI_MAX_EVENT_SIZE];
  hci_command_hdr *cmd_hdr;
  int flags;

  ALOGI("%s: HCI RESET ", __func__);

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);
  cmd[0]  = HCI_COMMAND_PKT;
  cmd_hdr->opcode = HCI_RESET;
  cmd_hdr->plen   = 0;

  /* Total length of the packet to be sent to the Controller */
  size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
  err = uart_transport_->UartWrite(cmd, size);
  if (err != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, err);
    err = -1;
    goto error;
  }

  /* Wait for command complete event */
  err = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
  if ( err < 0) {
    ALOGE("%s: Failed to set patch info on Controller", __func__);
    goto error;
  }

 error:
  return err;

}

int PatchDLManager::ReadNewHciEvent(unsigned char* buf, int size)
{
  int retval;
  unsigned char protocol_byte;
  unsigned char hdr[BT_EVT_HDR_SIZE];
  unsigned char packet_len;
  unsigned short tot_len;

  ALOGV("%s: >", __func__);

  if (size < 0) {
    ALOGE("%s: Invalid size: %d\n", __func__, size);
    return -1;
  }

  do {
    retval = uart_transport_->Read(&protocol_byte, 1);
    if (retval < 0) {
      ALOGE("%s: read error", __func__);
      return -1;
    }

    if (protocol_byte == LOG_BT_EVT_PACKET_TYPE) {
      break;
    } else {
      ALOGI("%s: Got an invalid proto byte: %d", __func__, protocol_byte);
    }
  } while (1);

  retval = uart_transport_->Read(hdr, BT_EVT_HDR_SIZE);
  if (retval < 0) {
    ALOGE("%s:error in reading hdr: %d", __func__, retval);
    return -1;
  }
  ALOGV("read scucesssssfully HDR");
  packet_len = hdr[BT_EVT_HDR_LEN_OFFSET];
  ALOGV("packet_len: %d\n", packet_len);

  buf[0] = protocol_byte;
  memcpy(buf + 1, hdr, BT_EVT_HDR_SIZE);
  retval = uart_transport_->Read(buf + BT_EVT_HDR_SIZE + 1, packet_len);
  if (retval < 0) {
    ALOGE("%s:error in reading buf: %d", __func__, retval);
    retval = -1;
    return retval;
  }
  tot_len = packet_len + BT_EVT_HDR_SIZE + 1;
  ALOGV("read scucesssssfully payload: tot_len: %d", tot_len);
  return tot_len;
}

int PatchDLManager::ReadHciEvent(unsigned char* buf, int size)
{
  int tot_len;
  unsigned short int opcode;
  tot_len = ReadNewHciEvent(buf, size);
  if (tot_len < 0) {
    ALOGE("%s: Error while reading the hci event", __func__);
    return -1;
  }

  bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, buf, tot_len);
  if (unified_hci &&  (buf[EVENTCODE_OFFSET] == VSEVENT_CODE)) {
    ALOGE("%s: Unexpected event recieved rather than CC", __func__);
    return 0;
  }
  if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
    if (buf[3] == LOG_BT_CONTROLLER_LOG) {
      /* if it is RAMDUMP event OR
       * if normal logging event and diag is enabled
       */
      if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
        ALOGV("It is RAMDUMP, keep looping to get more RAMDUMP events");
        //HAVE A TERMINATION
        ReadVsHciEvent(buf, size);
      } else {
        ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
        return 0;
      }
    }
  } else if (buf[1] == EVT_CMD_COMPLETE) {
    ALOGD("%s: Expected CC", __func__);
    if (tot_len > UNIFIED_HCI_CC_MIN_LENGTH) {
        opcode = (buf[4] | (buf[5] << 8));
        ALOGI("opcode 0x%x", opcode);
      if ((HCI_VS_WIPOWER_CMD_OPCODE == opcode) && (UNIFIED_HCI_CODE == buf[6])) {
        unified_hci = true;
        ALOGI("HCI Unified command interface supported");
      }
    }
    if (unified_hci) {
      GetVsHciEvent(buf);
     }
  } else {
    ALOGE("%s: Unexpected event : protocol byte: %d", __func__, buf[1]);
    tot_len = -1;
  }
  return tot_len;
}

int PatchDLManager::ReadVsHciEvent(unsigned char* buf, int size)
{
  int tot_len;
  bool collecting_ram_dump = false;

  do {
    tot_len = ReadNewHciEvent(buf, size);
    if (tot_len < 0) {
      ALOGE("%s: Error while reading the hci event", __func__);
      return -1;
    }

    bt_logger_->ProcessRx(HCI_PACKET_TYPE_EVENT, &buf[1], tot_len);

    if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
      /* if it is RAMDUMP event OR
       * if normal logging event and diag is enabled
       */
      if (buf[3] == LOG_BT_CONTROLLER_LOG) {
        if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
          ALOGV("It is RAMDUMP, keep looping to get more RAMDUMP events");
          //HAVE A TERMINATION
          collecting_ram_dump = true;
        } else {
          ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
          return 0;
        }
      } else {
        ALOGV("VSC Event! good");
      }
    } else {
      ALOGE("%s: Unexpected event! : opcode: %d", __func__, buf[1]);
      tot_len = -1;
    }

  } while (collecting_ram_dump);

  /* Check if the set patch command is successful or not */
  if (GetVsHciEvent(buf) != HCI_CMD_SUCCESS)
    return -1;

  return tot_len;
}

int PatchDLManager::DisableInternalLdo()
{
  int ret = 0;

  if (IsExtldoEnabled()) {
    unsigned char cmd[5] = { 0x01, 0x0C, 0xFC, 0x01, 0x32 };
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    ALOGI(" %s ", __func__);
    ret = HciSendVsCmd(cmd, rsp, 5);
    if (ret != 5) {
      ALOGE("%s: Send failed with ret value: %d", __func__, ret);
      ret = -1;
    } else {
      /* Wait for command complete event */
      ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      if ( ret < 0) {
        ALOGE("%s: Failed to get response from controller", __func__);
      }
    }
  }
  return ret;
}

int PatchDLManager::GetVsHciEvent(unsigned char *rsp)
{
  int err = 0;
  unsigned char paramlen = 0;
  unsigned char EMBEDDED_MODE_CHECK = 0x02;
  FILE *btversionfile = 0;
  unsigned int soc_id = 0;
  unsigned int productid = 0;
  unsigned short patchversion = 0;
  char build_label[255];
  int build_lbl_len;
  unsigned short buildversion = 0;
  unsigned int opcode = 0;
  unsigned char subOpcode = 0;
  unsigned int ocf = 0;
  unsigned int ogf = 0;
  unsigned char status = 0;

  if ( (rsp[EVENTCODE_OFFSET] == VSEVENT_CODE) || (rsp[EVENTCODE_OFFSET] == EVT_CMD_COMPLETE))
    ALOGV("%s: Received HCI-Vendor Specific event", __func__);
  else {
    ALOGI("%s: Failed to receive HCI-Vendor Specific event", __func__);
    err = -EIO;
    goto failed;
  }

  if (!unified_hci) {
    ocf = rsp[CMD_RSP_OFFSET];
    subOpcode = rsp[RSP_TYPE_OFFSET];
    ALOGV("%s: Parameter Length: 0x%x", __func__, paramlen = rsp[EVT_PLEN]);
    ALOGV("%s: Command response: 0x%x", __func__, ocf = rsp[CMD_RSP_OFFSET]);
    ALOGV("%s: Response type   : 0x%x", __func__, subOpcode = rsp[RSP_TYPE_OFFSET]);
  }
  else
  {
    ALOGI("%s: Opcode: 0x%x", __FUNCTION__, opcode = rsp[5]<<8 | rsp[4]);
    ALOGI("%s: ocf: 0x%x", __FUNCTION__, ocf = opcode & 0x03ff);
    ALOGI("%s: ogf: 0x%x", __FUNCTION__, ogf = opcode >> 10);
    ALOGI("%s: Status: 0x%x", __FUNCTION__, status = rsp[6]);
    ALOGI("%s: Sub-Opcode: 0x%x", __FUNCTION__, subOpcode = rsp[7]);
  }

  /* Check the status of the operation */
  switch ( ocf ) {
    case EDL_CMD_REQ_RES_EVT:
      ALOGV("%s: Command Request Response", __func__);
      switch (subOpcode) {
        case EDL_PATCH_VER_RES_EVT:
        case EDL_APP_VER_RES_EVT:
         if (!unified_hci )
         {
          productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET + 3] << 24 |
                                     rsp[PATCH_PROD_ID_OFFSET + 2] << 16 |
                                     rsp[PATCH_PROD_ID_OFFSET + 1] << 8 |
                                     rsp[PATCH_PROD_ID_OFFSET]  );
          patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET + 1] << 8 |
                                           rsp[PATCH_PATCH_VER_OFFSET] );
          buildversion = (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET + 1] << 8 |
                        rsp[PATCH_ROM_BUILD_VER_OFFSET] );

          ALOGI("\t Current Product ID\t\t: 0x%08x", productid);
          ALOGI("\t Current Patch Version\t\t: 0x%04x", patchversion);
          ALOGI("\t Current ROM Build Version\t: 0x%04x", buildversion);

          if (paramlen - 10) {
            soc_id = (unsigned int)(rsp[PATCH_SOC_VER_OFFSET + 3] << 24 |
                                    rsp[PATCH_SOC_VER_OFFSET + 2] << 16 |
                                    rsp[PATCH_SOC_VER_OFFSET + 1] << 8 |
                                    rsp[PATCH_SOC_VER_OFFSET] );
            ALOGI("\t Current SOC Version\t\t: 0x%08x", soc_id);
          }
        }
         else //unified supported
         {
          ALOGI("\t unified Current Product ID\t\t: 0x%08x",
                productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 3] << 24 |
                                           rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 2] << 16 |
                                           rsp[PATCH_PROD_ID_OFFSET_UNIFIED + 1] << 8 |
                                           rsp[PATCH_PROD_ID_OFFSET_UNIFIED]  ));

          /* Patch Version indicates FW patch version */
          ALOGI("\t unified Current Patch Version\t\t: 0x%04x",
                (patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET_UNIFIED + 1] << 8 |
                                                 rsp[PATCH_PATCH_VER_OFFSET_UNIFIED] )));

          /* ROM Build Version indicates ROM build version like 1.0/1.1/2.0 */
          ALOGI("\t unified Current ROM Build Version\t: 0x%04x", buildversion =
                  (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED + 1] << 8 |
                        rsp[PATCH_ROM_BUILD_VER_OFFSET_UNIFIED] ));

          if (paramlen - 10) {
            ALOGI("\t unified Current SOC Version\t\t: 0x%08x", soc_id =
                    (unsigned int)(rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 3] << 24 |
                                   rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 2] << 16 |
                                   rsp[PATCH_SOC_VER_OFFSET_UNIFIED + 1] << 8 |
                                   rsp[PATCH_SOC_VER_OFFSET_UNIFIED]  ));
          }
         }
          if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "wb"))) {
            fprintf(btversionfile, "Bluetooth Controller Product ID    : 0x%08x\n", productid);
            fprintf(btversionfile, "Bluetooth Controller Patch Version : 0x%04x\n", patchversion);
            fprintf(btversionfile, "Bluetooth Controller Build Version : 0x%04x\n", buildversion);
            fprintf(btversionfile, "Bluetooth Controller SOC Version   : 0x%08x\n", soc_id);
            fclose(btversionfile);
          } else {
            ALOGE("Failed to dump SOC version info. Errno:%d", errno);
          }

          chipset_ver_ = QCA_BT_VER(productid, buildversion, soc_id);
          break;
        case EDL_TVL_DNLD_RES_EVT:
        case EDL_CMD_EXE_STATUS_EVT:
          switch (err = rsp[CMD_STATUS_OFFSET]) {
            case HCI_CMD_SUCCESS:
              ALOGV("%s: Download Packet successfully!", __func__);
              break;
            case PATCH_LEN_ERROR:
              ALOGE("%s: Invalid patch length argument passed for EDL PATCH "
                    "SET REQ cmd", __func__);
              break;
            case PATCH_VER_ERROR:
              ALOGE("%s: Invalid patch version argument passed for EDL PATCH "
                    "SET REQ cmd", __func__);
              break;
            case PATCH_CRC_ERROR:
              ALOGE("%s: CRC check of patch failed!!!", __func__);
              break;
            case PATCH_NOT_FOUND:
              ALOGE("%s: Invalid patch data!!!", __func__);
              break;
            case TLV_TYPE_ERROR:
              ALOGE("%s: TLV Type Error !!!", __func__);
              break;
            default:
              ALOGE("%s: Undefined error (0x%x)", __func__, err);
              break;
          }
          break;
        case HCI_VS_GET_BUILD_VER_EVT:
          build_lbl_len = rsp[5];
          memcpy(build_label, &rsp[6], build_lbl_len);
          *(build_label + build_lbl_len) = '\0';

          ALOGI("BT SoC FW SU Build info: %s, %d", build_label, build_lbl_len);
          if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "a+b"))) {
            fprintf(btversionfile, "Bluetooth Contoller SU Build info  : %s\n", build_label);
            fclose(btversionfile);
          } else {
            ALOGI("Failed to dump  FW SU build info. Errno:%d", errno);
          }
          break;
        case EDL_BOARD_ID_RESPONSE:
          ALOGI("%s: board id %x %x!!", __func__, rsp[6], rsp[7]);
          if (rsp[6] <= 0x00) {
            board_id_[0] = Convert2Ascii((rsp[7] & MSB_NIBBLE_MASK) >> 4);
            board_id_[1] = Convert2Ascii(rsp[7] & LSB_NIBBLE_MASK);
            board_id_[2] = '\0';
          } else {
            board_id_[0] = Convert2Ascii((rsp[6] & MSB_NIBBLE_MASK) >> 4);
            board_id_[1] = Convert2Ascii(rsp[6] & LSB_NIBBLE_MASK);
            board_id_[2] = Convert2Ascii((rsp[7] & MSB_NIBBLE_MASK) >> 4);
            board_id_[3] = Convert2Ascii(rsp[7] & LSB_NIBBLE_MASK);
            board_id_[4] = '\0';
          }
          break;
      }
      break;

    case NVM_ACCESS_CODE:
      ALOGI("%s: NVM Access Code!!!", __func__);
      err = HCI_CMD_SUCCESS;
      break;
    case EDL_SET_BAUDRATE_RSP_EVT:
      if (rsp[BAUDRATE_RSP_STATUS_OFFSET] != BAUDRATE_CHANGE_SUCCESS) {
        ALOGE("%s: Set Baudrate request failed - 0x%x", __func__,
              rsp[CMD_STATUS_OFFSET]);
        err = -1;
      }
      break;
    case EDL_WIP_QUERY_CHARGING_STATUS_EVT:
      /* Query charging command has below return values
         0 - in embedded mode not charging
         1 - in embedded mode and charging
         2 - hadofff completed and in normal mode
         3 - no wipower supported on mtp. so irrepective of charging
         handoff command has to be sent if return values are 0 or 1.
         These change include logic to enable generic BT turn on sequence.*/
      if (rsp[4] < EMBEDDED_MODE_CHECK) {
        ALOGI("%s: WiPower Charging in Embedded Mode!!!", __func__);
        wipower_handoff_ready_ = rsp[4];
        wipower_flag_ = 1;
      }
      break;
    case EDL_WIP_START_HANDOFF_TO_HOST_EVENT:
      /*TODO: rsp code 00 mean no charging
         this is going to change in FW soon*/
      if (rsp[4] == NON_WIPOWER_MODE) {
        ALOGE("%s: WiPower Charging hand off not ready!!!", __func__);
      }
      break;
    case HCI_VS_GET_ADDON_FEATURES_EVENT:
      if ((rsp[4] & ADDON_FEATURES_EVT_WIPOWER_MASK)) {
        ALOGD("%s: WiPower feature supported!!", __func__);
        property_set("persist.bluetooth.a4wp", "true");
      }
      break;
    case HCI_VS_STRAY_EVT:
      /* WAR to handle stray Power Apply EVT during patch download */
      ALOGD("%s: Stray HCI VS EVENT", __func__);
      if (patch_dnld_pending_ && dnld_fd_in_progress_ != -1) {
        unsigned char rsp[HCI_MAX_EVENT_SIZE];
        memset(rsp, 0x00, HCI_MAX_EVENT_SIZE);
        ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE);
      } else {
        ALOGE("%s: Not a valid status!!!", __func__);
        err = -1;
      }
      break;
    default:
      ALOGE("%s: Not a valid status!!!", __func__);
      err = -1;
      break;
  }

 failed:
  return err;
}

bool PatchDLManager::IsExtldoEnabled()
{
  bool is_extldo = false;
  char extldo_prop[PROPERTY_VALUE_MAX];

  property_get("wc_transport.extldo", extldo_prop, "disabled");

  if (!strncmp(extldo_prop, "enabled", strlen(extldo_prop))) {
    is_extldo = true;
  }

  return is_extldo;
}

int PatchDLManager::HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size)
{
  int ret = 0;

  /* Send the HCI command packet to UART for transmission */
  ret = uart_transport_->UartWrite(cmd, size);
  if (ret != size) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
    goto failed;
  }

  if (wait_vsc_evt_) {
    /* Check for response from the Controller */
    if (!unified_hci) {
     if (ReadVsHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      ret = -ETIMEDOUT;
      ALOGI("%s: Failed to get ReadVsHciEvent Event from SOC", __func__);
      goto failed;
     }
      ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
    }
    else
    {
     if (ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE) < 0) {
      ret = -ETIMEDOUT;
      ALOGI("%s: Failed to get ReadHciEvent Event from SOC", __func__);
      goto failed;
     }
      ALOGV("%s: Received HCI-Vendor Specific Event from SOC", __func__);
    }
  }

 failed:
  return ret;
}

void PatchDLManager::FrameHciPkt(
  unsigned char *cmd,
  int edl_cmd, unsigned int p_base_addr,
  int segtNo, int size
)
{
  int offset = 0;
  hci_command_hdr *cmd_hdr;

  memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

  cmd_hdr = (hci_command_hdr*)(cmd + 1);

  cmd[0]      = HCI_COMMAND_PKT;
  cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
  cmd_hdr->plen   = size;
  cmd[4]      = edl_cmd;

  switch (edl_cmd) {
    case EDL_PATCH_SET_REQ_CMD:
      /* Copy the patch header info as CMD params */
      memcpy(&cmd[5], phdr_buffer_, PATCH_HDR_LEN);
      ALOGV("%s: Sending EDL_PATCH_SET_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_DLD_REQ_CMD:
      offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
      p_base_addr += offset;
      cmd_hdr->plen   = (size + 6);
      cmd[5]  = (size + 4);
      cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
      cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
      cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
      cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
      memcpy(&cmd[10], (pdata_buffer_ + offset), size);

      ALOGV("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
            __func__, size);
      ALOGV("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
            "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
            cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
      break;
    case EDL_PATCH_ATCH_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_RST_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_RESET_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_VER_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_VER_REQ_CMD", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_PATCH_TLV_REQ_CMD:
      ALOGV("%s: Sending EDL_PATCH_TLV_REQ_CMD", __func__);
      /* Parameter Total Length */
      cmd[3] = size + 2;

      /* TLV Segment Length */
      cmd[5] = size;
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
      offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
      memcpy(&cmd[6], (pdata_buffer_ + offset), size);
      break;
    case EDL_GET_BUILD_INFO:
      ALOGV("%s: Sending EDL_GET_BUILD_INFO", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    case EDL_GET_BOARD_ID:
      ALOGV("%s: Sending EDL_GET_BOARD_ID", __func__);
      ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
      break;
    default:
      ALOGE("%s: Unknown EDL CMD !!!", __func__);
  }
}

unsigned char PatchDLManager::IsSibsEnabled()
{
  /* Check whether IBS is enabled through the macro 'WCNSS_IBS_ENABLED'
     and the property 'persist.service.bdroid.sibs'. */
#ifdef WCNSS_IBS_ENABLED
  return IbsHandler::IsEnabled();
#else
  return false;
#endif
}

unsigned char PatchDLManager::Convert2Ascii(unsigned char temp)
{
  unsigned char n = temp;

  if ( n  >= 0 && n <= 9)
    n = n + 0x30;
  else
    n = n + 0x57;
  return n;
}

void PatchDLManager::UpdateNapierNvm(tlv_nvm_hdr *nvm)
{
  uint8_t *nvm_byte_ptr = (uint8_t *)nvm;

  if (!nvm)
    return;

  nvm_byte_ptr += sizeof(tlv_nvm_hdr);

  /* Update Tag#17: HCI UART Settings */
  if (nvm->tag_id == TAG_NUM_17) {
    uint8_t baudrate = uart_transport_->GetMaxBaudrate();

    ALOGI("%s: baudrate %02x", __func__, baudrate);

    /* Byte#1: UART baudrate */
    *(nvm_byte_ptr + 1) = baudrate;
  }
}

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
int PatchDLManager::SendCrashCommand()
{
  int ret = 0;
  unsigned char cmd[5] = { 0x01, 0x0C, 0xFC, 0x01, 0x28 };
  unsigned char rsp[HCI_MAX_EVENT_SIZE];

  ALOGI(" %s ", __func__);
  ret = uart_transport_->UartWrite(cmd, 5);
  if (ret != 5) {
    ALOGE("%s: Send failed with ret value: %d", __func__, ret);
    ret = -1;
  } else {
    /* Wait for command complete event */

    ret = ReadHciEvent(rsp, HCI_MAX_EVENT_SIZE);
    if ( ret < 0) {
      ALOGE("%s: Failed to get response from controller", __func__);
    }
  }
  return ret;
}
#endif    //DEBUG_CRASH_SOC_WHILE_DLD

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
