/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "modules/data/DataCommon.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"

namespace rildata {

class SetRatPrefMessage : public SolicitedMessage<RIL_Errno>,public add_message_id<SetRatPrefMessage> {
    private:

    public:
    int ratPrefType;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_RAT_PREF";
	SetRatPrefMessage() = delete;
    inline SetRatPrefMessage( int rat_pref ):SolicitedMessage<RIL_Errno>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        ratPrefType = rat_pref;
    }
    ~SetRatPrefMessage();

    string dump();
};
}
