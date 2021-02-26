/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#include "gtest/gtest.h"
#include <framework/QcrilInitMessage.h>
#include <framework/legacy.h>
#include <framework/Util.h>

#include "modules/lpa/LpaModule.h"
#include "LpaClientModule.h"



using namespace std;

TEST(LpaFeatureSanity, TestRegisteredMessageProcessing) {
    LpaModule *lpaModule = new LpaModule();
    lpaModule->init();

    ASSERT_TRUE(false == lpaModule->isReady());

    delete lpaModule;
    lpaModule = nullptr;
}

TEST(LpaFeatureSanity, TestQcrilInitHandling) {
    LpaModule *lpaModule = new LpaModule();
    lpaModule->init();

    // Create and send QCRIL Init.
    std::shared_ptr<QcrilInitMessage> qcril_init_msg = std::make_shared<QcrilInitMessage>(QCRIL_DEFAULT_INSTANCE_ID);
    qcril_init_msg->broadcast();


    threadSleep(1);

    //LPA module should have received it and initialized.
    ASSERT_TRUE(true == lpaModule->isReady());

    delete lpaModule;
    lpaModule = nullptr;
}

TEST(LpaFeatureSanity, TestRespFromTelephony) {
    LpaClientModule *lpaClient = new LpaClientModule();
    lpaClient->init();

    LpaModule *lpaModule = new LpaModule();
    lpaModule->init();

    UimStubModule *uimStubModule = new UimStubModule();
    uimStubModule->init();

    std::shared_ptr<QcrilInitMessage> qcril_init_msg =
            std::make_shared<QcrilInitMessage>(QCRIL_DEFAULT_INSTANCE_ID);
    Dispatcher::getInstance().broadcast(qcril_init_msg);

    threadSleep(1);

    //LPA module should have received it and initialized.
    ASSERT_TRUE(true == lpaModule->isReady());

    // Trigger a request from LPA Client.
    // TODO LPA REARCH - need valid test params.
    char msg[10] = {};
    size_t msglen = sizeof(msg);

    lpaClient->sendUserReqMsg(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID,
                             QCRIL_EVT_UIM_LPA_USER_REQ, (void*)msg, msglen, nullptr);

    threadSleep(1);

    // UIM Module should receive the user request and resent a response.
    ASSERT_TRUE(lpa_MessageType_UIM_LPA_MSG_RESPONSE == uimStubModule->getLastMsgTypeHandled());

    // The client should should receive the LpaResponseMessage.
    // TODO: Need to expand the UT to receive the data via simulated HIDL or mock socket interface.
    // The expected response will be in LpaModule. Need to verify with logs for this step of validation.

    delete lpaModule;
    lpaModule = nullptr;

    delete lpaClient;
    lpaClient = nullptr;

    delete uimStubModule;
    uimStubModule = nullptr;
}
#endif