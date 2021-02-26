#ifndef QDSS_QMI_CTRL_SVC_H
#define QDSS_QMI_CTRL_SVC_H
/*
  File: qdss_qmi_service.h

  QDSS QMI Control Service
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/qmi/ctrl_service/src/qdss_qmi_ctrl_svc.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  02/26/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
qmi_csi_service_handle qdss_qmi_ctrl_svc_init(qmi_csi_os_params *os_params);

/*===========================================================================*/

#endif /* #ifndef QDSS_QMI_CTRL_SVC_H */

