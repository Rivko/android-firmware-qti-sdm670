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
#include <modules/android/RilRequestMessage.h>
#include "legacy/qcril_data_defs.h"
#include "modules/data/DataCommon.h"

namespace rildata {

class RilRequestGetDataCallProfileMessage : public SolicitedMessage<generic_callback_payload>,
                                            public add_message_id<RilRequestGetDataCallProfileMessage> {
    private:
    legacy_request_payload params;

    public:
    /**
    * Any user data provided by the caller that we
    * need to pass back.
    */
    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_GET_DATA_CALL_PROFILE";
    RilRequestGetDataCallProfileMessage() = delete;
    ~RilRequestGetDataCallProfileMessage();

    inline RilRequestGetDataCallProfileMessage( const qcril_request_params_type &p ):
              SolicitedMessage<generic_callback_payload>(get_class_message_id()),params(p){
      mName = MESSAGE_NAME;
    }
    string dump();
    qcril_request_params_type &get_params() {
      return params.get_params();
    }
};

}//namespace
