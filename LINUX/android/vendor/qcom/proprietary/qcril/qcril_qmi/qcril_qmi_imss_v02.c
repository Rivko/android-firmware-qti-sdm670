/******************************************************************************
  @file    qcril_qmi_imss_v02.c
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

  ---------------------------------------------------------------------------

  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#include <cutils/properties.h>
#include "qcril_qmi_client.h"
#include "qcril_qmi_imss.h"
#include "qcril_qmi_imss_internal.h"
#include "qcril_qmi_imss_v02.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "qcril_qmi_ims_radio_agent.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_ims_misc.h"
#include "qcril_qmi_npb_utils.h"
#include "qcril_qmi_radio_config_imss.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_imsa.h"
#include "qcril_arb.h"
#include "qcril_data.h"
#include <cutils/properties.h>

extern boolean feature_voice_dom_pref_on_toggle_ims_cap;
extern boolean feature_disabled_modem_req;
void qcril_qmi_imss_ims_service_enable_config_ind_hdlr(void *ind_data_ptr);
void qcril_qmi_imss_handle_ims_reg_change_event_resp
(
 const qcril_request_params_type *const params_ptr,
 RIL_Errno ril_err
);

uint8_t qcril_qmi_imss_convert_ril_to_imss_wfc_status_v02
(
  qcril_ims_setting_wfc_status_type wfc_status
);

ims_settings_call_mode_option_enum_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_preference_v02
(
  qcril_ims_setting_wfc_preference_type wfc_preference
);

qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
(
  uint8_t wfc_status
);

qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
(
  ims_settings_call_mode_option_enum_v01 wfc_preference
);

void qcril_qmi_imss_send_request_not_supported
(
   const qcril_request_params_type *const params_ptr
);

//===========================================================================
// qcril_qmi_imss_request_set_ims_registration_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_registration_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  ims_Registration* ims_in_data_ptr = NULL;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type reqlist_entry;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  uint8_t is_state_change_in_progress = FALSE;
  uint8_t ims_service_enabled = FALSE;

  QCRIL_NOTUSED( ret_ptr );

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    ims_in_data_ptr = (ims_Registration *)params_ptr->data;
    QCRIL_LOG_INFO("has_state: %d, state: %d", ims_in_data_ptr->has_state,
           ims_in_data_ptr->state);

    if(feature_disabled_modem_req)
    {
      QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
      res = RIL_E_SUCCESS;
      break;
    }

    if(ims_in_data_ptr->has_state != TRUE)
    {
      QCRIL_LOG_ERROR("Invalid parameters: state is not present; Send failure");
      res = RIL_E_GENERIC_FAILURE;
      break;
    }

    qcril_reqlist_default_entry( params_ptr->t, params_ptr->event_id,
        QCRIL_DEFAULT_MODEM_ID, QCRIL_REQ_AWAITING_CALLBACK,
        QCRIL_EVT_NONE, NULL, &reqlist_entry );

    if (qcril_reqlist_new( instance_id, &reqlist_entry ) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to Add into Req list; Send failure");
      res = RIL_E_GENERIC_FAILURE;
      break;
    }

    qcril_qmi_imss_info_lock();
    is_state_change_in_progress = qcril_qmi_ims_cached_info.imss_state_change_requested;
    qcril_qmi_imss_info_unlock();

    if (is_state_change_in_progress)
    {
      // IMS State change already in progress; Send failure.
      QCRIL_LOG_INFO("IMS State change already in progress; Send failure");
      res = RIL_E_GENERIC_FAILURE;
      break;
    }

    qcril_qmi_imss_info_lock();
    qcril_qmi_ims_cached_info.imss_state_change_requested = TRUE;
    qcril_qmi_ims_cached_info.imss_new_reg_state = ims_in_data_ptr->state;
    qcril_qmi_imss_info_unlock();

    QCRIL_LOG_INFO("Need to change voice domain pref? %s",
        feature_voice_dom_pref_on_toggle_ims_cap ? "Yes" : "No");
    if (feature_voice_dom_pref_on_toggle_ims_cap)
    {
      // IMS OFF -> ON Case
      // Set the Voice Domain Pref to PS_PREF before enabling IMS
      // Call QMI NAS API to set the voice domain pref
      if (ims_in_data_ptr->state == ims_Registration_RegState_REGISTERED)
      {
        res = qcril_qmi_nas_set_voice_domain_preference(
            NAS_VOICE_DOMAIN_PREF_PS_PREF_V01,
            reqlist_entry.req_id);
        QCRIL_LOG_INFO("qcril_qmi_nas_set_voice_domain_preference res = %d", res);
        break;
      }
    }

    // feature_voice_dom_pref_on_toggle_ims_cap not enabled or
    // IMS ON -> OFF Case
    // Call QMI IMSS API to set the ims test mode.
    ims_service_enabled =
          (ims_Registration_RegState_REGISTERED == ims_in_data_ptr->state) ? TRUE : FALSE;
    res = qcril_qmi_imss_set_ims_service_enabled(ims_service_enabled,
          reqlist_entry.req_id);
  }while(FALSE);

  if (!is_state_change_in_progress && (RIL_E_SUCCESS != res))
  {
    qcril_qmi_imss_info_lock();
    qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
    qcril_qmi_imss_info_unlock();
  }

  if(feature_disabled_modem_req || RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(instance_id,
            params_ptr->t, params_ptr->event_id, res);
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_registration_v02

//===========================================================================
// qcril_qmi_imss_set_ims_service_enabled
//===========================================================================
RIL_Errno qcril_qmi_imss_set_ims_service_enabled(uint8_t ims_service_enabled, uint16 req_id)
{
  IxErrnoType qmi_client_error;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  uint32 user_data;

  ims_settings_set_ims_service_enable_config_req_msg_v01 qmi_req;
  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *qmi_resp = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("ims_service_enabled = %s",
          ims_service_enabled ? "TRUE" : "FALSE");

  memset(&qmi_req, 0, sizeof(qmi_req));
  qmi_req.ims_service_enabled_valid = TRUE;
  qmi_req.ims_service_enabled = ims_service_enabled;
  qmi_resp = qcril_malloc(sizeof(*qmi_resp));

  if (NULL != qmi_resp)
  {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error = qcril_qmi_client_send_msg_async (
            QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
            &qmi_req,
            sizeof(qmi_req),
            qmi_resp,
            sizeof(*qmi_resp),
            (void*)(uintptr_t)user_data );
    QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error );

    if (E_SUCCESS == qmi_client_error)
    {
      res = RIL_E_SUCCESS;
    }
  }
  QCRIL_LOG_FUNC_RETURN();

  return res;
} // qcril_qmi_imss_set_ims_service_enabled

//===========================================================================
// qcril_qmi_imss_request_set_ims_srv_status_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_srv_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  ims_Info* ims_in_data_ptr = NULL;
  IxErrnoType qmi_client_error = E_SUCCESS;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  uint32 user_data;
  qcril_reqlist_public_type reqlist_entry;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  ims_settings_set_ims_service_enable_config_req_msg_v01 qmi_req;
  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *qmi_resp = NULL;

  uint8_t enabled = FALSE;

  QCRIL_NOTUSED( ret_ptr );

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (feature_disabled_modem_req)
    {
      QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
      res = RIL_E_SUCCESS;
      break;
    }

    ims_in_data_ptr = (ims_Info *)params_ptr->data;

    qcril_reqlist_default_entry(
        params_ptr->t,
        params_ptr->event_id,
        QCRIL_DEFAULT_MODEM_ID,
        QCRIL_REQ_AWAITING_CALLBACK,
        QCRIL_EVT_NONE,
        NULL,
        &reqlist_entry );

    if (qcril_reqlist_new(instance_id, &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to Add into Req list; Send failure");
      res = RIL_E_GENERIC_FAILURE;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));

    QCRIL_LOG_INFO( "has_calltype: %d, calltype: %d",
        ims_in_data_ptr->has_callType, ims_in_data_ptr->callType );

    if (ims_in_data_ptr->has_callType &&
        ims_in_data_ptr->accTechStatus.arg &&
        ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0] && // only one acctechstatus will be used
        ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->has_status)
    {
      QCRIL_LOG_INFO( "has_status: %d, status: %d",
          ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->has_status,
          ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->status);

      if (ims_StatusType_STATUS_DISABLED ==
          ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->status)
      {
        enabled = FALSE;
      }
      else
      {
        enabled = TRUE;
      }

      if (ims_CallType_CALL_TYPE_VOICE == ims_in_data_ptr->callType &&
          ((((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
            ims_RadioTechType_RADIO_TECH_WIFI) ||
           (((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
            ims_RadioTechType_RADIO_TECH_IWLAN)))
      {
        qmi_req.wifi_calling_enabled_valid = TRUE;
        qmi_req.wifi_calling_enabled = enabled;
        QCRIL_LOG_INFO("Sending wifi call setting through ims_service_enable, value: %d", enabled);
      }
      else if (ims_CallType_CALL_TYPE_VOICE == ims_in_data_ptr->callType &&
          (((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
           ims_RadioTechType_RADIO_TECH_LTE))
      {
        qmi_req.volte_enabled_valid = TRUE;
        qmi_req.volte_enabled = enabled;
      }
      else if (ims_CallType_CALL_TYPE_VT == ims_in_data_ptr->callType)
      {
        qmi_req.videotelephony_enabled_valid = TRUE;
        qmi_req.videotelephony_enabled = enabled;
      }
      else
      {
        QCRIL_LOG_ERROR("request call_type/networkMode is not a valid value");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("request misses some necessary information");
      break;
    }

    qmi_resp = qcril_malloc(sizeof(*qmi_resp));
    if (NULL == qmi_resp)
    {
      QCRIL_LOG_ERROR("qcril_malloc failed");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id );

    qmi_client_error = qcril_qmi_client_send_msg_async(
            QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
            &qmi_req, sizeof(qmi_req),
            qmi_resp, sizeof(*qmi_resp),
            (void*)(uintptr_t)user_data );
    QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error );

    if (E_SUCCESS == qmi_client_error)
    {
      res = RIL_E_SUCCESS;
    }
  } while(FALSE);

  if (feature_disabled_modem_req || RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(instance_id, params_ptr->t,
        params_ptr->event_id, res);
    if (qmi_resp != NULL)
    {
       qcril_free( qmi_resp );
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_srv_status_v02

//===========================================================================
// qcril_qmi_imss_request_query_vt_call_quality_v02
//===========================================================================
void qcril_qmi_imss_request_query_vt_call_quality_v02
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  QCRIL_NOTUSED(ret_ptr);
  qcril_qmi_imss_send_request_not_supported(params_ptr);
} // qcril_qmi_imss_request_query_vt_call_quality_v02

//===========================================================================
// qcril_qmi_imss_request_set_vt_call_quality_v02
//===========================================================================
void qcril_qmi_imss_request_set_vt_call_quality_v02
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  QCRIL_NOTUSED(ret_ptr);
  qcril_qmi_imss_send_request_not_supported(params_ptr);
} // qcril_qmi_imss_request_set_vt_call_quality_v02

//===========================================================================
// qcril_qmi_imss_request_get_wifi_calling_status_v02
//===========================================================================
void qcril_qmi_imss_request_get_wifi_calling_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  QCRIL_NOTUSED(ret_ptr);
  qcril_qmi_imss_send_request_not_supported(params_ptr);
} // qcril_qmi_imss_request_get_wifi_calling_status_v02

//===========================================================================
// qcril_qmi_imss_request_set_wifi_calling_status_v02
//===========================================================================
void qcril_qmi_imss_request_set_wifi_calling_status_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  QCRIL_NOTUSED(ret_ptr);
  qcril_qmi_imss_send_request_not_supported(params_ptr);
}  // qcril_qmi_imss_request_set_wifi_calling_status_v02

//===========================================================================
// @function qcril_qmi_imss_send_request_not_supported
// @brief Function used to send not supported response
//===========================================================================
void qcril_qmi_imss_send_request_not_supported
(
   const qcril_request_params_type *const params_ptr
)
{
  qcril_request_resp_params_type resp;

  if(NULL != params_ptr)
  {
    qcril_default_request_resp_params(QCRIL_DEFAULT_INSTANCE_ID,
      params_ptr->t, params_ptr->event_id, RIL_E_REQUEST_NOT_SUPPORTED, &resp );
    qcril_send_request_response(&resp);
  }
  else
  {
    QCRIL_LOG_ERROR("param_ptr is null");
  }
}

//===========================================================================
// qcril_qmi_imss_request_set_ims_config_v02
//===========================================================================
void qcril_qmi_imss_request_set_ims_config_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  uint32 user_data;
  qcril_reqlist_public_type reqlist_entry;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  ims_ConfigMsg* ims_config_msg_ptr;

  /* Radio config mapping related types.
   Needed for processing the request*/
  qcril_qmi_radio_config_params_type config_params;
  qcril_qmi_radio_config_item_value_type config_item_type;
  qcril_qmi_radio_config_handler_type* req_handler = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
          QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qcril_qmi_radio_config_req_data_type store_item;

  /* To Convert the int value to bool  */
  bool bool_value;

  QCRIL_NOTUSED(ret_ptr);
  QCRIL_LOG_FUNC_ENTRY();
  do
  {
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("NULL data in Params");
      break;
    }

    if(feature_disabled_modem_req)
    {
      QCRIL_LOG_INFO("Modem IMS config is disabled. Respond to Telephony with success");
      qcril_send_empty_payload_request_response(instance_id,
              params_ptr->t,
              params_ptr->event_id,
              RIL_E_SUCCESS);
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS;
      break;
    }
    /* Read the input params and get the radio config item
     * mapping to the request config item
     */
    ims_config_msg_ptr = (ims_ConfigMsg *) params_ptr->data;
    if(!ims_config_msg_ptr->has_item)
    {
      QCRIL_LOG_ERROR(
              "Config Item is not present in the request,",
              "has_item is false..sending failure");
      break;
    }

    config_params.config_item =
          qcril_qmi_ims_map_ims_config_to_radio_config_item(ims_config_msg_ptr->item);

    if(config_params.config_item == QCRIL_QMI_RADIO_CONFIG_INVALID)
    {
      QCRIL_LOG_ERROR("Invalid config item: %d. Doesnt exist in radio config",
              ims_config_msg_ptr->item);
      break;
    }

    memset(&store_item, 0x0, sizeof(store_item));
    store_item.config_item = config_params.config_item;

    qcril_reqlist_default_entry(params_ptr->t, params_ptr->event_id, QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK, QCRIL_EVT_NONE, NULL, &reqlist_entry);

    /* Store the config item in the req_list entry
     * which will be used while sending back the response
     */
    reqlist_entry.req_datalen = sizeof(store_item);
    reqlist_entry.req_data = (void *) &store_item;

    if( qcril_reqlist_new(instance_id, &reqlist_entry) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR("Cannot create a new reqlist entry");
      break;
    }
    QCRIL_LOG_INFO("Created a new reqlist entry");
    user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
            QCRIL_DEFAULT_MODEM_ID, reqlist_entry.req_id );

    QCRIL_LOG_INFO("processing request - config item: %s",
            qcril_qmi_radio_config_get_item_log_str(config_params.config_item));

    /* Get the value type of the config item and
     * assign data to config params appropriately
     */
    config_item_type = qcril_qmi_radio_config_get_item_value_type(config_params.config_item);

    // If item value type is boolean and input params has int val
    // As per imsproto for now the items coming in Boolean are placed
    // in intValue as there is no corresponding API in ImsConfig.java
    if((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN)
        && (ims_config_msg_ptr->has_intValue))
    {
      QCRIL_LOG_INFO("Config item received is of boolean type in intValue, value: %d",
              ims_config_msg_ptr->intValue);

      bool_value = ims_config_msg_ptr->intValue ? TRUE : FALSE;
      config_params.config_item_value_len = sizeof(bool_value);
      config_params.config_item_value = (void *)(&bool_value);
    }
    // If item value type is int and input params has int val
    else if((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT)
            && (ims_config_msg_ptr->has_intValue))
    {
      QCRIL_LOG_INFO("Config item received is an integer, value: %d",
              ims_config_msg_ptr->intValue);

      config_params.config_item_value_len = sizeof(ims_config_msg_ptr->intValue);
      config_params.config_item_value = (void *)(&ims_config_msg_ptr->intValue);
    }
    // If item value type is string and input params has string val
    else if((config_item_type == QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING)
             && (ims_config_msg_ptr->stringValue.arg != NULL))
    {
      config_params.config_item_value_len = strlen(ims_config_msg_ptr->stringValue.arg);
      QCRIL_LOG_INFO("Config item received is a string, value: %s",
              ims_config_msg_ptr->stringValue.arg);
      config_params.config_item_value = (void *) ims_config_msg_ptr->stringValue.arg;
    }
    // If none of them then the input params are invalid
    else
    {
      QCRIL_LOG_ERROR("..invalid parameters for the config items value");
      radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_INVALID_PARAMETER;
      break;
    }
    config_params.extra_data_len = sizeof(user_data);
    config_params.extra_data = (void *)(uintptr_t)user_data;
    config_params.config_item_value_type = config_item_type;

    /* Get the set request handler and call it with config params */
    req_handler = qcril_qmi_radio_config_find_set_config_req_handler(
            config_params.config_item);

    if(req_handler == NULL)
    {
      QCRIL_LOG_ERROR("NULL req handler for the item");
      break;
    }
    radio_config_error = (req_handler)(&config_params);
    QCRIL_LOG_INFO("Returned from req handler with radio_config_error: %d",
            radio_config_error);
  }while(FALSE);

  /* If calling the req handler fails due to above reasons
   * Send Failure response back
   */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
  {
    QCRIL_LOG_ERROR("radio config error: %d..sending empty response", radio_config_error);
    qcril_qmi_imss_set_ims_config_log_and_send_response(params_ptr->t,
                                                        NULL,
                                                        radio_config_error,
                                                        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_config_v02

//===========================================================================
// qcril_qmi_imss_request_get_ims_config_v02
//===========================================================================
void qcril_qmi_imss_request_get_ims_config_v02
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  uint32 user_data;
  qcril_reqlist_public_type reqlist_entry;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;

  ims_ConfigMsg* ims_config_msg_ptr;

  /* Radio config mapping related types. */
  qcril_qmi_radio_config_params_type config_params;
  qcril_qmi_radio_config_handler_type* req_handler = NULL;
  qcril_qmi_radio_config_error_type radio_config_error =
    QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
  qcril_qmi_radio_config_req_data_type store_item;

  QCRIL_NOTUSED(ret_ptr);
  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if(params_ptr->data == NULL)
    {
      QCRIL_LOG_ERROR("NULL data in Params");
      break;
    }
    /* Read the input params and get the radio config item
     * mapping to the request config item
     */
    ims_config_msg_ptr = (ims_ConfigMsg *) params_ptr->data;
    if(!ims_config_msg_ptr->has_item)
    {
      QCRIL_LOG_ERROR("Config Item is not present in the request, has_item is false..sending failure");
      break;
    }
    config_params.config_item =
         qcril_qmi_ims_map_ims_config_to_radio_config_item(ims_config_msg_ptr->item);
    if(config_params.config_item == QCRIL_QMI_RADIO_CONFIG_INVALID)
    {
      QCRIL_LOG_ERROR("Invalid config item: %d. Doesnt exist in radio config",
                      ims_config_msg_ptr->item);
      break;
    }

    memset(&store_item, 0x0, sizeof(store_item));
    store_item.config_item = config_params.config_item;

    qcril_reqlist_default_entry(params_ptr->t, params_ptr->event_id, QCRIL_DEFAULT_MODEM_ID,
                                QCRIL_REQ_AWAITING_CALLBACK, QCRIL_EVT_NONE, NULL, &reqlist_entry);

    /* Store the config item in the req_list entry
     * which will be used while sending back the response
     */
    reqlist_entry.req_datalen = sizeof(store_item);
    reqlist_entry.req_data = (void *) &store_item;

    if( qcril_reqlist_new(instance_id, &reqlist_entry) != E_SUCCESS )
    {
      QCRIL_LOG_ERROR("Cannot create a new reqlist entry");
      break;
    }
    QCRIL_LOG_INFO("Created a new reqlist entry");
    user_data = QCRIL_COMPOSE_USER_DATA( instance_id,
                                         QCRIL_DEFAULT_MODEM_ID,
                                         reqlist_entry.req_id );

    QCRIL_LOG_INFO("processing request - config item: %d", config_params.config_item);

    config_params.config_item_value_len = 0;
    config_params.config_item_value = NULL;
    config_params.extra_data_len = sizeof(user_data);
    config_params.extra_data = (void *)(uintptr_t)user_data;

    /* Get the get request handler and call it with config params */
    req_handler = qcril_qmi_radio_config_find_get_config_req_handler(config_params.config_item);

    if(req_handler == NULL)
    {
      QCRIL_LOG_ERROR("NULL req handler for the item");
      break;
    }

    radio_config_error = (req_handler)(&config_params);

    QCRIL_LOG_INFO("Returned from req handler with radio_config_error: %d",
                    radio_config_error);
  }while(FALSE);

  /* If calling the req handler fails due to above reasons
   * Send Failure response back
   */
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
  {
    QCRIL_LOG_ERROR("radio config error: %d..sending empty response", radio_config_error);
    qcril_qmi_imss_get_ims_config_log_and_send_response(params_ptr->t,
                                                        NULL,
                                                        radio_config_error,
                                                        QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
  }

  QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_imss_request_get_ims_config_v02

//===========================================================================
// qcril_qmi_imss_command_cb_helper_v02
//===========================================================================
void qcril_qmi_imss_command_cb_helper_v02
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
   qcril_instance_id_e_type instance_id;
   uint32 user_data;
   uint16 req_id;
   qcril_reqlist_public_type req_info;
   qcril_request_params_type req_data;
   qmi_resp_callback_type * qmi_resp_callback;
   int config_item;

   QCRIL_LOG_FUNC_ENTRY();
   QCRIL_NOTUSED(ret_ptr);

   qmi_resp_callback = (qmi_resp_callback_type *) params_ptr->data;
   if( qmi_resp_callback )
   {
      if (qmi_resp_callback->data_buf != NULL)
      {
         user_data = ( uint32 )(uintptr_t) qmi_resp_callback->cb_data;
         instance_id = QCRIL_EXTRACT_INSTANCE_ID_FROM_USER_DATA( user_data );
         req_id = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA( user_data );

         memset(&req_data, 0, sizeof(req_data));
         req_data.modem_id = QCRIL_DEFAULT_MODEM_ID;
         req_data.instance_id = instance_id;
         req_data.datalen = qmi_resp_callback->data_buf_len;
         req_data.data = qmi_resp_callback->data_buf;

         /* Lookup the Token ID */
         if ( qcril_reqlist_query_by_req_id( req_id, &instance_id, &req_info ) == E_SUCCESS )
         {
            if( qmi_resp_callback->transp_err != QMI_NO_ERR )
            {
               QCRIL_LOG_INFO("Transp error (%d) recieved from QMI for RIL request %d",
                       qmi_resp_callback->transp_err, req_info.request);
               /* Send GENERIC_FAILURE response */
               qcril_send_empty_payload_request_response( instance_id, req_info.t,
                       req_info.request, RIL_E_GENERIC_FAILURE );
            }
            else if( (req_info.request == QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG) ||
                    (req_info.request == QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG))
            {
               /* If the request is one of the get/set
                  config event id's process it seperately */
               req_data.t = req_info.t;
               req_data.event_id = req_info.request;

               /* Extract the stored config item from the reqlist entry
                  and call dispatch function */
               config_item =
                   qcril_qmi_imss_extract_config_item_from_reqlist_data(req_info.req_data,
                                                                        req_info.req_datalen);

               QCRIL_LOG_INFO("Calling config dispatch response helper, Config_item: %d", config_item);
               qcril_qmi_imss_dispatch_config_response_helper(&req_data, config_item,
                                                              qmi_resp_callback->msg_id);
            }
            else
            {
               req_data.t = req_info.t;
               req_data.event_id = req_info.request;
               switch(qmi_resp_callback->msg_id)
               {
                 case QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01:
                   qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr(&req_data);
                   break;
                 case QMI_IMS_SETTINGS_SEND_RTT_RESP_V01:
                   qcril_qmi_imss_request_send_rtt_msg_resp_hdlr(&req_data);
                   break;
                 default:
                  QCRIL_LOG_INFO("Unsupported QMI IMSS message %d", qmi_resp_callback->msg_id);
                  break;
               }
            }
         }
         else
         {
            QCRIL_LOG_ERROR( "Req ID: %d not found", req_id );
         }

         qcril_free( qmi_resp_callback->data_buf );
      }
      else
      {
         QCRIL_LOG_ERROR("qmi_resp_callback->data_buf is NULL");
      }
   }
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_command_cb_helper_v02

//===========================================================================
// qcril_qmi_imss_unsol_ind_cb_helper_v02
//===========================================================================
void qcril_qmi_imss_unsol_ind_cb_helper_v02
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  qmi_ind_callback_type * qmi_callback = NULL;

  uint32_t decoded_payload_len = 0;
  qmi_client_error_type qmi_err = QMI_NO_ERR;
  void* decoded_payload = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  if( NULL != params_ptr && NULL != params_ptr->data )
  {
    qmi_callback = (qmi_ind_callback_type*) params_ptr->data;
    qmi_idl_get_message_c_struct_len(
                     qcril_qmi_client_get_service_object(QCRIL_QMI_CLIENT_IMS_SETTING),
                     QMI_IDL_INDICATION,
                     qmi_callback->msg_id,
                     &decoded_payload_len);

    if(decoded_payload_len)
    {
      decoded_payload = qcril_malloc(decoded_payload_len);
    }

    if ( decoded_payload || !decoded_payload_len )
    {
        if( decoded_payload_len )
        {
          qmi_err = qmi_client_message_decode(qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_IMS_SETTING),
                                              QMI_IDL_INDICATION,
                                              qmi_callback->msg_id,
                                              qmi_callback->data_buf,
                                              qmi_callback->data_buf_len,
                                              decoded_payload,
                                              (int)decoded_payload_len);
        }

        if ( QMI_NO_ERR == qmi_err )
        {
          switch(qmi_callback->msg_id)
          {
              case QMI_IMS_SETTINGS_IMS_SERVICE_ENABLE_CONFIG_IND_V01:
                qcril_qmi_imss_ims_service_enable_config_ind_hdlr(decoded_payload);
                break;

              case QMI_IMS_SETTINGS_RTT_RCVD_DATA_IND_V01:
                qcril_qmi_imss_rtt_rcvd_data_ind_hdlr(decoded_payload);
                break;

              default:
                QCRIL_LOG_INFO("Unknown QMI IMSA indication %d", qmi_callback->msg_id);
                break;
            }
          }
          else
          {
              QCRIL_LOG_INFO("Indication decode failed for msg %d of svc %d with error %d",
                qmi_callback->msg_id, QCRIL_QMI_CLIENT_IMS_SETTING, qmi_err);
          }

          if( decoded_payload_len )
          {
            qcril_free(decoded_payload);
          }
    }

    if( qmi_callback->data_buf )
    {
      qcril_free(qmi_callback->data_buf);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr or params_ptr->data is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_unsol_ind_cb_helper_v02

//===========================================================================
// qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
)
{
  ims_settings_set_ims_service_enable_config_rsp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL != params_ptr)
  {
    resp = params_ptr->data;

    if (NULL != resp)
    {
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));
      QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
    }
    else
    {
      QCRIL_LOG_ERROR("params_ptr->data is NULL");
    }

    do
    {
      if(QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS == params_ptr->event_id)
      {
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                params_ptr->t, params_ptr->event_id, ril_err);
      }
      if(QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE == params_ptr->event_id)
      {
        qcril_qmi_imss_handle_ims_reg_change_event_resp(params_ptr, ril_err);
      }
    } while (FALSE);
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_ims_service_enable_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_handle_ims_reg_change_event_resp
//===========================================================================
void qcril_qmi_imss_handle_ims_reg_change_event_resp
(
  const qcril_request_params_type *const params_ptr,
  RIL_Errno ril_err
)
{
  boolean need_to_set_voice_domain_pref = FALSE;
  nas_voice_domain_pref_enum_type_v01 voice_domain_pref = NAS_VOICE_DOMAIN_PREF_PS_PREF_V01;
  qcril_reqlist_public_type req_info;
  boolean send_response = TRUE;

  QCRIL_LOG_INFO(".. Need to change voice domain pref? %s",
    feature_voice_dom_pref_on_toggle_ims_cap ? "Yes" : "No");

  do
  {
    if(TRUE != qcril_qmi_ims_cached_info.imss_state_change_requested)
    {
      QCRIL_LOG_ERROR("IMSS state change is not requested.");
      break;
    }

    QCRIL_LOG_INFO("token id = 0x%x", qcril_log_get_token_id(params_ptr->t));
    if(qcril_reqlist_query(QCRIL_DEFAULT_INSTANCE_ID, params_ptr->t, &req_info) !=
            RIL_E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Reqlist entry for token id 0x%x is not found", params_ptr->t);
      break;
    }

    qcril_req_state_e_type state = req_info.state[QCRIL_DEFAULT_INSTANCE_ID];
    QCRIL_LOG_INFO("req state is %s", qcril_reqlist_lookup_state_name(state));
    if(state == QCRIL_REQ_COMPLETED_FAILURE)
    {
      QCRIL_LOG_INFO("state is QCRIL_REQ_COMPLETED_FAILURE!!");
      ril_err = RIL_E_GENERIC_FAILURE;
      break;
    }

    if(RIL_E_SUCCESS == ril_err)
    {
      qcril_qmi_imss_info_lock();
      if(feature_voice_dom_pref_on_toggle_ims_cap &&
              (qcril_qmi_ims_cached_info.imss_new_reg_state !=
              ims_Registration_RegState_REGISTERED))
      {
        voice_domain_pref = NAS_VOICE_DOMAIN_PREF_CS_ONLY_V01;
        need_to_set_voice_domain_pref = TRUE;
      }
      qcril_qmi_ims_cached_info.imss_reg_state_valid = TRUE;
      qcril_qmi_ims_cached_info.imss_reg_state = qcril_qmi_ims_cached_info.imss_new_reg_state;
      qcril_qmi_imss_info_unlock();
      QCRIL_LOG_INFO(".. IMS state changed to %d\n", qcril_qmi_ims_cached_info.imss_reg_state);
    }
    else
    {
      QCRIL_LOG_INFO(".. Failed to change IMS state and remains in state %d\n",
              qcril_qmi_ims_cached_info.imss_reg_state);

      QCRIL_LOG_INFO("Failure; updating state to QCRIL_REQ_COMPLETED_FAILURE");
      qcril_reqlist_update_state( QCRIL_DEFAULT_INSTANCE_ID,
              QCRIL_DEFAULT_MODEM_ID,
              req_info.t,
              QCRIL_REQ_COMPLETED_FAILURE);

      qcril_qmi_imss_info_lock();
      if (feature_voice_dom_pref_on_toggle_ims_cap &&
              qcril_qmi_ims_cached_info.imss_new_reg_state ==
              ims_Registration_RegState_REGISTERED)
      {
        voice_domain_pref = NAS_VOICE_DOMAIN_PREF_CS_ONLY_V01;
        need_to_set_voice_domain_pref = TRUE;
      }
      qcril_qmi_imss_info_unlock();
    }
  } while(FALSE);

  if (need_to_set_voice_domain_pref)
  {
    if (qcril_qmi_nas_set_voice_domain_preference(voice_domain_pref, req_info.req_id) ==
        RIL_E_SUCCESS)
    {
      send_response = FALSE;
    }
  }

  if (send_response)
  {
    qcril_qmi_imss_info_lock();
    qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
    qcril_qmi_imss_info_unlock();

    qcril_send_empty_payload_request_response( QCRIL_DEFAULT_INSTANCE_ID,
        params_ptr->t, params_ptr->event_id, ril_err);
  }
}

