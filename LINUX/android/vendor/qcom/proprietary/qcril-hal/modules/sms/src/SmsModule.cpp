/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <framework/QcrilInitMessage.h>
#include <modules/android/ril_message_factory.h>
#include <modules/qmi/ModemEndPointFactory.h>
#include <modules/qmi/QmiSetupRequestCallback.h>
#include <modules/qmi/QmiIndMessage.h>
#include <modules/sms/SmsImsNetworkStateChangedInd.h>
#include <modules/sms/SmsTriggerPowerUpOptimization.h>
#include <modules/qmi/QmiSmsSetupRequestCallback.h>
#include <modules/qmi/EndpointStatusIndMessage.h>
#include <translators/android/send_sms_translator.h>
#include <translators/android/send_sms_expect_more_translator.h>
#include <translators/android/write_sms_to_sim_translator.h>
#include <translators/android/set_smsc_address_translator.h>
#include <translators/android/gsm_set_broadcast_sms_config_translator.h>
#include <translators/android/cdma_send_sms_translator.h>
#include <translators/android/cdma_set_broadcast_sms_config_translator.h>
#include <translators/android/ims_send_sms_translator.h>
#include <framework/Message.h>
#include <framework/Module.h>
#include <framework/Log.h>
#include <framework/qcril_event.h>
#include <functional>
#include <framework/PolicyManager.h>
#include <framework/SingleDispatchRestriction.h>
#include <framework/RejectSimultaneousDispatchRestriction.h>

#include "SmsModule.h"
#include "qcril_qmi_sms.h"

#undef TAG
#define TAG "SmsModule"

using std::placeholders::_1;

static send_sms_translator sSendSmsTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SEND_SMS, char**, RIL_SMS_Response*, sSendSmsTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SEND_SMS);

static send_sms_expect_more_translator sSendSmsExpectMoreTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SEND_SMS_EXPECT_MORE, char**, RIL_SMS_Response*, sSendSmsExpectMoreTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SEND_SMS_EXPECT_MORE);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_SMS_ACKNOWLEDGE, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_SMS_ACKNOWLEDGE);

static write_sms_to_sim_translator sWriteSmsToSimTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_WRITE_SMS_TO_SIM, RIL_SMS_WriteArgs*, int*, sWriteSmsToSimTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_WRITE_SMS_TO_SIM);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_DELETE_SMS_ON_SIM, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_DELETE_SMS_ON_SIM);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_GET_SMSC_ADDRESS, void, char*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_GET_SMSC_ADDRESS);

static set_smsc_address_translator sSetSmscAddressTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_SMSC_ADDRESS, char*, void, sSetSmscAddressTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_SMSC_ADDRESS);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_REPORT_SMS_MEMORY_STATUS);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG, void, RIL_GSM_BroadcastSmsConfigInfo**);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG);

static gsm_set_broadcast_sms_config_translator sGsmSetBroadcastSmsConfigTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG, RIL_GSM_BroadcastSmsConfigInfo**, void, sGsmSetBroadcastSmsConfigTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION);

static cdma_send_sms_translator sCdmaSendSmsTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_SEND_SMS, RIL_CDMA_SMS_Message*, RIL_SMS_Response*, sCdmaSendSmsTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_SEND_SMS);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, RIL_CDMA_SMS_Ack*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM, RIL_CDMA_SMS_WriteArgs*, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG, void, RIL_CDMA_BroadcastSmsConfigInfo**);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG);

static cdma_set_broadcast_sms_config_translator sCdmaSetBroadcastSmsConfigTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG, RIL_CDMA_BroadcastSmsConfigInfo**, void, sCdmaSetBroadcastSmsConfigTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION, int*, void);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION);

QCRIL_ANDROID_DECLARE_REQUEST(RIL_REQUEST_IMS_REGISTRATION_STATE, void, int*);
QCRIL_ANDROID_DEFINE_REQUEST(RIL_REQUEST_IMS_REGISTRATION_STATE);

static ims_send_sms_translator sImsSendSmsTranslator;
QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_IMS_SEND_SMS, RIL_IMS_SMS_Message*, RIL_SMS_Response*, sImsSendSmsTranslator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_IMS_SEND_SMS);

static load_module<SmsModule> sms_module;

SmsModule *getSmsModule() {
    return &(sms_module.get_module());
}

