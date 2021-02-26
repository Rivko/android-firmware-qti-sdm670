/******************************************************************************
  @file    qcril_qmi_imss.c
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI IMS Setting.

  ---------------------------------------------------------------------------

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#include <cutils/properties.h>
#include "qcril_qmi_client.h"
#include "qcril_qmi_imss.h"
#include "qcril_qmi_imss_internal.h"
#include "ip_multimedia_subsystem_settings_v01.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_ims_radio_agent.h"
#include "qcril_qmi_ims_misc.h"
#include "qcril_qmi_npb_utils.h"
#include "qcril_qmi_radio_config_imss.h"
#include "qcril_qmi_nas.h"
#include "qcril_qmi_imsa.h"
#include "qcril_qmi_imss_v02.h"
#include "qcril_arb.h"
#include "qcril_data.h"
#include <cutils/properties.h>

#define IMSS_MODEM_VERSION_MAX 2
#define IMSS_HANDLERS_MAX 11
#define BIT_POS_IMS_SERVICE_ENABLE_CONFIG_MSG_ID 0x80
#define BYTE_POS_IMS_SERVICE_ENABLE_CONFIG_MSG_ID 17

//===========================================================================
//                     GLOBALS
//===========================================================================
struct ims_cached_info_type   qcril_qmi_ims_cached_info;

int qcril_qmi_imss_get_modem_version(void);

boolean feature_voice_dom_pref_on_toggle_ims_cap = FALSE;
boolean feature_disabled_modem_req               = FALSE;

uint8_t modem_version = 0; // 0 for old 1 for new modem versions

typedef struct
{
  int event_id;
  qcril_req_handler_type *handler;
} qcril_qmi_imss_table_entry_type;

static qcril_qmi_imss_table_entry_type qcril_qmi_imss_table[IMSS_MODEM_VERSION_MAX][IMSS_HANDLERS_MAX] =
{
  {
    { QCRIL_EVT_QMI_IMSS_HANDLE_COMM_CALLBACKS, qcril_qmi_imss_command_cb_helper},
    { QCRIL_EVT_QMI_IMSS_HANDLE_INDICATIONS, qcril_qmi_imss_unsol_ind_cb_helper},
    { QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE, qcril_qmi_imss_request_set_ims_registration},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS, qcril_qmi_imss_request_set_ims_srv_status},
    { QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY, qcril_qmi_imss_request_query_vt_call_quality},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY, qcril_qmi_imss_request_set_vt_call_quality},
    { QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_get_wifi_calling_status},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_set_wifi_calling_status},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG, qcril_qmi_imss_request_set_ims_config},
    { QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG, qcril_qmi_imss_request_get_ims_config},
    { -1, NULL}
  },
  {
    { QCRIL_EVT_QMI_IMSS_HANDLE_COMM_CALLBACKS, qcril_qmi_imss_command_cb_helper_v02},
    { QCRIL_EVT_QMI_IMSS_HANDLE_INDICATIONS, qcril_qmi_imss_unsol_ind_cb_helper_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_IMS_REG_STATE_CHANGE, qcril_qmi_imss_request_set_ims_registration_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_SERVICE_STATUS, qcril_qmi_imss_request_set_ims_srv_status_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY, qcril_qmi_imss_request_query_vt_call_quality_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_VT_CALL_QUALITY, qcril_qmi_imss_request_set_vt_call_quality_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_get_wifi_calling_status_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS, qcril_qmi_imss_request_set_wifi_calling_status_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG, qcril_qmi_imss_request_set_ims_config_v02},
    { QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG, qcril_qmi_imss_request_get_ims_config_v02},
    { -1, NULL}
  }
};

void qcril_qmi_imss_init_handlers(void)
{
  // query modem and identify imss supports new version or not.
  // set "modem_version" based on the modem version

  modem_version = qcril_qmi_imss_get_modem_version();

  qcril_qmi_imss_table_entry_type *imss_table = qcril_qmi_imss_table[modem_version];
  int i = 0;

  QCRIL_LOG_DEBUG("Update the handlers for modem_version : %d", modem_version);

  for (i = 0; i < IMSS_HANDLERS_MAX; i++)
  {
      QCRIL_LOG_DEBUG("event_id = %d", imss_table[i].event_id);
      if (imss_table[i].event_id != -1)
      {
          qcril_update_event_table_entry(imss_table[i].event_id, imss_table[i].handler);
      }
  }

  qcril_qmi_radio_config_update_meta_table(modem_version);
  QCRIL_LOG_DEBUG("IMSS table update complete.");
}

//===========================================================================
// qcril_qmi_imss_init
//===========================================================================
void qcril_qmi_imss_init(void)
{
   char prop_str[PROPERTY_VALUE_MAX];

   qcril_qmi_imss_init_handlers();

   // register for indication
   qmi_client_error_type ret = QMI_NO_ERR;
   ims_settings_config_ind_reg_req_msg_v01 ind_reg_req;
   ims_settings_config_ind_reg_rsp_msg_v01 *ind_reg_resp_ptr = qcril_malloc(sizeof(ims_settings_config_ind_reg_rsp_msg_v01));

   pthread_mutexattr_init( &qcril_qmi_ims_cached_info.imss_info_lock_mutex_atr );
   pthread_mutex_init( &qcril_qmi_ims_cached_info.imss_info_lock_mutex,
                       &qcril_qmi_ims_cached_info.imss_info_lock_mutex_atr );

   qcril_qmi_imss_clear_client_provisioning_config_info();

   qcril_qmi_imss_info_lock();
   qcril_qmi_ims_cached_info.imss_reg_state_valid = FALSE;
   qcril_qmi_ims_cached_info.imss_reg_state = ims_Registration_RegState_NOT_REGISTERED;
   qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
   qcril_qmi_ims_cached_info.imss_new_reg_state = ims_Registration_RegState_NOT_REGISTERED;
   qcril_qmi_imss_info_unlock();


   memset(prop_str, 0, sizeof(prop_str));
   property_get(QMI_RIL_IMSS_VOICE_DOMAIN_PREF_ON_IMS_TOGGLE, prop_str, "");
   QCRIL_LOG_INFO("Property: %s, value: %s",
           QMI_RIL_IMSS_VOICE_DOMAIN_PREF_ON_IMS_TOGGLE, prop_str);
   feature_voice_dom_pref_on_toggle_ims_cap =
           ((strcmp(prop_str, "true") == 0) || (strcmp(prop_str, "1") == 0));

   feature_disabled_modem_req = is_modem_ims_config_disable();

   if (ind_reg_resp_ptr)
   {
      memset(&ind_reg_req, 0, sizeof(ind_reg_req));
      ind_reg_req.client_provisioning_config_valid = TRUE;
      ind_reg_req.client_provisioning_config = 1;  // enable

      ind_reg_req.reg_mgr_config_valid = TRUE;
      ind_reg_req.reg_mgr_config = 1;  // enable

      ind_reg_req.ims_service_enable_config_enabled_valid = TRUE;
      ind_reg_req.ims_service_enable_config_enabled = 1;

      ind_reg_req.rtt_config_enabled_valid = TRUE;
      ind_reg_req.rtt_config_enabled = 1; //enabled

      ret = qcril_qmi_client_send_msg_async( QCRIL_QMI_CLIENT_IMS_SETTING,
                                             QMI_IMS_SETTINGS_CONFIG_IND_REG_REQ_V01,
                                             (void*) &ind_reg_req,
                                             sizeof(ind_reg_req),
                                             (void*) ind_reg_resp_ptr,
                                             sizeof(*ind_reg_resp_ptr),
                                             (void*)0
                                            );
      if (QMI_NO_ERR != ret)
      {
         QCRIL_LOG_ERROR("registration for indication failed %d", ret);
         qcril_free(ind_reg_resp_ptr);
      }
   }
   else
   {
      QCRIL_LOG_ERROR("ind_reg_resp_ptr malloc failed");
   }

   //Get the intial config values
   //Fetch the WFC status
   if(1 == modem_version)
   {
     qcril_qmi_imss_get_ims_service_enable_config();
   }
   else
   {
     qcril_qmi_imss_get_ims_reg_config();

     qcril_qmi_imss_get_client_provisioning_config();
   }

} // qcril_qmi_imss_init

//===========================================================================
// qcril_qmi_imss_cleanup
//===========================================================================
void qcril_qmi_imss_cleanup(void)
{
  QCRIL_LOG_FUNC_ENTRY();

  qcril_qmi_imss_info_lock();
  qcril_qmi_ims_cached_info.imss_reg_state_valid = FALSE;
  qcril_qmi_ims_cached_info.imss_reg_state = ims_Registration_RegState_NOT_REGISTERED;
  qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
  qcril_qmi_ims_cached_info.imss_new_reg_state = ims_Registration_RegState_NOT_REGISTERED;
  qcril_qmi_imss_info_unlock();

  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_imss_info_lock
//===========================================================================
void qcril_qmi_imss_info_lock()
{
    pthread_mutex_lock( &qcril_qmi_ims_cached_info.imss_info_lock_mutex );
} // qcril_qmi_imss_info_lock

//===========================================================================
// qcril_qmi_imss_info_unlock
//===========================================================================
void qcril_qmi_imss_info_unlock()
{
    pthread_mutex_unlock( &qcril_qmi_ims_cached_info.imss_info_lock_mutex );
} // qcril_qmi_imss_info_unlock

//===========================================================================
// qcril_qmi_imss_request_set_ims_registration
//===========================================================================
void qcril_qmi_imss_request_set_ims_registration
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
  uint8_t ims_test_mode_enabled = FALSE;

  QCRIL_NOTUSED( ret_ptr );

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    ims_in_data_ptr = (ims_Registration *)params_ptr->data;
    QCRIL_LOG_INFO("has_state: %d, state: %d", ims_in_data_ptr->has_state, ims_in_data_ptr->state);

    if (feature_disabled_modem_req)
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
    ims_test_mode_enabled =
      (ims_Registration_RegState_REGISTERED == ims_in_data_ptr->state) ? FALSE : TRUE;
    res = qcril_qmi_imss_set_ims_test_mode_enabled(ims_test_mode_enabled, reqlist_entry.req_id);
  } while (FALSE);

  // imss state change is not in progress and failed to initiate IMS test mode enable flag
  if (!is_state_change_in_progress && (RIL_E_SUCCESS != res))
  {
    qcril_qmi_imss_info_lock();
    qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
    qcril_qmi_imss_info_unlock();
  }

  if (feature_disabled_modem_req || RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(instance_id,
        params_ptr->t, params_ptr->event_id, res);
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_registration

//===========================================================================
// qcril_qmi_imss_request_set_ims_srv_status
//===========================================================================
void qcril_qmi_imss_request_set_ims_srv_status
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

  ims_settings_set_qipcall_config_req_msg_v01 qmi_req;
  ims_settings_set_qipcall_config_rsp_msg_v01 *qmi_resp = NULL;

  //Set the wifi calling setting through client provisioning settings.
  //If the radio tech is WLAN. So declare the variable.
  ims_settings_set_client_provisioning_config_req_msg_v01 qmi_client_provisioning_req;
  ims_settings_set_client_provisioning_config_rsp_msg_v01 *qmi_client_provisioning_resp = NULL;

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
        &reqlist_entry);

    if (qcril_reqlist_new(instance_id, &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to Add into Req list; Send failure");
      res = RIL_E_GENERIC_FAILURE;
      break;
    }

    memset(&qmi_req, 0, sizeof(qmi_req));
    memset(&qmi_client_provisioning_req, 0, sizeof(qmi_client_provisioning_req));

    QCRIL_LOG_INFO("has_calltype: %d, calltype: %d",
        ims_in_data_ptr->has_callType, ims_in_data_ptr->callType);

    if (ims_in_data_ptr->has_callType &&
        ims_in_data_ptr->accTechStatus.arg &&
        ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0] && // only one acctechstatus will be used
        ((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->has_status)
    {
      QCRIL_LOG_INFO("has_status: %d, status: %d",
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

      user_data = QCRIL_COMPOSE_USER_DATA(instance_id, QCRIL_DEFAULT_MODEM_ID,
          reqlist_entry.req_id);

      if (ims_CallType_CALL_TYPE_VOICE == ims_in_data_ptr->callType &&
          ((((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
            ims_RadioTechType_RADIO_TECH_WIFI) ||
           (((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
            ims_RadioTechType_RADIO_TECH_IWLAN)))
      {
        // Its a wifi call setting. Hence, send client provisioning request with
        // the wifi calling on/off values.
        qmi_client_provisioning_req.wifi_call_valid = TRUE;
        qmi_client_provisioning_req.wifi_call =
          enabled ? IMS_SETTINGS_WFC_STATUS_ON_V01 : IMS_SETTINGS_WFC_STATUS_OFF_V01;

        qmi_client_provisioning_resp = qcril_malloc(sizeof(*qmi_client_provisioning_resp));
        if (NULL == qmi_client_provisioning_resp)
        {
          QCRIL_LOG_ERROR("qcril_malloc failed");
          break;
        }

        QCRIL_LOG_INFO("Sending wifi call setting through set_service, value: %d", enabled);

        qmi_client_error = qcril_qmi_client_send_msg_async(
            QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_REQ_V01,
            &qmi_client_provisioning_req, sizeof(qmi_client_provisioning_req),
            qmi_client_provisioning_resp, sizeof(*qmi_client_provisioning_resp),
            (void*)(uintptr_t)user_data );
      }
      else
      {
        if (ims_CallType_CALL_TYPE_VOICE == ims_in_data_ptr->callType &&
            (((ims_StatusForAccessTech**)ims_in_data_ptr->accTechStatus.arg)[0]->networkMode ==
             ims_RadioTechType_RADIO_TECH_LTE))
        {
          qmi_req.volte_enabled_valid = TRUE;
          qmi_req.volte_enabled = enabled;
        }
        else if (ims_CallType_CALL_TYPE_VT == ims_in_data_ptr->callType)
        {
          qmi_req.vt_calling_enabled_valid = TRUE;
          qmi_req.vt_calling_enabled = enabled;
        }
        else
        {
          QCRIL_LOG_ERROR("request call_type/networkMode is not a valid value");
          break;
        }

        qmi_resp = qcril_malloc(sizeof(*qmi_resp));
        if (NULL == qmi_resp)
        {
          QCRIL_LOG_ERROR("qcril_malloc failed");
          break;
        }

        qmi_client_error = qcril_qmi_client_send_msg_async(
            QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_REQ_V01,
            &qmi_req, sizeof(qmi_req),
            qmi_resp, sizeof(*qmi_resp),
            (void*)(uintptr_t)user_data );
      }
    }
    else
    {
      QCRIL_LOG_ERROR("request misses some necessary information");
      break;
    }
    QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);
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
      qcril_free(qmi_resp);
    }
    if (qmi_client_provisioning_resp != NULL)
    {
      qcril_free(qmi_client_provisioning_resp);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_ims_srv_status

//===========================================================================
// qcril_qmi_imss_request_query_vt_call_quality
//===========================================================================
void qcril_qmi_imss_request_query_vt_call_quality
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    IxErrnoType               qmi_client_error;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;

    ims_settings_get_qipcall_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        qcril_reqlist_default_entry(params_ptr->t,
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
                    reqlist_entry.req_id);

            qmi_client_error = qcril_qmi_client_send_msg_async(QCRIL_QMI_CLIENT_IMS_SETTING,
                    QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_REQ_V01,
                    NULL,
                    0,
                    qmi_resp,
                    sizeof(*qmi_resp),
                    (void*)(uintptr_t)user_data );
            QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);

            if (E_SUCCESS == qmi_client_error)
            {
                res = RIL_E_SUCCESS;
            }
        }
    } while(FALSE);

    if (RIL_E_SUCCESS != res)
    {
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                params_ptr->t,
                params_ptr->event_id,
                res);
        if( qmi_resp != NULL )
        {
            qcril_free( qmi_resp );
        }
    }
} // qcril_qmi_imss_request_query_vt_call_quality

//===========================================================================
// qcril_qmi_imss_request_set_vt_call_quality
//===========================================================================
void qcril_qmi_imss_request_set_vt_call_quality
(
 const qcril_request_params_type *const params_ptr,
 qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    ims_VideoCallQuality     *ims_vcq_data_ptr = NULL;
    IxErrnoType               qmi_client_error;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;

    ims_settings_set_qipcall_config_req_msg_v01  qmi_req;
    ims_settings_set_qipcall_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        ims_vcq_data_ptr = (ims_VideoCallQuality *)params_ptr->data;

        qcril_reqlist_default_entry( params_ptr->t,
                params_ptr->event_id,
                QCRIL_DEFAULT_MODEM_ID,
                QCRIL_REQ_AWAITING_CALLBACK,
                QCRIL_EVT_NONE,
                NULL,
                &reqlist_entry );
        if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &reqlist_entry) == E_SUCCESS)
        {
            memset(&qmi_req, 0, sizeof(qmi_req));

            QCRIL_LOG_INFO( "has_quality: %d, quality: %d",
                            ims_vcq_data_ptr->has_quality,
                            ims_vcq_data_ptr->quality );
            if (ims_vcq_data_ptr->has_quality)
            {
                qmi_req.vt_quality_selector_valid = TRUE;
                if (ims_Quality_HIGH == ims_vcq_data_ptr->quality)
                {
                    qmi_req.vt_quality_selector = IMS_SETTINGS_VT_QUALITY_LEVEL_0_V01;
                }
                else if (ims_Quality_LOW == ims_vcq_data_ptr->quality)
                {
                    qmi_req.vt_quality_selector = IMS_SETTINGS_VT_QUALITY_LEVEL_2_V01;
                }
                else
                {
                    QCRIL_LOG_ERROR("request quality is not a valid value");
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
            user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
                    QCRIL_DEFAULT_MODEM_ID,
                    reqlist_entry.req_id );

            qmi_client_error = qcril_qmi_client_send_msg_async( QCRIL_QMI_CLIENT_IMS_SETTING,
                    QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_REQ_V01,
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
} // qcril_qmi_imss_request_set_vt_call_quality

//===========================================================================
// qcril_qmi_imss_request_get_wifi_calling_status
//===========================================================================
void qcril_qmi_imss_request_get_wifi_calling_status
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    IxErrnoType               qmi_client_error;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;

    ims_settings_get_client_provisioning_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        qcril_reqlist_default_entry(params_ptr->t,
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
                    reqlist_entry.req_id);

            qmi_client_error = qcril_qmi_client_send_msg_async(QCRIL_QMI_CLIENT_IMS_SETTING,
                    QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_REQ_V01,
                    NULL,
                    0,
                    qmi_resp,
                    sizeof(*qmi_resp),
                    (void*)(uintptr_t)user_data );
            QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);

            if (E_SUCCESS == qmi_client_error)
            {
                res = RIL_E_SUCCESS;
            }
        }
    } while(FALSE);

    if (RIL_E_SUCCESS != res)
    {
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                params_ptr->t,
                params_ptr->event_id,
                res);
        if( qmi_resp != NULL )
        {
            qcril_free( qmi_resp );
        }
    }
} // qcril_qmi_imss_request_get_wifi_calling_status

//===========================================================================
//QCRIL_EVT_IMS_SOCKET_REQ_SET_VOLTE_PREF
//===========================================================================
void qcril_qmi_imss_request_set_volte_preference
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  ims_VolteUserPref         *ims_volte_user_pref_ptr = NULL;
  qcril_reqlist_public_type reqlist_entry;
  RIL_Errno                 res = RIL_E_GENERIC_FAILURE;
  nas_voice_domain_pref_enum_type_v01 voice_domain_pref;

  QCRIL_NOTUSED(ret_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    ims_volte_user_pref_ptr = (ims_VolteUserPref *)params_ptr->data;

    if (!ims_volte_user_pref_ptr)
    {
      QCRIL_LOG_ERROR("Invalid arguments");
      break;
    }

    qcril_reqlist_default_entry( params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry );

    QCRIL_LOG_INFO("has_mode: %d, mode: %d",
        ims_volte_user_pref_ptr->has_mode, ims_volte_user_pref_ptr->mode);

    if (ims_volte_user_pref_ptr->has_mode)
    {
      if(FALSE ==
          qcril_qmi_ims_map_ims_volte_user_pref_to_qmi_nas_voice_domain_pref(
                            ims_volte_user_pref_ptr->mode, &voice_domain_pref))
      {
        break;
      }

      if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID, &reqlist_entry) == E_SUCCESS)
      {
        res = qcril_qmi_nas_set_voice_domain_preference(
                         voice_domain_pref, reqlist_entry.req_id);
      }
    }
  } while(FALSE);

  if (RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
            params_ptr->t,
            params_ptr->event_id,
            res);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_request_set_handover_config

//===========================================================================
//QCRIL_EVT_IMS_SOCKET_REQ_GET_VOLTE_PREF
//===========================================================================
void qcril_qmi_imss_request_get_volte_preference
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  RIL_Errno                 res = RIL_E_GENERIC_FAILURE;
  nas_voice_domain_pref_enum_type_v01 voice_domain_pref;
  QCRIL_NOTUSED(ret_ptr);

  res = qcril_qmi_nas_get_voice_domain_preference(&voice_domain_pref);

  if (RIL_E_SUCCESS != res)
  {
    qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
            params_ptr->t,
            params_ptr->event_id,
            res);
  }
  else
  {
    ims_VolteUserPref  ims_volte_user_pref;
    memset(&ims_volte_user_pref, 0, sizeof(ims_volte_user_pref));

    QCRIL_LOG_INFO("voice_domain_pref: %d", voice_domain_pref);

    ims_volte_user_pref.has_mode =
        qcril_qmi_ims_map_qmi_nas_voice_domain_pref_to_ims_volte_user_pref(
                voice_domain_pref, &ims_volte_user_pref.mode);

    imsRadioSendMessage(params_ptr->t,
            ims_MsgType_RESPONSE,
            qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
            qcril_qmi_ims_map_ril_error_to_ims_error(res),
            (void *)&ims_volte_user_pref,
            sizeof(ims_volte_user_pref));
  }

}

//===========================================================================
// qcril_qmi_imss_request_get_handover_config
//===========================================================================
void qcril_qmi_imss_request_get_handover_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    IxErrnoType               qmi_client_error;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;

    ims_settings_get_handover_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        qcril_reqlist_default_entry(params_ptr->t,
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
                    reqlist_entry.req_id);

            qmi_client_error = qcril_qmi_client_send_msg_async(QCRIL_QMI_CLIENT_IMS_SETTING,
                    QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_REQ_V01,
                    NULL,
                    0,
                    qmi_resp,
                    sizeof(*qmi_resp),
                    (void*)(uintptr_t)user_data );
            QCRIL_LOG_INFO(".. qmi send async res %d", (int) qmi_client_error);

            if (E_SUCCESS == qmi_client_error)
            {
                res = RIL_E_SUCCESS;
            }
        }
    } while(FALSE);

    if (RIL_E_SUCCESS != res)
    {
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                params_ptr->t,
                params_ptr->event_id,
                res);
        if( qmi_resp != NULL )
        {
            qcril_free( qmi_resp );
        }
    }
} // qcril_qmi_imss_request_get_handover_config

//===========================================================================
// qcril_qmi_imss_request_set_handover_config
//===========================================================================
void qcril_qmi_imss_request_set_handover_config
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    ims_HandoverConfigMsg     *ims_ho_config_ptr = NULL;
    IxErrnoType               qmi_client_error;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;

    ims_settings_set_handover_config_req_msg_v01  qmi_req;
    ims_settings_set_handover_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        ims_ho_config_ptr = (ims_HandoverConfigMsg  *)params_ptr->data;

        if (!ims_ho_config_ptr)
        {
            QCRIL_LOG_ERROR("Invalid arguments");
            break;
        }

        qcril_reqlist_default_entry( params_ptr->t,
                params_ptr->event_id,
                QCRIL_DEFAULT_MODEM_ID,
                QCRIL_REQ_AWAITING_CALLBACK,
                QCRIL_EVT_NONE,
                NULL,
                &reqlist_entry );

        memset(&qmi_req, 0, sizeof(qmi_req));

        QCRIL_LOG_INFO("has_hoConfig: %d, hoConfig: %d",
                ims_ho_config_ptr->has_hoConfig, ims_ho_config_ptr->hoConfig);

        if (ims_ho_config_ptr->has_hoConfig)
        {
            qmi_req.ims_pdn_ho_enabled_valid =
                qcril_qmi_ims_map_ims_ho_config_to_qmi_imss_ho_config(
                        ims_ho_config_ptr->hoConfig, &qmi_req.ims_pdn_ho_enabled);
        }

        if (!qmi_req.ims_pdn_ho_enabled_valid)
        {
            QCRIL_LOG_ERROR("Invalid ho config request");
            break;
        }

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
                    QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_REQ_V01,
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
} // qcril_qmi_imss_request_set_handover_config

//===========================================================================
// qcril_qmi_imss_request_set_wifi_calling_status
//===========================================================================
void qcril_qmi_imss_request_set_wifi_calling_status
(
   const qcril_request_params_type *const params_ptr,
   qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
    ims_WifiCallingInfo      *ims_wci_data_ptr = NULL;
    IxErrnoType               qmi_client_error;
    uint32                    user_data;
    qcril_reqlist_public_type reqlist_entry;
    RIL_Errno                 res = RIL_E_GENERIC_FAILURE;

    ims_settings_set_client_provisioning_config_req_msg_v01  qmi_req;
    ims_settings_set_client_provisioning_config_rsp_msg_v01 *qmi_resp = NULL;

    QCRIL_NOTUSED(ret_ptr);

    QCRIL_LOG_FUNC_ENTRY();

    do
    {
        ims_wci_data_ptr = (ims_WifiCallingInfo  *)params_ptr->data;

        if (!ims_wci_data_ptr)
        {
            QCRIL_LOG_ERROR("Invalid arguments");
            break;
        }

        qcril_reqlist_default_entry( params_ptr->t,
                params_ptr->event_id,
                QCRIL_DEFAULT_MODEM_ID,
                QCRIL_REQ_AWAITING_CALLBACK,
                QCRIL_EVT_NONE,
                NULL,
                &reqlist_entry );

        memset(&qmi_req, 0, sizeof(qmi_req));

        QCRIL_LOG_INFO("has_status: %d, status: %d "
                "has_preference: %d, preference: %d",
                ims_wci_data_ptr->has_status, ims_wci_data_ptr->status,
                ims_wci_data_ptr->has_preference, ims_wci_data_ptr->preference);

        if (ims_wci_data_ptr->has_status)
        {
            qmi_req.wifi_call_valid =
                qcril_qmi_ims_map_wificallingstatus_to_ims_settings_wfc_status(
                        ims_wci_data_ptr->status, &qmi_req.wifi_call);
        }
        if (ims_wci_data_ptr->has_preference)
        {
            qmi_req.wifi_call_preference_valid =
                qcril_qmi_ims_map_wificallingpreference_to_ims_settings_wfc_preference(
                        ims_wci_data_ptr->preference, &qmi_req.wifi_call_preference);
        }

        if (!qmi_req.wifi_call_valid && !qmi_req.wifi_call_preference_valid)
        {
            QCRIL_LOG_ERROR("request misses some necessary information");
            break;
        }

        // Store the preference
        reqlist_entry.req_data = qcril_malloc(sizeof(ims_WifiCallingInfo));
        if (reqlist_entry.req_data)
        {
            reqlist_entry.req_datalen = sizeof (ims_WifiCallingInfo);
            memcpy(reqlist_entry.req_data, ims_wci_data_ptr, sizeof (ims_WifiCallingInfo));
        }

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
                    QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_REQ_V01,
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
}  // qcril_qmi_imss_request_set_wifi_calling_status

//===========================================================================
// qcril_qmi_imss_request_set_ims_config
//
// Functionality:
// 1. Reads config item from the ims request
// 2. creates a reqlist entry (store the config item id here)
// 3. Depending on the type of item's value, process the data
// 4. and Store the processed data in Config params
// 5. gets the set request handler for the config item
// 6. Call the handler with config params,
//    to send down the appropriate req
//===========================================================================
void qcril_qmi_imss_request_set_ims_config
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
  qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
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
                                QCRIL_REQ_AWAITING_CALLBACK, QCRIL_EVT_NONE,
                                NULL, &reqlist_entry);

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

        bool_value = (ims_config_msg_ptr->intValue ? TRUE : FALSE );
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
        QCRIL_LOG_INFO("Config item received is a string, value: %s", ims_config_msg_ptr->stringValue.arg);
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
    req_handler = qcril_qmi_radio_config_find_set_config_req_handler(config_params.config_item);

    if(req_handler == NULL)
    {
      QCRIL_LOG_ERROR("NULL req handler for the item");
      break;
    }
    radio_config_error = (req_handler)(&config_params);
    QCRIL_LOG_INFO("Returned from req handler with radio_config_error: %d", radio_config_error);
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
} // qcril_qmi_imss_request_set_ims_config

//===========================================================================
// qcril_qmi_imss_request_get_ims_config
//
// Functionality:
// 1. Reads config item from the ims request
// 2. creates a reqlist entry (store the config item id here)
// 3. Store the processed data in Config params
// 5. gets the get request handler for the config item
// 6. Call the handler with config params,
//    to send down the appropriate req
//===========================================================================
void qcril_qmi_imss_request_get_ims_config
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
    qcril_qmi_radio_config_error_type radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
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
} //qcril_qmi_imss_request_get_ims_config

//===========================================================================
// qcril_qmi_imss_get_ims_reg_config
//===========================================================================
void qcril_qmi_imss_get_ims_reg_config()
{
   IxErrnoType qmi_client_error;
   ims_settings_get_reg_mgr_config_rsp_msg_v01 qmi_ims_get_reg_config_resp;

   QCRIL_LOG_FUNC_ENTRY();

   memset(&qmi_ims_get_reg_config_resp, 0x0, sizeof(qmi_ims_get_reg_config_resp));
   qmi_client_error = qcril_qmi_client_send_msg_sync ( QCRIL_QMI_CLIENT_IMS_SETTING,
                                                       QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG_REQ_V01,
                                                       NULL,
                                                       0,
                                                       &qmi_ims_get_reg_config_resp,
                                                       sizeof(qmi_ims_get_reg_config_resp) );
   QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error );

   if (E_SUCCESS == qmi_client_error)
   {
      if( qmi_ims_get_reg_config_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
      {
          qcril_qmi_imss_info_lock();
          qcril_qmi_ims_cached_info.imss_reg_state_valid = qmi_ims_get_reg_config_resp.ims_test_mode_valid;
          qcril_qmi_ims_cached_info.imss_reg_state = (qmi_ims_get_reg_config_resp.ims_test_mode == TRUE ) ? ims_Registration_RegState_NOT_REGISTERED : ims_Registration_RegState_REGISTERED;
          qcril_qmi_imss_info_unlock();
          QCRIL_LOG_INFO(".. IMS has_state: %d, state: %d", qcril_qmi_ims_cached_info.imss_reg_state_valid, qcril_qmi_ims_cached_info.imss_reg_state);
      }
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_ims_reg_config

//===========================================================================
// qcril_qmi_imss_set_reg_mgr_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_reg_mgr_config_resp_hdlr
(
 const qcril_request_params_type *const params_ptr
)
{
  ims_settings_set_reg_mgr_config_rsp_msg_v01 *resp;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  boolean send_response = TRUE;
  boolean need_to_set_voice_domain_pref = FALSE;
  nas_voice_domain_pref_enum_type_v01 voice_domain_pref = NAS_VOICE_DOMAIN_PREF_PS_PREF_V01;
  qcril_reqlist_public_type req_info;

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

    QCRIL_LOG_INFO(".. Need to change voice domain pref? %s",
        feature_voice_dom_pref_on_toggle_ims_cap ? "Yes" : "No");

    do {
      if(TRUE != qcril_qmi_ims_cached_info.imss_state_change_requested)
      {
        QCRIL_LOG_ERROR("imss state change is not requested!!");
        break;
      }

      QCRIL_LOG_INFO("token id = 0x%x", qcril_log_get_token_id(params_ptr->t));
      if (qcril_reqlist_query(QCRIL_DEFAULT_INSTANCE_ID, params_ptr->t, &req_info) != E_SUCCESS)
      {
        QCRIL_LOG_ERROR("Reqlist entry for token id 0x%x is not found", params_ptr->t);
        break;
      }

      qcril_req_state_e_type state = req_info.state[QCRIL_DEFAULT_INSTANCE_ID];
      QCRIL_LOG_INFO("req state is %s", qcril_reqlist_lookup_state_name(state));
      if (state == QCRIL_REQ_COMPLETED_FAILURE)
      {
        QCRIL_LOG_INFO("state is QCRIL_REQ_COMPLETED_FAILURE!!");
        ril_err = RIL_E_GENERIC_FAILURE;
        break;
      }

      if(RIL_E_SUCCESS == ril_err)
      {
        qcril_qmi_imss_info_lock();
        if (feature_voice_dom_pref_on_toggle_ims_cap &&
            (qcril_qmi_ims_cached_info.imss_new_reg_state != ims_Registration_RegState_REGISTERED))
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
            qcril_qmi_ims_cached_info.imss_new_reg_state == ims_Registration_RegState_REGISTERED)
        {
          voice_domain_pref = NAS_VOICE_DOMAIN_PREF_CS_ONLY_V01;
          need_to_set_voice_domain_pref = TRUE;
        }
        qcril_qmi_imss_info_unlock();
      }
    } while (FALSE);

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
  else
  {
    QCRIL_LOG_ERROR("params_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_reg_mgr_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_set_qipcall_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_qipcall_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    ims_settings_set_qipcall_config_rsp_msg_v01 *resp;
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
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID, params_ptr->t, params_ptr->event_id, ril_err);
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_qipcall_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_get_qipcall_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_get_qipcall_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    ims_settings_get_qipcall_config_rsp_msg_v01 *resp = NULL;
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

        if (RIL_E_SUCCESS == ril_err)
        {
            if (QCRIL_EVT_IMS_SOCKET_REQ_QUERY_VT_CALL_QUALITY == params_ptr->event_id)
            {
                ims_VideoCallQuality video_quality;
                memset(&video_quality, 0, sizeof(video_quality));

                QCRIL_LOG_INFO( "vt_quality_selector_valid: %d, vt_quality_selector: %d",
                                resp->vt_quality_selector_valid,
                                resp->vt_quality_selector );

                if (resp->vt_quality_selector_valid)
                {
                    if (resp->vt_quality_selector == IMS_SETTINGS_VT_QUALITY_LEVEL_0_V01)
                    {
                        video_quality.has_quality = TRUE;
                        video_quality.quality = ims_Quality_HIGH;
                    }
                    else if (resp->vt_quality_selector == IMS_SETTINGS_VT_QUALITY_LEVEL_2_V01)
                    {
                        video_quality.has_quality = TRUE;
                        video_quality.quality = ims_Quality_LOW;
                    }
                    else
                    {
                        ril_err = RIL_E_GENERIC_FAILURE;
                        QCRIL_LOG_ERROR("Unexpected value from modem\n");
                    }
                    imsRadioSendMessage(params_ptr->t,
                        ims_MsgType_RESPONSE,
                        ims_MsgId_REQUEST_QUERY_VT_CALL_QUALITY,
                        qcril_qmi_ims_map_ril_error_to_ims_error(ril_err),
                        (void *)&video_quality,
                        sizeof(video_quality));
                }
            }
        }
        else
        {
            qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                    params_ptr->t,
                    params_ptr->event_id,
                    ril_err);
        }
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_qipcall_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_set_client_provisioning_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_client_provisioning_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    ims_settings_set_client_provisioning_config_rsp_msg_v01 *resp;
    RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_FUNC_ENTRY();

    if (NULL != params_ptr)
    {
        resp = params_ptr->data;

        if (NULL != resp)
        {
            ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                    QMI_NO_ERR, &(resp->resp));
            QCRIL_LOG_ESSENTIAL("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
        }
        else
        {
            QCRIL_LOG_ERROR("params_ptr->data is NULL");
        }

        QCRIL_LOG_INFO("event_id = %d", params_ptr->event_id);

        if (params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS)
        {
            qcril_qmi_imss_set_wifi_pref_to_qcril_data(params_ptr);
        }

        qcril_send_empty_payload_request_response(
                QCRIL_DEFAULT_INSTANCE_ID,
                params_ptr->t,
                params_ptr->event_id,
                ril_err);
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_client_provisioning_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_get_client_provisioning_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_get_client_provisioning_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
    ims_settings_get_client_provisioning_config_rsp_msg_v01 *resp = NULL;
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

        if (RIL_E_SUCCESS == ril_err)
        {
            if (QCRIL_EVT_IMS_SOCKET_REQ_GET_WIFI_CALLING_STATUS == params_ptr->event_id)
            {
                ims_WifiCallingInfo  wifi_calling_info;
                memset(&wifi_calling_info, 0, sizeof(wifi_calling_info));

                QCRIL_LOG_INFO("wifi_call_valid: %d, wifi_call: %d "
                        "wifi_call_preference_valid: %d, wifi_call_preference: %d",
                        resp->wifi_call_valid, resp->wifi_call,
                        resp->wifi_call_preference_valid, resp->wifi_call_preference);

                if (resp->wifi_call_valid)
                {
                    wifi_calling_info.has_status =
                        qcril_qmi_ims_map_ims_settings_wfc_status_to_wificallingstatus(
                                resp->wifi_call, &wifi_calling_info.status);
                }
                if (resp->wifi_call_preference_valid)
                {
                    wifi_calling_info.has_preference =
                        qcril_qmi_ims_map_ims_settings_wfc_preference_to_wificallingpreference(
                                resp->wifi_call_preference, &wifi_calling_info.preference);
                }
                imsRadioSendMessage(params_ptr->t,
                        ims_MsgType_RESPONSE,
                        qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
                        qcril_qmi_ims_map_ril_error_to_ims_error(ril_err),
                        (void *)&wifi_calling_info,
                        sizeof(wifi_calling_info));
            }
        }
        else
        {
            qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                    params_ptr->t,
                    params_ptr->event_id,
                    ril_err);
        }
    }
    else
    {
        QCRIL_LOG_ERROR("params_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_client_provisioning_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_get_handover_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_get_handover_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  ims_settings_get_handover_config_rsp_msg_v01 *resp = NULL;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  if (NULL != params_ptr)
  {
    resp = params_ptr->data;

    if (NULL != resp)
    {
      ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR,
              &(resp->resp));
      QCRIL_LOG_INFO("ril_err: %d, qmi res: %d", (int) ril_err, (int)resp->resp.error);
    }
    else
    {
      QCRIL_LOG_ERROR("params_ptr->data is NULL");
    }

    if (RIL_E_SUCCESS == ril_err)
    {
      if (QCRIL_EVT_IMS_SOCKET_REQ_GET_HANDOVER_CONFIG == params_ptr->event_id)
      {
        ims_HandoverConfigMsg  handover_config;
        memset(&handover_config, 0, sizeof(handover_config));

        QCRIL_LOG_INFO("ims_pdn_ho_enabled_valid: %d, ims_pdn_ho_enabled_valid: %d",
                resp->ims_pdn_ho_enabled_valid, resp->ims_pdn_ho_enabled);

        if (resp->ims_pdn_ho_enabled_valid)
        {
          handover_config.has_hoConfig =
              qcril_qmi_ims_map_ims_settings_ho_config_to_ims_ho_config(
                      resp->ims_pdn_ho_enabled, &handover_config.hoConfig);
        }
        imsRadioSendMessage(params_ptr->t,
                ims_MsgType_RESPONSE,
                qcril_qmi_ims_map_event_to_request(params_ptr->event_id),
                qcril_qmi_ims_map_ril_error_to_ims_error(ril_err),
                (void *)&handover_config,
                sizeof(handover_config));
      }
    }
    else
    {
      qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
              params_ptr->t,
              params_ptr->event_id,
              ril_err);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} //qcril_qmi_imss_get_handover_config_resp_hdlr


//===========================================================================
// qcril_qmi_imss_set_handover_config_resp_hdlr
//===========================================================================
void qcril_qmi_imss_set_handover_config_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
   ims_settings_set_handover_config_rsp_msg_v01 *resp;
   RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;

   QCRIL_LOG_FUNC_ENTRY();

   if (NULL != params_ptr)
   {
      resp = params_ptr->data;

      if (NULL != resp)
      {
         ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &(resp->resp));
      }
      else
      {
         QCRIL_LOG_ERROR("params_ptr->data is NULL");
      }

      if( RIL_E_SUCCESS != ril_err )
      {
         QCRIL_LOG_INFO(".. Failed to change IMS handover config");
      }

      qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID, params_ptr->t, params_ptr->event_id, ril_err);
   }
   else
   {
      QCRIL_LOG_ERROR("params_ptr is NULL");
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_set_handover_config_resp_hdlr

//===========================================================================
// qcril_qmi_imss_extract_config_item_from_reqlist_data
//
// Functionality:
// 1. Converts the req_data stored in the reqlist to
//    integer and returns
// 2. If the req data is not valid, it returns -1
//===========================================================================
int qcril_qmi_imss_extract_config_item_from_reqlist_data(void *req_data, size_t req_datalen)
{
  qcril_qmi_radio_config_req_data_type *config_data = NULL;
  QCRIL_LOG_FUNC_ENTRY();
  int ret = QCRIL_QMI_RADIO_CONFIG_INVALID;
  if(req_datalen > 0 && req_data != NULL)
  {
    config_data = (qcril_qmi_radio_config_req_data_type *)req_data;
    ret = config_data->config_item;
    QCRIL_LOG_INFO("Config Item found: %d", ret);
  }
  else
  {
    QCRIL_LOG_ERROR("No req data/len Stored. Cant process the response further..len: %d", req_datalen);
  }
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

//===========================================================================
// qcril_qmi_imss_get_ims_config_log_and_send_response
//
// Functionality:
// 1. Reads config item and data from the config params
// 2. If the error is SUCCESS
//    Depending on the type of the items value,
//    process and log the data and generate the
//    response to be sent
// 3. If error reports a failure then check for settings error
//    form the config response accordingly
// 4. And send the reponse with the formed structure
// 5. If there are no config params then send empty response
//    with failure
//===========================================================================
void qcril_qmi_imss_get_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type * const config_params,
   qcril_qmi_radio_config_error_type radio_config_error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_ConfigMsg ims_resp;

  memset(&ims_resp, 0, sizeof(ims_resp));
  qcril_qmi_radio_config_item_value_type item_value_type;

  bool send_response = FALSE;

  if(config_params != NULL)
  {
    /*Get the ims config item from the radio config item and put in resp structure */
    ims_resp.has_item = TRUE;
    ims_resp.item =
        qcril_qmi_ims_map_radio_config_to_ims_config_item(config_params->config_item);

    /* Get the item value type */
    item_value_type = qcril_qmi_radio_config_get_item_value_type(config_params->config_item);

    /* If the config_item_value exists in config params and error is SUCCESS then
       Process(and log) the config_params data accordingly */
    if(radio_config_error == QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
       config_params->config_item_value != NULL &&
       config_params->config_item_value_len >0 &&
       config_params->config_item_value_type == item_value_type)
    {
      /* Evaluate and then assign value correctly */
      switch(item_value_type)
      {
        case QCRIL_QMI_RADIO_CONFIG_ITEM_IS_INT:
          // If the type is integer
          ims_resp.has_intValue = TRUE;
          ims_resp.intValue = *((uint32_t *)config_params->config_item_value);
          send_response = TRUE;
          QCRIL_LOG_INFO("Config value for %s: %d",
                          qcril_qmi_radio_config_get_item_log_str(config_params->config_item),
                          ims_resp.intValue);
          break;

        case QCRIL_QMI_RADIO_CONFIG_ITEM_IS_BOOLEAN:
          // If the type is boolean
          ims_resp.has_intValue = TRUE;
          ims_resp.intValue = *((bool *) config_params->config_item_value);
          send_response = TRUE;
          QCRIL_LOG_INFO("Config value for %s: %d",
                          qcril_qmi_radio_config_get_item_log_str(config_params->config_item),
                          ims_resp.intValue);
          break;

        case QCRIL_QMI_RADIO_CONFIG_ITEM_IS_STRING:
          // If the type is String
          ims_resp.stringValue.arg = qcril_malloc(config_params->config_item_value_len + 1);
          if(ims_resp.stringValue.arg != NULL)
          {
            strlcpy( ims_resp.stringValue.arg,
                    (char *)config_params->config_item_value,
                    config_params->config_item_value_len + 1);
            send_response = TRUE;
            QCRIL_LOG_INFO("Config value for %s: %s",
                            qcril_qmi_radio_config_get_item_log_str(config_params->config_item),
                            ims_resp.stringValue.arg);
          }
          else
          {
            QCRIL_LOG_ERROR("Malloc failure for string value, send empty failure resp");
            radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
            send_response = FALSE;
          }
          break;

        default:
          QCRIL_LOG_ERROR("Unknown data type");
          send_response = FALSE;
          radio_config_error = QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
          break;

      }
    }
    /* If the value is not sent or it is a failure
       then send error cause in the response. */
    else
    {
      ims_resp.has_errorCause = TRUE;
      ims_resp.errorCause =
           qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause(settings_resp);
      QCRIL_LOG_INFO("sending ConfigFailureCause..%d", ims_resp.errorCause);
      send_response = TRUE;
    }
    if(send_response)
    {
      imsRadioSendMessage( t,
                             ims_MsgType_RESPONSE,
                             ims_MsgId_REQUEST_GET_IMS_CONFIG,
                             qcril_qmi_ims_map_radio_config_error_to_ims_error(radio_config_error),
                             (void *)&ims_resp,
                             sizeof(ims_resp));
      qcril_qmi_npb_release(ims_ConfigMsg_fields, &ims_resp);
    }
  }
  if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS &&
     !send_response)
  {
    QCRIL_LOG_INFO("..sending NULL response");
    imsRadioSendMessage( t,
                           ims_MsgType_RESPONSE,
                           ims_MsgId_REQUEST_GET_IMS_CONFIG,
                           qcril_qmi_ims_map_radio_config_error_to_ims_error(radio_config_error),
                           NULL, 0);
  }
  QCRIL_LOG_FUNC_RETURN();
}// qcril_qmi_imss_get_ims_config_log_and_send_response

