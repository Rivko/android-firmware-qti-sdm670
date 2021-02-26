/*===========================================================================

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "framework/Log.h"
#include "modules/uim/qcril_uim_legacy.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "qtiTelephonyService.pb.h"
#include "qcril_qmi_oem_misc.h"
#include "qcril_uim_util.h"

#define QCRIL_UIM_GBA_INIT_SEC_PROTOCOL_LEN      5

/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/
extern "C" void qcril_qmi_oem_socket_proto_send
(
  RIL_Token proto_token,
  QtiTelephonyService_RILOEMMessageId message_id,
  QtiTelephonyService_RILOEMMessageType type,
  boolean has_error,
  QtiTelephonyService_RILOEMError oem_proto_error,
  const void* msg,
  int msg_len
);

/*===========================================================================

  FUNCTION:  qcril_uim_gba_get_non_prov_session_type

 ==========================================================================*/
/*!
     @brief
      Fetches the sesstion enum type for the passed slot index.

     @return
      Session type enum
                                                                           */
/*=========================================================================*/
static qmi_uim_session_type qcril_uim_gba_get_non_prov_session_type
(
  uint8  slot_id
)
{
  qmi_uim_session_type session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1;

  switch (slot_id)
  {
    case 0:
      session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_1;
      break;
    case 1:
      session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_2;
      break;
    case 2:
      session_type = QMI_UIM_SESSION_TYPE_NON_PROV_SLOT_3;
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported slot_id: 0x%x", slot_id);
      break;
  }

  QCRIL_LOG_INFO("session_type found: 0x%x", session_type);
  return session_type;
} /* qcril_uim_gba_get_non_prov_session_type */


/*===========================================================================

  FUNCTION:  qcril_uim_gba_convert_slot_id

 ==========================================================================*/
/*!
     @brief
      Converts the RILOEM slot id enum type to a slot index.

     @return
      Converted slot index
                                                                           */
/*=========================================================================*/
static uint8 qcril_uim_gba_convert_slot_id
(
  QtiTelephonyService_RILOEMSlotIdentifier  RILOEM_slot_id
)
{
  uint8 slot_id = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;

  switch (RILOEM_slot_id)
  {
    case QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_ONE:
      slot_id = 0;
      break;
    case QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_TWO:
      slot_id = 1;
      break;
    case QtiTelephonyService_RILOEMSlotIdentifier_SLOT_ID_THREE:
      slot_id = 2;
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported slotId: 0x%x", RILOEM_slot_id);
      break;
  }

  QCRIL_LOG_INFO("slot_id converted: 0x%x", slot_id);
  return slot_id;
} /* qcril_uim_gba_convert_slot_id */


/*===========================================================================

  FUNCTION:  qcril_uim_gba_get_gw_session_type_from_slot

 ==========================================================================*/
/*!
     @brief
      This is used to obtain GW session type corresponding to the slot ID passed in.
      This function is used to obtain only primary and secondary sessions.

     @return
      Associated Session Type.
      It is valid only when either it is primary or secondary GW session
                                                                           */
/*=========================================================================*/
static qmi_uim_session_type qcril_uim_gba_get_gw_session_type_from_slot
(
  uint8  slot_id
)
{
  unsigned short         session_index  = 0xFFFF;
  qmi_uim_session_type   session_type   = QMI_UIM_SESSION_TYPE_MAX;

  session_index = qcril_uim.card_status.index_gw_pri_prov;

  if (((session_index >> 8) & 0xFF) == slot_id)
  {
    session_type = QMI_UIM_SESSION_TYPE_PRI_GW_PROV;
  }
  else
  {
    session_index = qcril_uim.card_status.index_gw_sec_prov;

    if (((session_index >> 8) & 0xFF) == slot_id)
    {
      session_type = QMI_UIM_SESSION_TYPE_SEC_GW_PROV;
    }
  }

  return session_type;
} /* qcril_uim_gba_get_gw_session_type_from_slot */


