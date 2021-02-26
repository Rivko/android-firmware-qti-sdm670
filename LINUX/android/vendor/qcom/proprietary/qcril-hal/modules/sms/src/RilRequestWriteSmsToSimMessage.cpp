/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestWriteSmsToSimMessage.h"
#include "telephony/ril.h"

RilRequestWriteSmsToSimMessage::~RilRequestWriteSmsToSimMessage() {
  RIL_SMS_WriteArgs *payload = static_cast<RIL_SMS_WriteArgs*>(params.data);
  delete payload->pdu;
  delete payload->smsc;
  delete payload;
}

void RilRequestWriteSmsToSimMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  RIL_SMS_WriteArgs *payload = static_cast<RIL_SMS_WriteArgs*>(const_cast<void*>(request.payload));
  RIL_SMS_WriteArgs *payload_copy = nullptr;

  if (payload) {
    char *pdu = nullptr;
    size_t pdu_len;
    if (payload->pdu) {
      pdu_len = strlen(payload->pdu) + 1;
      pdu = new char[pdu_len];
      if (pdu) {
        memcpy(pdu, payload->pdu, pdu_len);
      }
    }

    char *smsc = nullptr;
    size_t smsc_len;
    if (payload->smsc) {
      smsc_len = strlen(payload->smsc) + 1;
      smsc = new char[smsc_len];
      if (smsc) {
        memcpy(smsc, payload->smsc, smsc_len);
      }
    }

    payload_copy = new RIL_SMS_WriteArgs;
    if (payload_copy) {
      payload_copy->status = payload->status;
      payload_copy->pdu = pdu;
      payload_copy->smsc = smsc;
    }
  }

  params.data = payload_copy;
}

string RilRequestWriteSmsToSimMessage::dump() {
  return mName;
}
