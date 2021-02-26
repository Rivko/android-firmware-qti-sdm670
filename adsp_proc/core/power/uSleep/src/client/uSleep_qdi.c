/*============================================================================== 
  FILE:         uSleep_qdi.c

  OVERVIEW:     Implements QDI layer for uSleep API's in island mode

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/client/uSleep_qdi.c#2 $
$DateTime: 2017/09/13 08:52:52 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep.h"
#include "uSleepi.h"
#include "uSleep_util.h"
#include "uSleep_qdi.h"
#include "client_os.h"
#include "qurt.h"

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/* Client QDI opener object */
uSleep_qdi_data g_uSleepClientObj[USLEEP_SUPPORTED_USER_PDS] = {0};

/* Data structure required for QDI init */
const uSleep_qdi_opener g_uSleepOpenerData = 
{
  {uSleep_QDIInvoke, QDI_REFCNT_PERM, 0} 
};

/*==============================================================================
                           INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleep_QDIExit
 * 
 * @brief QDI interface function to exit uImage mode and return to normal 
 *        operational mode 
 *  
 * @return Status from function call (see function for values) 
 */
static inline uint32 uSleep_QDIExit(int client_handle)
{
  return(uSleep_exit());
}

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * uSleep_QDIInvoke
 */
int uSleep_QDIInvoke(int             client_handle,
                     qurt_qdi_obj_t  *pobj,
                     int             method,
                     qurt_qdi_arg_t  a1,
                     qurt_qdi_arg_t  a2,
                     qurt_qdi_arg_t  a3,
                     qurt_qdi_arg_t  a4,
                     qurt_qdi_arg_t  a5,
                     qurt_qdi_arg_t  a6,
                     qurt_qdi_arg_t  a7,
                     qurt_qdi_arg_t  a8,
                     qurt_qdi_arg_t  a9)
{
  int64           temp;
  uSleep_qdi_data *clntobj = (uSleep_qdi_data *)pobj;
   
  switch(method)
  {
    case QDI_ISLAND_CHECK_ALLOC:
      return TRUE;
      
    case QDI_OPEN:
      return uSleep_QDIOpen(client_handle, clntobj);

    case USLEEP_QDI_EXIT:
      return uSleep_QDIExit(client_handle);

    case USLEEP_QDI_INSERT_CALLBACK:
      /* Only one signal per PD, so it should be null if inserting here */
      CORE_VERIFY(NULL == clntobj->client_signals);
      clntobj->client_signals = (uSleep_signals *)a1.ptr;

      return uSleep_QDIInsertCallback(client_handle, clntobj->client_signals);

    case USLEEP_QDI_ADD_CALLBACK_LATENCY:
      temp = (int64)clntobj->enterLat + (int32)a1.num;
      CORE_VERIFY((temp >= 0) && (temp < 0x7ffffff));
      clntobj->enterLat += (int32)a1.num;

      temp = (int64)clntobj->exitLat + (int32)a2.num;
      CORE_VERIFY((temp >= 0) && (temp < 0x7ffffff));
      clntobj->exitLat  += (int32)a2.num;

      return uSleep_QDIAddCallbackLatency(client_handle,
                                          (int32)a1.num,
                                          (int32)a2.num);

    case USLEEP_QDI_RESET_TRANS_STATS:
      return uSleep_QDIResetTransStats(client_handle);

    case QDI_CLOSE:
    default:
      return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }
}

