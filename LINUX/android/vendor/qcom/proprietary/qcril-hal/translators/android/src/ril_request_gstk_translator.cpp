/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <telephony/ril.h>
#include <framework/Log.h>
#include "modules/uim/GstkRilRequestMessage.h"
#include "modules/android/qcril_android_event_context.h"
#include "ril_request_gstk_translator.h"

#define TAG "ril_request_gstk_translator"

static ril_request_gstk_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, void, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING);
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND, char *, char *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND);
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE, char *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE);
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM, int *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM);
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS, char *, RIL_SIM_IO_Response *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS);


ril_request_gstk_translator::ril_request_gstk_translator
(
  void
)
{
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS, callback);
}


std::shared_ptr<SolicitedMessage<generic_callback_payload>> ril_request_gstk_translator::translate_request
(
  const ril_request_type &request
)
{
  auto msg = std::make_shared<GstkRilRequestMessage>(&request);

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  return msg;
}


std::shared_ptr<ril_response_type> ril_request_gstk_translator::translate_response
(
  std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg,
  Message::Callback::Status status,
  std::shared_ptr<generic_callback_payload> payload
)
{
  std::shared_ptr<ril_response_type>      ret           = std::make_shared<ril_response_type>();
  std::shared_ptr<gstk_ril_resp_payload>  resp_ptr      = std::static_pointer_cast<gstk_ril_resp_payload>(payload);
  std::shared_ptr<GstkRilRequestMessage>  req_ptr       = std::static_pointer_cast<GstkRilRequestMessage>(msg);
  qcril_event_context                   * ctx           = nullptr;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (status == Message::Callback::Status::NO_HANDLER_FOUND)
  {
    ret->e = RIL_E_REQUEST_NOT_SUPPORTED;
  }
  else if (status != Message::Callback::Status::SUCCESS)
  {
    ret->e = RIL_E_INTERNAL_ERR;
  }
  else
  {
    ret->e = RIL_E_SUCCESS;
  }
  ret->response = nullptr;
  ret->response_len = 0;

  if (req_ptr != nullptr)
  {
    ctx  = static_cast<qcril_event_context *>(req_ptr->get_token());

    if (ctx != nullptr)
    {
      ret->instance_id = ctx->instance_id;
      ret->req_id      = ctx->req_id;
      ret->t           = ctx;
    }

    if (resp_ptr != nullptr)
    {
      ret->e = resp_ptr->mRilErr;
      ret->response_len = resp_ptr->mRspLen;
      ret->response = resp_ptr->get();
    }
    else
    {
      ret->e = RIL_E_NO_MEMORY;
    }
  }

  return ret;
}

std::shared_ptr<ril_unsol_type> ril_request_gstk_translator::translate_unsol(std::shared_ptr<UnSolicitedMessage> msg)
{
  std::shared_ptr<ril_unsol_type> ret;
  (void)msg;

  return ret;
}


ril_request_gstk_translator::~ril_request_gstk_translator()
{
}

