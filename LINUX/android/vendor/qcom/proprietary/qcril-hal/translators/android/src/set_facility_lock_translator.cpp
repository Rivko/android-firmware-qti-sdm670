/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestSetFacilityLockMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class set_facility_lock_translator: public legacy_translator<RIL_REQUEST_SET_FACILITY_LOCK> {
public:
    set_facility_lock_translator() = default;
    ~set_facility_lock_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestSetFacilityLockMessage>(request);
    }
};

static set_facility_lock_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_FACILITY_LOCK,
        char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_FACILITY_LOCK);
        
