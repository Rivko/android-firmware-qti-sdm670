#pragma once
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>
#include <modules/android/ril_message_factory.h>

class AndroidRequestRegisterMessage: public SolicitedMessage<bool>, public add_message_id<AndroidRequestRegisterMessage>
{
    private:
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.android.register_request";

        unsigned request_id;
        ril_factory_method factory;
        string dump() {
            return AndroidRequestRegisterMessage::MESSAGE_NAME;
        }
        AndroidRequestRegisterMessage(unsigned id, const ril_factory_method &factory):
            SolicitedMessage<bool>(get_class_message_id()),
            request_id(id),
            factory(factory)
        {
            mName = MESSAGE_NAME;
        }

};
