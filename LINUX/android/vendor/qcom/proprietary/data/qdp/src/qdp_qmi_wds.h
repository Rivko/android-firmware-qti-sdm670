/******************************************************************************

                       Q D P _ Q M I _W D S . H

******************************************************************************/

/******************************************************************************

  @file    qdp_qmi_wds.h
  @brief   Network Manager QMI wireless data service header

  DESCRIPTION
  Network Manager QMI wireless data service header

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef __QDP_QMI_WDS_H__
#define __QDP_QMI_WDS_H__

#ifndef QMI_CLIENT_H
  #undef QMI_INTERNAL_ERR
  #undef QMI_SERVICE_ERR
  #undef QMI_TIMEOUT_ERR
  #undef QMI_EXTENDED_ERR
  #undef QMI_PORT_NOT_OPEN_ERR
  #undef QMI_MEMCOPY_ERROR
  #undef QMI_INVALID_TXN
  #undef QMI_CLIENT_ALLOC_FAILURE
#endif

#include "qmi_client.h"
#include "wireless_data_service_v01.h"
#include "qdp.h"

#define QDP_QMI_CLIENT_INVALID (NULL)

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
);

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
);

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
);

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
  wds_delete_profile_req_msg_v01   *del_profile
);

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
  qmi_client_type                    clnt_hndl,
  wds_get_profile_list_req_msg_v01   *req,
  wds_get_profile_list_resp_msg_v01  *resp
);

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
);

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
);

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
);

#endif /* __QDP_QMI_WDS_H__ */
