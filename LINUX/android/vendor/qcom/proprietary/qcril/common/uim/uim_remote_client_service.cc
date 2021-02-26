/******************************************************************************
@file    uim_remote_client_service.c
@brief   qcril uim remote client service

DESCRIPTION
Implements the server side of the IUimRemoteServiceClient interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/

#include "uim_remote_client_service.h"

extern "C" {
  #include "qcril_log.h"
  #include "qcril_uim_sap.h"
}

using namespace vendor::qti::hardware::radio::uim_remote_client::V1_0::implementation;
using ::android::hardware::Status;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_client {
namespace V1_0 {
namespace implementation {

static sp<UimRemoteClientImpl> uimRemoteClient = NULL;


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::UimRemoteClientImpl

===========================================================================*/
UimRemoteClientImpl::UimRemoteClientImpl()
{
  mResponseCb = NULL;
  mIndicationCb = NULL;
} /* UimRemoteClientImpl::UimRemoteClientImpl() */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::setCallback

===========================================================================*/
Return<void> UimRemoteClientImpl::setCallback
(
  const sp<IUimRemoteServiceClientResponse>& responseCallback,
  const sp<IUimRemoteServiceClientIndication>& indicationCallback
)
{
  QCRIL_LOG_INFO("UimRemoteClientImpl::setCallback");
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr)
  {
    mResponseCb->unlinkToDeath(this);
  }
  mIndicationCb = indicationCallback;
  mResponseCb = responseCallback;
  if (mResponseCb != nullptr)
  {
    mResponseCb->linkToDeath(this, 0);
  }
  return Status::ok();
} /* UimRemoteClientImpl::setCallback */

sp<IUimRemoteServiceClientResponse> UimRemoteClientImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<IUimRemoteServiceClientIndication> UimRemoteClientImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}
/*===========================================================================

FUNCTION:  UimRemoteClientImpl::serviceDied

===========================================================================*/
void UimRemoteClientImpl::serviceDied
(
 uint64_t,
 const ::android::wp<::android::hidl::base::V1_0::IBase> &
)
{
  QCRIL_LOG_INFO("UimRemoteClientImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
} /* UimRemoteClientImpl::serviceDied */

/*===========================================================================

FUNCTION:  UimRemoteClientImpl::clearCallbacks

===========================================================================*/
void UimRemoteClientImpl::clearCallbacks()
{
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr)
  {
    mResponseCb = NULL;
  }
  if (mIndicationCb != nullptr)
  {
    mIndicationCb = NULL;
  }
} /* UimRemoteClientImpl::resetResponseCallback() */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::setInstanceId

