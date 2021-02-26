/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/RilRequestSetFacilityLockMessage.h"
#include "telephony/ril.h"

RilRequestSetFacilityLockMessage::~RilRequestSetFacilityLockMessage() {
  if (params != nullptr) {
    const size_t elements = params->datalen / sizeof(char*);
    if (params->data) {
      const char** payload = (const char**)params->data;
      for (size_t i = 0; i < elements; i++) {
        if (payload[i])
          delete []payload[i];
      }
      delete []payload;
    }

    delete params;
    params = nullptr;
  }
}

void RilRequestSetFacilityLockMessage::deepCopy(const ril_request_type &request) {
  char** data = nullptr;
  bool success = false;
  const size_t elements = request.payload_len / sizeof(char*); // should be 4, need check?

  do {
    params = new qcril_request_params_type;
    if (params == nullptr)
      break;
    params->instance_id = request.instance_id;
    params->modem_id = QCRIL_DEFAULT_MODEM_ID;
    params->event_id = request.req_id;
    params->t = request.t;
    params->datalen = request.payload_len;
    params->data = nullptr;

    const char** payload = (const char**)request.payload;
    if (!payload || !elements)
      break;
    data = new char*[elements];
    if (data == nullptr)
      break;
    // fill as nullptr
    memset((void*)data, 0, request.payload_len);

    size_t i = 0;
    for (i = 0; i < elements; i++) {
      int length;
      if ((payload[i]) && (length = strlen(payload[i]))) {
        data[i] = new char[length + 1];
        if (data[i] == nullptr) break;
        strlcpy(data[i], payload[i], length + 1);
      }
    }
    if (i >= elements)
      success = true;

  } while (false);

  if (!success) {
    if (data) {
      for (size_t i = 0; i < elements; i++) {
        if (data[i])
          delete []data[i];
      }
      delete []data;
    }
    if (params)
      delete params;
    params = nullptr;
  } else {
    params->data = (void*)data;
  }
}

string RilRequestSetFacilityLockMessage::dump() {
  return mName;
}
