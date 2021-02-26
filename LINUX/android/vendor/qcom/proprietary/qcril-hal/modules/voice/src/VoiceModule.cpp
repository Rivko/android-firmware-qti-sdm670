/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <unordered_map>

#include "framework/Dispatcher.h"
#include "framework/Looper.h"
#include "framework/ModuleLooper.h"
#include "framework/PolicyManager.h"
#include "framework/SingleDispatchRestriction.h"
#include "framework/SingleDispatchAndFamilyPairRestriction.h"
#include "framework/Log.h"
#include <framework/ThreadPoolManager.h>

#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "modules/voice/RilRequestDialSyncMessage.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/VoiceModemEndPoint.h"

#include "modules/voice/VoiceModule.h"
#include "modules/android/ril_message_factory.h"
#include "qcril_qmi_voice.h"
// translator

#ifdef USING_NAS_PUBLIC_API
// NAS interface
#include "modules/nas/NasModuleInterface.h"
#include "modules/nas/NasModuleGetSyncInterfaceMessage.h"
#include "modules/voice/NasApi.h"

// NAS indication
#include "modules/nas/VoiceRegistrationStatusChangeInd.h"
#include "modules/nas/NetworkServiceRteChangeInd.h"
#endif

#undef  TAG
#define TAG "VoiceModule"

// RIL requests that do not need translator
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_ANSWER, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_ANSWER);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_HANGUP, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_HANGUP);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, void, RIL_Call **);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_LAST_CALL_FAIL_CAUSE, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_DTMF, char*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_DTMF);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_DTMF_START, char*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_DTMF_START);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_DTMF_STOP, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_DTMF_STOP);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_FLASH, char*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_FLASH);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SET_TTY_MODE, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SET_TTY_MODE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_QUERY_TTY_MODE, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_QUERY_TTY_MODE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SET_CALL_WAITING, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SET_CALL_WAITING);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_QUERY_CLIP, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_QUERY_CLIP);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_GET_CLIR, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_GET_CLIR);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_QUERY_CALL_WAITING, int*, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_QUERY_CALL_WAITING);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SET_CLIR, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SET_CLIR);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SEND_USSD, char*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SEND_USSD);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CANCEL_USSD, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CANCEL_USSD);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CONFERENCE, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CONFERENCE);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_EXPLICIT_CALL_TRANSFER, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_EXPLICIT_CALL_TRANSFER);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SEPARATE_CONNECTION, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SEPARATE_CONNECTION);
QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_UDUB, void, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_UDUB);

static load_module<VoiceModule> voiceModule;

VoiceModule *getVoiceModule() {
    return &(voiceModule.get_module());
}

