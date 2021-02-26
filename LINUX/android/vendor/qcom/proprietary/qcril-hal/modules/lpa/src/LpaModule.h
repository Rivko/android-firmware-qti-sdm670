/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include <framework/QcrilInitMessage.h>
#include "modules/uim/UimLpaRespMessage.h"
#include "modules/uim/UimLpaIndicationMsg.h"
#include "modules/lpa/LpaQmiUimHttpIndicationMsg.h"
#include "LpaQmiUimHttpResponseMsg.h"
#include "LpaService.h"
#include "framework/QcrilInitMessage.h"

using namespace vendor::qti::hardware::radio::lpa::V1_0::implementation;

class LpaModule : public Module
{
  public:
    LpaModule();
    ~LpaModule();

    void init();

    bool isReady();

    void handleUimLpaUserRequest(int token, lpa_service_user_req_type *user_req_ptr);
    void handleUimLpaHttpTxnCompletedRequest(int token, lpa_service_http_transaction_req_type *http_req_ptr);
#ifdef QMI_RIL_UTF
    void qcrilHalLpaModuleCleanup();
#endif

  private:
    bool mReady;
    bool mRestart;
    sp<UimLpaImpl> uimLpa;
    void broadcastReady();

    void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
    void handleUimLpaResponseMessage(std::shared_ptr<UimLpaRespMessage> msg);
    void handleUimLpaIndicationMessage(std::shared_ptr<UimLpaIndicationMsg> msg);
    void handleQmiUimHttpIndication(std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg);
    void handleQmiUimHttpResponse(std::shared_ptr<LpaQmiUimHttpResponseMsg> msg);
};

LpaModule *getLpaModule();
