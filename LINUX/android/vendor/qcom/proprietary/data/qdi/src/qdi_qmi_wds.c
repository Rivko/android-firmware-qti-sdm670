/******************************************************************************

                       Q D I _ Q M I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    qdi_qmi_wds.c
  @brief   QMI WDS interface file for QDI

  DESCRIPTION
  QMI WDS interface file for QDI

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qmi_client.h"
#include "qmi.h"
#include "qmi_client_instance_defs.h"
#include "qmi_client_helper.h"
#include "qmi_platform.h"

#include "qmi_i.h"
#include "ds_cmdq.h"
#include "qdi.h"
#include "qdi_debug.h"
#include "qdi_qmi_wds.h"
#include "qdi_i.h"
#include "wireless_data_service_v01.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static qmi_client_type  qdi_qmi_wds_clnt_hndl, qdi_qmi_wds_notifier;
static qmi_cci_os_signal_type qdi_qmi_wds_os_params;

static QMI_PLATFORM_MUTEX_DATA_TYPE qdi_qmi_wds_client_data_list_mutex;

typedef struct qdi_qmi_wds_client_data_type
{
  struct qdi_qmi_wds_client_data_type *next;
  qmi_client_type  client_clnt_handle;
  struct
  {
    unsigned long pkt_data_handle;
  } data_call;
} qdi_qmi_wds_client_data_type;

static qdi_qmi_wds_client_data_type *qdi_qmi_wds_client_data_list = NULL;


static qdi_qmi_wds_client_data_type *
qdi_qmi_wds_get_client_data
(
  qmi_client_type clnt_handle
)
{
  qdi_qmi_wds_client_data_type *client_data,*prev;
  QMI_SLL_FIND (client_data,prev,qdi_qmi_wds_client_data_list,(client_data->client_clnt_handle == clnt_handle));
  (void)prev;  /* Keep lint happy */
  return client_data;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_handle_start_nw_rsp
===========================================================================*/
/*!
@brief
  Handle start network interface async response

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
static int
qdi_qmi_wds_handle_start_nw_rsp
(
  qmi_client_type                               clnt_handle,
  wds_start_network_interface_resp_msg_v01     *resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDI_FAILURE;
  qdi_qmi_wds_client_data_type *client_data;

  QDI_LOG_TRACE_ENTRY;

  if(!resp)
  {
    QDI_LOG_ERROR("%s(): Invalid input. resp=[%p] ", __func__, resp);
    goto bail;
  }

  /* Lock the client data list mutex */
  QMI_PLATFORM_MUTEX_LOCK (&qdi_qmi_wds_client_data_list_mutex);

  /* Get a pointer to the client data based on user handle */
  client_data = qdi_qmi_wds_get_client_data (clnt_handle);

  /* Make sure we got back valid pointer */
  if (client_data == NULL)
  {
    QDI_LOG_ERROR("%s(): Unable to retrieve client data for hndl [%p]",
                   __func__, clnt_handle);
  }
  else
  {
    client_data->data_call.pkt_data_handle = resp->pkt_data_handle;
    rc = QMI_NO_ERR;
  }

  QMI_PLATFORM_MUTEX_UNLOCK (&qdi_qmi_wds_client_data_list_mutex);

  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_async_cb