SmsModule::SmsModule() {
    mName = "SmsModule";
    ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint();

    mMessageHandler = {
        HANDLER(QcrilInitMessage, SmsModule::handleQcrilInit),
        {REG_MSG("SMS_QMI_IND"), std::bind(&SmsModule::handleSmsQmiIndMessage, this, _1)},
        // QMI Async response
        HANDLER(QmiAsyncResponseMessage, SmsModule::handleQmiAsyncRespMessage),
        {REG_MSG("SMS_ENDPOINT_STATUS_IND"),
          std::bind(&SmsModule::handleSmsEndPointStatusIndMessage, this, _1)},

        HANDLER(RilRequestSendSmsMessage, SmsModule::handleSendSms),
        HANDLER(RilRequestSendSmsExpectMoreMessage, SmsModule::handleSendSmsExpectMore),
        LEGACY_HANDLER(RIL_REQUEST_SMS_ACKNOWLEDGE, SmsModule::handleSmsAcknowledge),
        HANDLER(RilRequestWriteSmsToSimMessage, SmsModule::handleWriteSmsToSim),
        LEGACY_HANDLER(RIL_REQUEST_DELETE_SMS_ON_SIM, SmsModule::handleDeleteSmsOnSim),
        LEGACY_HANDLER(RIL_REQUEST_GET_SMSC_ADDRESS, SmsModule::handleGetSmscAddress),
        HANDLER(RilRequestSetSmscAddressMessage, SmsModule::handleSetSmscAddress),
        LEGACY_HANDLER(RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, SmsModule::handleReportSmsMemoryStatus),
        LEGACY_HANDLER(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG, SmsModule::handleGsmGetBroadcastSmsConfig),
        HANDLER(RilRequestGsmSetBroadcastSmsConfigMessage, SmsModule::handleGsmSetBroadcastSmsConfig),
        LEGACY_HANDLER(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION, SmsModule::handleGsmSmsBroadcastActivation),
        HANDLER(RilRequestCdmaSendSmsMessage, SmsModule::handleCdmaSendSms),
        LEGACY_HANDLER(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, SmsModule::handleCdmaSmsAcknowledge),
        LEGACY_HANDLER(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM, SmsModule::handleCdmaWriteSmsToRuim),
        LEGACY_HANDLER(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, SmsModule::handleCdmaDeleteSmsOnRuim),
        LEGACY_HANDLER(RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG, SmsModule::handleCdmaGetBroadcastSmsConfig),
        HANDLER(RilRequestCdmaSetBroadcastSmsConfigMessage, SmsModule::handleCdmaSetBroadcastSmsConfig),
        LEGACY_HANDLER(RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION, SmsModule::handleCdmaSmsBroadcastActivation),
        LEGACY_HANDLER(RIL_REQUEST_IMS_REGISTRATION_STATE, SmsModule::handleImsRegistrationState),
        HANDLER(RilRequestImsSendSmsMessage, SmsModule::handleImsSendSms),
    };
    qcril_qmi_sms_pre_init();
}

SmsModule::~SmsModule() {
    qcril_qmi_sms_destroy();
}

void SmsModule::init() {
    Module::init();

    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_DELETE_SMS_ON_SIM, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GET_SMSC_ADDRESS, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_SET_SMSC_ADDRESS, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION, unused);
    QCRIL_ANDROID_REGISTER_REQUEST(RIL_REQUEST_IMS_REGISTRATION_STATE, unused);

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestCdmaSendSmsMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());
    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestImsSendSmsMessage::MESSAGE_NAME,
       std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(RilRequestWriteSmsToSimMessage::MESSAGE_NAME,
      std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_SMS_ACKNOWLEDGE, callback,
      std::make_shared<SingleDispatchRestriction>());

    QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, callback,
      std::make_shared<SingleDispatchRestriction>());

    std::vector<std::string> ring1{RilRequestSendSmsMessage::MESSAGE_NAME,
      RilRequestSendSmsExpectMoreMessage::MESSAGE_NAME};
    PolicyManager::getInstance().setMessageRestriction(ring1,
      std::make_shared<SingleDispatchRestriction>());

}

void SmsModule::handleQcrilInit(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    // Init QMI services
    QmiSmsSetupRequestCallback qmiSmsSetupCallback("SmsModule-Token");
    ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint()->requestSetup("SmsModule-Token", &qmiSmsSetupCallback);
}

void SmsModule::handleSmsQmiIndMessage(std::shared_ptr<Message> msg) {
    auto shared_indMsg(std::static_pointer_cast<QmiIndMessage>(msg));

    Log::getInstance().d("[SmsModule]: Handling msg = " + msg->dump());
    QmiIndMsgDataStruct *indData = shared_indMsg->getData();
    if (indData != nullptr) {
        qcril_qmi_sms_unsolicited_indication_cb_helper(indData->msgId, indData->indData,
                                                       indData->indSize);
    } else {
        Log::getInstance().d("Unexpected, null data from message");
    }
}

