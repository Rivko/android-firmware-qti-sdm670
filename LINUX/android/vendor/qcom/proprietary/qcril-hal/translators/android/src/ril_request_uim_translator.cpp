/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <telephony/ril.h>
#include <framework/Log.h>
#include "modules/uim/Uimi.h"
#include "modules/uim/UimRilRequestMessage.h"
#include "ril_request_uim_translator.h"
#include "modules/android/qcril_android_event_context.h"

#define TAG "ril_request_uim_translator"

static ril_request_uim_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_GET_SIM_STATUS, void, RIL_CardStatus_v6 *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_GET_SIM_STATUS);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ENTER_SIM_PIN, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ENTER_SIM_PIN);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ENTER_SIM_PIN2, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ENTER_SIM_PIN2);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ENTER_SIM_PUK, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ENTER_SIM_PUK);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ENTER_SIM_PUK2, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ENTER_SIM_PUK2);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_CHANGE_SIM_PIN, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CHANGE_SIM_PIN);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_CHANGE_SIM_PIN2, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CHANGE_SIM_PIN2);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_GET_IMSI, char **, char *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_GET_IMSI);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_IO, RIL_SIM_IO_v6 *, RIL_SIM_IO_Response *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_IO);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_ISIM_AUTHENTICATION, char *, char *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_ISIM_AUTHENTICATION);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_OPEN_CHANNEL, RIL_OpenChannelParams *, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_OPEN_CHANNEL);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_CLOSE_CHANNEL, int *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_CLOSE_CHANNEL);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, RIL_SIM_APDU *, RIL_SIM_IO_Response *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, RIL_SIM_APDU *, RIL_SIM_IO_Response *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_GET_ATR, int *, char *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_GET_ATR);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SIM_AUTHENTICATION, RIL_SimAuthentication *, RIL_SIM_IO_Response *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SIM_AUTHENTICATION);

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(RIL_REQUEST_SET_SIM_CARD_POWER, int *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_SIM_CARD_POWER);

ril_request_uim_translator::ril_request_uim_translator()
{
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GET_SIM_STATUS, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ENTER_SIM_PIN, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ENTER_SIM_PIN2, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ENTER_SIM_PUK, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ENTER_SIM_PUK2, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CHANGE_SIM_PIN, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CHANGE_SIM_PIN2, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GET_IMSI, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_IO, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ISIM_AUTHENTICATION, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_OPEN_CHANNEL, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_CLOSE_CHANNEL, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_GET_ATR, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SIM_AUTHENTICATION, callback);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, callback);
}


std::shared_ptr<SolicitedMessage<generic_callback_payload>> ril_request_uim_translator::translate_request(const ril_request_type &request)
{
  auto msg = std::make_shared<UimRilRequestMessage>(&request);

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);
  if (msg->is_valid == false)
  {
    msg = nullptr;
  }
  return msg;
}

std::shared_ptr<ril_response_type> ril_request_uim_translator::translate_response(std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg, Message::Callback::Status status, std::shared_ptr<generic_callback_payload> payload)
{
  std::shared_ptr<ril_response_type>     ret            = std::make_shared<ril_response_type>();
  std::shared_ptr<uim_ril_resp_payload>  resp_ptr       = std::static_pointer_cast<uim_ril_resp_payload>(payload);
  std::shared_ptr<UimRilRequestMessage>  req_ptr        = std::static_pointer_cast<UimRilRequestMessage>(msg);
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
  }

  return ret;
}

std::shared_ptr<ril_unsol_type> ril_request_uim_translator::translate_unsol(std::shared_ptr<UnSolicitedMessage> msg)
{
  std::shared_ptr<ril_unsol_type> ret;
  (void)msg;

  return ret;
}

ril_request_uim_translator::~ril_request_uim_translator()
{
}