VoiceModule::VoiceModule() {
  mName = "VoiceModule";
  mLooper = nullptr;
  mInitialized = false;
  ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();

  /* _1 is the place holder for argument to handler method
     TODO: wrap function handler into a macro to make it more pleasing
  */
  using std::placeholders::_1;
  mMessageHandler = {
    // Qcril init complete message
    HANDLER(QcrilInitMessage, VoiceModule::handleQcrilInit),
    // RIL request with translators

    // flow control: single
    HANDLER(RilRequestDialMessage, VoiceModule::handleDialMessage),
    // flow control: exempt
    HANDLER(RilRequestQueryFacilityLockMessage, VoiceModule::handleQueryFacilityLockMessage),
    // flow control: exempt
    HANDLER(RilRequestSetFacilityLockMessage, VoiceModule::handleSetFacilityLockMessage),

    HANDLER(RilRequestDialSyncMessage, VoiceModule::handleDialSyncMessage),
    HANDLER(RilRequestSetCallForwardMessage, VoiceModule::handleSetCallForwardMessage),
    HANDLER(RilRequestQueryCallForwardMessage, VoiceModule::handleQueryCallForwardMessage),
    HANDLER(RilRequestChangeBarringPasswordMessage, VoiceModule::handleChangeBarringPwdMessage),
    HANDLER(RilRequestCdmaBurstDtmfMessage, VoiceModule::handleCdmaBurstDtmfMessage),

    // RIL request without translators
    LEGACY_HANDLER(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, VoiceModule::handleSetSuppSvcNotification),
    LEGACY_HANDLER(RIL_REQUEST_ANSWER, VoiceModule::handleAnswerMessage),
    LEGACY_HANDLER(RIL_REQUEST_HANGUP, VoiceModule::handleHangupMessage),
    LEGACY_HANDLER(RIL_REQUEST_GET_CURRENT_CALLS, VoiceModule::handleGetCurrentCallMessage),
    LEGACY_HANDLER(RIL_REQUEST_LAST_CALL_FAIL_CAUSE, VoiceModule::handleGetCallFailureMessage),
    LEGACY_HANDLER(RIL_REQUEST_DTMF, VoiceModule::handleDtmfMessage),
    LEGACY_HANDLER(RIL_REQUEST_DTMF_START, VoiceModule::handleDtmfStartMessage),
    LEGACY_HANDLER(RIL_REQUEST_DTMF_STOP, VoiceModule::handleDtmfStopMessage),
    LEGACY_HANDLER(RIL_REQUEST_CDMA_FLASH, VoiceModule::handleCdmaFlashMessage),
    LEGACY_HANDLER(RIL_REQUEST_SET_TTY_MODE, VoiceModule::handleSetTtyMessage),
    LEGACY_HANDLER(RIL_REQUEST_QUERY_TTY_MODE, VoiceModule::handleQueryTtyMessage),
    LEGACY_HANDLER(RIL_REQUEST_SET_CALL_WAITING, VoiceModule::handleSetCallWaitingMessage),
    LEGACY_HANDLER(RIL_REQUEST_QUERY_CLIP, VoiceModule::handleQueryClipMessage),
    LEGACY_HANDLER(RIL_REQUEST_GET_CLIR, VoiceModule::handleGetClirMessage),
    LEGACY_HANDLER(RIL_REQUEST_QUERY_CALL_WAITING, VoiceModule::handleQueryCallWaitingMessage),
    LEGACY_HANDLER(RIL_REQUEST_SET_CLIR, VoiceModule::handleSetClirMessage),
    LEGACY_HANDLER(RIL_REQUEST_SEND_USSD, VoiceModule::handleSendUssdMessage),
    LEGACY_HANDLER(RIL_REQUEST_CANCEL_USSD, VoiceModule::handleCancelUssdMessage),
    LEGACY_HANDLER(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
            VoiceModule::handleCdmaSetPrefVoicePrivacyModeMessage),
    LEGACY_HANDLER(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
            VoiceModule::handleCdmaQueryPrefVoicePrivacyModeMessage),
    LEGACY_HANDLER(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
            VoiceModule::handleHangupWaitingOrBackgroundMessage),
    LEGACY_HANDLER(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
            VoiceModule::HandleHangupForegroundResumeBackgroundMessage),
    LEGACY_HANDLER(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
            VoiceModule::handleSwitchHoldingAndActiveMessage),
    LEGACY_HANDLER(RIL_REQUEST_CONFERENCE, VoiceModule::handleConferenceMessage),
    LEGACY_HANDLER(RIL_REQUEST_EXPLICIT_CALL_TRANSFER,
            VoiceModule::handleExplicitCallTransferMessage),
    LEGACY_HANDLER(RIL_REQUEST_SEPARATE_CONNECTION, VoiceModule::handleSeperateConnectionMessage),
    LEGACY_HANDLER(RIL_REQUEST_UDUB, VoiceModule::handleUdubMessage),

    // public APIs
    HANDLER(VoiceLimitIndMessage, VoiceModule::handleLimitedIndMessage),
    HANDLER(VoiceGetAnswerCallModeMessage, VoiceModule::handleGetAnswerCallModeMessage),
    HANDLER(VoiceCheckSpecificCallMessage, VoiceModule::handleCheckSpecificCallMessage),
    HANDLER(VoiceImsConnectedMessage, VoiceModule::handleImsConnectedMessage),
    HANDLER(VoiceRteProcessPendingCallMessage, VoiceModule::handleRteProcessPendingCall),
    HANDLER(VoiceCheckCallPresentMessage, VoiceModule::handleCheckCallPresentMessage),

#ifdef USING_NAS_PUBLIC_API
    // NAS indication
    {VoiceRegistrationStatusChangeInd::get_class_message_id(),
     std::bind(&VoiceModule::HandleVoiceRegStatusIndMessage, this, _1)},
    {NetworkServiceRteChangeInd::get_class_message_id(),
     std::bind(&VoiceModule::HandleServiceRteChangeIndMessage, this, _1)},
#endif
    // QMI indication
    {REG_MSG("VOICE_QMI_IND"),
     std::bind(&VoiceModule::handleVoiceQmiIndMessage, this, _1)},
    // QMI Async response
    HANDLER(QmiAsyncResponseMessage, VoiceModule::handleQmiAsyncRespMessage),
    // End Point Status Indication
    {REG_MSG("VOICE_ENDPOINT_STATUS_IND"),
     std::bind(&VoiceModule::handleVoiceEndpointStatusIndMessage, this, _1)},
  };
}

