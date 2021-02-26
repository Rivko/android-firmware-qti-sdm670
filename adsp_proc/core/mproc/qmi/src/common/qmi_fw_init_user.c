/******************************************************************************
  @file    qmi_fw_init.c
  @brief   QMI framework init

  DESCRIPTION
  QMI framework init module
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_csi_target.h"
#include "qmi_csi_common.h"
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_si.h"
#include "common_v01.h"
#include "qmi_common.h"
#include "dll_global_def.h"  /* Definitions for DLL macros */

extern qmi_csi_xport_ops_type qcsi_ipc_ops;
extern qmi_cci_xport_ops_type qcci_ipc_ops;

extern void qmi_ping_service_start(void);
extern void qmi_ping_client_start(void);

static int qmi_fw_inited = 0;

DLL_API_GLOBAL void qmi_fw_init(void)
{
  if(qmi_fw_inited)
    return;
  qmi_fw_inited = 1;

  qmi_cci_common_init();

  /* IPC Router transport with QCSI & QCCI */
  qmi_csi_xport_start(&qcsi_ipc_ops, NULL);
  qmi_cci_xport_start(&qcci_ipc_ops, NULL);
}

DLL_API_GLOBAL void qmi_fw_service_init(void)
{
  /* Start ping service */
//  qmi_ping_service_start();    

}

