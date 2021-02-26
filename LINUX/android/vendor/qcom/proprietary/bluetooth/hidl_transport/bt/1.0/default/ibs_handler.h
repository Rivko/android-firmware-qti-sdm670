/*==========================================================================
Description
  Wcnss_ibs has the state-machine and logic to process sleep bytes to excercise
  SIBS (Software In-band sleep) protocol

# Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once

#include <hidl/HidlSupport.h>
#include "hci_uart_transport.h"
#include "wake_lock.h"
#include "logger.h"

#define IBS_WAKE_ACK        0xFC
#define IBS_WAKE_IND        0xFD
#define IBS_SLEEP_IND       0xFE
/* LPM WAKE state */
enum {
  WCNSS_IBS_WAKE_DEASSERTED = 0,              /* initial state */
  WCNSS_IBS_WAKE_W4_TX_DONE,
  WCNSS_IBS_WAKE_W4_TIMEOUT,
  WCNSS_IBS_WAKE_ASSERTED
};
/* low power mode control block */
typedef struct
{
  //uint8_t state;                          /* Low power mode state */
  uint8_t wake_state;                     /* LPM WAKE state */
  uint8_t no_tx_data;
  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
}BluetoothLpm;

/* HCI_IBS transmit side sleep protocol states */
typedef enum {
  IBS_TX_ASLEEP = 0,
  IBS_TX_WAKING,
  IBS_TX_AWAKE,
}tx_ibs_states;

/* HCI_IBS receive side sleep protocol states */
typedef enum {
  IBS_RX_ASLEEP = 0,
  IBS_RX_AWAKE,
}rx_ibs_states;

typedef enum {
  IBS_TX_VOTE_CLOCK_ON = 0,
  IBS_TX_VOTE_CLOCK_OFF,
  IBS_RX_VOTE_CLOCK_ON,
  IBS_RX_VOTE_CLOCK_OFF,
} ibs_clock_state_vote;

typedef struct {
  pthread_mutex_t wack_lock;
  pthread_mutex_t hci_tx_ibs_lock;
  pthread_mutex_t clock_vote_lock;
  uint8_t tx_ibs_state;
  uint8_t tx_vote;
  uint8_t rx_ibs_state;
  uint8_t rx_vote;

  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
  uint8_t CLK_STATE;
}IbsStateMachine;

typedef struct {
  uint8_t timer_created;
  timer_t timer_id;
  uint32_t timeout_ms;
  uint8_t clk_vote;
}IbsVoteOffTimer;

#define WAKELOCK_PATH "/sys/power/wake_lock"
#define WAKEUNLOCK_PATH "/sys/power/wake_unlock"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef WCNSS_IBS_ENABLED

class IbsHandler {
 public:
  static IbsHandler* Init(HciUartTransport* theTransport);
  static IbsHandler* Get();
  static void CleanUp();
  static bool IsEnabled();
  ~IbsHandler();
  int WakeAssert(void);
  void ProcessIbsCmd(uint8_t *data);
  void TransmitDone(uint8_t tx_done);
  bool IsIbsCmd(unsigned char byte);

 private:
  IbsHandler(HciUartTransport* theTransport);
  void InitInt ();
  void SerialClockVote(uint8_t vote);
  static void WakeRetransTimeout(union sigval sig);
  void StartWackTimer(void);
  void StopWackTimer(void);
  void StartVoteOffTimer();
  void StopVoteOffTimer();
  static void VoteOffTimeout(union sigval sig);
  static void IdleTimeout(union sigval sig);
  void WriteIbsCmd(uint8_t ibs_data);
  void VoteOnClock();
  void VoteOffClock();
  int DeviceWakeUp();
  void DeviceSleep();
  void StartIdleTimer(void);
  void StopIdleTimer(void);

  static IbsHandler* instance_;
  int tty_fd_;
  HciUartTransport* uart_transport_;
  IbsStateMachine state_machine_;
  BluetoothLpm bt_lpm_;
  IbsVoteOffTimer voteoff_timer_;
  Logger* bt_logger_;
  pthread_cond_t wack_cond_;
  unsigned char wack_recvd_;
  unsigned int num_try_;
  unsigned int delayed_wakelock;
};

#endif

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android
