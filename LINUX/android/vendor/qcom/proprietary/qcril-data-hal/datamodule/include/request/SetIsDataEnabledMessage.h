/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "modules/data/DataCommon.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {

class SetIsDataEnabledMessage : public SolicitedMessage<RIL_Errno>,
                                public add_message_id<SetIsDataEnabledMessage> {
    private:
	legacy_request_payload params;

    public:
    bool is_data_enabled;
    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_IS_DATA_ENABLED";
    SetIsDataEnabledMessage()= delete;
    inline SetIsDataEnabledMessage( const qcril_request_params_type &p ):
               SolicitedMessage<RIL_Errno>(get_class_message_id()),params(p) {
      mName = MESSAGE_NAME;
      is_data_enabled = *((uint8_t *)p.data);
    }
    ~SetIsDataEnabledMessage();
    qcril_request_params_type &get_params() {
          return params.get_params();
    }
    string dump();
};
}
