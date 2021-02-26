/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class ToggleLimitedSysIndMessage : public SolicitedMessage<int>,
                                   public add_message_id<ToggleLimitedSysIndMessage> {
    public:

    qcril_data_limited_sys_ind_switch_type sysIndSwitch;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_TOGGLE_LIMITED_SYS_INDICATION";
    ToggleLimitedSysIndMessage() = delete;
    inline ToggleLimitedSysIndMessage( qcril_data_limited_sys_ind_switch_type const & sys_ind_switch ):
                   SolicitedMessage<int>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        sysIndSwitch = sys_ind_switch;
    }
    ~ToggleLimitedSysIndMessage();

    string dump();
};
}
