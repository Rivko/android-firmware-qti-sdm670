/******************************************************************************

                     D S I _ N E T C T R L _ Q M I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi_wds.c
  @brief   QMI WDS interface file for DSI Netctrl

  DESCRIPTION
  QMI WDS interface file for DSI Netctrl

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qmi_client_instance_defs.h"
#include "qmi_client_helper.h" /* header file includes qmi_client.h */

#include "dsi_netctrli.h" /* declarations shared internally */
#include "dsi_config.h" /* declarations shared internally */
#include "dsi_netctrl_qmi_wds.h"

#include "dsi_netctrl_cb_thrd.h"

#include "qmi_util.h"
#include "wireless_data_service_v01.h"

#define DSI_QMI_RESULT_CODE_TYPE_ID       0x0002

#define DSI_QMI_WDS_ROUTE_LOOK_UP_MSG_ID          0xFFF2

/* Route look-up response TLV ID's */
#define DSI_QMI_WDS_ROUTE_LOOKUP_TYPE_REQ_TLV_ID  0x01
#define DSI_QMI_WDS_ROUTE_LOOKUP_IFACE_TLV_ID     0x01
#define DSI_QMI_WDS_ROUTE_LOOKUP_QMI_INST_TLV_ID  0x10
#define DSI_QMI_WDS_ROUTE_LOOKUP_MUX_ID_TLV_ID    0x13

/* Start NW Interface TLV ID's */
#define DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_REQ_TLV_ID                  0x14
#define DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_REQ_TLV_ID            0x19
#define DSI_QMI_WDS_ROUTE_LOOKUP_TECH_PREF_REQ_TLV_ID                 0x30
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_REQ_TLV_ID               0x31
#define DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_REQ_TLV_ID         0x32
#define DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_REQ_TLV_ID         0x34
#define DSI_QMI_WDS_ROUTE_LOOKUP_DATA_CALL_TYPE_TLV_ID                0x35

/* EMBMS Content Desc update request */
#define DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TMGI_TLV_ID      0x01
#define DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TRANX_ID_TLV_ID  0x02
#define DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TLV_OFFSET       0x10

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static qmi_client_type        qmi_wds_clnt_hndl;
static qmi_client_type        dsi_qmi_wds_notifier;
static qmi_cci_os_signal_type dsi_qmi_wds_os_params;
static qmi_cci_os_signal_type dsi_qmi_wds_notifier_os_params;
static boolean                dsi_qmi_wds_err_cb_reg = FALSE;
static boolean                dsi_qmi_wds_notif_cb_reg = FALSE;

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_notify_cb
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
static void dsi_qmi_wds_notify_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
)
{
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;
  qmi_sys_event_type sys_ev;

  (void)user_handle;
  (void)service_obj;

  switch(service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      DSI_LOG_DEBUG("%s(): WDS service is up!\n", __func__);
      if (dsi_config.modem_ssr_state != (int)DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND)
      {
        DSI_LOG_DEBUG("%s(): Not in SSR, ignoring WDS service up event!\n",
                       __func__);
        return;
      }

      cmd_buf = (dsi_netctrl_cb_cmd_t *)
      dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
      if (NULL == cmd_buf)
      {
        DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
        return;
      }

      /* Update the SSR state */
      dsi_config.modem_ssr_state = DSI_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND;

      /* set parameters in our internal structure  */
      cmd_buf->cmd_data.data_union.qmi_sys_ind.event_id = DSI_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND;
      cmd_buf->cmd_data.data_union.qmi_sys_ind.user_data = notify_cb_data;

      /* set broad category to discriminate data, at the end
         dsc_cmd_q is going to call the execute_f with data */
      cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_SYS_IND;

      /* prepare ds_cmd_t required by ds_cmdq */
      cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
      cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
      /* self pointer. this will be freed later */
      cmd_buf->cmd.data = cmd_buf;

      /* post command to global dsi_netctrl_cb queue */
      DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                       &cmd_buf->cmd);
      if(DSI_SUCCESS != ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd))
      {
        DSI_LOG_ERROR("%s(): failed to put commmand\n", __func__);
        ds_free(cmd_buf);
      }
      break;

  case QMI_CLIENT_SERVICE_COUNT_DEC:
    DSI_LOG_DEBUG("%s(): WDS service is down!", __func__);
    break;

  default:
    DSI_LOG_ERROR("%s(): Unsupported service event=%d!\n",
                   __func__, service_event);
    break;
  }

  DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_sys_cb: EXIT");
}

