/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
//#include <framework/Log.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>
#include "legacy/qcril_event_facade.h"
#include "request/RilRequestSetupDataCallMessage.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

class SetupDataCallTranslator : public legacy_translator<RIL_REQUEST_SETUP_DATA_CALL> {
public:
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
        qcril_request_params_type params {
            request.instance_id,
            QCRIL_DEFAULT_MODEM_ID, /* Always pass in dfault */
            request.req_id_android,
            request.req_id, /* event_id --> req_id */
            const_cast<void *>(request.payload), /* no deepcopy here, just pass it in to the message for that */
            request.payload_len,
            request.t,
            NULL
        };
        return std::make_shared<rildata::RilRequestSetupDataCallMessage>(params);

    }
};

static SetupDataCallTranslator setupdatacall_translator;

#if (QCRIL_RIL_VERSION >= 11)
    QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SETUP_DATA_CALL, char**, RIL_Data_Call_Response_v11, setupdatacall_translator);
    QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SETUP_DATA_CALL);
#else
    #warning "Failed to define RIL_REQUEST_SETUP_DATA_CALL for ril version #QCRIL_RIL_VERSION"
#endif
