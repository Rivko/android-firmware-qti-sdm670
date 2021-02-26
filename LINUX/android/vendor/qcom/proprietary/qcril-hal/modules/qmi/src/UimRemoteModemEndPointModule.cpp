/*===========================================================================

  Copyright (c)2008-2010, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "framework/Log.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceDownIndMessage.h"
#include "modules/qmi/QmiUimRemoteSetupRequest.h"
#include "modules/uim_remote/UimRmtQmiUimRemoteReqMsg.h"
#include "modules/uim_remote/UimRmtQmiUimRemoteIndMsg.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"
#include "modules/uim/UimOemHook.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client.h"
#include "user_identity_module_remote_v01.h"
#include "UimRemoteModemEndPointModule.h"

#define TAG "UimRemoteModemEndPointModule"

static RIL_HookRemoteSimInfo  remote_sim_info = {0, RIL_UICC_REMOTE_TRANSPORT_OTHER, RIL_UICC_REMOTE_USAGE_NORMAL, RIL_UICC_REMOTE_CONNECTION_UNAVAILABLE};

/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

typedef struct
{
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg>   reqPtr;
  UimRemoteModemEndPointModule              * me;
} UimRemoteUserData;

#define QCRIL_QMI_UIM_REMOTE_FREE_PTR(ptr)                                  \
  if (ptr != NULL)                                                          \
  {                                                                         \
    delete ptr;                                                             \
    ptr = NULL;                                                             \
  }                                                                         \

/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  qcril_uim_remote_convert_slot_type_to_instance_id
===========================================================================*/
static qcril_instance_id_e_type qcril_uim_remote_convert_slot_type_to_instance_id
(
  uim_remote_slot_type_enum_v01      slot_type
)
{
  qcril_instance_id_e_type instance = QCRIL_MAX_INSTANCE_ID;

  /* Convert instance id to slot index */
  switch (slot_type)
  {
    case UIM_REMOTE_SLOT_1_V01:
      instance = QCRIL_DEFAULT_INSTANCE_ID;
      break;
    case UIM_REMOTE_SLOT_2_V01:
      instance = QCRIL_SECOND_INSTANCE_ID;
      break;
    case UIM_REMOTE_SLOT_3_V01:
      instance = QCRIL_THIRD_INSTANCE_ID;
      break;
    default:
      QCRIL_LOG_ERROR("Invalid slot type for conversion: 0x%x \n", slot_type);
      break;
  }

  QCRIL_LOG_DEBUG( "instance found: 0x%x\n", instance );
  return instance;
} /* qcril_uim_remote_convert_slot_type_to_instance_id */


/*===========================================================================
  FUNCTION  qmi_uim_notify_cb - QMI Notify callback function
===========================================================================*/
static void qmi_uim_notify_cb
(
  qmi_client_type user_handle,
  qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event,
  void *notify_cb_data
)
{
  /* Wrapper for member method*/
  UimRemoteModemEndPointModule *me = (UimRemoteModemEndPointModule *)notify_cb_data;

  if (me != nullptr)
  {
    me->onQmiNotifyCb(user_handle, service_obj, service_event);
  }
} /* qmi_uim_notify_cb */


/*=========================================================================
  FUNCTION:  qcril_uim_remote_cmd_callback
===========================================================================*/
static void qcril_uim_remote_cmd_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_rmt_rsp_ptr,
  unsigned int                   qmi_rmt_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  UimRemoteUserData                        *cb_data_ptr  = (UimRemoteUserData *)resp_cb_data_ptr;
  std::shared_ptr<qmi_uim_remote_resp_msg>  resp_msg_ptr =
    std::shared_ptr<qmi_uim_remote_resp_msg>(
      new qmi_uim_remote_resp_msg{QMI_UIM_REMOTE_RESULT_FAILURE, transp_err, (uint32_t)QMI_ERR_INTERNAL_V01});

  QCRIL_NOTUSED(user_handle);
  QCRIL_NOTUSED(qmi_rmt_rsp_len);

  QCRIL_LOG_INFO( "qcril_uim_remote_cmd_callback: msg_id = 0x%x ", msg_id);

  /* Sanity check */
  if (cb_data_ptr != nullptr )
  {
    /* Process only the supported RESP messages */
    switch (msg_id)
    {
      case QMI_UIM_REMOTE_EVENT_RESP_V01:
        if (qmi_rmt_rsp_ptr != nullptr)
        {
          uim_remote_event_resp_msg_v01 *evt_rsp_ptr =
            (uim_remote_event_resp_msg_v01 *)qmi_rmt_rsp_ptr;

          resp_msg_ptr->result = (qmi_uim_remote_result)evt_rsp_ptr->resp.result;
          resp_msg_ptr->err_code = evt_rsp_ptr->resp.error;
        }
        break;
      case QMI_UIM_REMOTE_APDU_RESP_V01:
        if (qmi_rmt_rsp_ptr != nullptr)
        {
          uim_remote_apdu_resp_msg_v01 *apdu_rsp_ptr =
            (uim_remote_apdu_resp_msg_v01 *)qmi_rmt_rsp_ptr;

          resp_msg_ptr->result = (qmi_uim_remote_result)apdu_rsp_ptr->resp.result;
          resp_msg_ptr->err_code = apdu_rsp_ptr->resp.error;
        }
        break;
      default:
        break;
    }

    if (cb_data_ptr->me != nullptr)
    {
      cb_data_ptr->me->onQmiAsyncCb(resp_msg_ptr, cb_data_ptr->reqPtr);
    }
  }

  /* Free allocated pointers only if event queueing fails */
  QCRIL_QMI_UIM_REMOTE_FREE_PTR(cb_data_ptr);
  if (qmi_rmt_rsp_ptr != nullptr)
  {
    delete (uint8_t *)qmi_rmt_rsp_ptr;
    qmi_rmt_rsp_ptr = nullptr;
  }
} /* qcril_uim_remote_cmd_callback */