/*===========================================================================
  FUNCTION:  dsi_qmi_wds_err_cb
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
static void dsi_qmi_wds_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
)
{
  dsi_netctrl_cb_cmd_t * cmd_buf = NULL;
  qmi_sys_event_type sys_ev;

  (void)user_handle;
  (void)error;

  /* Update the SSR state */
  dsi_config.modem_ssr_state = (int)DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND;

  /* Clear the error callback registered flag */
  dsi_qmi_wds_err_cb_reg = FALSE;

  cmd_buf = (dsi_netctrl_cb_cmd_t *)
    dsi_malloc(sizeof(dsi_netctrl_cb_cmd_t));
  if (NULL == cmd_buf)
  {
    DSI_LOG_FATAL("%s","*** malloc failed for dsi_netctrl_cb cmd ***");
    return;
  }

  /* set parameters in our internal structure  */
  cmd_buf->cmd_data.data_union.qmi_sys_ind.event_id = DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND;
  cmd_buf->cmd_data.data_union.qmi_sys_ind.user_data = err_cb_data;

  /* set broad category to discriminate data, at the end
     dsc_cmd_q is going to call the execute_f with data */
  cmd_buf->cmd_data.type = DSI_NETCTRL_CB_CMD_QMI_SYS_IND;

  /* prepare ds_cmd_t required by ds_cmdq */
  cmd_buf->cmd.execute_f = dsi_netctrl_cb_cmd_exec;
  cmd_buf->cmd.free_f = dsi_netctrl_cb_cmd_free;
  /* self pointer. this will be freed later */
  cmd_buf->cmd.data = cmd_buf;

  /* post command to global dsi_netctrl_cb queue */
  DSI_LOG_VERBOSE(">>>posting cmd [%p] to dsi_netctrl_cb queue",
                &cmd_buf->cmd);
  if(DSI_SUCCESS != ds_cmdq_enq(&dsi_netctrl_cb_cmdq, &cmd_buf->cmd))
  {
    DSI_LOG_ERROR("%s(): failed to put commmand\n", __func__);
    ds_free(cmd_buf);
  }

  DSI_LOG_VERBOSE("%s", ">>>dsi_qmi_sys_cb: EXIT");
}


/*===========================================================================
    FUNCTION  dsi_qmi_wds_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
static int
dsi_qmi_wds_set_client_ip_pref
(
  qmi_client_type           clnt_handle,
  wds_ip_family_enum_v01    ip_family
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_set_client_ip_family_pref_req_msg_v01 req;
  wds_set_client_ip_family_pref_resp_msg_v01 resp;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.ip_preference = ip_family;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
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
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  dsi_qmi_wds_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_init_qmi_client
(
  const char               *dev_str,
  qmi_client_type          *clnt_handle
)
{
  int ret = DSI_ERROR;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;
  qmi_client_qmux_instance_type qcci_conn_id;

  DSI_LOG_ENTRY;

  memset(&dsi_qmi_wds_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_handle = DSI_INVALID_WDS_HNDL;

  if (DSI_SUCCESS !=(dsi_qmi_map_dev_conn_instance(dev_str, &qcci_conn_id)))
  {
    DSI_LOG_ERROR("%s(): bad device ID received\n", __func__);
    goto bail;
  }

  rc = qmi_client_wds_init_instance(dev_str,
                                    wds_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    NULL,
                                    NULL,
                                    &dsi_qmi_wds_os_params,
                                    DSI_QMI_TIMEOUT,
                                    clnt_handle);

  if (QMI_NO_ERR != rc)
  {
    DSI_LOG_ERROR("%s(): failed on qmi_client_wds_init_instance with rc=%d!\n", __func__, rc);
    *clnt_handle = DSI_INVALID_WDS_HNDL;
    goto bail;
  }

  if (!dsi_qmi_wds_err_cb_reg) {
    /* Register for err callback */
    (void)qmi_client_register_error_cb(*clnt_handle,
                                       dsi_qmi_wds_err_cb,
                                       (void *)dev_str);
    dsi_qmi_wds_err_cb_reg = TRUE;
  }

  if (!dsi_qmi_wds_notif_cb_reg) {
    /* Register for notification callback */
    (void)qmi_client_notifier_init(wds_get_service_object_v01(),
                                   &dsi_qmi_wds_notifier_os_params,
                                   &dsi_qmi_wds_notifier);

    (void)qmi_client_register_notify_cb(dsi_qmi_wds_notifier,
                                        dsi_qmi_wds_notify_cb,
                                        (void *)dev_str);

    dsi_qmi_wds_notif_cb_reg = TRUE;
  }

  ret = DSI_SUCCESS;
  DSI_LOG_DEBUG("%s(): qmi_client_init_instance successful\n", __func__);

