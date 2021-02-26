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

#include <hidl/HidlSupport.h>
#include "data_handler.h"
#include "logger.h"
#include <cutils/properties.h>
#include <utils/Log.h>
#include <pthread.h>
#include <chrono>
#include <thread>

#define LOG_TAG "vendor.qti.bluetooth@1.0-data_handler"

namespace {

using android::hardware::bluetooth::V1_0::implementation::DataHandler;

DataHandler *data_handler = nullptr;

std::mutex init_mutex_;

std::mutex evt_wait_mutex_;
std::condition_variable event_wait_cv;
bool event_wait;
uint16_t awaited_evt;

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;
using DataReadCallback = std::function<void(HciPacketType, const hidl_vec<uint8_t>*)>;
using InitializeCallback = std::function<void(bool success)>;

bool DataHandler::Init(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  // lock required incase of multiple binder threads
  ALOGW("DataHandler:: Init()");
  std::unique_lock<std::mutex> guard(init_mutex_);
  if (!data_handler) {
    data_handler = new DataHandler();
  }
  return data_handler->Open(type, init_cb, data_read_cb);
}

void DataHandler::CleanUp(ProtocolType type)
{
  // lock is required incase of multiple binder threads
  std::unique_lock<std::mutex> guard(init_mutex_);

  ALOGW("DataHandler::CleanUp()");
  if (data_handler && data_handler->Close(type)) {
    delete data_handler;
    data_handler = NULL;
  }
}

DataHandler * DataHandler::Get()
{
  return data_handler;
}

DataHandler::DataHandler() {
  soc_type_ = GetSocTypeInt();
  logger_ = Logger::Get();
}
BluetoothSocType DataHandler::GetSocType()
{
  return soc_type_;
}

BluetoothSocType DataHandler::GetSocTypeInt()
{
  int ret = 0;
  char soc[PROPERTY_VALUE_MAX];

  ret = property_get("qcom.bluetooth.soc", soc, NULL);
  if (ret != 0) {
    ALOGW("qcom.bluetooth.soc set to %s\n", soc);
    if (!strncasecmp(soc, "rome", sizeof("rome"))) {
      soc_type_ = BT_SOC_ROME;
    } else if (!strncasecmp(soc, "cherokee", sizeof("cherokee"))) {
      soc_type_ = BT_SOC_CHEROKEE;
    } else if (!strncasecmp(soc, "ath3k", sizeof("ath3k"))) {
      soc_type_ = BT_SOC_AR3K;
    } else if (!strncasecmp(soc, "napier", sizeof("napier"))) {
      soc_type_ = BT_SOC_NAPIER;
    } else if (!strncasecmp(soc, "naples_uart", sizeof("naples_uart"))) {
      soc_type_ = BT_SOC_NAPLES_UART;
    } else {
      ALOGI("qcom.bluetooth.soc not set, so using pronto.\n");
      soc_type_ = BT_SOC_DEFAULT;
    }
  } else {
    ALOGE("%s: Failed to get soc type, so using pronto", __func__);
    soc_type_ = BT_SOC_DEFAULT;
  }
  return soc_type_;
}

// this is used to send the actual packet.
size_t DataHandler::SendData(ProtocolType ptype, HciPacketType packet_type,
                             const uint8_t *data, size_t length)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  {
    it = protocol_info_.find(ptype);
    if (it == protocol_info_.end()) {
      ALOGE("%s: NO entry found for the protocol %d \n", __func__, ptype);
      return 0;
    }
    if (init_status_ != INIT_STATUS_SUCCESS) {
      ALOGE("%s: BT Daemon not initialized, ignore packet", __func__);
      return 0;
    }
  }
  logger_->ProcessTx(packet_type, data, length);
  return controller_->SendPacket(packet_type, data, length);
}