/*===========================================================================
  FUNCTION:  qcril_uim_remote_client_request_event
===========================================================================*/
int UimRemoteModemEndPointModule::qcril_uim_remote_client_request_event
(
  qmi_uim_remote_event_req_msg  * event_req_ptr,
  void                          * orig_req_ptr
)
{
  qmi_txn_handle                                    txn_handle;
  int                                               qmi_err_code    = QMI_INTERNAL_ERR;
  uim_remote_event_req_msg_v01                    * qmi_rmt_req_ptr = NULL;
  uim_remote_event_resp_msg_v01                   * qmi_rmt_rsp_ptr = NULL;

  if (event_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_request_event, event req is null");
    return qmi_err_code;
  }

  QCRIL_LOG_INFO("qcril_uim_remote_client_request_event, event: 0x%x, slot_id: 0x%x)\n",
                  event_req_ptr->event, event_req_ptr->slot);

  /* Allocate & fill up QMI Remote request */
  qmi_rmt_req_ptr = new uim_remote_event_req_msg_v01{};
  if (qmi_rmt_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_req_ptr");
    return QMI_SERVICE_ERR;
  }

  qmi_rmt_req_ptr->event_info.slot = (uim_remote_slot_type_enum_v01)event_req_ptr->slot;
  qmi_rmt_req_ptr->event_info.event =(uim_remote_event_type_enum_v01)event_req_ptr->event;

  if (event_req_ptr->event == QMI_UIM_REMOTE_CONNECTION_UNAVAILABLE)
  {
    memset(&remote_sim_info, 0 , sizeof(remote_sim_info));
  }

  /* Add ATR if needed */
  if ((event_req_ptr->event == QMI_UIM_REMOTE_CARD_INSERTED)||
      (event_req_ptr->event == QMI_UIM_REMOTE_CARD_RESET))
  {
    if ((event_req_ptr->atr_len != 0) &&
        (event_req_ptr->atr_len <= QMI_UIM_REMOTE_MAX_ATR_LEN) &&
        (event_req_ptr->atr_len <= QMI_UIM_REMOTE_MAX_ATR_LEN_V01))
    {
      qmi_rmt_req_ptr->atr_valid = 0x01;
      qmi_rmt_req_ptr->atr_len = event_req_ptr->atr_len;
      memcpy(qmi_rmt_req_ptr->atr, event_req_ptr->atr, event_req_ptr->atr_len);
    }
  }

  /* Add wakeup support TLV if needed */
  if (event_req_ptr->wakeup_support_valid)
  {
    qmi_rmt_req_ptr->wakeup_support_valid = 0x01;
    qmi_rmt_req_ptr->wakeup_support = event_req_ptr->wakeup_support ? 0x01 : 0x00;
  }

  /* Add Error code TLV if needed */
  if ((event_req_ptr->event == QMI_UIM_REMOTE_CARD_ERROR) &&
      (event_req_ptr->error_cause_valid))
  {
    qmi_rmt_req_ptr->error_cause_valid = 0x01;
    qmi_rmt_req_ptr->error_cause = (uim_remote_card_error_type_enum_v01)event_req_ptr->error_cause;
  }

  /* Add transport type if valid */
  if (event_req_ptr->transport_valid)
  {
    QCRIL_LOG_DEBUG( "Remote SIM transport=%d",event_req_ptr->transport);
    qmi_rmt_req_ptr->transport_valid = 0x01;
    qmi_rmt_req_ptr->transport = (uim_remote_transport_enum_v01)event_req_ptr->transport;
  }

  /* Add usage if valid */
  if (event_req_ptr->usage_valid)
  {
    QCRIL_LOG_DEBUG( "Remote SIM usage=%d",event_req_ptr->usage_valid);
    qmi_rmt_req_ptr->usage_valid = 0x01;
    qmi_rmt_req_ptr->usage = (uim_remote_usage_enum_v01)event_req_ptr->usage;
  }

  /* Add Apdu timeout if valid */
  if (event_req_ptr->apdu_timeout_valid)
  {
    QCRIL_LOG_DEBUG( "Remote SIM apdu_timeout=%d",event_req_ptr->apdu_timeout);
    qmi_rmt_req_ptr->apdu_timeout_valid = 0x01;
    qmi_rmt_req_ptr->apdu_timeout       = event_req_ptr->apdu_timeout;
  }

  /* Add Disable all polling if valid */
  if (event_req_ptr->disable_all_polling_valid)
  {
    QCRIL_LOG_DEBUG( "Remote SIM disable_all_polling=%d",event_req_ptr->disable_all_polling);
    qmi_rmt_req_ptr->disable_all_polling_valid = 0x01;
    qmi_rmt_req_ptr->disable_all_polling       = event_req_ptr->disable_all_polling;
  }

  /* Allocate response pointer since it is an async command */
  qmi_rmt_rsp_ptr = new uim_remote_event_resp_msg_v01{};
  if (qmi_rmt_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_rsp_ptr");
    QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_req_ptr);
    return QMI_SERVICE_ERR;
  }

  qmi_err_code = qmi_client_send_msg_async(
                   mQmiSvcClient,
                   QMI_UIM_REMOTE_EVENT_REQ_V01,
                   (void *) qmi_rmt_req_ptr,
                   sizeof(uim_remote_event_req_msg_v01),
                   (void *) qmi_rmt_rsp_ptr,
                   sizeof(uim_remote_event_resp_msg_v01),
                   qcril_uim_remote_cmd_callback,
                   orig_req_ptr,
                   &txn_handle);
  /* On successful API call, free only the request */
  if (qmi_err_code != 0)
  {
    QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_rsp_ptr);
  }

  /* In any case of error, check & free all the allocated pointers */
  QCRIL_LOG_ERROR("qcril_uim_remote_client_request_event, qmi_err_code: 0x%x", qmi_err_code);
  QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_req_ptr);
  return qmi_err_code;
} /* qcril_uim_remote_client_request_event */