VoiceModule::~VoiceModule() {
}

void VoiceModule::init() {
  /* Call base init before doing any other stuff.*/
  Module::init();

  // move voice pre init here
  qcril_qmi_voice_pre_init();

  // register message
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_DTMF, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_DTMF_START, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_DTMF_STOP, unused);

  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SET_CALL_WAITING, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_QUERY_CLIP, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GET_CLIR, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_QUERY_CALL_WAITING, unused);

  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_ANSWER, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_HANGUP, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CONFERENCE, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_EXPLICIT_CALL_TRANSFER, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SEPARATE_CONNECTION, unused);
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_UDUB, unused);

  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_GET_CURRENT_CALLS,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_LAST_CALL_FAIL_CAUSE,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_CDMA_FLASH,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single - dedicated thread
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_SET_TTY_MODE,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: multiple - dedicated thread
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_QUERY_TTY_MODE,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_SET_CLIR,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: exempt
  QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SEND_USSD, unused);
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_CANCEL_USSD,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: single
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
        unused, std::make_shared<SingleDispatchRestriction>());
  // flow control: multiple - dedicated thread
  QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
        unused, std::make_shared<SingleDispatchRestriction>());

  std::vector<std::string> restMsg {RilRequestDialMessage::MESSAGE_NAME};
  PolicyManager::getInstance().setMessageRestriction(restMsg,
        std::make_shared<SingleDispatchRestriction>());

  // ring1: DTMF related. Set DTMF_START and DTMF_STOP as family pair
  std::vector<std::string> ring1 {
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_DTMF),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_DTMF_START),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_DTMF_STOP),
        RilRequestCdmaBurstDtmfMessage::MESSAGE_NAME
  };
  auto rest = std::make_shared<SingleDispatchAndFamilyPairRestriction>();
  if (rest) {
    rest->setMessagePair(QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_DTMF_START),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_DTMF_STOP));
  }
  PolicyManager::getInstance().setMessageRestriction(ring1, rest);

  // ring2: CALL related
  std::vector<std::string> ring2 {
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_HANGUP),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_CONFERENCE),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_EXPLICIT_CALL_TRANSFER),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_SEPARATE_CONNECTION),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_UDUB),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_ANSWER)
  };
  PolicyManager::getInstance().setMessageRestriction(ring2,
        std::make_shared<SingleDispatchRestriction>());

  // ring3: Supplementary Service related
  std::vector<std::string> ring3 {
        RilRequestQueryCallForwardMessage::MESSAGE_NAME,
        RilRequestSetCallForwardMessage::MESSAGE_NAME,
        RilRequestChangeBarringPasswordMessage::MESSAGE_NAME,
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_QUERY_CALL_WAITING),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_SET_CALL_WAITING),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_QUERY_CLIP),
        QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(RIL_REQUEST_GET_CLIR)
  };
  PolicyManager::getInstance().setMessageRestriction(ring3,
        std::make_shared<SingleDispatchRestriction>());
}

void VoiceModule::handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  /* Init QMI services.*/
  QmiSetupRequestCallback qmiVoiceSetupCallback("VoiceModule-Token");
  auto voiceEndPoint = ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();
  voiceEndPoint->requestSetup("Client-Voice-Serv-Token", &qmiVoiceSetupCallback);
}

void VoiceModule::handleVoiceEndpointStatusIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<EndpointStatusIndMessage> shared_msg(
    std::static_pointer_cast<EndpointStatusIndMessage>(msg));

  if (shared_msg->getState() == ModemEndPoint::State::OPERATIONAL) {
     if (!mInitialized) {
       mInitialized = true;
       qcril_qmi_voice_init();
     }
  } else {
    // state is not operational
    mInitialized = false;
    qcril_qmi_voice_cleanup();
  }
}

void VoiceModule::handleVoiceQmiIndMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<QmiIndMessage> shared_indMsg(
        std::static_pointer_cast<QmiIndMessage>(msg));

  QmiIndMsgDataStruct *indData = shared_indMsg->getData();
  if (indData != nullptr) {
    qcril_qmi_voice_unsol_ind_cb_helper(indData->msgId, indData->indData,
            indData->indSize);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void VoiceModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

// only for gtest
void VoiceModule::handleDialSyncMessage(std::shared_ptr<RilRequestDialSyncMessage> shared_dialMsg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + shared_dialMsg->dump());
  std::shared_ptr<string> shared_pResponse = std::make_shared<string>("Thank you for dialing ");
  shared_dialMsg->sendResponse(
      shared_dialMsg, Message::Callback::Status::SUCCESS, shared_pResponse);
}

