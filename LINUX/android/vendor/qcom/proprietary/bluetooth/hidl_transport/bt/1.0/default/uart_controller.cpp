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

#include <hidl/HidlSupport.h>
#include "uart_controller.h"
#include "hci_uart_transport.h"
#include "power_manager.h"
#include <android-base/logging.h>
#include <cutils/properties.h>
#include <utils/Log.h>

#undef LOG_TAG
#define LOG_TAG "vendor.qti.bluetooth@1.0-uart_controller"

#define PROC_PANIC_PATH     "/proc/sysrq-trigger"
#define BT_MAX_HCI_PKT_SIZE (1024 + 76) /* Max pkt size + safe margin */
#define DIAG_SSR_TIMEOUT (5000) // SOC sends first packet after 4 sec once watchdog ssr is triggred
                                // So keeping this higher valie for all DIAG initiated SSRs
#define REG_SSR_TIMEOUT   (500) // Regular SSR timeout value(not diag created SSRs)
namespace {

using android::hardware::bluetooth::V1_0::implementation::UartController;

ProtocolType GetProtocol(HciPacketType pkt_type)
{
  ProtocolType type = TYPE_BT;

  switch (pkt_type) {
    case HCI_PACKET_TYPE_COMMAND:
    case HCI_PACKET_TYPE_ACL_DATA:
    case HCI_PACKET_TYPE_SCO_DATA:
    case HCI_PACKET_TYPE_EVENT:
      type = TYPE_BT;
      break;
    case HCI_PACKET_TYPE_ANT_CTRL:
    case HCI_PACKET_TYPE_ANT_DATA:
      type = TYPE_ANT;
      break;
    case HCI_PACKET_TYPE_FM_CMD:
    case HCI_PACKET_TYPE_FM_EVENT:
      type = TYPE_FM;
      break;
    default:
      break;
  }
  return type;
};

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

const int LEN_PACKET_TO_SOC = 1100;
const uint8_t CMD_TIMEOUT = 0x22;
const int BT_SSR_TRIGGERED = 0xee;
const int LENGTH_DBG_INFO_EVT = 6;
extern bool soc_need_reload_patch;
using PacketReadCallback = std::function<void(ProtocolType, HciPacketType, const hidl_vec<uint8_t>*)>;

UartController::UartController(BluetoothSocType soc_type)
  : soc_type_(soc_type), soc_crashed(false),
  hci_packetizer_([this](hidl_vec<uint8_t> *data) { OnPacketReady(data); })
{
  init_done_ = false;
  hci_transport_ = nullptr;
  is_cmd_timeout = false;
  force_special_byte_enabled_ = false;

  logger_ = Logger::Get();

  logger_->RegisterCrashDumpCallback([this](bool crash_dump_in_progress) {
    char value[PROPERTY_VALUE_MAX] = {'\0'};
#ifdef ENABLE_FW_CRASH_DUMP
    const char* default_value = "1"; //crash device after collecting crash dump
#else
    const char* default_value = "3"; //restart BT host after collecting crash dump
#endif

    property_get("persist.service.bdroid.ssrlvl", value, default_value);
    int ssr_level = atoi(value);
    ALOGI("%s: ssr_level set to %d\n", __func__, ssr_level);
    if (crash_dump_in_progress) {
      StopSocCrashWaitTimer();
      Logger::Get()->ResetSsrTriggeredFlag();
      //After SOC crashes, it will reset IBS to false
      soc_crashed = true;
    } else {
      SignalCrashDumpFinish();

      if ((ssr_level == 1 || ssr_level == 2) && (is_cmd_timeout == false)) {
        /* call kernel panic to report it to crashscope */
        bt_kernel_panic();
      } else if (ssr_level == 3){
        ReportSocFailure(true);
      }
      is_cmd_timeout = false;
    }
  });

  //force special byte to be sent to SOC
#ifdef ENABLE_FW_CRASH_DUMP
  const char* default_send_special_byte = "true";
#else
  const char* default_send_special_byte = "false";
#endif

  char value[PROPERTY_VALUE_MAX] = { 0 };
  if (property_get("wc_transport.force_special_byte", value,
                    default_send_special_byte)) {
    force_special_byte_enabled_ = (strcmp(value, "false") == 0) ? false : true;
  }

  soc_crash_wait_timer_state_ = TIMER_NOT_CREATED;
}

void UartController::bt_kernel_panic(void)
{
  char panic_set ='c';

  ALOGV("%s: open: %s", __func__, PROC_PANIC_PATH);
  int fd_sysrq = open(PROC_PANIC_PATH, O_WRONLY);
  if(fd_sysrq < 0) {
    ALOGE("%s: open (%s) fail - %s (%d)", __func__,
            PROC_PANIC_PATH, strerror(errno), errno);
    //ignore if panic path can't be opened
  }
  else if (write(fd_sysrq, &panic_set, 1) < 0) {
    ALOGE("%s: write (%s) fail - %s (%d)", __func__, PROC_PANIC_PATH, strerror(errno), errno);
  }

  return;
}

void UartController::WaitforCrashdumpFinish()
{
  if (soc_crash_wait_timer_state_ != TIMER_ACTIVE) {
   return;
  }

  std::unique_lock<std::mutex> lk(cv_m);
  if (cv.wait_for(lk, std::chrono::seconds(10), [this]{ 
    return (soc_crash_wait_timer_state_ != TIMER_ACTIVE);})) {
    ALOGD("%s: finished collecting crashdump\n", __func__);
  } else {
    ALOGD("%s: collecting crashdump timed out\n", __func__);
  }
}

void UartController::SignalCrashDumpFinish()
{
  std::lock_guard<std::mutex> lk(cv_m);
  ALOGD("%s: notify the waiting clients \n", __func__);
  cv.notify_all();
}

bool UartController::Init(PacketReadCallback pkt_read_cb)
{
  HciUartTransport* uart_transport = nullptr;
  PatchDLManager* patch_dl_manager = nullptr;
  char skip_patch_download[PROPERTY_VALUE_MAX];
  if (init_done_) {
    ALOGD("already initialized, return");
    return true;
  }

  read_cb_ = pkt_read_cb;
  power_manager_.Init(soc_type_);

  // power off the chip first
  power_manager_.SetPower(false);

  // power on the chip using power manager
  power_manager_.SetPower(true);

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: Acquire();
#endif

  // initialize the HCI transport
  if (!(uart_transport = new HciUartTransport())) {
    goto error;
  }

  hci_transport_ = static_cast<HciTransport*> (uart_transport);

  uart_transport->Init(soc_type_);
  patch_dl_manager = new PatchDLManager(soc_type_, uart_transport);
  sibs_enabled_ = patch_dl_manager->IsSibsEnabled();
  ALOGV("%s: sibs_enabled = %d\n ", __func__, sibs_enabled_);
#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_)
    IbsHandler::Init(uart_transport);
#endif

  uart_transport->ClockOperation(USERIAL_OP_CLK_ON);

  //Download the NVM/RAM patch
  property_get("wc_transport.skip_patch_dload", skip_patch_download, "false");
  if (strcmp(skip_patch_download, "true") != 0) {
    if (patch_dl_manager->PerformChipInit() < 0) {
    // Always turn UART clock off even if BT fail to boot.
      uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
      goto error;
    }
  } else {
    /* Change baud rate 115.2 kbps to 3Mbps*/
    if (patch_dl_manager->SetBaudRateReq() < 0) {
      ALOGE("%s: Baud rate change failed!", __func__);
      goto error;
    }
    ALOGI("%s: Baud rate changed successfully ", __func__);
  }
  // set up the fd watcher now
  fd_watcher_.WatchFdForNonBlockingReads(
    uart_transport->GetCtrlFd(),
    [this](int fd) { OnDataReady(fd); });

  /* (1) If IBS is enabled (default), UART clock can be voted on/off
         dynamically by IbsHandler. So it's suitable to turn UART
         clock off after patch downloading is completed.
     (2) If IBS is disabled, UART clock should always be turned on.
         Otherwise, BT fail to work after patch downloading. */
#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_) {
    uart_transport->ClockOperation(USERIAL_OP_CLK_OFF);
  }
#endif

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: ReleaseDelay(500);
#endif

