/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/legacy.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/* Unsol indication to notify SW MBN update status
   @Sender : MbnModule
*/
class SwMbnUpdateStatusInd : public UnSolicitedMessage,
                                public add_message_id<SwMbnUpdateStatusInd>
{
private:
    bool updateStatus;

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.mbn.sw_mbn_update_status";
    SwMbnUpdateStatusInd() = default;
    ~SwMbnUpdateStatusInd() {};

    inline SwMbnUpdateStatusInd(bool state):
                        UnSolicitedMessage(get_class_message_id()),updateStatus(state) {
      mName = MESSAGE_NAME;
    }

    bool getData() { return updateStatus; };

    string dump() {
        return mName + "{}";
    }

    std::shared_ptr<UnSolicitedMessage> clone() {
        return (std::make_shared<SwMbnUpdateStatusInd>(updateStatus));
    }
};