/*===========================================================================
  FUNCTION:  qcril_uim_remote_client_request_apdu
===========================================================================*/
int UimRemoteModemEndPointModule::qcril_uim_remote_client_request_apdu
(
  qmi_uim_remote_apdu_req_msg  * apdu_req_ptr,
  void                         * orig_req_ptr
)
{
  qmi_txn_handle                       txn_handle;
  int                                  qmi_err_code    = QMI_INTERNAL_ERR;
  uim_remote_apdu_req_msg_v01        * qmi_rmt_req_ptr = NULL;
  uim_remote_apdu_resp_msg_v01       * qmi_rmt_rsp_ptr = NULL;

  if (apdu_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("qcril_uim_remote_client_request_apdu, apdu req is null");
    return qmi_err_code;
  }

  /* Allocate & fill up QMI Remote request */
  qmi_rmt_req_ptr = new uim_remote_apdu_req_msg_v01{};
  if (qmi_rmt_req_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_req_ptr");
    return QMI_SERVICE_ERR;
  }

  QCRIL_LOG_INFO("qcril_uim_remote_client_request_apdu, slot_id: 0x%x)\n", apdu_req_ptr->slot);

  memset(qmi_rmt_req_ptr, 0, sizeof(uim_remote_apdu_req_msg_v01));
  qmi_rmt_req_ptr->slot = (uim_remote_slot_type_enum_v01)apdu_req_ptr->slot;
  qmi_rmt_req_ptr->apdu_status = (qmi_result_type_v01)apdu_req_ptr->apdu_status;

  qmi_rmt_req_ptr->apdu_id = apdu_req_ptr->apdu_id;
  if (apdu_req_ptr->response_apdu_info_valid)
  {
    qmi_rmt_req_ptr->response_apdu_info_valid = 0x01;
    qmi_rmt_req_ptr->response_apdu_info.total_response_apdu_size =
      apdu_req_ptr->response_apdu_info.total_response_apdu_size;
    qmi_rmt_req_ptr->response_apdu_info.response_apdu_segment_offset =
      apdu_req_ptr->response_apdu_info.response_apdu_segment_offset;
  }

  if(apdu_req_ptr->response_apdu_segment_len != 0 &&
     apdu_req_ptr->response_apdu_segment_len <= QMI_UIM_REMOTE_MAX_RESPONSE_APDU_SEGMENT_LEN &&
     apdu_req_ptr->response_apdu_segment_len <= QMI_UIM_REMOTE_MAX_RESPONSE_APDU_SEGMENT_LEN_V01)
  {
    qmi_rmt_req_ptr->response_apdu_segment_valid = 0x01;
    qmi_rmt_req_ptr->response_apdu_segment_len = apdu_req_ptr->response_apdu_segment_len;
    memcpy(qmi_rmt_req_ptr->response_apdu_segment,
           apdu_req_ptr->response_apdu_segment,
           apdu_req_ptr->response_apdu_segment_len);
  }

  /* Allocate response pointer since it is an async command */
  qmi_rmt_rsp_ptr = new uim_remote_apdu_resp_msg_v01{};
  if (qmi_rmt_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "error allocating memory for qmi_rmt_rsp_ptr");
    QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_req_ptr);
    return QMI_SERVICE_ERR;
  }

  /* Proceed with Event request */
  qmi_err_code = qmi_client_send_msg_async(
                   mQmiSvcClient,
                   QMI_UIM_REMOTE_APDU_REQ_V01,
                   (void *) qmi_rmt_req_ptr,
                   sizeof(uim_remote_apdu_req_msg_v01),
                   (void *) qmi_rmt_rsp_ptr,
                   sizeof(uim_remote_apdu_resp_msg_v01),
                   qcril_uim_remote_cmd_callback,
                   orig_req_ptr,
                   &txn_handle);
  /* On successful API call, free only the request */
  if (qmi_err_code != 0)
  {
    QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_rsp_ptr);
  }

  /* In any case of error, check & free all the allocated pointers */
  QCRIL_LOG_ERROR("qcril_uim_remote_client_request_apdu, qmi_err_code: 0x%x", qmi_err_code);
  QCRIL_QMI_UIM_REMOTE_FREE_PTR(qmi_rmt_req_ptr);
  return qmi_err_code;
} /* qcril_uim_remote_client_request_apdu */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_apdu_ind

