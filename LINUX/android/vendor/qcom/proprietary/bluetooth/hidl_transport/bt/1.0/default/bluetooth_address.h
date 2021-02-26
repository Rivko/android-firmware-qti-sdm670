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

#include <fcntl.h>

#include <string>
#include <vector>
#include <string.h>
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"
#include "device_management_service_v01.h"
#include "comdef.h"

/* Android system property for fetching the modem type */
#define QMI_UIM_PROPERTY_BASEBAND               "ro.baseband"
#define DMS_QMI_TIMEOUT (2000)

/* Android system property values for various modem types */
#define QMI_UIM_PROP_BASEBAND_VALUE_APQ         "apq"
#define QMI_UIM_PROP_BASEBAND_VALUE_SDA         "sda"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

// The property key stores the storage location of Bluetooth Device Address
static constexpr char PROPERTY_BT_BDADDR_PATH[] = "ro.bt.bdaddr_path";

// Check for a legacy address stored as a property.
static constexpr char PERSIST_BDADDR_PROPERTY[] =
    "persist.service.bdroid.bdaddr";

// If there is no valid bdaddr available from PROPERTY_BT_BDADDR_PATH and there
// is no available persistent bdaddr available from PERSIST_BDADDR_PROPERTY,
// use a factory set address.
static constexpr char FACTORY_BDADDR_PROPERTY[] = "ro.boot.btmacaddr";

#define QMI_INIT_MAX_TRY              (5)
#define QMI_INIT_RETRY_INTERVAL_SEC   (2)

// Encapsulate handling for Bluetooth Addresses:
class BluetoothAddress {
 private:
  static uint8_t modem_bdaddr_[6];
  static bool read_addr_from_modemnv_;
  static bool tried_reading_modemnv_;
  static bool dms_init_done_;
  static qmi_client_os_params dms_qmi_client_os_params_;
  static qmi_client_type dms_qmi_client_;
  // Conversion constants
  static constexpr size_t kStringLength = sizeof("XX:XX:XX:XX:XX:XX") - 1;
  static constexpr size_t kBytes = (kStringLength + 1) / 3;
  static void le2bd(uint8_t *localAddr);

 public:
  static void BytesToString(const uint8_t* addr, char* addr_str);

  static bool StringToBytes(const char* addr_str, uint8_t* addr);

  static bool GetLocalAddress(uint8_t* addr);

  static bool QmiDmsInit();

  static void QmiDmsCleanup();

  static bool QmiDmsGetAddress(unsigned char *pBdAddr);
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
