/*!
  @file
  qcril_qmi_ims_radio_service.h

  @brief

*/

/*===========================================================================

   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef IMS_RADIO_SERVICE_H
#define IMS_RADIO_SERVICE_H

#include "comdef.h"
#include "imsIF.pb.h"
#include "ril.h"
#include <utils/RWLock.h>

#include <vendor/qti/hardware/radio/ims/1.0/IImsRadio.h>
#include <hidl/HidlSupport.h>

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::sp;
using ::android::RWLock;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace ims {
namespace V1_0 {
namespace implementation {

void imsRadioProcessResponse(ims_MsgTag tag, void* data);
void imsRadioRegisterService();

/** Class implementing the IImsRadio server implementation for all requests coming in
 * from Ims to RIL. Also registers the callback of type IImsRadioResponse to send the
 * solicited response back and IImsRadioIndication to send unsolicited responses.
 */
class ImsRadioImpl : public IImsRadio, public hidl_death_recipient {
    sp<IImsRadioResponse> mImsRadioResponseCb;
    sp<IImsRadioIndication> mImsRadioIndicationCb;
    RWLock mCallbackLock;

    sp<IImsRadioIndication> getIndicationCallback();
    sp<IImsRadioResponse> getResponseCallback();

    void serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase>&);
    void sendEmptyErrorResponse(int32_t token, ims_MsgId id, ims_Error error);
    void callForwardStatusInternal(int32_t token, const CallForwardInfo& callForwardInfo,
            ims_MsgId id);
#ifdef QMI_RIL_UTF
public:
    // make setCallback and clearCallbacks as public for UTF
#endif
    Return<void> setCallback(
      const sp<IImsRadioResponse>& mImsRadioResponseCbParam,
      const sp<IImsRadioIndication>& mImsRadioIndicationCbParam);
    void clearCallbacks();

public:
    void processResponse(ims_MsgTag tag, void* data);
    ImsRadioImpl();
    ~ImsRadioImpl();
    ims_Error processRequest(int32_t token, ims_MsgId id, void* data);
    Return<void> dial(int32_t token, const DialRequest& dialRequest);
    Return<void> getImsRegistrationState(int32_t token);
    Return<void> answer(int32_t token, CallType callType, IpPresentation presentation,
                         RttMode rttMode);
    Return<void> hangup(int32_t token, const HangupRequestInfo& hangup);
    Return<void> requestRegistrationChange(int32_t token, RegState state);
    Return<void> queryServiceStatus(int32_t token);
    Return<void> setServiceStatus(int32_t token, const ServiceStatusInfo& srvStatusInfo);
    Return<void> hold(int32_t token, uint32_t callId);
    Return<void> resume(int32_t token, uint32_t callId);
    Return<void> setConfig(int32_t token, const ConfigInfo& config);
    Return<void> getConfig(int32_t token, const ConfigInfo& config);
    Return<void> conference(int32_t token);
    Return<void> getClip(int32_t token);
    Return<void> getClir(int32_t token);
    Return<void> setClir(int32_t token, const ClirInfo& clirInfo);
    Return<void> getColr(int32_t token);
    Return<void> setColr(int32_t token, const ColrInfo& colrInfo);
    Return<void> exitEmergencyCallbackMode(int32_t token);
    Return<void> sendDtmf(int32_t token, const DtmfInfo& dtmfInfo);
    Return<void> startDtmf(int32_t token, const DtmfInfo& dtmfInfo);
    Return<void> stopDtmf(int32_t token);
    Return<void> setUiTtyMode(int32_t token, const TtyInfo& ttyInfo);
    Return<void> modifyCallInitiate(int32_t token, const CallModifyInfo& callModifyInfo);
    Return<void> cancelModifyCall(int32_t token, uint32_t callId);
    Return<void> modifyCallConfirm(int32_t token, const CallModifyInfo& callModifyInfo);
    Return<void> getCallWaiting(int32_t token, uint32_t serviceClass);
    Return<void> setSuppServiceNotification(int32_t token, ServiceClassStatus status);
    Return<void> queryCallForwardStatus(int32_t token, const CallForwardInfo& callForwardInfo);
    Return<void> setCallForwardStatus(int32_t token, const CallForwardInfo& callForwardInfo);
    Return<void> setCallWaiting(int32_t token, ServiceClassStatus inStatus,
            uint32_t inServiceClass);
    Return<void> explicitCallTransfer(int32_t token, const ExplicitCallTransferInfo& ectInfo);
    Return<void> suppServiceStatus(int32_t token, int32_t operationType, FacilityType facilityType,
            const CbNumListInfo& cbNumListInfo);
    Return<void> deflectCall(int32_t token, const DeflectRequestInfo& deflectRequestInfo);
    template<typename T> void checkReturnStatus(const Return<T>& ret);
    Return<void> getRtpStatistics(int32_t token);
    Return<void> getRtpErrorStatistics(int32_t token);
    Return<void> addParticipant(int32_t token, const DialRequest& dialRequest);
    Return<void> sendGeolocationInfo(int32_t token, double lat, double lon,
            const AddressInfo& addressInfo);
    Return<void> getImsSubConfig(int32_t token);
    boolean hasValidCallbacks();
    boolean sendMessage(RIL_Token token, ims_MsgType type, ims_MsgId message_id,
            ims_Error error, const void* msg, int msg_len);
    Return<void> sendRttMessage(int32_t token, const hidl_string& rttMessage);
};
} // namespace implementation
} // namespace V1_0
} // namespace ims
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
#endif  // IMS_RADIO_SERVICE_H
