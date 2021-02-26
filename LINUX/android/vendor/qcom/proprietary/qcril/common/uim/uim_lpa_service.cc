/******************************************************************************
@file    uim_lpa_service.c
@brief   qcril uim lpa service

DESCRIPTION
Implements the server side of the IUimLpa interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/

#include <vendor/qti/hardware/radio/lpa/1.0/types.h>
#include "uim_lpa_service.h"

extern "C" {
  #include "qcril_log.h"
}

using namespace vendor::qti::hardware::radio::lpa::V1_0::implementation;
using ::android::hardware::Status;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace lpa {
namespace V1_0 {
namespace implementation {

static sp<UimLpaImpl> uimLpa = NULL;


/*===========================================================================

FUNCTION:  UimLpaImpl::UimLpaImpl

===========================================================================*/
UimLpaImpl::UimLpaImpl()
{
  mResponseCb = NULL;
  mIndicationCb = NULL;
} /* UimLpaImpl::UimLpaImpl() */


/*===========================================================================

FUNCTION:  UimLpaImpl::setCallback

===========================================================================*/
Return<void> UimLpaImpl::setCallback
(
  const sp<IUimLpaResponse>& responseCallback,
  const sp<IUimLpaIndication>& indicationCallback
)
{
  QCRIL_LOG_INFO("UimLpaImpl::setCallback");
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
} /* UimLpaImpl::setCallback */

sp<IUimLpaResponse> UimLpaImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<IUimLpaIndication> UimLpaImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}

/*===========================================================================

FUNCTION:  UimLpaImpl::serviceDied

===========================================================================*/
void UimLpaImpl::serviceDied
(
 uint64_t,
 const ::android::wp<::android::hidl::base::V1_0::IBase> &
)
{
  QCRIL_LOG_INFO("UimLpaImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
} /* UimLpaImpl::serviceDied */

/*===========================================================================

FUNCTION:  UimLpaImpl::clearCallbacks

===========================================================================*/
void UimLpaImpl::clearCallbacks()
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
} /* UimLpaImpl::clearCallbacks() */

/*===========================================================================

FUNCTION:  UimLpaImpl::setInstanceId

===========================================================================*/
void UimLpaImpl::setInstanceId
(
  int instanceId
)
{
  mInstanceId = instanceId;
} /* UimLpaImpl::setInstanceId */


/*===========================================================================

FUNCTION:  UimLpaImpl::uimLpaHttpTxnIndication

===========================================================================*/
void UimLpaImpl::uimLpaHttpTxnIndication
(
  qcril_uim_http_transaction_ind_type  * txnInd
)
{
  UimLpaHttpTransactionInd           httpTxnInd;
  uint8_t                            h_count = 0;
  UimLpaHttpCustomHeader           * headers = NULL;

  sp<IUimLpaIndication> indCb = getIndicationCallback();
  if (indCb == NULL || txnInd == NULL)
  {
    QCRIL_LOG_ERROR("uimLpaHttpTxnIndication indicationCb is null or txnInd NULL");
    return;
  }

  memset(&httpTxnInd, 0x00, sizeof(httpTxnInd));

  httpTxnInd.tokenId = txnInd->tokenId;
  httpTxnInd.payload.setToExternal(txnInd->payload, txnInd->payload_len);

  if (txnInd->contentType != NULL)
  {
    httpTxnInd.contentType = txnInd->contentType;
  }
  else
  {
    httpTxnInd.contentType.setToExternal(NULL, 0);
  }

  if (txnInd->no_of_headers != 0)
  {
    headers = new UimLpaHttpCustomHeader[txnInd->no_of_headers];

    if (headers == NULL)
    {
      QCRIL_LOG_ERROR("Allocation failed for headers");
    }

    if (txnInd->customHeaders != NULL)
    {
      for (h_count = 0 ; h_count < txnInd->no_of_headers; h_count++)
      {
        headers[h_count].headerName = txnInd->customHeaders[h_count].headerName;
        headers[h_count].headerValue = txnInd->customHeaders[h_count].headerValue;
      }
    }
  }
  httpTxnInd.customHeaders.setToExternal(headers, txnInd->no_of_headers);
  httpTxnInd.url = txnInd->url;

  Return<void> ret = indCb->UimLpaHttpTxnIndication(httpTxnInd);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }

  if (headers != NULL)
  {
    delete[] headers;
    headers = NULL;
  }
} /* UimLpaImpl::uimLpaHttpTxnIndication */


