/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include "framework/SolicitedMessage.h"
#include "modules/data/DataCommon.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"

namespace rildata {

class UpdateMtuMessage : public SolicitedMessage<int>,public add_message_id<UpdateMtuMessage>{
    public:

    unsigned int Mtu;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_UPDATE_MTU";
    UpdateMtuMessage() = delete;

    inline UpdateMtuMessage( unsigned int mtu ):SolicitedMessage<int>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        Mtu = mtu;
    }
    ~UpdateMtuMessage();

    string dump();
};
}