  if (patch_dl_manager) {
    delete patch_dl_manager;
    patch_dl_manager = nullptr;
  }

  Logger::Get()->Init(hci_transport_);
  init_done_ = true;
  ALOGD("Init succeded");
  return init_done_;

 error:
  ALOGE("Init failed");
  if (patch_dl_manager) {
    delete patch_dl_manager;
    patch_dl_manager = nullptr;
  }

  if (hci_transport_) {
    hci_transport_->CleanUp();
    delete hci_transport_;
    hci_transport_ = nullptr;
  }

#ifdef WAKE_LOCK_ENABLED
  Wakelock :: Release();
#endif

  return init_done_;
}
void UartController::StartSocCrashWaitTimer(int ssrtimeout)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;
  uint32_t timeout_ms;

  ALOGV("%s", __func__);

  if (soc_crash_wait_timer_state_ == TIMER_NOT_CREATED) {
    se.sigev_notify_function = (void (*)(union sigval))SocCrashWaitTimeout;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = this;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &soc_crash_wait_timer_);
    if (status == 0) {
      ALOGV("%s: SoC Crash wait timer created", __func__);
      soc_crash_wait_timer_state_ = TIMER_CREATED;
    } else {
      ALOGE("%s: Error creating timer %d\n", __func__, status);
    }
  }

  if (soc_crash_wait_timer_state_ == TIMER_CREATED) {
    timeout_ms = ssrtimeout;
    ts.it_value.tv_sec = timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(soc_crash_wait_timer_, 0, &ts, 0);
    if (status < 0)
      ALOGE("%s:Failed to set soc Crash wait timer: %d", __func__, status);
    else {
      ALOGV("%s: time started", __func__);
      soc_crash_wait_timer_state_ = TIMER_ACTIVE;
    }
  }
}

