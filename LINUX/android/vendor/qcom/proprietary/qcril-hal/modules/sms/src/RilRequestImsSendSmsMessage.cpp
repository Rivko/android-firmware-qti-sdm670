/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestImsSendSmsMessage.h"
#include "telephony/ril.h"

RilRequestImsSendSmsMessage::~RilRequestImsSendSmsMessage() {
  delete[] gsm_message;
  delete[] smsc_address;
  delete[] gw_pdu;
  delete cdma_sms_message;
  delete static_cast<RIL_IMS_SMS_Message*>(params.data);
}

void RilRequestImsSendSmsMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  RIL_IMS_SMS_Message *payload = static_cast<RIL_IMS_SMS_Message*>(const_cast<void*>(request.payload));
  RIL_IMS_SMS_Message *ims_sms_message = nullptr;

  if (payload) {
    ims_sms_message = new RIL_IMS_SMS_Message;
    if (ims_sms_message) {
      memcpy(ims_sms_message, payload, sizeof(RIL_IMS_SMS_Message));

      if (ims_sms_message->tech == RADIO_TECH_3GPP) {
        if (payload->message.gsmMessage) {
          gsm_message = new char*[2];
          if (gsm_message) {
            if (payload->message.gsmMessage[0]) {
              size_t smsc_address_len = strlen(payload->message.gsmMessage[0]) + 1;
              smsc_address = new char[smsc_address_len];
              if (smsc_address) {
                memcpy(smsc_address, payload->message.gsmMessage[0], smsc_address_len);
              }
            }
            gsm_message[0] = smsc_address;

            if (payload->message.gsmMessage[1]) {
              size_t gw_pdu_len = strlen(payload->message.gsmMessage[1]) + 1;
              gw_pdu = new char[gw_pdu_len];
              if (gw_pdu) {
                memcpy(gw_pdu, payload->message.gsmMessage[1], gw_pdu_len);
              }
            }
            gsm_message[1] = gw_pdu;
          }
        }
        ims_sms_message->message.gsmMessage = gsm_message;
      } else {
        if (payload->message.cdmaMessage) {
          cdma_sms_message = new RIL_CDMA_SMS_Message;
          if (cdma_sms_message) {
            memcpy(cdma_sms_message, payload->message.cdmaMessage, sizeof(RIL_CDMA_SMS_Message));
          }
        }
        ims_sms_message->message.cdmaMessage = cdma_sms_message;
      }
    }
  }

  params.data = ims_sms_message;
}

string RilRequestImsSendSmsMessage::dump() {
  return mName;
}
