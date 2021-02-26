/******************************************************************************
  @file    qmi_fw_init.c
  @brief   QMI framework init

  DESCRIPTION
  QMI framework init module
   
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#ifdef FEATURE_QMI_MESSAGE_BUS
#include "qmi_mb_xport.h"
#endif

extern qmi_csi_xport_ops_type qcsi_ipc_ops;
extern qmi_cci_xport_ops_type qcci_ipc_ops;

/* Loopback */
extern qmi_csi_xport_ops_type qcsi_loopback_ops;
extern qmi_cci_xport_ops_type qcci_loopback_ops;
extern void qmi_loopback_xport_init(void);

#ifdef FEATURE_QMI_MESSAGE_BUS
extern void qmi_mb_fw_init(void);
extern qmi_mb_xport_ops_type qmb_ipc_ops;
#endif

extern void qmi_ping_service_start(void);
extern void qmi_ping_client_start(void);
extern void qmi_si_init(void);
static int qmi_fw_inited = 0;

void qmi_fw_init(void)
{
  if(qmi_fw_inited)
    return;
  qmi_fw_inited = 1;

  /* TODO Enable when there are users */
//  qmi_si_init();

  qmi_cci_common_init();

#ifdef FEATURE_QMI_LOOPBACK_TRANSPORT
  /* Add the loopback transport only to User PD */
  qmi_loopback_xport_init();
  qmi_csi_xport_start(&qcsi_loopback_ops, NULL);
  qmi_cci_xport_start(&qcci_loopback_ops, NULL);
#endif

  /* IPC Router transport with QCSI & QCCI */
  qmi_csi_xport_start(&qcsi_ipc_ops, NULL);
  qmi_cci_xport_start(&qcci_ipc_ops, NULL);

#ifdef FEATURE_QMI_MESSAGE_BUS
  qmi_mb_fw_init();
  qmi_mb_xport_start(&qmb_ipc_ops, NULL);
#endif

#ifdef FEATURE_QMI_TEST_SERVICE_COMPONENTS
  qmi_ping_service_start();

/* Ping client enabled only for bringup testing eng builds */
//  qmi_ping_client_start();
#endif
}

