/******************************************************************************
@file    uim_service.cc
@brief   qcril uim service

DESCRIPTION
Implements the server side of the IUim interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/

#include "uim_service.h"

extern "C" {
  #include "qcril_log.h"
  #include "qcril_uim_security.h"
}

using namespace vendor::qti::hardware::radio::uim::V1_0::implementation;
using namespace vendor::qti::hardware::radio::uim;
using namespace vendor::qti::hardware::radio::uim::V1_0;
using namespace vendor::qti::hardware::radio::uim::V1_1;
using ::android::hardware::Status;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim {
namespace V1_0 {
namespace implementation {

static sp<UimImpl> uim = NULL;


/*===========================================================================

FUNCTION:  UimImpl::UimImpl

===========================================================================*/
UimImpl::UimImpl()
{
  mResponseCb = NULL;
  mIndicationCb = NULL;
} /* UimImpl::UimImpl() */


/*===========================================================================

FUNCTION:  UimImpl::setCallback

===========================================================================*/
Return<void> UimImpl::setCallback
(
  const sp<IUimResponse>& responseCallback,
  const sp<IUimIndication>& indicationCallback
)
{
  QCRIL_LOG_INFO("UimImpl::setCallback");
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

  mResponseCbV1_1 =
    V1_1::IUimResponse::castFrom(responseCallback).withDefault(nullptr);

  return Status::ok();
} /* UimImpl::setCallback */

sp<IUimResponse> UimImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<V1_1::IUimResponse> UimImpl::getResponseCallbackV1_1() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCbV1_1;
}

sp<IUimIndication> UimImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}

