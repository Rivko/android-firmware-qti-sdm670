/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/nas/NasSetNetworkSelectionManualRequestMessage.h"
#include "telephony/ril.h"

NasSetNetworkSelectionManualRequestMessage::~NasSetNetworkSelectionManualRequestMessage() {
  if (params != nullptr) {
    delete [](char *)params->data;
    delete params;
    params = nullptr;
  }
}

void NasSetNetworkSelectionManualRequestMessage::deepCopy(const ril_request_type &request) {
  do {
    params = new qcril_request_params_type;
    if (!params)
      break;

    // fill legacy params
    params->instance_id = request.instance_id;
    params->modem_id = QCRIL_DEFAULT_MODEM_ID;
    params->event_id = request.req_id;
    params->t = request.t;
    params->datalen = 0;
    params->data = nullptr;

    if (request.payload) {
        params->datalen = std::strlen((char*)request.payload);
        size_t buf_len = params->datalen + 1;
        params->data = new char[buf_len];
        if (params->data) {
            strlcpy(static_cast<char*>(params->data),
                static_cast<const char*>(request.payload), buf_len);
        } else {
            params->datalen = 0;
        }
    }

  } while (false);
}

string NasSetNetworkSelectionManualRequestMessage::dump() {
  return mName;
}

