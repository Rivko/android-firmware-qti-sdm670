/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestQueryCallForwardMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class query_callforward_translator: public legacy_translator<RIL_REQUEST_QUERY_CALL_FORWARD_STATUS> {
public:
    query_callforward_translator() = default;
    ~query_callforward_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestQueryCallForwardMessage>(request);
    }
};

static query_callforward_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,
        RIL_CallForwardInfo*, RIL_CallForwardInfo **, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_QUERY_CALL_FORWARD_STATUS);
