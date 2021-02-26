/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "framework/Log.h"
#include "modules/uim/UimLpaReqMessage.h"
#include "modules/uim/UimLpaRespMessage.h"
#include "LpaUimReqMessageCallback.h"

void LpaUimReqMessageCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg, Status status,
  std::shared_ptr<qcril_uim_lpa_response_type> responseDataPtr
)
{
  qcril_uim_lpa_response_type            * rsp_data_ptr = nullptr;
  std::shared_ptr<UimLpaRespMessage>       resp_msg_ptr = nullptr;
  std::shared_ptr<UimLpaReqMessage>        req_msg_ptr((
    std::static_pointer_cast<UimLpaReqMessage>(solicitedMsg)));
  int                                      token        = -1;
  void                                   * data_ptr     = nullptr;
  uimLpaResponseType                       resp_id      = UIM_LPA_UNKOWN_RESP_ID;

  Log::getInstance().d(
      "[LpaUimReqMessageCallback]: Callback executed. client data = " +
      mClientToken);

  if (status == Message::Callback::Status::SUCCESS &&
      responseDataPtr != nullptr)
  {
    rsp_data_ptr = responseDataPtr.get();
  }

  if (req_msg_ptr != nullptr)
  {
    token = req_msg_ptr->get_token();
  }

  if (rsp_data_ptr != nullptr)
  {
    data_ptr = rsp_data_ptr->data_ptr;
    resp_id = rsp_data_ptr->resp_id;
  }

  resp_msg_ptr = std::make_shared<UimLpaRespMessage>(resp_id, data_ptr, token);

  if (resp_msg_ptr != nullptr)
  {
    resp_msg_ptr->dispatch();
  }
}

Message::Callback *LpaUimReqMessageCallback::clone()
{
  LpaUimReqMessageCallback *clone = new LpaUimReqMessageCallback(mClientToken);
  return clone;
}
