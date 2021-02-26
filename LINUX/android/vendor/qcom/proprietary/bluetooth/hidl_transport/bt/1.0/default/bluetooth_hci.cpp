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

#include <utils/Log.h>
#include "bluetooth_hci.h"
#include "bluetooth_address.h"

#define LOG_TAG "vendor.qti.bluetooth@1.0-bluetooth_hci"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

static const uint8_t HCI_DATA_TYPE_COMMAND = 1;
static const uint8_t HCI_DATA_TYPE_ACL = 2;
static const uint8_t HCI_DATA_TYPE_SCO = 3;


BluetoothHci::BluetoothHci()
  : deathRecipient(new BluetoothDeathRecipient(this))
{
}
Return<void> BluetoothHci::initialize(
  const ::android::sp<IBluetoothHciCallbacks>& cb)
{
  ALOGW("BluetoothHci::initialize()");
  if(event_cb_ == nullptr) {
    cb->linkToDeath(deathRecipient, 0);
    event_cb_ = cb;
  } else {
    ALOGW("BluetoothHci::initialize already one client is active");
    ALOGW("BluetoothHci: initialize returning INITIALIZATION_ERROR");
    auto hidl_client_status =
                    cb->initializationComplete(Status::INITIALIZATION_ERROR);
    if(!hidl_client_status.isOk()) {
      ALOGE("Client dead, callback initializationComplete failed");
    }
    return Void();
  }

  bool rc = DataHandler::Init( TYPE_BT,
    [this](bool status) {
      if (event_cb_ != nullptr) {
        auto hidl_client_status = event_cb_->initializationComplete(
          status ? Status::SUCCESS : Status::INITIALIZATION_ERROR);
        if(!hidl_client_status.isOk()) {
          ALOGE("Client dead, callback initializationComplete failed");
        }
      }
    },
    [this](HciPacketType type, const hidl_vec<uint8_t> *packet) {
      if (event_cb_ == nullptr) {
        ALOGE("BluetoothHci: event_cb_ is null");
        return;
      }
      switch (type) {
        case HCI_PACKET_TYPE_EVENT:
        {
          ALOGD("BluetoothHci::event call back-->");
          auto hidl_client_status = event_cb_->hciEventReceived(*packet);
          if(!hidl_client_status.isOk()) {
            ALOGE("Client dead, callback hciEventReceived failed");
          }
          ALOGD("BluetoothHci::event call back<--");
        }
        break;
        case HCI_PACKET_TYPE_ACL_DATA:
        {
          ALOGD("BluetoothHci::data call back-->");
          auto hidl_client_status = event_cb_->aclDataReceived(*packet);
          if(!hidl_client_status.isOk()) {
            ALOGE("Client dead, callback aclDataReceived failed");
          }
          ALOGD("BluetoothHci::data call back<--");
        }
        break;
        default:
          ALOGE("%s Unexpected event type %d", __func__, type);
          break;
      }
    });
  if (!rc && (event_cb_ != nullptr)) {
    auto hidl_client_status = event_cb_->initializationComplete(Status::INITIALIZATION_ERROR);
    if(!hidl_client_status.isOk()) {
      ALOGE("Client dead, callback initializationComplete failed");
    }
    ALOGE("BluetoothHci: error INITIALIZATION_ERROR");
  }
  return Void();
}

Return<void> BluetoothHci::close()
{
  ALOGW("BluetoothHci::close()");
  DataHandler *data_handler = DataHandler::Get();
  if (event_cb_ != nullptr) event_cb_->unlinkToDeath(deathRecipient);
  if(data_handler != nullptr) data_handler->sendCommandWait(HCI_RESET_CMD);
  DataHandler::CleanUp(TYPE_BT);
  event_cb_ = nullptr;
  ALOGW("BluetoothHci::close, finish cleanup");
  return Void();
}

Return<void> BluetoothHci::sendHciCommand(const hidl_vec<uint8_t>& command)
{
  sendDataToController(HCI_PACKET_TYPE_COMMAND, command);
  return Void();
}

Return<void> BluetoothHci::sendAclData(const hidl_vec<uint8_t>& data)
{
  sendDataToController(HCI_PACKET_TYPE_ACL_DATA, data);
  return Void();
}

Return<void> BluetoothHci::sendScoData(const hidl_vec<uint8_t>& data)
{
  sendDataToController(HCI_PACKET_TYPE_SCO_DATA, data);
  return Void();
}

void BluetoothHci::sendDataToController(HciPacketType type,
                                        const hidl_vec<uint8_t>& data)
{
  DataHandler *data_handler = DataHandler::Get();

  if(data_handler != nullptr) {
    data_handler->SendData(TYPE_BT, type, data.data(), data.size());
  }
}



// Methods from ::android::hidl::base::V1_0::IBase follow.

IBluetoothHci* HIDL_FETCH_IBluetoothHci(const char* /* name */)
{
  uint8_t vnd_local_bd_addr_[6];
  BluetoothAddress::GetLocalAddress(vnd_local_bd_addr_);
  return new BluetoothHci();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
