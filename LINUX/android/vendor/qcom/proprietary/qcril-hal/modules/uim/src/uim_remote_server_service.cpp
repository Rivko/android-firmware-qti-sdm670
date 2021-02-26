/******************************************************************************
@file    uim_remote_remote_service.c
@brief   qcril uim remote server service

DESCRIPTION
Implements the server side of the IUimRemoteServiceServer interface.
Handles RIL requests and responses and indications to be received
and sent to client respectively

---------------------------------------------------------------------------

Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
---------------------------------------------------------------------------
******************************************************************************/

#include "uim_remote_server_service.h"
#include "framework/Log.h"
#include "modules/uim/qcril_uim_sap.h"

#define TAG "UIM_REMOTE_SERVER_SERVICE"

using namespace vendor::qti::hardware::radio::uim_remote_server::V1_0::implementation;
using ::android::hardware::Status;

namespace vendor {
namespace qti {
namespace hardware {
namespace radio {
namespace uim_remote_server {
namespace V1_0 {
namespace implementation {

typedef enum {
  UIM_REMOTE_SERVER_CONNECT_REQ,
  UIM_REMOTE_SERVER_DISCONNECT_REQ,
  UIM_REMOTE_SERVER_APDU_REQ,
  UIM_REMOTE_SERVER_TRANSFER_ATR_REQ,
  UIM_REMOTE_SERVER_POWER_REQ,
  UIM_REMOTE_SERVER_RESET_SIM_REQ,
  UIM_REMOTE_SERVER_TRANSFER_CARD_READER_STATUS_REQ,
  UIM_REMOTE_SERVER_SET_TRANSFER_PROTOCOL_REQ
} UimRemoteServerRequestType;

static sp<UimRemoteServerImpl> uimRemoteServer = NULL;

static qmi_uim_sap_status_type qcril_sap_state = QMI_UIM_SAP_STATE_NOT_ENABLED;


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::UimRemoteServerImpl

===========================================================================*/
UimRemoteServerImpl::UimRemoteServerImpl()
{
  mResponseCb = NULL;
  mIndicationCb = NULL;
} /* UimRemoteServerImpl::UimRemoteServerImpl() */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::setCallback

===========================================================================*/
Return<void> UimRemoteServerImpl::setCallback
(
  const sp<IUimRemoteServiceServerResponse>& responseCallback,
  const sp<IUimRemoteServiceServerIndication>& indicationCallback
)
{
  QCRIL_LOG_INFO("UimRemoteServerImpl::setCallback");
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
} /* UimRemoteServerImpl::setCallback */

sp<IUimRemoteServiceServerResponse> UimRemoteServerImpl::getResponseCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mResponseCb;
}

sp<IUimRemoteServiceServerIndication> UimRemoteServerImpl::getIndicationCallback() {
  RWLock::AutoRLock lock(mCallbackLock);
  return mIndicationCb;
}

/*===========================================================================

FUNCTION:  UimRemoteServerImpl::serviceDied

===========================================================================*/
void UimRemoteServerImpl::serviceDied
(
 uint64_t,
 const ::android::wp<::android::hidl::base::V1_0::IBase> &
)
{
  QCRIL_LOG_INFO("UimRemoteServerImpl::serviceDied: Client died. Cleaning up callbacks");
  clearCallbacks();
} /* UimRemoteServerImpl::serviceDied */

/*===========================================================================

FUNCTION:  UimRemoteServerImpl::clearCallbacks

===========================================================================*/
void UimRemoteServerImpl::clearCallbacks()
{
  QCRIL_LOG_ERROR("UimRemoteServerImpl::clearCallbacks");
  RWLock::AutoWLock lock(mCallbackLock);
  if (mResponseCb != nullptr)
  {
    mResponseCb = NULL;
  }
  if (mIndicationCb != nullptr)
  {
    mIndicationCb = NULL;
  }
} /* UimRemoteServerImpl::clearCallbacks() */

/*===========================================================================

FUNCTION:  UimRemoteServerImpl::setInstanceId

===========================================================================*/
void UimRemoteServerImpl::setInstanceId
(
  int instanceId
)
{
  mInstanceId = instanceId;
} /* UimRemoteServerImpl::setInstanceId */


/*=========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServerProcessIndication

===========================================================================*/
void UimRemoteServerImpl::uimRemoteServerProcessIndication
(
  qmi_uim_sap_status_type  sap_state
)
{
  QCRIL_LOG_INFO("Sap state :%d", sap_state);

  Return<void> ret;
  sp<IUimRemoteServiceServerIndication> indCb = getIndicationCallback();

  if (indCb == NULL)
  {
    QCRIL_LOG_ERROR("mIndicationCb is null");
  }

  qcril_sap_state = sap_state;
  switch(sap_state)
  {
    case QMI_UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY:
      ret = indCb->uimRemoteServiceServerDisconnectIndication(
      UimRemoteServiceServerDisconnectType::UIM_REMOTE_SERVICE_SERVER_DISCONNECT_GRACEFUL);
      break;
    case QMI_UIM_SAP_STATE_CONNECTION_ERROR:
    case QMI_UIM_SAP_STATE_NOT_ENABLED:
      ret = indCb->uimRemoteServiceServerStatusIndication(
      UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_NOT_ACCESSIBLE);
      break;
    case QMI_UIM_SAP_STATE_CONNECTED_SUCCESSFULLY:
    case QMI_UIM_SAP_STATE_CONNECTING:
    case QMI_UIM_SAP_STATE_DISCONNECTING:
      break;
  }

  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send indication. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteServerImpl::uimRemoteServerProcessIndication */


/*=========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServerProcessRequestResp

===========================================================================*/
void UimRemoteServerImpl::uimRemoteServerProcessRequestResp
(
  int             request_id,
  int             err_code,
  int             token,
  uint8_t       * data_ptr,
  uint32_t        data_len
)
{
  Return<void> ret;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();
  sp<IUimRemoteServiceServerIndication> indCb = getIndicationCallback();

  QCRIL_LOG_INFO("Response token :%d, request: %d err : %d", token, request_id, err_code);

  if (respCb == NULL)
  {
    QCRIL_LOG_ERROR("mResponseCb is null");
    return;
  }

  switch(request_id)
  {
    case UIM_REMOTE_SERVER_CONNECT_REQ:
      if (err_code != QCRIL_UIM_SAP_CONNECT_SUCCESS)
      {
        if (qcril_uim_sap_get_active_client() == QCRIL_UIM_SAP_CLIENT_ID_QCRIL)
        {
          qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_MAX_CLIENTS);
        }
      }
      ret = respCb->uimRemoteServiceServerConnectResponse(token, (UimRemoteServiceServerConnectRsp)err_code, 0);
      if (err_code == QCRIL_UIM_SAP_CONNECT_SUCCESS &&
          qcril_sap_state == QMI_UIM_SAP_STATE_CONNECTED_SUCCESSFULLY &&
          indCb != NULL)
      {
        Return<void> indRet = indCb->uimRemoteServiceServerStatusIndication(
        UimRemoteServiceServerStatus::UIM_REMOTE_SERVICE_SERVER_CARD_RESET);
        if (!indRet.isOk())
        {
          QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
        }
      }
      break;

    case UIM_REMOTE_SERVER_DISCONNECT_REQ:
      ret = respCb->uimRemoteServiceServerDisconnectResponse(token);
      qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_MAX_CLIENTS);
      break;

    case UIM_REMOTE_SERVER_APDU_REQ:
      {
        hidl_vec<uint8_t> data;

        data.setToExternal(data_ptr, data_len);
        ret = respCb->uimRemoteServiceServerApduResponse(token, (UimRemoteServiceServerResultCode)err_code, data);
      }
      break;

    case UIM_REMOTE_SERVER_TRANSFER_ATR_REQ:
      {
        hidl_vec<uint8_t> data;

        data.setToExternal(data_ptr, data_len);
        ret = respCb->uimRemoteServiceServerTransferAtrResponse(token, (UimRemoteServiceServerResultCode)err_code, data);
      }
      break;

    case UIM_REMOTE_SERVER_POWER_REQ:
      ret = respCb->uimRemoteServiceServerPowerResponse(token, (UimRemoteServiceServerResultCode)err_code);
      break;

    case UIM_REMOTE_SERVER_RESET_SIM_REQ:
      ret = respCb->uimRemoteServiceServerResetSimResponse(token, (UimRemoteServiceServerResultCode)err_code);
      break;

    case UIM_REMOTE_SERVER_TRANSFER_CARD_READER_STATUS_REQ:
      {
        int32_t cardReaderStatus = -1;

        if (data_ptr != NULL &&
            data_len > 0 &&
            data_len <= sizeof(int32_t))
        {
          memcpy(&cardReaderStatus,
          data_ptr,
          data_len);
        }
        ret = respCb->uimRemoteServiceServerTransferCardReaderStatusResponse(token,
                                                                            (UimRemoteServiceServerResultCode)err_code,
                                                                            cardReaderStatus);
      }
      break;

    case UIM_REMOTE_SERVER_SET_TRANSFER_PROTOCOL_REQ:
      ret = respCb->uimRemoteServiceServerTransferProtocolResponse(token, (UimRemoteServiceServerResultCode)err_code);
      break;

    default:
      break;
  }

