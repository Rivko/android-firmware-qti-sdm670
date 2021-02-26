/*==========================================================================
Description
  This file taps regular BT UART data (hci command, event, ACL packets)
  and writes into QXDM. It also writes the controller logs (controller's
  printf strings, LMP RX and TX data) received from BT SOC over the UART.

# Copyright (c) 2013, 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc..

===========================================================================*/
#pragma once

#include "diag/include/log.h"
#include "diag/include/diag_lsm.h"
#include "diagi.h"
#include "diaglogi.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "hci_transport.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

class DiagInterface
{
 private:
  static const uint16_t log_bt_hdr_size_;
  void FormatLmp(uint8_t *, uint8_t *, int);
  HciTransport *hci_transport_;
  void GetTime(char *);
  bool is_ssr_triggered_;
 public:
  DiagInterface();
  bool Init(HciTransport *hci_transport);
  void *SsrBtHandler(void *, uint16);
  void SendLogs(const uint8_t *, int, int);
  bool isSsrTriggered();
  void ResetSsrTriggeredFlag();
  void CleanUp();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
