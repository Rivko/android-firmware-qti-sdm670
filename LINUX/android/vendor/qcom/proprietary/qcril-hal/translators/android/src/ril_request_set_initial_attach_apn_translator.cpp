/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/nas/NasSetInitialAttachApnRequestMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class ril_request_set_initial_attach_apn_translator: public legacy_translator<RIL_REQUEST_SET_INITIAL_ATTACH_APN> {
public:
    ril_request_set_initial_attach_apn_translator() = default;
    ~ril_request_set_initial_attach_apn_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<NasSetInitialAttachApnRequestMessage>(request);
    }
};

static ril_request_set_initial_attach_apn_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_INITIAL_ATTACH_APN, RIL_InitialAttachApn *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_INITIAL_ATTACH_APN);


