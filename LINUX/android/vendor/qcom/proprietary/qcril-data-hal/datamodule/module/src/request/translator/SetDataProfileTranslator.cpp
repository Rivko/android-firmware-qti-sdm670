/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
//#include <framework/Log.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

#include "request/RilRequestSetDataProfileMessage.h"
#include "legacy/qcril_event_facade.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"


#if (QCRIL_RIL_VERSION < 15)

class SetDataProfileTranslator : public legacy_translator<RIL_REQUEST_SET_DATA_PROFILE> {
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
        return std::make_shared<rildata::RilRequestSetDataProfileMessage>(params);

    }
};


static SetDataProfileTranslator SetDataProfile_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_DATA_PROFILE, RIL_DataProfileInfo **,
                                            void, SetDataProfile_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_DATA_PROFILE);

#else

class SetDataProfileTranslator_v15 : public legacy_translator<RIL_REQUEST_SET_DATA_PROFILE> {
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
        return std::make_shared<rildata::RilRequestSetDataProfileMessage_v15>(params);

    }
};


static SetDataProfileTranslator_v15 SetDataProfile_translator_v15;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_DATA_PROFILE, RIL_DataProfileInfo_v15 **,
                                            void, SetDataProfile_translator_v15);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_DATA_PROFILE);

#endif /* QCRIL_RIL_VERSION >= 15 */
