/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <string.h>
#include "modules/voice/RilRequestCdmaBurstDtmfMessage.h"
#include "telephony/ril.h"
#include "framework/Log.h"

#define CDMA_BURST_DTMF_DATA_LEN 3

RilRequestCdmaBurstDtmfMessage::~RilRequestCdmaBurstDtmfMessage() {
  if (params.data) {
    qcril_free(((char **)params.data)[0]);
    qcril_free(((char **)params.data)[1]);
    qcril_free(((char **)params.data)[2]);
    qcril_free(params.data);
  }
}

void RilRequestCdmaBurstDtmfMessage::deepCopy(const ril_request_type &request) {
  bool success = false;
  char **dtmf_data = nullptr;

  do {
    // fill legacy params
    params.instance_id = request.instance_id;
    params.modem_id = QCRIL_DEFAULT_MODEM_ID;
    params.event_id = request.req_id;
    params.t = request.t;
    params.datalen = 0;
    params.data = nullptr;

    dtmf_data = (char **) qcril_malloc(CDMA_BURST_DTMF_DATA_LEN * sizeof(char*));
    if (dtmf_data) {
      // data[0] is a DTMF string
      dtmf_data[0] = (char *) qmi_ril_util_str_clone(((char **)request.payload)[0]);
      if (dtmf_data[0] == nullptr) {
        Log::getInstance().d("[RilRequestCdmaBurstDtmfMessage] str clone failed for dtmf_data[0]");
        break;
      }
      // data[1] is the DTMF ON length in milliseconds
      dtmf_data[1] = (char *) qmi_ril_util_str_clone(((char **)request.payload)[1]);
      if (dtmf_data[1] == nullptr) {
        Log::getInstance().d("[RilRequestCdmaBurstDtmfMessage] str clone failed for dtmf_data[1]");
        break;
      }
      // data[1] is the DTMF OFF length in milliseconds
      dtmf_data[2] = (char *) qmi_ril_util_str_clone(((char **)request.payload)[2]);
      if (dtmf_data[2] == nullptr) {
        Log::getInstance().d("[RilRequestCdmaBurstDtmfMessage] str clone failed for dtmf_data[2]");
        break;
      }
      params.data = (void *) dtmf_data;
      success = true;
    } else {
      Log::getInstance().d("[RilRequestCdmaBurstDtmfMessage] malloc for dtmf_data");
      break;
    }
    params.datalen = request.payload_len;
  } while (false);

  if (!success) {
    if (dtmf_data) {
      qcril_free(dtmf_data[0]);
      qcril_free(dtmf_data[1]);
      qcril_free(dtmf_data[2]);
      qcril_free(dtmf_data);
    }
  }
}

string RilRequestCdmaBurstDtmfMessage::dump() {
  return mName;
}
