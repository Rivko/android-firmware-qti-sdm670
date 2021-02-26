/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#define TAG "LPA_Client_Module"

#include <framework/ModuleLooper.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Log.h>

extern "C" {
#include <framework/qcrili.h>
}

#include "LpaClientModule.h"
#include "modules/lpa/LpaUserReqMessage.h"
#include "modules/lpa/LpaInitMessage.h"
#include "modules/lpa/LpaResponseMessage.h"

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
*/
LpaClientModule::LpaClientModule() {
  mName = "LpaClientModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
}

/* Follow RAII.
*/
LpaClientModule::~LpaClientModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
*/
void LpaClientModule::init() {
    Module::init();
}

void LpaClientModule::sendUserReqMsg(qcril_instance_id_e_type instance_id,
        qcril_modem_id_e_type modem_id, qcril_evt_e_type uim_req,
        void *data, size_t datalen, RIL_Token token) {

    m_uim_req = uim_req;

    std::shared_ptr<LpaUserReqMessage> lpa_msg = std::make_shared<LpaUserReqMessage>(nullptr,
                                       instance_id,
                                       modem_id,
                                       uim_req,
                                       data,
                                       datalen,
                                       token
                                        );
    Dispatcher::getInstance().dispatch(lpa_msg);
}

/*
 * List of individual private handlers for the subscribed messages.
*/
void LpaClientModule::handleLpaResponseMessage(std::shared_ptr<Message> msg) {

    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

    std::shared_ptr<LpaResponseMessage> lpaResp(
            std::static_pointer_cast<LpaResponseMessage>(msg));

    Log::getInstance().d("[" + mName + "]: lpaResp->m_message_id = %d" + std::to_string(lpaResp->m_message_id));

    m_last_msg_type_handled = lpaResp->m_type;

    string *sampleResponse = new string("SAMPLE-RESPONSE");
    std::shared_ptr<string> shared_pResponse(sampleResponse);
    lpaResp->sendResponse(msg, Message::Callback::Status::SUCCESS, shared_pResponse);

    // TODO : Validate m_uim_req with msg type returned and other validations.
}

UimStubModule::UimStubModule() {
  mName = "UimStubModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(LpaUserReqMessage, UimStubModule::handleLpaUserReqMessage),
  };
  m_last_msg_type_handled = lpa_MessageType_UIM_LPA_MSG_UNKNOWN;
}

/* Follow RAII.
*/
UimStubModule::~UimStubModule() {
}

/*
 * Module specific initialization that does not belong to RAII .
*/
void UimStubModule::init() {
    Module::init();
}

void UimStubModule::handleLpaUserReqMessage(std::shared_ptr<Message> msg) {
    // Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

    std::shared_ptr<LpaUserReqMessage> lpaUserReq(
            std::static_pointer_cast<LpaUserReqMessage>(msg));

    Log::getInstance().d("[" + mName + "]: Handling msg = " + lpaUserReq->dump());

    Log::getInstance().d("[" + mName + "]: Received User Req, sending canned response...");
    sendLpaResponseMessage(msg, true, nullptr,
                           lpa_MessageType_UIM_LPA_MSG_RESPONSE,
                           lpa_MessageId_UIM_LPA_HTTP_TXN_IND,
                           false,
                           lpa_Error_UIM_LPA_ERR_SUCCESS,
                           nullptr,
                           0);
}

void UimStubModule::sendLpaResponseMessage(std::shared_ptr<Message> msg,
                bool has_token, RIL_Token token,
                lpa_MessageType type,
                lpa_MessageId message_id,
                bool has_error,
                lpa_Error error,
                const void* msgbuf, int msg_len) {

    m_last_msg_type_handled = type;
    std::shared_ptr<LpaResponseMessage> lpa_msg = std::make_shared<LpaResponseMessage>(nullptr,
                                       has_token, token,
                                       type,
                                       message_id,
                                       has_error,
                                       error,
                                       msgbuf,
                                       msg_len
                                        );
    Dispatcher::getInstance().dispatch(lpa_msg);

    Log::getInstance().d("[" + mName + "]: Dispatched...");

    string *sampleResponse = new string("SAMPLE-RESPONSE");
    std::shared_ptr<string> shared_pResponse(sampleResponse);
    lpa_msg->sendResponse(msg, Message::Callback::Status::SUCCESS, shared_pResponse);
}
#endif
