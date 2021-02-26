/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "modules/uim/qcril_uim_legacy.h"
#include "framework/Log.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/UimOemHook.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "qcril_uim_util.h"
#include "qcril_uim_slot_mapping.h"

/*===========================================================================

                               GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_get_physical_slot_mapping_resp

===========================================================================*/
/*!
    @brief
    Processes the response for physical slot mapping command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_physical_slot_mapping_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err                  = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qcril_uim_original_request_type * original_request_ptr     = NULL;
  uint8                             slot_id                  = 0;
  RIL_PhySlotMap                    logical_phy_slot_mapping;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  memset(&logical_phy_slot_mapping, 0, sizeof(logical_phy_slot_mapping));

  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_get_physical_slot_mapping_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_uim_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  logical_phy_slot_mapping.array_len = params_ptr->qmi_rsp_data.rsp_data.phy_slot_mapping_rsp.num_slots;

  if(logical_phy_slot_mapping.array_len <= QMI_UIM_MAX_CARD_COUNT)
  {
    for (slot_id = 0; slot_id < logical_phy_slot_mapping.array_len; slot_id++)
    {
      switch(params_ptr->qmi_rsp_data.rsp_data.phy_slot_mapping_rsp.logical_slot[slot_id])
      {
        case QMI_UIM_PHY_SLOT_1:
          logical_phy_slot_mapping.slot_map[slot_id] = 0;
          break;
        case QMI_UIM_PHY_SLOT_2:
          logical_phy_slot_mapping.slot_map[slot_id] = 1;
          break;
        case QMI_UIM_PHY_SLOT_3:
          logical_phy_slot_mapping.slot_map[slot_id] = 2;
          break;
        default:
          ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
          break;
      }
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Send the response */
  if(ril_err == RIL_E_SUCCESS)
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &logical_phy_slot_mapping,
                       sizeof(logical_phy_slot_mapping),
                       TRUE,
                       NULL);
  }
  else
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       NULL,
                       0,
                       TRUE,
                       NULL);
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_get_physical_slot_mapping_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_get_slots_status_resp

===========================================================================*/
/*!
    @brief
    Processes the response for get slots status.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_slots_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token                    = NULL;
  RIL_Errno                         ril_err                  = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qcril_uim_original_request_type * original_request_ptr     = NULL;
  uint8                             slot_id                  = 0;
  RIL_SlotsStatus                   slots_status;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  memset(&slots_status, 0, sizeof(slots_status));
  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_get_slots_status_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_uim_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  slots_status.no_of_slots = params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.no_of_slots;

  if(slots_status.no_of_slots <= QMI_UIM_MAX_SLOT_COUNT)
  {
    for (slot_id = 0; slot_id < slots_status.no_of_slots; slot_id++)
    {
      slots_status.slot_status[slot_id].card_state =
        (Ril_CardPresenceState)params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.slot_status[slot_id].card_state;
      slots_status.slot_status[slot_id].slot_state =
        (Ril_SlotState)params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.slot_status[slot_id].slot_state;
      slots_status.slot_status[slot_id].iccid_len =
        params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.slot_status[slot_id].iccid_len;

      if(slots_status.slot_status[slot_id].iccid_len > QMI_UIM_MAX_ICCID_LEN)
      {
        slots_status.slot_status[slot_id].iccid_len = QMI_UIM_MAX_ICCID_LEN;
      }
      memcpy(slots_status.slot_status[slot_id].iccid,
             params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.slot_status[slot_id].iccid,
             QMI_UIM_MAX_ICCID_LEN);

      if(slots_status.slot_status[slot_id].slot_state == RIL_PHYSICAL_SLOT_STATE_ACTIVE)
      {
        switch(params_ptr->qmi_rsp_data.rsp_data.slots_status_rsp.slot_status[slot_id].logical_slot)
        {
          case QMI_UIM_SLOT_1:
            slots_status.slot_status[slot_id].logical_slot = 0;
            break;
          case QMI_UIM_SLOT_2:
            slots_status.slot_status[slot_id].logical_slot = 1;
            break;
          case QMI_UIM_SLOT_3:
            slots_status.slot_status[slot_id].logical_slot = 2;
            break;
          default:
            QCRIL_LOG_ERROR("%s", "Invalid input, incorrect logical slot id ");
            QCRIL_ASSERT(0);
            return;
        }
      }
    }
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Send the response */
  if(ril_err == RIL_E_SUCCESS)
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       &slots_status,
                       sizeof(slots_status),
                       TRUE,
                       NULL);
  }
  else
  {
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       NULL,
                       0,
                       TRUE,
                       NULL);
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_get_slots_status_resp */


