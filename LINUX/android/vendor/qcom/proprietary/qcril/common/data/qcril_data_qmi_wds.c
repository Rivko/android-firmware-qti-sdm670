/******************************************************************************

                 Q C R I L _ D A T A _ Q M I _ W D S . C

******************************************************************************/

/******************************************************************************

  @file    qcril_data_qmi_wds.c
  @brief   QMI WDS interface file for DSI Netctrl

  DESCRIPTION
  QMI WDS interface file for DSI Netctrl

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"
#include "qmi_client_helper.h"

#include "qcril_data_qmi_wds.h"
#include "qcril_log.h"
#include "ril.h"
#include "IxErrno.h"
#include "qcril_data_utils.h"

#include "qmi_i.h"
#include "wireless_data_service_v01.h"


/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

extern int global_instance_id;
extern int global_modem_id;
static qmi_cci_os_signal_type qcril_data_qmi_wds_os_params;


/*===========================================================================

  FUNCTION:  qcril_data_qmi_wds_ind_cb

===========================================================================*/
/*!
    @brief
    Receives qmi wds indications

    @return
    None.
*/
/*=========================================================================*/
void
qcril_data_qmi_wds_ind_cb
(
  qmi_client_type        wds_hndl,
  unsigned long          ind_id,
  unsigned char         *ind_buf,
  int                   ind_buf_len,
  void                  *user_data
)
{
  qcril_data_wds_event_data_t* event_data = NULL;
  void *ind_data_ptr = NULL;
  int ind_data_len = 0;
  int rc;

  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_ind_cb: ENTRY" );

  QCRIL_LOG_DEBUG( "%s %d ind", "qcril_data_qmi_wds_ind_cb",ind_id );

  if ((event_data = malloc(sizeof(qcril_data_wds_event_data_t))) != NULL)
  {
    switch(ind_id)
    {
      case QMI_WDS_EVENT_REPORT_IND_V01:
        {
          QCRIL_LOG_DEBUG("%s", "process QMI_WDS_EVENT_REPORT_IND_V01");

          ind_data_ptr = &event_data->ind_data.event_report;
          ind_data_len = sizeof(event_data->ind_data.event_report);
        }
        break;

      case QMI_WDS_EXTENDED_IP_CONFIG_IND_V01:
        {
          QCRIL_LOG_DEBUG("%s", "process QMI_WDS_EXTENDED_IP_CONFIG_IND_V01");

          ind_data_ptr = &event_data->ind_data.ext_ip_ind;
          ind_data_len = sizeof(event_data->ind_data.ext_ip_ind);
        }
        break;

      default:
        {
          QCRIL_LOG_DEBUG("%s", "Unkown WDS IND received");
          QCRIL_DATA_RELEASE_STORAGE(event_data)
          goto bail;
        }
    }

   /* Lock could not be acquired without blocking.  Make a deep copy of wds
       indication data and post the event to QCRIL for processing. */
    event_data->wds_hndl = wds_hndl;
    event_data->user_data = user_data;
    event_data->ind_id = ind_id;

    rc = qmi_client_message_decode( wds_hndl,
                                    QMI_IDL_INDICATION,
                                    ind_id,
                                    ind_buf,
                                    ind_buf_len,
                                    ind_data_ptr,
                                    (int)ind_data_len);
    if(rc != QMI_NO_ERR)
    {
      QCRIL_LOG_ERROR("%s: failed decode for ind [0x%x], err [%d]",
                       __func__, ind_id, rc);
      QCRIL_DATA_RELEASE_STORAGE(event_data)
      goto bail;
    }

    if(E_SUCCESS != qcril_event_queue(global_instance_id,
                                      global_modem_id,
                                      QCRIL_DATA_NOT_ON_STACK,
                                      QCRIL_EVT_DATA_WDS_EVENT_CALLBACK,
                                      event_data,
                                      sizeof(qcril_data_wds_event_data_t),
                                      (RIL_Token) QCRIL_TOKEN_ID_INTERNAL))
    {
      QCRIL_LOG_ERROR("%s", "qcril_event_queue failed\n");
      QCRIL_DATA_RELEASE_STORAGE(event_data)
    }
    /* If E_SUCCESS, event_data and its inner pointer will be freed after qm_wds indication
    is processed, at qcril_data_wds_event_hdlr */
  }

bail:
  QCRIL_LOG_DEBUG( "%s", "qcril_data_qmi_wds_ind_cb: EXIT" );
}


