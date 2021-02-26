/******************************************************************************

                       Q D P _ Q M I _W D S . C

******************************************************************************/

/******************************************************************************

  @file    qdp_qmi_wds.c
  @brief   QDP QMI wireless data service helper

  DESCRIPTION
  QDP QMI wireless data service helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qdp_qmi_wds.h"
#include "qmi_cci_target.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "qmi_client_instance_defs.h"
#include "qdp_platform.h"
#include "qmi_client_helper.h"

#define QDP_QMI_TIMEOUT                (10000)

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

static qmi_cci_os_signal_type qdp_qmi_wds_os_params;

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/
#define QDP_QMI_SEND_SYNC_REQ_RESP(msg_id)                                  \
  do                                                                        \
  {                                                                         \
    if (QDP_QMI_CLIENT_INVALID == clnt_hndl)                                \
    {                                                                       \
      QDP_LOG_ERROR("%s(): invalid clnt_hndl!\n",                           \
                    __func__);                                              \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    if (!req || !resp)                                                      \
    {                                                                       \
      QDP_LOG_ERROR("%s(): invalid input params!\n",                        \
                    __func__);                                              \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    ret = QDP_FAILURE;                                                      \
    memset (resp, 0, sizeof(*resp));                                        \
                                                                            \
    /* Send ext ip config registration */                                   \
    rc = qmi_client_send_msg_sync(clnt_hndl,                                \
                                  msg_id,                                   \
                                  (void *)req,                              \
                                  sizeof(*req),                             \
                                  (void*)resp,                              \
                                  sizeof(*resp),                            \
                                  QDP_QMI_TIMEOUT);                         \
                                                                            \
    if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp->resp.result)    \
    {                                                                       \
      QDP_LOG_ERROR("%s(): failed with rc=%d result=%d err=%d!\n",          \
                    __func__, rc, resp->resp.result, resp->resp.error);     \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    ret = QDP_SUCCESS;                                                      \
  }                                                                         \
  while(0)