//===========================================================================
// qcril_qmi_imss_set_ims_config_log_and_send_response
//
// Functionality:
// 1. Get and log the config item from the params
// 2. If the error is FAILURE then put the settings error
//    in the response structure
// 3. Send the response with response structure formed
//    accordingly
// 5. If there are no config params then send empty response
//    with failure
//===========================================================================
void qcril_qmi_imss_set_ims_config_log_and_send_response
(
   RIL_Token t,
   const qcril_qmi_radio_config_params_type * const config_params,
   qcril_qmi_radio_config_error_type radio_config_error,
   qcril_qmi_radio_config_settings_resp_type settings_resp
)
{
  QCRIL_LOG_FUNC_ENTRY();

  ims_ConfigMsg ims_resp;
  memset(&ims_resp, 0, sizeof(ims_resp));

  if(config_params != NULL)
  {
    /* Get Config item */
    ims_resp.has_item = TRUE;
    ims_resp.item =
        qcril_qmi_ims_map_radio_config_to_ims_config_item(config_params->config_item);

    QCRIL_LOG_INFO("Config item: %s",
                    qcril_qmi_radio_config_get_item_log_str(config_params->config_item));

    /* If error reported then get error */
    if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      ims_resp.has_errorCause = TRUE;
      ims_resp.errorCause =
           qcril_qmi_ims_map_radio_config_settings_resp_to_ims_config_failcause(settings_resp);
      QCRIL_LOG_INFO("sending ConfigFailureCause..%d", ims_resp.errorCause);
    }
    imsRadioSendMessage( t,
                         ims_MsgType_RESPONSE,
                         ims_MsgId_REQUEST_SET_IMS_CONFIG,
                         qcril_qmi_ims_map_radio_config_error_to_ims_error(radio_config_error),
                         (void *)&ims_resp,
                         sizeof(ims_resp));
    qcril_qmi_npb_release(ims_ConfigMsg_fields, &ims_resp);
  }
  else
  {
    if(radio_config_error != QCRIL_QMI_RADIO_CONFIG_ERROR_SUCCESS)
    {
      QCRIL_LOG_INFO("..sending NULL response");
      imsRadioSendMessage( t,
                             ims_MsgType_RESPONSE,
                             ims_MsgId_REQUEST_SET_IMS_CONFIG,
                             qcril_qmi_ims_map_radio_config_error_to_ims_error(radio_config_error),
                             NULL, 0);
    }
  }
  QCRIL_LOG_FUNC_RETURN();
}// qcril_qmi_imss_set_ims_config_log_and_send_response


