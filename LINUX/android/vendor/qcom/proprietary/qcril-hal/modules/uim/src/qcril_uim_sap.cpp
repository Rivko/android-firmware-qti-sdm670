/*===========================================================================

  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "framework/Log.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "qcril_uim_util.h"

#define TAG "QCRIL_UIM_SAP"

typedef struct
{
  qcril_uim_sap_ind_handler          ind_handler;
  qcril_uim_sap_request_resp_handler resp_handler;
  qcril_uim_sap_card_status_handler  card_status_handler;
} qcril_uim_sap_response_functions;

static qcril_uim_sap_response_functions  qcril_sap_response_api[QCRIL_UIM_SAP_MAX_CLIENTS];
static qcril_uim_sap_clients_type        active_client_id = QCRIL_UIM_SAP_MAX_CLIENTS;
static qcril_card_status_e_type          qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_UNKNOWN;


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_sap_convert_slot_type_to_instance_id

===========================================================================*/
/*!
    @brief
    Routine to convert a QMI slot enum type to RIL instance

    @return
    qcril_instance_id_e_type RIL instance.
*/
/*=========================================================================*/
static qcril_instance_id_e_type qcril_uim_sap_convert_slot_type_to_instance_id
(
  qmi_uim_slot_type   qmi_slot_type
)
{
  qcril_instance_id_e_type instance_id = QCRIL_MAX_INSTANCE_ID;
  switch (qmi_slot_type)
  {
     case QMI_UIM_SLOT_1:
       instance_id = QCRIL_DEFAULT_INSTANCE_ID;
       break;

     case QMI_UIM_SLOT_2:
       instance_id = QCRIL_SECOND_INSTANCE_ID;
       break;

    case QMI_UIM_SLOT_3:
      instance_id = QCRIL_THIRD_INSTANCE_ID;
      break;

    default:
      QCRIL_LOG_ERROR( "Invalid slot_id returned: 0x%x\n", qmi_slot_type);
  }

  return instance_id;
} /* qcril_uim_sap_convert_slot_type_to_instance_id */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_uim_sap_convert_slot_id_to_slot_type

===========================================================================*/
/*!
    @brief
    Routine to convert a RIL slot id to QMI slot enum type.

    @return
    Boolean indicating the outcome of the request.
*/
/*=========================================================================*/
boolean qcril_uim_sap_convert_slot_id_to_slot_type
(
  uint32_t              slot_id,
  qmi_uim_slot_type   * qmi_slot_type_ptr
)
{
  if (qmi_slot_type_ptr == NULL)
  {
    return FALSE;
  }

  switch (slot_id)
  {
     case 0:
       *qmi_slot_type_ptr = QMI_UIM_SLOT_1;
       break;

     case 1:
       *qmi_slot_type_ptr = QMI_UIM_SLOT_2;
       break;

    case 2:
      *qmi_slot_type_ptr = QMI_UIM_SLOT_3;
      break;

    default:
      QCRIL_LOG_ERROR( "Invalid slot_id returned: 0x%x\n", slot_id);
      return FALSE;
  }

  return TRUE;
} /* qcril_uim_sap_convert_slot_id_to_slot_type */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_get_active_client

===========================================================================*/
/*!
    @brief
    This function return active client ID

    @return
    Active client id.

*/
/*=========================================================================*/
qcril_uim_sap_clients_type qcril_uim_sap_get_active_client()
{
  return active_client_id;
} /* qcril_uim_sap_get_active_client */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_set_active_client

===========================================================================*/
/*!
    @brief
    This function sets active client ID

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_set_active_client(qcril_uim_sap_clients_type client_id)
{
  active_client_id = client_id;
} /* qcril_uim_sap_set_active_client */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_send_request

