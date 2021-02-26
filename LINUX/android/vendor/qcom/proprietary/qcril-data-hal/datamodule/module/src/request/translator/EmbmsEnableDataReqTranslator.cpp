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

#include "request/RilRequestEmbmsEnableDataReqMessage.h"
#include "legacy/qcril_event_facade.h"

#include "qcril_qmi_oem_reqlist.h"
// ***** IMPORTANT *******
// NOTE: version.h always be included when using
// QCRIL_RIL_VERSION
// ***********************
#include "modules/android/version.h"

class EmbmsEnableDataReqTranslator : public legacy_translator<QCRIL_REQ_HOOK_EMBMS_ENABLE> {
public:
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
       return std::make_shared<rildata::RilRequestEmbmsEnableDataReqMessage>();
       #pragma unused(request)
    }
};


static EmbmsEnableDataReqTranslator EmbmsEnableDataReq_translator;

#if (QCRIL_RIL_VERSION >= 12)
    QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(HOOK_EMBMS_ENABLE_DATA_REQ, void,
                                            embms_enable_embms_resp_msg_v01, EmbmsEnableDataReq_translator);
    QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(HOOK_EMBMS_ENABLE_DATA_REQ);
#else
    /*TODO*/
#endif
#endif