===========================================================================*/
/*!
@brief
 Main async callback function registered during client init. It decodes the async
response calls a fuction to post a command for processing

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None

*/
/*=========================================================================*/
static void
qdi_qmi_wds_async_cb
(
  qmi_client_type                clnt_handle,
  unsigned int                   msg_id,
  void                          *resp_c_struct,
  unsigned int                   resp_c_struct_len,
  void                          *resp_cb_data,
  qmi_client_error_type          transp_err
)
{
  qdi_qmi_wds_user_async_cb_type user_cb;
  qdi_qmi_wds_async_rsp_data_type rsp_data;
  int sys_err_code = QMI_NO_ERR;
  int qmi_err_code = QMI_SERVICE_ERR_NONE;
  qdi_cmdq_cmd_type  *cmd = NULL;

  (void)resp_c_struct_len;

  QDI_LOG_TRACE_ENTRY;

  if (transp_err != QMI_NO_ERR)
  {
    QDI_LOG_ERROR("%s(): Discarding aborted reply, msg_id=[0x%x], handle=[%p]",
                      __func__, msg_id, clnt_handle);
    goto bail;
  }

  memset((void*)&rsp_data, 0x0, sizeof(rsp_data));

  switch (msg_id)
  {
    case QMI_WDS_START_NETWORK_INTERFACE_RESP_V01:
      {
        memcpy(&rsp_data.start_nw_rsp,
               resp_c_struct,
               sizeof(rsp_data.start_nw_rsp));
        if(rsp_data.start_nw_rsp.resp.result != QMI_RESULT_SUCCESS_V01)
        {
          sys_err_code = QMI_SERVICE_ERR;
          qmi_err_code = rsp_data.start_nw_rsp.resp.error;
        }
        else if(QDI_SUCCESS !=
                    qdi_qmi_wds_handle_start_nw_rsp(clnt_handle,
                                                    &rsp_data.start_nw_rsp))
        {
          sys_err_code = QMI_INTERNAL_ERR;
        }
      }
      break;

    case QMI_WDS_STOP_NETWORK_INTERFACE_RESP_V01:
      {
        wds_stop_network_interface_resp_msg_v01 *resp =
          (wds_stop_network_interface_resp_msg_v01 *)resp_c_struct;
        if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
        {
          sys_err_code = QMI_SERVICE_ERR;
          qmi_err_code = resp->resp.error;
        }
      }
      break;

    case QMI_WDS_ABORT_RESP_V01:
      {
        wds_abort_resp_msg_v01 *resp =
          (wds_abort_resp_msg_v01 *)resp_c_struct;
        if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
        {
          sys_err_code = QMI_SERVICE_ERR;
          qmi_err_code = resp->resp.error;
        }
      }
      break;

    case QMI_WDS_RESET_RESP_V01:
      {
        wds_reset_resp_msg_v01 *resp =
          (wds_reset_resp_msg_v01 *)resp_c_struct;
        if(resp->resp.result != QMI_RESULT_SUCCESS_V01)
        {
          sys_err_code = QMI_SERVICE_ERR;
          qmi_err_code = resp->resp.error;
        }
      }
      break;

    default:
      QDI_LOG_ERROR("%s(): Discard unknown async resp, msg_id=[0x%x], handle=[%p]\n",
                        __func__, msg_id, clnt_handle);
      goto bail;
  }

  /* Free memory allocated for resp buffer in async send */
  if( NULL != resp_c_struct)
  {
    free(resp_c_struct);
  }

  cmd = ds_malloc(sizeof(qdi_cmdq_cmd_type));

  if (!cmd)
  {
    QDI_LOG_ERROR("Failed to allocate command");
    goto bail;
  }

  /* Store the response data in the command buffer */
  cmd->cmd_data.cmd = QDI_CMDQ_CMD_TYPE_WDS_ASYNC_RESP;
  cmd->cmd_data.data.async_resp.user_handle  = clnt_handle;
  cmd->cmd_data.data.async_resp.service_id   = QMI_WDS_SERVICE;
  cmd->cmd_data.data.async_resp.sys_err_code = sys_err_code;
  cmd->cmd_data.data.async_resp.qmi_err_code = qmi_err_code;
  cmd->cmd_data.data.async_resp.user_data    = resp_cb_data;
  cmd->cmd_data.data.async_resp.rsp_id       = msg_id;
  cmd->cmd_data.data.async_resp.rsp_data     = rsp_data;

  /* Update the processing and free functions to call */
  cmd->ds_cmd.execute_f = qdi_process_wds_async_cb;
  cmd->ds_cmd.free_f    = qdi_cmdq_free;

  /* Enqueue the command to be processed later in cmdq thread context */
  if (ds_cmdq_enq(&qdi_cmdq, &cmd->ds_cmd) < 0)
  {
    QDI_LOG_ERROR("Failed to enqueue command for rsp_id [0x%x]", msg_id);
    ds_free(cmd);
    goto bail;
  }

bail:
  QDI_LOG_TRACE_EXIT;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_ind_cb
===========================================================================*/
/*!
@brief
 Main callback function registered during client init. It decodes the indication
data and calls a fuction to post a command for processing

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None

*/
/*=========================================================================*/
static void
qdi_qmi_wds_ind_cb
(
  qmi_client_type                 clnt_handle,
  unsigned int                    msg_id,
  void                           *ind_buf,
  unsigned int                    ind_buf_len,
  void                           *ind_cb_data
)
{
  qdi_qmi_wds_indication_data_type ind_data;
  int rc;
  void *ind_data_ptr = NULL;
  int ind_data_len = 0;
  qdi_cmdq_cmd_type  *cmd = NULL;

  QDI_LOG_TRACE_ENTRY;

  QDI_LOG_MED("%s(): received indication=[0x%x]\n", __func__, msg_id);

  switch(msg_id)
  {
    case QMI_WDS_PKT_SRVC_STATUS_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_PKT_SRVC_STATUS_IND_V01");

        ind_data_ptr = &ind_data.pkt_srvc_status;
        ind_data_len = sizeof(ind_data.pkt_srvc_status);
      }
      break;

    case QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_TMGI_ACTIVATE_IND_V01");

        ind_data_ptr = &ind_data.embms_activate_status;
        ind_data_len = sizeof(ind_data.embms_activate_status);
      }
      break;

    case QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_TMGI_DEACTIVATE_IND_V01");

        ind_data_ptr = &ind_data.embms_deactivate_status;
        ind_data_len = sizeof(ind_data.embms_deactivate_status);
      }
      break;

    case QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_TMGI_ACT_DEACT_IND_V01");

        ind_data_ptr = &ind_data.embms_act_deact_status;
        ind_data_len = sizeof(ind_data.embms_act_deact_status);
      }
      break;

    case QMI_WDS_EMBMS_TMGI_LIST_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_TMGI_LIST_IND_V01");

        ind_data_ptr = &ind_data.embms_list;
        ind_data_len = sizeof(ind_data.embms_list);
      }
      break;

    case QMI_WDS_EMBMS_SAI_LIST_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_SAI_LIST_IND_V01");

        ind_data_ptr = &ind_data.sai_list;
        ind_data_len = sizeof(ind_data.sai_list);
      }
      break;

    case QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_CONTENT_DESC_CONTROL_IND_V01");

        ind_data_ptr = &ind_data.embms_content_desc_control;
        ind_data_len = sizeof(ind_data.embms_content_desc_control);
      }
      break;

    case QMI_WDS_EMBMS_SVC_INTEREST_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_EMBMS_SVC_INTEREST_IND_V01");

        ind_data_ptr = &ind_data.embms_svc_interest;
        ind_data_len = sizeof(ind_data.embms_svc_interest);
      }
      break;

    case QMI_WDS_HANDOFF_INFORMATION_IND_V01:
      {
        QDI_LOG_LOW("%s", "process QMI_WDS_HANDOFF_INFORMATION_IND_V01");

        ind_data_ptr = &ind_data.handoff_info;
        ind_data_len = sizeof(ind_data.handoff_info);
      }
      break;

    default:
      {
        QDI_LOG_ERROR("Dropping unkown indication [0x%x]", msg_id);
        goto bail;
      }
      break;
  }

  rc = qmi_client_message_decode( clnt_handle,
                                  QMI_IDL_INDICATION,
                                  msg_id,
                                  ind_buf,
                                  ind_buf_len,
                                  ind_data_ptr,
                                  (int)ind_data_len);
  if(rc != QMI_NO_ERR)
  {
    QDI_LOG_ERROR("%s(): failed decode for ind [0x%x], err [%d]",
                   __func__, msg_id, rc);
    goto bail;
  }

  cmd = ds_malloc(sizeof(qdi_cmdq_cmd_type));

  if (!cmd)
  {
    QDI_LOG_ERROR("Failed to allocate command");
    goto bail;
  }

  /* Store the indication data in the command buffer */
  cmd->cmd_data.cmd = QDI_CMDQ_CMD_TYPE_WDS_IND;
  cmd->cmd_data.data.wds_ind.user_handle = clnt_handle;
  cmd->cmd_data.data.wds_ind.service_id  = QMI_WDS_SERVICE;
  cmd->cmd_data.data.wds_ind.user_data   = ind_cb_data;
  cmd->cmd_data.data.wds_ind.ind_id      = msg_id;
  cmd->cmd_data.data.wds_ind.ind_data    = ind_data;

  /* Update the processing and free functions to call */
  cmd->ds_cmd.execute_f = qdi_process_wds_ind;
  cmd->ds_cmd.free_f    = qdi_cmdq_free;

  /* Enqueue the command to be processed later in cmdq thread context */
  if (ds_cmdq_enq(&qdi_cmdq, &cmd->ds_cmd) < 0)
  {
    QDI_LOG_ERROR("Failed to enqueue command for ind_id [0x%x]", msg_id);
    ds_free(cmd);
    goto bail;
  }

