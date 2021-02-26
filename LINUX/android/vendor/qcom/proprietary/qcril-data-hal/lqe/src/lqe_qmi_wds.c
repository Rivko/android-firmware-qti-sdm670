/******************************************************************************

                         L Q E _ Q M I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    lqe_qmi_wds.c
  @brief   LTE uplink/downlink quality estimation

  DESCRIPTION
  Implementation of wrapper over a bunch of QMI APIs which
  provide LTE quality estimation data to its clients.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include "wireless_data_service_v01.h"
#include "qmi_client.h"
#include "qmi_client_helper.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"
#include "ds_util.h"
#include "lqei.h"
#include "lqe_qmi_wds.h"
#include "lqe_cmdq.h"

typedef struct lqe_qmi_wds_config_s {
  boolean is_valid;
  qmi_client_type client_handle;
} lqe_qmi_wds_config_t;

lqe_qmi_wds_config_t lqe_wds_clients[LQE_CLIENT_MAX];

static qmi_client_qmux_instance_type qcci_conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_0;
static const char *default_qmi_port = QMI_PORT_RMNET_0;

static qmi_client_type        lqe_qmi_wds_notifier;
static qmi_cci_os_signal_type lqe_qmi_wds_os_params;
static qmi_cci_os_signal_type lqe_qmi_wds_notifier_os_params;
static boolean                lqe_qmi_wds_notif_cb_reg = FALSE;

#define LQE_QMI_TIMEOUT       (10000)

/*===========================================================================
  FUNCTION  lqe_qmi_get_wds_client
===========================================================================*/
/*!
@brief
  Returns a pointer to the wds client object instance
*/
/*=========================================================================*/
static inline lqe_qmi_wds_config_t* lqe_qmi_get_wds_client
(
  int qmi_client_index
)
{

  if (qmi_client_index >=0 && qmi_client_index < LQE_CLIENT_MAX)
  {
    return &lqe_wds_clients[qmi_client_index];
  }
  else
  {
    LQE_LOG_ERROR("%s(): invalid index passed", __func__);
    return NULL;
  }
}

/*===========================================================================
  FUNCTION  lqe_wds_get_handle_from_qmi_client
============================================================================*/
static inline int lqe_wds_get_handle_from_qmi_client
(
 qmi_client_type   user_handle,
 int               *client_handle
)
{
  int index = 0;

  for (index = 0; index < LQE_CLIENT_MAX; index++)
  {
    if (user_handle == lqe_wds_clients[index].client_handle &&
          lqe_wds_clients[index].is_valid)
    {
      *client_handle = index;
      return LQE_SUCCESS;
    }
  }

  LQE_LOG_INFO("%s(): Unable to find a wds client handle for [%p]",
              __func__, user_handle);
  return LQE_FAILURE;
}

/*===========================================================================
  FUNCTION:  lqe_qmi_wds_notify_cb
===========================================================================*/
/*!
@brief
  Notification callback function regisgtered during client init to be
  notified of service availability or service down events

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_qmi_wds_notify_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
)
{
  lqe_cmdq_cmd_type * cmd_buf = NULL;
  lqe_qmi_sys_event_type sys_ev;

  (void)user_handle;
  (void)service_obj;

  LQE_LOG_ENTRY;

  cmd_buf = lqe_cmdq_alloc_cmd();
  if (NULL == cmd_buf)
  {
    LQE_LOG_ERROR("%s(): cmd alloc failed\n", __func__);
    goto bail;
  }

  switch(service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      LQE_LOG_INFO("%s(): WDS service is up!\n", __func__);
      if (lqe_config.modem_ssr_state != LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND)
      {
        LQE_LOG_INFO("%s(): Not in SSR, ignoring WDS service up event!\n",
                       __func__);
        goto bail;
      }

      /* set parameters in our internal structure  */
      cmd_buf->event = LQE_CB_CMD_QMI_SYS_IND;
      cmd_buf->cmd_data.qmi_sys_ind.event_id = (int)LQE_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND;
      cmd_buf->cmd_data.qmi_sys_ind.user_data = notify_cb_data;

      /* post command to global lqe_cb queue */
      if(LQE_SUCCESS != lqe_cmdq_enqueue_cmd(cmd_buf))
      {
        LQE_LOG_ERROR("%s(): failed to put commmand\n", __func__);
        lqe_cmdq_free_cmd(cmd_buf);
      }
      break;

  case QMI_CLIENT_SERVICE_COUNT_DEC:
    break;

  default:
    LQE_LOG_ERROR("%s(): Unsupported service event=%d!\n",
                   __func__, service_event);
    break;
  }

