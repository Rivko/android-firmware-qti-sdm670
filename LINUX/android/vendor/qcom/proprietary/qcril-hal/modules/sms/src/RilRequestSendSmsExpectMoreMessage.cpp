/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestSendSmsExpectMoreMessage.h"
#include "telephony/ril.h"

RilRequestSendSmsExpectMoreMessage::~RilRequestSendSmsExpectMoreMessage() {
  char **payload = static_cast<char**>(params.data);
  delete payload[0];
  delete payload[1];
  delete payload;
}

void RilRequestSendSmsExpectMoreMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  const char **payload = static_cast<const char**>(const_cast<void*>(request.payload));
  char **payload_copy = nullptr;

  if (payload) {
    char *smsc_address = nullptr;
    if (payload[0]) {
      size_t smsc_address_len = strlen(payload[0]) + 1;
      smsc_address = new char[smsc_address_len];
      if (smsc_address) {
        memcpy(smsc_address, payload[0], smsc_address_len);
      }
    }

    char *sms = nullptr;
    if (payload[1]) {
      size_t sms_len = strlen(payload[1]) + 1;
      sms = new char[sms_len];
      if (sms) {
        memcpy(sms, payload[1], sms_len);
      }
    }

    payload_copy = new char*[2];
    if (payload_copy) {
      payload_copy[0] = smsc_address;
      payload_copy[1] = sms;
    }
  }

  params.data = payload_copy;
}

string RilRequestSendSmsExpectMoreMessage::dump() {
  return mName;
}
