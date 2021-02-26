/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "framework/add_message_id.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class GetDdsSubIdMessage : public SolicitedMessage<DDSSubIdInfo>,
                           public add_message_id<GetDdsSubIdMessage> {

    public:
        static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_GET_DDS_SUB";

        GetDdsSubIdMessage():SolicitedMessage<DDSSubIdInfo>(get_class_message_id()) {
            mName = MESSAGE_NAME;
        }
        ~GetDdsSubIdMessage();

        string dump();
};
}
