/******************************************************************************
@file    uim_lpa_service.h
@brief   qcril uim lpa service

DESCRIPTION
Implements the server side of the IUimLpa interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_0_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_0_H

#include <vendor/qti/hardware/radio/lpa/1.0/IUimLpa.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/RWLock.h>
#include "modules/lpa/lpa_service_types.h"

class LpaModule;

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::sp;
using ::android::wp;
using ::android::RWLock;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_0 {
namespace implementation {

class UimLpaImpl : public IUimLpa, public hidl_death_recipient {
  sp<IUimLpaResponse> mResponseCb;
  sp<IUimLpaIndication> mIndicationCb;
  int mInstanceId;
  RWLock mCallbackLock;
  LpaModule * mModule;

  sp<IUimLpaResponse> getResponseCallback();
  sp<IUimLpaIndication> getIndicationCallback();

  void clearCallbacks();
  // Functions from hidl_death_recipient
  void serviceDied( uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

  // Methods from IUimLpaResponse
  Return<void> setCallback(const sp<IUimLpaResponse>& responseCallback, const sp<IUimLpaIndication>& indicationCallback);
  Return<void> UimLpaUserRequest(int32_t token, const UimLpaUserReq& userReq);
  Return<void> UimLpaHttpTxnCompletedRequest(int32_t token, UimLpaResult result, const hidl_vec<uint8_t>& responsePayload, const hidl_vec<UimLpaHttpCustomHeader>& customHeaders);

public:
  UimLpaImpl(LpaModule * module);
  void resetIndicationCallback();
  void resetResponseCallback();
  void setInstanceId(int instanceId);
  void uimLpaHttpTxnCompletedResponse(int32_t token, lpa_service_result_type result);
  void uimLpaUserResponse(int32_t token, lpa_service_user_resp_type * user_resp);
  void uimLpaAddProfileProgressInd(lpa_service_add_profile_progress_ind_type * progInd);
  void uimLpaHttpTxnIndication(lpa_service_http_transaction_ind_type * txnInd);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace lpa
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_RADIO_UIM_LPA_V1_0_H
