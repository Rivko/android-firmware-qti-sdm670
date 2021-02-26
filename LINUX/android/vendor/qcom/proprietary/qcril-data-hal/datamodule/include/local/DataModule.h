/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "module/IDataModule.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class DataModule : public IDataModule {
 public:
  DataModule();
  ~DataModule();
  void init();

 private:

  void handleQcrilInitMessage(std::shared_ptr<Message> msg);
  void broadcastReady();

  void handleSetupDataCallMessage(std::shared_ptr<Message> msg);
  void handleDataCallListMessage(std::shared_ptr<Message> msg);

void handleGetDataCallProfileMessage(std::shared_ptr<Message> msg);

void handleDeactivateDataCallMessage(std::shared_ptr<Message> msg);

void handleEmbmsActivateDeactivateTmgiMessage(std::shared_ptr<Message> msg);

void handleEmbmsActivateTmgiMessage(std::shared_ptr<Message> msg);

void handleEmbmsContentDescUpdateMessage(std::shared_ptr<Message> msg);

void handleEmbmsDeactivateTmgiMessage(std::shared_ptr<Message> msg);

void handleEmbmsEnableDataReqMessage(std::shared_ptr<Message> msg);

void handleEmbmsGetActiveTmgiMessage(std::shared_ptr<Message> msg);

void handleEmbmsGetAvailTmgiMessage(std::shared_ptr<Message> msg);

void handleEmbmsSendIntTmgiListMessage(std::shared_ptr<Message> msg);

void handleGoDormantMessage(std::shared_ptr<Message> msg);

void handleLastDataCallFailCauseMessage(std::shared_ptr<Message> msg);

void handleProcessScreenStateChangeMessage(std::shared_ptr<Message> msg);

void handleProcessStackSwitchMessage(std::shared_ptr<Message> msg);

void handlePullLceDataMessage(std::shared_ptr<Message> msg);

void handleSetApnInfoMessage(std::shared_ptr<Message> msg);

void handleSetDataProfileMessage(std::shared_ptr<Message> msg);

void handleSetIsDataEnabledMessage(std::shared_ptr<Message> msg);

void handleSetIsDataRoamingEnabledMessage(std::shared_ptr<Message> msg);

void handleSetLteAttachProfileMessage(std::shared_ptr<Message> msg);

void handleSetQualityMeasurementMessage(std::shared_ptr<Message> msg);

void handleSetRatPrefMessage(std::shared_ptr<Message> msg);

void handleStartLceMessage(std::shared_ptr<Message> msg);

void handleStopLceMessage(std::shared_ptr<Message> msg);

void handleToggleDormancyIndMessage(std::shared_ptr<Message> msg);

void handleToggleLimitedSysIndMessage(std::shared_ptr<Message> msg);

void handleUpdateMtuMessage(std::shared_ptr<Message> msg);

void handleDataEmbmsActiveMessage(std::shared_ptr<Message> msg);

void handleGetDdsSubIdMessage(std::shared_ptr<Message> msg);

void handleDataRequestDDSSwitchMessage(std::shared_ptr<Message> msg);

#if (QCRIL_RIL_VERSION >= 15)
void handleSetLteAttachProfileMessage_v15(std::shared_ptr<Message> msg);
void handleSetDataProfileMessage_v15(std::shared_ptr<Message> msg);

void handleStartKeepaliveMessage(std::shared_ptr<Message> msg);
void handleStopKeepaliveMessage(std::shared_ptr<Message> msg);
#endif

   void handleNasRequestDataShutdown(std::shared_ptr<Message> msg);
};

} //namespace

