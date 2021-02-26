/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestSetSmscAddressMessage.h"
#include "telephony/ril.h"

RilRequestSetSmscAddressMessage::~RilRequestSetSmscAddressMessage() {
  delete static_cast<char*>(params.data);
}

void RilRequestSetSmscAddressMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  char *payload = static_cast<char*>(const_cast<void*>(request.payload));

  char *smsc_address = nullptr;
  size_t smsc_address_len;
  if (payload) {
    smsc_address_len = strlen(payload) + 1;
    smsc_address = new char[smsc_address_len];
    if (smsc_address) {
      memcpy(smsc_address, payload, smsc_address_len);
    }
  }

  params.data = smsc_address;
}

string RilRequestSetSmscAddressMessage::dump() {
  return mName;
}
