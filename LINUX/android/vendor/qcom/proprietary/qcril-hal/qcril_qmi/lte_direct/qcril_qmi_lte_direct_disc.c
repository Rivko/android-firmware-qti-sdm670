/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_lte_direct_disc.c
  @brief   qcril qmi - lte direct discovery

  DESCRIPTION
    Handles requests, callbacks, indications for QMI LTE.

******************************************************************************/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "qcril_qmi_client.h"
#include "qcril_qmi_lte_direct_disc.h"
#include "qcril_reqlist.h"
#include "qcril_qmi_lte_direct_disc_misc.h"
#include "qcril_qmi_oemhook_agent.h"
#include "lte_v01.h"
#include "qcril_qmi_radio_config_socket.h"

/*===========================================================================
                    INTERNAL DEFINITIONS AND TYPES
===========================================================================*/

boolean qcril_qmi_nas_dms_is_in_online_mode();
/*===========================================================================
                    QMI LTE DIRECT DISC GLOBALS
===========================================================================*/
static qcril_qmi_lte_direct_disc_overview_type lte_direct_disc_overview;

/*===========================================================================
                                FUNCTIONS
===========================================================================*/
static void qcril_qmi_lte_direct_disc_terminate_all_apps
(
 void
);
static qcril_qmi_lte_direct_disc_exec_overview_type *
qcril_qmi_lte_direct_disc_add_to_exec_overview
(
 const char *os_app_id,
 const char *exp,
 uint32_t    op
);
static void
qcril_qmi_lte_direct_disc_remove_from_exec_overview
(
 const char *os_app_id,
 const char *exp,
 uint32_t    op
);


/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_set_lted_config_sync

===========================================================================*/
/*!
  @brief
  Sets LTED CONFIG to modem

  @return
  RIL_Errno
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_set_lted_config_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedConfig *config
)
{
  qmi_lte_disc_set_lted_config_req_msg_v01   req;
  qmi_lte_disc_set_lted_config_resp_msg_v01 *resp = NULL;
  RIL_Errno                                  ril_err   = RIL_E_GENERIC_FAILURE;
  size_t i;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0x00, sizeof(req));

  do
  {
    if (config == NULL)
    {
      QCRIL_LOG_ERROR("Null data received");
      break;
    }

    if (!config->has_osId ||
        !config->osId.has_lsb || !config->osId.has_msb)
    {
      QCRIL_LOG_ERROR("Invalid OS ID");
      break;
    }

    QCRIL_LOG_INFO("OSID lsb = 0x%lx, msb = 0x%lx", config->osId.lsb, config->osId.msb);
    QCRIL_LOG_INFO("OSID = 0x%lx%lx", config->osId.msb, config->osId.lsb);

    req.OsId.value_1_64   = config->osId.lsb;
    req.OsId.value_65_128 = config->osId.msb;

    QCRIL_LOG_INFO("config->apn = %p", config->apn);
    if (config->apn != NULL)
    {
      int len = strlen(config->apn);
      QCRIL_LOG_INFO("apn_len = %d, apn = %s", len, config->apn);
      if (len > 0)
      {
        req.DedicatedApnName_valid = TRUE;
        memcpy(&req.DedicatedApnName, config->apn, len);
      }
    }

    QCRIL_LOG_INFO("config->announcingPolicy_count = %d", config->announcingPolicy_count);
    if (config->announcingPolicy_count > 0)
    {
      req.AnnouncingPolicyList_valid = TRUE;
      req.AnnouncingPolicyList_len = config->announcingPolicy_count;

      for (i = 0; i < config->announcingPolicy_count; i++)
      {
        com_qualcomm_qti_radioconfiginterface_AnnouncingPolicy ann_policy =
                config->announcingPolicy[i];

        // PLMN
        qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi(
                ann_policy.plmn,
                &(req.AnnouncingPolicyList[i].PlmnId));

        if (ann_policy.has_validityTime)
        {
          // T4005 ValidityTime
          req.AnnouncingPolicyList[i].T4005_ValidityTime = ann_policy.validityTime;
          QCRIL_LOG_INFO("announcingPolicy[%d]: validityTime = %d", i, ann_policy.validityTime);
        }

        if (ann_policy.has_range)
        {
          // Range
          QCRIL_LOG_INFO("announcingPolicy[%d]: range = %d", i, ann_policy.range);
          qcril_qmi_lte_direct_disc_map_ril_range_to_qmi(
                  ann_policy.range,
                  &(req.AnnouncingPolicyList[i].Range));
        }
      }
    }

    QCRIL_LOG_INFO("config->monitoringPolicy_count = %d", config->monitoringPolicy_count);
    if (config->monitoringPolicy_count > 0)
    {
      req.MonitoringPolicyList_valid = TRUE;
      req.MonitoringPolicyList_len = config->monitoringPolicy_count;

      for (i = 0; i < config->monitoringPolicy_count; i++)
      {
        com_qualcomm_qti_radioconfiginterface_MonitoringPolicy mon_policy =
                config->monitoringPolicy[i];

        // PLMN
        qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi(
                mon_policy.plmn,
                &(req.MonitoringPolicyList[i].plmn_id));

        if (mon_policy.has_validityTime)
        {
          // T4005 ValidityTime
          req.MonitoringPolicyList[i].T4005_ValidityTime = mon_policy.validityTime;
          QCRIL_LOG_INFO("MonitoringPolicy[%d]: validityTime = %d", i, mon_policy.validityTime);
        }

        if (mon_policy.has_remainingTime)
        {
          // RemainingTime
          req.MonitoringPolicyList[i].RemainingTime = mon_policy.remainingTime;
          QCRIL_LOG_INFO("MonitoringPolicy[%d]: remainingTime = %d", i, mon_policy.remainingTime);
        }
      }
    }
    QCRIL_LOG_INFO("config->bakPassword->size = %d",
            config->bakPassword ? config->bakPassword->size : -1);
    if (config->bakPassword && config->bakPassword->size > 0)
    {
      req.BAKPassword_valid = TRUE;
      req.BAKPassword_len = config->bakPassword->size;
      if (req.BAKPassword_len < QMI_LTE_DISC_PSK_MAX_LEN_V01)
      {
        memcpy(&req.BAKPassword, config->bakPassword->bytes, req.BAKPassword_len);
      }
      qcril_qmi_print_hex(config->bakPassword->bytes, config->bakPassword->size);
    }

    resp = qcril_malloc(sizeof(qmi_lte_disc_set_lted_config_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    ril_err = qcril_qmi_client_send_msg_sync_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_SET_LTED_CONFIG_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN();
  return ril_err;
} /* qcril_qmi_lte_direct_disc_set_lted_config_sync */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_lted_config_sync

===========================================================================*/
/*!
  @brief
  Gets LTED CONFIG from modem

  @return
  RIL_Errno
  Out parameter config is filled if returns RIL_E_SUCCESS
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_get_lted_config_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedConfig *config /* Out parameter */
)
{
  qmi_lte_disc_get_lted_config_resp_msg_v01 *resp = NULL;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  size_t i;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (config == NULL)
    {
      QCRIL_LOG_ERROR("Null data received");
      break;
    }

    resp = qcril_malloc(sizeof(qmi_lte_disc_get_lted_config_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    ril_err = qcril_qmi_client_send_msg_sync_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_GET_LTED_CONFIG_REQ_V01,
            NULL,
            0,
            resp,
            sizeof(*resp),
            QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);

    if (ril_err == RIL_E_SUCCESS)
    {
      QCRIL_LOG_INFO("DedicatedApnName_valid = %d", resp->DedicatedApnName_valid);
      if (resp->DedicatedApnName_valid)
      {
        QCRIL_LOG_INFO("DedicatedApnName = %s", resp->DedicatedApnName);
        /*
        int len = strlen(resp->DedicatedApnName);
        config->apn = qcril_malloc(len+1);
        memcpy(config->apn, resp->DedicatedApnName, len);
        */
        config->apn = qmi_ril_util_str_clone(resp->DedicatedApnName);
      }
      else
      {
        QCRIL_LOG_INFO("DedicatedApnName not valid");
      }

      QCRIL_LOG_INFO("AnnouncingPolicy_valid = %d, AnnouncingPolicy_len = %d",
          resp->AnnouncingPolicy_valid, resp->AnnouncingPolicy_len);
      if (resp->AnnouncingPolicy_valid)
      {
        config->announcingPolicy_count = resp->AnnouncingPolicy_len;
        config->announcingPolicy = qcril_malloc(
                sizeof(com_qualcomm_qti_radioconfiginterface_LtedConfig) *
                config->announcingPolicy_count);

        if (config->announcingPolicy == NULL)
        {
          QCRIL_LOG_ERROR("Memory allocation failed");
          ril_err = RIL_E_GENERIC_FAILURE;
          break;
        }

        for(i = 0; i < resp->AnnouncingPolicy_len; i++)
        {
          qmi_lte_disc_announcing_policy_info_type_v01 qmi_ann_policy =
                  resp->AnnouncingPolicy[i];

          config->announcingPolicy[i].plmn =
                  qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_Plmn));

          QCRIL_LOG_INFO("AnnouncingPolicy[%d]: plmn = 0x%x, 0x%x, 0x%x",
                  i,
                  qmi_ann_policy.PlmnId.plmn_id[0],
                  qmi_ann_policy.PlmnId.plmn_id[1],
                  qmi_ann_policy.PlmnId.plmn_id[2]);
          // PLMN
          qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril(
                  &(qmi_ann_policy.PlmnId),
                  config->announcingPolicy[i].plmn);

          QCRIL_LOG_INFO("AnnouncingPolicy[%d]: T4005_ValidityTime = %d",
                  i, qmi_ann_policy.T4005_ValidityTime);
          config->announcingPolicy[i].has_validityTime = TRUE;
          config->announcingPolicy[i].validityTime = qmi_ann_policy.T4005_ValidityTime;

          QCRIL_LOG_INFO("AnnouncingPolicy[%d]: Range = %d", i, qmi_ann_policy.Range);
          config->announcingPolicy[i].has_range =
              qcril_qmi_lte_direct_disc_map_qmi_range_to_ril(
                      qmi_ann_policy.Range,
                      &(config->announcingPolicy[i].range));
        }
      }

      QCRIL_LOG_INFO("MonitoringPolicy_valid = %d, MonitoringPolicy_len = %d",
          resp->MonitoringPolicy_valid, resp->MonitoringPolicy_len);
      if (resp->MonitoringPolicy_valid)
      {
        config->monitoringPolicy_count = resp->MonitoringPolicy_len;
        config->monitoringPolicy = qcril_malloc(
                sizeof(com_qualcomm_qti_radioconfiginterface_LtedConfig) *
                config->monitoringPolicy_count);

        if (config->monitoringPolicy == NULL)
        {
          QCRIL_LOG_ERROR("Memory allocation failed");
          ril_err = RIL_E_GENERIC_FAILURE;
          break;
        }

        for(i = 0; i < resp->MonitoringPolicy_len; i++)
        {
          qmi_lte_disc_monitoring_policy_info_type_v01 qmi_mon_policy =
                  resp->MonitoringPolicy[i];

          config->monitoringPolicy[i].plmn =
              qcril_malloc(sizeof(com_qualcomm_qti_radioconfiginterface_Plmn));

          QCRIL_LOG_INFO("MonitoringPolicy[%d]: plmn = 0x%x, 0x%x, 0x%x", i,
                  qmi_mon_policy.plmn_id.plmn_id[0],
                  qmi_mon_policy.plmn_id.plmn_id[1],
                  qmi_mon_policy.plmn_id.plmn_id[2]);

          // PLMN
          qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril(
                  &(qmi_mon_policy.plmn_id),
                  config->monitoringPolicy[i].plmn);

          // Validity time
          QCRIL_LOG_INFO("MonitoringPolicy[%d]: T4005_ValidityTime = %d",
                  i, qmi_mon_policy.T4005_ValidityTime);
          config->monitoringPolicy[i].has_validityTime = TRUE;
          config->monitoringPolicy[i].validityTime = qmi_mon_policy.T4005_ValidityTime;

          // Remaining time
          QCRIL_LOG_INFO("MonitoringPolicy[%d]: RemainingTime = %d",
                  i, qmi_mon_policy.RemainingTime);
          config->monitoringPolicy[i].has_remainingTime = TRUE;
          config->monitoringPolicy[i].remainingTime = qmi_mon_policy.RemainingTime;
        }
      }
    }

  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN();
  return ril_err;
} /* qcril_qmi_lte_direct_disc_get_lted_config_sync */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_set_lted_category_sync

