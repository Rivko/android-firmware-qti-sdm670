/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once

#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class SetLteAttachProfileMessage : public SolicitedMessage<RIL_Errno>,
                                   public add_message_id<SetLteAttachProfileMessage> {
private:
    RIL_InitialAttachApn *payload;
    void deepCopy(RIL_InitialAttachApn &);

public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_LTE_ATTACH_PROFILE";

    SetLteAttachProfileMessage() = delete;
    SetLteAttachProfileMessage(RIL_InitialAttachApn &req);
    ~SetLteAttachProfileMessage();

    RIL_InitialAttachApn* get_params();
    string dump();
};

#if (QCRIL_RIL_VERSION >= 15)
class SetLteAttachProfileMessage_v15 : public SolicitedMessage<RIL_Errno>,
                                       public add_message_id<SetLteAttachProfileMessage_v15> {
private:
    RIL_InitialAttachApn_v15 *payload;
    void deepCopy(RIL_InitialAttachApn_v15 &);

public:
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_LTE_ATTACH_PROFILE_v15";

    SetLteAttachProfileMessage_v15() = delete;
    SetLteAttachProfileMessage_v15(RIL_InitialAttachApn_v15 &req);
    ~SetLteAttachProfileMessage_v15();

    RIL_InitialAttachApn_v15* get_params();
    string dump();
};
#endif /* QCRIL_RIL_VERSION >= 15 */

} //namespace
