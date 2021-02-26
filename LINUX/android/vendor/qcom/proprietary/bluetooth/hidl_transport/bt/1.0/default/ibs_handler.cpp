/*==========================================================================
Description
  Wcnss_ibs has the state-machine and logic to process sleep bytes to excercise
  SIBS (Software In-band sleep) protocol

# Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
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
#include <ibs_handler.h>
#include "hci_uart_transport.h"

#define LOG_TAG "vendor.qti.bluetooth@1.0-ibs_handler"

#define TX_IDLE_DELAY           (10)
#define UART_VOTEOFF_DELAY      (100)
#define WAKELOCK_RELEASE_DELAY  (150)
#define CLOCK_OFF               (0)
#define CLOCK_ON                (1)

#define NUM_WACK_RETRANSMIT      (128)
#define DEFAULT_LPM_IDLE_TIMEOUT (1000)

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef WCNSS_IBS_ENABLED

IbsHandler *IbsHandler::instance_ = NULL;
IbsHandler *IbsHandler::Init(HciUartTransport* theTransport)
{
  if (!instance_) {
    instance_ = new IbsHandler(theTransport);
  }
  instance_->InitInt();
  return instance_;
}

void IbsHandler::InitInt()
{
  int status = 0;

  memset(&bt_lpm_, 0, sizeof(bt_lpm_));
  memset(&voteoff_timer_, 0, sizeof(voteoff_timer_));
  bt_lpm_.timeout_ms = DEFAULT_LPM_IDLE_TIMEOUT;

  tty_fd_ = uart_transport_->GetCtrlFd();
  ALOGD("%s: tty_fd = %d", __func__, tty_fd_);
  memset(&state_machine_, 0, sizeof(state_machine_));
  state_machine_.CLK_STATE = CLOCK_OFF;
  num_try_ = 0;

  struct sigevent se;
  se.sigev_notify_function = VoteOffTimeout;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &voteoff_timer_.timer_id;
  se.sigev_notify_attributes = NULL;

  status = timer_create(CLOCK_MONOTONIC, &se, &voteoff_timer_.timer_id);
  if (status == 0) {
    ALOGV("%s: Vote off timer created", __func__);
    voteoff_timer_.timer_created = true;
  }

  delayed_wakelock = property_get_int32("persist.set.rx_delay", WAKELOCK_RELEASE_DELAY);
}

IbsHandler * IbsHandler::Get()
{
  return instance_;
}

void IbsHandler::CleanUp(void)
{
  if (instance_) {
    delete instance_;
    instance_ = NULL;
  }
}

bool IbsHandler::IsEnabled()
{
  char value[PROPERTY_VALUE_MAX] = { '\0' };

  property_get("persist.service.bdroid.sibs", value, "true");
  return (strcmp(value, "true") == 0) ? true : false;
}

IbsHandler::IbsHandler(HciUartTransport* theTransport)
{
  ALOGD("%s", __func__);
  uart_transport_ = theTransport;
  bt_logger_ = Logger::Get();
}

IbsHandler::~IbsHandler()
{
  ALOGI("%s", __func__);
  if (bt_lpm_.timer_created == true) {
    timer_delete(bt_lpm_.timer_id);
    bt_lpm_.timer_created = false;
  }
  if (state_machine_.timer_created == true) {
    timer_delete(state_machine_.timer_id);
    state_machine_.timer_created = false;
  }
  if (voteoff_timer_.timer_created == true) {
    timer_delete(voteoff_timer_.timer_id);
    voteoff_timer_.timer_created = false;
  }
#ifdef WAKE_LOCK_ENABLED
  Wakelock :: Release();
#endif
}


bool IbsHandler::IsIbsCmd(unsigned char byte)
{
  if (byte == IBS_WAKE_ACK || byte == IBS_WAKE_IND || byte == IBS_SLEEP_IND) {
    return true;
  }
  return false;
}
void IbsHandler::WriteIbsCmd(uint8_t ibs_data)
{
  int ret;

  ret = uart_transport_->UartWrite(&ibs_data, 1);
  if (ret < 0) {
    ALOGE("%s: Failed to write IBS data %x", __func__, ibs_data);
  }

#ifdef DUMP_RINGBUF_LOG
  bt_logger_->SaveIbsToRingBuf(RingBuffer::HOST_TO_SOC, ibs_data);
#endif
}

void IbsHandler::WakeRetransTimeout(union sigval sig)
{
  uint8_t ibs_data;

  ALOGV("%s: Retransmit wake ind and restart timer", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  if (instance_->num_try_ == NUM_WACK_RETRANSMIT) {
    ALOGD("%s: Failed to get wake ack from the SOC ", __func__);
    instance_->wack_recvd_ = false;
    pthread_cond_signal(&instance_->wack_cond_);
    instance_->SerialClockVote(IBS_TX_VOTE_CLOCK_OFF);
    return;
  }
  instance_->StartWackTimer();
  ibs_data = IBS_WAKE_IND;
  ALOGI("%s: Writing HCI_IBS_WAKE_IND", __func__);
  if (instance_->tty_fd_ > 0)
    instance_->WriteIbsCmd(ibs_data);
  else {
    ALOGE("%s: Invalid FD", __func__);
    return;
  }
  instance_->num_try_++;
}

void IbsHandler::StopWackTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);
  num_try_ = 0;
  if (state_machine_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop set wack timer", __func__);
    else if (status == 0)
      ALOGV("%s: Wack timer Stopped", __func__);
  }
}

void IbsHandler::SerialClockVote(uint8_t vote)
{
  uint8_t new_vote;

  pthread_mutex_lock(&state_machine_.clock_vote_lock);
  uint8_t old_vote = (state_machine_.tx_vote | state_machine_.rx_vote);
  ALOGV("%s", __func__);

  switch (vote) {
    case IBS_TX_VOTE_CLOCK_ON:
      state_machine_.tx_vote = 1;
      new_vote = 1;
      break;
    case IBS_RX_VOTE_CLOCK_ON:
      state_machine_.rx_vote = 1;
      new_vote = 1;
      break;
    case IBS_TX_VOTE_CLOCK_OFF:
      state_machine_.tx_vote = 0;
      new_vote = state_machine_.rx_vote | state_machine_.tx_vote;
      break;
    case IBS_RX_VOTE_CLOCK_OFF:
      state_machine_.rx_vote = 0;
      new_vote = state_machine_.rx_vote | state_machine_.tx_vote;
      break;
    default:
      ALOGE("SerialClockVote: Wrong vote requested!\n");
      pthread_mutex_unlock(&state_machine_.clock_vote_lock);
      return;
  }
  ALOGV("new_vote: (%d) ## old-vote: (%d)", new_vote, old_vote);

  if (new_vote != old_vote) {
    if (new_vote) {
      /*vote UART CLK ON using UART driver's ioctl() */
      ALOGI("%s: vote for UART CLK ON", __func__);
      VoteOnClock();
      state_machine_.CLK_STATE = CLOCK_ON;
