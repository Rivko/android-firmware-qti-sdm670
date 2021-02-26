/******************************************************************************

                       Q D I _ Q M I _ W D S . H

******************************************************************************/

/******************************************************************************

  @file     qdi_qmi_wds.h
  @brief    QMI WDS interface file header for QDI

  DESCRIPTION
  QMI WDS interface file header for QDI

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef QDI_QMI_WDS_H
#define QDI_QMI_WDS_H

#include "qmi.h"
#include "qmi_client.h"

#include "ds_cmdq.h"
#include "qdi.h"
#include "wireless_data_service_v01.h"

#define QDI_QMI_TIMEOUT              (10000)

typedef  void (*qdi_qmi_wds_user_async_cb_type)
(
  int                          user_handle,
  qmi_service_id_type          service_id,
  int                          sys_err_code,
  int                          qmi_err_code,
  void                         *user_data,
  int                           rsp_id,
  qdi_qmi_wds_async_rsp_data_type  *rsp_data
);


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
);

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
);


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
  qmi_client_type          clnt_handle,
  wds_ip_family_enum_v01   ip_family
);


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
);


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
);


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
);


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
);

#endif /* QDI_QMI_WDS_H */