/*===========================================================================

FUNCTION:  UimLpaImpl::uimLpaAddProfileProgressInd

===========================================================================*/
void UimLpaImpl::uimLpaAddProfileProgressInd
(
  qcril_uim_lpa_add_profile_progress_ind_type * progInd
)
{
  UimLpaAddProfileProgressInd  progress;

  sp<IUimLpaIndication> indCb = getIndicationCallback();
  if (indCb == NULL || progInd == NULL)
  {
    QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd indicationCb is null or progInd");
    return;
  }

  memset(&progress, 0x00, sizeof(progress));

  progress.status              = (UimLpaAddProfileStatus)progInd->status;
  progress.cause               = (UimLpaAddProfileFailureCause)progInd->cause;
  progress.progress            = progInd->progress;
  progress.policyMask          = (UimLpaProfilePolicyMask)progInd->policyMask;
  progress.userConsentRequired = progInd->userConsentRequired;

  Return<void> ret = indCb->UimLpaAddProfileProgressIndication(progress);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimLpaImpl::uimLpaAddProfileProgressInd */


/*===========================================================================

FUNCTION:  UimLpaImpl::uimLpaUserResponse

===========================================================================*/
void UimLpaImpl::uimLpaUserResponse
(
  int32_t                         token,
  qcril_uim_lpa_user_resp_type  * user_resp
)
{
  UimLpaUserResp       userResp;
  UimLpaProfileInfo  * profiles = NULL;
  uint8_t              p_count  = 0;

  sp<IUimLpaResponse> respCb = getResponseCallback();
  if (respCb == NULL || user_resp == NULL)
  {
    QCRIL_LOG_ERROR("UimLpaImpl::uimLpaUserResponse responseCb is null or userResp NULL");
    return;
  }

  memset(&userResp, 0x00, sizeof(userResp));

  QCRIL_LOG_INFO("UimRemoteClientImpl::uimRemoteClientEventResponse token=%d event=%d", token, user_resp->event);

  userResp.event = (UimLpaUserEventId)user_resp->event;
  userResp.result = (UimLpaResult)user_resp->result;
  userResp.eid.setToExternal(user_resp->eid, user_resp->eid_len);

  if (user_resp->no_of_profiles != 0 && user_resp->profiles != NULL)
  {
    profiles = new UimLpaProfileInfo[user_resp->no_of_profiles];

    if (profiles == NULL)
    {
      QCRIL_LOG_ERROR("Allocation failed");
      return;
    }

    for (p_count = 0; p_count < user_resp->no_of_profiles; p_count++)
    {
      profiles[p_count].state = (UimLpaProfileState)user_resp->profiles[p_count].state;
      profiles[p_count].iccid.setToExternal(
                 user_resp->profiles[p_count].iccid,
                 user_resp->profiles[p_count].iccid_len);

      profiles[p_count].profileName = user_resp->profiles[p_count].profileName;
      profiles[p_count].nickName = user_resp->profiles[p_count].nickName;
      profiles[p_count].spName = user_resp->profiles[p_count].spName;
      profiles[p_count].iconType =
         (UimLpaIconType)user_resp->profiles[p_count].iconType;
      profiles[p_count].icon.setToExternal(
         user_resp->profiles[p_count].icon,
         user_resp->profiles[p_count].icon_len);
      profiles[p_count].profileClass =
        (UimLpaProfileClassType)user_resp->profiles[p_count].profileClass;
      profiles[p_count].profilePolicy =
        (UimLpaProfilePolicyMask)user_resp->profiles[p_count].profilePolicy;
    }
  }
  userResp.profiles.setToExternal(profiles, user_resp->no_of_profiles);

  userResp.srvAddr.smdpAddress = user_resp->srvAddr.smdpAddress;
  userResp.srvAddr.smdsAddress = user_resp->srvAddr.smdsAddress;

  Return<void> ret = respCb->UimLpaUserResponse(token, userResp);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }

  if (profiles != NULL)
  {
    delete[] profiles;
    profiles = NULL;
  }
} /* UimLpaImpl::uimLpaUserResponse */