bail:
  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION:  lqe_qmi_wds_err_cb
===========================================================================*/
/*!
@brief
  Error callback function regisgtered during client init to be notified of
  service going away on the Modem (SSR).

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_qmi_wds_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
)
{
  lqe_cmdq_cmd_type * cmd_buf = NULL;
  lqe_qmi_sys_event_type sys_ev;

  (void)user_handle;
  (void)error;

  LQE_LOG_ENTRY;

  LQE_LOG_INFO("%s(): WDS service is down!\n", __func__);

  cmd_buf = lqe_cmdq_alloc_cmd();
  if (NULL == cmd_buf)
  {
    LQE_LOG_ERROR("%s(): cmd alloc failed\n", __func__);
    goto bail;
  }

  /* set parameters in our internal structure  */
  cmd_buf->event = LQE_CB_CMD_QMI_SYS_IND;
  cmd_buf->cmd_data.qmi_sys_ind.event_id = (int)LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND;
  cmd_buf->cmd_data.qmi_sys_ind.user_data = err_cb_data;

  /* post command to global lqe_cb queue */
  if(LQE_SUCCESS != lqe_cmdq_enqueue_cmd(cmd_buf))
  {
    LQE_LOG_ERROR("%s(): failed to put commmand\n", __func__);
    lqe_cmdq_free_cmd(cmd_buf);
  }

bail:
  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION  lqe_qmi_wds_callback
===========================================================================*/
/*!
@brief
  Call back function registered for QMI WDS indications
*/
/*=========================================================================*/
static void lqe_qmi_wds_callback
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  lqe_wds_ind_event_info_t  ind_data;
  qmi_client_error_type   qmi_err;
  void *ind_data_ptr = NULL;
  int   ind_data_len = 0;
  int   ind_event;
  int   wds_client_handle;

  LQE_LOG_ENTRY;

  if (ind_buf == NULL)
  {
    LQE_LOG_ERROR("%s(): Called with null pointers!\n", __func__);
    goto bail;
  }

  (void)ind_cb_data;

  if (LQE_SUCCESS != lqe_wds_get_handle_from_qmi_client(user_handle,
                                                        &wds_client_handle))
  {
    LQE_LOG_ERROR("%s(): client handle not found\n", __func__);
    goto bail;
  }

  switch (msg_id)
  {
    case QMI_WDS_DOWNLINK_THROUGHPUT_INFO_IND_V01:
      {
        LQE_LOG_INFO("process QMI_WDS_DOWNLINK_THROUGHPUT_INFO_IND_V01");

        ind_data_ptr = &ind_data.dl_throughput_info;
        ind_data_len = sizeof(ind_data.dl_throughput_info);
        ind_event = LQE_DOWNLINK_THROUGHPUT_IND_EV;
      }
      break;

    default:
      {
        LQE_LOG_INFO("%s(): No handler for WDS IND id %d", __func__, msg_id);
        goto bail;
      }
      break;
  }

  qmi_err = qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      ind_buf,
                                      ind_buf_len,
                                      ind_data_ptr,
                                      ind_data_len);
  if(QMI_NO_ERR != qmi_err)
  {
    LQE_LOG_ERROR("%s(): Invalid filter mode ind msg error %d", __func__, qmi_err);
    goto bail;
  }

  /* Allocate a reinit module cmd */
  lqe_cmdq_cmd_type *cmd = lqe_cmdq_alloc_cmd();

  if (NULL == cmd)
  {
    LQE_LOG_ERROR("%s: cmd alloc failed\n", __func__);
    goto bail;
  }

  /* Update the reinit request */
  cmd->lqe_handle = wds_client_handle;
  cmd->event      = LQE_CB_CMD_QMI_WDS_IND;

  cmd->cmd_data.qmi_wds_ind.event_id = ind_event;
  cmd->cmd_data.qmi_wds_ind.ind_data = ind_data;

  /* Enqueue the command in the command queue */
  if (LQE_SUCCESS != lqe_cmdq_enqueue_cmd(cmd))
  {
    LQE_LOG_ERROR("%s(): cmd enq failed\n", __func__);
    lqe_cmdq_free_cmd(cmd);
    goto bail;
  }

