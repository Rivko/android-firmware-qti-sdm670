#ifndef DBGLOG_PRIV_H
#define DBGLOG_PRIV_H

/*=============================================================================

FILE:         dbglog_priv.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "comdef.h"
//#include "uart.h"
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"

struct _qdss_dbglog {
  // uart_handle port;
   boolean    is_initialized;
   qurt_mutex_t Mutex; 
   qurt_qdi_obj_t qdss_dbglogQDIDrv;   
};


#define QDSS_DBGLOG_QDI_UART_TRANSMIT                 (1+QDI_PRIVATE)

/*
 * Micro DBGLOG QDI layer driver name.
 */
#define qdss_dbglogQDIName                            "/dev/qdss_dbglogQDI"

int qdss_dbglogQDI_Open
(
  int hClientHandle,
  qurt_qdi_obj_t *objptr,
  const char *devname,
  int mode
);

int qdss_dbglogQDI_Invoke(int hClientHandle,
              qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);

void qdss_dbglogQDI_Release(qurt_qdi_obj_t *qdiobj);              


extern struct _qdss_dbglog qdss_dbglog;


#endif //DBGLOG_PRIV_H