void DataHandler::OnPacketReady(ProtocolType ptype, HciPacketType type,
                                const hidl_vec<uint8_t>*hidl_data)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  bool is_ctlr_log_pkt = false;

  uint16_t len = hidl_data->size();
  const uint8_t* data = hidl_data->data();

  logger_->ProcessRx(type, data, len);

  if (logger_->IsControllerLogPkt(type, data, len)) {
    ALOGD("%s:Received a controller log packet\n", __func__);
    if(!logger_->IsHciFwSnoopEnabled()) {
      delete hidl_data;
      return;
    }
  }

  if (type == HCI_PACKET_TYPE_EVENT && data_handler->isAwaitedEvent(data, len)) {
    ALOGW("%s: Received event for command sent internally: %02x %02x \n",
            __func__, data[3], data[4]);
    delete hidl_data;
    event_wait = true;
    event_wait_cv.notify_all();
    return;
  }

  ProtocolCallbacksType *cb_data = nullptr;
  {
    std::unique_lock<std::mutex> guard(internal_mutex_);
    // update the pending Init cb and other callbacks
    it = protocol_info_.find(ptype);
    if (it != protocol_info_.end()) {
      cb_data = (ProtocolCallbacksType*)it->second;
    }
  }
  // execute callbacks here
  if(cb_data != nullptr) {
    cb_data->data_read_cb(type, hidl_data);
  }
  delete hidl_data;
}

// signal handler
void DataHandler::usr1_handler(int s)
{
  bool status = TRUE;

  ALOGI("%s: exit\n", __func__);
  pthread_exit(&status);
}

bool DataHandler::Open(ProtocolType type, InitializeCallback init_cb,
                       DataReadCallback data_read_cb)
{
  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  std::unique_lock<std::mutex> guard(internal_mutex_);
  ALOGI("Open init_status %d \n", init_status_);

  // update the pending Init cb and other callbacks
  it = protocol_info_.find(type);
  if (it == protocol_info_.end()) {
    ProtocolCallbacksType *cb_data  = new (ProtocolCallbacksType);
    cb_data->type = type;
    cb_data->is_pending_init_cb = true;
    cb_data->init_cb = init_cb;
    cb_data->data_read_cb = data_read_cb;
    protocol_info_[type] = cb_data;
  }
  switch (init_status_) {
    case INIT_STATUS_INITIALIZING:
      return true;
      break;
    case INIT_STATUS_SUCCESS:
      it = protocol_info_.find(type);
      if (it != protocol_info_.end()) {
        ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it->second;
        cb_data->is_pending_init_cb = false;
        cb_data->init_cb(true);
      }
      return true;
      break;
    case INIT_STATUS_FAILED:
      init_thread_.join();
    // fallback to below case
    case INIT_STATUS_IDLE:
      init_status_ = INIT_STATUS_INITIALIZING;
#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Init();
#endif
      break;
  }

  init_thread_ = std::thread([this]() {
    bool status = false;
    struct sigaction old_sa, sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = usr1_handler;
    sigaction(SIGUSR1, &sa, &old_sa);

    if(soc_type_ == BT_SOC_SMD) {
      controller_ = static_cast<Controller *> (new MctController(soc_type_));
    } else {
      controller_ = static_cast<Controller *> (new UartController(soc_type_));
    }
    if (controller_) {
      status = controller_->Init([this](ProtocolType ptype, HciPacketType type,
                                        const hidl_vec<uint8_t> *hidl_data)   {
                                   OnPacketReady(ptype, type, hidl_data);
                                 });
    }

    std::unique_lock<std::mutex> guard(internal_mutex_);
    if (status) {
      init_status_ = INIT_STATUS_SUCCESS;
      ALOGI("Firmware download succeded\n");
    } else {
      ALOGW("Controller Init failed ");
      init_status_ = INIT_STATUS_FAILED;
      delete controller_;
      controller_ = nullptr;

#ifdef DUMP_IPC_LOG
      logger_->DumpUartIpcLogs();
#endif

#ifdef DUMP_RINGBUF_LOG
      logger_->DumpRingBuffers();
#endif
      //Reset logger so that we can get correct log file index next time.
      logger_->Cleanup();
    }

    std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
    for (auto& it: protocol_info_) {
      ProtocolCallbacksType *cb_data = (ProtocolCallbacksType*)it.second;
      cb_data->is_pending_init_cb = false;
      cb_data->init_cb(status);
    }
    // clear the list if the controller open call fails
    if (!status) {
      protocol_info_.clear();
    }
  });
  return true;
}

