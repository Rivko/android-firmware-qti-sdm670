/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/RilRequestDialMessage.h"
#include "telephony/ril.h"

RilRequestDialMessage::~RilRequestDialMessage() {
  if (params != nullptr) {
    RIL_Dial *dial = (RIL_Dial*)params->data;
    delete []dial->address;
    if (dial->uusInfo) {
      if (dial->uusInfo->uusData)
        delete []dial->uusInfo->uusData;
      delete dial->uusInfo;
    }
    delete dial;
    delete params;
  }
}

void RilRequestDialMessage::deepCopy(const ril_request_type &request) {
  RIL_Dial *dial = nullptr;
  bool success = false;

  do {
    params = new qcril_request_params_type;
    if (!params)
      break;

    // fill legacy params
    params->instance_id = request.instance_id;
    params->modem_id = QCRIL_DEFAULT_MODEM_ID;
    params->event_id = request.req_id;
    params->t = request.t;
    params->datalen = request.payload_len;
    params->data = nullptr;

    dial = new RIL_Dial;
    if (!dial)
      break;
    RIL_Dial* payload = (RIL_Dial*)request.payload;
    if (!payload)
      break;

    // copy the clir and address
    dial->clir = payload->clir;
    size_t addr_len = strlen(payload->address);
    dial->address = new char[addr_len+1];
    if (!dial->address)
      break;
    memcpy(dial->address, payload->address, addr_len + 1);

    // copy the uusInfo part
    if (payload->uusInfo) {
      dial->uusInfo = new RIL_UUS_Info;
      if (!dial->uusInfo)
        break;
      dial->uusInfo->uusType = payload->uusInfo->uusType;
      dial->uusInfo->uusDcs =  payload->uusInfo->uusDcs;
      if ((dial->uusInfo->uusLength = payload->uusInfo->uusLength)) {
        dial->uusInfo->uusData = new char[dial->uusInfo->uusLength];
        if (!dial->uusInfo->uusData || !payload->uusInfo->uusData)
          break;
        memcpy(dial->uusInfo->uusData, payload->uusInfo->uusData,
                     dial->uusInfo->uusLength);
      }
    } else {
      dial->uusInfo = nullptr;
    }
    success = true;
  } while (false);

  if (!success) { // free the allocated memory
    if (dial) {
      if (dial->address)
        delete []dial->address;
      if (dial->uusInfo) {
        if (dial->uusInfo->uusData)
          delete []dial->uusInfo->uusData;
        delete dial->uusInfo;
      }
      delete dial;
    }

    if (params) {
      delete params;
      params = nullptr;
    }
  } else {
    params->data = (void*)dial;
  }
}

string RilRequestDialMessage::dump() {
  return mName;
}