===========================================================================*/
/*!
    @brief
    Handles the APDU indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_apdu_ind
(
  uim_remote_apdu_ind_msg_v01              * ind_msg_ptr,
  qmi_uim_remote_apdu_ind_msg              * apdu_ptr
)
{
  qcril_instance_id_e_type  instance_id = qmi_ril_get_process_instance_id();

  if (ind_msg_ptr == NULL || apdu_ptr == FALSE)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_msg_ptr");
    return FALSE;
  }

  if (ind_msg_ptr->command_apdu_len == 0 ||
      ind_msg_ptr->command_apdu_len > QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN)
  {
    QCRIL_LOG_ERROR("%s", "Zero command_apdu_len");
    return FALSE;
  }

  QCRIL_LOG_INFO( "%s slot: 0x%x, apdu_id:0x%x",
                  __FUNCTION__,
                  ind_msg_ptr->slot,
                  ind_msg_ptr->apdu_id);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  apdu_ptr->apdu_id = ind_msg_ptr->apdu_id;
  apdu_ptr->command_apdu_len =
    (ind_msg_ptr->command_apdu_len > QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN_V01) ? QMI_UIM_REMOTE_MAX_COMMAND_APDU_LEN_V01 : ind_msg_ptr->command_apdu_len;

  memcpy(apdu_ptr->command_apdu, ind_msg_ptr->command_apdu, apdu_ptr->command_apdu_len);
  return TRUE;
} /* qcril_uim_remote_handle_apdu_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_connect_ind

===========================================================================*/
/*!
    @brief
    Handles the Connect indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_connect_ind
(
  uim_remote_connect_ind_msg_v01           * ind_msg_ptr
)
{
  qcril_instance_id_e_type  instance_id = qmi_ril_get_process_instance_id();

  QCRIL_LOG_INFO( "%s slot: 0x%x", __FUNCTION__, ind_msg_ptr->slot);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  if(remote_sim_info.isEnable)
  {
    /* Send OEMHook unsolicited response to framework */
    qcril_hook_unsol_response(instance_id,
                              QCRIL_EVT_HOOK_UNSOL_REMOTE_SIM_STATUS_IND,
                              (char *)&remote_sim_info,
                              sizeof(remote_sim_info));
  }

  return TRUE;
} /* qcril_uim_remote_handle_connect_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_disconnect_ind

===========================================================================*/
/*!
    @brief
    Handles the Disconnect indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_disconnect_ind
(
  uim_remote_disconnect_ind_msg_v01        * ind_msg_ptr
)
{
  qcril_instance_id_e_type  instance_id = qmi_ril_get_process_instance_id();

  QCRIL_LOG_INFO( "%s slot: 0x%x", __FUNCTION__, ind_msg_ptr->slot);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  /* Send unsol indication for remote sim status to ATEL */
  if(remote_sim_info.isEnable)
  {
    RIL_HookRemoteSimInfo  remote_sim;
    memset(&remote_sim, 0, sizeof(remote_sim));
    /* Send OEMHook unsolicited response to framework */
    qcril_hook_unsol_response(instance_id,
                              QCRIL_EVT_HOOK_UNSOL_REMOTE_SIM_STATUS_IND,
                              (char *)&remote_sim,
                              sizeof(remote_sim));
  }

  return TRUE;
} /* qcril_uim_remote_handle_disconnect_ind */



/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_pup_ind

