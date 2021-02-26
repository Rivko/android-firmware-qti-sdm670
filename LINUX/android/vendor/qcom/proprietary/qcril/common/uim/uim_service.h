/******************************************************************************
@file    uim_service.h
@brief   qcril uim service

DESCRIPTION
Implements the server side of the IUim interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_V1_0_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_V1_0_H

#include <vendor/qti/hardware/radio/uim/1.0/IUim.h>
#include <vendor/qti/hardware/radio/uim/1.1/IUim.h>
#include <vendor/qti/hardware/radio/uim/1.0/IUimResponse.h>
#include <vendor/qti/hardware/radio/uim/1.1/IUimResponse.h>
#include <vendor/qti/hardware/radio/uim/1.0/types.h>
#include <vendor/qti/hardware/radio/uim/1.1/types.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/RWLock.h>

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
namespace uim {
namespace V1_0 {
namespace implementation {

class UimImpl : public V1_1::IUim, public hidl_death_recipient {
  sp<IUimResponse> mResponseCb;
  sp<V1_1::IUimResponse> mResponseCbV1_1;
  sp<IUimIndication> mIndicationCb;
  int mInstanceId;
  RWLock mCallbackLock;

  sp<IUimResponse> getResponseCallback();
  sp<V1_1::IUimResponse> getResponseCallbackV1_1();
  sp<IUimIndication> getIndicationCallback();

  void clearCallbacks();
  // Functions from hidl_death_recipient
  void serviceDied( uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

  // Methods from IUimResponse
  Return<void> setCallback(const sp<IUimResponse>& responseCallback, const sp<IUimIndication>& indicationCallback);
  Return<void> UimRemoteSimlockRequest(int32_t token, UimRemoteSimlockOperationType simlockOp, const hidl_vec<uint8_t>& simlockData);
  Return<void> uimRemoteSimlockRequest_1_1(int32_t token,
         V1_1::UimRemoteSimlockOperationType simlockOp, const hidl_vec<uint8_t>& simlockData);

public:
  UimImpl();
  void resetIndicationCallback();
  void resetResponseCallback();
  void setInstanceId(int instanceId);
  void uimRemoteSimlockResponse(int token, UimRemoteSimlockResponseType resp, V1_1::UimRemoteSimlockOperationType op_type,
    uint8_t * rsp_ptr, uint32_t rsp_len, UimRemoteSimlockVersion version, UimRemoteSimlockStatus status, int timer_value);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace uim
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_RADIO_UIM_V1_0_H
