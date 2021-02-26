/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestChangeBarringPasswordMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class change_barring_password_translator: public legacy_translator<RIL_REQUEST_CHANGE_BARRING_PASSWORD> {
public:
    change_barring_password_translator() = default;
    ~change_barring_password_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestChangeBarringPasswordMessage>(request);
    }
};

static change_barring_password_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_CHANGE_BARRING_PASSWORD,
        char **, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CHANGE_BARRING_PASSWORD);
        
