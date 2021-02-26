/******************************************************************************
  @file    qcril_qmi_oemhook_service.cc
  @brief   qcril qmi - oemhook_service

  DESCRIPTION
    Implements the server side of the IQtiOemHook interface. Handles RIL
    requests and responses and indications to be received and sent to client
    respectively

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "qcril_qmi_oemhook_service.h"

extern "C" {
    #include "qcril_log.h"
}

using namespace vendor::qti::hardware::radio::qcrilhook::V1_0::implementation;
using ::android::hardware::Void;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qcrilhook {
namespace V1_0 {
namespace implementation {

static sp<OemHookImpl> oemHook = NULL;

OemHookImpl::OemHookImpl() {
    mResponseCb = NULL;
    mIndicationCb = NULL;
}

/*
 * Service Died callback.
 */
void OemHookImpl::serviceDied(
    uint64_t, const ::android::wp<::android::hidl::base::V1_0::IBase> &) {
  QCRIL_LOG_DEBUG("OemHookImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

/*
 * Resets indication callback upon client's death
 */
void OemHookImpl::clearCallbacks() {
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    mResponseCb = nullptr;
  }
  if (mIndicationCb != nullptr) {
    mIndicationCb = nullptr;
  }
}

//===========================================================================
// oem_hook_impl::setCallback
//
//===========================================================================
//===========================================================================
/*!
    @brief
    Registers the callbacks for IQtiOemHookResponse and IQtiOemHookIndication
    passed by the client as a parameter

    @return
    None
*/
/*=========================================================================*/
Return<void> OemHookImpl::setCallback(const sp<IQtiOemHookResponse> &responseCallback,
                                      const sp<IQtiOemHookIndication> &indicationCallback) {
  QCRIL_LOG_DEBUG("OemHookImpl::setCallback");
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    mResponseCb->unlinkToDeath(this);
  }
  mResponseCb = responseCallback;
  mIndicationCb = indicationCallback;
  mResponseCb->linkToDeath(this, 0);

  return Void();
}

sp<IQtiOemHookResponse> OemHookImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<IQtiOemHookIndication> OemHookImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}

//===========================================================================
// oem_hook_impl::oemHookRawRequest
//
//===========================================================================
//===========================================================================
/*!
    @brief
    Processes the oemhook request sent by client

    @return
    None
*/
/*=========================================================================*/
Return<void> OemHookImpl::oemHookRawRequest(int32_t serial, const hidl_vec<uint8_t>& data) {
    size_t dataLen = data.size();
    //Type casting it as qcril_request_params_type takes only void * and not const
    unsigned char *buf = const_cast<unsigned char*>(data.data());
    QCRIL_LOG_INFO("OemHookImpl::oemHookRawRequest serial=%d length=%d",serial, dataLen);
    processIncomingOemhookMessage(mInstanceId, serial, buf, dataLen);
    return Void();
}

/*
 * Sends oemHook indication/unsol to telephony via callback
 */
void OemHookImpl::sendIndication(unsigned char* buf, size_t bufLen) {
    QCRIL_LOG_INFO("OemHookImpl::sendIndication length=%d", bufLen);
    sp<IQtiOemHookIndication> indCb = getIndicationCallback();
    if (indCb == NULL) {
      QCRIL_LOG_ERROR("OemHookImpl::sendIndication indicationCb is null");
      return;
    }
    hidl_vec<uint8_t> data;
    data.setToExternal((uint8_t*)buf, bufLen, true); /* Send true to allow hidl_vec to delete memory*/
    Return<void> ret = indCb->oemHookRawIndication(data);
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
    }
}

/*
 * Sends oemHook response to telephony via callback
 */
void OemHookImpl::sendResponse(int32_t serial, RIL_Errno error, unsigned char* buf, size_t bufLen) {
    sp<IQtiOemHookResponse> respCb = getResponseCallback();
    if (respCb == NULL) {
      QCRIL_LOG_ERROR("OemHookImpl::sendResponse responseCb is null");
      return;
    }
    hidl_vec<uint8_t> data;
    data.setToExternal((uint8_t*)buf, bufLen, true); /* Send true to allow hidl_vec to delete memory*/
    QCRIL_LOG_INFO("OemHookImpl::sendResponse serial=%d error=%d length=%d", serial, error, bufLen);
    Return<void> ret = respCb->oemHookRawResponse(serial, (RadioError)error, data);
    if (!ret.isOk()) {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
}

/*
 * Sets instance id
 */
void OemHookImpl::setInstanceId(qcril_instance_id_e_type oemSocketInstanceId) {
    mInstanceId = oemSocketInstanceId;
}

/*
 * Register oemhook service with Service Manager
 */
uint8_t oemHookRegisterService(qcril_instance_id_e_type oemSocketInstanceId) {
    android::status_t ret = android::OK;
    if (oemHook == NULL) {
        oemHook = new OemHookImpl();
        oemHook->setInstanceId(oemSocketInstanceId);
        std::string serviceName = "oemhook";
        ret = oemHook->registerAsService(serviceName + std::to_string(oemSocketInstanceId));
        QCRIL_LOG_INFO("OemHookRegisterService instanceId=%d status=%d", oemSocketInstanceId, ret);
    }
    return ret == android::OK;
}

void sendOemhookIndication(qcril_instance_id_e_type oemSocketInstanceId,
    unsigned char* buf, size_t bufLen) {
    if (oemHook != NULL) {
        oemHook->sendIndication(buf, bufLen);
    } else {
        QCRIL_LOG_ERROR("OemHookImpl::sendOemhookIndication:OemHook service is null instanceId=%d",
            oemSocketInstanceId);
    }
}

void sendOemhookResponse(qcril_instance_id_e_type oemSocketInstanceId, int serial,
    RIL_Errno error, unsigned char* buf, size_t bufLen) {
    if (oemHook != NULL) {
        oemHook->sendResponse(serial, error, buf, bufLen);
    } else {
        QCRIL_LOG_ERROR("OemHookImpl::sendOemhookResponse:OemHook service is null instanceId=%d",
            oemSocketInstanceId);
    }
}

} // namespace implementation
} // namespace V1_0
} // namespace qcrilhook
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