bool DataHandler::Close(ProtocolType type)
{
  ALOGD("%s: Wait for collecting crash dump to finish\n", __func__);
  if (!controller_) {
#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif
    return false;
  }
  controller_->WaitforCrashdumpFinish();

  std::map<ProtocolType, ProtocolCallbacksType *>::iterator it;
  std::unique_lock<std::mutex> guard(internal_mutex_);
  bool status = false;

  it = protocol_info_.find(type);
  if (it != protocol_info_.end()) {
    ProtocolCallbacksType *cb_data =
      reinterpret_cast<ProtocolCallbacksType *> (it->second);
    delete (cb_data);
    protocol_info_.erase(it);
  }

  ALOGI("DataHandler:: init_status %d", init_status_);

  if (!protocol_info_.size()) {
    protocol_info_.clear();

    if(data_handler && data_handler->soc_type_ == BT_SOC_CHEROKEE ) {
      data_handler->sendCommandWait(SOC_PRE_SHUTDOWN_CMD);
    }

    guard.unlock();
    if (init_status_ == INIT_STATUS_SUCCESS) {
      controller_->Cleanup();
      ALOGW("controller Cleanup done");
      delete controller_;
      controller_ = nullptr;
    }
    guard.lock();

    if (init_status_ > INIT_STATUS_IDLE ) {
      if ( INIT_STATUS_INITIALIZING == init_status_) {
       pthread_kill(init_thread_.native_handle(), SIGUSR1);

#ifdef DUMP_IPC_LOG
        logger_->DumpUartIpcLogs();
#endif

#ifdef DUMP_RINGBUF_LOG
        logger_->DumpRingBuffers();
#endif

        // close the transport
        controller_->Disconnect();

#ifdef WAKE_LOCK_ENABLED
        Wakelock :: CleanUp();
#endif

        ALOGE("Killing daemon to recover as firmware download is stuck ");
        kill(getpid(), SIGKILL);

        //Reset logger so that we can get correct log file index next time.
        logger_->Cleanup();
      }

      if (init_thread_.joinable()) {
        init_thread_.join();
        ALOGI("DataHandler:: joined Init thread \n");
      }

      init_status_ = INIT_STATUS_IDLE;
    }

#ifdef WAKE_LOCK_ENABLED
    Wakelock :: CleanUp();
#endif

    status = true;
  }

  return status;
}

void DataHandler::sendCommandWait(HciCommand cmd)
{
  std::unique_lock<std::mutex> guard(evt_wait_mutex_);
  if(init_status_ != INIT_STATUS_SUCCESS)
    return;

  switch (cmd) {
    case HCI_RESET_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x03, 0x0C, 0x00};
      int length = 3;

      ALOGI("Sending HCI Reset \n");
      controller_->SendPacket(packet_type, data, length);
      awaited_evt = *(uint16_t *)(&data);
      break;
    }

    case SOC_PRE_SHUTDOWN_CMD:
    {
      HciPacketType packet_type = HCI_PACKET_TYPE_COMMAND;
      const uint8_t data[] = {0x08, 0xFC, 0x00};
      int length = 3;

      ALOGI("Sending Pre-shutdown command \n");
      controller_->SendPacket(packet_type, data, length);
      awaited_evt = *(uint16_t *)(&data);
      break;
    }

    default:
      return;
  }

  event_wait = false;
  event_wait_cv.wait_for(guard, std::chrono::milliseconds(500), []{return event_wait;});
  awaited_evt = 0;
}

inline bool DataHandler::isAwaitedEvent(const uint8_t *buff, uint16_t len)
{
  if(len < 5)
    return false;

  if(awaited_evt && (*(uint16_t *)(&buff[3])) == awaited_evt)
    return true;
  else
    return false;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