===========================================================================*/
/*!
    @brief
    This function sends SAP request to qcril_uim_qcci

    @return
    int.

*/
/*=========================================================================*/
int qcril_uim_sap_send_request
(
  qcril_uim_sap_clients_type   client_id,
  qcril_uim_sap_request_type   request_type,
  int                          message_id,
  void                       * token,
  void                       * sap_req_param_ptr
)
{
  qcril_uim_original_request_type   * original_req_ptr = NULL;
  qcril_instance_id_e_type            instance_id      = qmi_ril_get_process_instance_id();
  qcril_uim_request_type              req_type         = QCRIL_UIM_REQUEST_SAP_REQUEST;
  int                                 qmi_result       = 0;

  /* Create userdata, it is freed in the callback */
  original_req_ptr = qcril_uim_allocate_orig_request(instance_id,
                                                     QCRIL_DEFAULT_MODEM_ID,
                                                     (RIL_Token)token,
                                                     message_id,
                                                     QMI_UIM_SESSION_TYPE_PRI_GW_PROV);
  if (original_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for original_request_ptr!");
    return FALSE;
  }

  /* Also update the client ID, so we can reference the right
     callback in the modem response */
  original_req_ptr->data.sap_info.client_id = client_id;
  original_req_ptr->is_sap = TRUE;

  if (request_type == QCRIL_UIM_SAP_CONNECTION_REQ)
  {
    req_type = QCRIL_UIM_REQUEST_SAP_CONNECTION;
  }

  /* Dispatch request to modem. In case of error, clean up */
  qmi_result = qcril_uim_send_qmi_async_msg(req_type,
                                            sap_req_param_ptr,
                                            original_req_ptr);
  if (qmi_result < 0)
  {
    QCRIL_LOG_ERROR("SAP connection request failed, qmi_result: 0x%x", qmi_result);
    QCRIL_UIM_SAP_FREE_PTR(original_req_ptr);
  }

  return qmi_result;
} /* qcril_uim_sap_send_request */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_send_err_response

===========================================================================*/
/*!
    @brief
    This function queues error response to event queue

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_send_err_response
(
  qcril_uim_sap_clients_type   client_id,
  int                          message_id,
  void                       * token,
  int                          error_code
)
{

  if (client_id >= QCRIL_UIM_SAP_MAX_CLIENTS ||
      qcril_sap_response_api[client_id].resp_handler == NULL)
  {
    QCRIL_LOG_ERROR("Invalid input client: %d", client_id);
    return;
  }

  qcril_sap_response_api[client_id].resp_handler(message_id,
                                                 error_code,
                                                 -1,
                                                 token,
                                                 NULL,
                                                 0);
} /* qcril_uim_sap_send_err_response */