//===========================================================================
// qcril_qmi_imss_client_ims_service_enable_config_ind_hdlr
//===========================================================================
void qcril_qmi_imss_ims_service_enable_config_ind_hdlr(void *ind_data_ptr)
{
  ims_settings_ims_service_enable_config_ind_msg_v01 *reg_ind_msg_ptr =
    (ims_settings_ims_service_enable_config_ind_msg_v01*) ind_data_ptr;
  uint8_t unsol_nw_status = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != reg_ind_msg_ptr)
  {
    qcril_qmi_imss_info_lock();
    qcril_qmi_ims_cached_info.wifi_calling_enabled_valid =
      reg_ind_msg_ptr->wifi_calling_enabled_valid;
    qcril_qmi_ims_cached_info.wifi_calling_enabled =
      qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02(reg_ind_msg_ptr->wifi_calling_enabled);
    qcril_qmi_ims_cached_info.call_mode_preference_valid =
      reg_ind_msg_ptr->call_mode_preference_valid;
    qcril_qmi_ims_cached_info.call_mode_preference =
    qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(reg_ind_msg_ptr->call_mode_preference);
    QCRIL_LOG_INFO(".. wifi_call_valid: %d, wifi_call: %d",
      qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
      qcril_qmi_ims_cached_info.wifi_calling_enabled);
    QCRIL_LOG_INFO(".. wifi_call_preference_valid: %d, wifi_call_preference: %d",
      qcril_qmi_ims_cached_info.call_mode_preference_valid,
       qcril_qmi_ims_cached_info.call_mode_preference);
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid =
      reg_ind_msg_ptr->service_mask_by_network_enabled_valid;
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled =
      reg_ind_msg_ptr->service_mask_by_network_enabled;
    QCRIL_LOG_INFO("servce_mask_by_network_enabled_valid: %d, servce_mask_by_network_enabled: %d",
      qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid,
      qcril_qmi_ims_cached_info.service_mask_by_network_enabled);
    qcril_qmi_imss_info_unlock();

    if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT))
    {
      unsol_nw_status = qcril_qmi_imss_compare_wfc_pref_unsol();
      if(unsol_nw_status)
      {
         QCRIL_LOG_INFO(".. WFC status changed notify UNSOL_NW_STATE_CHANGED");
         qcril_qmi_nas_wave_voice_data_status();
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//  qcril_qmi_imss_convert_ril_to_imss_wfc_status_v02
//===========================================================================
uint8_t qcril_qmi_imss_convert_ril_to_imss_wfc_status_v02
(
  qcril_ims_setting_wfc_status_type wfc_status
)
{
  uint8_t res;

  switch(wfc_status)
  {
    case QCRIL_IMS_SETTING_WFC_OFF:
      res = FALSE;
      break;

    case QCRIL_IMS_SETTING_WFC_ON:
      res = TRUE;
      break;

    default:
      res = QMI_RIL_FF;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_ril_to_imss_wfc_preference_v02
//===========================================================================
ims_settings_call_mode_option_enum_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_preference_v02
(
  qcril_ims_setting_wfc_preference_type wfc_preference
)
{
  ims_settings_call_mode_option_enum_v01 res;

  switch(wfc_preference)
  {
    case QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED:
      res = IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED:
      res = IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01;
      break;

    default:
      res = IMS_SETTINGS_CALL_MODE_PREFERRED_NONE_V01;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
//===========================================================================
qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02
(
  uint8_t wfc_status
)
{
  qcril_ims_setting_wfc_status_type res;

  switch(wfc_status)
  {
    case FALSE:
      res = QCRIL_IMS_SETTING_WFC_OFF;
      break;

    case TRUE:
      res = QCRIL_IMS_SETTING_WFC_ON;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
//===========================================================================
qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02
(
  ims_settings_call_mode_option_enum_v01 wfc_preference
)
{
  qcril_ims_setting_wfc_preference_type res;

  switch(wfc_preference)
  {
    case IMS_SETTINGS_CALL_MODE_PREFERRED_WIFI_V01:
      res = QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED;
      break;

    case IMS_SETTINGS_CALL_MODE_PREFERRED_CELLULAR_V01:
      res = QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_PREF_NONE;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_get_ims_service_enable_config
//===========================================================================
void qcril_qmi_imss_get_ims_service_enable_config()
{
   IxErrnoType qmi_client_error;
   ims_settings_get_ims_service_enable_config_rsp_msg_v01
     qmi_ims_get_ims_service_enable_config_resp;

   QCRIL_LOG_FUNC_ENTRY();

   memset(&qmi_ims_get_ims_service_enable_config_resp, 0x0,
     sizeof(qmi_ims_get_ims_service_enable_config_resp));
   qmi_client_error = qcril_qmi_client_send_msg_sync ( QCRIL_QMI_CLIENT_IMS_SETTING,
     QMI_IMS_SETTINGS_GET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01,
     NULL,
     0,
     &qmi_ims_get_ims_service_enable_config_resp,
     sizeof(qmi_ims_get_ims_service_enable_config_resp) );
   QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error );

   if (E_SUCCESS == qmi_client_error)
   {
      if( qmi_ims_get_ims_service_enable_config_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
      {
        qcril_qmi_imss_store_get_ims_service_enable_resp(
          &qmi_ims_get_ims_service_enable_config_resp);
      }
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_ims_service_enable_config

//===========================================================================
//  qcril_qmi_imss_store_get_ims_service_enable_resp
//===========================================================================
void qcril_qmi_imss_store_get_ims_service_enable_resp
(
  ims_settings_get_ims_service_enable_config_rsp_msg_v01* qmi_resp
)
{
  qcril_qmi_imss_info_lock();
  qcril_qmi_ims_cached_info.imss_reg_state_valid =
    qmi_resp->ims_service_enabled_valid;
  qcril_qmi_ims_cached_info.imss_reg_state =
    (qmi_resp->ims_service_enabled == TRUE ) ?
      ims_Registration_RegState_NOT_REGISTERED : ims_Registration_RegState_REGISTERED;
  QCRIL_LOG_INFO(".. IMS has_state: %d, state: %d",
    qcril_qmi_ims_cached_info.imss_reg_state_valid,
      qcril_qmi_ims_cached_info.imss_reg_state);
  qcril_qmi_ims_cached_info.wifi_calling_enabled_valid =
    qmi_resp->wifi_calling_enabled_valid;
  qcril_qmi_ims_cached_info.wifi_calling_enabled =
    qcril_qmi_imss_convert_imss_to_ril_wfc_status_v02(
      qmi_resp->wifi_calling_enabled);
  qcril_qmi_ims_cached_info.call_mode_preference_valid =
    qmi_resp->call_mode_preference_valid;
  qcril_qmi_ims_cached_info.call_mode_preference =
    qcril_qmi_imss_convert_imss_to_ril_wfc_preference_v02(
      qmi_resp->call_mode_preference);
  QCRIL_LOG_INFO(".. wifi_call_valid: %d, wifi_call: %d",
    qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
    qcril_qmi_ims_cached_info.wifi_calling_enabled);
  QCRIL_LOG_INFO(".. wifi_call_preference_valid: %d, wifi_call_preference: %d",
    qcril_qmi_ims_cached_info.call_mode_preference_valid,
    qcril_qmi_ims_cached_info.call_mode_preference);
  qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid =
    qmi_resp->service_mask_by_network_enabled_valid;
  qcril_qmi_ims_cached_info.service_mask_by_network_enabled =
    qmi_resp->service_mask_by_network_enabled;
  QCRIL_LOG_INFO("servce_mask_by_network_enabled_valid: %d, \
    servce_mask_by_network_enabled: %d",
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid,
    qcril_qmi_ims_cached_info.service_mask_by_network_enabled);
  qcril_qmi_imss_info_unlock();
}

//===========================================================================
//  qcril_qmi_imss_get_service_mask_by_network_enabled
//===========================================================================
ims_settings_omadm_services_type_mask_v01 qcril_qmi_imss_get_service_mask_by_network_enabled()
{
  ims_settings_omadm_services_type_mask_v01 service_mask =
    IMS_SETTINGS_OMADM_SERVICES_BIT_MASK_NONE_V01;

  qcril_qmi_imss_info_lock();
  if(qcril_qmi_ims_cached_info.service_mask_by_network_enabled_valid)
  {
    service_mask = qcril_qmi_ims_cached_info.service_mask_by_network_enabled;
  }
  qcril_qmi_imss_info_unlock();

  return service_mask;
}

//===========================================================================
// qcril_qmi_imss_request_send_rtt_msg
//===========================================================================
void qcril_qmi_imss_request_send_rtt_msg
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  ims_settings_send_rtt_req_msg_v01  qmi_req;
  ims_settings_send_rtt_resp_msg_v01 *qmi_resp = NULL;
  qcril_reqlist_public_type reqlist_entry;
  uint32 user_data;
  IxErrnoType qmi_client_error;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  memset(&qmi_req, 0, sizeof(qmi_req));

  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    ims_RttMessage* ims_rtt_msg_ptr = (ims_RttMessage  *)params_ptr->data;
    if (!ims_rtt_msg_ptr)
    {
      QCRIL_LOG_ERROR("Invalid arguments");
      break;
    } else if (ims_rtt_msg_ptr->rttMessage.arg == NULL)
    {
      QCRIL_LOG_ERROR("Invalid Arguments for RTT Message");
      break;
    }
    qmi_req.rtt_data_len = strlen(ims_rtt_msg_ptr->rttMessage.arg);

    if (IMS_SETTINGS_RTT_DATA_LEN_MAX_V01 <= qmi_req.rtt_data_len)
    {
      QCRIL_LOG_ERROR("Invalid rtt string length %d", qmi_req.rtt_data_len);
      break;
    }
    strlcpy((char*)qmi_req.rtt_data,
            (char*)ims_rtt_msg_ptr->rttMessage.arg,
            IMS_SETTINGS_RTT_DATA_LEN_MAX_V01);
    QCRIL_LOG_DEBUG("Rtt Message : %s ",qmi_req.rtt_data);

    qcril_reqlist_default_entry( params_ptr->t,
                                 params_ptr->event_id,
                                 QCRIL_DEFAULT_MODEM_ID,
                                 QCRIL_REQ_AWAITING_CALLBACK,
                                 QCRIL_EVT_NONE,
                                 NULL,
                                 &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &reqlist_entry) == E_SUCCESS)
    {
      qmi_resp = qcril_malloc(sizeof(*qmi_resp));
      if (NULL == qmi_resp)
      {
        QCRIL_LOG_ERROR("qcril_malloc failed");
        break;
      }
      user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
                                          QCRIL_DEFAULT_MODEM_ID,
                                          reqlist_entry.req_id );
      qmi_client_error = qcril_qmi_client_send_msg_async( QCRIL_QMI_CLIENT_IMS_SETTING,
                                                          QMI_IMS_SETTINGS_SEND_RTT_REQ_V01,
                                                          &qmi_req,
                                                          sizeof(qmi_req),
                                                          qmi_resp,
                                                          sizeof(*qmi_resp),
                                                          (void*)(uintptr_t)user_data );
      QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error );
      if (E_SUCCESS == qmi_client_error)
      {
        res = RIL_E_SUCCESS;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Cannot create a new reqlist entry");
      break;
    }
  } while(FALSE);

  if (RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                                              params_ptr->t,
                                              params_ptr->event_id,
                                              res);
    if(qmi_resp != NULL)
    {
      qcril_free(qmi_resp);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}  // qcril_qmi_imss_request_send_rtt_msg

//===========================================================================
// qcril_qmi_imss_request_send_rtt_msg_resp_hdlr
//===========================================================================
void qcril_qmi_imss_request_send_rtt_msg_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  ims_settings_send_rtt_resp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL != params_ptr)
  {
    resp = params_ptr->data;

    if (NULL != resp)
    {
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR,
                                                                        &(resp->resp));
      QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
    }
    else
    {
      QCRIL_LOG_ERROR("params_ptr->data is NULL");
    }
    qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                                              params_ptr->t, params_ptr->event_id, ril_err);
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_send_rtt_msg_resp_hdlr

//===========================================================================
// qcril_qmi_imss_rtt_rcvd_data_ind_hdlr
//===========================================================================
void qcril_qmi_imss_rtt_rcvd_data_ind_hdlr(void *ind_data_ptr)
{
  ims_settings_rtt_ind_msg_v01 *rtt_message_incoming = (ims_settings_rtt_ind_msg_v01 *)ind_data_ptr;
  int rtt_msg_len;
  ims_RttMessage resp;

  QCRIL_LOG_FUNC_ENTRY();

  if( rtt_message_incoming != NULL)
  {
    rtt_msg_len =  (int)rtt_message_incoming->rtt_data_len + 1;
    resp.rttMessage.arg = qcril_malloc(sizeof(char) * rtt_msg_len);
    if (resp.rttMessage.arg)
    {
      strlcpy(resp.rttMessage.arg, (char*)rtt_message_incoming->rtt_data,
                                    rtt_msg_len);
      QCRIL_LOG_INFO("Rtt incoming Message %s",resp.rttMessage.arg);
      imsRadioSendMessage(0,
                          ims_MsgType_UNSOL_RESPONSE,
                          ims_MsgId_UNSOL_RESPONSE_RTT_MSG_RECEIVED,
                          ims_Error_E_SUCCESS,
                          &resp,
                          sizeof(resp));
      qcril_free(resp.rttMessage.arg);
    }
    else
    {
      QCRIL_LOG_ERROR("memory malloc failed");
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
}
