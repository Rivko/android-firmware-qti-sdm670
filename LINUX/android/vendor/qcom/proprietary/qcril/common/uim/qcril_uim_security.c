/*===========================================================================

  Copyright (c) 2010, 2014 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/sandbox/users/micheleb/ril/qcril_uim_security.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/18   nr      New IMPI SimLock category changes
07/23/15   vr      SIMLOCK Temperory unlock status indication
03/18/15   at      Perso reactivation support
08/07/14   yt      Retrieve perso retries using QMI_UIM_GET_CONFIGURATION
01/13/14   at      Send remaining retries for PIN APIs with empty PIN string
11/06/13   rak     Support change in ME Deperso request params.
01/29/13   yt      Support for third SIM card slot
01/14/13   yt      Fix critical KW errors
10/31/12   at      Explicit query for card status during PIN responses
03/22/12   at      Replacing malloc()/free() with qcril_malloc()/qcril_free()
08/19/11   yt      Fixed Klocwork errors
07/14/11   yt      Support for returning number of perso retries
04/11/11   yt      Support for silent PIN1 verification
03/22/11   at      Support for ril.h v6
01/18/11   at      Removed slot id parameter from all requests
11/12/10   at      Added support for UIM queue implementation
10/06/10   at      Support for handling instance_id passed in requests
09/28/10   at      Changes to pass correct error code for change pin when
                   disabled case
09/21/10   at      Support for UPIN in all PIN related interfaces
09/09/10   at      Changed the way sessions are fetched
06/29/10   at      Changes to support pin verification APIs
05/13/10   at      Fixed compile errors & clean up for merging with mainline
04/13/10   mib     Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "ril.h"
#include "IxErrno.h"
#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_reqlist.h"
#include "qcril_uim.h"
#include "qcril_uim_util.h"
#include "qcril_uim_queue.h"
#include "qcril_uim_restart.h"
#include "qcril_uim_card.h"
#include "qcril_uim_qcci.h"
#include "qcril_uim_security.h"
#include <string.h>
#include <cutils/properties.h>


/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_fetch_retries

===========================================================================*/
/*!
    @brief
    Finds the app related to the passed session type & fetches its remaining
    retry attempts depending upon the request type - PIN1/PIN2/PUK1/PUK2.

    @return
    RIL_E_SUCCESS if successfully found, else QCRIL_UIM_RIL_E_INTERNAL_ERR
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_fetch_retries
(
  int                           request_id,
  qmi_uim_session_type          session_type,
  int                         * num_retries_ptr
)
{
  uint8                  index     = 0;
  uint8                  slot      = 0;
  qmi_uim_app_state_type app_state = QMI_UIM_APP_STATE_UNKNOWN;
  RIL_Errno              err       = QCRIL_UIM_RIL_E_INTERNAL_ERR;

  if (num_retries_ptr == NULL)
  {
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  /* Fetch approriate App index to extract the retries */
  err = qcril_uim_extract_index(&index, &slot, session_type);
  if (err != RIL_E_SUCCESS)
  {
    return err;
  }

  if ((index >= QMI_UIM_MAX_APP_PER_CARD_COUNT) || (slot >= QMI_UIM_MAX_CARD_COUNT))
  {
    QCRIL_LOG_INFO("Invalid indexes for retries: slot_index: 0x%x, app_index: 0x%x",
                   slot, index);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  app_state = qcril_uim.card_status.card[slot].application[index].app_state;

  /* Also check if the retry info is relevant based on the App state */
  if ((app_state == QMI_UIM_APP_STATE_UNKNOWN) || (app_state == QMI_UIM_APP_STATE_DETECTED))
  {
    QCRIL_LOG_ERROR("Invalid App state for retries: 0x%x, slot_index: 0x%x, app_index: 0x%x",
                    app_state, slot, index);
    return RIL_E_INVALID_SIM_STATE;
  }

  /* Fetch retries based on the request type */
  switch (request_id)
  {
    case RIL_REQUEST_ENTER_SIM_PIN:
      *num_retries_ptr = qcril_uim.card_status.card[slot].application[index].pin1_num_retries;
      break;
    case RIL_REQUEST_ENTER_SIM_PIN2:
      *num_retries_ptr = qcril_uim.card_status.card[slot].application[index].pin2_num_retries;
      break;
    case RIL_REQUEST_ENTER_SIM_PUK:
      *num_retries_ptr = qcril_uim.card_status.card[slot].application[index].puk1_num_retries;
      break;
    case RIL_REQUEST_ENTER_SIM_PUK2:
      *num_retries_ptr = qcril_uim.card_status.card[slot].application[index].puk2_num_retries;
      break;
    default:
      QCRIL_LOG_ERROR( "Unsupported Request ID 0x%x\n", request_id);
      return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  QCRIL_LOG_INFO( "Remaining retries: 0x%x, slot: 0x%x, app_index: 0x%x)\n",
                 *num_retries_ptr, slot, index);

  return RIL_E_SUCCESS;
} /* qcril_uim_fetch_retries */


/*===========================================================================

  FUNCTION:  qcril_uim_is_pin_disabled

===========================================================================*/
/*!
    @brief
    Checks if the passed app's PIN1 is disabled

    @return
    TRUE if disabled, else FALSE
*/
/*=========================================================================*/
static boolean qcril_uim_is_pin_disabled
(
  qmi_uim_session_type              session_type
)
{
  uint8   index   = 0;
  uint8   slot    = 0;

  if (qcril_uim_extract_index(&index, &slot, session_type) == RIL_E_SUCCESS)
  {
    if ((index < QMI_UIM_MAX_APP_PER_CARD_COUNT) && (slot < QMI_UIM_MAX_CARD_COUNT))
    {
      /* Check appropriate pin state */
      if (qcril_uim.card_status.card[slot].application[index].univ_pin ==
          QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
      {
        if (qcril_uim.card_status.card[slot].upin_state ==
            QMI_UIM_PIN_STATE_DISABLED)
        {
          return TRUE;
        }
      }
      else if (qcril_uim.card_status.card[slot].application[index].pin1_state ==
               QMI_UIM_PIN_STATE_DISABLED)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
} /* qcril_uim_is_pin_disabled */


/*===========================================================================

  FUNCTION:  qcril_uim_pin_resp

===========================================================================*/
/*!
    @brief
    Handle PIN operation confirmations

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_pin_resp
(
  qcril_uim_callback_params_type             * params_ptr,
  qcril_request_return_type                  * const ret_ptr
)
{
  RIL_Token                                 token;
  RIL_Errno                                 ril_err;
  qmi_uim_pin_id_type                       pin_id;
  int                                       num_retries;
  qcril_uim_original_request_type         * original_request_ptr = NULL;
  qcril_uim_indication_params_type        * card_status_ind_ptr  = NULL;
  qmi_uim_rsp_data_type                     card_status_rsp;
  int                                       qmi_err_code         = 0;

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  /* retreive original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  /* Query card status before sending the response. This is needed to comply to certain
     race condition scenarios. An exmaple is when the get sim status is called soon after the
     PIN response - the PIN status is actually changed but QCRIL hasnt gotten the indication yet */
  card_status_ind_ptr = qcril_malloc(sizeof(qcril_uim_indication_params_type));
  if (card_status_ind_ptr != NULL)
  {
    memset(card_status_ind_ptr, 0x00, sizeof(qcril_uim_indication_params_type));
    memset(&card_status_rsp, 0x00, sizeof(qmi_uim_rsp_data_type));
    qmi_err_code = qcril_qmi_uim_get_card_status(qcril_uim.qmi_handle,
                                                 NULL,
                                                 NULL,
                                                 &card_status_rsp);
    if (qmi_err_code < 0)
    {
      QCRIL_LOG_ERROR( "Get card status failed, qmi_err_code: 0x%x\n", qmi_err_code);
    }
    else
    {
      /* Proceed with updating the global using the common function.
         It takes care of sending the UNSOL before the PIN response */
      card_status_ind_ptr->instance_id = original_request_ptr->instance_id;
      card_status_ind_ptr->modem_id    = original_request_ptr->modem_id;
      card_status_ind_ptr->ind_id      = QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG;
      memcpy(&card_status_ind_ptr->ind_data.status_change_ind,
             &card_status_rsp.rsp_data.get_card_status_rsp.card_status,
             sizeof(qmi_uim_card_status_type));
      qcril_uim_process_status_change_ind(card_status_ind_ptr, ret_ptr);
    }

    /* Clean up the allocated memory */
    qcril_free(card_status_ind_ptr);
    card_status_ind_ptr = NULL;
  }

  /* Token */
  token = (RIL_Token)original_request_ptr->token;

  QCRIL_LOG_DEBUG( "qcril_uim_pin_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  /* Retrieve the PIN id */
  pin_id = QMI_UIM_PIN_ID_PIN1;
  if (original_request_ptr->request_id == RIL_REQUEST_ENTER_SIM_PIN2 ||
      original_request_ptr->request_id == RIL_REQUEST_ENTER_SIM_PUK2 ||
      original_request_ptr->request_id == RIL_REQUEST_CHANGE_SIM_PIN2)
  {
    pin_id = QMI_UIM_PIN_ID_PIN2;
  }

  /* Response code for RIL */
  switch(params_ptr->qmi_rsp_data.qmi_err_code)
  {
    case QMI_ERR_NONE_V01:
      ril_err = RIL_E_SUCCESS;
      break;

    case QMI_ERR_INCORRECT_PIN_V01:
    case QMI_ERR_PIN_PERM_BLOCKED_V01:
      ril_err = RIL_E_PASSWORD_INCORRECT;
      break;

    case QMI_ERR_PIN_BLOCKED_V01:
      ril_err = RIL_E_PASSWORD_INCORRECT;
      if (pin_id == QMI_UIM_PIN_ID_PIN2)
      {
        ril_err = RIL_E_SIM_PUK2;
      }
      break;

    default:
      ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      /* Pass the correct error on "change PIN1 when PIN1 is disabled" case */
      if ((params_ptr->qmi_rsp_data.rsp_id == QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG) &&
          (original_request_ptr->request_id == RIL_REQUEST_CHANGE_SIM_PIN) &&
          qcril_uim_is_pin_disabled(original_request_ptr->session_type))
      {
        ril_err = RIL_E_REQUEST_NOT_SUPPORTED;
        QCRIL_LOG_DEBUG( "PIN disabled, change sim pin not supported, qmi_err_code:0x%x \n",
                         params_ptr->qmi_rsp_data.qmi_err_code);
      }
      break;
  }

  /* Compose number of retries */
  switch(params_ptr->qmi_rsp_data.rsp_id)
  {
    case QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG:
      num_retries = params_ptr->qmi_rsp_data.rsp_data.set_pin_protection_rsp.num_retries;
      break;
    case QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG:
      num_retries = params_ptr->qmi_rsp_data.rsp_data.verify_pin_rsp.num_retries;
      break;
    case QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG:
      num_retries = params_ptr->qmi_rsp_data.rsp_data.unblock_pin_rsp.num_unblock_retries;
      break;
    case QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG:
      num_retries = params_ptr->qmi_rsp_data.rsp_data.change_pin_rsp.num_retries;
      break;
    default:
      /* This case should never happen!! */
      ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      num_retries = 0;
      break;
  }

  if(pin_id == QMI_UIM_PIN_ID_PIN1)
  {
    if (ril_err == RIL_E_SUCCESS)
    {
      qcril_uim_store_encrypted_pin(&params_ptr->qmi_rsp_data,
                                    original_request_ptr->session_type);
    }
    else
    {
      /* Reset PIN1 data if PIN1 operation was unsuccessful */
      qcril_uim_clear_encrypted_pin(original_request_ptr->session_type);
    }
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     (void*)&num_retries,
                     sizeof(int),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_pin_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_deperso_resp

===========================================================================*/
/*!
    @brief
    Handle deperso operation confirmation

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_deperso_resp
(
  qcril_uim_callback_params_type  * params_ptr
)
{
  RIL_Token                             token;
  RIL_Errno                             ril_err;
  int                                   num_retries;
  qcril_uim_original_request_type     * original_request_ptr = NULL;
  qcril_enter_deperso_resp_msg          oemhook_resp;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  /* retreive original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }
  /* Token and response code for RIL */
  token   = (RIL_Token)original_request_ptr->token;
  ril_err = qcril_uim_convert_err_value(params_ptr->qmi_rsp_data.qmi_err_code);

  QCRIL_LOG_DEBUG( "qcril_uim_deperso_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  /* Number of retries: Android RIL currently supports only the possibility
     to deactivate the perso feature. */
  if (original_request_ptr->request_id == QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ)
  {
    memset(&oemhook_resp, 0x00, sizeof(qcril_enter_deperso_resp_msg));
    oemhook_resp.number_of_retries = params_ptr->qmi_rsp_data.rsp_data.depersonalization_rsp.num_retries;
    /* Generate response */
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       (void*)&oemhook_resp,
                       sizeof(qcril_enter_deperso_resp_msg),
                       TRUE,
                       NULL);
  }
  else
  {
    num_retries = params_ptr->qmi_rsp_data.rsp_data.depersonalization_rsp.num_retries;
    /* Generate response */
    qcril_uim_response(original_request_ptr->instance_id,
                       token,
                       ril_err,
                       (void*)&num_retries,
                       sizeof(int),
                       TRUE,
                       NULL);
  }

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;

} /* qcril_uim_deperso_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_perso_resp

===========================================================================*/
/*!
    @brief
    Handle perso operation confirmation

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_perso_resp
(
  qcril_uim_callback_params_type  * params_ptr
)
{
  RIL_Token                             token;
  RIL_Errno                             ril_err;
  int                                   num_retries          = -1;
  qcril_uim_original_request_type     * original_request_ptr = NULL;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return;
  }

  /* retreive original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return;
  }

  /* Update Token */
  token   = (RIL_Token)original_request_ptr->token;

  /* Update response error code */
  switch(params_ptr->qmi_rsp_data.qmi_err_code)
  {
    case 0:  /* QMI_ERR_NONE */
      ril_err = RIL_E_SUCCESS;
      break;

    case 12: /* QMI_ERR_INCORRECT_PIN */
    case 36: /* QMI_ERR_PIN_PERM_BLOCKED */
      ril_err = RIL_E_PASSWORD_INCORRECT;
      break;

    case 35: /* QMI_ERR_PIN_BLOCKED */
      ril_err = RIL_E_PASSWORD_INCORRECT;
      break;

    default:
      ril_err = QCRIL_UIM_RIL_E_INTERNAL_ERR;
      break;
  }

  QCRIL_LOG_DEBUG( "qcril_uim_perso_resp: token=%d qmi_err_code=%d \n",
                    qcril_log_get_token_id(token),
                    params_ptr->qmi_rsp_data.qmi_err_code);

  /* Update number of retries */
  if (params_ptr->qmi_rsp_data.rsp_data.personalization_rsp.num_retries_valid)
  {
    num_retries = params_ptr->qmi_rsp_data.rsp_data.personalization_rsp.num_retries;
  }

  /* Generate response */
  qcril_uim_response(original_request_ptr->instance_id,
                     token,
                     ril_err,
                     (void*)&num_retries,
                     sizeof(int),
                     TRUE,
                     NULL);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_perso_resp */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_unlock_resp

===========================================================================*/
/*!
    @brief
    Handle remote unlock response

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_remote_unlock_resp
(
  qcril_uim_callback_params_type  * params_ptr
)
{
  qcril_uim_original_request_type        * original_request_ptr = NULL;
  qcril_uim_remote_simlock_version         version;
  qcril_uim_remote_simlock_status          status;
  qcril_uim_remote_simlock_response_type   response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_SUCCESS;
  qcril_uim_remote_simlock_operation_type  op_type  = QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_ENCRYPTED_KEY;
  uint8_t                                * data_ptr = NULL;
  uint32_t                                 data_len = 0;
  int                                      timer_value = 0;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  memset(&version, 0x00, sizeof(version));
  memset(&status, 0x00, sizeof(status));

  if(params_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return;
  }

  /* retreive original request */
  original_request_ptr = (qcril_uim_original_request_type*)params_ptr->orig_req_data;
  if(original_request_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    return;
  }

  QCRIL_LOG_DEBUG( "qcril_uim_perso_resp: qmi_err_code=%d \n",
                    params_ptr->qmi_rsp_data.qmi_err_code);

  if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_NONE_V01 ||
      params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_NO_EFFECT_V01)
  {
    response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_SUCCESS;
  }
  else if (params_ptr->qmi_rsp_data.qmi_err_code == QMI_ERR_INCOMPATIBLE_STATE_V01)
  {
    response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_TIMER_EXPIRED;
  }
  else
  {
    response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE;
  }

  switch(original_request_ptr->data.remote_unlock_op)
  {
    case QMI_UIM_REMOTE_UNLOCK_RETRIEVE_ENCRYPTED_KEY:
      data_ptr = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_ptr;
      data_len = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_len;
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_ENCRYPTED_KEY;
      break;
    case QMI_UIM_REMOTE_UNLOCK_PROCESS_SIMLOCK_DATA:
      data_ptr = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_ptr;
      data_len = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_len;
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_PROCESS_SIMLOCK_DATA;
      break;
    case QMI_UIM_REMOTE_UNLOCK_GENERATE_MAC:
      data_ptr = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_ptr;
      data_len = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_len;
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_HMAC;
      break;
    case QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION:
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_GET_MAX_SUPPORTED_VERSION;
      version.majorVersion =
        params_ptr->qmi_rsp_data.rsp_data.remote_simlock_version.major_version;
      version.minorVersion =
        params_ptr->qmi_rsp_data.rsp_data.remote_simlock_version.minor_version;
      break;
    case QMI_UIM_REMOTE_UNLOCK_GENERATE_BLOB:
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_BLOB;
      data_ptr = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_ptr;
      data_len = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_rsp.data_len;
      break;
    case QMI_UIM_REMOTE_UNLOCK_START_TIMER:
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_START_UNLOCK_TIMER;
      timer_value = params_ptr->qmi_rsp_data.rsp_data.remote_unlock_timer;
      break;
    case QMI_UIM_REMOTE_UNLOCK_STOP_TIMER:
      op_type = QCRIL_UIM_REMOTE_SIMLOCK_STOP_UNLOCK_TIMER;
      break;
    default:
      response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE;
      break;
  }

  /* Generate response */
  qcril_uim_remote_simlock_response(original_request_ptr->token,
                                    response,
                                    op_type,
                                    data_ptr,
                                    data_len,
                                    version,
                                    status,
                                    timer_value);

  /* Free memory allocated originally in the request */
  qcril_free(original_request_ptr);
  original_request_ptr = NULL;
} /* qcril_uim_remote_unlock_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_request_enter_pin

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_ENTER_SIM_PIN/RIL_REQUEST_ENTER_SIM_PIN2 requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_enter_pin
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                       res;
  RIL_Errno                                 err;
  uint16                                    aid_size             = 0;
  uint16                                    first_level_df_path  = 0;
  uint8                                     slot                 = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  uint8                                     index                = 0;
  uint8                                     prov_slot            = 0;
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */
  qmi_uim_verify_pin_params_type            pin_params;
  uint8                                  ** in_ptr    = NULL;
  qcril_uim_original_request_type         * original_request_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);
  /* Sanity checks
     in_ptr[0]: PIN value
     in_ptr[1]: AID vaule */
  if(in_ptr == NULL || in_ptr[0] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                       TRUE, "NULL pointer in PIN data");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_enter_pin(%s, %s)\n",
                  in_ptr[0],
                  (in_ptr[1] != NULL) ? (const char *)in_ptr[1] : "NULL" );

  memset(&pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Update the file path based on passed aid pointer */
  aid_size = (in_ptr[1] == NULL) ? 0 : strlen((const char*)in_ptr[1]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  QCRIL_ASSERT(slot < QMI_UIM_MAX_CARD_COUNT);

  /* Extract session type, we need a provisioning session for pin operations */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[1],
                                       first_level_df_path,
                                       &pin_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[1],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &pin_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", pin_params.session_info.session_type);

  /* PIN id */
  switch (params_ptr->event_id)
  {
    case RIL_REQUEST_ENTER_SIM_PIN:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN1;
      break;
    case RIL_REQUEST_ENTER_SIM_PIN2:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;
      break;
    default:
      /* Invalid PIN ID */
      QCRIL_LOG_ERROR( "Invalid Pin from RIL Request ID 0x%x\n", params_ptr->event_id);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE, NULL);
      return;
  }

#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  /* Check if PIN1 is replaced by UPIN for the specified App */
  if ((pin_params.pin_id == QMI_UIM_PIN_ID_PIN1) &&
      (qcril_uim_extract_index(&index,
                               &prov_slot,
                               pin_params.session_info.session_type) == RIL_E_SUCCESS))
  {
    if ((index >= 0) && (index < QMI_UIM_MAX_APP_PER_CARD_COUNT) &&
        (prov_slot == slot) &&
        (prov_slot >= 0) && (prov_slot < QMI_UIM_MAX_CARD_COUNT))
    {
      if (qcril_uim.card_status.card[prov_slot].application[index].univ_pin ==
          QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
      {
        pin_params.pin_id = QMI_UIM_PIN_ID_UPIN;
        QCRIL_LOG_INFO("PIN1 replaced by UPIN for card[%d].application[%d]",
                       prov_slot, index);
      }
    }
  }
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */

  /* Special case for empty PIN string. In this case the PIN retries left is returned
     immediately from the global card status */
  if (strlen((const char*)in_ptr[0]) == 0)
  {
    int num_retries = 0;

    QCRIL_LOG_INFO( "Empty PIN string, fetching retries. Request_id: %d", params_ptr->event_id);

    err = qcril_uim_fetch_retries(params_ptr->event_id,
                                  pin_params.session_info.session_type,
                                  &num_retries);

    /* Generate response */
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, err,
                       (void*)&num_retries, sizeof(int), TRUE, NULL);
    return;
  }

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         pin_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* PIN value */
  pin_params.pin_data.data_ptr = in_ptr[0];
  pin_params.pin_data.data_len = strlen((const char*)in_ptr[0]);

  /* Proceed with verify pin */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "verify pin" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_VERIFY_PIN,
                                     qcril_uim.qmi_handle,
                                     &pin_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
    /* Clean up any original request if allocated */
    if (original_request_ptr)
    {
      qcril_free(original_request_ptr);
      original_request_ptr = NULL;
    }
  }
} /* qcril_uim_request_enter_pin */


/*===========================================================================

  FUNCTION:  qcril_uim_request_enter_puk

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_ENTER_SIM_PUK/RIL_REQUEST_ENTER_SIM_PUK2 requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_enter_puk
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                       res;
  RIL_Errno                                 err;
  uint16                                    aid_size             = 0;
  uint16                                    first_level_df_path  = 0;
  uint8                                     slot                 = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  uint8                                     index                = 0;
  uint8                                     prov_slot            = 0;
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */
  qmi_uim_unblock_pin_params_type           pin_params;
  uint8                                  ** in_ptr    = NULL;
  qcril_uim_original_request_type         * original_request_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);
  /* Sanity checks
     in_ptr[0]: PUK value
     in_ptr[1]: new PIN value
     in_ptr[2]: AID vaule */
  if(in_ptr == NULL || in_ptr[0] == NULL || in_ptr[1] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                       TRUE, "NULL pointer in PIN data");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_enter_puk(%s, %s, %s)\n",
                  in_ptr[0],
                  in_ptr[1],
                  (in_ptr[2] != NULL) ? (const char *)in_ptr[2] : "NULL" );

  memset(&pin_params, 0, sizeof(qmi_uim_unblock_pin_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Update the file path based on passed aid pointer */
  aid_size = (in_ptr[2] == NULL) ? 0 : strlen((const char*)in_ptr[2]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  QCRIL_ASSERT(slot < QMI_UIM_MAX_CARD_COUNT);

  /* Extract session type, we need a provisioning session for pin operations */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[2],
                                       first_level_df_path,
                                       &pin_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[2],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &pin_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", pin_params.session_info.session_type);

  /* PIN id */
  switch (params_ptr->event_id)
  {
    case RIL_REQUEST_ENTER_SIM_PUK:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN1;
      break;
    case RIL_REQUEST_ENTER_SIM_PUK2:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;
      break;
    default:
      /* Invalid PIN ID */
      QCRIL_LOG_ERROR( " Invalid Pin from RIL Request ID 0x%x\n", params_ptr->event_id);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE, NULL);
      return;
  }

#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  /* Check if PIN1 is replaced by UPIN for the specified App */
  if ((pin_params.pin_id == QMI_UIM_PIN_ID_PIN1) &&
      (qcril_uim_extract_index(&index,
                               &prov_slot,
                               pin_params.session_info.session_type) == RIL_E_SUCCESS))
  {
    if ((index >= 0) && (index < QMI_UIM_MAX_APP_PER_CARD_COUNT) &&
        (prov_slot == slot) &&
        (prov_slot >= 0) && (prov_slot < QMI_UIM_MAX_CARD_COUNT))
    {
      if (qcril_uim.card_status.card[prov_slot].application[index].univ_pin ==
          QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
      {
        pin_params.pin_id = QMI_UIM_PIN_ID_UPIN;
        QCRIL_LOG_INFO("PIN1 replaced by UPIN for card[%d].application[%d]",
                       prov_slot, index);
      }
    }
  }
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */

  /* Special case for empty PUK string. In this case the PUK retries left is returned
     immediately from the global card status */
  if (strlen((const char*)in_ptr[0]) == 0)
  {
    int num_retries = 0;

    QCRIL_LOG_INFO( "Empty PUK string, fetching retries. Request_id: %d", params_ptr->event_id);

    err = qcril_uim_fetch_retries(params_ptr->event_id,
                                  pin_params.session_info.session_type,
                                  &num_retries);

    /* Generate response */
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, err,
                       (void*)&num_retries, sizeof(int), TRUE, NULL);
    return;
  }

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         pin_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* PUK value */
  pin_params.puk_data.data_ptr = in_ptr[0];
  pin_params.puk_data.data_len = strlen((const char*)in_ptr[0]);

  /* New PIN value */
  pin_params.new_pin_data.data_ptr = in_ptr[1];
  pin_params.new_pin_data.data_len = strlen((const char*)in_ptr[1]);

  /* Proceed with unblock pin */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "unblock pin" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_UNBLOCK_PIN,
                                     qcril_uim.qmi_handle,
                                     &pin_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
    /* Clean up any original request if allocated */
    if (original_request_ptr)
    {
      qcril_free(original_request_ptr);
      original_request_ptr = NULL;
    }
  }
} /* qcril_uim_request_enter_puk */


/*===========================================================================

  FUNCTION:  qcril_uim_request_change_pin

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_CHANGE_SIM_PIN/RIL_REQUEST_CHANGE_SIM_PIN2 requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_change_pin
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                       res;
  RIL_Errno                                 err;
  uint16                                    aid_size             = 0;
  uint16                                    first_level_df_path  = 0;
  uint8                                     slot                 = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  uint8                                     index                = 0;
  uint8                                     prov_slot            = 0;
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */
  qmi_uim_change_pin_params_type            pin_params;
  uint8                                  ** in_ptr    = NULL;
  qcril_uim_original_request_type         * original_request_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);
  /* Sanity checks
     in_ptr[0]: old PIN value
     in_ptr[1]: new PIN value
     in_ptr[2]: AID vaule */
  if(in_ptr == NULL || in_ptr[0] == NULL || in_ptr[1] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                       TRUE, "NULL pointer in PIN data");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_change_pin(%s, %s, %s)\n",
                  in_ptr[0],
                  in_ptr[1],
                  (in_ptr[2] != NULL) ? (const char *)in_ptr[2] : "NULL" );

  memset(&pin_params, 0, sizeof(qmi_uim_change_pin_params_type));

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Update the file path based on passed aid pointer */
  aid_size = (in_ptr[2] == NULL) ? 0 : strlen((const char*)in_ptr[2]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  QCRIL_ASSERT(slot < QMI_UIM_MAX_CARD_COUNT);

  /* Extract session type, we need a provisioning session for pin operations */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[2],
                                       first_level_df_path,
                                       &pin_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[2],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &pin_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", pin_params.session_info.session_type);

  /* PIN id */
  switch (params_ptr->event_id)
  {
    case RIL_REQUEST_CHANGE_SIM_PIN:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN1;
      break;
    case RIL_REQUEST_CHANGE_SIM_PIN2:
      pin_params.pin_id = QMI_UIM_PIN_ID_PIN2;
      break;
    default:
      /* Invalid PIN ID */
      QCRIL_LOG_ERROR( " Invalid Pin from RIL Request ID 0x%x\n", params_ptr->event_id);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE, NULL);
      return;
  }

#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  /* Check if PIN1 is replaced by UPIN for the specified App */
  if ((pin_params.pin_id == QMI_UIM_PIN_ID_PIN1) &&
      (qcril_uim_extract_index(&index,
                               &prov_slot,
                               pin_params.session_info.session_type) == RIL_E_SUCCESS))
  {
    if ((index >= 0) && (index < QMI_UIM_MAX_APP_PER_CARD_COUNT) &&
        (prov_slot == slot) &&
        (prov_slot >= 0) && (prov_slot < QMI_UIM_MAX_CARD_COUNT))
    {
      if (qcril_uim.card_status.card[prov_slot].application[index].univ_pin ==
          QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
      {
        pin_params.pin_id = QMI_UIM_PIN_ID_UPIN;
        QCRIL_LOG_INFO("PIN1 replaced by UPIN for card[%d].application[%d]",
                       prov_slot, index);
      }
    }
  }
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN*/

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         pin_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* Old PIN value */
  pin_params.old_pin_data.data_ptr = in_ptr[0];
  pin_params.old_pin_data.data_len = strlen((const char*)in_ptr[0]);

  /* New PIN value */
  pin_params.new_pin_data.data_ptr = in_ptr[1];
  pin_params.new_pin_data.data_len = strlen((const char*)in_ptr[1]);

  /* Proceed with change pin */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "change pin" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_CHANGE_PIN,
                                     qcril_uim.qmi_handle,
                                     &pin_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
    /* Clean up any original request if allocated */
    if (original_request_ptr)
    {
      qcril_free(original_request_ptr);
      original_request_ptr = NULL;
    }
  }
} /* qcril_uim_request_change_pin */


/*===========================================================================

  FUNCTION:  qcril_uim_request_set_pin_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_SET_PIN1_STATUS request from
    the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_pin_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                     modem_id  = QCRIL_MAX_MODEM_ID - 1;
  uint8                                  ** in_ptr    = NULL;
  int                                       res;
  RIL_Errno                                 err;
  uint8                                     slot       = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint16                                    aid_size   = 0;
  uint16                                    first_level_df_path = 0;
#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  uint8                                     index      = 0;
  uint8                                     prov_slot  = 0;
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */
  qcril_uim_callback_params_type            callback_params;
  qmi_uim_set_pin_protection_params_type    pin_params;
  qcril_uim_original_request_type         * original_request_ptr = NULL;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&pin_params, 0, sizeof(qmi_uim_set_pin_protection_params_type));
  memset(&callback_params, 0, sizeof(qcril_uim_callback_params_type));

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);

  /* Sanity checks
     in_ptr[0]: facility string code
     in_ptr[1]: lock/unlock
     in_ptr[2]: password
	 in_ptr[3]: service class bit (unused)
     in_ptr[4]: AID value */
  if(in_ptr == NULL || in_ptr[0] == NULL || in_ptr[1] == NULL || in_ptr[2] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0,
                       TRUE, "NULL pointer in PIN data");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_set_pin_status(%s, %s, %s, %s)\n",
                  in_ptr[0],
                  in_ptr[1],
                  in_ptr[2],
                  (in_ptr[4] != NULL) ? (const char *)in_ptr[4] : "NULL" );

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Check facility string */
  if (memcmp(in_ptr[0], "SC", 2) != 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                       TRUE, "unsupported facilty string" );
    return;
  }

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  QCRIL_ASSERT(slot < QMI_UIM_MAX_CARD_COUNT);

  /* Update the file path based on passed aid pointer */
  aid_size = (in_ptr[4] == NULL) ?  0 : strlen((const char *)in_ptr[4]);
  first_level_df_path = (aid_size == 0) ?
                         QCRIL_UIM_FILEID_DF_CDMA : QCRIL_UIM_FILEID_ADF_USIM_CSIM;

  /* Extract session type, we need a provisioning session for pin operations */
  err = qcril_uim_extract_session_type(slot,
                                       (const char *)in_ptr[4],
                                       first_level_df_path,
                                       &pin_params.session_info,
                                       NULL,
                                       0);
  if ((err != RIL_E_SUCCESS) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
      (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
  {
    if (first_level_df_path == QCRIL_UIM_FILEID_DF_CDMA)
    {
      err = qcril_uim_extract_session_type(slot,
                                           (const char *)in_ptr[4],
                                           QCRIL_UIM_FILEID_DF_GSM,
                                           &pin_params.session_info,
                                           NULL,
                                           0);
    }
    if ((err != RIL_E_SUCCESS) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_1) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_2) ||
        (pin_params.session_info.session_type == QMI_UIM_SESSION_TYPE_CARD_SLOT_3))
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                         TRUE, "error extracting session info");
      return;
    }
  }

  QCRIL_LOG_INFO( "Session type found: %d", pin_params.session_info.session_type);

  /* pin parameters */
  if (*in_ptr[1] == '0')
  {
    pin_params.pin_operation = QMI_UIM_PIN_OP_DISABLE;
  }
  else if (*in_ptr[1] == '1')
  {
    pin_params.pin_operation = QMI_UIM_PIN_OP_ENABLE;
  }
  else
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                       TRUE, "invalid input paramter data[1]");
    return;
  }

  pin_params.pin_id = QMI_UIM_PIN_ID_PIN1;
  pin_params.pin_data.data_ptr = (unsigned char*)in_ptr[2];
  pin_params.pin_data.data_len = strlen((const char*)in_ptr[2]);

#ifdef FEATURE_QCRIL_UIM_QMI_UPIN
  /* In case of UPIN, only disable is currently supported */
  if ((pin_params.pin_operation == QMI_UIM_PIN_OP_DISABLE) &&
      (qcril_uim_extract_index(&index,
                               &prov_slot,
                               pin_params.session_info.session_type) == RIL_E_SUCCESS))
  {
    if ((index >= 0) && (index < QMI_UIM_MAX_APP_PER_CARD_COUNT) &&
        (prov_slot == slot) &&
        (prov_slot >= 0) && (prov_slot < QMI_UIM_MAX_CARD_COUNT))
    {
      if (qcril_uim.card_status.card[prov_slot].application[index].univ_pin ==
          QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
      {
        pin_params.pin_id = QMI_UIM_PIN_ID_UPIN;
        QCRIL_LOG_INFO("PIN1 replaced by UPIN for card[%d].application[%d]",
                       prov_slot, index);
      }
    }
  }
#endif /* FEATURE_QCRIL_UIM_QMI_UPIN */

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         pin_params.session_info.session_type);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* Proceed with set pin protection */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "set pin protection" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_SET_PIN,
                                     qcril_uim.qmi_handle,
                                     &pin_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_MODEM_ERR,
                       NULL, 0, TRUE, "error in qmi_uim_set_pin_protection");
    /* Clean up any original request if allocated */
    if (original_request_ptr)
    {
      qcril_free(original_request_ptr);
      original_request_ptr = NULL;
    }
  }
} /* qcril_uim_request_set_pin_status */


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_pin_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_GET_PIN1_STATUS requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_pin_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  uint8                          ** in_ptr      = NULL;
  RIL_Errno                         err;
  uint8                             slot        = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  int                               ret_value   = 0;
  qmi_uim_pin_status_type           pin1_status = QMI_UIM_PIN_STATE_UNKNOWN;

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  /* Parse input pin info */
  in_ptr = (uint8 **)(params_ptr->data);

  /* Sanity checks
     in_ptr[0]: facility string code
     in_ptr[1]: password
     in_ptr[2]: service class bit (unused)
     in_ptr[3]: AID value */
  if(in_ptr == NULL || in_ptr[0] == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0,
                       TRUE, "NULL pointer in PIN data");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "qcril_uim_request_get_pin_status(%s, %s, %s)\n",
                  in_ptr[0],
                  (in_ptr[1] != NULL) ? (const char *)in_ptr[1] : "NULL",
                  (in_ptr[3] != NULL) ? (const char *)in_ptr[3] : "NULL" );

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Check facility string */
  if (memcmp(in_ptr[0], "SC", 2) != 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0,
                       TRUE, "unsupported facilty string" );
    return;
  }

  /* Fetch slot info */
  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
  QCRIL_ASSERT(slot < QMI_UIM_MAX_CARD_COUNT);

  /* Fetch PIN1 or UPIN status for the specified App */
  err = qcril_uim_extract_pin1_status(slot,
                                      (const char *)in_ptr[3],
                                      &pin1_status);
  if (err != RIL_E_SUCCESS)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, err, NULL, 0,
                       TRUE, "error extracting pin1 status");
    return;
  }

  switch(pin1_status)
  {
    case QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED:
    case QMI_UIM_PIN_STATE_ENABLED_VERIFIED:
    case QMI_UIM_PIN_STATE_BLOCKED:
    case QMI_UIM_PIN_STATE_PERM_BLOCKED:
      QCRIL_LOG_INFO( "%s", "PIN enabled\n" );
      ret_value = 1;
      break;
    case QMI_UIM_PIN_STATE_DISABLED:
      QCRIL_LOG_INFO( "%s", "PIN disabled\n" );
      ret_value = 0;
      break;
    default:
      QCRIL_LOG_INFO( "Unknown pin status 0x%x \n", pin1_status);
      err = QCRIL_UIM_RIL_E_INVALID_SIM_STATE;
      break;
  }

  qcril_uim_response(params_ptr->instance_id, params_ptr->t, err, &ret_value, sizeof(int),
                     TRUE, NULL);
} /* qcril_uim_request_get_pin_status */


/*===========================================================================

  FUNCTION:  qcril_uim_convert_perso_feature_id_to_perso_feature_status

===========================================================================*/
/*!
    @brief
    Converts personalization feature ID to personalization feature status

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_convert_perso_feature_id_to_perso_feature_status
(
  qmi_uim_perso_feature_id_type      perso_feature_id,
  qmi_uim_perso_feature_status_type *perso_feature_status_ptr
)
{
  boolean status = TRUE;

  if(perso_feature_status_ptr == NULL)
  {
    return FALSE;
  }

  switch(perso_feature_id)
  {
    case QMI_UIM_PERSO_FEATURE_GW_NW:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_NW;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_NS:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_NS;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_SP:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_SP;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_CP:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_CP;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_SIM:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_SIM;
      break;
    case QMI_UIM_PERSO_FEATURE_1X_NW1:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_NW1;
      break;
    case QMI_UIM_PERSO_FEATURE_1X_NW2:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_NW2;
      break;
    case QMI_UIM_PERSO_FEATURE_1X_HRPD:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_HRPD;
      break;
    case QMI_UIM_PERSO_FEATURE_1X_SP:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_SP;
      break;
    case QMI_UIM_PERSO_FEATURE_1X_CP:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_CP;
      break;
     case QMI_UIM_PERSO_FEATURE_1X_RUIM:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_1X_RUIM;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_SPN:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_SPN;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_SP_EHPLMN;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_ICCID:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_ICCID;
      break;
    case QMI_UIM_PERSO_FEATURE_GW_IMPI:
      *perso_feature_status_ptr = QMI_UIM_PERSO_FEATURE_STATUS_GW_IMPI;
      break;
    default:
      status = FALSE;
      break;
  }

  return status;
} /* qcril_uim_convert_perso_feature_id_to_perso_feature_status */


/*===========================================================================

  FUNCTION:  qcril_uim_get_perso_retries

===========================================================================*/
/*!
    @brief
    Gets the personalization retries for the feature by sending
    QMI_UIM_GET_CONFIGURATION command to modem.

    @return
    None
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_get_perso_retries
(
  qmi_uim_perso_feature_id_type    perso_feature,
  int                            * num_retries_ptr
)
{
  int                                    ret    = 0;
  uint8                                  i      = 0;
  RIL_Errno                              errval = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  qmi_uim_get_configuration_params_type  get_config_params;
  qmi_uim_rsp_data_type                  rsp_data;

  if (num_retries_ptr == NULL)
  {
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  *num_retries_ptr = 0;

  memset(&get_config_params, 0x00, sizeof(get_config_params));
  memset(&rsp_data, 0x00, sizeof(rsp_data));

  get_config_params.perso_status = QMI_UIM_TRUE;
  ret = qcril_qmi_uim_get_configuration(qcril_uim.qmi_handle,
                                        &get_config_params,
                                        NULL,
                                        NULL,
                                        &rsp_data);

  if (ret != 0)
  {
    return qcril_uim_convert_err_value(ret);
  }

  for (i = 0;
       i < rsp_data.rsp_data.get_configuration_rsp.perso_status_len && i < QMI_UIM_MAX_PERSO_FEATURES;
       i++)
  {
    if (rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == perso_feature)
    {
      *num_retries_ptr = rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
      errval = RIL_E_SUCCESS;
      break;
    }
  }

  return errval;
} /* qcril_uim_get_perso_retries */


