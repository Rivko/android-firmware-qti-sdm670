/******************************************************************************

                       DSI_NETCTRL_QMI_QOS.C

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi_qos.c
  @brief   DSI Netctrl QMI quality of service helper

  DESCRIPTION
  DSI Netctrl QMI quality of service helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_client.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"
#include "qmi_client_helper.h"

#include "assert.h"
#include "dsi_netctrli.h" /* declarations shared internally */
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl_qmi_qos.h"
#include "dsi_netctrl_cb_thrd.h"

#include "qmi_i.h"
#include "quality_of_service_v01.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
#define DSI_QMI_QOS_REQUEST_QOS_MSG_ID                      0x0020
#define DSI_QMI_QOS_RELEASE_QOS_MSG_ID                      0x0021
#define DSI_QMI_QOS_SUSPEND_QOS_MSG_ID                      0x0022
#define DSI_QMI_QOS_RESUME_QOS_MSG_ID                       0x0023
#define DSI_QMI_QOS_MODIFY_QOS_MSG_ID                       0x0024
#define DSI_QMI_QOS_GET_GRNTD_QOS_INFO_MSG_ID               0x0025
#define DSI_QMI_QOS_STATUS_REPORT_STATE_MSG_ID              0x0027
#define DSI_QMI_QOS_GET_FILTER_PARAMS_MSG_ID                0x002C

static qmi_cci_os_signal_type dsi_qmi_qos_os_params;