void SmsModule::handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  QmiAsyncRespData *asyncResp = msg->getData();
  if (asyncResp != nullptr && asyncResp->cb != nullptr) {
    asyncResp->cb(asyncResp->msgId, asyncResp->respData, asyncResp->respDataSize,
        asyncResp->cbData, asyncResp->traspErr);
  } else {
    Log::getInstance().d("Unexpected, null data from message");
  }
}

void SmsModule::handleSmsEndPointStatusIndMessage(std::shared_ptr<Message> msg) {
    auto shared_msg(
        std::static_pointer_cast<EndpointStatusIndMessage>(msg));

    Log::getInstance().d("[SmsModule]: Handling msg = " + msg->dump());
    if (shared_msg->getState() == ModemEndPoint::State::OPERATIONAL) {
        if (!mReady) {
            qmi_ril_set_sms_svc_status( QMI_RIL_SMS_SVC_NOT_INITIALZIED );
            qmi_ril_set_primary_client_status( QMI_RIL_SMS_PRIMARY_CLIENT_NOT_SET );

            qmi_client_error_type client_err = qcril_qmi_sms_init();
            if (client_err)
            {
                Log::getInstance().d("qcril_qmi_sms_init returned failure:" + std::to_string(client_err));
            }

            client_err = qcril_sms_perform_initial_configuration();
            if (client_err != QMI_NO_ERR)
            {
                Log::getInstance().d("Failed to perform SMS initial configuration.");
            }

            mReady = true;
        }
    } else {
        mReady = false;
        qmi_ril_set_sms_svc_status( QMI_RIL_SMS_SVC_NOT_INITIALZIED );
        qmi_ril_set_primary_client_status( QMI_RIL_SMS_PRIMARY_CLIENT_NOT_SET );

    }
}

void SmsModule::handleSendSms(std::shared_ptr<RilRequestSendSmsMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_send_sms(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleSendSmsExpectMore(std::shared_ptr<RilRequestSendSmsExpectMoreMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_send_sms_expect_more(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleSmsAcknowledge(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_sms_acknowledge(&msg->get_params(), &ret_ptr);
}

void SmsModule::handleWriteSmsToSim(std::shared_ptr<RilRequestWriteSmsToSimMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_write_sms_to_sim(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleDeleteSmsOnSim(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_delete_sms_on_sim(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleGetSmscAddress(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_get_smsc_address(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleSetSmscAddress(std::shared_ptr<RilRequestSetSmscAddressMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_set_smsc_address(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleReportSmsMemoryStatus(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_report_sms_memory_status(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleGsmGetBroadcastSmsConfig(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_gsm_get_broadcast_sms_config(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleGsmSetBroadcastSmsConfig(std::shared_ptr<RilRequestGsmSetBroadcastSmsConfigMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_gsm_set_broadcast_sms_config(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleGsmSmsBroadcastActivation(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_gsm_sms_broadcast_activation(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleCdmaSendSms(std::shared_ptr<RilRequestCdmaSendSmsMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_send_sms(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleCdmaSmsAcknowledge(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_sms_acknowledge(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleCdmaWriteSmsToRuim(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_write_sms_to_ruim(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleCdmaDeleteSmsOnRuim(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_delete_sms_on_ruim(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleCdmaGetBroadcastSmsConfig(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_get_broadcast_sms_config(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleCdmaSetBroadcastSmsConfig(std::shared_ptr<RilRequestCdmaSetBroadcastSmsConfigMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_set_broadcast_sms_config(&msg->get_request_params(), &ret_ptr);
}

void SmsModule::handleCdmaSmsBroadcastActivation(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_cdma_sms_broadcast_activation(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleImsRegistrationState(std::shared_ptr<RilRequestMessage> msg) {
    auto shared_msg(std::static_pointer_cast<RilRequestMessage>(msg));
    qcril_request_return_type ret_ptr;
    qcril_sms_request_ims_registration_state(&shared_msg->get_params(), &ret_ptr);
}

void SmsModule::handleImsSendSms(std::shared_ptr<RilRequestImsSendSmsMessage> msg) {
    qcril_request_return_type ret_ptr;
    qcril_sms_request_ims_send_sms(&msg->get_request_params(), &ret_ptr);
}

#ifdef QMI_RIL_UTF
void SmsModule::qcrilHalSmsModuleCleanup()
{
    std::shared_ptr<SmsModemEndPoint> mSmsModemEndPoint =
                ModemEndPointFactory<SmsModemEndPoint>::getInstance().buildEndPoint();
    SmsModemEndPointModule* mSmsModemEndPointModule =
               (SmsModemEndPointModule*)mSmsModemEndPoint->mModule;
    mSmsModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif
