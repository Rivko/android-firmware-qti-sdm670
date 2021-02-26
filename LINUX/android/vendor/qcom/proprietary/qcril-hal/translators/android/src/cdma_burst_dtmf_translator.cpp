/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <modules/voice/RilRequestCdmaBurstDtmfMessage.h>
#include <translators/android/legacy_translator.h>
#include <telephony/ril.h>

/**
 * cdma_burst_dtmf_translator : Translator for RIL_REQUEST_CDMA_BURST_DTMF
 */
class cdma_burst_dtmf_translator : public legacy_translator<RIL_REQUEST_CDMA_BURST_DTMF> {
  public:
    cdma_burst_dtmf_translator() = default;
    ~cdma_burst_dtmf_translator() {};
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(
        const ril_request_type &request) {
      return std::make_shared<RilRequestCdmaBurstDtmfMessage>(request);
    }
};

static cdma_burst_dtmf_translator the_translator;

QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_BURST_DTMF, char **, void, the_translator);
QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(RIL_REQUEST_CDMA_BURST_DTMF);
