/******************************************************************************
  @file    qcril_qmi_oemhook_service.h
  @brief   qcril qmi - oemhook_service

  DESCRIPTION
    Implements the server side of the IQtiOemHook interface.

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef VENDOR_QCOM_HARDWARE_QCRIL_QCRILHOOK_V1_0_QTIOEMHOOK_H
#define VENDOR_QCOM_HARDWARE_QCRIL_QCRILHOOK_V1_0_QTIOEMHOOK_H

#include <vendor/qti/hardware/radio/qcrilhook/1.0/IQtiOemHook.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <ril.h>
#include <utils/RWLock.h>

extern "C" {
    #include "qcrili.h"
}

void processIncomingOemhookMessage
(
    qcril_instance_id_e_type oemSocketInstanceId,
    int serial,
    unsigned char *data,
    size_t dataLen
);

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::RWLock;
using ::android::sp;
using ::android::wp;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qcrilhook {
namespace V1_0 {
namespace implementation {

uint8_t oemHookRegisterService(qcril_instance_id_e_type oemSocketInstanceId);
void sendOemhookResponse
(
    qcril_instance_id_e_type oemSocketInstanceId,
    int32_t serial,
    RIL_Errno error,
    unsigned char* buf,
    size_t bufLen
);

void sendOemhookIndication
(
    qcril_instance_id_e_type oemSocketInstanceId,
    unsigned char *buf,
    size_t bufLen
);

class OemHookImpl : public IQtiOemHook, public hidl_death_recipient {
    sp<IQtiOemHookResponse> mResponseCb;
    sp<IQtiOemHookIndication> mIndicationCb;
    qcril_instance_id_e_type mInstanceId;
    RWLock mCallbackLock;

    sp<IQtiOemHookResponse> getResponseCallback();
    sp<IQtiOemHookIndication> getIndicationCallback();

    // Function from hidl_death_recipient
    void serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

#ifdef QMI_RIL_UTF
  public:
    // make setCallback and clearCallbacks as public for UTF
#endif
    void clearCallbacks();

    // Functions from IQtiOemHook
    Return<void> setCallback(const sp<IQtiOemHookResponse> &responseCallback,
                             const sp<IQtiOemHookIndication> &indicationCallback);
    Return<void> oemHookRawRequest(int32_t serial, const hidl_vec<uint8_t>& data);

    public:
        OemHookImpl();
        void sendResponse(int32_t serial, RIL_Errno error, unsigned char* buf, size_t bufLen);
        void sendIndication(unsigned char *buf, size_t bufLen);
        void setInstanceId(qcril_instance_id_e_type oemSocketInstanceId);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace qcrilhook
}  // namespace radio
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#ifdef QMI_RIL_UTF
using namespace vendor::qti::hardware::radio::qcrilhook::V1_0::implementation;

sp<OemHookImpl> &getOemHook();

extern "C" void resetOemHook();

#endif

#endif  // VENDOR_QCOM_HARDWARE_QCRIL_QCRILHOOK_V1_0_QTIOEMHOOK_H
