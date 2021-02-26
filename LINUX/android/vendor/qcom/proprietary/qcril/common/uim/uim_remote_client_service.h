/* ---------------------------------------------------------------------------
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------------- */
#ifndef VENDOR_QTI_HARDWARE_RADIO_UIM_REMOTE_CLIENT_V1_0_H
#define VENDOR_QTI_HARDWARE_RADIO_UIM_REMOTE_CLIENT_V1_0_H

#include <vendor/qti/hardware/radio/uim_remote_client/1.0/IUimRemoteServiceClient.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <ril.h>
#include <utils/RWLock.h>

extern "C" {
  #include "qcril_uim_remote.h"
}

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
namespace uim_remote_client {
namespace V1_0 {
namespace implementation {

class UimRemoteClientImpl : public IUimRemoteServiceClient, public hidl_death_recipient {
  sp<IUimRemoteServiceClientResponse> mResponseCb;
  sp<IUimRemoteServiceClientIndication> mIndicationCb;
  int mInstanceId;
  RWLock mCallbackLock;

  sp<IUimRemoteServiceClientResponse> getResponseCallback();
  sp<IUimRemoteServiceClientIndication> getIndicationCallback();

  void clearCallbacks();
  // Functions from hidl_death_recipient
  void serviceDied( uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

  // Methods from IUimRemoteServiceClientResponse
  Return<void> setCallback(const sp<IUimRemoteServiceClientResponse>& responseCallback, const sp<IUimRemoteServiceClientIndication>& indicationCallback);
  Return<void> UimRemoteServiceClientEventReq(int32_t token, const UimRemoteEventReqType& eventReq);
  Return<void> UimRemoteServiceClientApduReq(int32_t token, UimRemoteClientApduStatus apduStatus, const hidl_vec<uint8_t>& apduResponse);

public:
  UimRemoteClientImpl();
  void setInstanceId(int instanceId);
  void uimRemoteClientEventResponse(int32_t token, qcril_uim_remote_event_resp_type eventResp);
  void uimRemoteClientApduResponse(int32_t token, qcril_uim_remote_apdu_resp_type apduResp);
  void uimRemoteClientSendApduInd(uint32_t apduLen, uint8_t * apduPtr);
  void uimRemoteClientConnectInd();
  void uimRemoteClientDisconnectInd();
  void uimRemoteClientPowerUpInd(bool hasTimeOut,
  int32_t timeOut,
  bool hasVoltageClass,
  qcril_uim_remote_voltage_class_type voltageClass);
  void uimRemoteClientPowerDownInd(bool hasMode, qcril_uim_remote_power_down_mode mode);
  void uimRemoteClientResetInd();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace uim_remote_client
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_RADIO_UIM_REMOTE_CLIENT_V1_0_H