#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Acquire();
#endif
    } else {
      /*vote UART CLK OFF using UART driver's ioctl() */
      ALOGI("%s: vote for UART CLK OFF", __func__);
      VoteOffClock();
      state_machine_.CLK_STATE = CLOCK_OFF;
#ifdef WAKE_LOCK_ENABLED
      Wakelock :: ReleaseDelay(delayed_wakelock);
#endif
    }
  }
  pthread_mutex_unlock(&state_machine_.clock_vote_lock);
}

void IbsHandler::StartWackTimer()
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (state_machine_.timer_created == false) {
    se.sigev_notify_function = (void (*)(union sigval))WakeRetransTimeout;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &state_machine_.timer_id;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &state_machine_.timer_id);
    if (status == 0) {
      ALOGV("%s: WACK timer created", __func__);
      state_machine_.timer_created = true;
    }
  }
  if (state_machine_.timer_created == true) {
    if (!state_machine_.timeout_ms)
      state_machine_.timeout_ms = TX_IDLE_DELAY;
    ts.it_value.tv_sec = state_machine_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000000 * (state_machine_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(state_machine_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to set wack timer", __func__);
  }
}
void IbsHandler::VoteOffTimeout(union sigval sig)
{
  ALOGI("%s: uart serival vote off", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  instance_->state_machine_.CLK_STATE = CLOCK_OFF;
  instance_->VoteOffClock();
}

void IbsHandler::StopVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  if (voteoff_timer_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s:Failed to stop set wack timer", __func__);
    else if (status == 0)
      ALOGV("%s: Wack timer Stopped", __func__);
  }
}
void IbsHandler::StartVoteOffTimer()
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);

  voteoff_timer_.timeout_ms = UART_VOTEOFF_DELAY;
  ts.it_value.tv_sec = voteoff_timer_.timeout_ms / 1000;
  ts.it_value.tv_nsec = 1000000 * (voteoff_timer_.timeout_ms % 1000);
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_settime(voteoff_timer_.timer_id, 0, &ts, 0);
  if (status == -1)
    ALOGE("%s:Failed to set wack timer", __func__);
}