/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_init_qmi_client
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
qcril_data_qmi_wds_init_qmi_client
(
  const char                      *dev_str,
  void                            *user_data,
  wds_bind_subscription_enum_v01   subs_id,
  qmi_client_type                 *clnt_handle
)
{
  int ret = E_FAILURE;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;

  QCRIL_LOG_FUNC_ENTRY();

  if(!clnt_handle)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. clnt_handle=[%p]!\n",
                     __func__, clnt_handle);
    goto bail;
  }

  memset(&qcril_data_qmi_wds_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_handle = QCRIL_DATA_HNDL_INVALID;

  rc = qmi_client_wds_init_instance(dev_str,
                                    wds_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    (qmi_client_ind_cb)qcril_data_qmi_wds_ind_cb,
                                    user_data,
                                    &qcril_data_qmi_wds_os_params,
                                    QCRIL_DATA_QMI_TIMEOUT,
                                    clnt_handle);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("%s(): failed on qmi_client_wds_init_instance with rc=%d!\n",
                     __func__, rc);
    *clnt_handle = QCRIL_DATA_HNDL_INVALID;
    goto bail;
  }

  if (E_SUCCESS != qcril_data_qmi_wds_bind_subscription(*clnt_handle,
                                                          subs_id))
  {
    QCRIL_LOG_ERROR("%s(): subs_id binding failed for client [%p]",
                     __func__, *clnt_handle);
  }

  ret = E_SUCCESS;
  QCRIL_LOG_INFO("%s(): qmi_client_wds_init_instance successful\n", __func__);

bail:
  QCRIL_LOG_FUNC_RETURN();
  return rc;
}

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - E_SUCCESS on successful operation,
        E_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_release_qmi_client