/*===========================================================================

  FUNCTION:  qcril_uim_request_enter_perso_key

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION or
    QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ requests from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_enter_perso_key
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_modem_id_e_type                   modem_id = QCRIL_MAX_MODEM_ID - 1;
  int                                     res;
  qmi_uim_depersonalization_params_type   perso_params;
  qcril_enter_deperso_req_msg             oemhook_req;
  qcril_enter_deperso_resp_msg            oemhook_resp;
  const char *                            depersonalization_code = NULL;
  qcril_uim_original_request_type       * original_request_ptr = NULL;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if(params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    QCRIL_ASSERT(0);
    return;
  }

  if (params_ptr->event_id == QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ)
  {
    uint16 offset   = 0;

    if ((params_ptr->data == NULL) || (params_ptr->datalen <= 2))
    {
      QCRIL_LOG_ERROR("Invalid data pointer or length, cannot process, datalen: 0x%x",
                      params_ptr->datalen);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                         NULL, 0, TRUE, "Invalid input");
      return;
    }

    memset(&oemhook_req, 0x00, sizeof(qcril_enter_deperso_req_msg));
    oemhook_req.deperso_type = (char *)params_ptr->data;

    /* Empty string for deperso_type is not supported */
    if (strlen(oemhook_req.deperso_type) == 0)
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                         NULL, 0, TRUE, "Empty deperso_type not supported");
      return;
    }

    QCRIL_LOG_INFO( "deperso_type: (%s)\n", oemhook_req.deperso_type );

    /* Check length. Note that deperso code at the very least has to be empty string */
    if (params_ptr->datalen <= (strlen(oemhook_req.deperso_type)+ 1))
    {
      QCRIL_LOG_ERROR("datalen mismatch, total datalen: %d", params_ptr->datalen);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INTERNAL_ERR,
                         NULL, 0, TRUE, "Invalid length");
      return;
    }

    /* Update offset to point to CK. Note that empty Deperso code is supported */
    offset += strlen(oemhook_req.deperso_type) + 1;
    oemhook_req.deperso_code = (char *)params_ptr->data + offset;

    QCRIL_LOG_INFO( "deperso_code: (%s)\n", oemhook_req.deperso_code );

    /* Also update the CK pointer */
    depersonalization_code = (const char *) oemhook_req.deperso_code;

    /* Update the feature type */
    switch (atoi(oemhook_req.deperso_type))
    {
      case RIL_PERSOSUBSTATE_SIM_NETWORK:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_NW;
        break;
      case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_NS;
        break;
      case RIL_PERSOSUBSTATE_SIM_CORPORATE:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_CP;
        break;
      case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_SP;
        break;
      case RIL_PERSOSUBSTATE_SIM_SPN:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_SPN;
        break;
      case RIL_PERSOSUBSTATE_SIM_SP_EHPLMN:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN;
        break;
      case RIL_PERSOSUBSTATE_SIM_ICCID:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_ICCID;
        break;
      case RIL_PERSOSUBSTATE_SIM_IMPI:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_IMPI;
        break;
      case RIL_PERSOSUBSTATE_SIM_SIM:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_SIM;
        break;
      case RIL_PERSOSUBSTATE_RUIM_NETWORK1:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_NW1;
        break;
      case RIL_PERSOSUBSTATE_RUIM_NETWORK2:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_NW2;
        break;
      case RIL_PERSOSUBSTATE_RUIM_HRPD:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_HRPD;
        break;
      case RIL_PERSOSUBSTATE_RUIM_CORPORATE:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_CP;
        break;
      case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_SP;
        break;
      case RIL_PERSOSUBSTATE_RUIM_RUIM:
        perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_1X_RUIM;
        break;
      default:
        /* Invalid feature ID */
        QCRIL_LOG_ERROR( " Invalid perso feature from RIL Request ID %s\n",
                         oemhook_req.deperso_type);
        qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                           QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0, TRUE, NULL);
        return;
    }
  }
  else if (params_ptr->event_id == RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION)
  {
    depersonalization_code = ((const char **) params_ptr->data)[0];
    if(depersonalization_code == NULL)
    {
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INVALID_ARGUMENTS, NULL, 0,
                         TRUE, "NULL pointer in perso data");
      return;
    }

    /* Update the feature type */
    perso_params.perso_feature = QMI_UIM_PERSO_FEATURE_GW_NW;
  }
  else
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, "Unsupported request_id");
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Return the number of remaining retries if perso key is empty */
  if(strlen(depersonalization_code) == 0)
  {
    uint8                                   slot                 = 0;
    int                                     num_retries          = 0;
    uint8                                   i                    = 0;
    boolean                                 perso_convert_status = FALSE;
    qmi_uim_perso_feature_status_type perso_feature_status       = QMI_UIM_PERSO_FEATURE_STATUS_UNKNOWN;

    slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);
    if( slot >= QMI_UIM_MAX_CARD_COUNT )
    {
      QCRIL_LOG_ERROR("Invalid slot 0x%x for instance id 0x%x",
                       slot, params_ptr->instance_id);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0, TRUE, NULL);
      return;
    }

    memset(&oemhook_resp, 0x00, sizeof(qcril_enter_deperso_resp_msg));

    perso_convert_status = qcril_uim_convert_perso_feature_id_to_perso_feature_status(perso_params.perso_feature,
                                                                                      &perso_feature_status);
    if(perso_convert_status)
    {
      for(i = 0; i < qcril_uim.card_status.card[slot].num_app; i++)
      {
        if((i != QCRIL_UIM_INVALID_APP_INDEX_VALUE) &&
           (qcril_uim.card_status.card[slot].application[i].app_state == QMI_UIM_APP_STATE_PERSO) &&
           (qcril_uim.card_status.card[slot].application[i].perso_feature == perso_feature_status) &&
           (qcril_uim.card_status.card[slot].application[i].perso_state == QMI_UIM_PERSO_STATE_CODE_REQUIRED))
        {
           if (params_ptr->event_id == QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ)
           {
             oemhook_resp.number_of_retries = qcril_uim.card_status.card[slot].application[i].perso_retries;
             qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                                (void*)&oemhook_resp, sizeof(qcril_enter_deperso_resp_msg),
                                TRUE, "sending num perso retries");
           }
           else
           {
             num_retries = qcril_uim.card_status.card[slot].application[i].perso_retries;
             qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                                (void*)&num_retries, sizeof(int), TRUE, "sending num perso retries");
           }
           return;
        }
      }
    }

    /* If perso retries are not available in QCRIL, retrieve them from modem
       using Get Configuration command. */
    if (qcril_uim_get_perso_retries(perso_params.perso_feature, &num_retries) == RIL_E_SUCCESS)
    {
      if (params_ptr->event_id == QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_REQ)
      {
        oemhook_resp.number_of_retries = num_retries;
        qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                           (void*)&oemhook_resp, sizeof(qcril_enter_deperso_resp_msg),
                           TRUE, "sending num perso retries");
      }
      else
      {
        qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                           (void*)&num_retries, sizeof(int), TRUE, "sending num perso retries");
      }
      return;
    }

    QCRIL_LOG_ERROR("Unable to find app that requires key for perso feature 0x%x",
                     perso_params.perso_feature);
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_INTERNAL_ERR, NULL, 0, TRUE, NULL);
    return;
  }

  /* Perso operation */
  perso_params.perso_operation = QMI_UIM_PERSO_OP_DEACTIVATE;

  /* New PIN value */
  perso_params.ck_data.data_ptr = (unsigned char *)depersonalization_code;
  perso_params.ck_data.data_len = strlen(depersonalization_code);

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         modem_id,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* Proceed with depersonalization */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "depersonalization" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_DEPERSO,
                                     qcril_uim.qmi_handle,
                                     &perso_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
    /* Clean up any original request if allocated */
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }
} /* qcril_uim_request_enter_perso_key */