/*=========================================================================

  FUNCTION:  qcril_uim_sap_qmi_handle_sap_connection_resp

===========================================================================*/
/*!
    @brief
    Handles the SAP connection request callback. Based on the response
    received from the modem, respective packed response types are constructed
    and the onRequestComplete is called. This completes the original request
    called by SAP client.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_qmi_handle_sap_connection_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  qcril_uim_original_request_type    * orig_request_ptr = NULL;
  uint8                                client_id        = 0;
  int                                  err_code         = QCRIL_UIM_SAP_SUCCESS;

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

  client_id = orig_request_ptr->data.sap_info.client_id;
  if (client_id >= QCRIL_UIM_SAP_MAX_CLIENTS ||
      qcril_sap_response_api[client_id].resp_handler == NULL)
  {
    QCRIL_LOG_ERROR("Invalid input client: %d", client_id);
    QCRIL_UIM_SAP_FREE_PTR(orig_request_ptr);
    return;
  }

  QCRIL_LOG_DEBUG("handle_sap_request_resp, token: %p, MsgId: 0x%x, qmi_err_code: 0x%x",
                  orig_request_ptr->token,
                  orig_request_ptr->request_id,
                  params_ptr->qmi_rsp_data.qmi_err_code);

  err_code = (params_ptr->qmi_rsp_data.qmi_err_code == 0) ?
                 QCRIL_UIM_SAP_SUCCESS : QCRIL_UIM_SAP_GENERIC_FAILURE;

  qcril_sap_response_api[client_id].resp_handler(orig_request_ptr->request_id,
                                                 err_code,
                                                 params_ptr->qmi_rsp_data.qmi_err_code,
                                                 orig_request_ptr->token,
                                                 NULL,
                                                 0);

  /* Free orig_request_ptr allocated in the request */
  QCRIL_UIM_SAP_FREE_PTR(orig_request_ptr);
} /* qcril_uim_sap_qmi_handle_sap_connection_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_sap_qmi_handle_sap_request_resp

===========================================================================*/
/*!
    @brief
    Handles the SAP request callback. Based on the response
    received from the modem, respective packed response types are constructed
    and the onRequestComplete is called. This completes the original request
    called on the RIL SAP socket.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_qmi_handle_sap_request_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  qcril_uim_original_request_type    * orig_request_ptr = NULL;
  uint8                                client_id        = 0;
  int                                  err_code         = QCRIL_UIM_SAP_SUCCESS;

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

  client_id = orig_request_ptr->data.sap_info.client_id;
  if (client_id >= QCRIL_UIM_SAP_MAX_CLIENTS ||
      qcril_sap_response_api[client_id].resp_handler == NULL)
  {
    QCRIL_LOG_ERROR("Invalid input client: %d", client_id);
    QCRIL_UIM_SAP_FREE_PTR(orig_request_ptr);
    return;
  }

  QCRIL_LOG_DEBUG("handle_sap_request_resp, token: %p, MsgId: 0x%x, qmi_err_code: 0x%x",
                  orig_request_ptr->token,
                  orig_request_ptr->request_id,
                  params_ptr->qmi_rsp_data.qmi_err_code);

  err_code = (params_ptr->qmi_rsp_data.qmi_err_code == 0) ?
               QCRIL_UIM_SAP_SUCCESS : QCRIL_UIM_SAP_GENERIC_FAILURE;

  qcril_sap_response_api[client_id].resp_handler(orig_request_ptr->request_id,
                                                 err_code,
                                                 params_ptr->qmi_rsp_data.qmi_err_code,
                                                 orig_request_ptr->token,
                                                 params_ptr->qmi_rsp_data.rsp_data.sap_response_rsp.sap_response.data_ptr,
                                                 params_ptr->qmi_rsp_data.rsp_data.sap_response_rsp.sap_response.data_len);

  /* Free orig_request_ptr allocated in the request */
  QCRIL_UIM_SAP_FREE_PTR(orig_request_ptr);
} /* qcril_uim_sap_qmi_handle_sap_request_resp */


/*===========================================================================

FUNCTION:  qcril_uim_sap_process_card_status

===========================================================================*/
/*!
    @brief
    This function handles the card state change indication.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_process_card_status
(
  qmi_uim_card_state_type   card_state,
  qmi_uim_card_error_type   err_type,
  uint8                     slot
)
{
  QCRIL_LOG_DEBUG("SAP card state: %d, err_type: %d, slot: %d",
    card_state, err_type, slot);

  if (qmi_ril_get_sim_slot() != slot ||
      active_client_id >= QCRIL_UIM_SAP_MAX_CLIENTS )
  {
    return;
  }

  if (card_state == QMI_UIM_CARD_STATE_PRESENT)
  {
    /* card state is present means sap is disconnectd */
    if (qcril_sap_response_api[active_client_id].ind_handler != NULL)
    {
      qcril_sap_response_api[active_client_id].ind_handler(
        QMI_UIM_SAP_STATE_DISCONNECTED_SUCCESSFULLY);
    }
  }
  else if (card_state == QMI_UIM_CARD_STATE_ERROR)
  {
    switch(err_type)
    {
      case QMI_UIM_CARD_ERROR_POWER_DOWN:
        qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_DOWN;
        break;
      case QMI_UIM_CARD_ERROR_SAP_CONNECTED:
        qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_UP;
        break;
      case QMI_UIM_CARD_ERROR_UNKNOWN_REMOVED:
        qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_ABSENT;
        break;
      default:
        qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_NOT_ACCESSIBLE;
        break;
    }
  }
  else if (card_state == QMI_UIM_CARD_STATE_ABSENT)
  {
    qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_ABSENT;
  }
  else
  {
    qcril_uim_sap_sim_state = QCRIL_CARD_STATUS_NOT_ACCESSIBLE;
  }

  QCRIL_LOG_DEBUG("SAP state: %d", qcril_uim_sap_sim_state);

  if (qcril_sap_response_api[active_client_id].card_status_handler != NULL)
  {
    qcril_sap_response_api[active_client_id].card_status_handler(qcril_uim_sap_sim_state);
  }

} /* qcril_uim_sap_process_card_status */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_ind