int IbsHandler::DeviceWakeUp()
{
  uint8_t ibs_data;
  int status = 0;

  ALOGV("%s", __func__);
  pthread_mutex_lock(&state_machine_.hci_tx_ibs_lock);
  switch (state_machine_.tx_ibs_state) {
    case IBS_TX_ASLEEP:
      ALOGV("%s: TX state ASLEEP, acquire SM lock", __func__);
      StopVoteOffTimer();
      state_machine_.tx_ibs_state = IBS_TX_WAKING;
      ALOGV("%s: UART TX Vote ON", __func__);
      SerialClockVote(IBS_TX_VOTE_CLOCK_ON);
      ALOGV("%s: Acquire wake lock", __func__);
      pthread_mutex_lock(&state_machine_.wack_lock);
      StartWackTimer();
      wack_recvd_ = false;
      ibs_data = IBS_WAKE_IND;
      ALOGI("%s: Writing IBS_WAKE_IND", __func__);
      if (tty_fd_ > 0)
        WriteIbsCmd(ibs_data);
      else {
        ALOGE("%s: Invalid FD", __func__);
        ALOGV("%s: Realease wake lock and SM lock", __func__);
        pthread_mutex_unlock(&state_machine_.wack_lock);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return -1;
      }
      while (wack_recvd_ == false) {
        pthread_cond_wait(&wack_cond_, &state_machine_.wack_lock);
        ALOGV("%s: Get wake ack from the SOC", __func__);
        if (num_try_ == NUM_WACK_RETRANSMIT) {
          ALOGE("%s:SoC not responding,stop sending wake byte", __func__);
          StopWackTimer();
          break;
        }
      }
      if (wack_recvd_ == true) {
        ALOGV("%s: Received wack ack", __func__);
        ALOGV("%s: TX state transition to AWAKE", __func__);
        state_machine_.tx_ibs_state = IBS_TX_AWAKE;
        wack_recvd_ = false;
        status = 0;
      } else {
        ALOGE("%s: Failed to wake SOC", __func__);
        state_machine_.tx_ibs_state = IBS_TX_ASLEEP;
        StopWackTimer();
        wack_recvd_ = false;
        status = -1;
      }
      ALOGV("%s: Realease wake lock and SM lock", __func__);
      pthread_mutex_unlock(&state_machine_.wack_lock);
      break;
    case IBS_TX_WAKING:
      ALOGV("%s: SOC is WAKING UP", __func__);
      break;
    case IBS_TX_AWAKE:
      ALOGV("%s: SOC is already AWAKE", __func__);
      break;
  }
  pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
  return status;
}

void IbsHandler::ProcessIbsCmd(uint8_t *data)
{
  uint8_t ibs_data;
  int status = 0;

  ALOGV("%s", __func__);

#ifdef DUMP_RINGBUF_LOG
  bt_logger_->SaveIbsToRingBuf(RingBuffer::SOC_TO_HOST, *data);
#endif

  switch (*data) {
    case IBS_WAKE_IND: //0xFD
      ALOGI("%s: Received IBS_WAKE_IND: 0xFD", __func__);
      StopVoteOffTimer();
      switch (state_machine_.rx_ibs_state) {
        case IBS_RX_ASLEEP:
          ALOGV("%s: UART RX Vote ON", __func__);
          SerialClockVote(IBS_RX_VOTE_CLOCK_ON);
          state_machine_.rx_ibs_state = IBS_RX_AWAKE;
          ibs_data = IBS_WAKE_ACK;
          ALOGI("%s: Writing IBS_WAKE_ACK", __func__);

          if (tty_fd_ > 0) {
            ALOGV("%s: Lock to write", __func__);
            WriteIbsCmd(ibs_data);
          } else {
            ALOGE("%s: Invalid FD", __func__);
            ALOGV("%s: Realease SM lock", __func__);
            return;
          }
          break;
        case IBS_RX_AWAKE:
          ibs_data = IBS_WAKE_ACK;
          ALOGI("%s: Writing IBS_WAKE_ACK", __func__);
          if (tty_fd_ > 0 )
            WriteIbsCmd(ibs_data);
          else {
            ALOGE("%s: Invalid FD", __func__);
            return;
          }
          break;
      }
      break;
    case IBS_SLEEP_IND: //0xFE
      ALOGI("%s: Received IBS_SLEEP_IND: 0xFE", __func__);
      switch (state_machine_.rx_ibs_state) {
        case IBS_RX_AWAKE:
          ALOGV("%s: RX path is awake, Vote Off uart", __func__);
          state_machine_.rx_ibs_state = IBS_RX_ASLEEP;
          ALOGV("%s: UART RX Vote Off", __func__);
          SerialClockVote(IBS_RX_VOTE_CLOCK_OFF);
          break;
        case IBS_RX_ASLEEP:
          ALOGV("%s: RX path is asleep", __func__);
          break;
      }
      break;
    case IBS_WAKE_ACK: ////0xFC
      switch (state_machine_.tx_ibs_state) {
        case IBS_TX_WAKING:
          ALOGI("%s: Received IBS_WAKE_ACK: 0xFC", __func__);
          pthread_mutex_lock(&state_machine_.wack_lock);
          ALOGV("%s: Signal wack_cond_", __func__);
          wack_recvd_ = true;
          StopWackTimer();
          pthread_cond_signal(&wack_cond_);
          pthread_mutex_unlock(&state_machine_.wack_lock);
          break;
        case IBS_TX_AWAKE:
          ALOGV("%s: TX SM is awake already, stop wack timer if running", __func__);
          StopWackTimer();
          break;
        default:
          ALOGE("%s: WAKE ACK from SOC, Unexpected TX state", __func__);

          if (pthread_mutex_trylock(&state_machine_.hci_tx_ibs_lock) == 0) {
            if (IBS_RX_AWAKE == state_machine_.rx_ibs_state) {
              ALOGV("%s: RX path is just awake, Vote Off uart again",__FUNCTION__);
              //Send Sleep Indication to SOC
              WriteIbsCmd(IBS_SLEEP_IND);
              state_machine_.rx_ibs_state = IBS_RX_ASLEEP;
              SerialClockVote(IBS_RX_VOTE_CLOCK_OFF);
            }

            pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
          }
          break;
      }
      break;
  }
}
void IbsHandler::DeviceSleep(void)
{
  uint8_t ibs_data;
  int status = 0;

  ALOGV("%s: Acquire SM lock", __func__);
  pthread_mutex_lock(&state_machine_.hci_tx_ibs_lock);
  switch (state_machine_.tx_ibs_state) {
    case IBS_TX_AWAKE:
      if (bt_lpm_.wake_state == WCNSS_IBS_WAKE_ASSERTED) {
        ALOGD("%s: Tx in progress", __func__);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return;
      }
      ALOGI("%s: TX Awake, Sending SLEEP_IND", __func__);
      ibs_data = IBS_SLEEP_IND;

      if (tty_fd_ > 0)
        WriteIbsCmd(ibs_data);
      else {
        ALOGE("%s: Invalid FD", __func__);
        pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
        return;
      }
      state_machine_.tx_ibs_state    = IBS_TX_ASLEEP;
      ALOGV("%s: UART TX Vote Off", __func__);
      SerialClockVote(IBS_TX_VOTE_CLOCK_OFF);
      break;
    case IBS_TX_ASLEEP:
      ALOGV("%s: TX Asleep", __func__);
      break;
    default:
      ALOGE("%s: Invalid TX SM", __func__);
      break;
  }
  pthread_mutex_unlock(&state_machine_.hci_tx_ibs_lock);
  return;
}