===========================================================================*/
/*!
    @brief
    Handles the power up indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_pup_ind
(
  uim_remote_card_power_up_ind_msg_v01     * ind_msg_ptr,
  qmi_uim_remote_card_power_up_ind_msg     * pup_ptr
)
{
  qcril_instance_id_e_type            instance_id   = qmi_ril_get_process_instance_id();

  if (ind_msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_msg_ptr");
    return FALSE;
  }

  QCRIL_LOG_INFO( "%s slot: 0x%x", __FUNCTION__, ind_msg_ptr->slot);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  if (ind_msg_ptr->voltage_class_valid)
  {
    pup_ptr->voltage_class_valid = TRUE;
    switch (ind_msg_ptr->voltage_class)
    {
      case UIM_REMOTE_VOLTAGE_CLASS_C_LOW_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_C_LOW;
        break;

      case UIM_REMOTE_VOLTAGE_CLASS_C_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_C;
        break;

      case UIM_REMOTE_VOLTAGE_CLASS_C_HIGH_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_C_HIGH;
        break;

      case UIM_REMOTE_VOLTAGE_CLASS_B_LOW_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_B_LOW;
        break;

      case UIM_REMOTE_VOLTAGE_CLASS_B_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_B;
        break;

      case UIM_REMOTE_VOLTAGE_CLASS_B_HIGH_V01:
        pup_ptr->voltage_class = QMI_UIM_REMOTE_VOLTAGE_CLASS_B_HIGH;
        break;

      default:
        /* Skip this field for unsupported enums */
        pup_ptr->voltage_class_valid = FALSE;
        break;
    }
  }

  return TRUE;
} /* qcril_uim_remote_handle_pup_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_pdown_ind

===========================================================================*/
/*!
    @brief
    Handles the power down indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_pdown_ind
(
  uim_remote_card_power_down_ind_msg_v01   * ind_msg_ptr,
  qmi_uim_remote_card_power_down_ind_msg   * pdown_ptr
)
{
  qcril_instance_id_e_type          instance_id   = qmi_ril_get_process_instance_id();

  if (ind_msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "NULL ind_msg_ptr");
    return FALSE;
  }

  QCRIL_LOG_INFO( "%s slot: 0x%x", __FUNCTION__, ind_msg_ptr->slot);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  if (ind_msg_ptr->mode_valid)
  {
    pdown_ptr->mode_valid = TRUE;
    switch (ind_msg_ptr->mode)
    {
      case UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE_V01:
        pdown_ptr->mode = QMI_UIM_REMOTE_POWER_DOWN_TELECOM_INTERFACE;
        break;

      case UIM_REMOTE_POWER_DOWN_CARD_V01:
        pdown_ptr->mode = QMI_UIM_REMOTE_POWER_DOWN_CARD;
        break;

      default:
        /* Skip this field for unsupported enums */
        pdown_ptr->mode_valid = FALSE;
        break;
    }
  }

  return TRUE;
} /* qcril_uim_remote_handle_pdown_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_remote_handle_reset_ind

===========================================================================*/
/*!
    @brief
    Handles the power down indication

    @return
    None
*/
/*=========================================================================*/
static boolean qcril_uim_remote_handle_reset_ind
(
  uim_remote_card_reset_ind_msg_v01        * ind_msg_ptr
)
{
  qcril_instance_id_e_type          instance_id   = qmi_ril_get_process_instance_id();

  QCRIL_LOG_INFO( "%s slot: 0x%x", __FUNCTION__, ind_msg_ptr->slot);

  if (instance_id != qcril_uim_remote_convert_slot_type_to_instance_id(ind_msg_ptr->slot))
  {
    QCRIL_LOG_INFO("Indication is not for this instance: %d", instance_id);
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_remote_handle_reset_ind */


/*=========================================================================

  FUNCTION:  qcril_uim_remote_ind_callback

===========================================================================*/
/*!
    @brief
    Callback implementation for the QMI UIM REMOTE indications. This will be
    called by QMI FW when the QMI UIM REMOTE in the modem sends indications.

    @return
    None
*/
/*=========================================================================*/
static void qcril_uim_remote_ind_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  unsigned char                * qmi_rmt_ind_ptr,
  unsigned int                   qmi_rmt_ind_len,
  void                         * ind_cb_data_ptr
)
{
  uint8_t                                 * decoded_payload_ptr  = NULL;
  uint32_t                                  decoded_payload_len  = 0;
  qmi_client_error_type                     qmi_err              = QMI_INTERNAL_ERR;
  std::shared_ptr<qmi_uim_remote_ind_msg>   ind_msg_ptr          =
    std::shared_ptr<qmi_uim_remote_ind_msg>(new qmi_uim_remote_ind_msg{});
  boolean                                   send_ind             = FALSE;

  QCRIL_LOG_INFO("qcril_uim_remote_ind_callback, msg_id: 0x%x", msg_id);

  if ((qmi_rmt_ind_ptr == nullptr) || (qmi_rmt_ind_len == 0) ||
      (ind_cb_data_ptr == nullptr) || (ind_msg_ptr == nullptr))
  {
    QCRIL_LOG_ERROR("%s","NULL ind_buf_ptr or zero qmi_rmt_ind_len");
    return;
  }

  /* First decode the message payload from QCCI */
  qmi_idl_get_message_c_struct_len(uim_remote_get_service_object_v01(),
                                   QMI_IDL_INDICATION,
                                   msg_id,
                                   &decoded_payload_len);
  if (decoded_payload_len == 0)
  {
    QCRIL_LOG_ERROR("%s: Failed to find decoded_payload_len");
    return;
  }

  /* Allocate decoded payload buffer */
  decoded_payload_ptr = new uint8_t[decoded_payload_len]{};
  if (decoded_payload_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Failed to allocate payload ptr, payload len: 0x%x\n",
                    decoded_payload_len);
    return;
  }

  /* Decode the Indication payload */
  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      qmi_rmt_ind_ptr,
                                      qmi_rmt_ind_len,
                                      decoded_payload_ptr,
                                      decoded_payload_len);
  if (qmi_err != QMI_NO_ERR)
  {
    QCRIL_LOG_ERROR("Failed to decode Indication: 0x%x, qmi_err: 0x%x", msg_id, qmi_err);
    if(decoded_payload_ptr != nullptr)
    {
      delete[] decoded_payload_ptr;
      decoded_payload_ptr = nullptr;
    }
    return;
  }

  /* Process only the supported IND messages */
  switch (msg_id)
  {
    case QMI_UIM_REMOTE_APDU_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_APDU_IND;
      send_ind = qcril_uim_remote_handle_apdu_ind((uim_remote_apdu_ind_msg_v01 *)decoded_payload_ptr,
                                                  &ind_msg_ptr->ind_data.apdu_ind);
      break;
    case QMI_UIM_REMOTE_CONNECT_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_CONNECT_IND;
      send_ind = qcril_uim_remote_handle_connect_ind((uim_remote_connect_ind_msg_v01 *)decoded_payload_ptr);
      break;
    case QMI_UIM_REMOTE_DISCONNECT_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_DISCONNECT_IND;
      send_ind = qcril_uim_remote_handle_disconnect_ind((uim_remote_disconnect_ind_msg_v01 *)decoded_payload_ptr);
      break;
    case QMI_UIM_REMOTE_CARD_POWER_UP_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_POWER_UP_IND;
      send_ind = qcril_uim_remote_handle_pup_ind((uim_remote_card_power_up_ind_msg_v01 *)decoded_payload_ptr,
                                                 &ind_msg_ptr->ind_data.pup_ind);
      break;
    case QMI_UIM_REMOTE_CARD_POWER_DOWN_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_POWER_DOWN_IND;
      send_ind = qcril_uim_remote_handle_pdown_ind((uim_remote_card_power_down_ind_msg_v01 *)decoded_payload_ptr,
                                                   &ind_msg_ptr->ind_data.pdown_ind);
      break;
    case QMI_UIM_REMOTE_CARD_RESET_IND_V01:
      ind_msg_ptr->ind_id = QMI_UIM_REMOTE_CARD_RESET_IND;
      send_ind = qcril_uim_remote_handle_reset_ind((uim_remote_card_reset_ind_msg_v01 *)decoded_payload_ptr);
      break;
    default:
      break;
  }

  if (send_ind)
  {
    ((UimRemoteModemEndPointModule *)ind_cb_data_ptr)->onQmiUnsolIndCb(ind_msg_ptr);
  }

  if(decoded_payload_ptr != nullptr)
  {
    delete[] decoded_payload_ptr;
    decoded_payload_ptr = nullptr;
  }
} /* qcril_uim_remote_ind_callback */


