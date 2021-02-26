/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/GenericCallback.h"
#include "framework/message_translator.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class ToggleDormancyIndMessage : public SolicitedMessage<int>,
                                 public add_message_id<ToggleDormancyIndMessage> {

    public:

    qcril_data_dormancy_ind_switch_type dormIndSwitch;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_TOGGLE_DORMANCY_INDICATION";
    ToggleDormancyIndMessage() = delete;
    inline ToggleDormancyIndMessage( qcril_data_dormancy_ind_switch_type const & dorm_ind_switch ):
                SolicitedMessage<int>(get_class_message_id()) {
        mName = MESSAGE_NAME;
        dormIndSwitch = dorm_ind_switch;
    }
    ~ToggleDormancyIndMessage();
    string dump();
};

}
