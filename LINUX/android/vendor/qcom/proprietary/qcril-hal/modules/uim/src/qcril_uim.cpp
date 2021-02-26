
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "framework/Log.h"
#include "modules/uim/qcril_uim_legacy.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/uim/UimGetMccMncRequestMsg.h"
#include "modules/uim/UimGetFacilityLockRequestMsg.h"
#include "modules/uim/UimSetFacilityLockRequestMsg.h"
#include "modules/uim/UimRilRequestMessage.h"
#include "modules/uim/UimGetAtrRequestMsg.h"
#include "modules/android/qcril_android_event_context.h"
#include "modules/uim/qcril_uim_gba.h"
#include "qcril_uim_card.h"
#include "qcril_uim_file.h"
#include "qcril_uim_security.h"
#include "qcril_uim_refresh.h"
#include "qcril_uim_util.h"
#include "qcril_uim_restart.h"
#include "qcril_uim_slot_mapping.h"
#include "UimOemHookRequestMsg.h"
#include "UimQmiUimResponseMsg.h"
#include "UimModule.h"
#include <string.h>
extern "C"
{
  #include <cutils/properties.h>
}

#ifdef QMI_RIL_UTF
#include "GstkModule.h"
#include "LpaModule.h"
#endif

/* Android property to enable supply voltage feature */
#define QCRIL_UIM_PROPERTY_FEATURE_VCC                 "persist.vendor.radio.qcril_uim_vcc_feature"
#define QCRIL_UIM_PROP_FEATURE_VCC_ENABLED_VALUE       "1"

/* Global variable with QCRIL status */
qcril_uim_struct_type   qcril_uim;

