/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/lpa/LpaUimHttpRequestMsg.h"
#include "framework/Log.h"
#include "LpaQmiUimHttpResponseMsg.h"
#include "LpaQmiUimHttpRequestCallback.h"

#define TAG "LpaQmiUimHttpRequestCallback"

/*===========================================================================
  FUNCTION  onResponse - response function
===========================================================================*/
void LpaQmiUimHttpRequestCallback::onResponse
(
  std::shared_ptr<Message>                         solicitedMsg,
  Message::Callback::Status                        status,
  std::shared_ptr<lpa_qmi_uim_http_rsp_data_type>  responseDataPtr
)
{
  lpa_qmi_uim_http_rsp_data_type             * rsp_data_ptr = nullptr;
  std::shared_ptr<LpaQmiUimHttpResponseMsg>    resp_msg_ptr = nullptr;
  std::shared_ptr<LpaUimHttpRequestMsg>        req_msg_ptr((
    std::static_pointer_cast<LpaUimHttpRequestMsg>(solicitedMsg)));

  Log::getInstance().d(
      "[LpaQmiUimHttpRequestCallback]: Callback executed. client data = " +
      mClientToken);

  if (status == Message::Callback::Status::SUCCESS &&
      responseDataPtr != nullptr)
  {
    rsp_data_ptr = responseDataPtr.get();
    if (req_msg_ptr != nullptr)
    {
      rsp_data_ptr->token = req_msg_ptr->get_token();
    }
  }

  resp_msg_ptr = std::make_shared<LpaQmiUimHttpResponseMsg>(rsp_data_ptr);

  if (resp_msg_ptr != nullptr)
  {
    resp_msg_ptr->dispatch();
  }
} /* LpaQmiUimHttpRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone - clone function
===========================================================================*/
Message::Callback *LpaQmiUimHttpRequestCallback::clone()
{
  LpaQmiUimHttpRequestCallback *clone = new LpaQmiUimHttpRequestCallback(mClientToken);
  return clone;
} /* LpaQmiUimHttpRequestCallback::clone */