/*---------------------------------------------------------------------------
  UimModemEndPointModule class functions
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  UimRemoteModemEndPointModule - Constructor
===========================================================================*/
UimRemoteModemEndPointModule::UimRemoteModemEndPointModule
(
  string name,
  ModemEndPoint *owner
)
{
  mName = name;
  mOwner = owner;
  mServiceObject = nullptr;
  mQmiSvcClient = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;

  mMessageHandler = {
       HANDLER(QmiUimRemoteSetupRequest, UimRemoteModemEndPointModule::handleQmiClientSetup),
       HANDLER(UimRmtQmiUimRemoteReqMsg, UimRemoteModemEndPointModule::handleQcrilUimRmtRequest),
       HANDLER(QmiServiceUpIndMessage, UimRemoteModemEndPointModule::handleQmiServiceUp),
       HANDLER(QmiServiceDownIndMessage, UimRemoteModemEndPointModule::handleQmiServiceDown),
  };
} /* UimRemoteModemEndPointModule::UimRemoteModemEndPointModule */


/*===========================================================================
  FUNCTION  UimRemoteModemEndPointModule - init
===========================================================================*/

void UimRemoteModemEndPointModule::init()
{
  QCRIL_LOG_INFO( "%s init\n", __FUNCTION__);

  /* Call base init before doing any other stuff.*/
  Module::init();
}


/*===========================================================================
  FUNCTION  UimRemoteModemEndPointModule - Destructor
===========================================================================*/
UimRemoteModemEndPointModule::~UimRemoteModemEndPointModule()
{
  QCRIL_LOG_INFO( "%s", __FUNCTION__);

  if (mQmiNotifyHandle != nullptr)
  {
    (void)qmi_client_release(mQmiNotifyHandle);
    mQmiNotifyHandle = nullptr;
  }
  if (mQmiSvcClient != nullptr)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }
  mLooper = nullptr;
} /* UimRemoteModemEndPointModule::~UimRemoteModemEndPointModule */


/*===========================================================================
  FUNCTION  onQmiNotifyCb - UimRemoteModemEndPointModule QMI client notfier
                                    callback handler
===========================================================================*/
void UimRemoteModemEndPointModule::onQmiNotifyCb
(
  qmi_client_type user_handle, qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event
)
{
  (void)user_handle;
  (void)service_obj;
  Log::getInstance().d("[UimRemoteModemEndPointModule]: onQmiNotifyCb()");

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]: qmiNotifyCb() Service up indication");

      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceUpIndMessage(nullptr)));

      break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]: qmiNotifyCb() Service down indication");
      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceDownIndMessage(QMI_SERVICE_ERR)));
      break;
    default:
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]: qmiNotifyCb() unsupported service "
          "event: " +
          std::to_string(service_event));
      break;
  }
  return;
} /* UimRemoteModemEndPointModule::onQmiNotifyCb */


