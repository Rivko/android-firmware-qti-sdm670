/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include <framework/legacy.h>
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "framework/add_message_id.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {

typedef struct
{
    RIL_Errno ril_err_no;
}DeactivateDataCallResp;

class RilRequestDeactivateDataCallMessage : public SolicitedMessage<generic_callback_payload>,
                                            public add_message_id<RilRequestDeactivateDataCallMessage> {
    private:
	legacy_request_payload params;

    public:
    static constexpr const char* MESSAGE_NAME = "RIL_REQUEST_DEACTIVATE_DATA_CALL";
    RilRequestDeactivateDataCallMessage() = delete;
    ~RilRequestDeactivateDataCallMessage();
    void deepCopy(qcril_request_params_type&);

    inline RilRequestDeactivateDataCallMessage( const qcril_request_params_type &p )
             :SolicitedMessage<generic_callback_payload>(get_class_message_id()), params(p) {
        mName = MESSAGE_NAME;
        deepCopy(params.get_params());
   }
   qcril_request_params_type &get_params() {
     return params.get_params();
   }
   string dump();
};

}//namespace