/*=========================================================================

  FUNCTION:  qcril_uim_switch_slot_resp

===========================================================================*/
/*!
    @brief
    Processes the response for switch physical slot command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_switch_slot_resp
(
  const qcril_uim_callback_params_type * const params_ptr
)
{
  RIL_Token                         token;
  RIL_Errno                         ril_err;
  qcril_uim_original_request_type * original_request_ptr = NULL;

  /* Sanity checks */
  if (params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Retrieve original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if (original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    return;
  }

  if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_UIM_SERVICE_ERR_NO_EFFECT)
  {
    ril_err = RIL_E_SUCCESS;
  }
  else
  {
    ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);
  }

  token   = original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_switch_slot_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_uim_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code );

  /* Send the response actual response will be in payload */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     RIL_E_SUCCESS,
                     &ril_err,
                     sizeof(ril_err),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_switch_slot_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_request_get_physical_slot_mapping

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ request from the
    framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_physical_slot_mapping
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type * original_req_ptr = NULL;
  uint8                             dummy            = 0;
  RIL_Errno                         ril_err          = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if ((params_ptr == NULL) ||
      (ret_ptr    == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     params_ptr->modem_id,
                                                     params_ptr->t,
                                                     params_ptr->event_id_android,
                                                     QMI_UIM_SESSION_TYPE_PRI_GW_PROV);
  if (original_req_ptr != NULL)
  {
    if (qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GET_PHYSICAL_SLOT_MAPPING,
                                     (void *)&dummy,
                                     original_req_ptr) >= 0)
    {
      return;
    }
    ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  /* Send the response */
  qcril_uim_response( params_ptr->instance_id,
                      params_ptr->t,
                      ril_err,
                      NULL,
                      0,
                      TRUE,
                      NULL );

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
} /* qcril_uim_request_get_physical_slot_mapping */


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_slots_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_UIM_REQUEST_GET_SLOT_STATUS request from the
    framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_slots_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type * original_req_ptr = NULL;
  uint8                             dummy            = 0;
  RIL_Errno                         ril_err          = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                     params_ptr->modem_id,
                                                     params_ptr->t,
                                                     params_ptr->event_id_android,
                                                     QMI_UIM_SESSION_TYPE_PRI_GW_PROV);
  if (original_req_ptr != NULL)
  {
    if (qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GET_SLOTS_STATUS,
                                     (void *)&dummy,
                                     original_req_ptr) >= 0)
    {
      return;
    }
    ril_err = QCRIL_UIM_RIL_E_MODEM_ERR;
  }
  else
  {
    ril_err = QCRIL_UIM_RIL_E_NO_MEMORY;
  }

  /* Send the response */
  qcril_uim_response( params_ptr->instance_id,
                      params_ptr->t,
                      ril_err,
                      NULL,
                      0,
                      TRUE,
                      NULL );

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
}/* qcril_uim_request_get_slots_status */


/*===========================================================================

  FUNCTION:  qcril_uim_request_switch_slot

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SWITCH_SLOT_REQ request from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_switch_slot
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_original_request_type   * original_req_ptr  = NULL;
  qmi_uim_physical_slot_mapping_type  params;
  uint8                               slot_id           = 0;
  RIL_PhySlotMap                    * logic_phy_mapping = NULL;
  RIL_Errno                           ril_error         = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if ((params_ptr == NULL) ||
      (ret_ptr    == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  if(params_ptr->data != NULL)
  {
    logic_phy_mapping = (RIL_PhySlotMap *)params_ptr->data;
    params.num_slots  = logic_phy_mapping->array_len;
    if(params.num_slots <= QMI_UIM_MAX_CARD_COUNT)
    {
      for(slot_id = 0; slot_id < params.num_slots; slot_id++)
      {
        /* Find slot info */
        switch(logic_phy_mapping->slot_map[slot_id])
        {
          case 0:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_1;
            break;
          case 1:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_2;
            break;
          case 2:
            params.logical_slot[slot_id] = QMI_UIM_PHY_SLOT_3;
            break;
          default:
            /* Send the response, actual response will be in payload */
            ril_error = QCRIL_UIM_RIL_E_INVALID_ARGUMENTS;
            qcril_uim_response( params_ptr->instance_id,
                                params_ptr->t,
                                RIL_E_SUCCESS,
                                &ril_error,
                                sizeof(ril_error),
                                TRUE,
                                NULL );
            return;
        }
      }

      /* Allocate original request */
      original_req_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         params_ptr->modem_id,
                                                         params_ptr->t,
                                                         params_ptr->event_id_android,
                                                         QMI_UIM_SESSION_TYPE_PRI_GW_PROV);
      if (original_req_ptr != NULL)
      {
        if (qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_SWITCH_SLOT_EXT,
                                         (void *)&params,
                                         original_req_ptr) >= 0)
        {
          return;
        }
        ril_error = QCRIL_UIM_RIL_E_MODEM_ERR;
      }
      else
      {
        ril_error = QCRIL_UIM_RIL_E_NO_MEMORY;
      }
    }
  }

  /* Send the response, actual response will be in payload */
  qcril_uim_response( params_ptr->instance_id,
                      params_ptr->t,
                      RIL_E_SUCCESS,
                      &ril_error,
                      sizeof(ril_error),
                      TRUE,
                      NULL );

  /* Clean up any original request if allocated */
  if (original_req_ptr)
  {
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
} /* qcril_uim_request_switch_slot */


