/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h"
#include "telephony/ril.h"

RilRequestCdmaSetBroadcastSmsConfigMessage::~RilRequestCdmaSetBroadcastSmsConfigMessage() {
  delete broadcast_sms_config_info;
  delete static_cast<RIL_CDMA_BroadcastSmsConfigInfo**>(params.data);
}

void RilRequestCdmaSetBroadcastSmsConfigMessage::deepCopy(const ril_request_type &request) {
  params.instance_id = request.instance_id;
  params.modem_id = QCRIL_DEFAULT_MODEM_ID;
  params.event_id = request.req_id;
  params.t = request.t;
  params.datalen = request.payload_len;

  RIL_CDMA_BroadcastSmsConfigInfo **payload = static_cast<RIL_CDMA_BroadcastSmsConfigInfo**>(const_cast<void*>(request.payload));
  RIL_CDMA_BroadcastSmsConfigInfo **payload_copy = nullptr;
  size_t num_elements;

  if (payload && params.datalen >= sizeof(RIL_CDMA_BroadcastSmsConfigInfo*)) {
    num_elements = params.datalen / sizeof(RIL_CDMA_BroadcastSmsConfigInfo*);
    broadcast_sms_config_info = new RIL_CDMA_BroadcastSmsConfigInfo[num_elements];
    if (broadcast_sms_config_info) {
      payload_copy = new RIL_CDMA_BroadcastSmsConfigInfo*[num_elements];
      if (payload_copy) {
        while (num_elements > 0) {
          num_elements--;
          memcpy(&broadcast_sms_config_info[num_elements], payload[num_elements], sizeof(RIL_CDMA_BroadcastSmsConfigInfo));
          payload_copy[num_elements] = &broadcast_sms_config_info[num_elements];
        }
      }
    }
  }

  params.data = payload_copy;
}

string RilRequestCdmaSetBroadcastSmsConfigMessage::dump() {
  return mName;
}