(
  qmi_client_type          clnt_hndl
)
{
  int rc;
  int ret = E_FAILURE;

  QCRIL_LOG_FUNC_ENTRY();

  rc = qmi_client_release(clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("%s(): failed to release clnt_hndl=%p rc=%d!\n",
                   __func__, clnt_hndl, rc);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind the subscription to given configuration

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_bind_subscription
(
  qmi_client_type                     clnt_handle,
  wds_bind_subscription_enum_v01      subs_id
)
{
  int rc;
  int ret = E_FAILURE;
  wds_bind_subscription_req_msg_v01   req;
  wds_bind_subscription_resp_msg_v01  resp;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.subscription = subs_id;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                    QMI_WDS_BIND_SUBSCRIPTION_REQ_V01,
                                    (void *)&req,
                                    sizeof(req),
                                    (void*)&resp,
                                    sizeof(resp),
                                    QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_set_client_ip_pref
(
  qmi_client_type           clnt_handle,
  wds_ip_family_enum_v01    ip_family
)
{
  int rc;
  int ret = E_FAILURE;
  wds_set_client_ip_family_pref_req_msg_v01 req;
  wds_set_client_ip_family_pref_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.ip_preference = ip_family;

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_SET_CLIENT_IP_FAMILY_PREF_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    if(rc == QMI_SERVICE_ERR && resp.resp.error == QMI_ERR_NO_EFFECT_V01)
    {
      QCRIL_LOG_INFO("%s(): WDS client [%p] already bound to ip family [%d]",
                      __func__, clnt_handle, ip_family);
    }
    else
    {
      QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                       __func__, rc, resp.resp.error);
      goto bail;
    }
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_go_dormant_req
===========================================================================*/
/*!
@brief
  Force the modem to go dormant

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_go_dormant_req
(
  qmi_client_type         clnt_handle
)
{
  int rc;
  int ret = E_FAILURE;
  wds_go_dormant_req_msg_v01 req;
  wds_go_dormant_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GO_DORMANT_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_dormant_status
===========================================================================*/
/*!
@brief
  Get the dormancy status from the modem

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_dormant_status
(
  qmi_client_type                clnt_handle,
  wds_dormancy_status_enum_v01  *dorm_status
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_dormancy_status_req_msg_v01 req;
  wds_get_dormancy_status_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!dorm_status)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. dorm_status=[%p]\n",
                     __func__, dorm_status);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_DORMANCY_STATUS_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);


  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  *dorm_status = resp.dormancy_status;
  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_event_report
===========================================================================*/
/*!
@brief
  Set the connection state reporting conditions

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_set_event_report
(
  qmi_client_type                         clnt_handle,
  const wds_set_event_report_req_msg_v01 *params
)
{
  int rc;
  int ret = E_FAILURE;
  wds_set_event_report_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!params)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. params=[%p]\n",
                     __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_SET_EVENT_REPORT_REQ_V01,
                                 (void *)params,
                                 sizeof(*params),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);


  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_pref_data_system
===========================================================================*/
/*!
@brief
  Queries the preferred data system

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_pref_data_system
(
  qmi_client_type                    clnt_handle,
  wds_curr_pref_data_sys_enum_v01   *current_sys
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_preferred_data_system_req_msg_v01 req;
  wds_get_preferred_data_system_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!current_sys)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. current_sys=[%p]\n",
                     __func__, current_sys);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_PREFERRED_DATA_SYSTEM_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  if(resp.current_sys_valid != TRUE)
  {
    QCRIL_LOG_ERROR("%s(): current data system returned is invalid", __func__);
    goto bail;
  }

  *current_sys = resp.current_sys;
  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_data_system_status
===========================================================================*/
/*!
@brief
  Set the connection state reporting conditions

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_current_data_system_status
(
  qmi_client_type                   clnt_handle,
  wds_data_system_status_type_v01  *sys_status
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_current_data_system_status_req_msg_v01  req;
  wds_get_current_data_system_status_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!sys_status)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. sys_status=[%p]\n",
                     __func__, sys_status);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_CURRENT_SYSTEM_STATUS_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);


  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  if(resp.system_status_valid != TRUE)
  {
    QCRIL_LOG_ERROR("%s(): data system status returned is invalid", __func__);
    goto bail;
  }

  *sys_status = resp.system_status;
  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}


/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_query_profile
===========================================================================*/
/*!
@brief
  Get the profile settings for the given profile id

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_query_profile
(
  qmi_client_type                        clnt_handle,
  wds_profile_identifier_type_v01       *profile_id,
  wds_get_profile_settings_resp_msg_v01 *profile_params
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_profile_settings_req_msg_v01  req;

  QCRIL_LOG_FUNC_ENTRY();

  if (!profile_id || !profile_params)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. profile_id=[%p], profile_params=[%p]\n",
                     __func__, profile_id, profile_params);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(profile_params, 0, sizeof(wds_get_profile_settings_resp_msg_v01));

  req.profile = *profile_id;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)profile_params,
                                sizeof(*profile_params),
                                QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || profile_params->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, profile_params->resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_lte_max_attach_pdn_num
===========================================================================*/
/*!
@brief
  Queries the maximum number of attached PDNs supported

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_lte_max_attach_pdn_num
(
  qmi_client_type       clnt_handle,
  unsigned int         *max_attach_pdn_num
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_lte_max_attach_pdn_num_req_msg_v01 req;
  wds_get_lte_max_attach_pdn_num_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!max_attach_pdn_num)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. max_attach_pdn_num=[%p]\n",
                     __func__, max_attach_pdn_num);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_GET_LTE_MAX_ATTACH_PDN_NUM_REQ_V01,
                                 (void *)&req,
                                 sizeof(req),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  if(!resp.max_attach_pdn_num_valid)
  {
    QCRIL_LOG_ERROR("%s(): returned max_attach_pdn_num is invalid", __func__);
    goto bail;
  }

  *max_attach_pdn_num = resp.max_attach_pdn_num;
  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_lte_attach_pdn_list
===========================================================================*/
/*!
@brief
  Queries the attached PDN list

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_lte_attach_pdn_list
(
  qmi_client_type                           clnt_handle,
  wds_get_lte_attach_pdn_list_resp_msg_v01 *pdn_list
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_lte_attach_pdn_list_req_msg_v01 req;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (!pdn_list)
    {
      QCRIL_LOG_ERROR("%s(): BAD input. pdn_list=[%p]\n",
                       __func__, pdn_list);
      break;
    }

    memset(&req, 0, sizeof(req));
    memset(pdn_list, 0, sizeof(wds_get_lte_attach_pdn_list_resp_msg_v01));

    rc = qmi_client_send_msg_sync (clnt_handle,
                                   QMI_WDS_GET_LTE_ATTACH_PDN_LIST_REQ_V01,
                                   (void *)&req,
                                   sizeof(req),
                                   (void*)pdn_list,
                                   sizeof(*pdn_list),
                                   QCRIL_DATA_QMI_TIMEOUT);

    if ( rc != QMI_NO_ERR || pdn_list->resp.result != QMI_RESULT_SUCCESS_V01)
    {
      QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                       __func__, rc, pdn_list->resp.error);
      break;
    }

    ret = E_SUCCESS;
  } while(0);

  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_set_lte_attach_pdn_list
===========================================================================*/
/*!
@brief
  Sets the LTE attach PDN list

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_set_lte_attach_pdn_list
(
  qmi_client_type                           clnt_handle,
  wds_set_lte_attach_pdn_list_req_msg_v01  *pdn_list
)
{
  int rc;
  int ret = E_FAILURE;
  wds_set_lte_attach_pdn_list_resp_msg_v01 resp;

  QCRIL_LOG_FUNC_ENTRY();

  if (!pdn_list)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. pdn_list=[%p]\n",
                     __func__, pdn_list);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync (clnt_handle,
                                 QMI_WDS_SET_LTE_ATTACH_PDN_LIST_REQ_V01,
                                 (void *)pdn_list,
                                 sizeof(*pdn_list),
                                 (void*)&resp,
                                 sizeof(resp),
                                 QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
     FUNCTION  qcril_data_qmi_wds_get_lte_attach_params
===========================================================================*/
/*!
@brief
  Queries the current attach PDN params

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_get_lte_attach_params
(
  qmi_client_type                           clnt_handle,
  wds_get_lte_attach_params_resp_msg_v01    *attach_param
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_lte_attach_params_req_msg_v01 req;

  QCRIL_LOG_FUNC_ENTRY();

  do
  {
    if (!attach_param)
    {
      QCRIL_LOG_ERROR("%s(): BAD input. pdn_list=[%p]\n",
                       __func__, attach_param);
      break;
    }

    memset(&req, 0, sizeof(req));
    memset(attach_param, 0, sizeof(wds_get_lte_attach_params_resp_msg_v01));

    rc = qmi_client_send_msg_sync (clnt_handle,
                                   QMI_WDS_GET_LTE_ATTACH_PARAMS_REQ_V01,
                                   (void *)&req,
                                   sizeof(req),
                                   (void*)attach_param,
                                   sizeof(*attach_param),
                                   QCRIL_DATA_QMI_TIMEOUT);

    if( rc != QMI_NO_ERR ||(attach_param->resp.result != QMI_RESULT_SUCCESS_V01))
    {
      QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                       __func__, rc, attach_param->resp.error);
      break;
    }

    ret = E_SUCCESS;

  } while(0);

  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_indication_register
===========================================================================*/
/*!
@brief
  Register for indication messages from the modem

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE   if there was an error sending QMI message

*/
/*=========================================================================*/
int qcril_data_qmi_wds_indication_register
(
  qmi_client_type                       clnt_handle,
  wds_indication_register_req_msg_v01  *params
)
{
  int rc;
  int ret = E_FAILURE;
  wds_indication_register_resp_msg_v01   resp;

  QCRIL_LOG_FUNC_ENTRY();

  if (!params)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. params=[%p]\n",
                     __func__, params);
    goto bail;
  }

  memset(&resp, 0, sizeof(resp));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_INDICATION_REGISTER_REQ_V01,
                                (void *)params,
                                sizeof(wds_indication_register_req_msg_v01),
                                (void*)&resp,
                                sizeof(resp),
                                QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_runtime_settings
===========================================================================*/
/*!
@brief
  Queries Modem for runtime settings

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_runtime_settings
(
  qmi_client_type                         clnt_handle,
  wds_req_settings_mask_v01               req_mask,
  wds_get_runtime_settings_resp_msg_v01  *runtime_set
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_runtime_settings_req_msg_v01 req;

  QCRIL_LOG_FUNC_ENTRY();

  if(!runtime_set)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. runtime_set=[%p]",
                     __func__, runtime_set);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (runtime_set, 0, sizeof(wds_get_runtime_settings_resp_msg_v01));

  req.requested_settings_valid = TRUE;
  req.requested_settings = req_mask;

  rc = qmi_client_send_msg_sync(clnt_handle,
                           QMI_WDS_GET_RUNTIME_SETTINGS_REQ_V01,
                           (void *)&req,
                           sizeof(req),
                           (void*)runtime_set,
                           sizeof(*runtime_set),
                           QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || runtime_set->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, runtime_set->resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_profile_list
===========================================================================*/
/*!
@brief
  Gets the list of profiles from the modem

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_profile_list
(
  qmi_client_type                clnt_handle,
  wds_profile_type_enum_v01      profile_type,
  unsigned int                  *num_profile_list_elements,
  wds_profile_info_type_v01     *profile_list
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_profile_list_req_msg_v01  req;
  wds_get_profile_list_resp_msg_v01  resp;

  QCRIL_LOG_FUNC_ENTRY();

  if(!num_profile_list_elements || !profile_list)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. num_profile_list_elements=[%p] profile_list=[%p]",
                    __func__, num_profile_list_elements, profile_list);
    goto bail;
  }

  memset(&req, 0, sizeof(req));
  memset(&resp, 0, sizeof(resp));

  req.profile_type_valid = TRUE;
  req.profile_type = profile_type;

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_PROFILE_LIST_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, resp.resp.error);
    goto bail;
  }

  *num_profile_list_elements = (resp.profile_list_len < *num_profile_list_elements)
                                ? resp.profile_list_len : *num_profile_list_elements;

  if(*num_profile_list_elements > 0)
  {
    memcpy(profile_list,
           resp.profile_list,
           sizeof(wds_profile_info_type_v01) * (*num_profile_list_elements));
  }
  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_bearer_tech_ex
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_current_bearer_tech_ex
(
  qmi_client_type                                 clnt_handle,
  wds_get_data_bearer_technology_ex_resp_msg_v01 *bearer_tech_ex
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_data_bearer_technology_ex_req_msg_v01 req;

  QCRIL_LOG_FUNC_ENTRY();

  if(!bearer_tech_ex)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. bearer_tech_ex=%p",
                     __func__, bearer_tech_ex);
    goto bail;
  }

  memset(bearer_tech_ex, 0, sizeof(wds_get_data_bearer_technology_ex_resp_msg_v01));

  rc = qmi_client_send_msg_sync(clnt_handle,
                                QMI_WDS_GET_DATA_BEARER_TECHNOLOGY_EX_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)bearer_tech_ex,
                                sizeof(*bearer_tech_ex),
                                QCRIL_DATA_QMI_TIMEOUT);

  if (rc != QMI_NO_ERR || bearer_tech_ex->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, bearer_tech_ex->resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
    FUNCTION  qcril_data_qmi_wds_get_current_bearer_tech
===========================================================================*/
/*!
@brief
  Gets the data bearer technology

@return
  int - E_SUCCESS   on successful operation
        E_FAILURE     if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_get_current_bearer_tech
(
  qmi_client_type                                       clnt_handle,
  wds_get_current_data_bearer_technology_resp_msg_v01  *bearer_tech
)
{
  int rc;
  int ret = E_FAILURE;
  wds_get_data_bearer_technology_req_msg_v01 req;

  QCRIL_LOG_FUNC_ENTRY();

  if(!bearer_tech)
  {
    QCRIL_LOG_ERROR("%s(): BAD input. bearer_tech=[%p]", __func__, bearer_tech);
    goto bail;
  }

  memset(bearer_tech, 0, sizeof(wds_get_current_data_bearer_technology_resp_msg_v01));

  rc = qmi_client_send_msg_sync(clnt_handle,
                           QMI_WDS_GET_CURRENT_DATA_BEARER_TECHNOLOGY_REQ_V01,
                           (void *)&req,
                           sizeof(req),
                           (void*)bearer_tech,
                           sizeof(*bearer_tech),
                           QCRIL_DATA_QMI_TIMEOUT);

  if ( rc != QMI_NO_ERR || bearer_tech->resp.result != QMI_RESULT_SUCCESS_V01)
  {
    QCRIL_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                     __func__, rc, bearer_tech->resp.error);
    goto bail;
  }

  ret = E_SUCCESS;

bail:
  QCRIL_LOG_FUNC_RETURN();
  return ret;
}

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_release_and_init_client
===========================================================================*/
/*!
@brief
  Create a QMI client, releasing any stale handle if necessary

@return
  int - E_SUCCESS on successful operation,
        E_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_release_and_init_client
(
  const char                     *dev_str,
  void                           *user_data,
  wds_bind_subscription_enum_v01  subs_id,
  qmi_client_type                *clnt_handle
)
{
  int rc;

  // Validate input parameters
  if (NULL == clnt_handle) {
      QCRIL_LOG_ERROR("NULL clnt_handle ptr");
      return E_FAILURE;
  }

  // Release stale handle
  if (QCRIL_DATA_HNDL_INVALID != *clnt_handle)
  {
    QCRIL_LOG_DEBUG("releasing stale QMI WDS handle [0x%x]", *clnt_handle);
    qcril_data_qmi_wds_release_qmi_client(*clnt_handle);
    *clnt_handle = QCRIL_DATA_HNDL_INVALID;
  }

  // Create a new client
  rc = qcril_data_qmi_wds_init_qmi_client(dev_str,
                                          user_data,
                                          subs_id,
                                          clnt_handle);
  if (QMI_NO_ERR != rc)
  {
    QCRIL_LOG_ERROR("Failed to allocate WDS client, rc=%d", rc);
    return E_FAILURE;
  }

  return E_SUCCESS;
}

/*===========================================================================
  FUNCTION  qcril_data_qmi_wds_init_qmi_client_ex
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handles based on IP family

@return
  int - E_SUCCESS on successful operation,
        E_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int
qcril_data_qmi_wds_init_qmi_client_ex
(
  const char                      *dev_str,
  void                            *user_data,
  wds_bind_subscription_enum_v01   subs_id,
  int                              dsi_ip_ver,
  qmi_client_type                 *clnt_handle_v4,
  qmi_client_type                 *clnt_handle_v6
)
{
  if (DSI_IP_VERSION_4 != dsi_ip_ver && DSI_IP_VERSION_6 != dsi_ip_ver &&
          DSI_IP_VERSION_4_6 != dsi_ip_ver)
  {
    QCRIL_LOG_ERROR("IP family %d currently not supported", dsi_ip_ver);
    return E_FAILURE;
  }

  // Maybe create IPv4 client
  if (DSI_IP_VERSION_4 == dsi_ip_ver || DSI_IP_VERSION_4_6 == dsi_ip_ver)
  {
    if (E_SUCCESS != qcril_data_qmi_wds_release_and_init_client(dev_str,
                user_data,
                subs_id,
                clnt_handle_v4))
    {
        return E_FAILURE;
    }
  }

  // Maybe create IPv6 client
  if (DSI_IP_VERSION_6 == dsi_ip_ver || DSI_IP_VERSION_4_6 == dsi_ip_ver)
  {
    if (E_SUCCESS != qcril_data_qmi_wds_release_and_init_client(dev_str,
                user_data,
                subs_id,
                clnt_handle_v6))
    {
        return E_FAILURE;
    }
    else if (E_SUCCESS != qcril_data_qmi_wds_set_client_ip_pref(
                           *clnt_handle_v6,
                           WDS_IP_FAMILY_IPV6_V01))
    {
      QCRIL_LOG_ERROR("Failed to bind WDS client to IPv6");
      qcril_data_qmi_wds_release_qmi_client(
              *clnt_handle_v6);
      *clnt_handle_v6 = QCRIL_DATA_HNDL_INVALID;

      // Backpedal possible IPv4 clients
      if (QCRIL_DATA_HNDL_INVALID != clnt_handle_v4)
      {
        qcril_data_qmi_wds_release_qmi_client(
                *clnt_handle_v4);
        *clnt_handle_v4 = QCRIL_DATA_HNDL_INVALID;
      }

      return E_FAILURE;
    }
    QCRIL_LOG_INFO("Successfully binded the WDS client to IPv6");
  }

  return E_SUCCESS;
}