/*===========================================================================

  FUNCTION:  qcril_uim_process_slot_status_change_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI slot status changed indication. Based
    on the indication received, if needed, it updates the global slot status.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_slot_status_change_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
)
{
  uint8                             i                          = 0;
  qmi_uim_slots_status_type       * slot_status_change_ind_ptr = NULL;
  uint8                             j                          = 0;
  RIL_SlotsStatus                   status;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if ((ind_param_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid inputs, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  slot_status_change_ind_ptr = (qmi_uim_slots_status_type*)&ind_param_ptr->ind_data.slot_status_change_ind;
  if (slot_status_change_ind_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, NULL status_change_ind_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  /* Below event is not supported in RIL UT */
  /* Slot status change notification is sent with oem hook message with slot id and
        slot status.  */
  QCRIL_LOG_INFO("QCRIL_EVT_HOOK_UNSOL_SLOT_STATUS_CHANGE_IND");
  status.no_of_slots = slot_status_change_ind_ptr->no_of_slots;

  for (i = 0; i < status.no_of_slots && i < QMI_UIM_MAX_SLOT_COUNT; i++)
  {
    status.slot_status[i].card_state   =
        (Ril_CardPresenceState)slot_status_change_ind_ptr->slot_status[i].card_state;
    status.slot_status[i].slot_state   =
        (Ril_SlotState)slot_status_change_ind_ptr->slot_status[i].slot_state;
    status.slot_status[i].iccid_len    = slot_status_change_ind_ptr->slot_status[i].iccid_len;

    if(status.slot_status[i].iccid_len > QMI_UIM_MAX_ICCID_LEN)
    {
      status.slot_status[i].iccid_len = QMI_UIM_MAX_ICCID_LEN;
    }
    memcpy(status.slot_status[i].iccid,
           slot_status_change_ind_ptr->slot_status[i].iccid,
           QMI_UIM_MAX_ICCID_LEN);

    if(status.slot_status[i].slot_state == RIL_PHYSICAL_SLOT_STATE_ACTIVE)
    {
      switch(slot_status_change_ind_ptr->slot_status[i].logical_slot)
      {
        case QMI_UIM_SLOT_1:
          status.slot_status[i].logical_slot = 0;
          break;
        case QMI_UIM_SLOT_2:
          status.slot_status[i].logical_slot = 1;
          break;
        case QMI_UIM_SLOT_3:
          status.slot_status[i].logical_slot = 2;
          break;
        default:
          QCRIL_LOG_ERROR("%s", "Invalid input, incorrect logical slot id ");
          QCRIL_ASSERT(0);
          return;
      }
    }

    QCRIL_LOG_INFO("LOGICAL_ID-0x%x, CARD STATE 0x%x, ACTIVITY_STATUS-0x%x, iccid_len = 0x%x",
                                              status.slot_status[i].logical_slot,
                                              status.slot_status[i].card_state,
                                              status.slot_status[i].slot_state,
                                              status.slot_status[i].iccid_len);
    for(j=0;j< status.slot_status[i].iccid_len;j++)
    {
      QCRIL_LOG_INFO("iccid 0x%x = 0x%x",j,status.slot_status[i].iccid[j]);
    }
  }

  qcril_hook_unsol_response(ind_param_ptr->instance_id,
                            QCRIL_EVT_HOOK_UNSOL_SLOT_STATUS_CHANGE_IND,
                            (char *)&status,
                            sizeof(status));
} /* qcril_uim_process_slot_status_change_ind */


#endif /* defined (FEATURE_QCRIL_UIM_QMI) */

