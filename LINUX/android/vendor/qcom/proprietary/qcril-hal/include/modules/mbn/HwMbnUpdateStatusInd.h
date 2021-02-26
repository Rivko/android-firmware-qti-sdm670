/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#pragma once
#include "framework/legacy.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/* Unsol indication to notify HW MBN update status
   @Sender : MbnModule
   @Receiver: NasModule
*/

class HwMbnUpdateStatusInd : public UnSolicitedMessage,
                                public add_message_id<HwMbnUpdateStatusInd>
{
private:
    bool mUpdateStatus;

public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.mbn.hw_mbn_update_status";
    HwMbnUpdateStatusInd() = default;
    ~HwMbnUpdateStatusInd() {};

    inline HwMbnUpdateStatusInd(bool state):
                        UnSolicitedMessage(get_class_message_id()),mUpdateStatus(state) {
      mName = MESSAGE_NAME;
    }

    bool getStatus() { return mUpdateStatus; };

    string dump() {
        return mName + "{}";
    }

    std::shared_ptr<UnSolicitedMessage> clone() {
        return (std::make_shared<HwMbnUpdateStatusInd>(mUpdateStatus));
    }
};