bool UartController::StopSocCrashWaitTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;
  uint32_t timeout_ms;
  bool was_active = false;

  ALOGV("%s", __func__);

  if (soc_crash_wait_timer_state_ == TIMER_ACTIVE) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    was_active = true;
    status = timer_settime(soc_crash_wait_timer_, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop soc Crash wait timer", __func__);
    else if (status == 0) {
      ALOGV("%s: Soc crash timer Stopped", __func__);
      soc_crash_wait_timer_state_ = TIMER_CREATED;
    }
  }
  return was_active;
}

void UartController::CleanupSocCrashWaitTimer()
{
  if (soc_crash_wait_timer_state_ == TIMER_ACTIVE)
    StopSocCrashWaitTimer();
  if (soc_crash_wait_timer_state_ == TIMER_CREATED) {
    timer_delete(soc_crash_wait_timer_);
    soc_crash_wait_timer_state_ = TIMER_NOT_CREATED;
    soc_crash_wait_timer_ = NULL;
  }
}
void UartController::SsrCleanup(uint8_t reason)
{
  /*Indicate it by sending special byte */
  if (reason == CMD_TIMEOUT) {
    if (force_special_byte_enabled_ && !logger_->IsCrashDumpStarted()) {
      is_cmd_timeout = true;
      ALOGD("%s: SSR triggered due to command tout", __func__);
      SendSpecialBuffer();
    }
  } else {
    Cleanup();
  }
}

void UartController::SocCrashWaitTimeout(union sigval sig)
{
  UartController *uart_controller = static_cast<UartController*>(sig.sival_ptr);

  if (uart_controller) {
    uart_controller->ReportSocFailure(false);
  }
}

