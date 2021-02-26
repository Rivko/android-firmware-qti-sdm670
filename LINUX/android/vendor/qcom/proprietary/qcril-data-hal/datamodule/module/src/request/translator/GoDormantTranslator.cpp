#if 0
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
//#include <framework/Log.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>
#include "request/RilRequestGoDormantMessage.h"
#include "legacy/qcril_event_facade.h"
#include "qcril_qmi_oem_reqlist.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

class GoDormantTranslator : public legacy_translator<QCRIL_REQ_HOOK_DATA_GO_DORMANT> {
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
        return std::make_shared<rildata::RilRequestGoDormantMessage>(params);

    }
};

static GoDormantTranslator godormant_translator;

#if (QCRIL_RIL_VERSION >= 12)
    QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(HOOK_DATA_GO_DORMANT, char*, RIL_Errno, godormant_translator);
    QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(HOOK_DATA_GO_DORMANT);
#else
    #warning "Failed to define QCRIL_REQ_HOOK_DATA_GO_DORMANT for ril version #QCRIL_RIL_VERSION"
#endif
#endif
