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
// Copyright 2017 The Android Open Source Project
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

#define LOG_TAG "vendor.qti.bluetooth@1.0-mct_controller"

#include <hidl/HidlSupport.h>
#include "mct_controller.h"
#include "hci_mct_transport.h"
#include <cutils/properties.h>
#include <utils/Log.h>

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

const int HCI_SSR_RETRY_COUNT = 6;

using PacketReadCallback = std::function<void(ProtocolType, HciPacketType, const hidl_vec<uint8_t>*)>;

MctController::MctController(BluetoothSocType soc_type)
  : soc_type_(soc_type),
  hci_event_packetizer_([this](hidl_vec<uint8_t> *data) { OnEventPacketReady(data); }),
  hci_acl_packetizer_([this](hidl_vec<uint8_t> *data) { OnAclPacketReady(data); })
{
  init_done_ = false;
  hci_transport_ = nullptr;
}

bool MctController::Init(PacketReadCallback pkt_read_cb)
{
  HciMctTransport* mct_transport = nullptr;
  NvmTagsManager* nvm_tags_manager = nullptr;

  if (init_done_) {
    ALOGD("already initialized, return");
    return true;
  }

  read_cb_ = pkt_read_cb;

  // initialize the HCI transport
  if (!(mct_transport = new HciMctTransport())) {
    goto error;
  }

  hci_transport_ = static_cast<HciTransport*> (mct_transport);

  mct_transport->Init(soc_type_);

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: Acquire();
#endif

  //Download the NVM tags
  nvm_tags_manager = new NvmTagsManager(mct_transport);
  if (nvm_tags_manager->SocInit() < 0) {
    goto error;
  }

  // set up the fd watcher now
  fd_watcher_.WatchFdForNonBlockingReads(
    mct_transport->GetCtrlFd(),
    [this](int fd) { OnEventDataReady(fd); });

  fd_watcher_.WatchFdForNonBlockingReads(
    mct_transport->GetDataFd(),
    [this](int fd) { OnAclDataReady(fd); });

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: Release();
#endif

  if (nvm_tags_manager) {
    delete nvm_tags_manager;
    nvm_tags_manager = nullptr;
  }

  init_done_ = true;
  ALOGD("Init succeded");
  return init_done_;

 error:
  ALOGE("Init failed");
  if (nvm_tags_manager) {
    delete nvm_tags_manager;
    nvm_tags_manager = nullptr;
  }

  if (hci_transport_) {
    hci_transport_->CleanUp();
    delete hci_transport_;
    hci_transport_ = nullptr;
  }
  return init_done_;
}

size_t MctController::SendPacket(HciPacketType packet_type,
                        const uint8_t *data, size_t length)
{
  return hci_transport_->Write(packet_type, data, length);
}

bool MctController::Cleanup(void)
{
  if (!init_done_) {
    ALOGD("MctController:: already closed, return");
    return false;
  }

  // stop the fd watcher
  fd_watcher_.StopWatchingFileDescriptors();

  if (hci_transport_) {
    hci_transport_->CleanUp();
    delete hci_transport_;
    hci_transport_ = nullptr;
  }
  init_done_ = false;
  return true;
}

void MctController::OnAclPacketReady(hidl_vec<uint8_t> *data)
{
  if(data == nullptr) {
    ALOGE("Error reading data from smd");
    return;
  }

  if (read_cb_) {
    read_cb_(TYPE_BT, HCI_PACKET_TYPE_ACL_DATA, data);
  }
}

void MctController::OnEventPacketReady(hidl_vec<uint8_t> *data)
{
  if(data == nullptr) {
    ALOGE("Error reading data from smd");
    return;
  }

  if (read_cb_) {
    read_cb_(TYPE_BT, HCI_PACKET_TYPE_EVENT, data);
  }
}

void MctController::OnAclDataReady(int fd)
{
  hci_acl_packetizer_.OnDataReady(fd, HCI_PACKET_TYPE_ACL_DATA);
}

void MctController::OnEventDataReady(int fd)
{
  uint8_t reset = DevInReset(fd);

  if(reset) {
    unsigned char bt_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x10, 0x01, 0x0f };
    hidl_vec<uint8_t> *bt_packet_ = new hidl_vec<uint8_t>;
    bt_packet_->resize(LENGTH_HW_ERROR_EVT);
    ALOGD("%s: Sending hw error event", __func__);
    memcpy(bt_packet_->data(), bt_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(TYPE_BT, HCI_PACKET_TYPE_EVENT, bt_packet_);
   } else {
     hci_event_packetizer_.OnDataReady(fd, HCI_PACKET_TYPE_EVENT);
   }
}

bool MctController::DevInReset(int fd)
{
  volatile int serial_bits;
  bool dev_reset_done = 0;
  uint8_t retry_count = 0;
  ioctl(fd, TIOCMGET, &serial_bits);
  if (serial_bits & TIOCM_OUT2) {
    while(serial_bits & TIOCM_OUT1) {
      ALOGD("%s: device in reset", __func__);
      sleep(2);
      retry_count++;
      ioctl(fd, TIOCMGET, &serial_bits);
      if((serial_bits & TIOCM_OUT1)) {
        dev_reset_done = 0;
      } else {
        dev_reset_done = 1;
        ALOGD("%s: device reset completed", __func__);
      }
      if(retry_count == HCI_SSR_RETRY_COUNT) {
        //treat it as ssr completed to kill the bt
        // process
        ALOGE("%s: Retrials exceeded, treating it reset completed", __func__);
        dev_reset_done = 1;
        break;
      }
    }
  }
  return dev_reset_done;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