void UartController::ReportSocFailure(bool dumped_uart_log)
{
  unsigned char bt_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x10, 0x01, 0x0f };
  unsigned char fm_eve_buf[LENGTH_HW_ERROR_EVT] = { 0x1A, 0x01, 0x0f };

  ALOGD("%s", __func__);

  if (!dumped_uart_log) {
#ifdef DUMP_RINGBUF_LOG
    logger_->DumpRingBuffers();
#endif

#ifdef DUMP_IPC_LOG
    logger_->DumpUartIpcLogs();
#endif
  }

  if (read_cb_) {
    hidl_vec<uint8_t> *bt_packet_ = new hidl_vec<uint8_t>;
    hidl_vec<uint8_t> *fm_packet_ = new hidl_vec<uint8_t>;
    bt_packet_->resize(LENGTH_HW_ERROR_EVT);
    fm_packet_->resize(LENGTH_HW_ERROR_EVT);

    ALOGD("%s send soc failure", __func__);
    memcpy(fm_packet_->data(), fm_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_FM_EVENT), HCI_PACKET_TYPE_FM_EVENT, fm_packet_);

    memcpy(bt_packet_->data(), bt_eve_buf, LENGTH_HW_ERROR_EVT);
    read_cb_(GetProtocol(HCI_PACKET_TYPE_EVENT), HCI_PACKET_TYPE_EVENT, bt_packet_);;
  }

  CleanupSocCrashWaitTimer();
}

void UartController::SendSpecialBuffer()
{
  unsigned char buf[LEN_PACKET_TO_SOC];
  int len(0);
  int retval;

  memset(buf, 0xfb, LEN_PACKET_TO_SOC);

  ALOGV("%s: sending special bytes to crash SOC", __func__);

  HciUartTransport* uart_transport = static_cast<HciUartTransport*> (hci_transport_);
  retval = uart_transport->UartWrite(buf, LEN_PACKET_TO_SOC);

  if (retval <= 0) {
    ALOGE("%s: Write Err : %d (%s)", __func__, retval, strerror(errno));
    ReportSocFailure(false);
  } else {
    StartSocCrashWaitTimer(REG_SSR_TIMEOUT);
  }
}

/**
  * this is to filter out Get Controller Debug Info cmd from stack.
  * we should crash SOC and collect ram dump after receiving it.
  */
bool UartController::command_is_get_dbg_info(const unsigned char* buf, int len)
{
    bool result = true;
    int i;
    const unsigned char get_dbg_info[] = {0x5B, 0xFD, 0x00};

    ALOGV("%s: lenth = %d ", __func__, len);
    if ( len != 3) {
        result = false;
    }
    else {
        for (i=0; i<len; i++) {
             if(buf[i] != get_dbg_info[i])
                result = false;
        }
    }

    return result;
}

size_t UartController::SendPacket(HciPacketType packet_type, const uint8_t *data, size_t length)
{
  size_t size = 0;
  int ibs_status = 0;

  if (Logger::Get()->isSsrTriggered() || (soc_crash_wait_timer_state_== TIMER_ACTIVE)) {
    ALOGD("<%s: give up Tx since crash dump has started", __func__);
    if (Logger::Get()->isSsrTriggered() && (soc_crash_wait_timer_state_ != TIMER_ACTIVE)) {
      StartSocCrashWaitTimer(DIAG_SSR_TIMEOUT);
      Logger::Get()->ResetSsrTriggeredFlag();
    }
    WaitforCrashdumpFinish();
    return size;
  }

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && !soc_crashed) {
    IbsHandler* hIbs = IbsHandler::Get();
    if (!hIbs) {
      ALOGW("<%s: Uart socket has been closed", __func__);
      return size;
    } else {
      ibs_status = hIbs->WakeAssert();
      if(ibs_status){
#ifdef DUMP_RINGBUF_LOG
        logger_->DumpRingBuffers();
#endif

#ifdef DUMP_IPC_LOG
        logger_->DumpUartIpcLogs();
#endif
        hci_transport_->Disconnect();

        ALOGE("Killing daemon to recover as unable wake up SOC");
#ifdef USER_DEBUG
        abort();
#else
        kill(getpid(), SIGKILL);
#endif
      }
    }
  }