extern int
qmi_qos_process_resume_qos_req
(
  unsigned char          **msg,
  int                     *msg_size,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_srvc_extract_return_code
(
  unsigned char          *msg,
  int                    msg_size,
  qmi_response_type_v01  *resp
);

extern int
qmi_qos_process_get_filter_params_resp
(
  unsigned char                  *msg,
  int                            msg_size,
  qmi_qos_granted_info_rsp_type  *granted_info
);

extern int
qmi_qos_process_request_qos_req
(
  unsigned char             **msg,
  int                        *msg_size,
  int                         num_qos_specs,
  qmi_qos_spec_type          *qos_spec_array,
  qmi_qos_req_opcode_type     req_opcode_type,
  qmi_qos_err_rsp_type       *qos_spec_errs
);

extern int
qmi_qos_process_qos_req_failure_rsp
(
unsigned char          *msg_buf,
int                    msg_buf_size,
qmi_qos_err_rsp_type   *qos_spec_errs
);

extern int
qmi_qos_process_qos_req_success_rsp
(
unsigned char   *msg_buf,
int             msg_buf_size,
int             num_qos_specs,
unsigned long   *qos_identifiers
);

extern int
qmi_qos_process_release_qos_req
(
  unsigned char          **msg,
  int                     *msg_size,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
);

extern int
qmi_qos_process_modify_primary_qos_req
(
  unsigned char            **msg,
  int                       *msg_size,
  qmi_qos_spec_type         *qos_spec,
  qmi_qos_err_rsp_type      *qos_spec_err
);

extern int
qmi_qos_process_modify_secondary_qos_req
(
  unsigned char            **msg,
  int                       *msg_size,
  int                        num_qos_specs,
  qmi_qos_spec_type         *qos_spec_array,
  qmi_qos_err_rsp_type      *qos_spec_errs
);

extern int
qmi_qos_process_suspend_qos_req
(
  unsigned char          **msg,
  int                     *msg_size,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
);

extern int
qmi_qos_process_resume_qos_req
(
  unsigned char          **msg,
  int                     *msg_size,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_secondary_granted_qos_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_granted_qos_resp
(
  unsigned char                  *msg,
  int                            msg_size,
  qmi_qos_granted_info_rsp_type  *granted_info
);

extern int
qmi_qos_process_get_status_req
(
  unsigned char          **msg,
  int                     *msg_size,
  unsigned long            qos_identifier
);

extern int
qmi_qos_process_get_status_resp
(
  unsigned char          **msg,
  int                     *msg_size,
  qmi_qos_status_info     *status_resp
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_filter_params_req
(
  unsigned char  **msg,
  int            *msg_size,
  unsigned long  qos_identifier
);

extern int
qmi_qos_process_get_primary_granted_qos_req
(
  unsigned char  **msg,
  int             *msg_size,
  unsigned long    qos_identifier
);

extern int
qmi_qos_process_get_filter_params_resp
(
  unsigned char                  *msg,
  int                            msg_size,
  qmi_qos_granted_info_rsp_type  *granted_info
);

extern void
qmi_qos_srvc_process_nested_event_report_ind
(
  unsigned char                 *rx_msg_buf,
  int                           rx_msg_len,
  qmi_qos_event_report_type     *ind_data
);


/*===========================================================================
  FUNCTION:  dsi_qmi_qos_ind_cb
===========================================================================*/
/*!
    @brief
    callback function registered for qos indications.
    This function will post a command to separate dsi_netctrl_cb thread
    for later processing.

    @return
    none
*/
/*=========================================================================*/
void dsi_qmi_qos_ind_cb
(
  qmi_client_type               qos_hndl,
  unsigned int                  msg_id,
  void                         *ind_buf,
  unsigned int                  ind_buf_len,
  void                         *ind_data
)
{
  int ret = DSI_ERROR;
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;
  int rc;

  DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_qos_ind_cb: ENTRY");

  do
  {
    if (NULL == ind_data)
    {
      DSI_LOG_FATAL("%s", "*** rcvd invalid ind_data ***");
      break;
    }

    cmd_buf = (dsi_netctrl_cb_cmd_t *)dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
    if (NULL == cmd_buf)
    {
      DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
      break;
    }

    DSI_LOG_VERBOSE(">>>received qos_ind with qos_hndl [0x%p] "
                    "msg_id [%d] user_data [0x%p] ind_id [%d] ind_data [%p]",
                    qos_hndl, msg_id, ind_data, msg_id, ind_data);

    switch (msg_id)
    {
      case QMI_QOS_EVENT_REPORT_IND_V01:
      {
        unsigned char *ind = (unsigned char *)ind_buf;
        int ind_len = (int) ind_buf_len;
        qmi_qos_event_report_type  *event_report =
            &cmd_buf->cmd_data.data_union.qos_ind.ind_data.event_report;

        qmi_qos_srvc_process_nested_event_report_ind(ind,
                                                     ind_len,
                                                     event_report);
        break;
      }

      default:
        DSI_LOG_ERROR("received unsupported indication type %d\n", msg_id);
        ret = DSI_ERROR;
        ds_free(cmd_buf);
        goto bail;
    }


    /* set parameters in our internal structure  */
    cmd_buf->cmd_data.data_union.qos_ind.qos_hndl = qos_hndl;
    cmd_buf->cmd_data.data_union.qos_ind.qmi_service_id = QMI_QOS_SERVICE;
    cmd_buf->cmd_data.data_union.qos_ind.user_data = ind_data;
    cmd_buf->cmd_data.data_union.qos_ind.ind_id = msg_id;

    /* set broad category to discriminate data, at the end
       dsc_cmd_q is going to call the execute_f with data */
    cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_QOS_IND;

    /* prepare ds_cmd_t required by ds_cmdq */
    cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
    cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
    /* self pointer. this will be freed later */
    cmd_buf->cmd.data = cmd_buf;

    /* post command to global dsi_netctrl_cb queue */
    DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                  &cmd_buf->cmd);
    ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd);

    ret = DSI_SUCCESS;
  } while (0);

bail:

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_qos_ind_cb: EXIT with suc");
  }
  else
  {
    DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_qos_ind_cb: EXIT with err");
  }
}

