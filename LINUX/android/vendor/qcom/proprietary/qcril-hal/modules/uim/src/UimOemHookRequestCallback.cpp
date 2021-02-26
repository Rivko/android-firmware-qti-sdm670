/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "UimOemHookRequestCallback.h"
#include "framework/Log.h"
#include "framework/legacy.h"


/*===========================================================================
  FUNCTION  onResponse - response function
===========================================================================*/
void UimOemHookRequestCallback::onResponse
(
  std::shared_ptr<Message>                solicitedMsg,
  Message::Callback::Status               status,
  std::shared_ptr<UimOemHookRspData>      rsp_data_ptr
)
{
  std::shared_ptr<UimOemHookRequestMsg>  req_msg_ptr((
    std::static_pointer_cast<UimOemHookRequestMsg>(solicitedMsg)));
  qcril_request_resp_params_type         resp = {};

  (void) status;

  Log::getInstance().d(
      "[UimOemHookRequestCallback]: Callback executed. client data = " +
      mClientToken);

  if (rsp_data_ptr != nullptr)
  {
    qcril_default_request_resp_params(rsp_data_ptr->instance_id,
                                      rsp_data_ptr->token,
                                      rsp_data_ptr->request,
                                      rsp_data_ptr->result,
                                      &resp);

    if (rsp_data_ptr->rsp_ptr != NULL)
    {
      resp.resp_pkt = rsp_data_ptr->rsp_ptr;
      resp.resp_len = rsp_data_ptr->rsp_len;
    }

    if (rsp_data_ptr->logstr != NULL)
    {
      resp.logstr = rsp_data_ptr->logstr;
    }
    qcril_send_request_response(&resp);
  }
} /* UimQmiUimRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone - clone function
===========================================================================*/
Message::Callback *UimOemHookRequestCallback::clone()
{
  UimOemHookRequestCallback *clone = new UimOemHookRequestCallback(mClientToken);
  return clone;
} /* UimQmiUimRequestCallback::clone */
