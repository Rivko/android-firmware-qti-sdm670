#ifndef QDSS_QMI_CTRL_HWE_H
#define QDSS_QMI_CTRL_HWE_H
/*
  File: qdss_qmi_ctrl_hwe.h

  QDSS QMI control service handlers for HW event requests.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/qmi/ctrl_service/src/qdss_qmi_ctrl_hwe.h#1 $

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
qmi_csi_cb_error qdss_qmi_ctrl_set_hwe_reg_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
);

#endif /* #ifndef QDSS_QMI_CTRL_HWE_H */