/*===========================================================================
  FUNCTION  dsi_qmi_qos_get_primary_granted_qos
===========================================================================*/
/*!
@brief
  Return the QoS parameters for a particular QOS flow.

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
static int
dsi_qmi_qos_get_filter_params
(
  qmi_client_type                clnt_hndl,
  uint32                         flow_id,
  qmi_qos_granted_info_rsp_type  *granted_info
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_get_filter_params_req(&tmp_msg_ptr,
                                                          &msg_size,
                                                          flow_id))
  {
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_hndl,
                                    DSI_QMI_QOS_GET_FILTER_PARAMS_MSG_ID,
                                    msg,
                                    sizeof(msg)-msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed for with rc=%d!\n",
                   __func__, rc);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code!\n", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): get_filter_params req failed for with rc=%d!\n",
                   __func__, resp.error);
    goto bail;
  }

  rc = qmi_qos_process_get_filter_params_resp(msg,
                                              actual_resp_len,
                                              granted_info);

  if (QMI_NO_ERR != rc)
  {
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
static int
dsi_qmi_qos_bind_subscription
(
  qmi_client_type                      clnt_handle,
  qos_bind_subscription_enum_v01       subs_id
)
{
  int rc;
  int ret = DSI_ERROR;
  qos_bind_subscription_req_msg_v01 req;
  qos_bind_subscription_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.bind_subs = subs_id;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_QOS_BIND_SUBSCRIPTION_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s failed with rc=%d, qmi_err=%d", __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}


/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  dsi_qmi_qos_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI QOS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_init_qmi_client
(
  const char                      *dev_str,
  void                            *ind_cb_data,
  qos_bind_subscription_enum_v01   subs_id,
  qmi_client_type                 *clnt_handle
)
{
  int ret = DSI_ERROR;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;

  DSI_LOG_ENTRY;

  if(!clnt_handle)
  {
    DSI_LOG_ERROR("%s(): invalid input. clnt_handle=%p\n",  __func__, clnt_handle);
    goto bail;
  }

  memset(&dsi_qmi_qos_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_handle = DSI_INVALID_QOS_HNDL;

  rc = qmi_client_qos_init_instance(dev_str,
                                    qos_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    dsi_qmi_qos_ind_cb,
                                    (qmi_client_ind_cb)ind_cb_data,
                                    &dsi_qmi_qos_os_params,
                                    DSI_QMI_TIMEOUT,
                                    clnt_handle);

  if (QMI_NO_ERR != rc)
  {
    DSI_LOG_ERROR("%s(): failed on qmi_client_qos_init_instance with rc=%d!\n",
                   __func__, rc);
    *clnt_handle = DSI_INVALID_QOS_HNDL;
    goto bail;
  }

  if(DSI_SUCCESS != dsi_qmi_qos_bind_subscription(*clnt_handle, subs_id))
  {
    DSI_LOG_ERROR("%s(): subs_id binding failed", __func__);
  }

  DSI_LOG_DEBUG("%s(): qmi_client_init_instance successful\n", __func__);
  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  dsi_qmi_qos_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI QOS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_release_qmi_client
(
  qmi_client_type          clnt_hndl
)
{
  int rc;
  int ret = DSI_ERROR;

  DSI_LOG_ENTRY;

  rc = qmi_client_release(clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    DSI_LOG_ERROR("%s: failed to release clnt_hndl=%p rc=%d!\n",
                   __func__, clnt_hndl, rc);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_set_client_ip_pref
(
  qmi_client_type           clnt_handle,
  qos_ip_family_enum_v01    ip_family
)
{
  int rc;
  int ret = DSI_ERROR;
  qos_set_client_ip_pref_req_msg_v01 req;
  qos_set_client_ip_pref_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.ip_preference = ip_family;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                    QMI_QOS_SET_CLIENT_IP_PREF_REQ_V01,
                                    (void *)&req,
                                    sizeof(req),
                                    (void*)&resp,
                                    sizeof(resp),
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_request_qos
===========================================================================*/
/*!
@brief
  Send QOS request to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_request_qos
(
  qmi_client_type             clnt_handle,
  int                         num_qos_specs,
  qmi_qos_spec_type          *qos_spec_array,
  qmi_qos_req_opcode_type     req_opcode_type,
  unsigned long              *qos_id_array,    /* Valid if DSI_SUCCESS */
  qmi_qos_err_rsp_type       *qos_spec_errs   /* Valid if DSI_ERROR */
)
{
  int rc;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_request_qos_req(&tmp_msg_ptr,
                                                    &msg_size,
                                                    num_qos_specs,
                                                    qos_spec_array,
                                                    req_opcode_type,
                                                    qos_spec_errs))
  {
    DSI_LOG_ERROR("%s(): failed to construct req msg", __func__);
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_QOS_REQUEST_QOS_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, resp_len=%d",
                   __func__, rc, actual_resp_len);
    if (rc == QMI_SERVICE_ERR)
    {
      (void) qmi_qos_process_qos_req_failure_rsp(msg,
                                                 actual_resp_len,
                                                 qos_spec_errs);
    }
    goto bail;
  }

  /* Extract response information */
  if(QMI_NO_ERR != qmi_qos_process_qos_req_success_rsp(msg,
                                                       actual_resp_len,
                                                       num_qos_specs,
                                                       qos_id_array))
  {
    DSI_LOG_ERROR("%s(): failed process success response", __func__);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_release_qos
===========================================================================*/
/*!
@brief
  Send QOS release to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_release_qos
(
  qmi_client_type          clnt_handle,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
)
{
  int rc;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_release_qos_req(&tmp_msg_ptr,
                                                    &msg_size,
                                                    num_qos_ids,
                                                    qos_id_array))
  {
    DSI_LOG_ERROR("%s(): failed to construct req msg", __func__);
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_QOS_RELEASE_QOS_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, resp_len=%d",
                   __func__, rc, actual_resp_len);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg, actual_resp_len, &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): failed with qmi_err=%d", __func__, resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_modify_qos
===========================================================================*/
/*!
@brief
  Send QOS modify to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_modify_qos
(
  qmi_client_type          clnt_handle,
  boolean                  is_primary,
  unsigned int             num_qos_specs,
  qmi_qos_spec_type       *qos_spec_list,
  qmi_qos_err_rsp_type    *qos_spec_err
)
{
  int rc;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if(is_primary)
  {
    if (QMI_NO_ERR != qmi_qos_process_modify_primary_qos_req(&tmp_msg_ptr,
                                                             &msg_size,
                                                             qos_spec_list,
                                                             qos_spec_err))
    {
      DSI_LOG_ERROR("%s(): failed to construct req msg for primary", __func__);
      goto bail;
    }
  }
  else
  {
    if (QMI_NO_ERR != qmi_qos_process_modify_secondary_qos_req(&tmp_msg_ptr,
                                                               &msg_size,
                                                               num_qos_specs,
                                                               qos_spec_list,
                                                               qos_spec_err))
    {
      DSI_LOG_ERROR("%s(): failed to construct req msg for secondary", __func__);
      goto bail;
    }
  }
  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_QOS_MODIFY_QOS_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, resp_len=%d",
                   __func__, rc, actual_resp_len);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg, actual_resp_len, &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): failed with qmi_err=%d", __func__, resp.error);
    (void) qmi_qos_process_qos_req_failure_rsp (msg,
                                                actual_resp_len,
                                                qos_spec_err);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_suspend_qos
===========================================================================*/
/*!
@brief
  Send QOS suspend to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_suspend_qos
(
  qmi_client_type          clnt_handle,
  unsigned char            num_qos_ids,
  unsigned long           *qos_id_array
)
{
  int rc;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_suspend_qos_req(&tmp_msg_ptr,
                                                    &msg_size,
                                                    num_qos_ids,
                                                    qos_id_array))
  {
    DSI_LOG_ERROR("%s(): failed to construct req msg for secondary", __func__);
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_QOS_SUSPEND_QOS_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);
  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, resp_len=%d",
                   __func__, rc, actual_resp_len);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg, actual_resp_len, &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): failed with qmi_err=%d", __func__, resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_qos_resume_qos
===========================================================================*/
/*!
@brief
  Send QOS resume to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_resume_qos
(
  qmi_client_type          clnt_handle,
  unsigned char            num_qos_ids,
  unsigned long            *qos_id_array
)
{
  int rc;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_resume_qos_req(&tmp_msg_ptr,
                                                    &msg_size,
                                                    num_qos_ids,
                                                    qos_id_array))
  {
    DSI_LOG_ERROR("%s(): failed to construct req msg for secondary", __func__);
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_QOS_RESUME_QOS_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, resp_len=%d",
                   __func__, rc, actual_resp_len);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg, actual_resp_len, &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): failed with error=%d", __func__, resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  dsi_qmi_qos_get_granted_qos
===========================================================================*/
/*!
@brief
  Return the QoS parameters for a particular QOS flow.

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_get_granted_qos
(
  qmi_client_type                clnt_hndl,
  boolean                        is_primary,
  uint32                         flow_id,
  qmi_qos_granted_info_rsp_type  *granted_info
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (is_primary)
  {
    if (QMI_NO_ERR != qmi_qos_process_get_primary_granted_qos_req(&tmp_msg_ptr,
                                                                  &msg_size,
                                                                  flow_id))
    {
      goto bail;
    }
  }
  else
  {
    if (QMI_NO_ERR != qmi_qos_process_get_secondary_granted_qos_req(&tmp_msg_ptr,
                                                                    &msg_size,
                                                                    flow_id))
    {
      goto bail;
    }
  }

  rc = qmi_client_send_raw_msg_sync(clnt_hndl,
                                    DSI_QMI_QOS_GET_GRNTD_QOS_INFO_MSG_ID,
                                    msg,
                                    sizeof(msg)-msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed for with rc=%d!\n", __func__, rc);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code\n", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): get_granted_qos req failed with rc=%d!\n",
                   __func__, resp.error);
    goto bail;
  }

  rc = qmi_qos_process_get_granted_qos_resp(msg,
                                            actual_resp_len,
                                            granted_info);

  if (QMI_NO_ERR != rc)
  {
    goto bail;
  }

  /* request modem for filter params for the qos flow */
  ret = dsi_qmi_qos_get_filter_params(clnt_hndl,
                                      flow_id,
                                      granted_info);

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  dsi_qmi_qos_get_status
===========================================================================*/
/*!
@brief
  Get the QOS status from the modem

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_get_status
(
  qmi_client_type                clnt_hndl,
  unsigned long                  qos_identifier,
  qmi_qos_status_info           *status_resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = DSI_ERROR;
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int msg_size;
  unsigned int actual_resp_len = 0;
  qmi_response_type_v01  resp;

  DSI_LOG_ENTRY;

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  if (QMI_NO_ERR != qmi_qos_process_get_status_req(&tmp_msg_ptr,
                                                   &msg_size,
                                                   qos_identifier))
  {
    goto bail;
  }

  rc = qmi_client_send_raw_msg_sync(clnt_hndl,
                                    DSI_QMI_QOS_STATUS_REPORT_STATE_MSG_ID,
                                    msg,
                                    sizeof(msg)-msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed for with rc=%d!\n", __func__, rc);
    goto bail;
  }

  if (QMI_NO_ERR != qmi_qos_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
    DSI_LOG_ERROR("%s(): failed to extract return code\n", __func__);
    goto bail;
  }
  else if (QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): get_granted_qos req failed with rc=%d!\n",
                   __func__, resp.error);
    goto bail;
  }

  if(QMI_NO_ERR != qmi_qos_process_get_status_resp((unsigned char**)(&msg),
                                                   (int*)&actual_resp_len,
                                                   status_resp))
  {
    DSI_LOG_ERROR("%s(): failed to get status resp!\n", __func__);
    goto bail;
  }

  ret = DSI_SUCCESS;
bail:
  DSI_LOG_EXIT;
  return ret;
}