/*===========================================================================
  FUNCTION  onQmiUnsolIndCb - UimRemoteModemEndPointModule QMI UIM unsol
                                    message handler
===========================================================================*/
void UimRemoteModemEndPointModule::onQmiUnsolIndCb
(
  std::shared_ptr<qmi_uim_remote_ind_msg>  ind_data_ptr
)
{
  std::shared_ptr<UimRmtQmiUimRemoteIndMsg> ind_msg_ptr =
       std::make_shared<UimRmtQmiUimRemoteIndMsg>(ind_data_ptr);

  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* UimRemoteModemEndPointModule::onQmiUnsolIndCb */


/*===========================================================================
  FUNCTION  UimRemoteModemEndPointModule::onQmiAsyncCb
===========================================================================*/
void UimRemoteModemEndPointModule::onQmiAsyncCb
(
  std::shared_ptr<qmi_uim_remote_resp_msg>   rsp_data_ptr,
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg>  req_msg_ptr 
)
{
  if (req_msg_ptr != nullptr)
  {
    if(rsp_data_ptr != nullptr &&
       rsp_data_ptr->result == QMI_UIM_REMOTE_RESULT_SUCCESS)
    {
      std::shared_ptr<qmi_uim_remote_request_msg> req_data_ptr = req_msg_ptr->get_message();

      if (req_data_ptr != nullptr &&
          req_data_ptr->req_id == QMI_UIM_REMOTE_REQ_EVENT_REQUEST &&
          req_data_ptr->req_data.event_req.event == QMI_UIM_REMOTE_CONNECTION_AVAILABLE)
      {
        remote_sim_info.isEnable = TRUE;
        remote_sim_info.transportType = (RIL_TransportType)req_data_ptr->req_data.event_req.transport;
        remote_sim_info.usage = (RIL_Usage)req_data_ptr->req_data.event_req.usage;
        remote_sim_info.remoteEvent = (RIL_remoteEvent)req_data_ptr->req_data.event_req.event;
      }
    }
    /* Send response back to UIM module */
    req_msg_ptr->sendResponse(req_msg_ptr, Message::Callback::Status::SUCCESS, rsp_data_ptr);
  }
} /* UimRemoteModemEndPointModule::onQmiAsyncCb */


/*===========================================================================
  FUNCTION  handleQmiClientSetup -  UimRemoteModemEndPointModule QMI Client
                                    setup request handler
===========================================================================*/
void UimRemoteModemEndPointModule::handleQmiClientSetup
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;

  Log::getInstance().d("[UimRemoteModemEndPointModule]: handleQmiClientSetup()");

  QCRIL_LOG_INFO( "UimRemoteModemEndPointModule : %d", mOwner->getState());

  if (mServiceObject == nullptr)
  {
    mServiceObject = uim_remote_get_service_object_v01();
    if (mServiceObject == nullptr)
    {
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]:Did not get uim_get_service_object");
      return;
    }
    else
    {
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]:Got uim_get_service_object");
    }

    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);

    if (rc == QMI_NO_ERR)
    {
      mMessageList.push_back(msg);
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmi_uim_notify_cb, this);
      if (rc != QMI_NO_ERR)
      {
        Log::getInstance().d(
            "[UimRemoteModemEndPointModule]: qmi_client_register_notify_cb "
            "failed: " +
            std::to_string(rc));
      }
    }
    else
    {
      Log::getInstance().d(
          "[UimRemoteModemEndPointModule]: qmi_client_notifier_init failed: " +
          std::to_string(rc));
    }
    return;
  }
  else
  {
    mMessageList.push_back(msg);
    return;
  }
} /* UimRemoteModemEndPointModule::handleQmiClientSetup */


/*===========================================================================
  FUNCTION  handleQmiServiceUp -  UimRemoteModemEndPointModule QMI service up
                                  indication handler
===========================================================================*/
void UimRemoteModemEndPointModule::handleQmiServiceUp
(
  std::shared_ptr<Message> msg
)
{
  qmi_client_error_type                     client_err     = QMI_NO_ERR;
  qmi_client_type                           qmi_svc_client = nullptr;
  qmi_service_info                          info;
  std::shared_ptr<qmi_uim_remote_ind_msg>   ind_msg_ptr    =
    std::shared_ptr<qmi_uim_remote_ind_msg>(new qmi_uim_remote_ind_msg{});

  (void)msg;
  Log::getInstance().d("[UimRemoteModemEndPointModule]: handleQmiServiceUp()");

  if (mQmiSvcClient != nullptr)
  {
    QCRIL_LOG_INFO( "Already Registered to service");
    return;
  }

  memset(&mOsParams, 0x00, sizeof(mOsParams));

  client_err = qmi_client_get_any_service(mServiceObject, &info);
  Log::getInstance().d("[UimRemoteModemEndPointModule]: client_info: " + std::to_string(client_err));

  client_err = qmi_client_init(&info,
                               mServiceObject,
                               (qmi_client_ind_cb)qcril_uim_remote_ind_callback,
                               this,
                               &mOsParams,
                               &qmi_svc_client);

  Log::getInstance().d("[UimRemoteModemEndPointModule]: client_err = " +
                       std::to_string(client_err));
  if (client_err != QMI_NO_ERR)
  {
    Log::getInstance().d("[UimRemoteModemEndPointModule]: Error in client init");
  }
  else
  {
    Log::getInstance().d("[UimRemoteModemEndPointModule]: No Error in client init");
    QCRIL_LOG_INFO( "Qmi service client : %p", qmi_svc_client);

    mQmiSvcClient = qmi_svc_client;
    mOwner->setState(ModemEndPoint::State::OPERATIONAL);

    // Notify clients of this end-point that  we have qmi client handle.
    for (auto msg : mMessageList)
    {
      Log::getInstance().d("[UimRemoteModemEndPointModule]: Notify client = " +
                           msg->dump());
      QCRIL_LOG_INFO( "handleQmiServiceUp : %d", mOwner->getState());
      std::shared_ptr<QmiUimRemoteSetupRequest> shared_msg =
          std::static_pointer_cast<QmiUimRemoteSetupRequest>(msg);
      if(shared_msg)
      {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
                                 nullptr);
      }
    }

    if (ind_msg_ptr == nullptr)
    {
      return;
    }

    ind_msg_ptr->ind_id = QMI_UIM_REMOTE_SRVC_UP_IND_MSG;

    std::shared_ptr<UimRmtQmiUimRemoteIndMsg> ind_ptr =
         std::make_shared<UimRmtQmiUimRemoteIndMsg>(ind_msg_ptr);
    if (ind_ptr != nullptr)
    {
      ind_ptr->broadcast();
    }

    mMessageList.clear();
  }
} /* UimRemoteModemEndPointModule::handleQmiServiceUp */


