/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
  * SmsImsNetworkStateChangedInd: Sent by clients to trigger an unsol via the CS pipe
  */
class SmsImsNetworkStateChangedInd: public UnSolicitedMessage, public add_message_id<SmsImsNetworkStateChangedInd>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.sms.imsnetworkstatechangedind";
    public:
        string dump() {
            return SmsImsNetworkStateChangedInd::MESSAGE_NAME;
        }
        SmsImsNetworkStateChangedInd():
            UnSolicitedMessage(get_class_message_id())
        {
            mName = MESSAGE_NAME;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            return (std::make_shared<SmsImsNetworkStateChangedInd>());
        }

        inline ~SmsImsNetworkStateChangedInd() {}
};