bail:
  DSI_LOG_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  dsi_qmi_wds_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_release_qmi_client
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
    DSI_LOG_ERROR("%s(): failed to release clnt_hndl [%p] rc [%d]!\n",
                   __func__, clnt_hndl, rc);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_bind_mux_data_port
===========================================================================*/
/*!
@brief
  Bind Mux ID for a give data port instance

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_bind_mux_data_port
(
  qmi_client_type                           clnt_handle,
  const wds_bind_mux_data_port_req_msg_v01 *params
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_bind_mux_data_port_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params [%p]\n", __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_BIND_MUX_DATA_PORT_REQ_V01,
                                (void *)params,
                                sizeof(*params),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d\n",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  dsi_qmi_wds_write_optional_route_look_up_tlvs
===========================================================================*/
/*!
@brief
  Takes the input UMTS profile data, and writes it in TLV form
  to the tx buffer.  Buffer pointers and length indicators are adjusted
  to reflect new TLV

@return
  QMI_INTERNAL_ERR if an error occurred, QMI_NO_ERR if not

@note

  - Dependencies
    - None.

  - Side Effects
    - Writes TLV to input buffer and updates pointers and byte count
*/
/*=========================================================================*/
static int
dsi_qmi_wds_write_optional_route_look_up_tlvs
(
  unsigned char                        **tx_buf,
  int                                   *tx_buf_len,
  dsi_qmi_wds_route_look_up_params_type *params
)
{
  wds_call_type_enum_v01    data_call_origin;
  int ret = DSI_ERROR;

  DSI_LOG_ENTRY;

  /* Profile index param */
  if (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_PARAM)
  {
    if (qmi_util_write_std_tlv (tx_buf,
                                   tx_buf_len,
                                   DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_REQ_TLV_ID,
                                   1,
                                   (void *)&params->profile_index) < 0)
    {
      goto bail;
    }
  }
  if (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_PARAM)
  {
    if (qmi_util_write_std_tlv (tx_buf,
                                   tx_buf_len,
                                   DSI_QMI_WDS_ROUTE_LOOKUP_PROFILE_IDX_3GPP2_REQ_TLV_ID,
                                   1,
                                   (void *)&params->profile_index_3gpp2) < 0)
    {
      goto bail;
    }
  }
  /* APN name */
  if (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_PARAM)
  {
    size_t name_len = strlen (params->apn_name);
    if (name_len > QMI_WDS_APN_NAME_MAX_V01)
    {
      goto bail;
    }

    if (qmi_util_write_std_tlv (tx_buf,
                                   tx_buf_len,
                                   DSI_QMI_WDS_ROUTE_LOOKUP_APN_NAME_REQ_TLV_ID,
                                   name_len,
                                   (void *)params->apn_name) < 0)
    {
      goto bail;
    }
  }

  if (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_PARAM)
  {
    if (qmi_util_write_std_tlv (tx_buf,
                                   tx_buf_len,
                                   DSI_QMI_WDS_ROUTE_LOOKUP_XTENDED_TECH_PREF_REQ_TLV_ID,
                                   2,
                                   (void *)&params->xtended_tech_pref) < 0)
    {
      goto bail;
    }
  }

  /* IP family preference */
  if (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_PARAM)
  {
    if (qmi_util_write_std_tlv (tx_buf,
                                   tx_buf_len,
                                   DSI_QMI_WDS_ROUTE_LOOKUP_IP_FAMILY_PREF_REQ_TLV_ID,
                                   1,
                                   (void *)&params->ip_family_pref) < 0)
    {
      goto bail;
    }
  }

  data_call_origin = (params->params_mask & DSI_QMI_WDS_ROUTE_LOOKUP_DATA_CALL_ORIGIN_PARAM) ?
                                  params->data_call_origin : WDS_CALL_TYPE_EMBEDDED_CALL_V01;

  if (qmi_util_write_std_tlv (tx_buf,
                              tx_buf_len,
                              DSI_QMI_WDS_ROUTE_LOOKUP_DATA_CALL_TYPE_TLV_ID,
                              1,
                              (void *)&data_call_origin) < 0)
  {
    goto bail;
  }


  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
  FUNCTION  dsi_qmi_wds_route_look_up
===========================================================================*/
/*!
@brief
  Queries modem for route look up

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_route_look_up
(
  qmi_client_type                        clnt_handle,
  dsi_qmi_wds_route_look_up_params_type *params,
  dsi_qmi_wds_route_look_up_rsp_type    *rsp_data
)
{
  int rc, ret = DSI_ERROR;
  unsigned char     msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int               msg_size;
  unsigned int actual_resp_len = 0;

  qmi_response_type_v01 resp;
  unsigned long type;
  unsigned long length;
  unsigned char *value_ptr;

  DSI_LOG_ENTRY;


  if ((!params) || (!rsp_data))
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p], rsp_data=[%p]\n",
                   __func__, params, rsp_data);
    goto bail;
  }

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  /*Fill in the Mandatory TLV */
  if (qmi_util_write_std_tlv (&tmp_msg_ptr,
                              &msg_size,
                              DSI_QMI_WDS_ROUTE_LOOKUP_TYPE_REQ_TLV_ID,
                              1,
                              (void *)&params->route_lookup) < 0)
  {
    goto bail;
  }

  /* Configure optional TLV's if the params are present */
  if (params->params_mask != 0)
  {
    if (DSI_SUCCESS != dsi_qmi_wds_write_optional_route_look_up_tlvs(&tmp_msg_ptr,
                                                                     &msg_size,
                                                                     params))
    {
      goto bail;
    }
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    DSI_QMI_WDS_ROUTE_LOOK_UP_MSG_ID,
                                    msg,
                                    sizeof(msg) - msg_size,
                                    msg,
                                    sizeof(msg),
                                    &actual_resp_len,
                                    DSI_QMI_TIMEOUT);

  if(rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d!", __func__, rc);
    goto bail;
  }

  if (QMI_NO_ERR != dsi_qmi_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
      DSI_LOG_ERROR("%s(): failed to extract error code", __func__);
      goto bail;
  }
  else if(QMI_RESULT_SUCCESS_V01 != resp.result)
  {
    DSI_LOG_ERROR("%s(): failed with qmi_err=%d", __func__, resp.error);

/* Dont treat Interface not found  as API failure but consider it as no instance found */
    if(QMI_ERR_INTERFACE_NOT_FOUND_V01 == resp.error)
    {
      memset(rsp_data, 0, sizeof(dsi_qmi_wds_route_look_up_rsp_type));
      rsp_data->qmi_inst_is_valid = FALSE;
      rsp_data->mux_id_is_valid = FALSE;
      ret = DSI_SUCCESS;
      rsp_data->result = resp.result;
    }
    goto bail;
  }

  /* Get route look_up information */

  /* Set tmp_msg_ptr to return data */
  tmp_msg_ptr = msg;

  /* Initialize indication that we go qmi_inst to FALSE */
  memset(rsp_data, 0, sizeof(dsi_qmi_wds_route_look_up_rsp_type));
  rsp_data->qmi_inst_is_valid = FALSE;

  /* Loop through all TLV's and process each one */
  while (msg_size > 0)
  {
    if (qmi_util_read_std_tlv (&tmp_msg_ptr,
                               (int *) &actual_resp_len,
                               &type,
                               &length,
                               &value_ptr) < 0)
    {
      break;
    }

    switch (type)
    {
      case DSI_QMI_WDS_ROUTE_LOOKUP_IFACE_TLV_ID:
        {
          READ_32_BIT_VAL (value_ptr,rsp_data->iface_handle);
          READ_32_BIT_VAL (value_ptr,rsp_data->priority);
          READ_16_BIT_VAL (value_ptr,rsp_data->tech_name);
          ret = DSI_SUCCESS;
        }
        break;

      /* Modem would send either QMI instance or MUX id but not both */
      case DSI_QMI_WDS_ROUTE_LOOKUP_QMI_INST_TLV_ID:
      case DSI_QMI_WDS_ROUTE_LOOKUP_MUX_ID_TLV_ID:
        {
          rsp_data->qmi_inst_is_valid = TRUE;
          READ_8_BIT_VAL (value_ptr,rsp_data->qmi_inst);
          if(DSI_QMI_WDS_ROUTE_LOOKUP_MUX_ID_TLV_ID == type)
          {
            --rsp_data->qmi_inst;
            DSI_LOG_DEBUG("Mapped MUX ID to QMI inst [%d]",(int)rsp_data->qmi_inst);
          }
        }
        break;

      default:
        {
          DSI_LOG_DEBUG ("%s(): unknown response TLV type = 0x%x",
                          __func__, (unsigned int)type);
        }
        break;
    }
  }

bail:
  DSI_LOG_EXIT;
  return ret;
}


