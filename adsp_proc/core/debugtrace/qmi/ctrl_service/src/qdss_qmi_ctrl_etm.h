#ifndef QDSS_QMI_CTRL_ETM_H
#define QDSS_QMI_CTRL_ETM_H
/*
  File: qdss_qmi_ctrl_etm.h

  QDSS QMI control service handlers for ETM requests.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1/debugtrace/qmi/ctrl_service/src/qdss_qmi_ctrl_etm.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  08/31/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "qmi_csi.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*===========================================================================*/
qmi_csi_cb_error qdss_qmi_ctrl_etm_get_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);
qmi_csi_cb_error qdss_qmi_ctrl_etm_set_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);

/*===========================================================================*/
qmi_csi_cb_error qdss_qmi_ctrl_etm_set_rpm_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);

/*===========================================================================*/
qmi_csi_cb_error qdss_qmi_ctrl_etm_get_config_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);
qmi_csi_cb_error qdss_qmi_ctrl_etm_set_config_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);

#endif /* #ifndef QDSS_QMI_CTRL_ETM_H */