#define QDP_QMI_SEND_SYNC_REQ(msg_id)                                       \
  do                                                                        \
  {                                                                         \
    if (QDP_QMI_CLIENT_INVALID == clnt_hndl)                                \
    {                                                                       \
      QDP_LOG_ERROR("%s(): invalid clnt_hndl!\n",                           \
                    __func__);                                              \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    if (!req)                                                               \
    {                                                                       \
      QDP_LOG_ERROR("%s(): invalid input params!\n",                        \
                    __func__);                                              \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    ret = QDP_FAILURE;                                                      \
    memset (&resp, 0, sizeof(resp));                                        \
                                                                            \
    /* Send ext ip config registration */                                   \
    rc = qmi_client_send_msg_sync(clnt_hndl,                                \
                                  msg_id,                                   \
                                  (void *)req,                              \
                                  sizeof(*req),                             \
                                  (void*)&resp,                             \
                                  sizeof(resp),                             \
                                  QDP_QMI_TIMEOUT);                         \
                                                                            \
    if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)     \
    {                                                                       \
      QDP_LOG_ERROR("%s(): failed with rc=%d result=%d err=%d!\n",          \
                    __func__, rc, resp.resp.result, resp.resp.error);       \
      goto bail;                                                            \
    }                                                                       \
                                                                            \
    ret = QDP_SUCCESS;                                                      \
  }                                                                         \
  while(0)

/*===========================================================================
  FUNCTION  qdp_qmi_wds_init_client
===========================================================================*/
/*!
@brief
  Initialize QMI WDS handle

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_init_client
(
  const char               *dev_str,
  qmi_client_type          *clnt_hndl
)
{
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;
  int epid = -1;
  int ep_type = -1;
  int mux_id = -1;
  qmi_client_qmux_instance_type qcci_conn_id;

  if (!clnt_hndl || !dev_str)
  {
    QDP_LOG_ERROR("%s(): invalid input params!\n", __func__);
    return QDP_FAILURE;
  }

  memset(&qdp_qmi_wds_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *clnt_hndl = QDP_QMI_CLIENT_INVALID;

  rc = qmi_client_wds_init_instance(dev_str,
                                    wds_get_service_object_v01(),
                                    QMI_CLIENT_INSTANCE_ANY,
                                    NULL,
                                    NULL,
                                    &qdp_qmi_wds_os_params,
                                    QDP_QMI_TIMEOUT,
                                    clnt_hndl);
  if (QMI_NO_ERR != rc)
  {
    QDP_LOG_ERROR("%s(): failed on qmi_client_wds_init_instance with rc=%d!\n", __func__, rc);
    goto bail;
  }

  return QDP_SUCCESS;

bail:
  if (QDP_QMI_CLIENT_INVALID != *clnt_hndl)
  {
    qmi_client_release(*clnt_hndl);
  }

  return QDP_FAILURE;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_release_client
===========================================================================*/
/*!
@brief
  Release QMI WDS handle

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_release_client
(
  qmi_client_type          clnt_hndl
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;

  if (QDP_QMI_CLIENT_INVALID == clnt_hndl)
  {
    QDP_LOG_ERROR("%s(): invalid clnt_hndl!\n",
                   __func__);
    goto bail;
  }

  rc = qmi_client_release(clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    QDP_LOG_ERROR("%s(): failed to release clnt_hndl=%p rc=%d!\n",
                   __func__, clnt_hndl, rc);
    goto bail;
  }

  ret = QDP_SUCCESS;

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_bind_subscription
===========================================================================*/
/*!
@brief
  Bind to the given subscription

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_bind_subscription
(
  qmi_client_type                    clnt_hndl,
  wds_bind_subscription_req_msg_v01  *req
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;
  wds_bind_subscription_resp_msg_v01  resp;

  QDP_QMI_SEND_SYNC_REQ(QMI_WDS_BIND_SUBSCRIPTION_REQ_V01);

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_delete_profile
===========================================================================*/
/*!
@brief
  Deletes the given profile

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_delete_profile
(
  qmi_client_type                  clnt_hndl,
  wds_delete_profile_req_msg_v01   *req
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;
  wds_delete_profile_resp_msg_v01 resp;

  QDP_QMI_SEND_SYNC_REQ(QMI_WDS_DELETE_PROFILE_REQ_V01);

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_get_profile_list
===========================================================================*/
/*!
@brief
  Query the list of profiles

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_get_profile_list
(
  qmi_client_type                   clnt_hndl,
  wds_get_profile_list_req_msg_v01  *req,
  wds_get_profile_list_resp_msg_v01 *resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;

  QDP_QMI_SEND_SYNC_REQ_RESP(QMI_WDS_GET_PROFILE_LIST_REQ_V01);

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_query_profile
===========================================================================*/
/*!
@brief
  Query the settings for a given profile

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_query_profile
(
  qmi_client_type                        clnt_hndl,
  wds_get_profile_settings_req_msg_v01   *req,
  wds_get_profile_settings_resp_msg_v01  *resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;

  QDP_QMI_SEND_SYNC_REQ_RESP(QMI_WDS_GET_PROFILE_SETTINGS_REQ_V01);

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_modify_profile
===========================================================================*/
/*!
@brief
  Modify the settings for a given profile

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_modify_profile
(
  qmi_client_type                           clnt_hndl,
  wds_modify_profile_settings_req_msg_v01   *req,
  wds_modify_profile_settings_resp_msg_v01  *resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;

  QDP_QMI_SEND_SYNC_REQ_RESP(QMI_WDS_MODIFY_PROFILE_SETTINGS_REQ_V01);

bail:
  return ret;
}

/*===========================================================================
  FUNCTION  qdp_qmi_wds_create_profile
===========================================================================*/
/*!
@brief
  Create a profile with the given parameters

@return
  int - QDP_SUCCESS on successful operation,
        QDP_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int qdp_qmi_wds_create_profile
(
  qmi_client_type                  clnt_hndl,
  wds_create_profile_req_msg_v01   *req,
  wds_create_profile_resp_msg_v01  *resp
)
{
  int rc = QMI_INTERNAL_ERR;
  int ret = QDP_FAILURE;

  QDP_QMI_SEND_SYNC_REQ_RESP(QMI_WDS_CREATE_PROFILE_REQ_V01);

bail:
  return ret;
}