/*===========================================================================
  FUNCTION  handleQmiServiceDown - UimRemoteModemEndPointModule QMI service
                                   down indication handler
===========================================================================*/
void UimRemoteModemEndPointModule::handleQmiServiceDown
(
  std::shared_ptr<Message> msg
)
{
  std::shared_ptr<qmi_uim_remote_ind_msg>   ind_msg_ptr          =
    std::shared_ptr<qmi_uim_remote_ind_msg>(new qmi_uim_remote_ind_msg{});

  (void)msg;

  if (mQmiSvcClient != NULL)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }

  QCRIL_LOG_INFO( "handleQmiServiceDown : %d", mOwner->getState());

  mOwner->setState(ModemEndPoint::State::NON_OPERATIONAL);
  memset(&remote_sim_info, 0 , sizeof(remote_sim_info));

  if (ind_msg_ptr == nullptr)
  {
    return;
  }

  ind_msg_ptr->ind_id = QMI_UIM_REMOTE_SRVC_DOWN_IND_MSG;

  std::shared_ptr<UimRmtQmiUimRemoteIndMsg> ind_ptr =
       std::make_shared<UimRmtQmiUimRemoteIndMsg>(ind_msg_ptr);
  if (ind_ptr != nullptr)
  {
    ind_ptr->broadcast();
  }
} /* UimRemoteModemEndPointModule::handleQmiServiceDown */


/*===========================================================================
  FUNCTION  handleQcrilUimRequest - UimRemoteModemEndPointModule QCRIL request
                                    handler
===========================================================================*/
void UimRemoteModemEndPointModule::handleQcrilUimRmtRequest
(
  std::shared_ptr<UimRmtQmiUimRemoteReqMsg> msg_ptr
)
{
  int                                           ret       = -1;
  UimRemoteUserData                           * user_data = nullptr;
  std::shared_ptr<qmi_uim_remote_request_msg>   req_ptr   = nullptr;
  std::shared_ptr<qmi_uim_remote_resp_msg>      resp_ptr  = nullptr;

  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_INFO("handleQcrilUimRmtRequest, null request msg_ptr");
    return;
  }

  req_ptr = msg_ptr->get_message();

  user_data = new UimRemoteUserData{msg_ptr, this};

  resp_ptr = std::shared_ptr<qmi_uim_remote_resp_msg>(new qmi_uim_remote_resp_msg{});

  if (resp_ptr != nullptr)
  {
    resp_ptr->result = QMI_UIM_REMOTE_RESULT_FAILURE;
    resp_ptr->trans_err = -1;
    resp_ptr->err_code = QMI_ERR_INTERNAL_V01;
  }
  
  QCRIL_LOG_INFO("handleQcrilUimRmtRequest, Service state %d", mOwner->getState());

  if (user_data != nullptr &&
      req_ptr != nullptr &&
      mOwner->getState() == ModemEndPoint::State::OPERATIONAL)
  {
    QCRIL_LOG_INFO("handleQcrilUimRmtRequest, request type %d", req_ptr->req_id);
    switch (req_ptr->req_id)
    {
      case QMI_UIM_REMOTE_REQ_EVENT_REQUEST:
        ret = qcril_uim_remote_client_request_event(&req_ptr->req_data.event_req, user_data);
        break;
      case QMI_UIM_REMOTE_REQ_APDU_REQUEST:
        ret = qcril_uim_remote_client_request_apdu(&req_ptr->req_data.apdu_req, user_data);
        break;
      default:
        break;
    }
  }

  if (ret != 0)
  {
    QCRIL_LOG_INFO("handleQcrilUimRmtRequest, Failed to send request");
    msg_ptr->sendResponse(msg_ptr, Message::Callback::Status::FAILURE, resp_ptr);
    if (user_data != nullptr)
    {
      delete user_data;
    }
  }
} /* UimRemoteModemEndPointModule::handleQcrilUimRmtRequest */
