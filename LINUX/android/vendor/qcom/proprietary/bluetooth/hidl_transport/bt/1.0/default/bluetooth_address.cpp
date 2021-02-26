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

#define LOG_TAG "vendor.qti.bluetooth@1.0-bluetooth_address"

#include "bluetooth_address.h"

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <utils/Log.h>
#include "bt_nv.h"

namespace {

bool fetch_vendor_addr(uint8_t * addr )
{
  nv_persist_item_type my_nv_item;
  nv_persist_stat_enum_type cmd_result;
  bool result = false;

  cmd_result = (nv_persist_stat_enum_type)bt_nv_cmd(NV_READ_F,
                                                    NV_BD_ADDR_I, &my_nv_item);
  ALOGE("CMD result: %d", cmd_result);
  if (NV_SUCCESS == cmd_result) {
    /* copy bytes */
    addr[0] = my_nv_item.bd_addr[0];
    addr[1] = my_nv_item.bd_addr[1];
    addr[2] = my_nv_item.bd_addr[2];
    addr[3] = my_nv_item.bd_addr[3];
    addr[4] = my_nv_item.bd_addr[4];
    addr[5] = my_nv_item.bd_addr[5];

    ALOGE("BD address read for NV_BD_ADDR_I: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
          (unsigned int)my_nv_item.bd_addr[0], (unsigned int)my_nv_item.bd_addr[1],
          (unsigned int)my_nv_item.bd_addr[2], (unsigned int)my_nv_item.bd_addr[3],
          (unsigned int)my_nv_item.bd_addr[4], (unsigned int)my_nv_item.bd_addr[5]);
    result = true;
  }
  return result;
}

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

bool BluetoothAddress::read_addr_from_modemnv_ = false;
bool BluetoothAddress::tried_reading_modemnv_ = false;
bool BluetoothAddress::dms_init_done_ = false;
uint8_t BluetoothAddress::modem_bdaddr_[6] = {'\0'};
qmi_client_os_params BluetoothAddress::dms_qmi_client_os_params_ = {};
qmi_client_type BluetoothAddress::dms_qmi_client_ = {};

bool BluetoothAddress::QmiDmsInit()
{
  qmi_client_error_type qmi_client_err = QMI_NO_ERR;
  qmi_idl_service_object_type dms_service;
  char prop_value[PROPERTY_VALUE_MAX];
  qmi_client_qmux_instance_type qmi_modem_port;
  int timeout = 4;

  /* Find out the modem type */
  memset(prop_value, 0x00, sizeof(prop_value));
  property_get(QMI_UIM_PROPERTY_BASEBAND, prop_value, "");

  /*check if it's APQ/SDA only device */
  if (strcmp(prop_value, QMI_UIM_PROP_BASEBAND_VALUE_APQ) == 0 ||
          strcmp(prop_value, QMI_UIM_PROP_BASEBAND_VALUE_SDA) == 0) {
    ALOGE("%s:Read BD address from Modem NV is not supported for %s devices \n",
            __func__, prop_value);
    return false;
  }

  dms_service = dms_get_service_object_v01();
  if(dms_service == NULL) {
      ALOGE("%s: Not able to get dms service handle\n", __func__);
      return false;
  }

  qmi_modem_port = (qmi_client_qmux_instance_type)QMI_CLIENT_INSTANCE_ANY;

  qmi_client_err = qmi_client_init_instance(dms_service, qmi_modem_port,
              NULL, NULL, &dms_qmi_client_os_params_, timeout, &dms_qmi_client_);

  if(qmi_client_err != QMI_NO_ERR) {
    ALOGE("%s :Error while Initializing qmi_client_init_instance: %d\n",
            __func__, qmi_client_err);
    return false;
  }

  dms_init_done_ = true;
  return true;
}

bool BluetoothAddress::QmiDmsGetAddress(unsigned char *pBdAddr)
{
  qmi_client_error_type qmi_client_err;
  dms_get_mac_address_req_msg_v01 addr_req;
  dms_get_mac_address_resp_msg_v01 addr_resp;

  if((dms_init_done_ == false) || (pBdAddr == NULL)) {
    return false;
  }
  /* clear the request & response contents */ 
  memset(&addr_req, 0, sizeof(addr_req));
  memset(&addr_resp, 0, sizeof(addr_resp));

  /*Request to get the Bluetooth address */
  addr_req.device = DMS_DEVICE_MAC_BT_V01;
  qmi_client_err = qmi_client_send_msg_sync(dms_qmi_client_,
          QMI_DMS_GET_MAC_ADDRESS_REQ_V01, &addr_req, sizeof(addr_req), &addr_resp,
          sizeof(addr_resp), DMS_QMI_TIMEOUT);

  if(qmi_client_err != QMI_NO_ERR){
    ALOGE("%s: Error:  %d\n", __func__, qmi_client_err);
    return false;
  }

  ALOGE("%s: addr_resp.mac_address_valid %d addr_resp.mac_address_len %d \n",
          __func__, addr_resp.mac_address_valid,addr_resp.mac_address_len );

  if (addr_resp.mac_address_valid && (addr_resp.mac_address_len == 6)) {
    memcpy(pBdAddr, addr_resp.mac_address, addr_resp.mac_address_len);
    ALOGE("%s: Succesfully Read BT address\n", __func__);
    return true;
  }
  return false;
}

void BluetoothAddress::QmiDmsCleanup()
{
  qmi_client_error_type qmi_client_err;
  ALOGV("->%s", __func__);

  if(dms_init_done_ == false) {
    return;
  }

  qmi_client_err = qmi_client_release(dms_qmi_client_);
  if(qmi_client_err != QMI_NO_ERR) {
    ALOGE("Error: while releasing qmi_client : %d\n", qmi_client_err);
  }
  ALOGV("<-%s", __func__);
}

void BluetoothAddress::BytesToString(const uint8_t* addr, char* addr_str)
{
  snprintf(addr_str, kStringLength+1, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
          addr[3], addr[4], addr[5]);
}

bool BluetoothAddress::StringToBytes(const char* addr_str, uint8_t* addr)
{
  if (addr_str == NULL) return false;
  if (strnlen(addr_str, kStringLength) != kStringLength) return false;
  unsigned char trailing_char = '\0';

  return (sscanf(addr_str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx%1c",
                 &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5],
                 &trailing_char) == kBytes);
}

