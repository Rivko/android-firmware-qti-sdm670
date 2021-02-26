/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "framework/legacy.h"
#include "framework/QcrilInitMessage.h"
#include "modules/android/RilRequestMessage.h"

#include "modules/voice/RilRequestDialSyncMessage.h"
#include "modules/voice/RilRequestDialMessage.h"
#include "modules/voice/RilRequestCdmaBurstDtmfMessage.h"
#include "modules/voice/RilRequestSetCallForwardMessage.h"
#include "modules/voice/RilRequestQueryCallForwardMessage.h"
#include "modules/voice/RilRequestQueryFacilityLockMessage.h"
#include "modules/voice/RilRequestSetFacilityLockMessage.h"
#include "modules/voice/RilRequestChangeBarringPasswordMessage.h"

#include "modules/voice/VoiceLimitIndMessage.h"
#include "modules/voice/VoiceGetAnswerCallModeMessage.h"
#include "modules/voice/VoiceCheckSpecificCallMessage.h"
#include "modules/voice/VoiceImsConnectedMessage.h"
#include "modules/voice/VoiceRteProcessPendingCallMessage.h"
#include "modules/voice/VoiceCheckCallPresentMessage.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"

#include "qcril_qmi_voice.h"
#include <string.h>

class VoiceModule : public Module {
 private:
  bool mInitialized;

 public:
  VoiceModule();
  ~VoiceModule();
  void init();
#ifdef QMI_RIL_UTF
  void qcrilHalVoiceModuleCleanup();
#endif

 private:
  // handle QMI IND
  void handleVoiceQmiIndMessage(std::shared_ptr<Message> msg);
  // handle QMI ASYNC RESP
  void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);
  // handle End Point Status Indication
  void handleVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg);
  // handle Qcril Init Message
  void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg);

  // RIL requests with translators
  void handleDialMessage(std::shared_ptr<RilRequestDialMessage> msg);
  void handleDialSyncMessage(std::shared_ptr<RilRequestDialSyncMessage> shared_dialMsg);
  void handleSetCallForwardMessage(std::shared_ptr<RilRequestSetCallForwardMessage> msg);
  void handleQueryCallForwardMessage(std::shared_ptr<RilRequestQueryCallForwardMessage> msg);
  void handleQueryFacilityLockMessage(std::shared_ptr<RilRequestQueryFacilityLockMessage> msg);
  void handleSetFacilityLockMessage(std::shared_ptr<RilRequestSetFacilityLockMessage> msg);
  void handleChangeBarringPwdMessage(std::shared_ptr<RilRequestChangeBarringPasswordMessage> msg);
  void handleCdmaBurstDtmfMessage(std::shared_ptr<RilRequestCdmaBurstDtmfMessage> msg);

  // RIL requests without translators
  void handleSetSuppSvcNotification(std::shared_ptr<RilRequestMessage> msg);
  void handleAnswerMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleHangupMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleGetCurrentCallMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleGetCallFailureMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleDtmfMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleDtmfStartMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleDtmfStopMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleCdmaFlashMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSetTtyMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleQueryTtyMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSetCallWaitingMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleQueryClipMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleGetClirMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleQueryCallWaitingMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSetClirMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSendUssdMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleCancelUssdMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleCdmaSetPrefVoicePrivacyModeMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleCdmaQueryPrefVoicePrivacyModeMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleHangupWaitingOrBackgroundMessage(std::shared_ptr<RilRequestMessage> msg);
  void HandleHangupForegroundResumeBackgroundMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSwitchHoldingAndActiveMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleConferenceMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleExplicitCallTransferMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleSeperateConnectionMessage(std::shared_ptr<RilRequestMessage> msg);
  void handleUdubMessage(std::shared_ptr<RilRequestMessage> msg);

  // public APIs
  void handleLimitedIndMessage(std::shared_ptr<VoiceLimitIndMessage> msg);
  void handleGetAnswerCallModeMessage(std::shared_ptr<VoiceGetAnswerCallModeMessage> msg);
  void handleCheckSpecificCallMessage(std::shared_ptr<VoiceCheckSpecificCallMessage> msg);
  void handleImsConnectedMessage(std::shared_ptr<VoiceImsConnectedMessage> msg);
  void handleRteProcessPendingCall(std::shared_ptr<VoiceRteProcessPendingCallMessage> msg);
  void handleCheckCallPresentMessage(std::shared_ptr<VoiceCheckCallPresentMessage> msg);

  // NAS Indications
  void HandleVoiceRegStatusIndMessage(std::shared_ptr<Message> msg);
  void HandleServiceRteChangeIndMessage(std::shared_ptr<Message> msg);
  // NAS public API
  void handleGetNasInterface();
};

VoiceModule *getVoiceModule();
