/* ---------------------------------------------------------------------------
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------------- */
#ifndef VENDOR_QTI_VENDOR_RADIO_UIM_REMOTE_SERVER_V1_0_H
#define VENDOR_QTI_VENDOR_RADIO_UIM_REMOTE_SERVER_V1_0_H

#include <vendor/qti/hardware/radio/uim_remote_server/1.0/IUimRemoteServiceServer.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <ril.h>
#include <utils/RWLock.h>

extern "C" {
#include "qcril_uim_sap.h"
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
namespace uim_remote_server {
namespace V1_0 {
namespace implementation {

class UimRemoteServerImpl : public IUimRemoteServiceServer, public hidl_death_recipient {
  sp<IUimRemoteServiceServerResponse> mResponseCb;
  sp<IUimRemoteServiceServerIndication> mIndicationCb;
  int mInstanceId;
  RWLock mCallbackLock;

  sp<IUimRemoteServiceServerResponse> getResponseCallback();
  sp<IUimRemoteServiceServerIndication> getIndicationCallback();

  void clearCallbacks();
  // Functions from hidl_death_recipient
  void serviceDied( uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

  Return<void> setCallback(const sp<IUimRemoteServiceServerResponse>& responseCallback, const sp<IUimRemoteServiceServerIndication>& indicationCallback);
  Return<void> uimRemoteServiceServerConnectReq(int32_t token, int32_t maxMsgSize);
  Return<void> uimRemoteServiceServerDisconnectReq(int32_t token);
  Return<void> uimRemoteServiceServerApduReq(int32_t token, UimRemoteServiceServerApduType type, const hidl_vec<uint8_t>& command);
  Return<void> uimRemoteServiceServerTransferAtrReq(int32_t token);
  Return<void> uimRemoteServiceServerPowerReq(int32_t token, bool state);
  Return<void> uimRemoteServiceServerResetSimReq(int32_t token);
  Return<void> uimRemoteServiceServerTransferCardReaderStatusReq(int32_t token);
  Return<void> uimRemoteServiceServerSetTransferProtocolReq(int32_t token, UimRemoteServiceServerTransferProtocol transferProtocol);

public:
  UimRemoteServerImpl();
  void resetIndicationCallback();
  void resetResponseCallback();
  void setInstanceId(int instanceId);
  void uimRemoteServerProcessRequestResp(int request_id, int err_code, int token_id, uint8_t *data_ptr, uint32_t data_len);
  void uimRemoteServerProcessIndication(qmi_uim_sap_status_type  sap_state);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace uim_remote_server
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_VENDOR_RADIO_UIM_REMOTE_SERVER_V1_0_H
