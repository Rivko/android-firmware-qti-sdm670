/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestDialMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class dial_translator: public legacy_translator<RIL_REQUEST_DIAL> {
public:
    dial_translator() = default;
    ~dial_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestDialMessage>(request);
    }
};

static dial_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_DIAL, RIL_Dial *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_DIAL);