#endif

  if ((HCI_PACKET_TYPE_COMMAND == packet_type) &&
      command_is_get_dbg_info(data, length)) {
    ALOGD("<%s: received Get_DBG_INFO command", __func__);
    SsrCleanup(CMD_TIMEOUT);
  } else if (ibs_status == 0){
    size = hci_transport_->Write(packet_type, data, length);
  } else {
    ALOGE("%s: Dropping packet! packet type = %d ibs_status = %d", __func__, packet_type, ibs_status);
  }

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_ && (ibs_status == 0) && !soc_crashed) {
    IbsHandler::Get()->TransmitDone(true);
  }
#endif

  return size;
}

bool UartController::Disconnect()
{
  fd_watcher_.StopThreadRoutine();
  hci_transport_->Disconnect();
  return true;
}

bool UartController::Cleanup(void)
{
  if (!init_done_) {
    ALOGD("UartController:: already closed, return");
    return false;
  }

  // stop the fd watcher
  fd_watcher_.StopWatchingFileDescriptors();

  // Cleaning Logger which ensures close of diag port
  Logger::Get()->Cleanup();

#ifdef WCNSS_IBS_ENABLED
  if (sibs_enabled_) {
    IbsHandler::Get()->CleanUp();
  }
#endif

  // clean up the transport
  if (hci_transport_) {
    hci_transport_->Disconnect();
    delete hci_transport_;
    hci_transport_ = nullptr;
  }

  // turn off the chip
  power_manager_.SetPower(false);
  power_manager_.Cleanup();

  init_done_ = false;
  return true;
}

void UartController::OnPacketReady(hidl_vec<uint8_t> *data)
{
  if(data == nullptr) {
    ALOGE("Error reading data from uart");
    ReportSocFailure(false);
    return;
  }

  if (data->size() > BT_MAX_HCI_PKT_SIZE) {
    ALOGE("Received large pkt %u from soc trigger ssr",
           data->size());
    SsrCleanup(CMD_TIMEOUT);
    return;
  }

  if (read_cb_) {
    read_cb_(GetProtocol(hci_packet_type_), hci_packet_type_, data);
  }
  // Get ready for the next type byte.
  hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
}

void UartController::OnDataReady(int fd)
{
  if (hci_packet_type_ == HCI_PACKET_TYPE_UNKNOWN) {
    uint8_t buffer[1] = { 0 };
    size_t bytes_read = TEMP_FAILURE_RETRY(read(fd, buffer, 1));
    CHECK(bytes_read == 1);

    hci_packet_type_ = static_cast<HciPacketType>(buffer[0]);

#ifdef WCNSS_IBS_ENABLED
    if (sibs_enabled_) {
      if (IbsHandler::Get()->IsIbsCmd(hci_packet_type_)) {
        IbsHandler::Get()->ProcessIbsCmd( reinterpret_cast <uint8_t *> (&hci_packet_type_));
        hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
        return;
      }
    }
#endif

    if (!IsHciPacketValid(hci_packet_type_)) {
      ALOGE("Invalid hci packet type byte received 0x%x", hci_packet_type_);
      hci_packet_type_ = HCI_PACKET_TYPE_UNKNOWN;
    }
  } else {
    hci_packetizer_.OnDataReady(fd, hci_packet_type_);
  }
}

bool UartController::IsHciPacketValid(HciPacketType type)
{
  bool result = false;

  if (HCI_PACKET_TYPE_EVENT == type || HCI_PACKET_TYPE_ACL_DATA == type ||
      HCI_PACKET_TYPE_SCO_DATA == type || HCI_PACKET_TYPE_COMMAND == type ||
      HCI_PACKET_TYPE_ANT_CTRL == type || HCI_PACKET_TYPE_ANT_DATA == type ||
      HCI_PACKET_TYPE_FM_CMD == type || HCI_PACKET_TYPE_FM_EVENT == type) {
    result = true;
  }

  return result;
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