/*=========================================================================

  FUNCTION:  qmi_uim_card_init_callback

===========================================================================*/
/*!
    @brief
    Special case callback for the intial card status QMI commands.

    @return
    None
*/
/*=========================================================================*/
void qmi_uim_card_init_callback
(
  qmi_uim_rsp_data_type        * rsp_data_ptr,
  void                         * user_data
)
{
  qmi_uim_indication_data_type     ind_data;
  qcril_uim_indication_params_type ind_param;
  qcril_request_return_type        ret;

  (void) user_data;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity check */
  if (rsp_data_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL rsp_data_ptr, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  memset(&ind_data, 0, sizeof(qmi_uim_indication_data_type));

  if(rsp_data_ptr->qmi_err_code >= 0)
  {
      QCRIL_LOG_QMI( QCRIL_DEFAULT_MODEM_ID, "qmi_uim_card_init_callback", "inside" );
      ind_data.status_change_ind.card_status = rsp_data_ptr->rsp_data.get_card_status_rsp.card_status;

      ind_data.status_change_ind.card_status_validity = rsp_data_ptr->rsp_data.get_card_status_rsp.card_status_validity;

      memset(&ind_param, 0x00, sizeof(ind_param));
      memset(&ret, 0x00, sizeof(ret));

      ind_param.ind_id      = QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG;
      ind_param.ind_data    = ind_data;
      ind_param.user_data   = user_data;
      ind_param.instance_id = qmi_ril_get_process_instance_id();

      qcril_uim_process_status_change_ind(&ind_param, &ret);
  }
} /* qmi_uim_card_init_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_response

===========================================================================*/
/*!
    @brief
    Send a response to framework for a specific request.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_response
(
  qcril_instance_id_e_type instance_id,
  RIL_Token                token,
  RIL_Errno                result,
  void*                    rsp_ptr,
  size_t                   rsp_len,
  boolean                  remove_entry,
  char*                    logstr
)
{
  UimRequestMsgData               *req_data_ptr = (UimRequestMsgData *)token;

  (void)remove_entry;

  if (req_data_ptr != NULL)
  {
    if(req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_GET_MCC_MNC)
    {
      std::shared_ptr<UimGetMccMncRequestMsg>  req_msg_ptr =
          std::static_pointer_cast<UimGetMccMncRequestMsg>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != nullptr)
      {
        auto resp_data_ptr = std::shared_ptr<qcril_mcc_mnc_info_type>(new qcril_mcc_mnc_info_type);

        if (resp_data_ptr != nullptr && rsp_ptr != nullptr)
        {
          memcpy(resp_data_ptr.get(), rsp_ptr, 
                 (sizeof(qcril_mcc_mnc_info_type) <= rsp_len) ? sizeof(qcril_mcc_mnc_info_type) : rsp_len);
        }
        req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
      }
    }
    else if(req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_GET_FACILITY_LOCK)
    {
      int                                            status        = 0;
      std::shared_ptr<UimGetFacilityLockRequestMsg>  req_msg_ptr   =
          std::static_pointer_cast<UimGetFacilityLockRequestMsg>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != nullptr)
      {
        std::shared_ptr<UimGetFacilityLockResponseMsg> resp_data_ptr = nullptr;
        if (rsp_ptr != nullptr)
        {
          status = *((int *)rsp_ptr);
        }
        resp_data_ptr = std::shared_ptr<UimGetFacilityLockResponseMsg>(new UimGetFacilityLockResponseMsg{result, status});
        req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
      }
    }
    else if(req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_SET_FACILITY_LOCK)
    {
      int                                            retries        = 0;
      std::shared_ptr<UimSetFacilityLockRequestMsg>  req_msg_ptr =
          std::static_pointer_cast<UimSetFacilityLockRequestMsg>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != NULL)
      {
        std::shared_ptr<UimSetFacilityLockResponseMsg> resp_data_ptr = nullptr;
        if (rsp_ptr != nullptr)
        {
          retries = *((int *)rsp_ptr);
        }
        resp_data_ptr = std::shared_ptr<UimSetFacilityLockResponseMsg>(new UimSetFacilityLockResponseMsg{result, retries});
        req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
      }
    }
    else if (req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_RIL_EVENT)
    {
      std::shared_ptr<UimRilRequestMessage>  req_msg_ptr =
          std::static_pointer_cast<UimRilRequestMessage>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != NULL)
      {
        auto resp_data_ptr = std::make_shared<uim_ril_resp_payload>(result, rsp_ptr, rsp_len);

        QCRIL_LOG_INFO( "Send response for Token :%d", qcril_log_get_uim_token_id(token));
        req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
      }
    }
    else if (req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_OEM_HOOK_EVENT)
    {
      std::shared_ptr<UimOemHookRequestMsg>  req_msg_ptr =
          std::static_pointer_cast<UimOemHookRequestMsg>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != NULL)
      {
        qcril_request_params_type    * params_ptr   = req_msg_ptr->get_params_ptr();

        if (params_ptr != nullptr)
        {
          auto resp_data_ptr = std::shared_ptr<UimOemHookRspData>(new UimOemHookRspData{instance_id,
                                                                                   req_data_ptr->ril_token,
                                                                                   result,
                                                                                   params_ptr->event_id,
                                                                                   rsp_ptr,
                                                                                   rsp_len,
                                                                                   logstr});
          params_ptr->t = req_data_ptr->ril_token;
          req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
        }
      }
    }
    else if (req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_GET_ATR)
    {
      std::shared_ptr<UimGetAtrRequestMsg>  req_msg_ptr =
          std::static_pointer_cast<UimGetAtrRequestMsg>(req_data_ptr->msg_ptr);

      if (req_msg_ptr != NULL)
      {
          auto resp_data_ptr = std::shared_ptr<UimAtrRspParam>(new UimAtrRspParam{result, (char *)rsp_ptr, rsp_len});
        req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);
      }
    }
    else
    {
      QCRIL_LOG_INFO( "Invalid Request :%d", req_data_ptr->req_type);
    }

    delete req_data_ptr;
  }
} /* qcril_uim_response */


/*=========================================================================

  FUNCTION:  qcril_uim_reset_state

===========================================================================*/
/*!
    @brief
    Reset state of QCRIL_UIM at power up and whenever modem resets.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_reset_state
(
  void
)
{
  /* Clean up refresh info */
  qcril_uim_init_refresh_info();

  /* Clean up Long APDU & select response info, if any */
  qcril_uim_cleanup_long_apdu_info();
  qcril_uim_cleanup_select_response_info();

  /* Initialize global variables */
  memset(&qcril_uim, 0, sizeof(qcril_uim_struct_type));
  qcril_uim.prov_session_info.session_state_gw_indexes[0] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;
  qcril_uim.prov_session_info.session_state_gw_indexes[1] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;
  qcril_uim.prov_session_info.session_state_gw_indexes[2] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;
  qcril_uim.prov_session_info.session_state_1x_indexes[0] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;
  qcril_uim.prov_session_info.session_state_1x_indexes[1] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;
  qcril_uim.prov_session_info.session_state_1x_indexes[2] =
                                QCRIL_UIM_PROV_SESSION_NOT_ACTIVATED;

  qcril_uim_init_card_status(&qcril_uim.card_status);

  qcril_uim.pin1_info[0].slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  qcril_uim.pin1_info[1].slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
} /* qcril_uim_reset_state */