===========================================================================*/
/*!
  @brief
  Sets LTED CATEGORY to modem

  @return
  RIL_Errno
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_set_lted_category_sync
(
   com_qualcomm_qti_radioconfiginterface_LtedCategory *category
)
{
  qmi_lte_disc_set_lted_category_req_msg_v01   req;
  qmi_lte_disc_set_lted_category_resp_msg_v01 *resp = NULL;
  RIL_Errno ril_err   = RIL_E_GENERIC_FAILURE;
  int       len = 0;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0x00, sizeof(req));

  do
  {
    if (category == NULL)
    {
      QCRIL_LOG_ERROR("Null data received");
      break;
    }

    if (category->osAppId != NULL)
    {
      len = strlen(category->osAppId);
      QCRIL_LOG_INFO("osAppId_len = %d, osAppId = %s", len, category->osAppId);
      if (len > 0 && len <= QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        req.OsAppId_valid = TRUE;
        memcpy(&req.OsAppId, category->osAppId, len);
      }
      else
      {
        QCRIL_LOG_ERROR("Invalid osAppId; len is %d", len);
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Invalid osAppId");
      break;
    }

    if (category->has_category)
    {
      req.lted_category_valid = qcril_qmi_lte_direct_disc_map_ril_category_to_qmi(
              category->category,
              &(req.lted_category));
      if (req.lted_category_valid != TRUE)
      {
        QCRIL_LOG_ERROR("Invalid Category");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Category is not set");
      break;
    }

    resp = qcril_malloc(sizeof(qmi_lte_disc_set_lted_category_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    ril_err = qcril_qmi_client_send_msg_sync_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_SET_LTED_CATEGORY_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN();
  return ril_err;
} /* qcril_qmi_lte_direct_disc_set_lted_category_sync */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_lted_category_sync

