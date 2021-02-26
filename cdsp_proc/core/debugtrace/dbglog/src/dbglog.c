/*=============================================================================

FILE:         dbglog.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "dbglog_priv.h"

extern const qurt_qdi_obj_t qdss_dbglogQDIOpener;

void dbglog_init(void) 
{
   if (qdss_dbglog.is_initialized)  {
      return;
   }

   qurt_mutex_init(&qdss_dbglog.Mutex);

   if (qurt_qdi_register_devname(qdss_dbglogQDIName, &qdss_dbglogQDIOpener) < 0)
   {
      return;
   }

 //  if (UART_SUCCESS==Uart_init(&qdss_dbglog.port,UART_SECOND_PORT)) {
   //   qdss_dbglog.is_initialized = TRUE;
  // }
}

