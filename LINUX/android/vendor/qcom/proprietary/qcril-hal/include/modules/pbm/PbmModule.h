/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/Module.h"
#include "modules/qmi/PbmModemEndPoint.h"
#include "modules/nas/NasEnableIndicationsIndMessage.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"

class PbmModule : public Module {
  public:
    PbmModule();
    ~PbmModule();
    void init();
#ifdef QMI_RIL_UTF
    void qcrilHalPbmModuleCleanup();
#endif

  private:
    bool mReady = false;

    void handleQcrilInit(std::shared_ptr<Message> msg);
    void handlePbmEndpointStatusIndMessage(std::shared_ptr<Message> msg);
    void handleEnablePbmIndications(std::shared_ptr<NasEnableIndicationsIndMessage> msg);
    void handlePbmQmiIndMessage(std::shared_ptr<Message> msg);
    void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
};

PbmModule *getPbmModule();
