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

typedef struct 
{
  boolean bIsInitialized;
  int32   nQDIHandle;

} qdss_dbglog_UserCtxtType;

static qdss_dbglog_UserCtxtType qdss_qdblog_UserCtxt;

int dbglog_uart(char *logbuf, int size)
{
  int nQDIResult;

  /*
   * Make sure the QDI path is open and all initialization has taken
   * place.
   */
  if( qdss_qdblog_UserCtxt.bIsInitialized == FALSE )
  {
    if(0 != (qdss_qdblog_UserCtxt.nQDIHandle = qurt_qdi_open(qdss_dbglogQDIName )))
    {
      qdss_qdblog_UserCtxt.bIsInitialized = TRUE;
    }
    else
    {
      return 1;
    }
  }

  /*
   * Send the request to the root PD.
   */
  nQDIResult = qurt_qdi_handle_invoke(qdss_qdblog_UserCtxt.nQDIHandle, 
                                      QDSS_DBGLOG_QDI_UART_TRANSMIT,
                                      logbuf,
                                      size);

  if(nQDIResult != 0)
  {
    return 2;
  }
  return 0;
}