/*=========================================================================

  FUNCTION:  qcril_uim_init_state

===========================================================================*/
/*!
    @brief
    Initializes QMI_UIM service and state of QCRIL_UIM. Called during
    initial power up and whenever modem restarts.

    @return
    None.
*/
/*=========================================================================*/
int qcril_uim_init_state
(
  bool *uim_ready_state
)
{
  int                            qmi_err_code    = 0;
  qmi_uim_event_reg_params_type  event_reg_params;
  qmi_uim_rsp_data_type          rsp_data;
  char                           vcc_prop_value[PROPERTY_VALUE_MAX];
  char                           temp_unlock_status[PROPERTY_VALUE_MAX];
  char                           temp_buff[100] = {0};
  uint8_t                        ret            = 0;

  if (uim_ready_state == nullptr)
  {
    return -1;
  }

  /* start UIM service */
  ret = qcril_uim_register_service(qmi_ril_get_process_instance_id());
  QCRIL_LOG_INFO( "UIM service res status: %d\n", ret);

  ret = qcril_uim_remote_server_register_service((int)qmi_ril_get_process_instance_id());
  QCRIL_LOG_INFO( "UIM remote server service res status: %d\n", ret);

  memset(vcc_prop_value, 0x00, sizeof(vcc_prop_value));
  property_get(QCRIL_UIM_PROPERTY_FEATURE_VCC, vcc_prop_value, "");

  /* Get the saved temparory unlock status of the slot */
  (void)snprintf(temp_buff, 99, "%s_%d", QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS, qmi_ril_get_sim_slot());
  memset(temp_unlock_status, 0x00, sizeof(temp_unlock_status));
  property_get(temp_buff, temp_unlock_status, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_FALSE);


  if (strcmp(temp_unlock_status, QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_TRUE)  == 0)
  {
    qcril_uim.temp_unlock_status = QMI_UIM_TRUE;
  }

  /* Register for both legacy and extended card status events to accomodate
     modems supporting either event. */
  memset(&event_reg_params, 0, sizeof(qmi_uim_event_reg_params_type));
  event_reg_params.card_status                = QMI_UIM_TRUE;
  event_reg_params.extended_card_status       = QMI_UIM_TRUE;
  event_reg_params.slot_status                = QMI_UIM_TRUE;
  event_reg_params.simlock_temp_unlock_status = QMI_UIM_TRUE;

  if (strcmp(vcc_prop_value, QCRIL_UIM_PROP_FEATURE_VCC_ENABLED_VALUE)  == 0)
  {
    event_reg_params.supply_voltage_status = QMI_UIM_TRUE;
  }

  event_reg_params.sap_connection = QMI_UIM_TRUE;

  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "event register" );
  qmi_err_code = qcril_uim_send_qmi_sync_msg(QCRIL_UIM_REQUEST_EVENT_REG,
                                             &event_reg_params,
                                             &rsp_data);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_INFO( "event_reg_params qmi_err_code :%d", qmi_err_code);
    return -1;
  }

  /* Get card status just in case modem is done initializing already */
  QCRIL_LOG_QMI( modem_id, "qmi_uim_service", "get card status" );
  memset(&rsp_data, 0x00, sizeof(rsp_data));
  qmi_err_code = qcril_uim_send_qmi_sync_msg(QCRIL_UIM_REQUEST_GET_CARD_STATUS,
                                             nullptr,
                                             &rsp_data);
  if (qmi_err_code != 0)
  {
    QCRIL_LOG_INFO("QCRIL_UIM_REQUEST_GET_CARD_STATUS qmi_err_code :%d", qmi_err_code);
    return -1;
  }

  /* Mandatory to set uim ready state before reading the sim state from modem and
     populate the SIM state */
  *uim_ready_state = true;

  qmi_uim_card_init_callback(&rsp_data, nullptr);

  /* Get modem temparory unlock status and send expiry indication if expired */
  qcril_uim_check_and_send_temp_unlock_expiry_ind();
  return 0;
} /* qcril_uim_init_state */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  qcril_uim_release

===========================================================================*/
void qcril_uim_release
(
  void
)
{
  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Cleanup refresh data */
  qcril_uim_cleanup_refresh_info();

  /* Clean up Long APDU info & select response info, if any */
  qcril_uim_cleanup_long_apdu_info();
  qcril_uim_cleanup_select_response_info();

  /* Reset qcril_uim global structure*/
  qcril_uim_reset_state();
} /* qcril_uim_release */


