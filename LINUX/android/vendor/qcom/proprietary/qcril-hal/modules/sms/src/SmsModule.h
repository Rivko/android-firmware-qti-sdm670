/******************************************************************************
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/
#pragma once

#include <framework/Module.h>
#include <framework/Dispatcher.h>
#include <modules/qmi/SmsModemEndPoint.h>
#include <modules/qmi/QmiAsyncResponseMessage.h>
#include <modules/android/RilRequestMessage.h>
#include <modules/sms/RilRequestSendSmsMessage.h>
#include <modules/sms/RilRequestSendSmsExpectMoreMessage.h>
#include <modules/sms/RilRequestWriteSmsToSimMessage.h>
#include <modules/sms/RilRequestSetSmscAddressMessage.h>
#include <modules/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h>
#include <modules/sms/RilRequestCdmaSendSmsMessage.h>
#include <modules/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h>
#include <modules/sms/RilRequestImsSendSmsMessage.h>

#include "comdef.h"

class SmsModule: public Module
{
    public:
        SmsModule();
        ~SmsModule();
        void init();
#ifdef QMI_RIL_UTF
        void qcrilHalSmsModuleCleanup();
#endif

    private:
        bool mReady = false;
        std::recursive_mutex mMutex;

        void handleQcrilInit(std::shared_ptr<Message> msg);
        void handleSmsQmiIndMessage(std::shared_ptr<Message> msg);
        void handleQmiAsyncRespMessage(std::shared_ptr<QmiAsyncResponseMessage> msg);

        void handleSendSms(std::shared_ptr<RilRequestSendSmsMessage> msg);
        void handleSendSmsExpectMore(std::shared_ptr<RilRequestSendSmsExpectMoreMessage> msg);
        void handleSmsAcknowledge(std::shared_ptr<RilRequestMessage> msg);
        void handleWriteSmsToSim(std::shared_ptr<RilRequestWriteSmsToSimMessage> msg);
        void handleDeleteSmsOnSim(std::shared_ptr<RilRequestMessage> msg);
        void handleGetSmscAddress(std::shared_ptr<RilRequestMessage> msg);
        void handleSetSmscAddress(std::shared_ptr<RilRequestSetSmscAddressMessage> msg);
        void handleReportSmsMemoryStatus(std::shared_ptr<RilRequestMessage> msg);
        void handleGsmGetBroadcastSmsConfig(std::shared_ptr<RilRequestMessage> msg);
        void handleGsmSetBroadcastSmsConfig(std::shared_ptr<RilRequestGsmSetBroadcastSmsConfigMessage> msg);
        void handleGsmSmsBroadcastActivation(std::shared_ptr<RilRequestMessage> msg);
        void handleCdmaSendSms(std::shared_ptr<RilRequestCdmaSendSmsMessage> msg);
        void handleCdmaSmsAcknowledge(std::shared_ptr<RilRequestMessage> msg);
        void handleCdmaWriteSmsToRuim(std::shared_ptr<RilRequestMessage> msg);
        void handleCdmaDeleteSmsOnRuim(std::shared_ptr<RilRequestMessage> msg);
        void handleCdmaGetBroadcastSmsConfig(std::shared_ptr<RilRequestMessage> msg);
        void handleCdmaSetBroadcastSmsConfig(std::shared_ptr<RilRequestCdmaSetBroadcastSmsConfigMessage> msg);
        void handleCdmaSmsBroadcastActivation(std::shared_ptr<RilRequestMessage> msg);
        void handleImsRegistrationState(std::shared_ptr<RilRequestMessage> msg);
        void handleImsSendSms(std::shared_ptr<RilRequestImsSendSmsMessage> msg);
        void handleSmsEndPointStatusIndMessage(std::shared_ptr<Message> msg);
};

SmsModule *getSmsModule();
