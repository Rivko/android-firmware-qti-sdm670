/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestCdmaSendSmsMessage.h"
#include "telephony/ril.h"

RilRequestCdmaSendSmsMessage::~RilRequestCdmaSendSmsMessage() {
  delete static_cast<RIL_CDMA_SMS_Message*>(params.data);
}

void RilRequestCdmaSendSmsMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  RIL_CDMA_SMS_Message *payload = static_cast<RIL_CDMA_SMS_Message*>(const_cast<void*>(request.payload));
  RIL_CDMA_SMS_Message *cdma_sms_message = nullptr;
  if (payload) {
    cdma_sms_message = new RIL_CDMA_SMS_Message;
    if (cdma_sms_message) {
      memcpy(cdma_sms_message, payload, sizeof(RIL_CDMA_SMS_Message));
    }
  }

  params.data = cdma_sms_message;
}

string RilRequestCdmaSendSmsMessage::dump() {
  return mName;
}
