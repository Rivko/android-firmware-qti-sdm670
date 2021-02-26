/******************************************************************************
  @file    qcril_qmi_qti_radio_service.h
  @brief   qcril qmi - qtiRadioService_service
---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
  ******************************************************************************/

#ifndef VENDOR_QTI_HARDWARE_QTIRADIO_V1_0_QTIRADIO_H
#define VENDOR_QTI_HARDWARE_QTIRADIO_V1_0_QTIRADIO_H

#include <framework/legacy.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <telephony/ril.h>
#include <utils/RWLock.h>
#include <vendor/qti/hardware/radio/qtiradio/1.0/IQtiRadio.h>

using ::android::RWLock;
using ::android::hardware::Return;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_string;
using ::android::sp;
using ::android::wp;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V1_0 {
namespace implementation {

RIL_Token qcril_qmi_qti_convert_radio_token_to_ril_token(uint32_t oem_token);
uint32_t qcril_qmi_qti_free_and_convert_ril_token_to_radio_token(RIL_Token ril_token);
void qcril_qmi_qti_radio_service_init(int instanceId);
void qtiGetAtrResponse(RIL_Token token, RIL_Errno error, char *buf, int bufLen);

class QtiRadioImpl : public IQtiRadio, public hidl_death_recipient {
  sp<IQtiRadioResponse> mResponseCb;
  sp<IQtiRadioIndication> mIndicationCb;
  RWLock mCallbackLock;
  qcril_instance_id_e_type mInstanceId;

  sp<IQtiRadioResponse> getResponseCallback();
  sp<IQtiRadioIndication> getIndicationCallback();

  void clearCallbacks();

  // Function from hidl_death_recipient
  void serviceDied(uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &);

  // Functions from IQtiRadio
  Return<void> setCallback(const sp<IQtiRadioResponse> &responseCallback,
                           const sp<IQtiRadioIndication> &indicationCallback);
  Return<void> getAtr(int32_t serial);

public:
  QtiRadioImpl(qcril_instance_id_e_type instaceId);
  void getAtrResponse(RIL_Token token, RIL_Errno error, char *buf, int bufLen);
  void sendQtiIndication();
};

} // namespace implementation
} // namespace V1_0
} // namespace qtiradio
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
#endif // VENDOR_QTI_HARDWARE_QTIRADIO_V1_0_QTIRADIO_H