/*===========================================================================

  FUNCTION:  qcril_uim_request_perso_reactivate

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_PERSONALIZATION_REACTIVATE_REQ requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_perso_reactivate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  int                                    res                  = 0;
  qcril_uim_original_request_type      * original_request_ptr = NULL;
  qmi_uim_personalization_params_type    perso_params;
  RIL_PersonalizationReq                 reactivate_req;
  uint8                                  offset = 0;
  uint32                                 perso_type           = 0;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity checks */
  if (params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    return;
  }

  if (params_ptr->data == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INTERNAL_ERR,
                       NULL, 0, TRUE, "NULL request pointer");
    return;
  }

  /* For OEMHook messages, client handlers need to parse the 
     request byte stream themselves */
  memset(&reactivate_req, 0x00, sizeof(RIL_PersonalizationReq));
  reactivate_req.controlKey = (char *)params_ptr->data;
  if (strlen(reactivate_req.controlKey) == 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INVALID_ARGUMENTS,
                       NULL, 0, TRUE, "Empty controlKey not supported");
    return;
  }

  offset = strlen(reactivate_req.controlKey) + 1;
  reactivate_req.persoType = *((uint8*)params_ptr->data + offset);

  perso_type = (uint32)reactivate_req.persoType;

  QCRIL_LOG_INFO( "qcril_uim_request_perso_reactivate for persoType (0x%x)\n",
                  reactivate_req.persoType );

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  memset(&perso_params, 0x00, sizeof(qmi_uim_personalization_params_type));

  /* Reactivate Perso feature */
  switch (perso_type)
  {
    case RIL_PERSOSUBSTATE_SIM_NETWORK:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_NW;
      break;
    case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_NS;
      break;
    case RIL_PERSOSUBSTATE_SIM_CORPORATE:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_CP;
      break;
    case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_SP;
      break;
    case RIL_PERSOSUBSTATE_SIM_SPN:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_SPN;
      break;
    case RIL_PERSOSUBSTATE_SIM_SP_EHPLMN:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN;
      break;
    case RIL_PERSOSUBSTATE_SIM_ICCID:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_ICCID;
      break;
    case RIL_PERSOSUBSTATE_SIM_IMPI:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_IMPI;
      break;
    case RIL_PERSOSUBSTATE_SIM_SIM:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_GW_SIM;
      break;
    case RIL_PERSOSUBSTATE_RUIM_NETWORK1:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_NW1;
      break;
    case RIL_PERSOSUBSTATE_RUIM_NETWORK2:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_NW2;
      break;
    case RIL_PERSOSUBSTATE_RUIM_HRPD:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_HRPD;
      break;
    case RIL_PERSOSUBSTATE_RUIM_CORPORATE:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_CP;
      break;
    case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_SP;
      break;
    case RIL_PERSOSUBSTATE_RUIM_RUIM:
      perso_params.reactivate_feature = QMI_UIM_PERSO_FEATURE_1X_RUIM;
      break;
    default:
      /* Invalid feature ID */
      QCRIL_LOG_ERROR( " Unsupported perso feature from RIL Request: 0x%x\n", reactivate_req.persoType);
      qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                         RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0, TRUE, NULL);
      return;
  }

  /* CK value */
  perso_params.ck_data.data_ptr = (unsigned char *)reactivate_req.controlKey;
  perso_params.ck_data.data_len = strlen(reactivate_req.controlKey);

  /* Allocate original request */
  original_request_ptr = qcril_uim_allocate_orig_request(params_ptr->instance_id,
                                                         QCRIL_MAX_MODEM_ID - 1,
                                                         (RIL_Token)params_ptr->t,
                                                         params_ptr->event_id,
                                                         0);
  if (original_request_ptr == NULL)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_NO_MEMORY, NULL, 0, TRUE,
                       "error allocating memory for original_request_ptr");
    return;
  }

  /* Proceed with depersonalization */
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "personalization" );
  res = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_PERSO,
                                     qcril_uim.qmi_handle,
                                     &perso_params,
                                     qmi_uim_callback,
                                     (void*)original_request_ptr);
  if (res < 0)
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t,
                       QCRIL_UIM_RIL_E_MODEM_ERR, NULL, 0, TRUE, NULL);
    /* Clean up any original request if allocated */
    qcril_free(original_request_ptr);
    original_request_ptr = NULL;
  }
} /* qcril_uim_request_perso_reactivate */


/*===========================================================================

  FUNCTION:  qcril_uim_request_perso_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_PERSONALIZATION_STATUS_REQ requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_perso_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  int                                    ret      = 0;
  uint8                                  i        = 0;
  qmi_uim_rsp_data_type                  rsp_data;
  RIL_PersonalizationStatusResp          perso_status_resp;
  qmi_uim_get_configuration_params_type  get_config_params;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity checks */
  if (params_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process response");
    return;
  }

  /* Add entry to ReqList */
  QCRIL_UIM_ADD_ENTRY_TO_REQUEST_LIST(params_ptr);

  /* Check which perso features are enabled & return the number of retries */
  memset(&rsp_data, 0x00, sizeof(qmi_uim_rsp_data_type));
  memset(&perso_status_resp, 0x00, sizeof(RIL_PersonalizationStatusResp));
  memset(&get_config_params, 0x00, sizeof(qmi_uim_get_configuration_params_type));

  get_config_params.perso_status = QMI_UIM_TRUE;
  ret = qcril_qmi_uim_get_configuration(qcril_uim.qmi_handle,
                                        &get_config_params,
                                        NULL,
                                        NULL,
                                        &rsp_data);

  if ((ret != 0) || (!rsp_data.rsp_data.get_configuration_rsp.perso_status_valid))
  {
    qcril_uim_response(params_ptr->instance_id, params_ptr->t, QCRIL_UIM_RIL_E_INTERNAL_ERR,
                       (void*)&perso_status_resp, sizeof(RIL_PersonalizationStatusResp),
                       TRUE, "Unable to retrieve status");
    return ;
  }

  for (i = 0;
       i < rsp_data.rsp_data.get_configuration_rsp.perso_status_len && i < QMI_UIM_MAX_PERSO_FEATURES;
       i++)
  {
    switch (rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature)
    {
      case QMI_UIM_PERSO_FEATURE_GW_NW:
        perso_status_resp.has_gwNWPersoStatus = TRUE;
        perso_status_resp.gwNWPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_NS:
        perso_status_resp.has_gwNWSubsetPersoStatus = TRUE;
        perso_status_resp.gwNWSubsetPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_SP:
        perso_status_resp.has_gwSPPersoStatus = TRUE;
        perso_status_resp.gwSPPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_CP:
        perso_status_resp.has_gwCPPersoStatus = TRUE;
        perso_status_resp.gwCPPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_SPN:
        perso_status_resp.has_gwSPNPersoStatus = TRUE;
        perso_status_resp.gwSPNPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN:
        perso_status_resp.has_gwSPEhplmnPersoStatus = TRUE;
        perso_status_resp.gwSPEhplmnPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_ICCID:
        perso_status_resp.has_gwICCIDPersoStatus = TRUE;
        perso_status_resp.gwICCIDPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_IMPI:
        perso_status_resp.has_gwIMPIPersoStatus = TRUE;
        perso_status_resp.gwIMPIPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_GW_SIM:
        perso_status_resp.has_gwSIMPersoStatus = TRUE;
        perso_status_resp.gwSIMPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_1X_NW1:
        perso_status_resp.has_gw1xNWType1PersoStatus = TRUE;
        perso_status_resp.gw1xNWType1PersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_1X_NW2:
        perso_status_resp.has_gw1xNWType2PersoStatus = TRUE;
        perso_status_resp.gw1xNWType2PersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      case QMI_UIM_PERSO_FEATURE_1X_RUIM:
        perso_status_resp.has_gw1xRUIMPersoStatus = TRUE;
        perso_status_resp.gw1xRUIMPersoStatus.verifyAttempts =
          rsp_data.rsp_data.get_configuration_rsp.perso_status[i].verify_left;
        break;

      default:
        QCRIL_LOG_INFO("Unsupported perso feature, skipping: 0x%x",
                         rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature);
        break;
    }
  }

  qcril_uim_response(params_ptr->instance_id, params_ptr->t, RIL_E_SUCCESS,
                       (void*)&perso_status_resp, sizeof(RIL_PersonalizationStatusResp), TRUE,
                     "sending num perso retries");
} /* qcril_uim_request_perso_status */


