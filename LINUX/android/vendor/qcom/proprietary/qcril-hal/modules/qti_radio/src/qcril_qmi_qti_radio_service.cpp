/******************************************************************************
  @ file    qcril_qmi_qti_radio_service.cpp
  @brief   qcril qmi - qtiRadioService_service

  DESCRIPTION
    Implements the server side of the IQtiqtiRadioService interface. Handles RIL
    requests and responses and indications to be received and sent to client
    respectively

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define TAG "RILQ"

#include "qcril_qmi_qti_radio_service.h"
#include "qcril_log.h"
#include "modules/qti_radio/UimGetAtrMsgCallback.h"
#include "modules/uim/UimGetAtrRequestMsg.h"
#include "qcril_reqlist.h"

using namespace vendor::qti::hardware::radio::qtiradio::V1_0::implementation;
using ::android::hardware::Void;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace qtiradio {
namespace V1_0 {
namespace implementation {

static sp<QtiRadioImpl> qtiRadioService = nullptr;

QtiRadioImpl::QtiRadioImpl(qcril_instance_id_e_type instanceId) {
  mResponseCb = nullptr;
  mIndicationCb = nullptr;
  mInstanceId = instanceId;
}

/*
 * Service Died callback.
 */
void QtiRadioImpl::serviceDied(uint64_t,
                               const ::android::wp<::android::hidl::base::V1_0::IBase> &) {
  QCRIL_LOG_DEBUG("QtiRadioImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
}

/*
 * Resets indication callback upon client's death
 */
void QtiRadioImpl::clearCallbacks() {
  RWLock::AutoWLock lock(mCallbackLock);
  if (mIndicationCb != nullptr) {
    mIndicationCb = nullptr;
  }
  if (mResponseCb != nullptr) {
    mResponseCb = nullptr;
  }
}

//===========================================================================
// QtiRadioImpl::setCallback
//
//===========================================================================
//===========================================================================
/*!
    @brief
    Registers the callbacks for IQtiRadioResponse and IQtiRadioIndication
    passed by the client as a parameter

    @return
    None
*/
/*=========================================================================*/
Return<void> QtiRadioImpl::setCallback(const sp<IQtiRadioResponse> &responseCallback,
                                       const sp<IQtiRadioIndication> &indicationCallback) {
  QCRIL_LOG_DEBUG("QtiRadioImpl::setCallback");
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr) {
    mResponseCb->unlinkToDeath(this);
  }
  mIndicationCb = indicationCallback;
  mResponseCb = responseCallback;
  if (mResponseCb != nullptr) {
    mResponseCb->linkToDeath(this, 0);
  }
  return Void();
}

sp<IQtiRadioResponse> QtiRadioImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<IQtiRadioIndication> QtiRadioImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}

//===========================================================================
// QtiRadioImpl::getAtr
//
//===========================================================================
//===========================================================================
/*!
    @brief
    Processes the getAtr request sent by client

    @return
    None
*/
/*=========================================================================*/
Return<void> QtiRadioImpl::getAtr(int32_t serial) {
  QCRIL_LOG_ERROR("QtiRadioImpl::getAtr serial=%d", serial);
  RIL_Token token = qcril_qmi_qti_convert_radio_token_to_ril_token(serial /*request token*/);
  UimGetAtrMsgCallback cb("get-atr");
  std::shared_ptr<UimGetAtrRequestMsg> req = std::make_shared<UimGetAtrRequestMsg>(token, &cb);
  if (req) {
    req->dispatch();
  }
  return Void();
}

void QtiRadioImpl::getAtrResponse(RIL_Token token, RIL_Errno error, char *buf, int bufLen) {
  qcril_reqlist_free(mInstanceId, token);
  sp<IQtiRadioResponse> respCb = getResponseCallback();
  if (respCb != nullptr) {
    QtiRadioResponseInfo responseInfo;
    hidl_string data;
    QCRIL_LOG_ERROR("QtiRadioImpl::getAtrResponse ");
    int serial = qcril_qmi_qti_free_and_convert_ril_token_to_radio_token(token);
    QCRIL_LOG_ERROR("QtiRadioImpl::getAtrResponse serial=%d", serial);
    responseInfo.serial = serial;
    responseInfo.type = QtiRadioResponseType::SOLICITED;
    responseInfo.error = (QtiRadioError)error;
    if (buf != nullptr) {
      data.setToExternal(buf, bufLen);
    }
    Return<void> ret = respCb->getAtrResponse(responseInfo, data);
    if (!ret.isOk()) {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  } else {
    QCRIL_LOG_ERROR("QtiRadioImpl::getAtrResponse respCb NULL");
  }
}

void QtiRadioImpl::sendQtiIndication() {}

/*
 * Register qtiRadioService service with Service Manager
 */
void qcril_qmi_qti_radio_service_init(int instanceId) {
  android::status_t ret = android::OK;
  QCRIL_LOG_ERROR("qcril_qmi_qti_radio_service_init");
  qtiRadioService = new QtiRadioImpl((qcril_instance_id_e_type)instanceId);
  std::string serviceName = "slot";
  QCRIL_LOG_INFO("qcril_qmi_qti_radio_service_init adding for slot %d", instanceId);
  ret = qtiRadioService->registerAsService(serviceName + std::to_string(instanceId + 1));
  QCRIL_LOG_INFO("qtiRadioServiceRegisterService instanceId=%d status=%d", instanceId, ret);
}

void qtiGetAtrResponse(RIL_Token token, RIL_Errno error, char *buf, int bufLen) {
  if (qtiRadioService != nullptr) {
    qtiRadioService->getAtrResponse(token, error, buf, bufLen);
  }
}

RIL_Token qcril_qmi_qti_convert_radio_token_to_ril_token(uint32_t oem_token) {
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (nullptr != ret) {
    uint32_t *tmp = (uint32_t *)ret;
    *tmp = oem_token ^ 0xc0000000;
  }
  return ret;
} // qcril_qmi_oem_convert_oem_token_to_ril_token

uint32_t qcril_qmi_qti_free_and_convert_ril_token_to_radio_token(RIL_Token ril_token) {
  uint32_t ret = 0xFFFFFFFF;
  if (ril_token) {
    ret = (*((uint32_t *)ril_token)) ^ 0xc0000000;
    QCRIL_LOG_INFO("oem token: %d", ret);
    qcril_free((void *)ril_token);
  } else {
    QCRIL_LOG_ERROR("ril_token is NULL");
  }

  return ret;
} // qcril_qmi_oem_free_and_convert_ril_token_to_oem_token

} // namespace implementation
} // namespace V1_0
} // namespace qtiradio
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
