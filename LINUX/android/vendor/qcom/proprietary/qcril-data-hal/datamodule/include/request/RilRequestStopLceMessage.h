#pragma once

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <framework/legacy.h>
#include "framework/Message.h"
#include "modules/data/DataCommon.h"
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>


namespace rildata {

class RilRequestStopLceMessage : public SolicitedMessage<generic_callback_payload>,
                                 public add_message_id<RilRequestStopLceMessage> {
    private:

    legacy_request_payload params;

    public:

    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_STOP_LCE";
    RilRequestStopLceMessage() = delete;
    ~RilRequestStopLceMessage();

    inline RilRequestStopLceMessage(const qcril_request_params_type &p):
               SolicitedMessage<generic_callback_payload>(get_class_message_id()),params(p) {
       mName = MESSAGE_NAME;
    }

    qcril_request_params_type &get_params() {
      return params.get_params();
    }
    string dump();
};

}//namespace