/*===========================================================================

FUNCTION:  UimLpaImpl::uimLpaHttpTxnCompletedResponse

===========================================================================*/
void UimLpaImpl::uimLpaHttpTxnCompletedResponse
(
  int32_t                      token,
  qcril_uim_http_result_type   result
)
{
  sp<IUimLpaResponse> respCb = getResponseCallback();
  if (respCb == NULL)
  {
    QCRIL_LOG_ERROR("uimLpaHttpTxnCompletedResponse responseCb is null");
    return;
  }
  QCRIL_LOG_INFO("UimLpaHttpTxnCompletedResponse token=%d result=%d", token, result);
  Return<void> ret = respCb->UimLpaHttpTxnCompletedResponse(token, (UimLpaResult)result);
  if (!ret.isOk()) {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
} /* UimLpaImpl::uimLpaHttpTxnCompletedResponse */


/*===========================================================================

FUNCTION:  UimLpaImpl::UimLpaUserRequest

===========================================================================*/
Return<void> UimLpaImpl::UimLpaUserRequest
(
  int32_t              token,
  const UimLpaUserReq& userReq
)
{
  int                         * tokenPtr  = new int(token);
  qcril_uim_lpa_user_req_type   user_req;

  memset(&user_req, 0x00, sizeof(user_req));

  user_req.event = (qcril_uim_lpa_user_event_type)userReq.event;
  user_req.activationCode = userReq.activationCode.c_str();
  user_req.confirmationCode = userReq.confirmationCode.c_str();
  user_req.nickname = userReq.nickname.c_str();
  user_req.iccid    = userReq.iccid.data();
  user_req.iccid_len = userReq.iccid.size();
  user_req.resetMask = userReq.resetMask;
  user_req.userOk = userReq.userOk;
  user_req.srvOpReq.opCode = (qcril_uim_lpa_srv_addr_op_type)userReq.srvOpReq.opCode;
  user_req.srvOpReq.smdpAddress = userReq.srvOpReq.smdpAddress.c_str();
  qcril_uim_lpa_user_request(tokenPtr, &user_req);

  return Status::ok();
} /* UimLpaImpl::UimLpaUserRequest */


/*===========================================================================

FUNCTION:  UimLpaImpl::UimLpaHttpTxnCompletedRequest

===========================================================================*/
Return<void> UimLpaImpl::UimLpaHttpTxnCompletedRequest
(
  int32_t                                 token,
  UimLpaResult                            result,
  const hidl_vec<uint8_t>&                responsePayload,
  const hidl_vec<UimLpaHttpCustomHeader>& customHeaders
)
{
  int                                 * tokenPtr  = new int(token);
  qcril_uim_http_transaction_req_type   http_req;

  memset(&http_req, 0x00, sizeof(http_req));

  http_req.tokenId = token;
  http_req.result  = (qcril_uim_http_result_type)result;
  http_req.payload = responsePayload.data();
  http_req.payload_len = responsePayload.size();

  if (customHeaders.size() != 0 &&
      customHeaders.data() != 0)
  {
    http_req.customHeaders =
      new qcril_uim_http_custom_header_req_type[customHeaders.size()]();

    if (http_req.customHeaders != NULL)
    {
      uint8_t                        itr     = 0;
      const UimLpaHttpCustomHeader * headers = customHeaders.data();

      for (; itr < customHeaders.size(); itr++)
      {
        http_req.customHeaders[itr].headerName = headers[itr].headerName.c_str();
        http_req.customHeaders[itr].headerValue = headers[itr].headerValue.c_str();
      }
      http_req.no_of_headers = customHeaders.size();
    }
  }
  qcril_uim_http_client_transaction_request(tokenPtr, &http_req);

  if (http_req.customHeaders != NULL)
  {
    delete[] http_req.customHeaders;
    http_req.customHeaders = NULL;
  }
  return Status::ok();
} /* UimLpaImpl::UimLpaHttpTxnCompletedRequest */


/*===========================================================================

FUNCTION:  qcril_uim_lpa_register_service

===========================================================================*/
extern "C" uint8_t qcril_uim_lpa_register_service
(
  int instanceId
)
{
  android::status_t ret = android::OK;
  uimLpa = new UimLpaImpl();
  uimLpa->setInstanceId(instanceId);
  std::string serviceName = "UimLpa";
  ret = uimLpa->registerAsService(serviceName + std::to_string(instanceId));
  QCRIL_LOG_INFO("qcril_uim_lpa_register_service instanceId=%d status=%d", instanceId, ret);

  return ret == android::OK;
} /* qcril_uim_lpa_register_service */


/*===========================================================================

FUNCTION:  qcril_uim_lpa_http_txn_completed_response

===========================================================================*/
extern "C" void qcril_uim_lpa_http_txn_completed_response
(
  void  * token_ptr,
  qcril_uim_http_result_type result
)
{
  if (uimLpa == NULL || token_ptr == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_lpa_http_txn_completed_response uim lpa service is null");
    return;
  }
  uimLpa->uimLpaHttpTxnCompletedResponse(*((int *)token_ptr), result);
  delete (int *)token_ptr;
} /* qcril_uim_lpa_http_txn_completed_response */


/*===========================================================================

FUNCTION:  qcril_uim_lpa_user_response

===========================================================================*/
extern "C" void qcril_uim_lpa_user_response
(
  void                         * token_ptr,
  qcril_uim_lpa_user_resp_type * user_resp_ptr
)
{
  if (uimLpa == NULL || token_ptr == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_lpa_user_response uim lpa service is null");
    return;
  }
  uimLpa->uimLpaUserResponse(*((int *)token_ptr), user_resp_ptr);
  delete (int *)token_ptr;
} /* qcril_uim_lpa_user_response */


/*===========================================================================

FUNCTION:  qcril_uim_lpa_add_profile_progress_ind

===========================================================================*/
extern "C" void qcril_uim_lpa_add_profile_progress_ind
(
  qcril_uim_lpa_add_profile_progress_ind_type * prog_ind_ptr
)
{
  if (uimLpa == NULL)
  {
    QCRIL_LOG_ERROR("uimLpaAddProfileProgressInd uim lpa service is null");
    return;
  }
  uimLpa->uimLpaAddProfileProgressInd(prog_ind_ptr);
} /* qcril_uim_lpa_add_profile_progress_ind */


/*===========================================================================

FUNCTION:  qcril_uim_lpa_http_txn_indication

===========================================================================*/
extern "C" void qcril_uim_lpa_http_txn_indication
(
  qcril_uim_http_transaction_ind_type * txn_ind_ptr
)
{
  if (uimLpa == NULL)
  {
    QCRIL_LOG_ERROR("qcril_uim_lpa_http_txn_indication uim lpa service is null");
    return;
  }
  uimLpa->uimLpaHttpTxnIndication(txn_ind_ptr);
} /* qcril_uim_lpa_http_txn_indication */

} // namespace implementation
} // namespace V1_0
} // namespace lpa
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor
