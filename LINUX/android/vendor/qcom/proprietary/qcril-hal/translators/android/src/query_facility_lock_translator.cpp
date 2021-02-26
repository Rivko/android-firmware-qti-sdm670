/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestQueryFacilityLockMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class query_facility_lock_translator: public legacy_translator<RIL_REQUEST_QUERY_FACILITY_LOCK> {
public:
    query_facility_lock_translator() = default;
    ~query_facility_lock_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<RilRequestQueryFacilityLockMessage>(request);
    }
};

static query_facility_lock_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_QUERY_FACILITY_LOCK,
        char **, int *, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_QUERY_FACILITY_LOCK);

