/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "framework/Log.h"
#include "framework/legacy.h"
#include "modules/qti_radio/UimGetAtrMsgCallback.h"
#include "qcril_qmi_qti_radio_service.h"
#include "modules/uim/UimGetAtrRequestMsg.h"

void UimGetAtrMsgCallback::onResponse(
  std::shared_ptr<Message> solicitedMsg, Status status,
  std::shared_ptr<UimAtrRspParam> responseDataPtr) {
  if (status == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[UimGetAtrMsgCallback]: Callback[msg = " +
            solicitedMsg->dump() + "] executed. client data = " +
            mClientToken + " status = Message::Callback::Status::SUCCESS");
    std::shared_ptr<UimGetAtrRequestMsg>
            req_msg_ptr((std::static_pointer_cast<UimGetAtrRequestMsg>(solicitedMsg)));
    std::shared_ptr<UimAtrRspParam>
            resp = std::static_pointer_cast<UimAtrRspParam>(responseDataPtr);
    if(resp) {
      vendor::qti::hardware::radio::qtiradio::V1_0::implementation::
            qtiGetAtrResponse(req_msg_ptr->get_token(), resp->err, resp->atr, (int)resp->atr_len);
    } else {
      Log::getInstance().d("[UimGetAtrMsgCallback]: resp is NULL..not processing ");
    }
  } else {
    Log::getInstance().d("[UimGetAtrMsgCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " FAILURE !!");
  }
}

Message::Callback *UimGetAtrMsgCallback::clone() {
  UimGetAtrMsgCallback *clone = new UimGetAtrMsgCallback(mClientToken);
  return clone;
}