===========================================================================*/
/*!
    @brief
    This is the SAP indication callback implementation for the QMI interface.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_ind
(
  qmi_uim_sap_indication_params_type   * ind_data_ptr
)
{
  qcril_instance_id_e_type   instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (ind_data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_data_ptr");
    return;
  }

  if (active_client_id >= QCRIL_UIM_SAP_MAX_CLIENTS ||
      qcril_sap_response_api[active_client_id].ind_handler == NULL)
  {
    QCRIL_LOG_ERROR("Invalid active client : %d", active_client_id);
    return;
  }

  instance_id = qmi_ril_get_process_instance_id();
  if (instance_id >= QCRIL_MAX_INSTANCE_ID ||
      instance_id != qcril_uim_sap_convert_slot_type_to_instance_id(ind_data_ptr->slot))
  {
    QCRIL_LOG_ERROR("Invalid value, instance_id: 0x%x", instance_id);
    return;
  }

  /* Call the handling function for this IND */
  qcril_sap_response_api[active_client_id].ind_handler(ind_data_ptr->sap_state);
} /* qcril_qmi_sap_ind_hdlr */


/*=========================================================================

FUNCTION:  qcril_uim_sap_get_sim_state

===========================================================================*/
/*!
    @brief
    Returns the current SIM state

    @return
    qcril_card_status_e_type
*/
/*=========================================================================*/
qcril_card_status_e_type qcril_uim_sap_get_sim_state()
{
  return qcril_uim_sap_sim_state;
} /* qcril_uim_sap_get_sim_state */


/*=========================================================================

FUNCTION:  qcril_uim_sap_set_sim_state

===========================================================================*/
/*!
    @brief
    Sets the current SIM state

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_set_sim_state(qcril_card_status_e_type state)
{
  qcril_uim_sap_sim_state = state;
} /* qcril_uim_sap_set_sim_state */


/*===========================================================================

  FUNCTION:  qcril_uim_sap_register_client

===========================================================================*/
/*!
    @brief
    This is the SAP client indication, response, err response registeration
    function.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_register_client
(
  qcril_uim_sap_clients_type            client_id,
  qcril_uim_sap_ind_handler             ind_func_ptr,
  qcril_uim_sap_request_resp_handler    req_rsp_fun_ptr,
  qcril_uim_sap_card_status_handler     card_state_func_ptr
)
{
  if (client_id >= QCRIL_UIM_SAP_MAX_CLIENTS)
  {
    QCRIL_LOG_ERROR("qcril_uim_sap_register_client Invalid client id :%d", client_id);
    return;
  }

  qcril_sap_response_api[client_id].ind_handler  = ind_func_ptr;
  qcril_sap_response_api[client_id].resp_handler = req_rsp_fun_ptr;
  qcril_sap_response_api[client_id].card_status_handler  = card_state_func_ptr;
} /* qcril_uim_sap_register_client */
