/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include <framework/QcrilInitMessage.h>
#include "qcril_pbm.h"
#include "modules/nas/qcril_qmi_nas.h"
#include "modules/pbm/PbmModule.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/PbmModemEndPoint.h"
#include "modules/qmi/QmiPbmSetupRequestCallback.h"

static load_module<PbmModule> pbm_module;

PbmModule *getPbmModule() {
    return &(pbm_module.get_module());
}

/*
 * 1. Indicate your preference for looper.
 * 2. Subscribe to the list of messages via mMessageHandler.
 * 3. Follow RAII practice.
 */
PbmModule::PbmModule() {
  mName = "PbmModule";
  ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint();

  using std::placeholders::_1;
  mMessageHandler = {
    HANDLER(QcrilInitMessage,PbmModule::handleQcrilInit),
    HANDLER(NasEnableIndicationsIndMessage, PbmModule::handleEnablePbmIndications),
    {REG_MSG("PBM_QMI_IND"),
             std::bind(&PbmModule::handlePbmQmiIndMessage, this, _1)},
    // QMI Async response
    HANDLER(QmiAsyncResponseMessage, PbmModule::handleQmiAsyncRespMessage),
    // End Point Status Indication
    {REG_MSG("PBM_ENDPOINT_STATUS_IND"),
             std::bind(&PbmModule::handlePbmEndpointStatusIndMessage, this, _1)},
  };
}

/* Follow RAII.
*/
PbmModule::~PbmModule() {}

/*
 * Module specific initialization that does not belong to RAII .
 */
void PbmModule::init() {
  Module::init();
  qcril_qmi_pbm_pre_init();
}

/*
 * List of individual private handlers for the subscribed messages.
 */
void PbmModule::handleQcrilInit(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  /* Init QMI services.*/
  QmiPbmSetupRequestCallback qmiPbmSetupCallback("PbmModule-Token");
  ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint()->requestSetup("PbmModule-Token", &qmiPbmSetupCallback);
}

void PbmModule::handlePbmEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  if (shared_indMsg->getState() == ModemEndPoint::State::OPERATIONAL) {
    if (!mReady) {
      mReady = true;
      qcril_qmi_pbm_init();
    }
  } else {
    // state is not operational
    mReady = false;
    // clean up
    qcril_qmi_pbm_cleanup();
  }
}

void PbmModule::handleEnablePbmIndications(std::shared_ptr<NasEnableIndicationsIndMessage> msg) {

    Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
    int state = msg->get_screen_status();

    if(!qcril_qmi_nas_decide_need_to_reg_pbm_ind())
    {
        qcril_qmi_pbm_enable_emergency_number_indications(state);
    }
}

void PbmModule::handlePbmQmiIndMessage(std::shared_ptr<Message> msg) {
  auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));
  Log::getInstance().d("[PbmModule]: Handling msg = " + msg->dump());
  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_pbm_unsolicited_indication_cb_helper(indData->msgId, indData->indData,
        indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void PbmModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

#ifdef QMI_RIL_UTF
void PbmModule::qcrilHalPbmModuleCleanup()
{
    std::shared_ptr<PbmModemEndPoint> mPbmModemEndPoint =
                    ModemEndPointFactory<PbmModemEndPoint>::getInstance().buildEndPoint();
    PbmModemEndPointModule* mPbmModemEndPointModule =
                   (PbmModemEndPointModule*)mPbmModemEndPoint->mModule;
    mPbmModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif
