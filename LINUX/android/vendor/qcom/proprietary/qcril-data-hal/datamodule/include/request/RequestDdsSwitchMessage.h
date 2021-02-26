/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "framework/add_message_id.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class RequestDdsSwitchMessage : public SolicitedMessage<RIL_Errno>,
                                public add_message_id<RequestDdsSwitchMessage> {
    public:
        DDSSubIdInfo dds_sub_info;

        static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_REQUEST_DDS_SWITCH";
        RequestDdsSwitchMessage() = delete;
        RequestDdsSwitchMessage(DDSSubIdInfo ddsSubInfo):
                 SolicitedMessage<RIL_Errno>(get_class_message_id()) {
            mName = MESSAGE_NAME;
            dds_sub_info.dds_sub_id = ddsSubInfo.dds_sub_id;
            dds_sub_info.switch_type = ddsSubInfo.switch_type;
        }
        ~RequestDdsSwitchMessage();
        string dump();
};
}
