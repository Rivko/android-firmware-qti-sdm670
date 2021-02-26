/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include <framework/legacy.h>
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {

class RilRequestLastDataCallFailCauseMessage : public SolicitedMessage<generic_callback_payload>,
                                               public add_message_id<RilRequestLastDataCallFailCauseMessage> {
    private:
    legacy_request_payload params;
    public:
    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE";
    ~RilRequestLastDataCallFailCauseMessage();

    inline RilRequestLastDataCallFailCauseMessage(const qcril_request_params_type &p):
              SolicitedMessage<generic_callback_payload>(get_class_message_id()),params(p){
        mName = MESSAGE_NAME;
    }
    qcril_request_params_type &get_params() {
      return params.get_params();
    }
    string dump();
};

}//namespace