bail:
  QDI_LOG_TRACE_EXIT;
}


/*===========================================================================
    FUNCTION  qdi_qmi_wds_bind_mux_data_port
===========================================================================*/
/*!
@brief
  Bind Mux ID for a give data port instance

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
static int
qdi_qmi_wds_bind_mux_data_port
(
  qmi_client_type                        clnt_handle,
  wds_bind_mux_data_port_req_msg_v01    *params
)
{
  int rc;
  int ret = QDI_FAILURE;
  wds_bind_mux_data_port_resp_msg_v01 resp;

  QDI_LOG_TRACE_ENTRY;

  if(!params)
  {
    QDI_LOG_ERROR("%s: Invalid input received", __func__);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                           QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
                           (void *)params,
                           sizeof(wds_bind_mux_data_port_req_msg_v01),
                           (void*)&resp,
                           sizeof(resp),
                           QDI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QDI_LOG_ERROR("%s failed with rc=%d, qmi_err=%d", __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
static int
qdi_qmi_wds_bind_subscription
(
  qmi_client_type                     clnt_handle,
  wds_bind_subscription_enum_v01      subs_id
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_bind_subscription_req_msg_v01   req;
  wds_bind_subscription_resp_msg_v01  resp;

  QDI_LOG_TRACE_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.subscription = subs_id;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                    QMI_WDS_BIND_SUBSCRIPTION_REQ_V01,
                                    (void *)&req,
                                    sizeof(req),
                                    (void*)&resp,
                                    sizeof(resp),
                                    QDI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QDI_LOG_ERROR("%s failed with rc=%d, qmi_err=%d", __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_ENTRY;
  return ret;
}

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qdi_qmi_wds_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - QDI_SUCCESS on successful operation,
        QDI_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int
qdi_qmi_wds_init_qmi_client
(
  const char                          *dev_str,
  void                                *user_ind_msg_hdlr_user_data,
  wds_bind_subscription_enum_v01       subs_id,
  qmi_client_type                     *clnt_handle
)
{
  int rc = QMI_INTERNAL_ERR;
  unsigned int sio_port = 0;
  int epid = -1;
  int ep_type = -1;
  int mux_id = -1;

  QDI_LOG_TRACE_ENTRY;

  memset(&qdi_qmi_wds_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_handle = QDI_INVALID_QMI_HANDLE;

  rc = qmi_client_wds_init_instance(dev_str,
                                    wds_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    qdi_qmi_wds_ind_cb,
                                    user_ind_msg_hdlr_user_data,
                                    &qdi_qmi_wds_os_params,
                                    QDI_QMI_TIMEOUT,
                                    clnt_handle);

  if (QMI_NO_ERR != rc)
  {
    QDI_LOG_ERROR("%s(): failed on qmi_client_wds_init_instance with rc=%d!\n",
                   __func__, rc);
    *clnt_handle = QDI_INVALID_QMI_HANDLE;
  }
  else
  {
    qdi_qmi_wds_client_data_type *client_data;

    if (QDI_SUCCESS != qdi_qmi_wds_bind_subscription(*clnt_handle,
                                                     subs_id))
    {
      QDI_LOG_ERROR("%s(): subs_id binding failed for client [%p]",
                     __func__, *clnt_handle);
    }


    client_data = malloc (sizeof (qdi_qmi_wds_client_data_type));
    if(!client_data)
    {
      rc = QMI_INTERNAL_ERR;
      QDI_LOG_ERROR("%s(): failed to allocate memory", __func__);
      if(*clnt_handle)
      {
        qdi_qmi_wds_release_qmi_client(*clnt_handle);
      }
      goto bail;
    }

    QDI_LOG_LOW("%s(): qmi_client_wds_init_instance successful\n", __func__);
    memset (client_data,0,sizeof (qdi_qmi_wds_client_data_type));
    client_data->client_clnt_handle = *clnt_handle;

    QMI_PLATFORM_MUTEX_LOCK (&qdi_qmi_wds_client_data_list_mutex);
    QMI_SLL_ADD(client_data, qdi_qmi_wds_client_data_list);
    QMI_PLATFORM_MUTEX_UNLOCK (&qdi_qmi_wds_client_data_list_mutex);
  }

bail:
  QDI_LOG_TRACE_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  qdi_qmi_wds_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - QDI_SUCCESS on successful operation,
        QDI_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdi_qmi_wds_release_qmi_client
(
  qmi_client_type          clnt_hndl
)
{
  int rc;
  int ret = QDI_FAILURE;
  qdi_qmi_wds_client_data_type *client_data = NULL, *prev = NULL;

  QDI_LOG_TRACE_ENTRY;

  QMI_PLATFORM_MUTEX_LOCK (&qdi_qmi_wds_client_data_list_mutex);
  QMI_SLL_FIND_AND_REMOVE (client_data,prev,qdi_qmi_wds_client_data_list,(client_data->client_clnt_handle == clnt_hndl));
  QMI_PLATFORM_MUTEX_UNLOCK (&qdi_qmi_wds_client_data_list_mutex);

  if (client_data == NULL)
  {
    QDI_LOG_ERROR("%s(): client data null for hndl [%p]", __func__, clnt_hndl);
  }
  else
  {
    free(client_data);
    client_data = NULL;
  }

  rc = qmi_client_release(clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    QDI_LOG_ERROR("%s(): failed to release clnt_hndl=%p rc=%d!\n",
                   __func__, clnt_hndl, rc);
    goto bail;
  }

  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qdi_qmi_wds_set_client_ip_pref
(
  qmi_client_type           clnt_handle,
  wds_ip_family_enum_v01    ip_family
)
{
  int rc;
  int ret = QDI_FAILURE;
  wds_set_client_ip_family_pref_req_msg_v01 req;
  wds_set_client_ip_family_pref_resp_msg_v01 resp;

  QDI_LOG_TRACE_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.ip_preference = ip_family;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                QDI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QDI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Queries Modem for runtime settings

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qdi_qmi_wds_get_runtime_settings
(
  qmi_client_type                         clnt_handle,
  uint32_t                                req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
)
{
  int rc;
  int ret = QDI_FAILURE;
  wds_get_runtime_settings_req_msg_v01 req;
  wds_get_runtime_settings_resp_msg_v01 resp;

  QDI_LOG_TRACE_ENTRY;

  if(!runtime_set)
  {
    QDI_LOG_ERROR("%s(): Invalid input. runtime_set=[%p]",
                   __func__, runtime_set);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  req.requested_settings_valid = TRUE;
  req.requested_settings = req_mask;

  rc = qmi_client_send_msg_sync( clnt_handle,
                                 QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QDI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QDI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  *runtime_set = resp;
  ret = QDI_SUCCESS;

bail:
  QDI_LOG_TRACE_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_start_nw_if
===========================================================================*/
/*!
@brief
  Start network interface with the given settings - Async call

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qdi_qmi_wds_start_nw_if
(
  qmi_client_type                            clnt_handle,
  wds_start_network_interface_req_msg_v01   *params,
  void                                      *user_data,
  qdi_wds_call_end_reason_type              *call_end_reason_resp,
  int                                       *qmi_err_code
)
{
  int rc = QMI_INTERNAL_ERR;
  qmi_txn_handle txn_handle;
  wds_start_network_interface_resp_msg_v01 *resp = NULL;

  QDI_LOG_TRACE_ENTRY;

  if (!call_end_reason_resp || !params || !qmi_err_code)
  {
    QDI_LOG_ERROR ("%s(): Invalid input. params [%p], "
                   "call_end_reason_resp [%p], qmi_err_code [%p]\n",
                    __func__, params, call_end_reason_resp, qmi_err_code);
    goto bail;
  }

  resp = (wds_start_network_interface_resp_msg_v01*)
         malloc(sizeof(wds_start_network_interface_resp_msg_v01));
  if(!resp)
  {
    QDI_LOG_ERROR("Out of memory");
    goto bail;
  }
  memset (resp, 0, sizeof(wds_start_network_interface_resp_msg_v01));

  /* Call end reason is optional, so initialize to UNDEFINED */
  call_end_reason_resp->legacy_reason = WDS_CER_UNSPECIFIED_V01;
  call_end_reason_resp->call_end_reason_verbose. \
    verbose_reason = WDS_VCER_INTERNAL_INTERNAL_ERROR_V01;
  call_end_reason_resp->call_end_reason_verbose. \
    verbose_reason_type = WDS_VCER_TYPE_UNSPECIFIED_V01;

  *qmi_err_code = QMI_ERR_NONE_V01;

  if(!params->call_type_valid)
  {
    QDI_LOG_LOW("%s(): call_type not set. Using EMBEDDED type", __func__);
    params->call_type_valid = TRUE;
    params->call_type = WDS_CALL_TYPE_EMBEDDED_CALL_V01;
  }

  rc = qmi_client_send_msg_async (clnt_handle,
                                  QMI_WDS_START_NETWORK_INTERFACE_REQ_V01,
                                  (void *)params,
                                  sizeof(*params),
                                  (void*)resp,
                                  sizeof(wds_start_network_interface_resp_msg_v01),
                                  qdi_qmi_wds_async_cb,
                                  user_data,
                                  &txn_handle);

  if ( rc != QMI_NO_ERR )
  {
    QDI_LOG_ERROR("%s(): failed with rc=%d", __func__, rc);
    if( NULL != resp)
    {
      free(resp);
    }
  }