===========================================================================*/
void UimRemoteClientImpl::setInstanceId
(
  int instanceId
)
{
  mInstanceId = instanceId;
} /* UimRemoteClientImpl::setInstanceId */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientSendApduInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientSendApduInd
(
  uint32_t  apduLen,
  uint8_t * apduPtr
)
{
  hidl_vec<uint8_t> data;
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();

  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientSendApduInd indicationCb is null");
    return;
  }

  QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientSendApduInd apduLen=%d", apduLen);
  if (apduLen == 0 || apduPtr == NULL)
  {
    return;
  }

  data.setToExternal(apduPtr, apduLen);
  Return<void> ret = indCb->UimRemoteServiceClientApduInd(data);
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientSendApduInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientConnectInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientConnectInd()
{
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();
  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientConnectInd indicationCb is null");
    return;
  }
  Return<void> ret = indCb->UimRemoteServiceClientConnectInd();
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientConnectInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientDisconnectInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientDisconnectInd()
{
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();
  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientDisconnectInd indicationCb is null");
    return;
  }
  Return<void> ret = indCb->UimRemoteServiceClientDisconnectInd();
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientDisconnectInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientPowerUpInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientPowerUpInd
(
  bool                                hasTimeOut,
  int32_t                             timeOut,
  bool                                hasVoltageClass,
  qcril_uim_remote_voltage_class_type voltageClass
)
{
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();
  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientPowerUpInd indicationCb is null");
    return;
  }
  Return<void> ret = indCb->UimRemoteServiceClientPowerUpInd(hasTimeOut,
                                          timeOut,
                                          hasVoltageClass,
                                          (UimRemoteClientVoltageClass)voltageClass);
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientPowerUpInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientPowerDownInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientPowerDownInd
(
  bool                             hasMode,
  qcril_uim_remote_power_down_mode mode
)
{
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();
  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientPowerDownInd indicationCb is null");
    return;
  }
  Return<void> ret = indCb->UimRemoteServiceClientPowerDownInd(hasMode,
      (UimRemoteClientPowerDownMode)mode);
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientPowerDownInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientResetInd

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientResetInd()
{
  sp<IUimRemoteServiceClientIndication> indCb = getIndicationCallback();
  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientResetInd indicationCb is null");
    return;
  }
  Return<void> ret = indCb->UimRemoteServiceClientResetInd();
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientResetInd */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientEventResponse

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientEventResponse
(
  int32_t                           token,
  qcril_uim_remote_event_resp_type  eventResp
)
{
  sp<IUimRemoteServiceClientResponse> respCb = getResponseCallback();
  if (respCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientEventResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientEventResponse token=%d eventResp=%d", token, eventResp);
  Return<void> ret = respCb->UimRemoteServiceClientEventResp(token, (UimRemoteClientEventRsp)eventResp);
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientEventResponse */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::uimRemoteClientApduResponse

===========================================================================*/
void UimRemoteClientImpl::uimRemoteClientApduResponse
(
  int32_t                           token,
  qcril_uim_remote_apdu_resp_type   apduResp
)
{
  sp<IUimRemoteServiceClientResponse> respCb = getResponseCallback();
  if (respCb == NULL)
  {
    QCRIL_LOG_ERROR("UimRemoteClientImpl::uimRemoteClientApduResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientApduResponse token=%d eventResp=%d", token, apduResp);
  Return<void> ret = respCb->UimRemoteServiceClientApduResp(token, (UimRemoteClientApduRsp)apduResp);
  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteClientImpl::uimRemoteClientApduResponse */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::UimRemoteServiceClientEventReq

===========================================================================*/
Return<void> UimRemoteClientImpl::UimRemoteServiceClientEventReq
(
  int32_t token,
  const UimRemoteEventReqType& eventReq
)
{
  qcril_uim_remote_event_req_type  event_req;

  event_req.event_type                  = (qcril_uim_remote_event_type)eventReq.event;
  event_req.atr_len                     = eventReq.atr.size();
  event_req.atr                         = eventReq.atr.data();
  event_req.has_wakeup_support          = eventReq.has_wakeupSupport;
  event_req.wakeup_support              = eventReq.wakeupSupport;
  event_req.has_error_code              = eventReq.has_errorCode;
  event_req.error_code                  = (qcril_uim_remote_error_cause_type)eventReq.errorCode;
  event_req.has_transport               = eventReq.has_transport;
  event_req.transport                   = (qcril_uim_remote_transport_type)eventReq.transport;
  event_req.has_usage                   = eventReq.has_usage;
  event_req.usage                       = (qcril_uim_remote_usage_type)eventReq.usage;
  event_req.has_apdu_timeout            = eventReq.has_apdu_timeout;
  event_req.apdu_timeout                = eventReq.apduTimeout;
  event_req.has_disable_all_polling     = eventReq.has_disable_all_polling;
  event_req.disable_all_polling         = eventReq.disableAllPolling;
  event_req.has_poll_timer              = eventReq.has_poll_timer;
  event_req.poll_timer                  = eventReq.pollTimer;

  qcril_uim_remote_client_request_event(token, event_req);

  return Status::ok();
} /* UimRemoteClientImpl::UimRemoteServiceClientEventReq */


/*===========================================================================

FUNCTION:  UimRemoteClientImpl::UimRemoteServiceClientApduReq

===========================================================================*/
Return<void> UimRemoteClientImpl::UimRemoteServiceClientApduReq
(
  int32_t token,
  UimRemoteClientApduStatus apduStatus,
  const hidl_vec<uint8_t>& apduResponse
)
{
  qcril_uim_remote_client_request_apdu(token,
  (qcril_uim_remote_apdu_status)apduStatus,
  apduResponse.size(),
  apduResponse.data());
  return Status::ok();
} /* UimRemoteClientImpl::UimRemoteServiceClientApduReq */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_register_service

===========================================================================*/
extern "C" uint8_t qcril_uim_remote_client_register_service
(
int instanceId
)
{
  android::status_t ret = android::OK;
  uimRemoteClient = new UimRemoteClientImpl();
  uimRemoteClient->setInstanceId(instanceId);
  std::string serviceName = "uimRemoteClient";
  ret = uimRemoteClient->registerAsService(serviceName + std::to_string(instanceId));
  QCRIL_LOG_INFO("qcril_uim_remote_client_register_service instanceId=%d status=%d", instanceId, ret);

  return ret == android::OK;
} /* qcril_uim_remote_client_register_service */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_event_rsp

===========================================================================*/
extern "C" void qcril_uim_remote_client_event_rsp
(
int32_t                           token,
qcril_uim_remote_event_resp_type  event_rsp
)
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_event_rsp uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientEventResponse(token, event_rsp);
} /* qcril_uim_remote_client_event_rsp */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_apdu_rsp

===========================================================================*/
extern "C" void qcril_uim_remote_client_apdu_rsp
(
  int32_t                          token,
  qcril_uim_remote_apdu_resp_type  apdu_rsp
)
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_apdu_rsp uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientApduResponse(token, apdu_rsp);
} /* qcril_uim_remote_client_apdu_rsp */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_send_apdu_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_send_apdu_ind
(
  uint32_t   apdu_len,
  uint8_t  * apdu_ptr
)
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_send_apdu_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientSendApduInd(apdu_len, apdu_ptr);
} /* qcril_uim_remote_client_send_apdu_ind */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_connect_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_connect_ind()
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_connect_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientConnectInd();
} /* qcril_uim_remote_client_connect_ind */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_disconnect_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_disconnect_ind()
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_disconnect_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientDisconnectInd();
} /* qcril_uim_remote_client_disconnect_ind */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_power_up_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_power_up_ind
(
  bool                                has_time_out,
  int32_t                             time_out,
  bool                                has_voltage_class,
  qcril_uim_remote_voltage_class_type voltage_class
)
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_power_up_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientPowerUpInd(has_time_out,
                                             time_out,
                                             has_voltage_class,
                                             voltage_class);
} /* qcril_uim_remote_client_power_up_ind */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_power_down_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_power_down_ind
(
  bool                             has_mode,
  qcril_uim_remote_power_down_mode mode
)
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_power_down_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientPowerDownInd(has_mode, mode);
} /* qcril_uim_remote_client_power_down_ind */


/*===========================================================================

FUNCTION:  qcril_uim_remote_client_reset_ind

===========================================================================*/
extern "C" void qcril_uim_remote_client_reset_ind()
{
  if (uimRemoteClient == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_reset_ind uim remote client service is null");
    return;
  }
  uimRemoteClient->uimRemoteClientResetInd();
} /* qcril_uim_remote_client_reset_ind */


} // namespace implementation
} // namespace V1_0
} // namespace uim_remote_client
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
