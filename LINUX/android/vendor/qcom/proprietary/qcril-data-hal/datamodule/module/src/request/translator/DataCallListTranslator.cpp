/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
//#include <framework/Log.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>
#include "request/RilRequestDataCallListMessage.h"
#include "legacy/qcril_event_facade.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

class DataCallListTranslator : public legacy_translator<RIL_REQUEST_DATA_CALL_LIST> {
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
        return std::make_shared<rildata::RilRequestDataCallListMessage>(params);

    }
};

static DataCallListTranslator datacalllist_translator;

/* NOTE: request and response types from ril.h */
// \todo need to handle else case?
#if (QCRIL_RIL_VERSION >= 11)
    QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_DATA_CALL_LIST, void *,  RIL_Data_Call_Response_v11, datacalllist_translator);
    QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_DATA_CALL_LIST);
#endif
