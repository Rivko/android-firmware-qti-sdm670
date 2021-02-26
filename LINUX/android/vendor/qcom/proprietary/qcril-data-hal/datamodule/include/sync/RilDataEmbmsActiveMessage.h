/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/add_message_id.h"

namespace rildata {

class RilDataEmbmsActiveMessage : public SolicitedSyncMessage<bool>,public add_message_id<RilDataEmbmsActiveMessage> {

    public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_GET_EMBMS_ACTIVE";
    inline RilDataEmbmsActiveMessage():SolicitedSyncMessage<bool>(get_class_message_id()) {
        mName = MESSAGE_NAME;
    }
    ~RilDataEmbmsActiveMessage();
    string dump();
};
}