bail:
  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION  lqe_release_wds_client
===========================================================================*/
/*!
@brief
  Release QMI handle
*/
/*=========================================================================*/
void lqe_release_wds_client
(
  int index
)
{
  int rc;
  lqe_qmi_wds_config_t *wds_client = NULL;

  LQE_LOG_ENTRY;

  LQE_LOG_DEBUG("%s(): releasing wds client with index [%d]", __func__, index);

  wds_client = lqe_qmi_get_wds_client(index);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  rc = qmi_client_release(wds_client->client_handle);

  LQE_LOG_DEBUG("%s(): qmi_client_release of qmi_clnt_hndl [%p], rc [%d]\n",
             __func__, wds_client->client_handle, rc);

  memset(wds_client, 0, sizeof(lqe_qmi_wds_config_t));

bail:
  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION  lqe_int_wds_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
int lqe_int_wds_client
(
  int index
)
{
  int client_index = 0;
  int ret = LQE_FAILURE;
  lqe_qmi_wds_config_t *wds_client = NULL;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;
  qmi_client_qmux_instance_type qcci_conn_id;

  LQE_LOG_ENTRY;

  if (index < 0 && index >= LQE_CLIENT_MAX)
  {
    LQE_LOG_ERROR("%s(): Invalid index passed\n", __func__);
    goto bail;
  }

  LQE_LOG_DEBUG("%s(): using index [%d]\n", __func__, index);

  wds_client = lqe_qmi_get_wds_client(index);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  memset(wds_client, 0, sizeof(lqe_qmi_wds_config_t));

  memset(&lqe_qmi_wds_os_params, 0, sizeof(qmi_cci_os_signal_type));

  rc = qmi_client_wds_init_instance(default_qmi_port,
                                    wds_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    lqe_qmi_wds_callback,
                                    NULL,
                                    &lqe_qmi_wds_os_params,
                                    LQE_QMI_TIMEOUT,
                                    &wds_client->client_handle);

  if (QMI_NO_ERR != rc)
  {
    LQE_LOG_ERROR("%s(): failed on qmi_client_wds_init_instance with rc=%d!\n", __func__, rc);
    wds_client = NULL;
    goto bail;
  }

  (void)qmi_client_register_error_cb(wds_client->client_handle,
                                     lqe_qmi_wds_err_cb,
                                     (void *)default_qmi_port);

  if (!lqe_qmi_wds_notif_cb_reg) {
    /* Register for notification callback */
    (void)qmi_client_notifier_init(wds_get_service_object_v01(),
                                   &lqe_qmi_wds_notifier_os_params,
                                   &lqe_qmi_wds_notifier);

    (void)qmi_client_register_notify_cb(lqe_qmi_wds_notifier,
                                        lqe_qmi_wds_notify_cb,
                                        (void *)default_qmi_port);

    lqe_qmi_wds_notif_cb_reg = TRUE;
  }

  lqe_wds_clients[index].is_valid = TRUE;
  ret = LQE_SUCCESS;
  LQE_LOG_DEBUG("%s(): qmi_client_init_instance successful\n", __func__);

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_wds_get_downlink_throughput_info_params
===========================================================================*/
int lqe_wds_get_downlink_throughput_report_status
(
  int                                      lqe_client_handle,
  lqe_downlink_throughput_report_status   *status
)
{
  int rc = 0;
  int ret = LQE_FAILURE;
  wds_query_downlink_throughput_reporting_status_req_msg_v01 req;
  wds_query_downlink_throughput_reporting_status_resp_msg_v01 resp;
  lqe_qmi_wds_config_t *wds_client = NULL;

  LQE_LOG_ENTRY;

  if (!status)
  {
    LQE_LOG_ERROR("%s(): BAD PARAMS received\n", __func__);
    goto bail;
  }

  wds_client = lqe_qmi_get_wds_client(lqe_client_handle);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(wds_client->client_handle,
                                QMI_WDS_QUERY_DOWNLINK_THROUGHPUT_REPORTING_STATUS_REQ_V01,
                                &req,
                                sizeof(req),
                                &resp,
                                sizeof(resp),
                                LQE_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    LQE_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  if (resp.actual_interval_valid)
  {
    status->actual_interval_valid = TRUE;
    status->actual_interval = resp.actual_interval;
    LQE_LOG_INFO("%s(): actual_interval [%u]", __func__, resp.actual_interval);
  }
  else
  {
    status->actual_interval_valid = FALSE;
  }

  if (resp.reporting_status_valid)
  {
    status->reporting_status_valid = TRUE;
    status->reporting_status = (lqe_thrpt_status_reason_enum)resp.reporting_status;
    LQE_LOG_INFO("%s(): reporting_status [%u]", __func__, status->reporting_status);
  }
  else
  {
    status->reporting_status_valid = FALSE;
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_wds_set_throughput_report_period
===========================================================================*/
int lqe_wds_set_throughput_report_period
(
  int                   lqe_client_handle,
  lqe_data_direction_e  direction,
  int                   report_interval_timer
)
{
  int rc = 0;
  int ret = LQE_FAILURE;
  lqe_qmi_wds_config_t *wds_client = NULL;

  LQE_LOG_ENTRY;

  wds_client = lqe_qmi_get_wds_client(lqe_client_handle);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  if (direction == LQE_DATA_DOWNLINK_DIRECTION)
  {
    wds_set_downlink_throughput_report_period_req_msg_v01 req;
    wds_set_downlink_throughput_report_period_resp_msg_v01 resp;

    memset(&req, 0, sizeof(req));
    memset(&resp, 0, sizeof(resp));

    req.downlink_throughput_report_period_valid = TRUE;
    req.downlink_throughput_report_period = report_interval_timer;

    rc = qmi_client_send_msg_sync(wds_client->client_handle,
                                QMI_WDS_SET_DOWNLINK_THROUGHPUT_REPORT_PERIOD_REQ_V01,
                                &req,
                                sizeof(req),
                                &resp,
                                sizeof(resp),
                                LQE_QMI_TIMEOUT);

    if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      if(resp.resp.result != QMI_RESULT_SUCCESS_V01 && resp.resp.error == QMI_ERR_INVALID_QMI_CMD_V01)
      {
        LQE_LOG_ERROR("%s(): LQE command is unsupported", __func__);
        ret = LQE_NOT_SUPPORTED;
      }
      else
      {
        LQE_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
      }
      goto bail;
    }

    LQE_LOG_INFO("%s(): succesfully set report_interval_timer in downlink direction : %d \n",
               __func__, report_interval_timer);
  }
  else if (direction == LQE_DATA_UPLINK_DIRECTION)
  {
    LQE_LOG_ERROR("%s(): uplink message not supported", __func__);
    ret = LQE_NOT_SUPPORTED;
    goto bail;
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_wds_toggle_throughput_indications
===========================================================================*/
int lqe_wds_toggle_throughput_indications
(
  int                         lqe_client_handle,
  lqe_data_direction_e        direction,
  lqe_indications_switch_e    ind_switch
)
{

  int rc = 0;
  int ret = LQE_FAILURE;
  lqe_qmi_wds_config_t *wds_client = NULL;

  LQE_LOG_ENTRY;

  wds_client = lqe_qmi_get_wds_client(lqe_client_handle);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  if (direction == LQE_DATA_DOWNLINK_DIRECTION)
  {
    wds_indication_register_req_msg_v01 req;
    wds_indication_register_resp_msg_v01 resp;

    memset((void *)&req, 0, sizeof(req));
    memset((void *)&resp, 0, sizeof(resp));

    if (LQE_TURN_ON_INDICATIONS == ind_switch)
    {
      req.report_dl_throughput_valid = TRUE;
      req.report_dl_throughput = 1;
      LQE_LOG_INFO("%s(): Turning on downlink throughput indications: %d \n",
                 __func__, req.report_dl_throughput);
    }
    else if ( LQE_TURN_OFF_INDICATIONS == ind_switch)
    {
      req.report_dl_throughput_valid = TRUE;
      req.report_dl_throughput = 0;
      LQE_LOG_INFO("%s(): Turning off downlink throughput indications: %d \n",
                 __func__, req.report_dl_throughput);
    }

    rc = qmi_client_send_msg_sync(wds_client->client_handle,
                                QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                &req,
                                sizeof(wds_indication_register_req_msg_v01),
                                &resp,
                                sizeof(wds_indication_register_resp_msg_v01),
                                LQE_QMI_TIMEOUT);

    if (QMI_NO_ERR != rc || resp.resp.result != QMI_RESULT_SUCCESS_V01)
    {
      LQE_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d\n",
                   __func__, rc, resp.resp.error);
      goto bail;
    }
  }
  else if (direction == LQE_DATA_UPLINK_DIRECTION)
  {
    LQE_LOG_ERROR("%s(): uplink message not supported\n", __func__);
    ret = LQE_NOT_SUPPORTED;
    goto bail;
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_wds_get_downlink_throughput_info
===========================================================================*/
int lqe_wds_get_downlink_throughput_info
(
  int         lqe_client_handle,
  int        *dl_allowed_rate,
  int        *confidence_level
)
{
  int rc = 0;
  int ret = LQE_FAILURE;
  wds_get_downlink_throughput_info_req_msg_v01 req;
  wds_get_downlink_throughput_info_resp_msg_v01 resp;
  lqe_qmi_wds_config_t *wds_client = NULL;

  LQE_LOG_ENTRY;

  if ((!dl_allowed_rate) || (!confidence_level))
  {
    LQE_LOG_ERROR("%s(): BAD PARAMS received", __func__);
    goto bail;
  }

  wds_client = lqe_qmi_get_wds_client(lqe_client_handle);

  if(!wds_client)
  {
    LQE_LOG_ERROR("%s(): invalid client\n", __func__);
    goto bail;
  }

  memset((void *)&req, 0, sizeof(req));
  memset((void *)&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(wds_client->client_handle,
                                QMI_WDS_GET_DOWNLINK_THROUGHPUT_INFO_REQ_V01,
                                &req,
                                sizeof(wds_get_downlink_throughput_info_req_msg_v01),
                                &resp,
                                sizeof(wds_get_downlink_throughput_info_resp_msg_v01),
                                LQE_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(resp.resp.result != QMI_RESULT_SUCCESS_V01 && resp.resp.error == QMI_ERR_INVALID_QMI_CMD_V01)
    {
      LQE_LOG_ERROR("%s(): LQE PULL command is unsupported", __func__);
      ret = LQE_NOT_SUPPORTED;
    }
    else
    {
      LQE_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    }
    goto bail;
  }

  if (resp.downlink_rate_valid)
  {
    *dl_allowed_rate = resp.downlink_rate;
    LQE_LOG_INFO("%s(): downlink_rate : %u", __func__, resp.downlink_rate );
  }
  else
  {
    *dl_allowed_rate = 0;
  }

  if (resp.confidence_level_valid)
  {
    *confidence_level = resp.confidence_level;
    LQE_LOG_INFO("%s(): confidence_level : %u", __func__, resp.confidence_level );
  }
  else
  {
    *confidence_level = 0;
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}
