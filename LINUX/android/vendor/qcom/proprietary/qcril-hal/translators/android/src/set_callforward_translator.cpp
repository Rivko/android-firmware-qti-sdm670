/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestSetCallForwardMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class set_callforward_translator: public legacy_translator<RIL_REQUEST_SET_CALL_FORWARD> {
public:
    set_callforward_translator() = default;
    ~set_callforward_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestSetCallForwardMessage>(request);
    }
};

static set_callforward_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_CALL_FORWARD,
        RIL_CallForwardInfo*, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_CALL_FORWARD);
