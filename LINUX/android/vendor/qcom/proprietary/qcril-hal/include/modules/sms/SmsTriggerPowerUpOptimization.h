/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

class SmsTriggerPowerUpOptimization: public UnSolicitedMessage, public add_message_id<SmsTriggerPowerUpOptimization>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.sms.triggerpowerupoptimization";
    public:
        string dump() {
            return SmsTriggerPowerUpOptimization::MESSAGE_NAME;
        }
        SmsTriggerPowerUpOptimization():
            UnSolicitedMessage(get_class_message_id())
        {
            mName = MESSAGE_NAME;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            return (std::make_shared<SmsTriggerPowerUpOptimization>());
        }

        inline ~SmsTriggerPowerUpOptimization() {}
};