/*=========================================================================

  FUNCTION:  qcril_uim_request_remote_sim_lock_unlock

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_REMOTE_SIM_LOCK_UNLOCK request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_remote_sim_lock_unlock
(
  void                                     * token_ptr,
  qcril_uim_remote_simlock_operation_type    op_type,
  const uint8_t                            * simlock_data_ptr,
  uint32_t                                   simlock_len
)
{
  qcril_uim_original_request_type      * original_req_ptr      = NULL;
  qmi_uim_remote_unlock_params_type      remote_unlock_params;
  qcril_uim_remote_simlock_version       version;
  qcril_uim_remote_simlock_status        status;
  int                                    ret                   = -1;

  QCRIL_LOG_INFO("qcril_uim_remote_lock_unlock operation type (0x%x)\n",
                  op_type);

  memset(&remote_unlock_params, 0x00, sizeof(remote_unlock_params));
  memset(&version, 0x00, sizeof(version));
  memset(&status, 0x00, sizeof(status));

  switch(op_type)
  {
    case QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_ENCRYPTED_KEY:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_RETRIEVE_ENCRYPTED_KEY;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_PROCESS_SIMLOCK_DATA:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_PROCESS_SIMLOCK_DATA;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_HMAC:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_GENERATE_MAC;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_GET_MAX_SUPPORTED_VERSION:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_GET_MAX_SUPPORTED_VERSION;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_BLOB:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_GENERATE_BLOB;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_START_UNLOCK_TIMER:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_START_TIMER;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_STOP_UNLOCK_TIMER:
      remote_unlock_params.operation = QMI_UIM_REMOTE_UNLOCK_STOP_TIMER;
      break;
    case QCRIL_UIM_REMOTE_SIMLOCK_GET_STATUS:
      {
        qmi_uim_get_configuration_params_type  get_config_params;
        qmi_uim_rsp_data_type                  rsp_data;
        qcril_uim_remote_simlock_response_type response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE;

        memset(&get_config_params, 0x00, sizeof(get_config_params));
        memset(&rsp_data, 0x00, sizeof(rsp_data));

        get_config_params.perso_status = QMI_UIM_TRUE;
        ret = qcril_qmi_uim_get_configuration(qcril_uim.qmi_handle,
                                              &get_config_params,
                                              NULL,
                                              NULL,
                                              &rsp_data);
        if (ret == 0)
        {
          uint32  i = 0;

          status.status = QCRIL_UIM_REMOTE_SIMLOCK_STATE_PERMANENT_UNLOCK;
          if (rsp_data.rsp_data.get_configuration_rsp.perso_status_valid)
          {
            for(i = 0; i < rsp_data.rsp_data.get_configuration_rsp.perso_status_len &&
                       i < QMI_UIM_MAX_PERSO_FEATURES; i++)
            {
              if(rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SP ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_NW ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_NS ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_CP ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SPN ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_ICCID ||
                 rsp_data.rsp_data.get_configuration_rsp.perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_IMPI)
              {
                status.status = QCRIL_UIM_REMOTE_SIMLOCK_STATE_LOCKED;
                break;
              }
            }
          }
          if (rsp_data.rsp_data.get_configuration_rsp.temp_perso_status_len != 0)
          {
            for(i = 0; i < rsp_data.rsp_data.get_configuration_rsp.temp_perso_status_len &&
                       i < QMI_UIM_MAX_PERSO_FEATURES; i++)
            {
              if(rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SP ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_NW ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_NS ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_CP ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SPN ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_SP_EHPLMN ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_ICCID ||
                 rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].feature == QMI_UIM_PERSO_FEATURE_GW_IMPI)
              {
                status.status = QCRIL_UIM_REMOTE_SIMLOCK_STATE_TEMPERORY_UNLOCK;
                status.unlockTime = rsp_data.rsp_data.get_configuration_rsp.temp_perso_status[i].unlock_time_left;
                break;
              }
            }
          }
          response = QCRIL_UIM_REMOTE_SIMLOCK_RESP_SUCCESS;
        }
        qcril_uim_remote_simlock_response(token_ptr,
                                          response,
                                          op_type,
                                          NULL,
                                          0,
                                          version,
                                          status,
                                          0);
      }
      return;
    default:
      qcril_uim_remote_simlock_response(token_ptr,
                                        QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE,
                                        op_type,
                                        NULL,
                                        0,
                                        version,
                                        status,
                                        0);
      return;
  }

  if (simlock_len > 0 && simlock_data_ptr != NULL)
  {
    remote_unlock_params.simlock_data.data_ptr = qcril_malloc(sizeof(uint8) * simlock_len);

    if (remote_unlock_params.simlock_data.data_ptr == NULL)
    {
      qcril_uim_remote_simlock_response(token_ptr,
                                        QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE,
                                        op_type,
                                        NULL,
                                        0,
                                        version,
                                        status,
                                        0);
      return;
    }

    memcpy(remote_unlock_params.simlock_data.data_ptr,
           simlock_data_ptr,
           simlock_len);
    remote_unlock_params.simlock_data.data_len = simlock_len;

  }

  /* Allocate original request */
  original_req_ptr = qcril_uim_allocate_orig_request(QCRIL_DEFAULT_INSTANCE_ID,
                                                     QCRIL_MAX_MODEM_ID - 1,
                                                     token_ptr,
                                                     0,
                                                     0);
  if (original_req_ptr == NULL)
  {
    qcril_uim_remote_simlock_response(token_ptr,
                                      QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE,
                                      op_type,
                                      NULL,
                                      0,
                                      version,
				      status,
                                      0);
    qcril_free(remote_unlock_params.simlock_data.data_ptr);
    return;
  }

  original_req_ptr->data.remote_unlock_op = remote_unlock_params.operation;
  QCRIL_LOG_QMI( QCRIL_MAX_MODEM_ID - 1, "qmi_uim_service", "remote SIM unlock" );
  ret = qcril_uim_queue_send_request(QCRIL_UIM_REQUEST_REMOTE_UNLOCK,
                                     qcril_uim.qmi_handle,
                                     &remote_unlock_params,
                                     qmi_uim_callback,
                                     (void*)original_req_ptr);
  if (ret < 0)
  {
    qcril_uim_remote_simlock_response(token_ptr,
                                      QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE,
                                      op_type,
                                      NULL,
                                      0,
                                      version,
                                      status,
                                      0);
    /* Clean up any original request if allocated */
    qcril_free(original_req_ptr);
    original_req_ptr = NULL;
  }
  qcril_free(remote_unlock_params.simlock_data.data_ptr);
} /* qcril_uim_request_remote_sim_lock_unlock */


/*===========================================================================

  FUNCTION:  qcril_uim_process_simlock_temp_unlock_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI SIMLOCK temporary unlock indication.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_simlock_temp_unlock_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
)
{
  uint8               slot = 0;
  char                temp_buff[100] = {0};

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if ((ind_param_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid inputs, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  slot = qcril_uim_instance_id_to_slot(ind_param_ptr->instance_id);
  if( slot >= QMI_UIM_MAX_CARD_COUNT )
  {
    QCRIL_LOG_ERROR("Invalid slot 0x%x for instance id 0x%x",
                     slot, ind_param_ptr->instance_id);
    return;
  }

  (void)snprintf(temp_buff, 99, "%s_%d", QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS, slot);

  QCRIL_LOG_INFO( "RSU status:0x%x and received status:0x%x \n",
           qcril_uim.temp_unlock_status,
           ind_param_ptr->ind_data.simlock_temp_unlock_ind.temporay_unlock_status[slot]);

  if (!ind_param_ptr->ind_data.simlock_temp_unlock_ind.temporay_unlock_status[slot] &&
      qcril_uim.temp_unlock_status)
  {
    /* Send OEMHook unsolicited response to framework */
    qcril_hook_unsol_response(slot,
                              (int) QCRIL_EVT_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED,
                              NULL,
                              0);
    QCRIL_LOG_INFO( "QCRIL_EVT_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED is sent \n");
  }
  if (ind_param_ptr->ind_data.simlock_temp_unlock_ind.temporay_unlock_status[slot] !=
      qcril_uim.temp_unlock_status)
  {
    qcril_uim.temp_unlock_status =
      ind_param_ptr->ind_data.simlock_temp_unlock_ind.temporay_unlock_status[slot];
    switch(qcril_uim.temp_unlock_status)
    {
      case TRUE:
        property_set(temp_buff, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_TRUE);
        break;
      case FALSE:
        property_set(temp_buff, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_FALSE);
        break;
    }

  }
} /* qcril_uim_process_simlock_temp_unlock_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_check_and_send_temp_unlock_expiry_ind

===========================================================================*/
/*!
    @brief
    Function to check the temperory unlock status and send expiry indication

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_check_and_send_temp_unlock_expiry_ind
(
  void
)
{

  int                                    ret    = 0;
  qmi_uim_get_configuration_params_type  get_config_params;
  qmi_uim_rsp_data_type                  rsp_data;
  uint8                                  slot   =  (uint8)qmi_ril_get_sim_slot();
  char                                   temp_buff[100] = {0};

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if( slot >= QMI_UIM_MAX_CARD_COUNT )
  {
    QCRIL_LOG_ERROR("Invalid slot 0x%x ", slot);
    return;
  }

  (void)snprintf(temp_buff, 99, "%s_%d", QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS, slot);
  memset(&get_config_params, 0x00, sizeof(get_config_params));
  memset(&rsp_data, 0x00, sizeof(rsp_data));

  get_config_params.perso_status = QMI_UIM_TRUE;
  ret = qcril_qmi_uim_get_configuration(qcril_uim.qmi_handle,
                                        &get_config_params,
                                        NULL,
                                        NULL,
                                        &rsp_data);

  if (ret != 0)
  {
    return;
  }

  QCRIL_LOG_INFO( "RSU status:0x%x and received status:0x%x \n",
           qcril_uim.temp_unlock_status,
           rsp_data.rsp_data.get_configuration_rsp.temp_unlock_status[slot]);

  if (!rsp_data.rsp_data.get_configuration_rsp.temp_unlock_status[slot] &&
      qcril_uim.temp_unlock_status)
  {
    /* Send OEMHook unsolicited response to framework */
    qcril_hook_unsol_response(slot,
                              (int) QCRIL_EVT_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED,
                              NULL,
                              0);
    QCRIL_LOG_INFO( "QCRIL_EVT_HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED is sent \n");
    qcril_uim.temp_unlock_status = FALSE;
    property_set(temp_buff, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_FALSE);
  }
  else if (!qcril_uim.temp_unlock_status &&
           rsp_data.rsp_data.get_configuration_rsp.temp_unlock_status[slot])
  {
    qcril_uim.temp_unlock_status = TRUE;
    property_set(temp_buff, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_TRUE);
  }
} /* qcril_uim_check_and_send_temp_unlock_expiry_ind */

#endif /* defined (FEATURE_QCRIL_UIM_QMI) */
