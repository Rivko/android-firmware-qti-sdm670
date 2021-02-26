/*=============================================================================

FILE:         dbglog_micro.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "dbglog.h"
#include "dbglog_priv.h"

#include "stdlib.h"

struct _qdss_dbglog qdss_dbglog;

const qurt_qdi_obj_t qdss_dbglogQDIOpener = {
    qdss_dbglogQDI_Invoke,
    QDI_REFCNT_PERM,
    qdss_dbglogQDI_Release    
};

int dbglog_uart(char *logbuf, int size)
{
   uint32 tx_bytes  = 0;
   if  (!qdss_dbglog.is_initialized) {
      return 1;
   }

   qurt_mutex_lock(&qdss_dbglog.Mutex);

   //tx_bytes = Uart_transmit(qdss_dbglog.port, logbuf, size);

   qurt_mutex_unlock(&qdss_dbglog.Mutex);
   
   if (size != tx_bytes) {
      return 2;
   }
   return 0;
}


int qdss_dbglogQDI_Open
(
  int hClientHandle,
  qurt_qdi_obj_t *objptr,
  const char *devname,
  int mode
)
{
  /*
   * Initialize the QDI object function pointers and create the handle.
   */
  qdss_dbglog.qdss_dbglogQDIDrv.invoke = qdss_dbglogQDI_Invoke;
  qdss_dbglog.qdss_dbglogQDIDrv.refcnt = QDI_REFCNT_INIT;
  qdss_dbglog.qdss_dbglogQDIDrv.release = qdss_dbglogQDI_Release;

  return qurt_qdi_handle_create_from_obj_t(hClientHandle,&qdss_dbglog.qdss_dbglogQDIDrv);

}

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
                          qurt_qdi_arg_t a9)
{
  switch (method) 
  {
    case QDI_OPEN:
      return qdss_dbglogQDI_Open(hClientHandle, (void *)pobj, a1.ptr, a2.num);

    case QDSS_DBGLOG_QDI_UART_TRANSMIT:
      return dbglog_uart(a1.ptr, a2.num);

    case QDI_ISLAND_CHECK_ALLOC:
      return 1;
	  
    default:
      return qurt_qdi_method_default(hClientHandle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);	
  }

  return -1;
}


void qdss_dbglogQDI_Release ( qurt_qdi_obj_t *qdiobj )
{
  free(qdiobj);
}
