/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/voice/SetFacilityLockCallback.h"
#include "framework/Log.h"
#include "framework/Message.h"

SetFacilityLockCallback::~SetFacilityLockCallback() {
  mClientToken = "";
}

void SetFacilityLockCallback::onResponse(std::shared_ptr<Message> solicitedMsg,
                         Status status,
                         std::shared_ptr<UimSetFacilityLockResponseMsg> responseDataPtr) {
  (void)solicitedMsg;

  int respData = 0;
  qcril_request_resp_params_type resp;

  if (status == Message::Callback::Status::SUCCESS && responseDataPtr) {
    RIL_Errno rilErr = responseDataPtr->ril_err;
    qcril_default_request_resp_params(mInstanceId,
                                      mRilToken,
                                      mEventId,
                                      rilErr,
                                      &resp);
    // no_of_retries is valid only if ril_err is not RIL_E_SUCCESS
    if (rilErr != RIL_E_SUCCESS) {
      respData = responseDataPtr->no_of_retries;
      resp.resp_pkt = (void*)&respData;
      resp.resp_len = sizeof(respData);
    }
  } else {
    qcril_default_request_resp_params(mInstanceId,
                                      mRilToken,
                                      mEventId,
                                      RIL_E_GENERIC_FAILURE,
                                      &resp);
  }

  qcril_send_request_response(&resp);
}

Message::Callback *SetFacilityLockCallback::clone() {
  SetFacilityLockCallback *clone = new SetFacilityLockCallback(mInstanceId,
        mRilToken, mEventId, mClientToken);
  return clone;
}
