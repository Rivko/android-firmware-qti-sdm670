/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/RilRequestSetCallForwardMessage.h"
#include "telephony/ril.h"

RilRequestSetCallForwardMessage::~RilRequestSetCallForwardMessage() {
  if (params != nullptr) {
    if (params->data != nullptr) {
      RIL_CallForwardInfo* cfInfo = (RIL_CallForwardInfo*)params->data;
      if (cfInfo->number != nullptr) {
        delete[] cfInfo->number;
      }
      delete cfInfo;
    }
    delete params;
  }
}

void RilRequestSetCallForwardMessage::deepCopy(const ril_request_type &request) {
  params = new qcril_request_params_type;
  if (params != nullptr) {
    params->instance_id = request.instance_id;
    params->modem_id = QCRIL_DEFAULT_MODEM_ID;
    params->event_id = request.req_id;
    params->t = request.t;
    params->datalen = 0;
    params->data = nullptr;;

    auto cfInfo = new RIL_CallForwardInfo;
    if (cfInfo == nullptr) {
        delete params;
        params = nullptr;
        return;
    }

    params->datalen = request.payload_len;
    params->data = (void*)cfInfo;

    RIL_CallForwardInfo* payload = (RIL_CallForwardInfo*)request.payload;
    if (payload != nullptr) {
      memcpy(cfInfo, payload, sizeof(RIL_CallForwardInfo));
      if (payload->number != nullptr) {
        // XXX: any maximum checking?
        size_t length = strlen(payload->number);
        if ((cfInfo->number = new char[length + 1]) == nullptr) {
          delete params;
          delete cfInfo;
          params = nullptr;
          cfInfo = nullptr;
        } else {
          strlcpy(cfInfo->number, payload->number, length + 1);
        }
      } // number != nullptr
    } else { // payload != nullptr
        delete params;
        delete cfInfo;
        params = nullptr;
        cfInfo = nullptr;
    }
  } // params != nullptr
}

string RilRequestSetCallForwardMessage::dump() {
  return mName;
}