/*=========================================================================

  FUNCTION:  qcril_uim_process_qmi_callback

===========================================================================*/
void qcril_uim_process_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_callback_params_type  * callback_params_ptr = NULL;
  qcril_uim_original_request_type * orig_ptr            = NULL;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity checks */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return;
  }

  callback_params_ptr = (qcril_uim_callback_params_type*)params_ptr->data;
  if ((callback_params_ptr == NULL) || (callback_params_ptr->orig_req_data == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, NULL callback_params_ptr");
    return;
  }

  orig_ptr = (qcril_uim_original_request_type *)callback_params_ptr->orig_req_data;

  if ((orig_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (orig_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, modem_id: 0x%x",
                    orig_ptr->instance_id,
                    orig_ptr->modem_id);
    qcril_free(orig_ptr);
    orig_ptr = NULL;
    callback_params_ptr->orig_req_data = NULL;
    return;
  }

  QCRIL_LOG_DEBUG( "qcril_uim_process_qmi_callback: Response for resp: %d \n",
                    callback_params_ptr->qmi_rsp_data.rsp_id);

  switch(callback_params_ptr->qmi_rsp_data.rsp_id)
  {
    case QMI_UIM_SRVC_READ_TRANSPARENT_RSP_MSG:
      if (orig_ptr->request_id == RIL_REQUEST_GET_IMSI)
      {
        qcril_uim_get_imsi_resp(callback_params_ptr);
      }
      else if (orig_ptr->request_id == UIM_INTERNAL_REQUEST_GET_MCC_MNC)
      {
        qcril_uim_get_mcc_mnc_resp(callback_params_ptr);
      }
      else
      {
        qcril_uim_read_binary_resp(callback_params_ptr);
      }
      break;

    case QMI_UIM_SRVC_READ_RECORD_RSP_MSG:
      if (orig_ptr->request_id == UIM_INTERNAL_REQUEST_REFRESH_REGISTER)
      {
        qcril_uim_refresh_read_pbr_record_resp(callback_params_ptr);
      }
      else
      {
        qcril_uim_read_record_resp(callback_params_ptr);
      }
      break;

    case QMI_UIM_SRVC_WRITE_TRANSPARENT_RSP_MSG:
      qcril_uim_update_binary_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_WRITE_RECORD_RSP_MSG:
      qcril_uim_update_record_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_FILE_ATTRIBUTES_RSP_MSG:
      if (orig_ptr->request_id == UIM_INTERNAL_REQUEST_REFRESH_REGISTER)
      {
        qcril_uim_refresh_get_pbr_attr_resp(callback_params_ptr);
      }
      else
      {
        qcril_uim_get_response_resp(callback_params_ptr);
      }
      break;

    case QMI_UIM_SRVC_REFRESH_REGISTER_RSP_MSG:
      qcril_uim_refresh_register_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG:
      qcril_uim_pin_resp(callback_params_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG:
      if (orig_ptr->request_id == UIM_INTERNAL_REQUEST_VERIFY_PIN2)
      {
        qcril_uim_process_internal_verify_pin_command_callback(callback_params_ptr);
      }
      else
      {
        qcril_uim_pin_resp(callback_params_ptr, ret_ptr);
      }
      break;

    case QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG:
      qcril_uim_pin_resp(callback_params_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG:
      qcril_uim_pin_resp(callback_params_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_DEPERSONALIZATION_RSP_MSG:
      qcril_uim_deperso_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_SERVICE_STATUS_RSP_MSG:
      qcril_uim_get_fdn_status_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SET_SERVICE_STATUS_RSP_MSG:
      qcril_uim_set_fdn_status_resp(callback_params_ptr);
      break;

     case QMI_UIM_SRVC_AUTHENTICATE_RSP_MSG:
      qcril_uim_sim_authenticate_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_LOGICAL_CHANNEL_RSP_MSG:
    case QMI_UIM_SRVC_OPEN_LOGICAL_CHANNEL_RSP_MSG:
      qcril_uim_logical_channel_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SEND_APDU_RSP_MSG:
      qcril_uim_send_apdu_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_ATR_RSP_MSG:
      qcril_uim_get_atr_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SEND_STATUS_RSP_MSG:
      qcril_uim_send_status_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_RESELECT_RSP_MSG:
      qcril_uim_reselect_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SUPPLY_VOLTAGE_RSP_MSG:
      qcril_uim_voltage_supply_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_PERSONALIZATION_RSP_MSG:
      qcril_uim_perso_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_POWER_DOWN_RSP_MSG:
    case QMI_UIM_SRVC_POWER_UP_RSP_MSG:
      qcril_uim_power_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_PHYSICAL_SLOT_MAPPING_RSP_MSG:
      qcril_uim_get_physical_slot_mapping_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SWITCH_SLOT_EXT_RSP_MSG:
      qcril_uim_switch_slot_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SET_APDU_BEHAVIOR_RSP_MSG:
      qcril_uim_set_apdu_behavior_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_SLOTS_STATUS_RSP_MSG:
      qcril_uim_get_slots_status_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GET_CARD_STATUS_RSP_MSG:
    case QMI_UIM_SRVC_EVENT_REG_RSP_MSG:
      break;

    case QMI_UIM_SRVC_CHANGE_PROV_SESSION_RSP_MSG:
      qcril_uim_change_prov_session_callback(&callback_params_ptr->qmi_rsp_data,
                                             orig_ptr);
      break;

    case QMI_UIM_SRVC_SAP_CONNECTION_RSP_MSG:
      qcril_uim_sap_qmi_handle_sap_connection_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_SAP_REQUEST_RSP_MSG:
      qcril_uim_sap_qmi_handle_sap_request_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GBA_RSP_MSG:
      qcril_uim_oem_gba_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_GBA_IMPI_RSP_MSG:
      qcril_uim_oem_get_impi_resp(callback_params_ptr);
      break;

    case QMI_UIM_SRVC_TERMINAL_CAPABILITY_RSP_MSG:
      if(orig_ptr->request_id == UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ)
      {
        qcril_uim_get_terminal_capability_resp(callback_params_ptr);
      }
      else if(orig_ptr->request_id == UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ)
      {
        qcril_uim_set_terminal_capability_resp(callback_params_ptr);
      }
      else
      {
        QCRIL_LOG_ERROR("Invalid request id 0x%x corresponding to terminal capability response",
                        orig_ptr->request_id);
      }
      break;

    case QMI_UIM_SRVC_REMOTE_UNLOCK_RSP_MSG:
      qcril_uim_remote_unlock_resp(callback_params_ptr);
      break;

    default:
      break;
  }
} /* qcril_uim_process_qmi_callback */


/*=========================================================================

  FUNCTION:  qcril_uim_process_qmi_indication

===========================================================================*/
void qcril_uim_process_qmi_indication
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_indication_params_type * ind_param_ptr = NULL;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  /* Sanity checks */
  if ((params_ptr == NULL) || (ret_ptr == NULL))
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    QCRIL_ASSERT(0);
    return;
  }

  if ((params_ptr->instance_id >= QCRIL_MAX_INSTANCE_ID) ||
      (params_ptr->modem_id    >= QCRIL_MAX_MODEM_ID))
  {
    QCRIL_LOG_ERROR("Invalid values, instance_id: 0x%x, modem_id: 0x%x",
                     params_ptr->instance_id, params_ptr->modem_id);
    QCRIL_ASSERT(0);
    return;
  }

  ind_param_ptr = (qcril_uim_indication_params_type*) params_ptr->data;
  if (ind_param_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, NULL ind_param_ptr");
    QCRIL_ASSERT(0);
    return;
  }

  ind_param_ptr->instance_id = params_ptr->instance_id;
  ind_param_ptr->modem_id = params_ptr->modem_id;

  switch(ind_param_ptr->ind_id)
  {
    case QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG:
      qcril_uim_process_status_change_ind(ind_param_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_REFRESH_IND_MSG:
      qcril_uim_process_refresh_ind(ind_param_ptr, ret_ptr);
      break;

#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC) || defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
    case QMI_UIM_SRVC_SEND_APDU_IND_MSG:
      qcril_uim_process_send_apdu_ind(ind_param_ptr, ret_ptr);
      break;
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC || RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */

    case QMI_UIM_SRVC_RECOVERY_IND_MSG:
      qcril_uim_process_recovery_ind(ind_param_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_SUPPLY_VOLTAGE_IND_MSG:
      qcril_uim_process_supply_voltage_ind(ind_param_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_SIMLOCK_TEMP_UNLOCK_IND_MSG:
      qcril_uim_process_simlock_temp_unlock_ind(ind_param_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_SLOT_STATUS_CHANGE_IND_MSG:
      qcril_uim_process_slot_status_change_ind(ind_param_ptr, ret_ptr);
      break;

    case QMI_UIM_SRVC_SAP_IND_MSG:
      qcril_uim_sap_ind(&ind_param_ptr->ind_data.sap_ind);
      break;

    case QMI_UIM_SRVC_LPA_APP_PROFILE_IND_MSG:
      qcril_uim_lpa_add_profile_ind(ind_param_ptr, ret_ptr);
      break;

    default:
      QCRIL_LOG_ERROR("Unsupported indication! 0x%x\n", ind_param_ptr->ind_id);
      break;
  }
} /* qcril_uim_process_qmi_indication */


/*=========================================================================

  FUNCTION:  qcril_uim_send_qmi_sync_msg

===========================================================================*/
/*!
    @brief
    QMI UIM send qmi sync message.

    @return
    None
*/
/*=========================================================================*/
int qcril_uim_send_qmi_sync_msg
(
  uint32                        msg_id,
  const void                  * params,
  qmi_uim_rsp_data_type       * rsp_data_ptr
)
{
  UimModule *uModule = getUimModule();

  if (uModule == nullptr || rsp_data_ptr == nullptr)
  {
    return -1;
  }

  return uModule->UimSendQmiUimRequest(msg_id,
                                       params,
                                       rsp_data_ptr,
                                       nullptr,
                                       nullptr);
} /* qcril_uim_send_qmi_sync_msg */


/*=========================================================================

  FUNCTION:  qcril_uim_send_qmi_async_msg

===========================================================================*/
/*!
    @brief
    QMI UIM send qmi async message.

    @return
    None
*/
/*=========================================================================*/

int qcril_uim_send_qmi_async_msg
(
  qcril_uim_request_type                     request_type,
  void                                     * param_data_ptr,
  void                                     * original_request_ptr
)
{
  UimModule *uModule = getUimModule();

  if (uModule == nullptr || original_request_ptr == nullptr)
  {
    return -1;
  }

  return uModule->UimSendQmiUimRequest(request_type,
                                       param_data_ptr,
                                       nullptr,
                                       original_request_ptr,
                                       qmi_uim_async_cb);
} /* qcril_uim_send_qmi_async_msg */


/*=========================================================================

  FUNCTION:  qcril_log_get_uim_token_id

===========================================================================*/
/*!
    @brief
    Prints token id.

    @return
    int
*/
/*=========================================================================*/
int qcril_log_get_uim_token_id
(
  RIL_Token t
)
{
  UimRequestMsgData  *req_data_ptr = (UimRequestMsgData *)t;

  if (req_data_ptr == nullptr)
  {
    return 0;
  }
  if (req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_OEM_HOOK_EVENT)
  {
    if (req_data_ptr->ril_token != nullptr)
    {
      return (*(int *) req_data_ptr->ril_token);
    }
  }
  else if (req_data_ptr->req_type == UimRequestMsgData::UIM_REQUEST_RIL_EVENT)
  {
    qcril_event_context  * ctx   = (qcril_event_context  *)req_data_ptr->ril_token;
    if (ctx != nullptr)
    {
      return (*(int *) ctx->t);
    }
  }

  return 0;
} /* qcril_log_get_uim_token_id */


/*===========================================================================

  FUNCTION:  qmi_uim_async_cb

===========================================================================*/
/*!
    @brief
    QMI callback function

    @return
    None
*/
/*=========================================================================*/
void qmi_uim_async_cb
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
)
{
  std::shared_ptr<UimQmiUimResponseMsg>  resp_msg_ptr =
   std::make_shared<UimQmiUimResponseMsg>(rsp_data,
                                          user_data);
  UimModule *uModule = getUimModule();

  if (resp_msg_ptr != nullptr && uModule != nullptr)
  {
    uModule->dispatch(resp_msg_ptr);
  }
} /* qmi_uim_async_cb */


#endif /* defined (FEATURE_QCRIL_UIM_QMI) */

#ifdef QMI_RIL_UTF
extern "C"
{
  void qcril_qmi_hal_uim_module_cleanup()
  {
    UimModule *uModule = getUimModule();
    GstkModule *gModule = getGstkModule();
    LpaModule *lModule = getLpaModule();

    uModule->qcrilHalUimModuleCleanup();
    gModule->qcrilHalGstkModuleCleanup();
    lModule->qcrilHalLpaModuleCleanup();
    qcril_uim_reset_state();
  }
}
#endif