void VoiceModule::handleDialMessage(std::shared_ptr<RilRequestDialMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_dial(msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleSetCallForwardMessage(std::shared_ptr<RilRequestSetCallForwardMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_set_call_forward(msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleQueryCallForwardMessage(std::shared_ptr<RilRequestQueryCallForwardMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_query_call_forward_status(
                msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleQueryFacilityLockMessage(std::shared_ptr<RilRequestQueryFacilityLockMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_query_facility_lock(
                msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleSetFacilityLockMessage(std::shared_ptr<RilRequestSetFacilityLockMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_set_facility_lock(
                msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleChangeBarringPwdMessage(std::shared_ptr<RilRequestChangeBarringPasswordMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());
  qcril_request_return_type ret_ptr;

  qcril_qmi_voice_request_change_barring_password(
                msg->get_request_params(), &ret_ptr);
}

void VoiceModule::handleSetSuppSvcNotification(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_set_supp_svc_notification(
        &msg->get_params(), &ret_ptr);
}

void VoiceModule::handleAnswerMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_answer(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleHangupMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_hangup(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleGetCurrentCallMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_get_current_atel_calls(
        &msg->get_params(), &ret_ptr);
}

void VoiceModule::handleGetCallFailureMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_last_call_fail_cause(
        &msg->get_params(), &ret_ptr);
}

void VoiceModule::handleDtmfMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_dtmf(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleDtmfStartMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_dtmf_start(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleDtmfStopMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_dtmf_stop(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleCdmaFlashMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_cdma_flash(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleCdmaBurstDtmfMessage(std::shared_ptr<RilRequestCdmaBurstDtmfMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_cdma_burst_dtmf(&(msg->get_request_params()), &ret_ptr);
}

void VoiceModule::handleSetTtyMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  ThreadPoolManager::getInstance().scheduleExecution(
    qcril_qmi_voice_request_set_tty_mode_wrapper, &msg->get_params());
}

void VoiceModule::handleQueryTtyMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  ThreadPoolManager::getInstance().scheduleExecution(
    qcril_voice_query_tty_mode_wrapper, &msg->get_params());
}

void VoiceModule::handleSetCallWaitingMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_set_call_waiting(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleQueryClipMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_query_clip(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleGetClirMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_get_clir(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleQueryCallWaitingMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_query_call_waiting(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleSetClirMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_set_clir(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleSendUssdMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  // Cancel the timer. There is no specific timeout for the SEND_USSD request, the response
  // for the SEND_USSD will be send only after the USSD session is completed.
  Dispatcher::getInstance().clearTimeoutForMessage(msg);

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_supsvc_request_send_ussd(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleCancelUssdMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_supsvc_request_cancel_ussd(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleCdmaSetPrefVoicePrivacyModeMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_set_preferred_voice_privacy_mode(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleCdmaQueryPrefVoicePrivacyModeMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  ThreadPoolManager::getInstance().scheduleExecution(
    qcril_qmi_voice_request_query_preferred_voice_privacy_mode_wrapper, &msg->get_params());

}

void VoiceModule::handleHangupWaitingOrBackgroundMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_hangup_waiting_or_background(
                &msg->get_params(), &ret_ptr);
}

void VoiceModule::HandleHangupForegroundResumeBackgroundMessage(
                    std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_hangup_foreground_resume_background(
                &msg->get_params(), &ret_ptr);
}

void VoiceModule::handleSwitchHoldingAndActiveMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_switch_waiting_or_holding_and_active(
                &msg->get_params(), &ret_ptr);
}

void VoiceModule::handleConferenceMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_conference(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleExplicitCallTransferMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_explicit_call_transfer(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleSeperateConnectionMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_seperate_connection(&msg->get_params(), &ret_ptr);
}

void VoiceModule::handleUdubMessage(std::shared_ptr<RilRequestMessage> msg) {
  Log::getInstance().d("[VoiceModule]: Handling msg = " + msg->dump());

  qcril_request_return_type ret_ptr;
  qcril_qmi_voice_request_manage_calls_udub(&msg->get_params(), &ret_ptr);
}

/////////////////// VOICE exported interfaces ////////////////////////////////

void VoiceModule::handleGetAnswerCallModeMessage(std::shared_ptr<VoiceGetAnswerCallModeMessage> msg) {
  call_mode_enum_v02 callMode = qcril_qmi_voice_get_answer_call_mode();

  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
    std::shared_ptr<call_mode_enum_v02>(new call_mode_enum_v02(callMode)));
}

void VoiceModule::handleCheckSpecificCallMessage(std::shared_ptr<VoiceCheckSpecificCallMessage> msg) {
  qcril_qmi_voice_call_filter filter;
  call_mode_enum_v02 call_mode = CALL_MODE_NO_SRV_V02;

  if ((filter = msg->getVoiceCallFilter()) == nullptr) {
    msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
  } else {
    boolean ret = qcril_qmi_voice_has_specific_call(filter, &call_mode);
    auto result = std::shared_ptr<SpecificCallResult>(new SpecificCallResult);
    if (result == nullptr) {
      msg->sendResponse(msg, Message::Callback::Status::FAILURE, nullptr);
    } else {
      result->has_call = (ret == TRUE);
      result->call_mode = call_mode;
      msg->sendResponse(msg, Message::Callback::Status::SUCCESS, result);
    }
  }
}

void VoiceModule::handleCheckCallPresentMessage(std::shared_ptr<VoiceCheckCallPresentMessage> msg) {
  bool present = false;

  switch (msg->getCallType()) {
  case VoiceCheckCallPresentMessage::ALL_CALL:
    present = qcril_qmi_voice_nas_control_is_any_calls_present();
    break;
  case VoiceCheckCallPresentMessage::VOICE_CALL:
    present = qmi_ril_voice_is_under_any_voice_calls();
    break;
  default:
    present = false;
  }
  msg->sendResponse(msg, Message::Callback::Status::SUCCESS,
      std::shared_ptr<bool>(new bool{present}));
}

void VoiceModule::handleLimitedIndMessage(std::shared_ptr<VoiceLimitIndMessage> msg) {
  qcril_qmi_voice_enable_voice_indications(msg->isIndLimited());
}

void VoiceModule::handleImsConnectedMessage(std::shared_ptr<VoiceImsConnectedMessage> msg) {
  (void)msg;
  qcril_qmi_voice_ims_send_unsol_radio_state_change();
  qcril_qmi_voice_ims_send_unsol_vice_dialog_refresh_info_helper();
}

void VoiceModule::handleRteProcessPendingCall(std::shared_ptr<VoiceRteProcessPendingCallMessage> msg) {
  qcril_qmi_voice_nas_control_process_calls_pending_for_specified_voice_rte(
        msg->getCallRadioTechFamily());
}

#ifdef USING_NAS_PUBLIC_API
// handle NAS indication
void VoiceModule::HandleVoiceRegStatusIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<VoiceRegistrationStatusChangeInd> shared_msg(
      std::static_pointer_cast<VoiceRegistrationStatusChangeInd>(msg));
  voiceRegStatusStruct voiceReg;
  // TODO: waiting for QIPL to provide get method
  voiceReg.stateValid = shared_msg->stateValid;
  voiceReg.state = shared_msg->state;
  voiceReg.radioTechValid = shared_msg->radioTechValid;
  voiceReg.radioTech = shared_msg->radioTech;
  voiceReg.statusOverviewValid = shared_msg->statusOverviewValid;
  voiceReg.statusOverview = shared_msg->statusOverview;

  qcril_qmi_voice_update_voice_registration_status(voiceReg);
}

void VoiceModule::HandleServiceRteChangeIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<NetworkServiceRteChangeInd> shared_msg(
      std::static_pointer_cast<NetworkServiceRteChangeInd>(msg));
  networkRteStruct networkRte;
  // TODO: waiting for QIPL to provide get method
  networkRte.voiceRteValid = shared_msg->voiceRteValid;
  networkRte.voiceRte = shared_msg->voiceRte;
  networkRte.dataRteValid = shared_msg->dataRteValid;
  networkRte.dataRte = shared_msg->dataRte;
  networkRte.imsRteValid = shared_msg->imsRteValid;
  networkRte.imsRte = shared_msg->imsRte;

  qcril_qmi_voice_update_service_rte_info(networkRte);
}
#endif

#ifdef QMI_RIL_UTF
void VoiceModule::qcrilHalVoiceModuleCleanup()
{
    std::shared_ptr<VoiceModemEndPoint> mVoiceModemEndPoint =
                    ModemEndPointFactory<VoiceModemEndPoint>::getInstance().buildEndPoint();
    VoiceModemEndPointModule* mVoiceModemEndPointModule =
                   (VoiceModemEndPointModule*)mVoiceModemEndPoint->mModule;
    mVoiceModemEndPointModule->cleanUpQmiSvcClient();
    mInitialized = false;
}
#endif