//===========================================================================
// qcril_qmi_imss_dispatch_config_response_helper
//
// Functionality:
// 1. Depending on req event id gets the specific handler
//    i.e. get - get_resp_handler and set - set_resp_handler
// 2. Stores the response params and config item
//    in the config params and calls the handler
//    with config params
//===========================================================================
void qcril_qmi_imss_dispatch_config_response_helper
(
   const qcril_request_params_type *const params_ptr,
   int config_item,
   unsigned long msg_id
)
{
  QCRIL_LOG_FUNC_ENTRY();
  qcril_qmi_radio_config_handler_type* resp_handler = NULL;
  qcril_qmi_radio_config_params_type config_params;
  memset(&config_params, 0, sizeof(config_params));


  bool send_failure_resp = FALSE;
  if(params_ptr != NULL)
  {
    if(config_item > QCRIL_QMI_RADIO_CONFIG_INVALID)
    {
        if(params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG)
        {
          QCRIL_LOG_INFO("get");
          resp_handler =
              qcril_qmi_radio_config_validate_and_find_get_config_resp_handler(config_item, msg_id);
        }
        else if(params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG)
        {
          QCRIL_LOG_INFO("set");
          resp_handler =
              qcril_qmi_radio_config_validate_and_find_set_config_resp_handler(config_item, msg_id);
        }
        //form up the function parameter with config item and other data in extra data
        config_params.config_item = config_item;
        config_params.config_item_value_len = 0;
        config_params.config_item_value = NULL;
        config_params.extra_data_len = sizeof(*params_ptr);
        config_params.extra_data = (void *)params_ptr;

        if(resp_handler != NULL )
        {
          (resp_handler)(&config_params);
          send_failure_resp = FALSE;
        }
        else
        {
          send_failure_resp = TRUE;
        }
    }
    else
    {
      send_failure_resp = TRUE;
    }
    /* If we need to send failure resp */
    if(send_failure_resp)
    {
      QCRIL_LOG_ERROR("Invalid config item stored in req list entry.Send failure resp");
      if(params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_GET_IMS_CONFIG)
      {
        qcril_qmi_imss_get_ims_config_log_and_send_response(params_ptr->t,
                                                            NULL,
                                                            QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE,
                                                            QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
      }
      else if(params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG)
      {
        qcril_qmi_imss_set_ims_config_log_and_send_response(params_ptr->t,
                                                            NULL,
                                                            QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE,
                                                            QCRIL_QMI_RADIO_CONFIG_SETTINGS_RESP_NO_ERR);
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR("params are null, cannot send response without token");
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_imss_command_cb_helper
//===========================================================================
void qcril_qmi_imss_command_cb_helper
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
               QCRIL_LOG_INFO("Transp error (%d) recieved from QMI for RIL request %d", qmi_resp_callback->transp_err, req_info.request);
               /* Send GENERIC_FAILURE response */
               qcril_send_empty_payload_request_response( instance_id, req_info.t, req_info.request, RIL_E_GENERIC_FAILURE );
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
               case QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_RSP_V01:
                  qcril_qmi_imss_set_reg_mgr_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_SET_QIPCALL_CONFIG_RSP_V01:
                  qcril_qmi_imss_set_qipcall_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_GET_QIPCALL_CONFIG_RSP_V01:
                  qcril_qmi_imss_get_qipcall_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_SET_CLIENT_PROVISIONING_CONFIG_RSP_V01:
                  qcril_qmi_imss_set_client_provisioning_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_RSP_V01:
                  qcril_qmi_imss_get_client_provisioning_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_GET_HANDOVER_CONFIG_RSP_V01:
                  qcril_qmi_imss_get_handover_config_resp_hdlr(&req_data);
                  break;

               case QMI_IMS_SETTINGS_SET_HANDOVER_CONFIG_RSP_V01:
                  qcril_qmi_imss_set_handover_config_resp_hdlr(&req_data);
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
} // qcril_qmi_imss_command_cb_helper

//===========================================================================
// qcril_qmi_imss_command_cb
//===========================================================================
void qcril_qmi_imss_command_cb
(
   qmi_client_type              user_handle,
   unsigned int                 msg_id,
   void                        *resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
)
{
   qmi_resp_callback_type qmi_resp_callback;
   memset(&qmi_resp_callback, 0, sizeof(qmi_resp_callback));
   QCRIL_LOG_FUNC_ENTRY();
   QCRIL_LOG_INFO(".. msg id %.2x", (int) msg_id );
   qmi_resp_callback.user_handle = user_handle;
   qmi_resp_callback.msg_id = msg_id;
   qmi_resp_callback.data_buf = (void*) resp_c_struct;
   qmi_resp_callback.data_buf_len = resp_c_struct_len;
   qmi_resp_callback.cb_data = resp_cb_data;
   qmi_resp_callback.transp_err = transp_err;
   qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                      QCRIL_DEFAULT_MODEM_ID,
                      QCRIL_DATA_ON_STACK,
                      QCRIL_EVT_QMI_IMSS_HANDLE_COMM_CALLBACKS,
                      (void*) &qmi_resp_callback,
                      sizeof(qmi_resp_callback),
                      (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_command_cb

//===========================================================================
// qcril_qmi_imss_get_client_provisioning_config
//===========================================================================
void qcril_qmi_imss_get_client_provisioning_config()
{
   IxErrnoType qmi_client_error;
   ims_settings_get_client_provisioning_config_rsp_msg_v01 qmi_ims_get_client_provisioning_config_resp;
   uint8_t wifi_call_valid = FALSE;
   ims_settings_wfc_status_enum_v01 wifi_call;
   uint8_t wifi_call_preference_valid = FALSE;
   ims_settings_wfc_preference_v01 wifi_call_preference;

   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_imss_info_lock();
   wifi_call_valid = qcril_qmi_ims_cached_info.wifi_calling_enabled_valid;
   wifi_call = qcril_qmi_imss_convert_ril_to_imss_wfc_status(
           qcril_qmi_ims_cached_info.wifi_calling_enabled);
   wifi_call_preference_valid = qcril_qmi_ims_cached_info.call_mode_preference_valid;
   wifi_call_preference = qcril_qmi_imss_convert_ril_to_imss_wfc_preference(
           qcril_qmi_ims_cached_info.call_mode_preference);
   qcril_qmi_imss_info_unlock();

   if(wifi_call_preference_valid && wifi_call_valid)
   {
      QCRIL_LOG_INFO("wifi_call status %d, wifi_call_preference: %d", wifi_call, wifi_call_preference);
      return;
   }

   memset(&qmi_ims_get_client_provisioning_config_resp, 0x0, sizeof(qmi_ims_get_client_provisioning_config_resp));
   qmi_client_error = qcril_qmi_client_send_msg_sync ( QCRIL_QMI_CLIENT_IMS_SETTING,
                                                       QMI_IMS_SETTINGS_GET_CLIENT_PROVISIONING_CONFIG_REQ_V01,
                                                       NULL,
                                                       0,
                                                       &qmi_ims_get_client_provisioning_config_resp,
                                                       sizeof(qmi_ims_get_client_provisioning_config_resp) );
   QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error );

   if (E_SUCCESS == qmi_client_error)
   {
      if( qmi_ims_get_client_provisioning_config_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
      {
          qcril_qmi_imss_info_lock();
          qcril_qmi_ims_cached_info.wifi_calling_enabled_valid = qmi_ims_get_client_provisioning_config_resp.wifi_call_valid;
          qcril_qmi_ims_cached_info.wifi_calling_enabled = qcril_qmi_imss_convert_imss_to_ril_wfc_status(
            qmi_ims_get_client_provisioning_config_resp.wifi_call);
          qcril_qmi_ims_cached_info.call_mode_preference_valid = qmi_ims_get_client_provisioning_config_resp.wifi_call_preference_valid;
          qcril_qmi_ims_cached_info.call_mode_preference = qcril_qmi_imss_convert_imss_to_ril_wfc_preference(
          qmi_ims_get_client_provisioning_config_resp.wifi_call_preference);
          QCRIL_LOG_INFO(".. client_prov_enabled_valid: %d, client_prov_enabled: %d",
            qmi_ims_get_client_provisioning_config_resp.enable_client_provisioning_valid,
            qmi_ims_get_client_provisioning_config_resp.enable_client_provisioning);
          QCRIL_LOG_INFO(".. wifi_call_valid: %d, wifi_call: %d", qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
            qcril_qmi_ims_cached_info.wifi_calling_enabled);
          QCRIL_LOG_INFO(".. wifi_call_preference_valid: %d, wifi_call_preference: %d", qcril_qmi_ims_cached_info.call_mode_preference_valid,
            qcril_qmi_ims_cached_info.call_mode_preference);
          qcril_qmi_imss_info_unlock();
      }
   }

   QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_get_client_provisioning_config

uint8_t qcril_qmi_imss_client_provisioning_wlan_status()
{
    uint8_t status = FALSE;
    uint8_t wifi_call_valid = FALSE;
    ims_settings_wfc_status_enum_v01 wifi_call;
    uint8_t wifi_call_preference_valid = FALSE;
    ims_settings_wfc_preference_v01 wifi_call_preference;

    if(1 == modem_version)
    {
        qcril_qmi_imss_get_ims_service_enable_config();
    }
    else
    {
        qcril_qmi_imss_get_client_provisioning_config();
    }

    qcril_qmi_imss_info_lock();
    wifi_call_valid = qcril_qmi_ims_cached_info.wifi_calling_enabled_valid;
    wifi_call = qcril_qmi_imss_convert_ril_to_imss_wfc_status(
      qcril_qmi_ims_cached_info.wifi_calling_enabled);
    wifi_call_preference_valid = qcril_qmi_ims_cached_info.call_mode_preference_valid;
    wifi_call_preference = qcril_qmi_imss_convert_ril_to_imss_wfc_preference(
      qcril_qmi_ims_cached_info.call_mode_preference);
    qcril_qmi_imss_info_unlock();

    QCRIL_LOG_DEBUG( "WiFi Call valid %d, WiFi Call %d", wifi_call_valid, wifi_call);
    QCRIL_LOG_DEBUG( "WiFi Call pref valid %d, WiFi Call pref %d", wifi_call_preference_valid, wifi_call_preference);

    if(wifi_call_preference_valid && wifi_call_valid && (IMS_SETTINGS_WFC_STATUS_ON_V01 == wifi_call) )
    {
        if( (IMS_SETTINGS_WFC_WLAN_PREFERRED_V01 == wifi_call_preference) ||
            (IMS_SETTINGS_WFC_WLAN_ONLY_V01 == wifi_call_preference)
          )
        {
            status = TRUE;
        }
    }

    QCRIL_LOG_INFO("wifi_call_preference STATUS: %d", status);
    return status;
}

//===========================================================================
// qcril_qmi_imss_clear_client_provisioning_config_info
//===========================================================================
void qcril_qmi_imss_clear_client_provisioning_config_info()
{
   QCRIL_LOG_FUNC_ENTRY();

   qcril_qmi_imss_info_lock();
   qcril_qmi_ims_cached_info.wifi_calling_enabled_valid = FALSE;
   qcril_qmi_ims_cached_info.wifi_calling_enabled = qcril_qmi_imss_convert_imss_to_ril_wfc_status(
     IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01);
   qcril_qmi_ims_cached_info.call_mode_preference_valid = FALSE;
   qcril_qmi_ims_cached_info.call_mode_preference =
     qcril_qmi_imss_convert_imss_to_ril_wfc_preference(IMS_SETTINGS_WFC_CALL_PREF_NONE_V01);
   qcril_qmi_imss_info_unlock();

   QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_imss_reg_mgr_config_ind_hdlr
//===========================================================================
void qcril_qmi_imss_reg_mgr_config_ind_hdlr(void *ind_data_ptr)
{
    ims_settings_reg_mgr_config_ind_msg_v01 *reg_mgr_cfg_ptr = NULL;

    QCRIL_LOG_FUNC_ENTRY();

    reg_mgr_cfg_ptr = (ims_settings_reg_mgr_config_ind_msg_v01*) ind_data_ptr;
    if(NULL != reg_mgr_cfg_ptr)
    {
        QCRIL_LOG_INFO(".. ims_test_mode_valid: %d, ims_test_mode: %d",
                                        reg_mgr_cfg_ptr->ims_test_mode_valid,
                                        reg_mgr_cfg_ptr->ims_test_mode);

        if(reg_mgr_cfg_ptr->ims_test_mode_valid)
        {
            qcril_qmi_imss_info_lock();
            qcril_qmi_ims_cached_info.imss_reg_state_valid = TRUE;
            qcril_qmi_ims_cached_info.imss_reg_state =
                                    ((TRUE == reg_mgr_cfg_ptr->ims_test_mode)?
                                      ims_Registration_RegState_NOT_REGISTERED:
                                      ims_Registration_RegState_REGISTERED);

            QCRIL_LOG_INFO(".. IMS state changed to %d\n",
                                qcril_qmi_ims_cached_info.imss_reg_state);
            qcril_qmi_imss_info_unlock();
        }
    }
    else
    {
        QCRIL_LOG_ERROR("ind_data_ptr is NULL");
    }

    QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
// qcril_qmi_imss_client_provisioning_config_ind_hdlr
//===========================================================================
void qcril_qmi_imss_client_provisioning_config_ind_hdlr(void *ind_data_ptr)
{
    ims_settings_client_provisioning_config_ind_msg_v01 *reg_ind_msg_ptr = (ims_settings_client_provisioning_config_ind_msg_v01*) ind_data_ptr;
    uint8_t unsol_nw_status = FALSE;

    QCRIL_LOG_FUNC_ENTRY();

    if(NULL != reg_ind_msg_ptr)
    {
        qcril_qmi_imss_info_lock();
        qcril_qmi_ims_cached_info.wifi_calling_enabled_valid =
          reg_ind_msg_ptr->wifi_call_valid;
        qcril_qmi_ims_cached_info.wifi_calling_enabled =
          qcril_qmi_imss_convert_imss_to_ril_wfc_status(reg_ind_msg_ptr->wifi_call);
        qcril_qmi_ims_cached_info.call_mode_preference_valid =
          reg_ind_msg_ptr->wifi_call_preference_valid;
        qcril_qmi_ims_cached_info.call_mode_preference =
          qcril_qmi_imss_convert_imss_to_ril_wfc_preference(reg_ind_msg_ptr->wifi_call_preference);
        QCRIL_LOG_INFO(".. client_prov_enabled_valid: %d, client_prov_enabled: %d",
          reg_ind_msg_ptr->client_provisioning_enabled_valid,
          reg_ind_msg_ptr->client_provisioning_enabled);
        QCRIL_LOG_INFO(".. wifi_call_valid: %d, wifi_call: %d",
          qcril_qmi_ims_cached_info.wifi_calling_enabled_valid,
          qcril_qmi_ims_cached_info.wifi_calling_enabled);
        QCRIL_LOG_INFO(".. wifi_call_preference_valid: %d, wifi_call_preference: %d",
          qcril_qmi_ims_cached_info.call_mode_preference_valid,
          qcril_qmi_ims_cached_info.call_mode_preference);
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
// qcril_qmi_imss_unsol_ind_cb_helper
//===========================================================================
void qcril_qmi_imss_unsol_ind_cb_helper
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
              case QMI_IMS_SETTINGS_REG_MGR_CONFIG_IND_V01:
                qcril_qmi_imss_reg_mgr_config_ind_hdlr(decoded_payload);
                break;

              case QMI_IMS_SETTINGS_CLIENT_PROVISIONING_CONFIG_IND_V01:
                qcril_qmi_imss_client_provisioning_config_ind_hdlr(decoded_payload);
                break;

              default:
                QCRIL_LOG_INFO("Unknown QMI IMSA indication %d", qmi_callback->msg_id);
                break;
            }
          }
          else
          {
              QCRIL_LOG_INFO("Indication decode failed for msg %d of svc %d with error %d", qmi_callback->msg_id, QCRIL_QMI_CLIENT_IMS_SETTING, qmi_err );
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

} // qcril_qmi_imss_unsol_ind_cb_helper

//===========================================================================
// qcril_qmi_imss_unsol_ind_cb
//===========================================================================
void qcril_qmi_imss_unsol_ind_cb
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void              *ind_buf,
  unsigned int       ind_buf_len,
  void              *ind_cb_data
)
{
  qmi_ind_callback_type qmi_callback;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_callback.data_buf = qcril_malloc(ind_buf_len);

  if( qmi_callback.data_buf )
  {
    qmi_callback.user_handle = user_handle;
    qmi_callback.msg_id = msg_id;
    memcpy(qmi_callback.data_buf,ind_buf,ind_buf_len);
    qmi_callback.data_buf_len = ind_buf_len;
    qmi_callback.cb_data = ind_cb_data;

    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                   QCRIL_DEFAULT_MODEM_ID,
                   QCRIL_DATA_ON_STACK,
                   QCRIL_EVT_QMI_IMSS_HANDLE_INDICATIONS,
                   (void*) &qmi_callback,
                   sizeof(qmi_callback),
                   (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }
  else
  {
    QCRIL_LOG_FATAL("malloc failed");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_imss_unsol_ind_cb

uint8_t qcril_qmi_imss_compare_wfc_pref_unsol()
{
    uint8_t ret = FALSE;
    uint8_t old_data_rat_status = FALSE;
    uint8_t imss_wlan_status = FALSE;
    uint8_t imsa_wlan_status = FALSE;
    uint8_t dsd_wlan_status = FALSE;
    uint32_t dsd_wlan_index = QMI_RIL_ZERO;
    uint8_t reported_tech_valid = FALSE;
    int reported_tech;
    uint8_t wifi_call_valid = FALSE;
    qcril_ims_setting_wfc_status_type wifi_call;
    uint8_t dsd_valid_rat_status = FALSE;
    uint32_t dsd_valid_rat_index = QMI_RIL_ZERO;

    QCRIL_LOG_FUNC_ENTRY();
    qcril_qmi_imss_fetch_wifi_call_info(&wifi_call_valid, &wifi_call);
    dsd_wlan_status = qcril_arb_check_wlan_rat_dsd_reported_helper(&dsd_wlan_index);
    old_data_rat_status = qcril_qmi_nas_fetch_data_reg_rat(&reported_tech_valid, &reported_tech);
    dsd_valid_rat_status = qcril_arb_find_index_rat_not_wlan_dsd_reported_helper(&dsd_valid_rat_index);
    QCRIL_LOG_INFO("WFC call valid %d, WFC status %d", wifi_call_valid, wifi_call);
    QCRIL_LOG_INFO("DSD WLAN status %d, WLAN index %d", dsd_wlan_status, dsd_wlan_index);
    QCRIL_LOG_INFO("DSD valid RAT status %d, RAT index %d", dsd_valid_rat_status, dsd_valid_rat_index);
    QCRIL_LOG_INFO("old data reg status %d, reported tech %d", old_data_rat_status, reported_tech);

    if(!dsd_wlan_status)
    {
        return ret;
    }

    imss_wlan_status = qcril_qmi_imss_client_provisioning_wlan_status();
    QCRIL_LOG_INFO("WFC available over WLAN status %d", imss_wlan_status);

    if(imss_wlan_status || (wifi_call_valid && (QCRIL_IMS_SETTING_WFC_ON == wifi_call) && !dsd_valid_rat_status) )
    {
        if(!old_data_rat_status || (RADIO_TECH_IWLAN != reported_tech))
        {
            QCRIL_LOG_INFO("WFC available over WLAN: notify ATeL as needed");
            ret = TRUE;
        }
    }
    else
    {
        imsa_wlan_status = qcril_qmi_imsa_ims_registered_wlan_status();
        QCRIL_LOG_INFO("IMS registered over WLAN status %d", imsa_wlan_status);

        if(!imsa_wlan_status && old_data_rat_status && (RADIO_TECH_IWLAN == reported_tech))
        {
            QCRIL_LOG_INFO("WFC not available over WLAN: notify ATeL as needed");
            ret = TRUE;
        }
    }

    return ret;
}

void qcril_qmi_imss_fetch_wifi_call_info(uint8_t *wifi_call_valid, qcril_ims_setting_wfc_status_type *wifi_call)
{
    if(1 == modem_version)
    {
        qcril_qmi_imss_get_ims_service_enable_config();
    }
    else
    {
        qcril_qmi_imss_get_client_provisioning_config();
    }

    qcril_qmi_imss_info_lock();
    *wifi_call_valid = qcril_qmi_ims_cached_info.wifi_calling_enabled_valid;
    *wifi_call = qcril_qmi_ims_cached_info.wifi_calling_enabled;
    qcril_qmi_imss_info_unlock();
}

void qcril_qmi_imss_fetch_wifi_call_pref(uint8_t *wifi_call_preference_valid, qcril_ims_setting_wfc_preference_type *wifi_call_preference)
{
    if(1 == modem_version)
    {
        qcril_qmi_imss_get_ims_service_enable_config();
    }
    else
    {
        qcril_qmi_imss_get_client_provisioning_config();
    }

    qcril_qmi_imss_info_lock();
    *wifi_call_preference_valid = qcril_qmi_ims_cached_info.call_mode_preference_valid;
    *wifi_call_preference = qcril_qmi_ims_cached_info.call_mode_preference;
    qcril_qmi_imss_info_unlock();
}

//===========================================================================
// qcril_qmi_imss_store_wifi_call_preference_to_req_info
//===========================================================================
void qcril_qmi_imss_store_wifi_call_preference_to_req_info
(
 uint16                          req_id,
 ims_settings_wfc_preference_v01 wifi_call_preference
)
{
    qcril_qmi_radio_config_req_data_type *config_data = NULL;
    qcril_reqlist_public_type             req_info;
    qcril_instance_id_e_type              instance_id = QCRIL_DEFAULT_INSTANCE_ID;
    qcril_ims_setting_wfc_preference_type *wfc_pref    = NULL;

    QCRIL_LOG_INFO("store wifi_call_preference = %d to req_id %d\n",
            wifi_call_preference, req_id);

    if ((qcril_reqlist_query_by_req_id(req_id, &instance_id, &req_info ) == E_SUCCESS) &&
            req_info.req_data)
    {
        wfc_pref = (qcril_ims_setting_wfc_preference_type *)qcril_malloc(
                sizeof(qcril_ims_setting_wfc_preference_type));
        if (wfc_pref)
        {
            *wfc_pref = qcril_qmi_imss_convert_imss_to_ril_wfc_preference(wifi_call_preference);
            config_data = (qcril_qmi_radio_config_req_data_type *)req_info.req_data;
            config_data->data_len = sizeof(qcril_ims_setting_wfc_preference_type);
            config_data->data = (void *)wfc_pref;
        }
    }
} // qcril_qmi_imss_store_wifi_call_preference_to_req_info

//===========================================================================
// qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info
//===========================================================================
boolean qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info
(
 qcril_reqlist_public_type       *req_info,
 qcril_ims_setting_wfc_preference_type *wfc_preference
)
{
    qcril_qmi_radio_config_req_data_type *config_data = NULL;
    boolean                               ret         = FALSE;

    if (wfc_preference && req_info && req_info->req_data)
    {
        config_data = (qcril_qmi_radio_config_req_data_type *)(req_info->req_data);
        if (config_data->data)
        {
            *wfc_preference = *(qcril_ims_setting_wfc_preference_type *)(config_data->data);
            QCRIL_LOG_INFO("retrieved wfc_preference = %d", *wfc_preference);
            qcril_free(config_data->data);
            config_data->data = NULL;
            ret = TRUE;
        }
    }

    return ret;
} // qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info

//===========================================================================
// qcril_qmi_imss_set_wifi_pref_to_qcril_data
//===========================================================================
RIL_Errno qcril_qmi_imss_set_wifi_pref_to_qcril_data
(
 const qcril_request_params_type *params_ptr
)
{
    qcril_reqlist_public_type       req_info;
    qcril_ims_setting_wfc_preference_type wifi_call_preference  = QCRIL_IMS_SETTING_WFC_PREF_NONE;
    int                             dsd_rat_preference = DSD_RAT_PREFERENCE_INACTIVE_V01;
    ims_WifiCallingInfo            *ims_wci_data_ptr = NULL;
    uint8_t                         wifi_call_valid = FALSE;
    qcril_ims_setting_wfc_status_type wifi_call;
    boolean                         is_wfc_off = FALSE;
    RIL_Errno                       ril_err          = RIL_E_GENERIC_FAILURE;

    QCRIL_LOG_INFO("Set wifi preference to QCRIL DATA");

    do
    {
        if (!params_ptr)
        {
            QCRIL_LOG_INFO("Invalid parameter");
            break;
        }
        if ((qcril_reqlist_query(QCRIL_DEFAULT_INSTANCE_ID, params_ptr->t, &req_info) !=
                    E_SUCCESS) ||
                !req_info.req_data)
        {
            QCRIL_LOG_INFO("Cannot find req_info");
            break;
        }

        // Check from cache or modem, if WFC is off
        qcril_qmi_imss_fetch_wifi_call_info(&wifi_call_valid, &wifi_call);
        is_wfc_off = ((wifi_call_valid == 1) && (wifi_call == QCRIL_IMS_SETTING_WFC_OFF));
        // Check event
        if (params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_SET_WIFI_CALLING_STATUS)
        {
            ims_wci_data_ptr = (ims_WifiCallingInfo *)req_info.req_data;
            QCRIL_LOG_INFO("has_preference = %d, preference = %d; has_status = %d,\
                    status = %d", ims_wci_data_ptr->has_preference, ims_wci_data_ptr->preference,
                    ims_wci_data_ptr->has_status, ims_wci_data_ptr->status);
            // Use the value from req buffer as higher priority
            if (ims_wci_data_ptr->has_status)
            {
                is_wfc_off = (ims_wci_data_ptr->status == ims_WifiCallingStatus_WIFI_STATUS_OFF);
            }
            // If WFC is off, force rat_pref as celluar only. Otherwise map the rat_pref
            // from wfc_pref
            if (is_wfc_off)
            {
                dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01;
            }
            else if (!ims_wci_data_ptr->has_preference ||
                     !qcril_qmi_ims_map_wificallingpreference_to_dsd_rat_preference(
                            ims_wci_data_ptr->preference, &dsd_rat_preference))
            {
              QCRIL_LOG_INFO("failed to map preference");
                break;
            }
        }
        else if (params_ptr->event_id == QCRIL_EVT_IMS_SOCKET_REQ_SET_IMS_CONFIG)
        {
            // If WFC is off, force rat_pref as celluar only. Otherwise map the rat_pref
            // from wfc_pref that is stored in req info.
            if (is_wfc_off)
            {
                dsd_rat_preference = DSD_RAT_PREFERENCE_CELLULAR_ONLY_V01;
            }
            else if (!qcril_qmi_imss_retrieve_wifi_call_preference_from_req_info(
                            &req_info, &wifi_call_preference) ||
                     !qcril_qmi_ims_map_ims_settings_wfc_preference_to_dsd_rat_preference(
                            wifi_call_preference, &dsd_rat_preference))
            {
                QCRIL_LOG_INFO("failed to map preference");
                break;
            }
        }
        else
        {
            QCRIL_LOG_INFO("Invalid parameters");
            break;
        }

        // Set RAT preference from QCRIL DATA;
#ifndef QMI_RIL_UTF
            ril_err = qcril_data_set_rat_preference(dsd_rat_preference);
            QCRIL_LOG_INFO("qcril_data_set_rat_preference res = %d\n", ril_err);
            if (ril_err != RIL_E_SUCCESS)
            {
                QCRIL_LOG_ERROR("QCRIL DATA API returned error");
            }
#endif
    } while (FALSE);

    return ril_err;
} // qcril_qmi_imss_set_wifi_pref_to_qcril_data

/**
*  This fuction checks if the property persist.vendor.radio.disable_modem_cfg is enabled.
*  If this property is enabled all modem IMS NV configuration changes are blocked.
*/
boolean is_modem_ims_config_disable()
{
  char  prop_str[PROPERTY_VALUE_MAX]    = {0};

  property_get(QMI_RIL_DISABLE_MODEM_CONFIG, prop_str, "");
  return (strncmp(prop_str, "true", QMI_RIL_DISABLE_MODEM_CONFIG_LENGTH) == 0);
}


//===========================================================================
// qcril_qmi_imss_set_ims_test_mode_enabled
//===========================================================================
RIL_Errno qcril_qmi_imss_set_ims_test_mode_enabled(uint8_t ims_test_mode_enabled, uint16 req_id)
{
  IxErrnoType qmi_client_error;
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  uint32 user_data;

  ims_settings_set_reg_mgr_config_req_msg_v01 qmi_req;
  ims_settings_set_reg_mgr_config_rsp_msg_v01 *qmi_resp = NULL;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("ims_test_mode_enabled = %s",
          ims_test_mode_enabled ? "TRUE" : "FALSE");

  memset(&qmi_req, 0, sizeof(qmi_req));
  qmi_req.ims_test_mode_enabled_valid = TRUE;
  qmi_req.ims_test_mode_enabled = ims_test_mode_enabled;
  qmi_resp = qcril_malloc(sizeof(*qmi_resp));

  if (NULL != qmi_resp)
  {
    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID, QCRIL_DEFAULT_MODEM_ID, req_id);

    qmi_client_error = qcril_qmi_client_send_msg_async (
            QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_SET_REG_MGR_CONFIG_REQ_V01,
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
} // qcril_qmi_imss_set_ims_test_mode_enabled

//===========================================================================
// qcril_qmi_imss_set_voice_domain_preference_follow_up
//===========================================================================
void qcril_qmi_imss_set_voice_domain_preference_follow_up(uint16 req_id)
{
  RIL_Errno res = RIL_E_GENERIC_FAILURE;
  uint8_t imss_state_change_requested = FALSE;
  ims_Registration_RegState imss_new_reg_state = ims_Registration_RegState_NOT_REGISTERED;
  qcril_reqlist_public_type req_info;
  qcril_instance_id_e_type instance_id = QCRIL_DEFAULT_INSTANCE_ID;
  boolean send_response = TRUE;
  qcril_req_state_e_type state = QCRIL_REQ_COMPLETED_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_LOG_INFO("req_id = %d", req_id);

  if (qcril_reqlist_query_by_req_id(req_id, &instance_id, &req_info) == E_SUCCESS)
  {
    if(req_info.request == QCRIL_EVT_IMS_SOCKET_REQ_SET_VOLTE_PREF)
    {
      state = req_info.state[QCRIL_DEFAULT_INSTANCE_ID];
      QCRIL_LOG_INFO("req state is %s", qcril_reqlist_lookup_state_name(state));
      if (state != QCRIL_REQ_COMPLETED_FAILURE)
      {
        res = RIL_E_SUCCESS;
      }

      QCRIL_LOG_INFO("Send response for IMS_REQ SET_VOLTE_PREF - res: %s",
          (res == RIL_E_SUCCESS ? "SUCCESS" : "FAILURE"));
      qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
              req_info.t, req_info.request, res);
    }
    else
    {
      qcril_qmi_imss_info_lock();
      imss_state_change_requested = qcril_qmi_ims_cached_info.imss_state_change_requested;
      imss_new_reg_state = qcril_qmi_ims_cached_info.imss_new_reg_state;
      qcril_qmi_imss_info_unlock();

      if (imss_state_change_requested)
      {
        state = req_info.state[QCRIL_DEFAULT_INSTANCE_ID];
        QCRIL_LOG_INFO("req state is %s", qcril_reqlist_lookup_state_name(state));
        if (state != QCRIL_REQ_COMPLETED_FAILURE)
        {
          res = RIL_E_SUCCESS;
          // Enable IMS if new reg state is REGISTERED.
          if (imss_new_reg_state == ims_Registration_RegState_REGISTERED)
          {
            if(1 == modem_version)
            {
              res = qcril_qmi_imss_set_ims_service_enabled(TRUE, req_id);
            }
            else
            {
              res = qcril_qmi_imss_set_ims_test_mode_enabled(FALSE, req_id);
            }

            if (res == RIL_E_SUCCESS)
            {
              send_response = FALSE;
            }
          }
        }
      }

      if (send_response)
      {
        qcril_qmi_imss_info_lock();
        qcril_qmi_ims_cached_info.imss_state_change_requested = FALSE;
        qcril_qmi_imss_info_unlock();

        QCRIL_LOG_INFO("Send response: res: %s",
            (res == RIL_E_SUCCESS ? "SUCCESS" : "FAILURE"));
        qcril_send_empty_payload_request_response(QCRIL_DEFAULT_INSTANCE_ID,
                req_info.t, req_info.request, res);
      }
    }
  }
  else
  {
    QCRIL_LOG_ERROR( "Req ID: %d not found", req_id );
  }
  QCRIL_LOG_FUNC_RETURN();
}

//===========================================================================
//  qcril_qmi_imss_get_modem_version
//  @brief
//    modem version values:
//    0 - Old IMSS APIs
//    1 - New IMSS APIs
//===========================================================================
int qcril_qmi_imss_get_modem_version(void)
{
    IxErrnoType qmi_client_error;
    qmi_get_supported_msgs_resp_v01 qmi_resp;
    int modem_version = 0;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&qmi_resp, 0x0, sizeof(qmi_resp));
    qmi_client_error = qcril_qmi_client_send_msg_sync (QCRIL_QMI_CLIENT_IMS_SETTING,
            QMI_IMS_SETTINGS_GET_SUPPORTED_MSGS_REQ_V01,
            NULL,
            0,
            &qmi_resp,
            sizeof(qmi_resp));
    QCRIL_LOG_INFO(".. qmi send sync res %d", (int) qmi_client_error);

    if(E_SUCCESS == qmi_client_error)
    {
        if(qmi_resp.supported_msgs_valid)
        {
          //Use new modem version if ims service enable config request is supported.
          modem_version = qcril_qmi_ims_is_msg_supported(
            QMI_IMS_SETTINGS_SET_IMS_SERVICE_ENABLE_CONFIG_REQ_V01, qmi_resp.supported_msgs,
            qmi_resp.supported_msgs_len);
        }
        else
        {
            modem_version = 0;
        }
    }
    else
    {
        modem_version = 0;
    }
    return modem_version;
}

//===========================================================================
//  qcril_qmi_imss_convert_ril_to_imss_wfc_status
//===========================================================================
ims_settings_wfc_status_enum_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_status
(
  qcril_ims_setting_wfc_status_type wfc_status
)
{
  ims_settings_wfc_status_enum_v01 res;

  switch(wfc_status)
  {
    case QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED:
      res = IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_OFF:
      res = IMS_SETTINGS_WFC_STATUS_OFF_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_ON:
      res = IMS_SETTINGS_WFC_STATUS_ON_V01;
      break;

    default:
      res = IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_ril_to_imss_wfc_preference
//===========================================================================
ims_settings_wfc_preference_v01 qcril_qmi_imss_convert_ril_to_imss_wfc_preference
(
  qcril_ims_setting_wfc_preference_type wfc_preference
)
{
  ims_settings_wfc_preference_v01 res;

  switch(wfc_preference)
  {
    case QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED:
      res = IMS_SETTINGS_WFC_WLAN_PREFERRED_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_WLAN_ONLY:
      res = IMS_SETTINGS_WFC_WLAN_ONLY_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED:
      res = IMS_SETTINGS_WFC_CELLULAR_PREFERRED_V01;
      break;

    case QCRIL_IMS_SETTING_WFC_CELLULAR_ONLY:
      res = IMS_SETTINGS_WFC_CELLULAR_ONLY_V01;
      break;

    default:
      res = IMS_SETTINGS_WFC_CALL_PREF_NONE_V01;
  }
  return res;
}

//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_status
//===========================================================================
qcril_ims_setting_wfc_status_type qcril_qmi_imss_convert_imss_to_ril_wfc_status
(
  ims_settings_wfc_status_enum_v01 wfc_status
)
{
  qcril_ims_setting_wfc_status_type res;

  switch(wfc_status)
  {
    case IMS_SETTINGS_WFC_STATUS_NOT_SUPPORTED_V01:
      res = QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED;
      break;

    case IMS_SETTINGS_WFC_STATUS_OFF_V01:
      res = QCRIL_IMS_SETTING_WFC_OFF;
      break;

    case IMS_SETTINGS_WFC_STATUS_ON_V01:
      res = QCRIL_IMS_SETTING_WFC_ON;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED;
  }
  return res;
}
//===========================================================================
//  qcril_qmi_imss_convert_imss_to_ril_wfc_preference
//===========================================================================
qcril_ims_setting_wfc_preference_type qcril_qmi_imss_convert_imss_to_ril_wfc_preference
(
  ims_settings_wfc_preference_v01 wfc_preference
)
{
  qcril_ims_setting_wfc_preference_type res;

  switch(wfc_preference)
  {
    case IMS_SETTINGS_WFC_WLAN_PREFERRED_V01:
      res = QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED;
      break;

    case IMS_SETTINGS_WFC_WLAN_ONLY_V01:
      res = QCRIL_IMS_SETTING_WFC_WLAN_ONLY;
      break;

    case IMS_SETTINGS_WFC_CELLULAR_PREFERRED_V01:
      res = QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED;
      break;

    case IMS_SETTINGS_WFC_CELLULAR_ONLY_V01:
      res = QCRIL_IMS_SETTING_WFC_CELLULAR_ONLY;
      break;

    default:
      res = QCRIL_IMS_SETTING_WFC_PREF_NONE;
  }
  return res;
}
