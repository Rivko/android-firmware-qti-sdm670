/*==============================================================================

  D A L   I N T E R R U P T   C O N T R O L L E R   L O G 

DESCRIPTION
  This modules contains the logging mechanism for Interrupt Controller.

REFERENCES

    Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/InterruptController/src/qurt/DALInterruptControllerLog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   cpaulo  First draft created. 
===========================================================================*/
#include "InterruptControllerLog.h"
#include <stdarg.h>

#define INT_ULOG_SIZE 4096
#define INT_HEADER_SIZE 64

static uint32 nIntLogMask = 0;

/*===========================================================================
FUNCTION IntLog_Init
 
DESCRIPTION 
  This function will be called from InterruptController_DeviceInit.
  This function will initialize the uLog required for Interrupt Controller.
===========================================================================*/
void IntLog_Init( ULogHandle* hL )
{
  char sztIntHeader[] = "InterruptController Log";

  ULogFront_RealTimeInit(hL,
                         "InterruptController",
                         INT_ULOG_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  if ( hL != NULL )
  {
    ULogCore_HeaderSet(*hL, sztIntHeader);
  }

  #ifdef INTLOG_REG_ENABLE
  nIntLogMask |= INTLOG_REG;
  #endif /* INTLOG_REG_ENABLE */

  #ifdef INTLOG_PDC_ENABLE
  nIntLogMask |= INTLOG_PDC;
  #endif /* INTLOG_MPM_ENABLE */

  #ifdef INTLOG_SETTRIGGER_ENABLE
  nIntLogMask |= INTLOG_SETTRIGGER;
  #endif /* INTLOG_SETTRIGGER_ENABLE */

  #ifdef INTLOG_ENABLE_ENABLE
  nIntLogMask |= INTLOG_ENABLE;
  #endif /* INTLOG_ENABLE_ENABLE */

  #ifdef INTLOG_CLEAR_ENABLE
  nIntLogMask |= INTLOG_CLEAR;
  #endif /* INTLOG_CLEAR_ENABLE */

  #ifdef INTLOG_TRIGGER_ENABLE
  nIntLogMask |= INTLOG_TRIGGER;
  #endif /* INTLOG_TRIGGER_ENABLE */
}

/*===========================================================================
FUNCTION IntLog_Out
 
DESCRIPTION 
  This function will be called to print a log to the buffer.  
===========================================================================*/
void IntLog_Out( uint32 nMask, ULogHandle hL, uint32 nArgs, const char* sztStr, ... )
{
  va_list pArgs;

  if ( nMask & nIntLogMask )
  {
    va_start(pArgs, sztStr);
    ULogFront_RealTimeVprintf(hL, nArgs, sztStr, pArgs);
    va_end(pArgs);
  }

  return;
}