/*===========================================================================
    FUNCTION  dsi_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_bind_subscription
(
  qmi_client_type                     clnt_handle,
  wds_bind_subscription_enum_v01      subs_id
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_bind_subscription_req_msg_v01   req;
  wds_bind_subscription_resp_msg_v01  resp;

  DSI_LOG_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.subscription = subs_id;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                    QMI_WDS_BIND_SUBSCRIPTION_REQ_V01,
                                    (void *)&req,
                                    sizeof(req),
                                    (void*)&resp,
                                    sizeof(resp),
                                    DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
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
    FUNCTION  dsi_qmi_wds_get_pkt_srvc_status
===========================================================================*/
/*!
@brief
  Get the packet service status from the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_get_pkt_srvc_status
(
  qmi_client_type           clnt_handle,
  wds_ip_family_enum_v01    ip_family,
  dsi_link_status_type     *pkt_srvc_status
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_pkt_srvc_status_req_msg_v01 req;
  wds_get_pkt_srvc_status_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!pkt_srvc_status)
  {
    DSI_LOG_ERROR("%s(): Invalid input. pkt_srvc_status=[%p]\n",
                   __func__, pkt_srvc_status);
    goto bail;
  }

  if(DSI_SUCCESS != dsi_qmi_wds_set_client_ip_pref(clnt_handle,
                                                   ip_family))
  {
    DSI_LOG_ERROR("%s(): failed to set client IP preference", __func__);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_PKT_SRVC_STATUS_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(resp.resp.result != QMI_RESULT_SUCCESS_V01 &&
        resp.resp.error == QMI_ERR_INCOMPATIBLE_STATE_V01)
    {
      DSI_LOG_DEBUG("%s(): iface is busy on other subs. "
                    "Setting srvc status as connected", __func__);
      *pkt_srvc_status = WDS_CONNECTION_STATUS_CONNECTED_V01;
    }
    else
    {
      DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
      goto bail;
    }
  }
  else
  {
    *pkt_srvc_status = (dsi_link_status_type)resp.connection_status;
  }
  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_pkt_statistics
===========================================================================*/
/*!
@brief
  Resets the packet statistics

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_get_pkt_statistics
(
  qmi_client_type                       clnt_handle,
  wds_stats_mask_v01                    stats_mask,
  wds_get_pkt_statistics_resp_msg_v01  *stats_resp
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_pkt_statistics_req_msg_v01 req;

  DSI_LOG_ENTRY;

  if (!stats_resp)
  {
    DSI_LOG_ERROR("%s(): Invalid input. stats_resp=[%p]\n",
                   __func__, stats_resp);
    goto bail;
  }

  memset(stats_resp, 0, sizeof(wds_get_pkt_statistics_resp_msg_v01));
  memset(&req, 0, sizeof(req));

  req.stats_mask = stats_mask;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_PKT_STATISTICS_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)stats_resp,
                                sizeof(*stats_resp),
                                DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || stats_resp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, stats_resp->resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_reset_pkt_statistics
===========================================================================*/
/*!
@brief
  Resets the packet statistics

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_reset_pkt_statistics
(
  qmi_client_type clnt_handle
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_reset_pkt_statistics_req_msg_v01 req;
  wds_reset_pkt_statistics_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_RESET_PKT_STATISTICS_REQ_V01,
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
    FUNCTION  dsi_qmi_wds_indication_register
===========================================================================*/
/*!
@brief
  Register for indication messages from the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_indication_register
(
  qmi_client_type                       clnt_handle,
  wds_indication_register_req_msg_v01  *params
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_indication_register_resp_msg_v01   resp;

  DSI_LOG_ENTRY;

  if (!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p]\n",
                   __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                (void *)params,
                                sizeof(*params),
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
    FUNCTION  dsi_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Queries Modem for runtime settings

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_runtime_settings
(
  qmi_client_type                         clnt_handle,
  wds_req_settings_mask_v01               req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_runtime_settings_req_msg_v01 req;
  wds_get_runtime_settings_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if(!runtime_set)
  {
    DSI_LOG_ERROR("%s(): Invalid input. runtime_set=[%p]\n",
                   __func__, runtime_set);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  req.requested_settings_valid = TRUE;
  req.requested_settings = req_mask;

  rc = qmi_client_send_msg_sync(clnt_handle,
                           QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01,
                           (void *)&req,
                           sizeof(req),
                           (void*)&resp,
                           sizeof(resp),
                           DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, resp.resp.error);
    goto bail;
  }

  *runtime_set = resp;
  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}


/*===========================================================================
    FUNCTION  dsi_qmi_wds_remove_delegated_ipv6_prefix
===========================================================================*/
/*!
@brief
  Removes delegated IPv6 prefix

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_remove_delegated_ipv6_prefix
(
  qmi_client_type                                      clnt_handle,
  const wds_remove_delegated_ipv6_prefix_req_msg_v01  *params
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_remove_delegated_ipv6_prefix_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if(!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p]", __func__, params);
    goto bail;
  }

  memset (&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_REMOVE_DELEGATED_IPV6_PREFIX_REQ_V01,
                                (void *)params,
                                sizeof(*params),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
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
    FUNCTION  dsi_qmi_wds_get_current_channel_rate
===========================================================================*/
/*!
@brief
  Gets the current channel rate

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_channel_rate
(
  qmi_client_type                                clnt_handle,
  wds_get_current_channel_rate_resp_msg_v01     *ch_rate_resp
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_current_channel_rate_req_msg_v01 req;

  DSI_LOG_ENTRY;

  if(!ch_rate_resp)
  {
    DSI_LOG_ERROR("%s(): Invalid input. ch_rate_resp=[%p]\n",
                   __func__, ch_rate_resp);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(ch_rate_resp, 0, sizeof(wds_get_current_channel_rate_resp_msg_v01));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_CURRENT_CHANNEL_RATE_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)ch_rate_resp,
                                sizeof(*ch_rate_resp),
                                DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || ch_rate_resp->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, ch_rate_resp->resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_current_bearer_tech_ex
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_bearer_tech_ex
(
  qmi_client_type                                 clnt_handle,
  wds_get_data_bearer_technology_ex_resp_msg_v01 *bearer_tech_ex
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_data_bearer_technology_ex_req_msg_v01 req;

  DSI_LOG_ENTRY;

  if(!bearer_tech_ex)
  {
    DSI_LOG_ERROR("%s(): Invalid input. bearer_tech_ex=[%p]",
                   __func__, bearer_tech_ex);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(bearer_tech_ex, 0, sizeof(wds_get_data_bearer_technology_ex_resp_msg_v01));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_DATA_BEARER_TECHNOLOGY_EX_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)bearer_tech_ex,
                                 sizeof(*bearer_tech_ex),
                                 DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || bearer_tech_ex->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, bearer_tech_ex->resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_get_current_bearer_tech
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR     if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_get_current_bearer_tech
(
  qmi_client_type                                       clnt_handle,
  wds_get_current_data_bearer_technology_resp_msg_v01  *bearer_tech
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_get_data_bearer_technology_req_msg_v01 req;

  DSI_LOG_ENTRY;

  if(!bearer_tech)
  {
    DSI_LOG_ERROR("%s(): Invalid input. bearer_tech=[%p]\n",
                   __func__, bearer_tech);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(bearer_tech, 0, sizeof(wds_get_current_data_bearer_technology_resp_msg_v01));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_CURRENT_DATA_BEARER_TECHNOLOGY_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)bearer_tech,
                                 sizeof(*bearer_tech),
                                 DSI_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || bearer_tech->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, bearer_tech->resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_activate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI activate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_activate
(
  qmi_client_type                              clnt_handle,
  const wds_embms_tmgi_activate_req_msg_v01   *params,
  int                                         *ext_err_code
)
{
  int rc, i;
  int ret = DSI_ERROR;
  wds_embms_tmgi_activate_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params || !ext_err_code)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p], ext_err_code=[%p]\n",
                   __func__, params, ext_err_code);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));
  *ext_err_code = 0;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                    QMI_WDS_EMBMS_TMGI_ACTIVATE_REQ_V01,
                                    (void *)params,
                                    sizeof(*params),
                                    (void*)&resp,
                                    sizeof(resp),
                                    DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(resp.extended_error_code_valid)
    {
      *ext_err_code = resp.extended_error_code;
    }
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d, ext_err=%d",
                    __func__, rc, resp.resp.error, *ext_err_code);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_deactivate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI deactivate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_deactivate
(
  qmi_client_type                                clnt_handle,
  const wds_embms_tmgi_deactivate_req_msg_v01   *params,
  int                                           *ext_err_code
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_embms_tmgi_deactivate_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params || !ext_err_code)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p], ext_err_code=[%p]\n",
                   __func__, params, ext_err_code);
    goto bail;
  }

  memset(&resp, 0, sizeof(wds_embms_tmgi_deactivate_resp_msg_v01));
  *ext_err_code = 0;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_EMBMS_TMGI_DEACTIVATE_REQ_V01,
                                (void *)params,
                                sizeof(*params),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(resp.extended_error_code_valid)
    {
      *ext_err_code = resp.extended_error_code;
    }
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d, ext_err=%d",
                    __func__, rc, resp.resp.error, *ext_err_code);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_activate_deactivate
===========================================================================*/
/*!
@brief
  Send EMBMS TMGI activate - deactivate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_activate_deactivate
(
  qmi_client_type                               clnt_handle,
  const wds_embms_tmgi_act_deact_req_msg_v01   *params,
  int                                          *ext_err_code
)
{
  int rc, i;
  int ret = DSI_ERROR;
  wds_embms_tmgi_act_deact_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params || !ext_err_code)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p], ext_err_code=[%p]\n",
                   __func__, params, ext_err_code);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));
  *ext_err_code = 0;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_EMBMS_TMGI_ACT_DEACT_REQ_V01,
                                (void *)params,
                                sizeof(*params),
                                (void*)&resp,
                                sizeof(resp),
                                DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(resp.extended_error_code_valid)
    {
      *ext_err_code = resp.extended_error_code;
    }
     DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d, ext_err=%d",
                    __func__, rc, resp.resp.error, *ext_err_code);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_tmgi_list_query
===========================================================================*/
/*!
@brief
  Queries modem for EMBMS TMGI list

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_wds_embms_tmgi_list_query
(
  qmi_client_type                               clnt_handle,
  const wds_embms_tmgi_list_query_req_msg_v01  *params,
  wds_embms_tmgi_list_query_resp_msg_v01       *list_info
)
{
  int rc;
  int ret = DSI_ERROR;

  DSI_LOG_ENTRY;

  if (!params || !list_info)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p] list_info=[%p]\n",
                   __func__, params, list_info);
    goto bail;
  }

  memset(list_info, 0, sizeof(wds_embms_tmgi_list_query_resp_msg_v01));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_EMBMS_TMGI_LIST_QUERY_REQ_V01,
                                (void *)params,
                                sizeof(*params),
                                (void*)list_info,
                                sizeof(*list_info),
                                DSI_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || list_info->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                   __func__, rc, list_info->resp.error);
    goto bail;
  }

  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}

/*===========================================================================
    FUNCTION  dsi_qmi_wds_embms_content_desc_update
===========================================================================*/
/*!
@brief
  Send EMBMS content description updaate message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_embms_content_desc_update
(
  qmi_client_type                                         clnt_handle,
  const dsi_qmi_wds_embms_content_desc_update_info_type  *params
)
{
  unsigned char msg[QMI_MAX_STD_MSG_SIZE], *tmp_msg_ptr;
  int           msg_size, actual_resp_len;
  int rc;
  int ret = DSI_ERROR;
  unsigned int i;
  qmi_response_type_v01 resp;

  DSI_LOG_ENTRY;

  if(!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p]\n", __func__, params);
    goto bail;
  }

  tmp_msg_ptr = msg;
  msg_size = sizeof(msg);

  /* write TMGI TLV */
  if (qmi_util_write_std_tlv (&tmp_msg_ptr,
                              &msg_size,
                              DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TMGI_TLV_ID,
                              sizeof(wds_embms_tmgi_type_v01),
                              (void *)&params->tmgi_list) < 0)
  {
    DSI_LOG_ERROR("%s(): failed to write tmgi TLV", __func__);
    goto bail;
  }

  /* write transaction id TLVs */
  if (qmi_util_write_std_tlv (&tmp_msg_ptr,
                              &msg_size,
                              DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TRANX_ID_TLV_ID,
                              2,
                              (void*)&params->dbg_trace_id) < 0)
  {
    DSI_LOG_ERROR("%s(): failed to write tranx id TLV", __func__);
    goto bail;
  }

  /* Process optional TLVs */
  /* Optional TLVs are passed in as an array of param-value pairs */
  /* Use the DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TLV_OFFSET to calculate
     actual TLV IDs */
  if(NULL != (void *)params->content_desc_ptr)
  {
    for(i=0; i < params->content_desc_len; i++)
    {
      if (qmi_util_write_std_tlv (&tmp_msg_ptr,
                                  &msg_size,
                                  params->content_desc_ptr[i].param_code
                                    + DSI_QMI_WDS_EMBMS_CONT_DESC_UPDATE_REQ_TLV_OFFSET,
                                  4,
                                  (void *)&params->content_desc_ptr[i].param_value) < 0)
      {
        DSI_LOG_ERROR("%s: failed to write content desc update param TLV", __func__);
        goto bail;
      }
    }
  }

  rc = qmi_client_send_raw_msg_sync(clnt_handle,
                                    QMI_WDS_EMBMS_CONTENT_DESC_UPDATE_REQ_V01,
                                    msg,
                                    sizeof(msg)-msg_size,
                                    msg,
                                    sizeof(msg),
                                    (unsigned int *) &actual_resp_len,
                                    (unsigned int) DSI_QMI_TIMEOUT);

  if(rc != QMI_NO_ERR || actual_resp_len == 0)
  {
    DSI_LOG_ERROR("%s(): failed with rc=%d!", __func__, rc);
    goto bail;
  }

  if (QMI_NO_ERR != dsi_qmi_srvc_extract_return_code(msg,
                                                     actual_resp_len,
                                                     &resp))
  {
      DSI_LOG_ERROR("%s(): failed to extract error code. rc=%d", __func__, rc);
      goto bail;
  }
  else if(QMI_RESULT_SUCCESS_V01 != resp.result)
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
    FUNCTION  dsi_qmi_wds_embms_svc_interest
===========================================================================*/
/*!
@brief
  Send EMBMS service interest info message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_embms_svc_interest
(
  qmi_client_type                                  clnt_handle,
  const wds_embms_svc_interest_info_req_msg_v01   *params
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_embms_svc_interest_info_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p]\n",
                   __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_EMBMS_SVC_INTEREST_INFO_REQ_V01,
                                (void *)params,
                                sizeof(*params),
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
    FUNCTION  dsi_qmi_wds_keep_alive_data_session
===========================================================================*/
/*!
@brief
  Send data session keep alive message

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_wds_keep_alive_data_session
(
  qmi_client_type                                  clnt_handle,
  const wds_keep_alive_data_session_req_msg_v01    *params
)
{
  int rc;
  int ret = DSI_ERROR;
  wds_keep_alive_data_session_resp_msg_v01 resp;

  DSI_LOG_ENTRY;

  if (!params)
  {
    DSI_LOG_ERROR("%s(): Invalid input. params=[%p]\n",
                   __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_KEEP_ALIVE_DATA_SESSION_REQ_V01,
                                (void *)params,
                                sizeof(*params),
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

