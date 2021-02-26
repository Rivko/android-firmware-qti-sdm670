/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/nas/NasSetNetworkSelectionManualRequestMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

class ril_request_set_network_selection_manual_translator: public legacy_translator<RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL> {
public:
    ril_request_set_network_selection_manual_translator() = default;
    ~ril_request_set_network_selection_manual_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request)
    {
      return std::make_shared<NasSetNetworkSelectionManualRequestMessage>(request);
    }
};

static ril_request_set_network_selection_manual_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, char *, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL);