/*=========================================================================

  FUNCTION:  qcril_uim_oem_get_impi_resp

===========================================================================*/
/*!
    @brief
    Handles the response of OEM get IMPI request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_get_impi_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  QtiTelephonyService_RILOEMError                  ril_err;
  QtiTelephonyService_RilOemImpiResponsePayload    impi_resp;
  qcril_binary_data_type                           impi_data;
  RIL_Token                                        token            = NULL;
  qcril_uim_original_request_type                * orig_request_ptr = NULL;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    return;
  }

  /* Retrieve original request */
  orig_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(orig_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    return;
  }

  /* Get parameters from original request param */
  token = orig_request_ptr->token;

  ril_err = (params_ptr->qmi_rsp_data.qmi_err_code == QMI_NO_ERR) ?
              QtiTelephonyService_RILOEMError_RIL_OEM_ERR_SUCCESS : QtiTelephonyService_RILOEMError_RIL_OEM_ERR_GENERIC_FAILURE;

  QCRIL_LOG_DEBUG("qcril_uim_oem_get_impi_resp, instance_id: %x, qmi_err_code: 0x%x",
                  orig_request_ptr->instance_id,
                  params_ptr->qmi_rsp_data.qmi_err_code);

  /* Update the response */
  memset(&impi_data, 0, sizeof(qcril_binary_data_type));
  memset(&impi_resp, 0, sizeof(QtiTelephonyService_RilOemImpiResponsePayload));

  /* If successful update the response with encrypted IMSI data */
  if ((params_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_len > 0) &&
      (params_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_ptr != NULL))
  {
    impi_data.len  = params_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_len;
    impi_data.data = params_ptr->qmi_rsp_data.rsp_data.gba_impi_rsp.impi.data_ptr;
  }

  impi_resp.impi.arg = &impi_data;

  /* Send the response */
  qcril_qmi_oem_socket_proto_send(token,
                                  qcril_qmi_oem_proto_map_event_to_request(orig_request_ptr->data.gba_event_id),
                                  QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE,
                                  TRUE,
                                  ril_err,
                                  &impi_resp,
                                  sizeof(QtiTelephonyService_RilOemImpiResponsePayload) + sizeof(qcril_binary_data_type));

  /* Free up allocated original req pointer */
  QCRIL_UIM_FREE_IF_NOT_NULL(orig_request_ptr);
} /* qcril_uim_oem_get_impi_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_oem_gba_resp

===========================================================================*/
/*!
    @brief
    Handles the response of OEM GBA initialization request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_gba_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  QtiTelephonyService_RILOEMError                    ril_err;
  QtiTelephonyService_RilOemGbaInitResponsePayload   gba_init_resp;
  qcril_binary_data_type                             naf_rsp_data;
  char                                             * b_tid_ptr        = NULL;
  char                                             * ks_lifetime_ptr  = NULL;
  RIL_Token                                          token            = NULL;
  qcril_uim_original_request_type                  * orig_request_ptr = NULL;

  /* Sanity check */
  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL params_ptr");
    return;
  }

  /* Retrieve original request */
  orig_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(orig_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL original_request_ptr");
    return;
  }

  /* Get parameters from original request param */
  token = orig_request_ptr->token;

  ril_err = (params_ptr->qmi_rsp_data.qmi_err_code == QMI_NO_ERR) ?
              QtiTelephonyService_RILOEMError_RIL_OEM_ERR_SUCCESS : QtiTelephonyService_RILOEMError_RIL_OEM_ERR_GENERIC_FAILURE;

  QCRIL_LOG_DEBUG("instance_id: %x, qmi_err_code: 0x%x",
                  orig_request_ptr->instance_id,
                  params_ptr->qmi_rsp_data.qmi_err_code);

  /* Update the response */
  memset(&naf_rsp_data, 0, sizeof(qcril_binary_data_type));
  memset(&gba_init_resp, 0, sizeof(QtiTelephonyService_RilOemGbaInitResponsePayload));

  /* Check for appropriate NAF response types & update */
  gba_init_resp.ksNAFType = (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.is_ks_naf_encrypted == TRUE) ?
    QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_ENCRYPTED : QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_PLAIN;

  if ((params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_ptr != NULL) &&
      (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_len > 0))
  {
    /* If plaintext, convert to base64 format */
    if (gba_init_resp.ksNAFType == QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_PLAIN)
    {
      naf_rsp_data.data = (uint8_t *)qcril_uim_alloc_bin_to_base64string(
                            params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_ptr,
                            params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_len);
      naf_rsp_data.len = (naf_rsp_data.data == NULL) ? 0 : strlen((const char *)naf_rsp_data.data);
    }
    /* else send data as is */
    else
    {
      naf_rsp_data.len  = params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_len;
      naf_rsp_data.data = params_ptr->qmi_rsp_data.rsp_data.gba_rsp.ks_naf.data_ptr;
    }
  }

  gba_init_resp.ksNAFResponse.arg = &naf_rsp_data;

  /* Update the B-TId */
  if ((params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_ptr != NULL) &&
      (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_len > 0))
  {
    /* Since modem doesnt send a NULL terminated B=TID, we need to create one here */
    b_tid_ptr = (char *)qcril_malloc(params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_len + 1);
    if (b_tid_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Couldnt allocate b_tid_ptr");
    }
    else
    {
      memset(b_tid_ptr, 0, (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_len + 1));
      memcpy(b_tid_ptr,
             params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_ptr,
             params_ptr->qmi_rsp_data.rsp_data.gba_rsp.b_tid.data_len);
    }
  }

  QCRIL_LOG_INFO("B-TID: %s", b_tid_ptr != NULL ? (char *)b_tid_ptr : "NULL");
  gba_init_resp.bootstrapTransactionId.arg = b_tid_ptr;

  /* Update the Key Lifetime */
  if ((params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_ptr != NULL) &&
      (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_len > 0))
  {
    /* Since modem doesnt send a NULL terminated KS lifetime, we need to create one here */
    ks_lifetime_ptr = (char *)qcril_malloc(params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_len + 1);
    if (ks_lifetime_ptr == NULL)
    {
      QCRIL_LOG_ERROR("%s", "Couldnt allocate ks_lifetime_ptr");
    }
    else
    {
      memset(ks_lifetime_ptr, 0, (params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_len + 1));
      memcpy(ks_lifetime_ptr,
             params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_ptr,
             params_ptr->qmi_rsp_data.rsp_data.gba_rsp.key_lifetime.data_len);
    }
  }

  QCRIL_LOG_INFO("KS Lifetime: %s", ks_lifetime_ptr != NULL ? (char *)ks_lifetime_ptr : "NULL");
  gba_init_resp.ksLifetime.arg = ks_lifetime_ptr;

  /* Send the response */
  qcril_qmi_oem_socket_proto_send(token,
                                  qcril_qmi_oem_proto_map_event_to_request(orig_request_ptr->data.gba_event_id),
                                  QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE,
                                  TRUE,
                                  ril_err,
                                  &gba_init_resp,
                                  sizeof(QtiTelephonyService_RilOemGbaInitResponsePayload) + sizeof(qcril_binary_data_type));

  /* Free up allocated pointers */
  QCRIL_UIM_FREE_IF_NOT_NULL(orig_request_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(b_tid_ptr);
  QCRIL_UIM_FREE_IF_NOT_NULL(ks_lifetime_ptr);
  if (gba_init_resp.ksNAFType == QtiTelephonyService_RILOEMKsNAFType_KS_NAF_TYPE_PLAIN)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(naf_rsp_data.data);
  }
} /* qcril_uim_oem_gba_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_oem_request_get_impi

===========================================================================*/
/*!
    @brief
    Handles MessageId_RIL_OEM_IMPI request from the client. This is sent
    from RIL via QCRIL_EVT_OEM_MSG_GET_IMPI.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_request_get_impi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  QtiTelephonyService_RilOemImpiResponsePayload     impi_resp;
  qmi_uim_gba_impi_params_type                      gba_impi_params;
  QtiTelephonyService_RilOemImpiRequestPayload    * in_ptr       = NULL;
  qcril_uim_original_request_type                 * orig_req_ptr = NULL;
  uint8                                             slot_id      = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint8                                             app_index    = QCRIL_UIM_INVALID_APP_INDEX_VALUE;
  RIL_Errno                                         err          = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  QtiTelephonyService_RILOEMError                   oem_err      = QtiTelephonyService_RILOEMError_RIL_OEM_ERR_INVALID_PARAMETER;
  qmi_uim_session_type                              session_type = QMI_UIM_SESSION_TYPE_PRI_GW_PROV;

  /* Sanity check */
  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    return;
  }

  /* Parse input info */
  in_ptr = (QtiTelephonyService_RilOemImpiRequestPayload *)(params_ptr->data);
  if(in_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL input pointer");
    goto report_error;
  }

  QCRIL_LOG_INFO("instance_id: 0x%x, secure: %d, slotId:0x%x, applicationType: 0x%x",
                 params_ptr->instance_id,
                 in_ptr->secure,
                 in_ptr->slotId,
                 in_ptr->applicationType);

  memset(&gba_impi_params, 0, sizeof(qmi_uim_gba_impi_params_type));

  /* Fetch slot info */
  slot_id = qcril_uim_gba_convert_slot_id(in_ptr->slotId);
  if (slot_id >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot_id: 0x%x", slot_id);
    goto report_error;
  }

  /* Fetch session info */
  switch (in_ptr->applicationType)
  {
    case QtiTelephonyService_RILOEMApplicationType_APPTYPE_USIM:
      session_type = qcril_uim_gba_get_gw_session_type_from_slot(slot_id);

      if(session_type != QMI_UIM_SESSION_TYPE_PRI_GW_PROV &&
         session_type != QMI_UIM_SESSION_TYPE_SEC_GW_PROV)
      {
        QCRIL_LOG_ERROR("%s","Invalid session type");
        goto report_error;
      }
      gba_impi_params.session_info.session_type = session_type;
      break;
    case QtiTelephonyService_RILOEMApplicationType_APPTYPE_ISIM:
      err = qcril_uim_extract_isim_index(&app_index, slot_id);
      if ((err != RIL_E_SUCCESS) || (app_index >= QMI_UIM_MAX_APP_PER_CARD_COUNT))
      {
        QCRIL_LOG_ERROR("ISIM app not found in slot_id: 0x%x", slot_id);
        goto report_error;
      }
      /* Copy AID */
      gba_impi_params.session_info.aid.data_len = qcril_uim.card_status.card[slot_id].application[app_index].aid_len;
      gba_impi_params.session_info.aid.data_ptr = (unsigned char *)qcril_uim.card_status.card[slot_id].application[app_index].aid_value;
      gba_impi_params.session_info.session_type = qcril_uim_gba_get_non_prov_session_type(slot_id);
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported app type: 0x%x", in_ptr->applicationType);
      goto report_error;
  }

  QCRIL_LOG_INFO( "session_type: %d\n", gba_impi_params.session_info.session_type );

  /* Also set encryption flag if client needs it */
  if (in_ptr->secure)
  {
    gba_impi_params.encrypt_data = QMI_UIM_TRUE;
  }

  /* Allocate original request, it is freed in qmi_uim_callback */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 (qcril_modem_id_e_type)(QCRIL_MAX_MODEM_ID - 1),
                                                 params_ptr->t,
                                                 params_ptr->event_id_android,
                                                 gba_impi_params.session_info.session_type);
  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr!");
    goto report_error;
  }
  orig_req_ptr->data.gba_event_id = params_ptr->event_id;

  /* Proceed with read transparent  */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "gba impi request" );
  if (qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GBA_IMPI,
                                   &gba_impi_params,
                                   orig_req_ptr) >= 0)
  {
    return;
  }