void BluetoothAddress::le2bd(uint8_t localAddr[6])
{
    int i;
    uint8_t temp;

    for (i = 0; i < 3; i++) {
        temp = localAddr[i];
        localAddr[i] = localAddr[5-i];
        localAddr[5-i] = temp;
    }
}

bool BluetoothAddress::GetLocalAddress(uint8_t *local_addr)
{
  char property[PROPERTY_VALUE_MAX] = { 0 };
  bool valid_bda = false;

  // Get addr from vendor location
  if (fetch_vendor_addr(local_addr)) {
    valid_bda = true;
  }
  // Get local bdaddr storage path from a system property.
  if (!valid_bda && property_get(PROPERTY_BT_BDADDR_PATH, property, NULL)) {
    int addr_fd;

    ALOGV("%s: Trying %s", __func__, property);

    addr_fd = open(property, O_RDONLY);
    if (addr_fd != -1) {
      int bytes_read = read(addr_fd, property, kStringLength);
      CHECK(bytes_read == kStringLength);
      close(addr_fd);

      // Null terminate the string.
      property[kStringLength] = '\0';

      // If the address is not all zeros, then use it.
      const uint8_t zero_bdaddr[kBytes] = { 0, 0, 0, 0, 0, 0 };
      if ((StringToBytes(property, local_addr)) &&
          (memcmp(local_addr, zero_bdaddr, kBytes) != 0)) {
        valid_bda = true;
        le2bd(local_addr);
        ALOGD("%s: Got Factory BDA %s", __func__, property);
      }
    }
  }

  // No BDADDR found in the file. Look for BDA in a factory property.
  if (!valid_bda && property_get(FACTORY_BDADDR_PROPERTY, property, NULL) &&
      StringToBytes(property, local_addr)) {
    valid_bda = true;
    le2bd(local_addr);
  }

  // Check if address is stored @ Modem NV
  if ((!valid_bda)&& property_get("persist.vendor.bluetooth.modem_nv_support",
                   property, "false") && (strcmp(property, "true") == 0))
 {
    ALOGV("%s: QMI initialization START ",__func__);
    if(!tried_reading_modemnv_) {
      int retry = 0;
      while (retry < QMI_INIT_MAX_TRY) {
        if (retry > 0)
          sleep(QMI_INIT_RETRY_INTERVAL_SEC);
        if (QmiDmsInit() == true) {
          if (QmiDmsGetAddress(modem_bdaddr_) == true) {
            char bdstr[kStringLength + 1];
            ALOGE("Read MODEM_NV_BD_ADDR 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
            modem_bdaddr_[0], modem_bdaddr_[1], modem_bdaddr_[2], modem_bdaddr_[3],
                modem_bdaddr_[4], modem_bdaddr_[5]);
            valid_bda = true;
            read_addr_from_modemnv_ = true;
            memcpy(local_addr, modem_bdaddr_, kBytes);

            /* Convert to ascii, and store as a persistent property */
            BytesToString(local_addr, bdstr);
            if (property_set(PERSIST_BDADDR_PROPERTY, (char*)bdstr) < 0) {
                ALOGE("%s: Failed to set MODEM_NV_BD_ADDR in prop %s", __func__,
                PERSIST_BDADDR_PROPERTY);
            }
          }
          QmiDmsCleanup();
          break;
        }
         ++retry;
        ALOGE("%s: QMI initialization failed in %d try...", __func__, retry);
      }
      tried_reading_modemnv_ = true;
    } else if(read_addr_from_modemnv_) {
       memcpy(local_addr, modem_bdaddr_, kBytes);
       valid_bda = true;
    }
  }

  // No factory BDADDR found. Look for a previously stored BDA.
  if (!valid_bda && property_get(PERSIST_BDADDR_PROPERTY, property, NULL) &&
      StringToBytes(property, local_addr)) {
    valid_bda = true;
    le2bd(local_addr);
  }

  /* Generate new BDA if necessary */
  if (!valid_bda) {
    char bdstr[kStringLength + 1];
    struct timespec cur_time;

    if (-1 == clock_gettime (CLOCK_MONOTONIC, &cur_time))
    {
      ALOGE("%s: clock_gettime failed\n", __func__);
    }

    srand((unsigned int)cur_time.tv_nsec);

    /* No autogen BDA. Generate one now. */
    local_addr[0] = 0x22;
    local_addr[1] = 0x22;
    local_addr[2] = (uint8_t)rand();
    local_addr[3] = (uint8_t)rand();
    local_addr[4] = (uint8_t)rand();
    local_addr[5] = (uint8_t)rand();

    /* Convert to ascii, and store as a persistent property */
    BytesToString(local_addr, bdstr);

    ALOGE("%s: No preset BDA! Generating BDA: %s for prop %s", __func__,
          (char*)bdstr, PERSIST_BDADDR_PROPERTY);
    ALOGE("%s: This is a bug in the platform!  Please fix!", __func__);

    if (property_set(PERSIST_BDADDR_PROPERTY, (char*)bdstr) < 0) {
      ALOGE("%s: Failed to set random BDA in prop %s", __func__,
            PERSIST_BDADDR_PROPERTY);
      valid_bda = false;
    } else {
      valid_bda = true;
      le2bd(local_addr);
    }
  }

  return valid_bda;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
