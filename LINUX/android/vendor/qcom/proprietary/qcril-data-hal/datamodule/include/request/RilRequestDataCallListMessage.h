/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#pragma once
#include <framework/legacy.h>
#include "modules/data/DataCommon.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include <string.h>
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {

class RilRequestDataCallListMessage : public SolicitedMessage<generic_callback_payload>,
                                      public add_message_id<RilRequestDataCallListMessage> {

    private:
    legacy_request_payload params;
    public:
    static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_DATA_CALL_LIST";
    ~RilRequestDataCallListMessage();
    inline RilRequestDataCallListMessage( const qcril_request_params_type &p ):
                SolicitedMessage<generic_callback_payload>(get_class_message_id()),params(p) {
        mName = MESSAGE_NAME;
    }
    string dump();
    qcril_request_params_type &get_params() {
        return params.get_params();
    }
};

}// namespace