bail:
  QDI_LOG_TRACE_EXIT;
  return rc;
}


/*===========================================================================
    FUNCTION  qdi_qmi_wds_stop_nw_if
===========================================================================*/
/*!
@brief
  Stops network interface for the given handle

@return
  int - QMI ERROR CODE

*/
/*=========================================================================*/
int
qdi_qmi_wds_stop_nw_if
(
  qmi_client_type                     clnt_handle,
  boolean                             async_send,
  void                               *user_data,
  int                                *qmi_err_code
)
{
  int rc = QMI_INTERNAL_ERR;
  qdi_qmi_wds_client_data_type *client_data;
  wds_stop_network_interface_req_msg_v01  req;
  wds_stop_network_interface_resp_msg_v01 *resp = NULL;

  QDI_LOG_TRACE_ENTRY;

  if(!qmi_err_code)
  {
    QDI_LOG_ERROR("%s(): Invalid input. qmi_err_code [%p]",
                   __func__, qmi_err_code);
    goto bail;
  }
  *qmi_err_code = QMI_ERR_NONE_V01;

  memset(&req, 0, sizeof(req));
  resp = (wds_stop_network_interface_resp_msg_v01*)
         malloc(sizeof(wds_stop_network_interface_resp_msg_v01));
  if( !resp)
  {
    QDI_LOG_ERROR("Out of memory");
    goto bail;
  }
  memset(resp, 0, sizeof(wds_stop_network_interface_resp_msg_v01));

 /* Lock the client data list mutex */
  QMI_PLATFORM_MUTEX_LOCK (&qdi_qmi_wds_client_data_list_mutex);

  /* Get a pointer to the client data based on user handle */
  client_data = qdi_qmi_wds_get_client_data (clnt_handle);

  /* Make sure we got back valid pointer */
  if (client_data == NULL)
  {
    QMI_PLATFORM_MUTEX_UNLOCK (&qdi_qmi_wds_client_data_list_mutex);
    QDI_LOG_ERROR("%s(): client data null for hndl [%p]", __func__, clnt_handle);
    goto bail;
  }
  else
  {
    req.pkt_data_handle = client_data->data_call.pkt_data_handle;
  }

  QMI_PLATFORM_MUTEX_UNLOCK (&qdi_qmi_wds_client_data_list_mutex);

  if (async_send)
  {
    qmi_txn_handle txn_handle;
    rc = qmi_client_send_msg_async (clnt_handle,
                                    QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                                    (void *)&req,
                                    sizeof(req),
                                    (void*)resp,
                                    sizeof(wds_stop_network_interface_resp_msg_v01),
                                    qdi_qmi_wds_async_cb,
                                    user_data,
                                    &txn_handle);

    if ( rc != QMI_NO_ERR )
    {
      QDI_LOG_ERROR("%s(): send async failed with rc=%d", __func__, rc);
      if( NULL != resp)
      {
        free(resp);
      }
    }
  }
  else
  {
    rc = qmi_client_send_msg_sync(clnt_handle,
                                  QMI_WDS_STOP_NETWORK_INTERFACE_REQ_V01,
                                  (void *)&req,
                                  sizeof(req),
                                  (void*)resp,
                                  sizeof(wds_stop_network_interface_resp_msg_v01),
                                  QDI_QMI_TIMEOUT);

    *qmi_err_code = resp->resp.error;
    if (rc != QMI_NO_ERR || resp->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QDI_LOG_ERROR("%s(): send sync failed with rc=%d, error=%d",
                     __func__, rc, resp->resp.error);
    }
    if( NULL != resp)
    {
      free(resp);
    }
  }

bail:
  QDI_LOG_TRACE_EXIT;
  return rc;
}

/*===========================================================================
    FUNCTION  qdi_qmi_wds_abort
===========================================================================*/
/*!
@brief
  Aborts a previously issued QMI_WDS command

@return
  int - QDI_SUCCESS   on successful operation
        QDI_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qdi_qmi_wds_abort
(
  qmi_client_type                   clnt_handle,
  int                               txn_handle,
  int                              *qmi_err_code
)
{
  int rc = QMI_INTERNAL_ERR;
  wds_abort_req_msg_v01   req;
  wds_abort_resp_msg_v01  resp;

  QDI_LOG_TRACE_ENTRY;

  if(!qmi_err_code)
  {
    QDI_LOG_ERROR("%s(): Invalid input. qmi_err_code [%p]",
                   __func__, qmi_err_code);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.tx_id = txn_handle;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_ABORT_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                QDI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QDI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
  }
  *qmi_err_code = resp.resp.error;

bail:

  QDI_LOG_TRACE_EXIT;
  return rc;
}