/*===========================================================================

FUNCTION:  UimImpl::serviceDied

===========================================================================*/
void UimImpl::serviceDied
(
 uint64_t,
 const ::android::wp<::android::hidl::base::V1_0::IBase> &
)
{
  QCRIL_LOG_INFO("UimImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
} /* UimImpl::serviceDied */

/*===========================================================================

FUNCTION:  UimImpl::clearCallbacks

===========================================================================*/
void UimImpl::clearCallbacks()
{
  QCRIL_LOG_ERROR("UimImpl::clearCallbacks");
  RWLock::AutoWLock lock(mCallbackLock);
  mResponseCb = NULL;
  mIndicationCb = NULL;
  mResponseCbV1_1 = NULL;
} /* UimImpl::clearCallbacks() */


/*===========================================================================

FUNCTION:  UimImpl::setInstanceId

===========================================================================*/
void UimImpl::setInstanceId
(
  int instanceId
)
{
  mInstanceId = instanceId;
} /* UimImpl::setInstanceId */


/*===========================================================================

FUNCTION:  UimImpl::uimRemoteSimlockResponse

===========================================================================*/
void UimImpl::uimRemoteSimlockResponse
(
  int                                   token,
  UimRemoteSimlockResponseType          response,
  V1_1::UimRemoteSimlockOperationType   op_type,
  uint8_t                             * rsp_ptr,
  uint32_t                              rsp_len,
  UimRemoteSimlockVersion               version,
  UimRemoteSimlockStatus                status,
  int                                   timer_value
)
{
  hidl_vec<uint8_t>       simlock_data = {};
  sp<IUimResponse>        respCb       = getResponseCallback();
  sp<V1_1::IUimResponse>  respCbV1_1   = getResponseCallbackV1_1();
  Return<void>            ret          = Status::ok();

  if (respCb == NULL && respCbV1_1 == NULL)
  {
    QCRIL_LOG_ERROR("uimRemoteSimlockResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("uimRemoteSimlockResponse token=%d", token);

  simlock_data.setToExternal(rsp_ptr, rsp_len);

  if (respCbV1_1 != NULL)
  {
    switch(op_type)
    {
      case V1_1::UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_UNLOCK_TIMER_START:
      case V1_1::UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_UNLOCK_TIMER_STOP:
        ret = respCbV1_1->uimRemoteSimlockTimerResponse_1_1(token, response, timer_value);
        break;
      default:
        ret = respCbV1_1->uimRemoteSimlockResponse_1_1(token,
                                                       response,
                                                       op_type,
                                                       simlock_data,
                                                       version,
                                                       status);
    }
  }
  else if (respCb != NULL)
  {
    ret = respCb->UimRemoteSimlockResponse(token,
                                           response,
                                           (UimRemoteSimlockOperationType)op_type,
                                           simlock_data,
                                           version,
                                           status);
  }

  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
}


/*===========================================================================

FUNCTION:  UimImpl::UimRemoteSimlockRequest

===========================================================================*/
Return<void> UimImpl::UimRemoteSimlockRequest
(
  int32_t token,
  UimRemoteSimlockOperationType simlockOp,
  const hidl_vec<uint8_t>& simlockData
)
{
  V1_1::UimRemoteSimlockOperationType  simlock_op =
                             (V1_1::UimRemoteSimlockOperationType)simlockOp;
  switch(simlockOp)
  {
    case UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_GENERATE_ENCRYPTED_KEY:
    case UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_PROCESS_SIMLOCK_DATA:
    case UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_GENERATE_HMAC:
    case UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_GET_MAX_SUPPORTED_VERSION:
    case UimRemoteSimlockOperationType::UIM_REMOTE_SIMLOCK_GET_STATUS:
      return uimRemoteSimlockRequest_1_1(token, simlock_op, simlockData);
    default:
      return Status::fromExceptionCode(Status::EX_ILLEGAL_ARGUMENT);
  }
} /* UimImpl::UimRemoteSimlockRequest */


/*===========================================================================

FUNCTION:  UimImpl::UimRemoteSimlockRequest

===========================================================================*/
Return<void> UimImpl::uimRemoteSimlockRequest_1_1
(
  int32_t token,
  V1_1::UimRemoteSimlockOperationType simlockOp,
  const hidl_vec<uint8_t>& simlockData
)
{
  int  * tokenPtr  = new int(token);

  qcril_uim_request_remote_sim_lock_unlock(tokenPtr,
  (qcril_uim_remote_simlock_operation_type)simlockOp,
                                           simlockData.data(),
                                           simlockData.size());

  return Status::ok();
} /* UimImpl::uimRemoteSimlockRequest_1_1 */


/*===========================================================================

FUNCTION:  qcril_uim_register_service

===========================================================================*/
extern "C" uint8_t qcril_uim_register_service
(
  int instanceId
)
{
  android::status_t ret = android::OK;
  uim = new UimImpl();
  uim->setInstanceId(instanceId);
  std::string serviceName = "Uim";
  ret = uim->registerAsService(serviceName + std::to_string(instanceId));
  QCRIL_LOG_INFO("qcril_uim_register_service instanceId=%d status=%d", instanceId, ret);

  return ret == android::OK;
} /* qcril_uim_register_service */


/*===========================================================================

FUNCTION:  qcril_uim_remote_simlock_response

===========================================================================*/
extern "C" void qcril_uim_remote_simlock_response
(
  void                                    * token_ptr,
  qcril_uim_remote_simlock_response_type    response,
  qcril_uim_remote_simlock_operation_type   op_type,
  uint8                                   * simlock_rsp_ptr,
  uint32                                    simlock_rsp_len,
  qcril_uim_remote_simlock_version          version,
  qcril_uim_remote_simlock_status           status,
  int                                       timer_value
)
{
  UimRemoteSimlockVersion    simlock_version;
  UimRemoteSimlockStatus     simlock_status;

  if (token_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Null token ptr");
    return;
  }

  simlock_version.majorVersion = version.majorVersion;
  simlock_version.minorVersion = version.minorVersion;

  simlock_status.status = (UimRemoteSimlockStatusType)status.status;
  simlock_status.unlockTime = status.unlockTime;

  uim->uimRemoteSimlockResponse(*((int *)token_ptr),
         (UimRemoteSimlockResponseType)response,
         (V1_1::UimRemoteSimlockOperationType)op_type,
                           simlock_rsp_ptr,
                           simlock_rsp_len,
                           simlock_version,
                           simlock_status,
                           timer_value);
} /* qcril_uim_remote_simlock_response */

} // namespace implementation
} // namespace V1_0
} // namespace uim
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
