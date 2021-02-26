#ifndef QDSS_QMI_CTRL_TRACER_H
#define QDSS_QMI_CTRL_TRACER_H
/*
  File: qdss_qmi_ctrl_tracer.h

  QDSS QMI service handlers for Tracer (swt) requests.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/qmi/ctrl_service/src/qdss_qmi_ctrl_tracer.h#1 $

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
qmi_csi_cb_error qdss_qmi_ctrl_tracer_get_swtrace_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_set_swtrace_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );

/*===========================================================================*/
qmi_csi_cb_error qdss_qmi_ctrl_tracer_get_entity_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_set_entity_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );

qmi_csi_cb_error qdss_qmi_ctrl_tracer_get_entity_all_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_set_entity_all_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );

/*===========================================================================*/
qmi_csi_cb_error qdss_qmi_ctrl_tracer_get_swevt_tag_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_get_swevt_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_set_swevt_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );
qmi_csi_cb_error qdss_qmi_ctrl_tracer_set_rpm_swe_h (
   qmi_req_handle req_handle,  //in
   unsigned int msg_id,  //in
   void *req_c_struct,  //in
   unsigned int req_c_struct_len,  //in
   void *service_cookie  //in
   );

#endif /* #ifndef QDSS_QMI_CTRL_TRACER_H */