===========================================================================*/
/*!
  @brief
  Gets LTED CATEGORY from modem

  @return
  RIL_Errno
  Out parameter category is filled if returns RIL_E_SUCCESS
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_get_lted_category_sync
(
 char     *osAppId,
 uint32_t *category /* Out parameter */
)
{
  qmi_lte_disc_get_lted_category_req_msg_v01   req;
  qmi_lte_disc_get_lted_category_resp_msg_v01 *resp = NULL;
  RIL_Errno ril_err = RIL_E_GENERIC_FAILURE;
  int       len     = 0;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0x00, sizeof(req));

  do
  {
    if (osAppId == NULL || category == NULL)
    {
      QCRIL_LOG_ERROR("Null data received");
      break;
    }

    len = strlen(osAppId);
    QCRIL_LOG_INFO("osAppId_len = %d, osAppId = %s", len, osAppId);
    if (len > 0 && len <= QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
    {
      req.OsAppId_valid = TRUE;
      memcpy(&req.OsAppId, osAppId, len);
    }
    else
    {
      QCRIL_LOG_ERROR("Invalid osAppId; len is %d", len);
      break;
    }

    resp = qcril_malloc(sizeof(qmi_lte_disc_get_lted_category_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    ril_err = qcril_qmi_client_send_msg_sync_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_GET_CATEGORY_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            QCRIL_QMI_SYNC_REQ_UNRESTRICTED_TIMEOUT);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);

    if (ril_err == RIL_E_SUCCESS)
    {
      QCRIL_LOG_INFO("lted_category_valid = %d, lted_category = %d",
              resp->lted_category_valid, resp->lted_category);

      if (!resp->lted_category_valid ||
              !qcril_qmi_lte_direct_disc_map_qmi_category_to_ril(
                  resp->lted_category, category))
      {
        QCRIL_LOG_ERROR("lted_category not valid");
        ril_err = RIL_E_GENERIC_FAILURE;
      }
    }
  }while(FALSE);

  QCRIL_LOG_FUNC_RETURN();
  return ril_err;
} /* qcril_qmi_lte_direct_disc_get_lted_category_sync */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_initialize

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_INITIALIZE

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_initialize
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  qcril_qmi_lte_direct_disc_terminate_all_apps();

  // Send success response
  qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
          LteDirectDiscovery_MsgType_RESPONSE,
          qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
          LteDirectDiscovery_Error_E_SUCCESS,
          NULL, 0);

  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_device_capability

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_DEVICE_CAPABILITY

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_get_device_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qmi_lte_get_subscription_info_resp_msg_v01   *resp = NULL;
  RIL_Errno                                     ril_err = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type                     reqlist_entry;
  uint32                                        user_data;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  do {
    resp = qcril_malloc(sizeof(qmi_lte_get_subscription_info_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_GET_SUBSCRIPTION_INFO_REQ_V01,
            NULL,
            0,
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_get_device_capability */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_terminate

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_TERMINATE

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_terminate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  LteDirectDiscovery_Terminate        *lted_in_data_ptr = NULL;
  qmi_lte_disc_terminate_req_msg_v01   req;
  qmi_lte_disc_terminate_resp_msg_v01 *resp = NULL;
  RIL_Errno                            ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type            reqlist_entry;
  uint32                               user_data;

  char *os_app_id  = NULL;
  int   len        = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  memset(&req, 0x00, sizeof(req));

  do
  {
    lted_in_data_ptr = (LteDirectDiscovery_Terminate *)params_ptr->data;

    if (lted_in_data_ptr == NULL)
    {
      QCRIL_LOG_ERROR("Null data received");
      break;
    }

    os_app_id = (char *)(lted_in_data_ptr->osAppId.arg);
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
      if (len > QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid OS App ID length: %d", len);
        break;
      }
      memcpy(&req.OsAppId, os_app_id, len);
    }
    else
    {
      QCRIL_LOG_ERROR("OS App ID is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("OS App ID = \"%s\"\n", os_app_id);

    resp = qcril_malloc(sizeof(qmi_lte_disc_terminate_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_TERMINATE_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  else
  {
    qcril_qmi_lte_direct_disc_remove_from_exec_overview(
        os_app_id, NULL,
        QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH|QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_terminate */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_service_status

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_SERVICE_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_get_service_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  qmi_lte_disc_get_service_status_resp_msg_v01 *resp = NULL;
  RIL_Errno                                     ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type                     reqlist_entry;
  uint32                                        user_data;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  do {
    resp = qcril_malloc(sizeof(qmi_lte_disc_get_service_status_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_GET_SERVICE_STATUS_REQ_V01,
            NULL,
            0,
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_get_service_status */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_publish

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_PUBLISH

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_publish
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  LteDirectDiscovery_Publish        *lted_in_data_ptr = NULL;
  qmi_lte_disc_publish_req_msg_v01   req;
  qmi_lte_disc_publish_resp_msg_v01 *resp = NULL;
  RIL_Errno                          ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type          reqlist_entry;
  uint32                             user_data;

  char *os_app_id  = NULL;
  char *expression = NULL;
  char *meta_data  = NULL;
  int   len        = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  memset(&req, 0x00, sizeof(req));

  do
  {
    lted_in_data_ptr = (LteDirectDiscovery_Publish *)params_ptr->data;

    os_app_id = (char *)(lted_in_data_ptr->osAppId.arg);
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
      if (len > QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid OS App ID length: %d", len);
        break;
      }
      memcpy(&req.OsAppId, os_app_id, len);
    }
    else
    {
      QCRIL_LOG_ERROR("OS App ID is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("OS App ID = \"%s\"\n", os_app_id);

    expression = (char *) lted_in_data_ptr->expression.arg;
    if (expression)
    {
      len = strlen(expression);
      if (len > QMI_LTE_DISC_PA_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid Expression length: %d", len);
        break;
      }
      req.Expression_valid = TRUE;
      memcpy(&req.Expression.prose_app_id_name, expression, len);
    }
    else
    {
      QCRIL_LOG_ERROR("Expression is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("expression = \"%s\"\n", expression);

    QCRIL_LOG_INFO("has_expressionValidityTime = %d, expressionValidityTime = %d",
            lted_in_data_ptr->has_expressionValidityTime,
            lted_in_data_ptr->expressionValidityTime);
    req.ExpressionValidityTime_valid = lted_in_data_ptr->has_expressionValidityTime;
    req.ExpressionValidityTime = lted_in_data_ptr->expressionValidityTime;

    meta_data = (char *)lted_in_data_ptr->metaData.arg;
    if (meta_data)
    {
      len = strlen(meta_data);
      if (len > QMI_LTE_DISC_METADATA_MAX_V01)
      {
        QCRIL_LOG_INFO("Invalid Meta Data length: %d", len);
      }
      else
      {
        req.MetaData_valid = TRUE;
        memcpy(&req.MetaData, meta_data, len);
        QCRIL_LOG_INFO("Meta Data = \"%s\"\n", meta_data);
      }
    }
    else
    {
      QCRIL_LOG_INFO("Meta Data is NULL!!");
    }

    QCRIL_LOG_INFO("has_discoveryType = %d, discoveryType = %d",
            lted_in_data_ptr->has_discoveryType,
            lted_in_data_ptr->discoveryType);
    if (lted_in_data_ptr->has_discoveryType)
    {
      req.DiscoveryType_valid = qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi(
              lted_in_data_ptr->discoveryType,
              &req.DiscoveryType);
    }

    QCRIL_LOG_INFO("has_duration = %d, duration = %d",
            lted_in_data_ptr->has_duration,
            lted_in_data_ptr->duration);
    req.PublishDuration_valid = lted_in_data_ptr->has_duration;
    req.PublishDuration = lted_in_data_ptr->duration;


    resp = qcril_malloc(sizeof(qmi_lte_disc_publish_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_PUBLISH_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  else
  {
    qcril_qmi_lte_direct_disc_add_to_exec_overview(os_app_id, expression,
        QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_publish */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_cancel_publish

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_PUBLISH

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_cancel_publish
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr /*!< Output parameter */
)
{
  LteDirectDiscovery_CancelPublish         *lted_in_data_ptr = NULL;
  qmi_lte_disc_cancel_publish_req_msg_v01   req;
  qmi_lte_disc_cancel_publish_resp_msg_v01 *resp = NULL;
  RIL_Errno                                 ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type                 reqlist_entry;
  uint32                                    user_data;

  char *os_app_id  = NULL;
  char *expression = NULL;
  int   len        = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  memset(&req, 0x00, sizeof(req));

  do
  {
    lted_in_data_ptr = (LteDirectDiscovery_CancelPublish *)params_ptr->data;

    os_app_id = (char *)(lted_in_data_ptr->osAppId.arg);
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
      if (len > QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid OS App ID length: %d", len);
        break;
      }
      memcpy(&req.OsAppId, os_app_id, len);
    }
    else
    {
      QCRIL_LOG_ERROR("OS App ID is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("OS App ID = \"%s\"\n", os_app_id);

    expression = (char *) lted_in_data_ptr->expression.arg;
    if (expression)
    {
      len = strlen(expression);
      if (len > QMI_LTE_DISC_PA_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid Expression length: %d", len);
        break;
      }
      req.Expression_valid = TRUE;
      memcpy(&req.Expression.prose_app_id_name, expression, len);
    }
    else
    {
      QCRIL_LOG_ERROR("Expression is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("expression = \"%s\"\n", expression);

    resp = qcril_malloc(sizeof(qmi_lte_disc_cancel_publish_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_CANCEL_PUBLISH_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  else
  {
    qcril_qmi_lte_direct_disc_remove_from_exec_overview(os_app_id, expression,
        QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_cancel_publish */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_subscribe

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_SUBSCRIBE

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_subscribe
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  LteDirectDiscovery_Subscribe        *lted_in_data_ptr = NULL;
  qmi_lte_disc_subscribe_req_msg_v01   req;
  qmi_lte_disc_subscribe_resp_msg_v01 *resp = NULL;
  RIL_Errno                            ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type            reqlist_entry;
  uint32                               user_data;

  char *os_app_id  = NULL;
  char *expression = NULL;
  int   len        = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  memset(&req, 0x00, sizeof(req));

  do
  {
    lted_in_data_ptr = (LteDirectDiscovery_Subscribe *)params_ptr->data;

    os_app_id = (char *)(lted_in_data_ptr->osAppId.arg);
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
      if (len > QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid OS App ID length: %d", len);
        break;
      }
      memcpy(&req.OsAppId, os_app_id, len);
    }
    else
    {
      QCRIL_LOG_ERROR("OS App ID is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("OS App ID = \"%s\"\n", os_app_id);

    expression = (char *) lted_in_data_ptr->expression.arg;
    if (expression)
    {
      len = strlen(expression);
      if (len > QMI_LTE_DISC_PA_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid Expression length: %d", len);
        break;
      }
      req.Expression_valid = TRUE;
      memcpy(&req.Expression.prose_app_id_name, expression, len);
    }
    else
    {
      QCRIL_LOG_ERROR("Expression is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("expression = \"%s\"\n", expression);

    QCRIL_LOG_INFO("has_expressionValidityTime = %d, expressionValidityTime = %d",
            lted_in_data_ptr->has_expressionValidityTime,
            lted_in_data_ptr->expressionValidityTime);
    req.ExpressionValidityTime_valid = lted_in_data_ptr->has_expressionValidityTime;
    req.ExpressionValidityTime = lted_in_data_ptr->expressionValidityTime;

    QCRIL_LOG_INFO("has_discoveryType = %d, discoveryType = %d",
            lted_in_data_ptr->has_discoveryType,
            lted_in_data_ptr->discoveryType);
    if (lted_in_data_ptr->has_discoveryType)
    {
      req.DiscoveryType_valid = qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi(
              lted_in_data_ptr->discoveryType,
              &req.DiscoveryType);
    }

    QCRIL_LOG_INFO("has_duration = %d, duration = %d",
            lted_in_data_ptr->has_duration,
            lted_in_data_ptr->duration);
    req.SubscribeDuration_valid = lted_in_data_ptr->has_duration;
    req.SubscribeDuration = lted_in_data_ptr->duration;

    resp = qcril_malloc(sizeof(qmi_lte_disc_subscribe_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_SUBSCRIBE_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  else
  {
    qcril_qmi_lte_direct_disc_add_to_exec_overview(os_app_id, expression,
        QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_subscribe */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_cancel_subscribe

===========================================================================*/
/*!
  @brief
  Handles QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_SUBSCRIBE

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_cancel_subscribe
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
)
{
  LteDirectDiscovery_CancelSubscribe         *lted_in_data_ptr = NULL;
  qmi_lte_disc_cancel_subscribe_req_msg_v01   req;
  qmi_lte_disc_cancel_subscribe_resp_msg_v01 *resp = NULL;
  RIL_Errno                                   ril_err   = RIL_E_GENERIC_FAILURE;
  qcril_reqlist_public_type                   reqlist_entry;
  uint32                                      user_data;

  char *os_app_id  = NULL;
  char *expression = NULL;
  int   len        = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  memset(&req, 0x00, sizeof(req));

  do
  {
    lted_in_data_ptr = (LteDirectDiscovery_CancelSubscribe *)params_ptr->data;

    os_app_id = (char *)(lted_in_data_ptr->osAppId.arg);
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
      if (len > QMI_LTE_DISC_OS_APP_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid OS App ID length: %d", len);
        break;
      }
      memcpy(&req.OsAppId, os_app_id, len);
    }
    else
    {
      QCRIL_LOG_ERROR("OS App ID is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("OS App ID = \"%s\"\n", os_app_id);

    expression = (char *) lted_in_data_ptr->expression.arg;
    if (expression)
    {
      len = strlen(expression);
      if (len > QMI_LTE_DISC_PA_ID_NAME_MAX_V01)
      {
        QCRIL_LOG_ERROR("Invalid Expression length: %d", len);
        break;
      }
      req.Expression_valid = TRUE;
      memcpy(&req.Expression.prose_app_id_name, expression, len);
    }
    else
    {
      QCRIL_LOG_ERROR("Expression is NULL!!");
      break;
    }
    QCRIL_LOG_INFO("len = %d", len);
    QCRIL_LOG_INFO("expression = \"%s\"\n", expression);

    resp = qcril_malloc(sizeof(qmi_lte_disc_cancel_subscribe_resp_msg_v01));
    if (resp == NULL)
    {
      QCRIL_LOG_ERROR("Memory allocation failed");
      break;
    }

    qcril_reqlist_default_entry(params_ptr->t,
            params_ptr->event_id,
            QCRIL_DEFAULT_MODEM_ID,
            QCRIL_REQ_AWAITING_CALLBACK,
            QCRIL_EVT_NONE,
            NULL,
            &reqlist_entry);

    if (qcril_reqlist_new(QCRIL_DEFAULT_INSTANCE_ID,
                &reqlist_entry) != E_SUCCESS)
    {
      QCRIL_LOG_ERROR("Failed to add into Req list");
      break;
    }

    user_data = QCRIL_COMPOSE_USER_DATA(QCRIL_DEFAULT_INSTANCE_ID,
            QCRIL_DEFAULT_MODEM_ID,
            reqlist_entry.req_id);

    ril_err = qcril_qmi_client_send_msg_async_ex (
            QCRIL_QMI_CLIENT_LTE,
            QMI_LTE_DISC_CANCEL_SUBSCRIBE_REQ_V01,
            &req,
            sizeof(req),
            resp,
            sizeof(*resp),
            (void*)(uintptr_t)user_data);
    QCRIL_LOG_ESSENTIAL("ril_err = %d", (int)ril_err);
  }while(FALSE);

  if (ril_err != RIL_E_SUCCESS)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            LteDirectDiscovery_Error_E_GENERIC_FAILURE,
            NULL, 0);
  }
  else
  {
    qcril_qmi_lte_direct_disc_remove_from_exec_overview(os_app_id, expression,
        QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_cancel_subscribe */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_send_unsol_expression_status

===========================================================================*/
/*!
  @brief
  Helper function for sending UNSOL_RESPONSE_EXPRESSION_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_send_unsol_expression_status
(
  const char                       *os_app_id,
  qmi_lte_disc_prose_app_id_v01    *expression,
  qmi_lte_disc_result_type_enum_v01 expression_result
)
{
  int     len        = 0;
  boolean send_unsol = FALSE;
  LteDirectDiscovery_ExpressionStatus exp_status;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&exp_status, 0x00, sizeof (LteDirectDiscovery_ExpressionStatus));

  do {
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
    }
    if ((os_app_id != NULL) && len > 0)
    {
      exp_status.osAppId.arg = qcril_malloc(len + 1);
      if (exp_status.osAppId.arg)
      {
        memcpy(exp_status.osAppId.arg, os_app_id, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty OS App ID; os_app_id = %s; len = %d",
              os_app_id, len);
      break;
    }

    len = 0;
    if (expression != NULL)
    {
      len = strlen(expression->prose_app_id_name);
    }
    if ((expression != NULL) && len > 0)
    {
      exp_status.expression.arg = qcril_malloc(len + 1);
      if (exp_status.expression.arg)
      {
        memcpy(exp_status.expression.arg, expression, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty Expression; expression = %s; len = %d",
              (expression ? expression->prose_app_id_name : ""), len);
      break;
    }
    exp_status.has_result = qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril(
            expression_result,
            &exp_status.result);

    // Good to go; send UNSOL_RESPONSE_EXPRESSION_STATUS
    send_unsol = TRUE;
  } while (FALSE);

  if (send_unsol)
  {
    QCRIL_LOG_INFO("sendig UNSOL_RESPONSE_EXPRESSION_STATUS");
    qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
            LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
            LteDirectDiscovery_MsgId_UNSOL_RESPONSE_EXPRESSION_STATUS,
            LteDirectDiscovery_Error_E_SUCCESS,
            &exp_status,
            sizeof(exp_status));
  }

  // free exp_status;
  if (exp_status.osAppId.arg)
  {
    qcril_free(exp_status.osAppId.arg);
  }
  if (exp_status.expression.arg)
  {
    qcril_free(exp_status.expression.arg);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_send_unsol_expression_status */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_send_unsol_authorization_result

===========================================================================*/
/*!
  @brief
  Helper function for sending UNSOL_RESPONSE_AUTHORIZATION_RESULT

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_send_unsol_authorization_result
(
  const char                       *os_app_id,
  qmi_lte_disc_result_type_enum_v01 qmi_auth_result
)
{
  int     len        = 0;
  boolean send_unsol = FALSE;

  LteDirectDiscovery_AuthorizationResult auth_result;
  memset (&auth_result, 0x00, sizeof (LteDirectDiscovery_AuthorizationResult));

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
    }
    if ((os_app_id != NULL) && len > 0)
    {
      auth_result.osAppId.arg = qcril_malloc(len + 1);
      if (auth_result.osAppId.arg)
      {
        memcpy(auth_result.osAppId.arg, os_app_id, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty OS App ID; os_app_id = %s; len = %d",
              os_app_id, len);
      break;
    }

    auth_result.has_result = qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril(
            qmi_auth_result,
            &auth_result.result);

    // Good to go; send UNSOL_RESPONSE_AUTHORIZATION_RESULT
    send_unsol = TRUE;
  } while (FALSE);

  if (send_unsol)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
            LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
            LteDirectDiscovery_MsgId_UNSOL_RESPONSE_AUTHORIZATION_RESULT,
            LteDirectDiscovery_Error_E_SUCCESS,
            &auth_result,
            sizeof(auth_result));
  }

  // free auth_result;
  if (auth_result.osAppId.arg)
  {
    qcril_free(auth_result.osAppId.arg);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_send_unsol_authorization_result */


/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_send_unsol_transmission_status

===========================================================================*/
/*!
  @brief
  Helper function for sending UNSOL_RESPONSE_TRANSMISSION_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_send_unsol_transmission_status
(
  const char                    *os_app_id,
  qmi_lte_disc_prose_app_id_v01 *expression,
  uint8_t                        publishStatusPerExpr
)
{
  int     len        = 0;
  boolean send_unsol = FALSE;

  LteDirectDiscovery_TransmissionStatus txn_status;
  memset (&txn_status, 0x00, sizeof (LteDirectDiscovery_TransmissionStatus));

  QCRIL_LOG_FUNC_ENTRY();

  do {
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
    }
    if ((os_app_id != NULL) && len > 0)
    {
      txn_status.osAppId.arg = qcril_malloc(len + 1);
      if (txn_status.osAppId.arg)
      {
        memcpy(txn_status.osAppId.arg, os_app_id, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty OS App ID; os_app_id = %s; len = %d",
              os_app_id, len);
      break;
    }

    len = 0;
    if (expression != NULL)
    {
      len = strlen(expression->prose_app_id_name);
    }
    if ((expression != NULL) && len > 0)
    {
      txn_status.expression.arg = qcril_malloc(len + 1);
      if (txn_status.expression.arg)
      {
        memcpy(txn_status.expression.arg, expression, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty Expression; expression = %s; len = %d",
              (expression ? expression->prose_app_id_name : ""), len);
      break;
    }
    txn_status.has_status = TRUE;
    if (publishStatusPerExpr == 0)
    {
      txn_status.status = 0; // Transmission is completed
    }
    else
    {
      txn_status.status = 1; // Pending Transmission
    }
    QCRIL_LOG_INFO("Transmission status = %d", txn_status.status);

    // Good to go; send UNSOL_RESPONSE_TRANSMISSION_STATUS
    send_unsol = TRUE;
  } while (FALSE);

  if (send_unsol)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
            LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
            LteDirectDiscovery_MsgId_UNSOL_RESPONSE_TRANSMISSION_STATUS,
            LteDirectDiscovery_Error_E_SUCCESS,
            &txn_status,
            sizeof(txn_status));
  }

  // free txn_status;
  if (txn_status.osAppId.arg)
  {
    qcril_free(txn_status.osAppId.arg);
  }
  if (txn_status.expression.arg)
  {
    qcril_free(txn_status.expression.arg);
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_send_unsol_transmission_status */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_send_unsol_reception_status

===========================================================================*/
/*!
  @brief
  Helper function for sending UNSOL_RESPONSE_RECEPTION_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_send_unsol_reception_status
(
  const char                    *os_app_id,
  qmi_lte_disc_prose_app_id_v01 *expression,
  uint8_t                        subscribeStatusPerExpr
)
{
  int     len        = 0;
  boolean send_unsol = FALSE;
  LteDirectDiscovery_ReceptionStatus recpn_status;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&recpn_status, 0x00, sizeof (LteDirectDiscovery_ReceptionStatus));

  do {
    if (os_app_id != NULL)
    {
      len = strlen(os_app_id);
    }
    if ((os_app_id != NULL) && len > 0)
    {
      recpn_status.osAppId.arg = qcril_malloc(len + 1);
      if (recpn_status.osAppId.arg)
      {
        memcpy(recpn_status.osAppId.arg, os_app_id, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty OS App ID; os_app_id = %s; len = %d",
              os_app_id, len);
      break;
    }

    len = 0;
    if (expression != NULL)
    {
      len = strlen(expression->prose_app_id_name);
    }
    if ((expression != NULL) && len > 0)
    {
      recpn_status.expression.arg = qcril_malloc(len + 1);
      if (recpn_status.expression.arg)
      {
        memcpy(recpn_status.expression.arg, expression, len);
      }
      else
      {
        QCRIL_LOG_ERROR("malloc failed");
        break;
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Empty Expression; expression = %s; len = %d",
              (expression ? expression->prose_app_id_name : ""), len);
      break;
    }
    recpn_status.has_status = TRUE;
    if (subscribeStatusPerExpr == 0)
    {
      recpn_status.status = 0;  // Subscription is completed
    }
    else
    {
      recpn_status.status = 1;  // Pending Subscription
    }
    QCRIL_LOG_INFO("Transmission status = %d", recpn_status.status);

    // Good to go; send UNSOL_RESPONSE_RECEPTION_STATUS
    send_unsol = TRUE;
  } while (FALSE);

  if (send_unsol)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
            LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
            LteDirectDiscovery_MsgId_UNSOL_RESPONSE_RECEPTION_STATUS,
            LteDirectDiscovery_Error_E_SUCCESS,
            &recpn_status,
            sizeof(recpn_status));
  }

  // free recpn_status;
  if (recpn_status.osAppId.arg)
  {
    qcril_free(recpn_status.osAppId.arg);
  }
  if (recpn_status.expression.arg)
  {
    qcril_free(recpn_status.expression.arg);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_send_unsol_reception_status */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_send_unsol_service_status

===========================================================================*/
/*!
  @brief
  Helper function for sending UNSOL_RESPONSE_SERVICE_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_send_unsol_service_status
(
  uint8_t PublishAllowed_valid,
  uint8_t PublishAllowed,
  uint8_t SubscribeAllowed_valid,
  uint8_t SubscribeAllowed
)
{
  boolean send_unsol = FALSE;
  LteDirectDiscovery_ServiceStatus svc_status;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&svc_status, 0x00, sizeof (LteDirectDiscovery_ServiceStatus));

  do {
    svc_status.has_publishAllowed = PublishAllowed_valid;
    svc_status.publishAllowed = PublishAllowed;
    svc_status.has_subscribeAllowed = SubscribeAllowed_valid;
    svc_status.subscribeAllowed = SubscribeAllowed;

    // Good to go; send UNSOL_RESPONSE_SERVICE_STATUS
    send_unsol = TRUE;
  } while (FALSE);

  if (send_unsol)
  {
    qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
            LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
            LteDirectDiscovery_MsgId_UNSOL_RESPONSE_SERVICE_STATUS,
            LteDirectDiscovery_Error_E_SUCCESS,
            &svc_status,
            sizeof(svc_status));
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_send_unsol_service_status */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_terminate_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle TERMINATE_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_terminate_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_terminate_resp_msg_v01 *resp = NULL;
  RIL_Errno                            ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                  qmi_result;
  qmi_error_type_v01                   qmi_error;
  LteDirectDiscovery_Error             lted_error;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_terminate_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
      QCRIL_LOG_INFO("FAILURE response; error = %d", lted_error);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
      QCRIL_LOG_INFO("SUCCESS response");
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            NULL, 0);
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_terminate_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_service_status_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle GET_SERVICE_STATUS_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_get_service_status_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_get_service_status_resp_msg_v01 *resp = NULL;
  RIL_Errno                          ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                qmi_result;
  qmi_error_type_v01                 qmi_error;
  LteDirectDiscovery_Error           lted_error;
  LteDirectDiscovery_ServiceStatus   svc_status;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&svc_status, 0x00, sizeof (LteDirectDiscovery_ServiceStatus));

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_get_service_status_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
      QCRIL_LOG_INFO("FAILURE response; error = %d", lted_error);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;

      if (resp->PublishAllowed_valid)
      {
        svc_status.has_publishAllowed = TRUE;
        svc_status.publishAllowed     = resp->PublishAllowed;
      }

      if (resp->SubscribeAllowed_valid)
      {
        svc_status.has_subscribeAllowed = TRUE;
        svc_status.subscribeAllowed     = resp->SubscribeAllowed;
      }

      QCRIL_LOG_INFO("SUCCESS response");
      QCRIL_LOG_INFO("PublishAllowed_valid = %d, PublishAllowed = %d",
          resp->PublishAllowed_valid, resp->PublishAllowed);
      QCRIL_LOG_INFO("SubscribeAllowed_valid = %d, SubscribeAllowed = %d",
          resp->SubscribeAllowed_valid, resp->SubscribeAllowed);
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            &svc_status,
            sizeof (LteDirectDiscovery_ServiceStatus));
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_get_service_status_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_publish_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle PUBLISH_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_publish_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_publish_resp_msg_v01 *resp        = NULL;
  RIL_Errno                          ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                qmi_result;
  qmi_error_type_v01                 qmi_error;
  LteDirectDiscovery_Error           lted_error;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_publish_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
      QCRIL_LOG_INFO("FAILURE response; error = %d", lted_error);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
      QCRIL_LOG_INFO("SUCCESS response");
      if (resp->OsAppId_valid)
      {
        QCRIL_LOG_INFO("OS App Id = %s", resp->OsAppId);
      }
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            NULL, 0);

    QCRIL_LOG_INFO("OsAppId_valid = %d, OsAppId = %s",
            resp->OsAppId_valid,
            resp->OsAppId_valid ? resp->OsAppId : "<not valid>");

    QCRIL_LOG_INFO("Expression_valid = %d, Expression = %s",
            resp->Expression_valid,
            resp->Expression_valid ? resp->Expression.prose_app_id_name : "<not valid>");

    QCRIL_LOG_INFO("ExpressionResult_valid = %d, ExpressionResult = %d",
            resp->ExpressionResult_valid,
            resp->ExpressionResult_valid ? resp->ExpressionResult : -1);

    if (resp->OsAppId_valid && resp->Expression_valid && resp->ExpressionResult_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_expression_status(
                resp->OsAppId,
                &(resp->Expression),
                resp->ExpressionResult);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_publish_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_subscribe_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle SUBSCRIBE_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_subscribe_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_subscribe_resp_msg_v01 *resp        = NULL;
  RIL_Errno                            ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                  qmi_result;
  qmi_error_type_v01                   qmi_error;
  LteDirectDiscovery_Error             lted_error;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_subscribe_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
      if (resp->OsAppId_valid)
      {
        QCRIL_LOG_INFO("OS App Id = %s", resp->OsAppId);
      }
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            NULL, 0);

    QCRIL_LOG_INFO("OsAppId_valid = %d, OsAppId = %s",
            resp->OsAppId_valid,
            resp->OsAppId_valid ? resp->OsAppId : "<not valid>");

    QCRIL_LOG_INFO("Expression_valid = %d, Expression = %s",
            resp->Expression_valid,
            resp->Expression_valid ? resp->Expression.prose_app_id_name : "<not valid>");

    QCRIL_LOG_INFO("ExpressionResult_valid = %d, ExpressionResult = %d",
            resp->ExpressionResult_valid,
            resp->ExpressionResult_valid ? resp->ExpressionResult : -1);

    if (resp->OsAppId_valid && resp->Expression_valid && resp->ExpressionResult_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_expression_status(
                resp->OsAppId,
                &(resp->Expression),
                resp->ExpressionResult);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_subscribe_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_cancel_publish_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle CANCEL_PUBLISH_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_cancel_publish_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_cancel_publish_resp_msg_v01 *resp        = NULL;
  RIL_Errno                                 ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                       qmi_result;
  qmi_error_type_v01                        qmi_error;
  LteDirectDiscovery_Error                  lted_error;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_cancel_publish_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
      if (resp->OsAppId_valid)
      {
        QCRIL_LOG_INFO("OS App Id = %s", resp->OsAppId);
      }
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            NULL, 0);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_cancel_publish_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_cancel_subscribe_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle CANCEL_SUBSCRIBE_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_cancel_subscribe_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_disc_cancel_subscribe_resp_msg_v01 *resp        = NULL;
  RIL_Errno                                   ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                         qmi_result;
  qmi_error_type_v01                          qmi_error;
  LteDirectDiscovery_Error                    lted_error;

  QCRIL_LOG_FUNC_ENTRY();

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_disc_cancel_subscribe_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
      if (resp->OsAppId_valid)
      {
        QCRIL_LOG_INFO("OS App Id = %s", resp->OsAppId);
      }
    }

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            NULL, 0);
  }

  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_cancel_subscribe_resp_hdlr */

/*=========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_get_subscription_info_resp_hdlr

===========================================================================*/
/*!
  @brief
  Handle GET_SUBSCRIPTION_INFO_RESP

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_get_subscription_info_resp_hdlr
(
  const qcril_request_params_type *const params_ptr
)
{
  qmi_lte_get_subscription_info_resp_msg_v01 *resp        = NULL;
  RIL_Errno                                   ril_err     = RIL_E_GENERIC_FAILURE;
  qmi_result_type_v01                         qmi_result;
  qmi_error_type_v01                          qmi_error;
  LteDirectDiscovery_Error                    lted_error;
  LteDirectDiscovery_DeviceCapability         device_capability;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&device_capability, 0x00, sizeof (LteDirectDiscovery_DeviceCapability));

  if(NULL != params_ptr->data)
  {
    resp = (qmi_lte_get_subscription_info_resp_msg_v01 *)params_ptr->data;
    qmi_result = resp->resp.result;
    qmi_error = resp->resp.error;

    ril_err = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(QMI_NO_ERR, &resp->resp);

    QCRIL_LOG_ESSENTIAL("qmi result %d, qmi err %d, ril err %d",
        (int)qmi_result, (int)qmi_error, (int)ril_err);

    if (RIL_E_SUCCESS != ril_err)
    {
      // Error response
      lted_error = qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error(&resp->resp);
    }
    else
    {
      // Success response
      lted_error = LteDirectDiscovery_Error_E_SUCCESS;
    }

    if (resp->LteDiscCapability_valid)
    {
      device_capability.has_capability = TRUE;
      device_capability.capability     = resp->LteDiscCapability;
    }

    QCRIL_LOG_INFO( "has_capability = %d, capability = %d",
        device_capability.has_capability, device_capability.capability);

    qcril_qmi_oem_socket_lte_direct_disc_send(params_ptr->t,
            LteDirectDiscovery_MsgType_RESPONSE,
            qcril_qmi_lte_direct_disc_map_event_to_request(params_ptr->event_id),
            lted_error,
            &device_capability,
            sizeof (LteDirectDiscovery_DeviceCapability));
  }
  QCRIL_LOG_FUNC_RETURN();
} /* qcril_qmi_lte_direct_disc_get_subscription_info_resp_hdlr */


/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_notification_ind_hdlr

===========================================================================*/
/*!
  @brief
  Handle QMI_LTE_DISC_NOTIFICATION_IND and send the below
  unsol indication to upper layers if the relevent TLVs
  are present.
  UNSOL_RESPONSE_EXPRESSION_STATUS
  UNSOL_RESPONSE_AUTHORIZATION_RESULT
  UNSOL_RESPONSE_TRANSMISSION_STATUS
  UNSOL_RESPONSE_RECEPTION_STATUS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_notification_ind_hdlr
(
  void *ind_data_ptr
)
{
  qmi_lte_disc_notification_ind_msg_v01 *notif_ind_msg =
      (qmi_lte_disc_notification_ind_msg_v01*) ind_data_ptr;
  uint32_t op_mask = QCRIL_QMI_LTE_DIRECT_DISC_OP_NONE;
  boolean reset_exec_overview = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  if (notif_ind_msg != NULL)
  {
    QCRIL_LOG_INFO("OsAppId_valid = %d, OsAppId = %s",
            notif_ind_msg->OsAppId_valid,
            notif_ind_msg->OsAppId_valid ? notif_ind_msg->OsAppId : "<not valid>");

    QCRIL_LOG_INFO("Expression_valid = %d, Expression = %s",
            notif_ind_msg->Expression_valid,
            notif_ind_msg->Expression_valid ?
                    notif_ind_msg->Expression.prose_app_id_name :
                    "<not valid>");

    QCRIL_LOG_INFO("ExpressionResult_valid = %d, ExpressionResult = %d",
            notif_ind_msg->ExpressionResult_valid,
            notif_ind_msg->ExpressionResult_valid ? notif_ind_msg->ExpressionResult : -1);

    if (notif_ind_msg->OsAppId_valid &&
            notif_ind_msg->Expression_valid &&
            notif_ind_msg->ExpressionResult_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_expression_status(
                notif_ind_msg->OsAppId,
                &(notif_ind_msg->Expression),
                notif_ind_msg->ExpressionResult);

        if (notif_ind_msg->ExpressionResult != QMI_LTE_DISC_SUCCESS_V01 ||
            notif_ind_msg->ExpressionResult != QMI_LTE_DISC_IN_PROGRESS_V01)
        {
          reset_exec_overview = TRUE;
          op_mask = QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH|QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE;
        }
    }

    QCRIL_LOG_INFO( "AuthorizationResult_valid = %d, AuthorizationResult = %d",
            notif_ind_msg->AuthorizationResult_valid,
            notif_ind_msg->AuthorizationResult_valid ? notif_ind_msg->AuthorizationResult: -1);

    if (notif_ind_msg->OsAppId_valid &&
            notif_ind_msg->AuthorizationResult_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_authorization_result(
                notif_ind_msg->OsAppId,
                notif_ind_msg->AuthorizationResult);

        if (notif_ind_msg->AuthorizationResult != QMI_LTE_DISC_SUCCESS_V01 ||
            notif_ind_msg->AuthorizationResult != QMI_LTE_DISC_IN_PROGRESS_V01)
        {
          reset_exec_overview = TRUE;
          op_mask = QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH|QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE;
        }
    }

    QCRIL_LOG_INFO( "PublishStatusPerExpr_valid = %d, PublishStatusPerExpr = %d",
            notif_ind_msg->PublishStatusPerExpr_valid,
            notif_ind_msg->PublishStatusPerExpr_valid ? notif_ind_msg->PublishStatusPerExpr: -1);

    if (notif_ind_msg->OsAppId_valid &&
            notif_ind_msg->Expression_valid &&
            notif_ind_msg->PublishStatusPerExpr_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_transmission_status(
                notif_ind_msg->OsAppId,
                &(notif_ind_msg->Expression),
                notif_ind_msg->PublishStatusPerExpr);

        if (notif_ind_msg->PublishStatusPerExpr == 0x00)
        {
          reset_exec_overview = TRUE;
          op_mask |= QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH;
        }
    }

    QCRIL_LOG_INFO( "SubscribeStatusPerExpr_valid = %d, SubscribeStatusPerExpr = %d",
            notif_ind_msg->SubscribeStatusPerExpr_valid,
            notif_ind_msg->SubscribeStatusPerExpr_valid ?
                    notif_ind_msg->SubscribeStatusPerExpr :
                    -1);

    if (notif_ind_msg->OsAppId_valid &&
            notif_ind_msg->Expression_valid &&
            notif_ind_msg->SubscribeStatusPerExpr_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_reception_status(
                notif_ind_msg->OsAppId,
                &(notif_ind_msg->Expression),
                notif_ind_msg->SubscribeStatusPerExpr);

        if (notif_ind_msg->SubscribeStatusPerExpr == 0x00)
        {
          reset_exec_overview = TRUE;
          op_mask |= QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE;
        }
    }

    if (reset_exec_overview)
    {
        qcril_qmi_lte_direct_disc_remove_from_exec_overview(
            notif_ind_msg->OsAppId, notif_ind_msg->Expression.prose_app_id_name,
            op_mask);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_lte_direct_disc_notification_ind_hdlr

/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_broadcast_notification_ind_hdlr

===========================================================================*/
/*!
  @brief
  Handle QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND and send
  UNSOL_RESPONSE_SERVICE_STATUS indication to upper layer
  if the publish allowed / subscribe allowed is present in
  the QMI LTE indication.

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_broadcast_notification_ind_hdlr
(
  void *ind_data_ptr
)
{
  qmi_lte_disc_broadcast_notification_ind_msg_v01 *notif_ind_msg =
        (qmi_lte_disc_broadcast_notification_ind_msg_v01*) ind_data_ptr;

  QCRIL_LOG_FUNC_ENTRY();

  if (notif_ind_msg != NULL)
  {
    QCRIL_LOG_INFO( "PublishAllowed_valid = %d, SubscribeAllowed_valid = %d",
            notif_ind_msg->PublishAllowed_valid, notif_ind_msg->SubscribeAllowed_valid);
    if (notif_ind_msg->PublishAllowed_valid ||
            notif_ind_msg->SubscribeAllowed_valid)
    {
        qcril_qmi_lte_direct_disc_send_unsol_service_status(
                notif_ind_msg->PublishAllowed_valid,
                notif_ind_msg->PublishAllowed,
                notif_ind_msg->SubscribeAllowed_valid,
                notif_ind_msg->SubscribeAllowed);
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }
  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_lte_direct_disc_broadcast_notification_ind_hdlr

/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_match_notification_ind_hdlr

===========================================================================*/
/*!
  @brief
  Handle QMI_LTE_DISC_MATCH_NOTIFICATION_IND and send
  UNSOL_RESPONSE_MATCH_EVENT indication to upper layer.

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_match_notification_ind_hdlr
(
  void *ind_data_ptr
)
{
  qmi_lte_disc_match_notification_ind_msg_v01 *notif_ind_msg =
        (qmi_lte_disc_match_notification_ind_msg_v01*) ind_data_ptr;
  LteDirectDiscovery_MatchEvent match;
  boolean send_unsol = FALSE;
  int     len        = 0;

  QCRIL_LOG_FUNC_ENTRY();

  memset (&match, 0x00, sizeof (LteDirectDiscovery_MatchEvent));

  if (notif_ind_msg != NULL)
  {
    do {
      QCRIL_LOG_INFO("OsAppId_valid = %d, OsAppId = %s",
              notif_ind_msg->OsAppId_valid,
              notif_ind_msg->OsAppId_valid ? notif_ind_msg->OsAppId : "<not valid>");

      len = notif_ind_msg->OsAppId_valid ? strlen(notif_ind_msg->OsAppId) : 0;
      if (len > 0)
      {
        match.osAppId.arg = qcril_malloc(len + 1);
        if (match.osAppId.arg)
        {
          memcpy(match.osAppId.arg, notif_ind_msg->OsAppId, len);
        }
        else
        {
          QCRIL_LOG_ERROR("malloc failed");
          break;
        }
      }
      else
      {
        QCRIL_LOG_ERROR("Empty OS App ID; len = %d", len);
        break;
      }

      QCRIL_LOG_INFO("Expression_valid = %d, Expression = %s",
              notif_ind_msg->Expression_valid,
              notif_ind_msg->Expression_valid ?
                  notif_ind_msg->Expression.prose_app_id_name :
                  "<not valid>");

      len = notif_ind_msg->Expression_valid ?
              strlen(notif_ind_msg->Expression.prose_app_id_name) : 0;
      if (len > 0)
      {
        match.expression.arg = qcril_malloc(len + 1);
        if (match.expression.arg)
        {
          memcpy(match.expression.arg, notif_ind_msg->Expression.prose_app_id_name, len);
        }
        else
        {
          QCRIL_LOG_ERROR("malloc failed");
          break;
        }
      }
      else
      {
        QCRIL_LOG_ERROR("Empty Expression; len = %d", len);
        break;
      }

      QCRIL_LOG_INFO("MatchedExpression_valid = %d, MatchedExpression = %s",
              notif_ind_msg->MatchedExpression_valid,
              notif_ind_msg->MatchedExpression_valid ?
                  notif_ind_msg->MatchedExpression.prose_app_id_name :
                  "<not valid>");

      len = notif_ind_msg->MatchedExpression_valid ?
              strlen(notif_ind_msg->MatchedExpression.prose_app_id_name) : 0;
      if (len > 0)
      {
        match.matchedExpression.arg = qcril_malloc(len + 1);
        if (match.matchedExpression.arg)
        {
          memcpy(match.matchedExpression.arg,
                  notif_ind_msg->MatchedExpression.prose_app_id_name, len);
        }
        else
        {
          QCRIL_LOG_ERROR("malloc failed");
          break;
        }
      }
      else
      {
        QCRIL_LOG_ERROR("Empty MatchedExpression; len = %d", len);
        break;
      }

      QCRIL_LOG_INFO("MatchEventState_valid = %d, MatchEventState = %d",
              notif_ind_msg->MatchEventState_valid,
              notif_ind_msg->MatchEventState_valid ? notif_ind_msg->MatchEventState_valid : -1);
      if (notif_ind_msg->MatchEventState_valid)
      {
        match.has_state = TRUE;
        if (notif_ind_msg->MatchEventState == 0x00)
        {
          match.state = 0; // Match start
        }
        else
        {
          match.state = 1; // Match end
        }
      }

      QCRIL_LOG_INFO("MetaDataIndex_valid = %d, MetaDataIndex = %d",
              notif_ind_msg->MetaDataIndex_valid,
              notif_ind_msg->MetaDataIndex_valid ? notif_ind_msg->MetaDataIndex : -1);

      if (notif_ind_msg->MetaDataIndex_valid)
      {
        match.has_metaDataIndex = TRUE;
        match.metaDataIndex     = notif_ind_msg->MetaDataIndex;
      }

      len = notif_ind_msg->MetaData_valid ? strlen(notif_ind_msg->MetaData) : 0;
      if (len > 0)
      {
        match.metaData.arg = qcril_malloc(len + 2);
        if (match.metaData.arg)
        {
          memcpy(match.metaData.arg, notif_ind_msg->MetaData, len);
        }
        else
        {
          QCRIL_LOG_ERROR("malloc failed");
          break;
        }
      }
      else
      {
        QCRIL_LOG_ERROR("Empty MetaData; len = %d", len);
      }

      // Good to go; send UNSOL_RESPONSE_MATCH_EVENT
      send_unsol = TRUE;
    } while (FALSE);

    if (send_unsol)
    {
      qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
              LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
              LteDirectDiscovery_MsgId_UNSOL_RESPONSE_MATCH_EVENT,
              LteDirectDiscovery_Error_E_SUCCESS,
              &match,
              sizeof(match));
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  // free match;
  if (match.osAppId.arg)
  {
    qcril_free(match.osAppId.arg);
  }
  if (match.expression.arg)
  {
    qcril_free(match.expression.arg);
  }
  if (match.matchedExpression.arg)
  {
    qcril_free(match.matchedExpression.arg);
  }
  if (match.metaData.arg)
  {
    qcril_free(match.metaData.arg);
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_lte_direct_disc_match_notification_ind_hdlr

/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr

===========================================================================*/
/*!
  @brief
  Handle QMI_LTE_SUBSCRIPTION_INFO_IND and send
  UNSOL_RESPONSE_DEVICE_CAPABILITY_CHANGED indication if capability
  is present in the QMI LTE indication.

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr
(
  void *ind_data_ptr
)
{
  qmi_lte_subscription_info_ind_msg_v01 *notif_ind_msg =
        (qmi_lte_subscription_info_ind_msg_v01*) ind_data_ptr;
  LteDirectDiscovery_DeviceCapability device_capability;
  boolean send_unsol = FALSE;

  QCRIL_LOG_FUNC_ENTRY();

  if (notif_ind_msg != NULL)
  {
    memset (&device_capability, 0x00, sizeof (LteDirectDiscovery_DeviceCapability));
    do {
      QCRIL_LOG_INFO("LteDiscCapability_valid = %d, LteDiscCapability = %d",
              notif_ind_msg->LteDiscCapability_valid,
              notif_ind_msg->LteDiscCapability);

      if (notif_ind_msg->LteDiscCapability_valid)
      {
        device_capability.has_capability = TRUE;
        device_capability.capability     = notif_ind_msg->LteDiscCapability;
      }
      else
      {
        QCRIL_LOG_ERROR("LteDiscCapability not valid");
        break;
      }

      // Good to go; send UNSOL_RESPONSE_DEVICE_CAPABILITY_CHANGED
      send_unsol = TRUE;
    } while (FALSE);

    if (send_unsol)
    {
      qcril_qmi_oem_socket_lte_direct_disc_send_unsol(
              LteDirectDiscovery_MsgType_UNSOL_RESPONSE,
              LteDirectDiscovery_MsgId_UNSOL_RESPONSE_DEVICE_CAPABILITY_CHANGED,
              LteDirectDiscovery_Error_E_SUCCESS,
              &device_capability,
              sizeof(device_capability));
    }
  }
  else
  {
    QCRIL_LOG_ERROR("ind_data_ptr is NULL");
  }

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr

/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_psk_expired_ind_hdlr

===========================================================================*/
/*!
  @brief
  Handle QMI_LTE_DISC_PSK_EXPIRED_IND_V01 and send
  UNSOL_RESPONSE_PSK_EXPIRED indication

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_psk_expired_ind_hdlr
(
  void *ind_data_ptr
)
{
  QCRIL_NOTUSED(ind_data_ptr);

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO("send RADIO_CONFIG_UNSOL_PSK_EXPIRED");

  qcril_qmi_radio_config_socket_send(
          0,
          com_qualcomm_qti_radioconfiginterface_MessageType_RADIO_CONFIG_MSG_INDICATION,
          com_qualcomm_qti_radioconfiginterface_MessageId_RADIO_CONFIG_UNSOL_PSK_EXPIRED,
          0, 0,
          NULL, 0);

  QCRIL_LOG_FUNC_RETURN();
} // qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr

/*===========================================================================

FUNCTION:  qcril_qmi_lte_direct_disc_lookup_command_name

===========================================================================*/
/*!
  @brief
  Lookup state name.

  @return
  A pointer to the state name.
 */
/*=========================================================================*/
static char *qcril_qmi_lte_direct_disc_lookup_command_name
(
  unsigned long msg
)
{
  switch( msg )
  {
    // Responses
    case QMI_LTE_INDICATION_REGISTER_RESP_V01:
      return "INDICATION REGISTER RESP";
    case QMI_LTE_DISC_SET_LTED_CONFIG_RESP_V01:
      return "SET LTED CONFIG RESP";
    case QMI_LTE_DISC_GET_LTED_CONFIG_RESP_V01:
      return "GET LTED CONFIG RESP";
    case QMI_LTE_DISC_SET_LTED_CATEGORY_RESP_V01:
      return "SET LTED CATEGORY RESP";
    case QMI_LTE_DISC_GET_CATEGORY_RESP_V01:
      return "GET CATEGORY RESP";
    case QMI_LTE_DISC_TERMINATE_RESP_V01:
      return "TERMINATE RESP";
    case QMI_LTE_DISC_GET_SERVICE_STATUS_RESP_V01:
      return "GET SERVICE STATUS RESP";
    case QMI_LTE_DISC_PUBLISH_RESP_V01:
      return "PUBLISH RESP";
    case QMI_LTE_DISC_SUBSCRIBE_RESP_V01:
      return "SUBSCRIBE RESP";
    case QMI_LTE_DISC_CANCEL_PUBLISH_RESP_V01:
      return "CANCEL PUBLISH RESP";
    case QMI_LTE_DISC_CANCEL_SUBSCRIBE_RESP_V01:
      return "CANCEL SUBSCRIBE RESP";
    case QMI_LTE_BIND_SUBSCRIPTION_RESP_V01:
      return "BIND SUBSCRIPTION RESP";
    case QMI_LTE_GET_SUBSCRIPTION_INFO_RESP_V01:
      return "GET SUBSCRIPTION RESP";
    // Indications
    case QMI_LTE_DISC_NOTIFICATION_IND_V01:
      return "NOTIFICATION IND";
    case QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_V01:
      return "BROADCAST NOTIFICATION IND";
    case QMI_LTE_DISC_MATCH_NOTIFICATION_IND_V01:
      return "MATCH NOTIFICATION IND";
    case QMI_LTE_SUBSCRIPTION_INFO_IND_V01:
      return "SUBSCRIPTION INFO IND";
  } /* end switch */
  return "Unknown";
} /* qcril_qmi_lte_direct_disc_lookup_command_name */

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_command_cb

===========================================================================*/
/*!
  @brief
  Common Callback for all the QMI LTE commands.

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_command_cb
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

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_LOG_INFO("msg_id 0x%.2x (%s)", msg_id,
        qcril_qmi_lte_direct_disc_lookup_command_name(msg_id));

  qmi_resp_callback.user_handle = user_handle;
  qmi_resp_callback.msg_id = msg_id;
  qmi_resp_callback.data_buf = (void*) resp_c_struct;
  qmi_resp_callback.data_buf_len = resp_c_struct_len;
  qmi_resp_callback.cb_data = resp_cb_data;
  qmi_resp_callback.transp_err = transp_err;

  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
          QCRIL_DEFAULT_MODEM_ID,
          QCRIL_DATA_ON_STACK,
          QCRIL_EVT_QMI_LTE_HANDLE_COMM_CALLBACKS,
          (void*) &qmi_resp_callback,
          sizeof(qmi_resp_callback),
          (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_lte_direct_disc_command_cb */

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_command_cb_helper

===========================================================================*/
/*!
  @brief
  Handle QCRIL_EVT_QMI_LTE_HANDLE_COMM_CALLBACKS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_command_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  qcril_instance_id_e_type   instance_id;
  uint32                     user_data;
  uint16                     req_id;
  qcril_reqlist_public_type  req_info;
  qcril_request_params_type  req_data;
  qmi_resp_callback_type    *qmi_resp_callback;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  qmi_resp_callback = (qmi_resp_callback_type *) params_ptr->data;
  if(qmi_resp_callback)
  {
    /*-----------------------------------------------------------------------*/
    QCRIL_ASSERT( qmi_resp_callback->data_buf != NULL );
    user_data   = ( uint32 )(uintptr_t) qmi_resp_callback->cb_data;
    instance_id = QCRIL_EXTRACT_INSTANCE_ID_FROM_USER_DATA( user_data );
    req_id      = QCRIL_EXTRACT_USER_ID_FROM_USER_DATA( user_data );
    req_data.modem_id    = QCRIL_DEFAULT_MODEM_ID;
    req_data.instance_id = instance_id;
    req_data.datalen     = qmi_resp_callback->data_buf_len;
    req_data.data        = qmi_resp_callback->data_buf;
    /*-----------------------------------------------------------------------*/

    QCRIL_LOG_INFO("msg_id 0x%.2x (%s)", qmi_resp_callback->msg_id,
        qcril_qmi_lte_direct_disc_lookup_command_name(qmi_resp_callback->msg_id));

    /* Lookup the Token ID */
    if (qcril_reqlist_query_by_req_id(req_id, &instance_id, &req_info) == E_SUCCESS)
    {
      if(qmi_resp_callback->transp_err != QMI_NO_ERR )
      {
        QCRIL_LOG_INFO("Transp error (%d) recieved from QMI for RIL request %d",
            qmi_resp_callback->transp_err, req_info.request);
        /* Send GENERIC_FAILURE response */
        qcril_qmi_oem_socket_lte_direct_disc_send(req_info.t,
                LteDirectDiscovery_MsgType_RESPONSE,
                qcril_qmi_lte_direct_disc_map_event_to_request(req_info.request),
                LteDirectDiscovery_Error_E_GENERIC_FAILURE,
                NULL, 0);
      }
      else
      {
        req_data.t = req_info.t;
        req_data.event_id = req_info.request;
        switch(qmi_resp_callback->msg_id)
        {
          case QMI_LTE_DISC_TERMINATE_RESP_V01:
            qcril_qmi_lte_direct_disc_terminate_resp_hdlr(&req_data);
            break;
          case QMI_LTE_DISC_GET_SERVICE_STATUS_RESP_V01:
            qcril_qmi_lte_direct_disc_get_service_status_resp_hdlr(&req_data);
            break;
          case QMI_LTE_DISC_PUBLISH_RESP_V01:
            qcril_qmi_lte_direct_disc_publish_resp_hdlr(&req_data);
            break;
          case QMI_LTE_DISC_SUBSCRIBE_RESP_V01:
            qcril_qmi_lte_direct_disc_subscribe_resp_hdlr(&req_data);
            break;
          case QMI_LTE_DISC_CANCEL_PUBLISH_RESP_V01:
            qcril_qmi_lte_direct_disc_cancel_publish_resp_hdlr(&req_data);
            break;
          case QMI_LTE_DISC_CANCEL_SUBSCRIBE_RESP_V01:
            qcril_qmi_lte_direct_disc_cancel_subscribe_resp_hdlr(&req_data);
            break;
          case QMI_LTE_GET_SUBSCRIPTION_INFO_RESP_V01:
            qcril_qmi_lte_direct_disc_get_subscription_info_resp_hdlr(&req_data);
            break;
          default:
            QCRIL_LOG_INFO("Unsupported QMI LTE message %d", qmi_resp_callback->msg_id);
            break;
        }
      }
    }
    else
    {
      QCRIL_LOG_ERROR("Req ID: %d not found", req_id);
    }

    qcril_free( qmi_resp_callback->data_buf );
  }

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_lte_direct_disc_command_cb_helper */

/*=========================================================================
  FUNCTION:  qcril_qmi_lte_direct_disc_unsol_ind_cb

===========================================================================*/
/*!
    @brief
    Callback for QMI LTE indications

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
  qmi_ind_callback_type qmi_callback;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_callback.data_buf = qcril_malloc(ind_buf_len);

  QCRIL_LOG_INFO("msg_id 0x%.2x (%s)", msg_id,
        qcril_qmi_lte_direct_disc_lookup_command_name(msg_id));


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
                   QCRIL_EVT_QMI_LTE_HANDLE_INDICATIONS,
                   (void*) &qmi_callback,
                   sizeof(qmi_callback),
                   (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }
  else
  {
    QCRIL_LOG_FATAL("malloc failed");
  }

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_lte_direct_disc_unsol_ind_cb */

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_unsol_ind_cb_helper

===========================================================================*/
/*!
  @brief
  Handle QCRIL_EVT_QMI_LTE_HANDLE_INDICATIONS

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_unsol_ind_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr // Output parameter
)
{
  qmi_ind_callback_type *qmi_callback       = (qmi_ind_callback_type*) params_ptr->data;
  qmi_client_error_type qmi_err             = QMI_NO_ERR;
  void                 *decoded_payload     = NULL;
  uint32_t              decoded_payload_len = 0;

  QCRIL_LOG_FUNC_ENTRY();
  QCRIL_NOTUSED(ret_ptr);

  if(qmi_callback)
  {
    qmi_idl_get_message_c_struct_len(
            qcril_qmi_client_get_service_object(QCRIL_QMI_CLIENT_LTE),
            QMI_IDL_INDICATION,
            qmi_callback->msg_id,
            &decoded_payload_len);

    if(decoded_payload_len)
    {
      decoded_payload = qcril_malloc(decoded_payload_len);
    }

    if (decoded_payload || !decoded_payload_len)
    {
      if(decoded_payload_len)
      {
        qmi_err = qmi_client_message_decode(
                qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_LTE),
                QMI_IDL_INDICATION,
                qmi_callback->msg_id,
                qmi_callback->data_buf,
                qmi_callback->data_buf_len,
                decoded_payload,
                (int)decoded_payload_len);
      }

      QCRIL_LOG_INFO("msg_id %.2x (%s)", qmi_callback->msg_id,
        qcril_qmi_lte_direct_disc_lookup_command_name(qmi_callback->msg_id));
      QCRIL_LOG_INFO(".. operational state %d", (int) qmi_ril_get_operational_status());

      if (qmi_err == QMI_NO_ERR)
      {
        switch(qmi_callback->msg_id)
        {
          case QMI_LTE_DISC_NOTIFICATION_IND_V01:
            qcril_qmi_lte_direct_disc_notification_ind_hdlr(decoded_payload);
            break;
          case QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_V01:
            qcril_qmi_lte_direct_disc_broadcast_notification_ind_hdlr(decoded_payload);
            break;
          case QMI_LTE_DISC_MATCH_NOTIFICATION_IND_V01:
            qcril_qmi_lte_direct_disc_match_notification_ind_hdlr(decoded_payload);
            break;
          case QMI_LTE_SUBSCRIPTION_INFO_IND_V01:
            qcril_qmi_lte_direct_disc_subscription_info_ind_hdlr(decoded_payload);
            break;
          case QMI_LTE_DISC_PSK_EXPIRED_IND_V01:
            qcril_qmi_lte_direct_disc_psk_expired_ind_hdlr(decoded_payload);
            break;
          default:
            QCRIL_LOG_INFO("Unknown QMI LTE indication %d", qmi_callback->msg_id);
            break;
        }
      }
      else
      {
        QCRIL_LOG_INFO("Indication decode failed for msg %d of svc %d with error %d",
            qmi_callback->msg_id, QCRIL_QMI_CLIENT_LTE, qmi_err);
      }

      if(decoded_payload_len)
      {
        qcril_free(decoded_payload);
      }
    }

    if(qmi_callback->data_buf)
    {
      qcril_free(qmi_callback->data_buf);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_lte_direct_disc_unsol_ind_cb_helper */

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_ind_registrations

===========================================================================*/
/*!
  @brief
  Register for the QMI LTE indications

  @return
  None.
 */
/*=========================================================================*/
void qcril_qmi_lte_direct_disc_ind_registrations
(
  void
)
{
  qmi_lte_indication_reg_req_msg_v01  indication_req;
  qmi_lte_indication_reg_resp_msg_v01 indication_resp_msg;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp_msg, 0, sizeof(indication_resp_msg));

  indication_req.indication_bitmask = (QMI_LTE_DISC_NOTIFICATION_IND_MASK_V01 |
                                       QMI_LTE_DISC_BROADCAST_NOTIFICATION_IND_MASK_V01 |
                                       QMI_LTE_DISC_MATCH_NOTIFICATION_IND_MASK_V01 |
                                       QMI_LTE_DISC_PSK_EXPIRED_IND_MASK_V01);

  if (qcril_qmi_client_send_msg_sync (
              QCRIL_QMI_CLIENT_LTE,
              QMI_LTE_INDICATION_REGISTER_REQ_V01,
              &indication_req,
              sizeof(indication_req),
              &indication_resp_msg,
              sizeof(indication_resp_msg)) !=E_SUCCESS )
  {
    QCRIL_LOG_INFO("QMI_LTE_INDICATION_REGISTER failed");
  }
  else
  {
    QCRIL_LOG_INFO("QMI_LTE_INDICATION_REGISTER error code: %d", indication_resp_msg.resp.error);
  }
  QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
FUNCTION: qcril_qmi_lte_direct_init

===========================================================================*/
void qcril_qmi_lte_direct_init
(
  void
)
{
  qcril_qmi_lte_direct_disc_event_map_init();

  qcril_qmi_lted_init_msg_meta();

  qcril_qmi_lte_direct_disc_init();
}

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_init

===========================================================================*/
/*!
  @brief
  Initialize the lte direct discovery client

  @return
  Success or failure
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_init
(
  void
)
{
  RIL_Errno res = RIL_E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&lte_direct_disc_overview, 0, sizeof(lte_direct_disc_overview));

  qcril_qmi_lte_direct_disc_ind_registrations();

  QCRIL_LOG_FUNC_RETURN_WITH_RET( (int)res );

  return res;
}

/*=========================================================================
FUNCTION:  qcril_qmi_lte_direct_disc_on_radio_state_change

===========================================================================*/
/*!
  @brief
  Notify the service status as not Allowed in case if the device
  is not in online mode.

  @return
  Success or failure
 */
/*=========================================================================*/
RIL_Errno qcril_qmi_lte_direct_disc_on_radio_state_change
(
  void
)
{
  RIL_Errno res = RIL_E_SUCCESS;

  QCRIL_LOG_FUNC_ENTRY();

  boolean is_in_online_mode = qcril_qmi_nas_dms_is_in_online_mode();

  QCRIL_LOG_INFO("is_in_online_mode = %d", is_in_online_mode);

  if (!is_in_online_mode)
  {
    qcril_qmi_lte_direct_disc_send_unsol_service_status(
            TRUE, FALSE,
            TRUE, FALSE);
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET( (int)res );
  return res;
}


static qcril_qmi_lte_direct_disc_exec_overview_type *
qcril_qmi_lte_direct_disc_add_to_exec_overview
(
 const char *os_app_id,
 const char *exp,
 uint32_t    op
)
{
  qcril_qmi_lte_direct_disc_exec_overview_type *entry = NULL;
  qcril_qmi_lte_direct_disc_exec_overview_type *iter  = NULL;
  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_iter  = NULL;
  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_entry = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  if (os_app_id && exp)
  {
    QCRIL_LOG_INFO("Add Os App Id: %s, expression: %s", os_app_id, exp);
    QCRIL_LOG_INFO("lte_direct_disc_overview.exec_overview_root = 0x%x",
            lte_direct_disc_overview.exec_overview_root);
    iter = lte_direct_disc_overview.exec_overview_root;
    while (iter)
    {
      QCRIL_LOG_INFO("iter Os App Id: %s", iter->os_app_id);
      if (strcmp(iter->os_app_id, os_app_id) == 0)
      {
        QCRIL_LOG_INFO("Found Os App Id match");
        entry = iter;
        break;
      }
      iter = iter->next;
    }
    QCRIL_LOG_INFO("entry = 0x%x", entry);

    if (entry)
    {
      if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH)
      {
        QCRIL_LOG_INFO("Look for publish expression");
        exp_iter = entry->publish_list;
        while (exp_iter)
        {
          QCRIL_LOG_INFO("exp_iter expression: %s", exp_iter->expression);
          if (strcmp(exp_iter->expression, exp) == 0)
          {
            QCRIL_LOG_INFO("Found publish expression match");
            exp_entry = exp_iter;
            break;
          }
          exp_iter = exp_iter->next;
        }
        if (!exp_entry)
        {
          QCRIL_LOG_INFO("Create new publish expression and add to list");
          exp_entry = (qcril_qmi_lte_direct_disc_overview_exp_type *)
                qcril_malloc(sizeof(qcril_qmi_lte_direct_disc_overview_exp_type));
          exp_entry->expression = qmi_ril_util_str_clone((char *)exp);
          exp_entry->next = entry->publish_list;
          entry->publish_list = exp_entry;
        }
        else
        {
          QCRIL_LOG_INFO("expression is already present in the publish list");
        }
      }
      if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE)
      {
        QCRIL_LOG_INFO("Look for subscribe expression");
        exp_iter = entry->subscribe_list;
        while (exp_iter)
        {
          QCRIL_LOG_INFO("exp_iter expression: %s", exp_iter->expression);
          if (strcmp(exp_iter->expression, exp) == 0)
          {
            QCRIL_LOG_INFO("Found subscribe expression match");
            exp_entry = exp_iter;
            break;
          }
          exp_iter = exp_iter->next;
        }
        if (!exp_entry)
        {
          QCRIL_LOG_INFO("Create new subscribe expression and add to list");
          exp_entry = (qcril_qmi_lte_direct_disc_overview_exp_type *)
                qcril_malloc(sizeof(qcril_qmi_lte_direct_disc_overview_exp_type));
          exp_entry->expression = qmi_ril_util_str_clone((char *)exp);
          exp_entry->next = entry->subscribe_list;
          entry->subscribe_list = exp_entry;
        }
        else
        {
          QCRIL_LOG_INFO("expression is already present in the subscribe list");
        }
      }
    }
    else
    {
      QCRIL_LOG_INFO("Add new entry for Os App Id: %s", os_app_id);

      // Create expression list entry (publish/suscribe)
      exp_entry = (qcril_qmi_lte_direct_disc_overview_exp_type *)
        qcril_malloc(sizeof(qcril_qmi_lte_direct_disc_overview_exp_type));
      exp_entry->expression = qmi_ril_util_str_clone((char *)exp);
      exp_entry->next = NULL;

      // Create os_app_id entry
      entry = qcril_malloc(sizeof(qcril_qmi_lte_direct_disc_exec_overview_type));
      entry->os_app_id = qmi_ril_util_str_clone((char *)os_app_id);
      if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH)
      {
        entry->publish_list = exp_entry;
      }
      else if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE)
      {
        entry->subscribe_list = exp_entry;
      }
      entry->next = lte_direct_disc_overview.exec_overview_root;
      lte_direct_disc_overview.exec_overview_root = entry;
    }
  }

  QCRIL_LOG_FUNC_RETURN();

  return entry;
}

static void
qcril_qmi_lte_direct_disc_remove_from_exec_overview
(
 const char *os_app_id,
 const char *exp,
 uint32_t    op
)
{
  qcril_qmi_lte_direct_disc_exec_overview_type *overview_prev = NULL;
  qcril_qmi_lte_direct_disc_exec_overview_type *overview_iter  = NULL;

  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_iter  = NULL;
  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_prev  = NULL;
  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_entry = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  if (os_app_id != NULL)
  {
    QCRIL_LOG_INFO("Remove req for Os App Id: %s, expression: %s", os_app_id, exp);
    overview_prev = NULL;
    overview_iter = lte_direct_disc_overview.exec_overview_root;
    while (overview_iter)
    {
      if (strcmp(overview_iter->os_app_id, os_app_id) == 0)
      {
        QCRIL_LOG_INFO("Found Os App Id match");
        break;
      }
      overview_prev = overview_iter;
      overview_iter = overview_iter->next;
    }
  }

  if (overview_iter)
  {
    if (exp)
    {
      if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH)
      {
        QCRIL_LOG_INFO("Look for publish expression");
        exp_iter = overview_iter->publish_list;
        exp_prev = NULL;
        while (exp_iter)
        {
          if (strcmp(exp_iter->expression, exp) == 0)
          {
            QCRIL_LOG_INFO("Found publish expression match");
            break;
          }
          exp_prev = exp_iter;
          exp_iter = exp_iter->next;
        }
        if (exp_iter)
        {
          QCRIL_LOG_INFO("Remove matched publish expression : 0x%x", exp_iter);
          if (exp_prev == NULL)
          {
            overview_iter->publish_list = exp_iter->next;
          }
          else
          {
            exp_prev->next = exp_iter->next;
          }
          qcril_free(exp_iter->expression);
          qcril_free(exp_iter);
        }
      }
      if (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE)
      {
        QCRIL_LOG_INFO("Look for subscribe expression");
        exp_iter = overview_iter->subscribe_list;
        exp_prev = NULL;
        while (exp_iter)
        {
          if (strcmp(exp_iter->expression, exp) == 0)
          {
            QCRIL_LOG_INFO("Found subscribe expression match");
            break;
          }
          exp_prev = exp_iter;
          exp_iter = exp_iter->next;
        }
        if (exp_iter)
        {
          QCRIL_LOG_INFO("Remove matched subscribe expression : 0x%x", exp_iter);
          if (exp_prev == NULL)
          {
            overview_iter->subscribe_list = exp_iter->next;
          }
          else
          {
            exp_prev->next = exp_iter->next;
          }
          qcril_free(exp_iter->expression);
          qcril_free(exp_iter);
        }
      }
    }
    else
    {
      QCRIL_LOG_INFO("Remove all expression matching OP: 0x%x", op);
      // Delete all publish/subscribe list
      while (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_PUBLISH &&
          overview_iter->publish_list)
      {
        exp_entry = overview_iter->publish_list;
        overview_iter->publish_list = exp_entry->next;
        qcril_free(exp_entry->expression);
        qcril_free(exp_entry);
      }
      while (op & QCRIL_QMI_LTE_DIRECT_DISC_OP_SUBSCRIBE &&
          overview_iter->subscribe_list)
      {
        exp_entry = overview_iter->subscribe_list;
        overview_iter->subscribe_list = exp_entry->next;
        qcril_free(exp_entry->expression);
        qcril_free(exp_entry);
      }
    }

    if (overview_iter->publish_list == NULL &&
        overview_iter->subscribe_list == NULL)
    {
      QCRIL_LOG_INFO("No publish/subscribe list, remove overview entry");
      if (overview_prev == NULL)
      {
        lte_direct_disc_overview.exec_overview_root = overview_iter->next;
      }
      else
      {
        overview_prev->next = overview_iter->next;
      }
      qcril_free(overview_iter->os_app_id);
      qcril_free(overview_iter);
    }
  }

  QCRIL_LOG_FUNC_RETURN();
}

static void
qcril_qmi_lte_direct_disc_terminate_all_apps()
{
  qcril_qmi_lte_direct_disc_exec_overview_type *entry = NULL;
  qcril_qmi_lte_direct_disc_exec_overview_type *iter  = NULL;
  qcril_qmi_lte_direct_disc_overview_exp_type  *exp_entry = NULL;

  qmi_lte_disc_terminate_req_msg_v01   req;
  qmi_lte_disc_terminate_resp_msg_v01 *resp = NULL;

  QCRIL_LOG_FUNC_ENTRY();

  entry = NULL;
  iter = lte_direct_disc_overview.exec_overview_root;
  while (iter)
  {
    entry = iter;
    iter = iter->next;

    memset(&req, 0x00, sizeof(req));
    memcpy(&req.OsAppId, entry->os_app_id, strlen(entry->os_app_id));

    resp = qcril_malloc(sizeof(qmi_lte_disc_terminate_resp_msg_v01));
    if (resp)
    {
      QCRIL_LOG_INFO("Terminate Os App Id: %s", entry->os_app_id);

      qcril_qmi_client_send_msg_async_ex (
          QCRIL_QMI_CLIENT_LTE,
          QMI_LTE_DISC_TERMINATE_REQ_V01,
          &req,
          sizeof(req),
          resp,
          sizeof(*resp),
          NULL);
    }

    while (entry->publish_list)
    {
      exp_entry = entry->publish_list;
      entry->publish_list = exp_entry->next;
      qcril_free(exp_entry->expression);
      qcril_free(exp_entry);
    }
    while (entry->subscribe_list)
    {
      exp_entry = entry->subscribe_list;
      entry->subscribe_list = exp_entry->next;
      qcril_free(exp_entry->expression);
      qcril_free(exp_entry);
    }
    qcril_free(entry->os_app_id);
    qcril_free(entry);
  }

  lte_direct_disc_overview.exec_overview_root = NULL;

  QCRIL_LOG_FUNC_RETURN();
}
