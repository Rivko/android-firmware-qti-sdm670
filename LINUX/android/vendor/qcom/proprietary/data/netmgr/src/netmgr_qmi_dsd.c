
/*****************************************************************************

      N E T M G R _ Q M I _D S D . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_dsd.c
  @brief   Network Manager QMI Data Sytstem determination service helper

  DESCRIPTION
  Network Manager QMI wireless data system determination service helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "netmgr_exec.h"
#include "netmgr_main.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"
#include "qmi_client_helper.h"
#include "netmgr_qmi_dsd.h"
#include "netmgr_util.h"
#include "qmi_i.h"
#include "qmi_util.h"


/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
static qmi_cci_os_signal_type  netmgr_qmi_dsd_os_params;

qmi_client_type         netmgr_dsd_clnt_hndl = NETMGR_QMI_CLIENT_INVALID;

/*===========================================================================
  FUNCTION  netmgr_qmi_dsd_ind_cb
===========================================================================*/
/*!
@brief
 Main callback function regisgterd during client init. It posts a command to
 do the required processing in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

void netmgr_qmi_dsd_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{

  netmgr_exec_cmd_t *cmd = NULL;
  qmi_client_error_type   qmi_err = QMI_INTERNAL_ERR;
  void *dest = NULL;
  int dest_len = 0;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT(ind_buf != NULL);

  /* Allocate a command object */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);


#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_wakelock_acquire();
#endif

  switch (msg_id) {
  case QMI_DSD_CURRENT_DDS_IND_V01:
  {
    netmgr_log_med("%s(): received QMI_DSD_CURRENT_DDS_IND_V01 indication",__func__);
    cmd->data.type                     = NETMGR_QMI_MSG_CMD;
    cmd->data.info.qmi_msg.type        = NETMGR_QMI_DSD_IND_CMD;

    /* Post command for processing in the command thread context */
    if (NETMGR_SUCCESS != netmgr_exec_put_cmd(cmd)) {
      NETMGR_ABORT("%s(): failed to put commmand\n", __func__);
      goto bail;
    }
    return;
  }
  default:
    netmgr_log_err("received unsupported indication type 0x%x\n", msg_id);
    goto bail;
  }

bail:
  if (NULL != cmd) {
    netmgr_exec_release_cmd(cmd);
  }
  NETMGR_LOG_FUNC_EXIT;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dsd_release_client
===========================================================================*/
/*!
@brief
  Release QMI DSD handle

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on failure

*/
/*=========================================================================*/
int netmgr_qmi_dsd_release_client
(
  void
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (NETMGR_QMI_CLIENT_INVALID == netmgr_dsd_clnt_hndl)
  {
    netmgr_log_err("%s(): invalid DSD clnt_hndl on link=0!",__func__);
    goto bail;
  }

  rc = qmi_client_release(netmgr_dsd_clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed to release clnt_hndl=%p on link=%0 rc=%d!\n",
                   __func__, netmgr_dsd_clnt_hndl,  rc);
    goto bail;
  }
  netmgr_dsd_clnt_hndl = NETMGR_QMI_CLIENT_INVALID;

  ret = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return ret;
}


/*===========================================================================

  FUNCTION:  netmgr_qmi_dsd_init

===========================================================================*/
/*!
    @brief
    Initialize the QMI DSD services for Netmgr

    @return
    none
*/
/*=========================================================================*/
void netmgr_qmi_dsd_init
(
  void
)
{
  qmi_idl_service_object_type  dsd_svc_obj;
  qmi_client_error_type        rc;
  int retry_counter = 0;
  dsd_indication_register_req_msg_v01  ind_reg_req;
  dsd_indication_register_resp_msg_v01 ind_reg_resp;
  dsd_bind_subscription_req_msg_v01    bind_req_msg;
  dsd_bind_subscription_resp_msg_v01   bind_resp_msg;

  /* Initialize QCCI client to send the open_port request */
  dsd_svc_obj = dsd_get_service_object_v01();
  if (dsd_svc_obj == NULL)
  {
    netmgr_log_err("%s(): DSD service not available", __func__);
    return ;
  }

  memset(&netmgr_qmi_dsd_os_params, 0, sizeof(qmi_cci_os_signal_type));

  netmgr_dsd_clnt_hndl = NETMGR_QMI_CLIENT_INVALID;

  rc = qmi_client_init_instance( dsd_svc_obj,
                                QMI_CLIENT_INSTANCE_ANY,
                                (qmi_client_ind_cb) netmgr_qmi_dsd_ind_cb,
                                NULL,
                                &netmgr_qmi_dsd_os_params,
                                NETMGR_QMI_TIMEOUT,
                                &netmgr_dsd_clnt_hndl);


  if (rc != QMI_NO_ERR)
  {
    netmgr_log_err("%s(): qmi_client_init_instance failed, err %d\n", __func__, rc);
    netmgr_dsd_clnt_hndl = NETMGR_QMI_CLIENT_INVALID;
    return ;
  }

  netmgr_log_med("netmgr_qmi_dsd_init(): qmi_client_init_instance SUCCESS, "
      "client: 0x%x\n", (unsigned int)netmgr_dsd_clnt_hndl);

  // Bind subscription
  memset(&bind_req_msg, 0, sizeof(bind_req_msg));
  memset(&bind_resp_msg, 0, sizeof(bind_resp_msg));

  bind_req_msg.bind_subs = DSD_PRIMARY_SUBS_V01;

  netmgr_log_med("binding subscription to subs to primary SUBS");

  rc = qmi_client_send_msg_sync(netmgr_dsd_clnt_hndl,
                                QMI_DSD_BIND_SUBSCRIPTION_REQ_V01,
                                &bind_req_msg,
                                sizeof(bind_req_msg),
                                &bind_resp_msg,
                                sizeof(bind_resp_msg),
                                NETMGR_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc ||
      QMI_RESULT_SUCCESS_V01 != bind_resp_msg.resp.result) {
    netmgr_log_err("failed to bind subscription, rc=%d, err=%d",
                    rc, bind_resp_msg.resp.error);

  }

  // Register indications
  memset(&ind_reg_req, 0, sizeof(ind_reg_req));
  memset(&ind_reg_resp, 0, sizeof(ind_reg_resp));

  ind_reg_req.report_current_dds_valid = 1;
  ind_reg_req.report_current_dds = 1;

  rc = qmi_client_send_msg_sync(netmgr_dsd_clnt_hndl,
                                QMI_DSD_INDICATION_REGISTER_REQ_V01,
                                &ind_reg_req,
                                sizeof(ind_reg_req),
                                &ind_reg_resp,
                                sizeof(ind_reg_resp),
                                NETMGR_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc ||
      QMI_RESULT_SUCCESS_V01 != ind_reg_resp.resp.result) {
    netmgr_log_err("failed to register DDS switch ind, rc=%d, err=%d",
                    rc, ind_reg_resp.resp.error);
  }

  NETMGR_LOG_FUNC_EXIT;

}