report_error:
    /* In any case of error, check & free all the allocated pointers */
    memset(&impi_resp, 0, sizeof(QtiTelephonyService_RilOemImpiResponsePayload));
    qcril_qmi_oem_socket_proto_send((RIL_Token)params_ptr->t,
                                    qcril_qmi_oem_proto_map_event_to_request(params_ptr->event_id),
                                    QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE,
                                    TRUE,
                                    oem_err,
                                    &impi_resp,
                                    sizeof(QtiTelephonyService_RilOemImpiResponsePayload));

    /* Clean up any original request if allocated */
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
} /* qcril_uim_oem_request_get_impi */


/*===========================================================================

  FUNCTION:  qcril_uim_oem_request_gba_init

===========================================================================*/
/*!
    @brief
    Handles MessageId_RIL_OEM_GBA_INIT request from the client. This is
    sent from RIL via QCRIL_EVT_OEM_MSG_GBA_INIT.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_oem_request_gba_init
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  QtiTelephonyService_RilOemGbaInitResponsePayload   gba_init_resp;
  qmi_uim_gba_params_type                            gba_init_params;
  QtiTelephonyService_RilOemGbaInitRequestPayload  * in_ptr       = NULL;
  qcril_binary_data_type             * protocol_ptr = NULL;
  char                               * tmp_data_ptr = NULL;
  uint16                               tmp_data_len = 0;
  qcril_uim_original_request_type    * orig_req_ptr = NULL;
  uint8                                slot_id      = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint8                                app_index    = QCRIL_UIM_INVALID_APP_INDEX_VALUE;
  RIL_Errno                            err          = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qmi_uim_session_type                 session_type = QMI_UIM_SESSION_TYPE_PRI_GW_PROV;

  /* Sanity check */
  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    return;
  }

  /* Parse input info */
  in_ptr = (QtiTelephonyService_RilOemGbaInitRequestPayload *)(params_ptr->data);
  if(in_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL input pointer");
    goto report_error;
  }

  QCRIL_LOG_INFO("instance_id: 0x%x, slotId:0x%x, applicationType: 0x%x",
                 params_ptr->instance_id,
                 in_ptr->slotId,
                 in_ptr->applicationType);

  memset(&gba_init_params, 0, sizeof(qmi_uim_gba_params_type));

  /* Fetch slot info */
  slot_id = qcril_uim_gba_convert_slot_id(in_ptr->slotId);
  if (slot_id >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot_id: 0x%x", slot_id);
    goto report_error;
  }

  /* Fetch session info */
  switch (in_ptr->applicationType)
  {
    case QtiTelephonyService_RILOEMApplicationType_APPTYPE_USIM:
      session_type = qcril_uim_gba_get_gw_session_type_from_slot(slot_id);

      if(session_type != QMI_UIM_SESSION_TYPE_PRI_GW_PROV &&
         session_type != QMI_UIM_SESSION_TYPE_SEC_GW_PROV)
      {
        QCRIL_LOG_ERROR("%s","Invalid session type");
        goto report_error;
      }
      gba_init_params.session_info.session_type = session_type;
      break;
    case QtiTelephonyService_RILOEMApplicationType_APPTYPE_ISIM:
      err = qcril_uim_extract_isim_index(&app_index, slot_id);
      if ((err != RIL_E_SUCCESS) || (app_index >= QMI_UIM_MAX_APP_PER_CARD_COUNT))
      {
        QCRIL_LOG_ERROR("ISIM app not found in slot_id: 0x%x", slot_id);
        goto report_error;
      }
      /* Copy AID */
      gba_init_params.session_info.aid.data_len = qcril_uim.card_status.card[slot_id].application[app_index].aid_len;
      gba_init_params.session_info.aid.data_ptr = (unsigned char *)qcril_uim.card_status.card[slot_id].application[app_index].aid_value;
      gba_init_params.session_info.session_type = qcril_uim_gba_get_non_prov_session_type(slot_id);
      break;
    default:
      QCRIL_LOG_ERROR("Unsupported app type: 0x%x", in_ptr->applicationType);
      goto report_error;
  }

  QCRIL_LOG_INFO( "session_type: %d\n", gba_init_params.session_info.session_type );

  /* Extract FQDN */
  tmp_data_ptr = (char *)in_ptr->nafFullyQualifiedDomainName.arg;
  tmp_data_len = (tmp_data_ptr == NULL) ? 0 : strlen((const char *)tmp_data_ptr);

  QCRIL_LOG_INFO("FQDN length: %d, FQDN: %s",
                 tmp_data_len,
                 tmp_data_ptr != NULL ? (char *)tmp_data_ptr : "NULL");

  /* Return error if FQDN isnt sent, note that tmp_data_len == 0 means
     FQDN pointer is invalid due to the above check above */
  if (tmp_data_len == 0)
  {
    QCRIL_LOG_ERROR("%s", "No FQDN in request !");
    goto report_error;
  }

  /* Add FQDN */
  gba_init_params.naf_id.fqdn.data_len = tmp_data_len;
  gba_init_params.naf_id.fqdn.data_ptr = (unsigned char *)tmp_data_ptr;

  /* Return error if security protocol is invalid */
  protocol_ptr = (qcril_binary_data_type *)in_ptr->securityProtocol.arg;
  if ((protocol_ptr       == NULL) ||
      (protocol_ptr->data == NULL) ||
      (protocol_ptr->len  != QCRIL_UIM_GBA_INIT_SEC_PROTOCOL_LEN))
  {
    QCRIL_LOG_ERROR("%s", "Invalid securityProtocol in request !");
    goto report_error;
  }

  /* Add security protocol */
  memcpy(gba_init_params.naf_id.protocol_id, protocol_ptr->data, protocol_ptr->len);

  /* Add bootstrapping info */
  gba_init_params.force_bootstrapping = in_ptr->forceBootstrapping ?
                                          QMI_UIM_TRUE : QMI_UIM_FALSE;
  QCRIL_LOG_INFO("FQDN forceBootstrapping: %s",
                 in_ptr->forceBootstrapping ? "TRUE" : "FALSE");

  /* Allocate original request */
  orig_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                 (qcril_modem_id_e_type)(QCRIL_MAX_MODEM_ID - 1),
                                                 (RIL_Token)params_ptr->t,
                                                 params_ptr->event_id_android,
                                                 gba_init_params.session_info.session_type);
  if (orig_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for orig_req_ptr");
    goto report_error;
  }
  orig_req_ptr->data.gba_event_id = params_ptr->event_id;

  /* Proceed with the GBA request */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "gba request" );
  if (qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GBA,
                                   &gba_init_params,
                                   orig_req_ptr) >= 0)
  {
    return;
  }

report_error:
    /* In any case of error, check & free all the allocated pointers */
    memset(&gba_init_resp, 0, sizeof(QtiTelephonyService_RilOemGbaInitResponsePayload));
    qcril_qmi_oem_socket_proto_send((RIL_Token)params_ptr->t,
                                  qcril_qmi_oem_proto_map_event_to_request(params_ptr->event_id),
                                  QtiTelephonyService_RILOEMMessageType_RIL_OEM_MSG_RESPONSE,
                                  TRUE,
                                  QtiTelephonyService_RILOEMError_RIL_OEM_ERR_GENERIC_FAILURE,
                                  &gba_init_resp,
                                  sizeof(QtiTelephonyService_RilOemGbaInitResponsePayload));

    /* Clean up any original request if allocated */
    QCRIL_UIM_FREE_IF_NOT_NULL(orig_req_ptr);
} /* qcril_uim_oem_request_gba_init */

