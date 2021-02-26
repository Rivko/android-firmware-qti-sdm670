/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include <framework/Message.h>
#include <framework/QcrilInitMessage.h>
#include "modules/qmi/QmiAsyncResponseMessage.h"
#include <modules/ims/ImsSetVoiceDomainPreferenceFollowupMessage.h>

class ImsModule : public Module {
  public:
    ImsModule();
    ~ImsModule();

    void init();

    bool isReady();
#ifdef QMI_RIL_UTF
    void qcrilHalImsModuleCleanup();
#endif

  private:
    bool mReady = false;
    uint8_t mModemEndPointReadyMask = 0x00;

    void initImsa();
    void handleImsaQmiIndMessage(std::shared_ptr<Message> msg);
    void handleImsaEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void initImss();
    void handleImssQmiIndMessage(std::shared_ptr<Message> msg);
    void handleImssEndpointStatusIndMessage(std::shared_ptr<Message> msg);

    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);

    void broadcastReady();
    void handleQcrilInitMessage(std::shared_ptr<QcrilInitMessage> msg);
    void handleSetVoiceDomainPreferenceFollowup(
        std::shared_ptr<ImsSetVoiceDomainPreferenceFollowupMessage> msg);

    bool isImsaModemEndpointReady();
    void setImsaModemEndpointReady();
    void setImsaModemEndpointNotReady();
    bool isImssModemEndpointReady();
    void setImssModemEndpointReady();
    void setImssModemEndpointNotReady();
    bool isAllModemEndpointsReady();
};

ImsModule *getImsModule();
