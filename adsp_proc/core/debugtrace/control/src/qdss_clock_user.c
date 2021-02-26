/*=============================================================================

FILE:         qdss_clock_user.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2017 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qdss_clock_priv.h"
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"


typedef struct 
{
  boolean bIsInitialized;
  int32   nQDIHandle;
} qdss_clock_UserCtxtType;

static qdss_clock_UserCtxtType qdss_clock_UserCtxt;

int QDSSLocalClkSet(int on_off)
{
  int nQDIResult;

  /*
   * Make sure the QDI path is open and all initialization has taken
   * place.
   */
  if( qdss_clock_UserCtxt.bIsInitialized == FALSE )
  {
    if(0 != (qdss_clock_UserCtxt.nQDIHandle = qurt_qdi_open(qdss_clockQDIName )))
    {
      qdss_clock_UserCtxt.bIsInitialized = TRUE;
    }
    else
    {
      return 1;
    }
  }

  /*
   * Send the request to the root PD.
   */
  nQDIResult = qurt_qdi_handle_invoke(qdss_clock_UserCtxt.nQDIHandle, 
                                      QDSS_CLOCK_QDI_SET,
                                      on_off);

  if(nQDIResult != 0)
  {
    return 2;
  }
  return 0;
}

int QDSSLocalClkOn(void)
{
   return QDSSLocalClkSet(1);
}

int QDSSLocalClkOff(void)
{
   return 0;
}