  if (!ret.isOk())
  {
    QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
  }
} /* UimRemoteServerImpl::uimRemoteServerProcessRequestResp */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerConnectReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerConnectReq
(
  int32_t token,
  int32_t maxMsgSize
)
{
  int                 err              = QCRIL_UIM_SAP_CONNECT_SUCCESS;
  uint8               active_client_id = qcril_uim_sap_get_active_client();
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Connect request token :%d, maxsize: %d active client : %d",
                  token, maxMsgSize, active_client_id);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_CONNECT_FAILURE;
  }
  else if (active_client_id == QCRIL_UIM_SAP_MAX_CLIENTS)
  {
    qmi_uim_sap_connection_params_type  sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.conn_condition = QMI_UIM_SAP_CONN_COND_BLOCK_VOICE;
    sap_request_params.operation_type = QMI_UIM_SAP_CONNECTION_CONNECT;

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_CONNECTION_REQ,
                                            (int)UIM_REMOTE_SERVER_CONNECT_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP connection request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_CONNECT_FAILURE;
    }
  }
  else
  {
    err = QCRIL_UIM_SAP_CONNECT_FAILURE;
  }

  if (err != QCRIL_UIM_SAP_CONNECT_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_CONNECT_REQ,
                                      token_ptr,
                                      (int)err);
    }
    else if (respCb != NULL)
    {
      Return<void> ret = respCb->uimRemoteServiceServerConnectResponse(token,
                                     (UimRemoteServiceServerConnectRsp)err,
                                     0);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  else
  {
    qcril_uim_sap_set_active_client(QCRIL_UIM_SAP_CLIENT_ID_QCRIL);
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerConnectReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerDisconnectReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerDisconnectReq
(
  int32_t token
)
{
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  bool                err              = FALSE;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Disconnect request token :%d", token);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = TRUE;
  }
  else
  {
    qmi_uim_sap_connection_params_type  sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.conn_condition = QMI_UIM_SAP_CONN_COND_BLOCK_VOICE;
    /* Note - for disconnect req, there is no mode passed from client */
    sap_request_params.operation_type  = QMI_UIM_SAP_CONNECTION_DISCONNECT;
    sap_request_params.disconnect_mode = QMI_UIM_SAP_DISCONNECT_MODE_GRACEFUL;

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_CONNECTION_REQ,
                                            (int)UIM_REMOTE_SERVER_DISCONNECT_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP disconnect request failed, qmi_result: 0x%x", qmi_result);
      err = TRUE;
    }
  }

  if (err)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_DISCONNECT_REQ,
                                      token_ptr,
                                      (int)err);
    }
    else if (respCb != NULL)
    {
      Return<void> ret = respCb->uimRemoteServiceServerDisconnectResponse(token);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerDisconnectReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerApduReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerApduReq
(
  int32_t                        token,
  UimRemoteServiceServerApduType type,
  const hidl_vec<uint8_t>&       command
)
{
  int                 err              = QCRIL_UIM_SAP_SUCCESS;
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("APDU request token :%d, type: %d", token, type);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }
  else if (command.data() != NULL &&
           command.size() > 0 &&
           command.size() <= QMI_UIM_MAX_APDU_DATA)
  {
    qmi_uim_sap_request_params_type   sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.request_type   = QMI_UIM_SAP_REQUEST_OP_SEND_APDU;
    sap_request_params.apdu.data_len  = command.size();
    sap_request_params.apdu.data_ptr  = (unsigned char *)command.data();

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_REQUEST_REQ,
                                            (int)UIM_REMOTE_SERVER_APDU_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP APDU request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_GENERIC_FAILURE;
    }
  }
  else
  {
    QCRIL_LOG_ERROR("Invalid APDU request");
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }

  if (err != QCRIL_UIM_SAP_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_APDU_REQ,
                                      token_ptr,
                                      err);
    }
    else if (respCb != NULL)
    {
      hidl_vec<uint8_t> data;

      data.setToExternal(NULL, 0);
      Return<void> ret = respCb->uimRemoteServiceServerApduResponse(token, (UimRemoteServiceServerResultCode)err, data);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerApduReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerTransferAtrReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerTransferAtrReq
(
  int32_t token
)
{
  int                 err              = QCRIL_UIM_SAP_SUCCESS;
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Transfer ATR request token: %d", token);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }
  else
  {
    qmi_uim_sap_request_params_type   sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.request_type   = QMI_UIM_SAP_REQUEST_OP_GET_ATR;

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_REQUEST_REQ,
                                            (int)UIM_REMOTE_SERVER_TRANSFER_ATR_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP Transfer ATR request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_GENERIC_FAILURE;
    }
  }

  if (err != QCRIL_UIM_SAP_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_TRANSFER_ATR_REQ,
                                      token_ptr,
                                      err);
    }
    else if (respCb != NULL)
    {
      hidl_vec<uint8_t> data;

      data.setToExternal(NULL, 0);
      Return<void> ret = respCb->uimRemoteServiceServerTransferAtrResponse(token, (UimRemoteServiceServerResultCode)err, data);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerTransferAtrReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerPowerReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerPowerReq
(
  int32_t token,
  bool    state
)
{
  int                 err              = QCRIL_UIM_SAP_SUCCESS;
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Power request token :%d, state: %d", token, state);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }
  else
  {
    qmi_uim_sap_request_params_type   sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    if (state)
    {
      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_POWER_SIM_ON;
    }
    else
    {
      sap_request_params.request_type = QMI_UIM_SAP_REQUEST_OP_POWER_SIM_OFF;
    }

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_REQUEST_REQ,
                                            (int)UIM_REMOTE_SERVER_POWER_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP power request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_GENERIC_FAILURE;
    }
  }

  if (err != QCRIL_UIM_SAP_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_POWER_REQ,
                                      token_ptr,
                                      err);
    }
    else if (respCb != NULL)
    {
      Return<void> ret = respCb->uimRemoteServiceServerPowerResponse(token, (UimRemoteServiceServerResultCode)err);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerPowerReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerResetSimReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerResetSimReq
(
  int32_t token
)
{
  int                 err              = QCRIL_UIM_SAP_SUCCESS;
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("SIM reset request token :%d", token);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }
  else
  {
    qmi_uim_sap_request_params_type   sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.request_type   = QMI_UIM_SAP_REQUEST_OP_RESET_SIM;

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_REQUEST_REQ,
                                            (int)UIM_REMOTE_SERVER_RESET_SIM_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP sim reset request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_GENERIC_FAILURE;
    }
  }

  if (err != QCRIL_UIM_SAP_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_RESET_SIM_REQ,
                                      token_ptr,
                                      err);
    }
    else if (respCb != NULL)
    {
      Return<void> ret = respCb->uimRemoteServiceServerResetSimResponse(token, (UimRemoteServiceServerResultCode)err);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerResetSimReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerTransferCardReaderStatusReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerTransferCardReaderStatusReq
(
  int32_t token
)
{
  int                 err              = QCRIL_UIM_SAP_SUCCESS;
  qmi_uim_slot_type   qmi_slot         = QMI_UIM_SLOT_1;
  int               * token_ptr        = NULL;
  int                 qmi_result       = 0;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Card reader status request token :%d", token);

  token_ptr = new int(token);

  if (!qcril_uim_sap_convert_slot_id_to_slot_type(qmi_ril_get_sim_slot(), &qmi_slot) ||
      token_ptr == NULL)
  {
    err = QCRIL_UIM_SAP_GENERIC_FAILURE;
  }
  else
  {
    qmi_uim_sap_request_params_type   sap_request_params;

    memset(&sap_request_params, 0x00, sizeof(sap_request_params));

    /* Update QMI parameters from request & dispatch it to modem */
    sap_request_params.slot           = qmi_slot;
    sap_request_params.request_type   = QMI_UIM_SAP_REQUEST_OP_READER_STATUS;

    qmi_result = qcril_uim_sap_send_request(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                            QCRIL_UIM_SAP_REQUEST_REQ,
                                            (int)UIM_REMOTE_SERVER_TRANSFER_CARD_READER_STATUS_REQ,
                                            token_ptr,
                                            &sap_request_params);
    if (qmi_result < 0)
    {
      QCRIL_LOG_ERROR("SAP Card reader status request failed, qmi_result: 0x%x", qmi_result);
      err = QCRIL_UIM_SAP_GENERIC_FAILURE;
    }
  }

  if (err != QCRIL_UIM_SAP_SUCCESS)
  {
    QCRIL_LOG_ERROR("Failed to send request err: 0x%x", err);
    if (token_ptr != NULL)
    {
      qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                      UIM_REMOTE_SERVER_TRANSFER_CARD_READER_STATUS_REQ,
                                      token_ptr,
                                      err);
    }
    else if (respCb != NULL)
    {
      Return<void> ret = respCb->uimRemoteServiceServerTransferCardReaderStatusResponse(token, (UimRemoteServiceServerResultCode)err, -1);
      if (!ret.isOk())
      {
        QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
      }
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerTransferCardReaderStatusReq */


/*===========================================================================

FUNCTION:  UimRemoteServerImpl::uimRemoteServiceServerSetTransferProtocolReq

===========================================================================*/
Return<void> UimRemoteServerImpl::uimRemoteServiceServerSetTransferProtocolReq
(
  int32_t                                token,
  UimRemoteServiceServerTransferProtocol transferProtocol
)
{
  int                 err         = QCRIL_UIM_SAP_NOT_SUPPORTED;
  int               * token_ptr   = NULL;
  sp<IUimRemoteServiceServerResponse> respCb = getResponseCallback();

  QCRIL_LOG_INFO("Card reader status request token :%d, protocol: %d",
                  token, transferProtocol);

  token_ptr = new int(token);

  if (token_ptr != NULL)
  {
    qcril_uim_sap_send_err_response(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                    UIM_REMOTE_SERVER_SET_TRANSFER_PROTOCOL_REQ,
                                    token_ptr,
                                    err);
  }
  else if (respCb != NULL)
  {
    Return<void> ret = respCb->uimRemoteServiceServerTransferProtocolResponse(token, (UimRemoteServiceServerResultCode)err);
    if (!ret.isOk())
    {
      QCRIL_LOG_ERROR("Unable to send response. Exception : %s", ret.description().c_str());
    }
  }
  return Status::ok();
} /* UimRemoteServerImpl::uimRemoteServiceServerSetTransferProtocolReq */


/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_qcril_sap_ind

===========================================================================*/
static void qcril_uim_sap_qmi_handle_qcril_sap_ind
(
  qmi_uim_sap_status_type  sap_state
)
{
  if (uimRemoteServer != NULL)
  {
    uimRemoteServer->uimRemoteServerProcessIndication(sap_state);
  }
} /* qcril_uim_sap_qmi_handle_qcril_sap_ind */


/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_qcril_sap_request_resp

===========================================================================*/
static void qcril_uim_sap_qmi_handle_qcril_sap_request_resp
(
    int             request_id,
    int             err_code,
    int             qmi_err_code,
    void          * token,
    uint8_t       * data_ptr,
    uint32_t        data_len
)
{
  int token_id = 0;

  QCRIL_LOG_INFO("sap request rsp request_id : %d, err_code : %d, qmi_err_code : %d",
                  request_id, err_code, qmi_err_code);

  if (token == NULL)
  {
    return;
  }

  token_id = *((int *)token);
  QCRIL_UIM_SAP_FREE_PTR(token);

  if (uimRemoteServer != NULL)
  {
    uimRemoteServer->uimRemoteServerProcessRequestResp(request_id,
                                                       err_code,
                                                       token_id,
                                                       data_ptr,
                                                       data_len);
  }
} /* qcril_uim_sap_qmi_handle_qcril_sap_request_resp */

} // namespace implementation
} // namespace V1_0
} // namespace uim_remote_server
} // namespace radio
} // namespace hardware
} // namespace qti
} // namespace vendor


/*===========================================================================

FUNCTION:  qcril_uim_remote_server_register_service

===========================================================================*/
uint8_t qcril_uim_remote_server_register_service
(
  int instanceId
)
{
  android::status_t ret = android::OK;
  uimRemoteServer = new UimRemoteServerImpl();
  uimRemoteServer->setInstanceId(instanceId);
  std::string serviceName = "uimRemoteServer";
  ret = uimRemoteServer->registerAsService(serviceName + std::to_string(instanceId));
  QCRIL_LOG_INFO("qcril_uim_remote_server_register_service instanceId=%d status=%d", instanceId, ret);
  qcril_uim_sap_register_client(QCRIL_UIM_SAP_CLIENT_ID_QCRIL,
                                qcril_uim_sap_qmi_handle_qcril_sap_ind,
                                qcril_uim_sap_qmi_handle_qcril_sap_request_resp,
                                NULL);
  return ret == android::OK;
} /* qcril_uim_remote_server_register_service */