void IbsHandler::IdleTimeout(union sigval sig)
{
  ALOGV("%s: Deassert SOC", __func__);
  if (!instance_) {
    ALOGD("%s: IBS handler has been destroyed ", __func__);
    return;
  }
  instance_->DeviceSleep();
}

void IbsHandler::StartIdleTimer(void)
{
  int status;
  struct itimerspec ts;
  struct sigevent se;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == false) {
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &bt_lpm_.timer_id;
    se.sigev_notify_function = (void (*)(union sigval))IdleTimeout;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &bt_lpm_.timer_id);

    if (status == 0)
      bt_lpm_.timer_created = true;
  }

  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = bt_lpm_.timeout_ms / 1000;
    ts.it_value.tv_nsec = 1000 * (bt_lpm_.timeout_ms % 1000);
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("%s: Failed to set LPM idle timeout", __func__);
  }
}

void IbsHandler::StopIdleTimer(void)
{
  int status;
  struct itimerspec ts;

  ALOGV("%s", __func__);
  if (bt_lpm_.timer_created == true) {
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(bt_lpm_.timer_id, 0, &ts, 0);
    if (status == -1)
      ALOGE("[STOP] Failed to set LPM idle timeout");
 }
}

void IbsHandler::TransmitDone(uint8_t tx_done)
{
  ALOGV("%s", __func__);
  bt_lpm_.no_tx_data = tx_done;
  if (tx_done == true) {
    ALOGV("%s: tx_done. Start idle timeout", __func__);
    bt_lpm_.wake_state = WCNSS_IBS_WAKE_W4_TIMEOUT;
    StartIdleTimer();
  }
}
void IbsHandler::VoteOnClock()
{
  uart_transport_->ClockOperation(USERIAL_OP_CLK_ON);
}
void IbsHandler::VoteOffClock()
{
  StopIdleTimer();
  uart_transport_->ClockOperation(USERIAL_OP_CLK_OFF);
}

int IbsHandler::WakeAssert(void)
{
  int ret = 0;

  ALOGV("%s", __func__);
  StopIdleTimer();
  bt_lpm_.wake_state = WCNSS_IBS_WAKE_ASSERTED;
  ret = DeviceWakeUp();
  if (ret != -1) {
    TransmitDone(false);
  }
  return ret;
}

#endif

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
