/*==============================================================================

  G P I O   I N T E R R U P T   C O N T R O L L E R   L O G 

DESCRIPTION
  This modules contains the logging mechanism for GPIO Interrupt logs.

REFERENCES

    Copyright (c) 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/GPIOInt/src/kernel/GPIOIntLog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/15   cpaulo  First draft created. 
===========================================================================*/
#include "GPIOIntLog.h"
#include <stdarg.h>

#define GPIOINT_ULOG_SIZE 4096
#define GPIOINT_HEADER_SIZE 64

static uint32 nGPIOIntLogMask = 0;

/*===========================================================================
FUNCTION GPIOIntLog_Init
 
DESCRIPTION 
  This function will be called from GPIOInt_DeviceInit.  This function will
  initialize the uLog required for GPIOInt.
===========================================================================*/
void GPIOIntLog_Init( ULogHandle* hL )
{
  char sztGPIOIntHeader[] = "GPIOInt Log";

  ULogFront_RealTimeInit(hL,
                         "GPIOInt",
                         GPIOINT_ULOG_SIZE,
                         ULOG_MEMORY_LOCAL,
                         ULOG_LOCK_OS);

  if ( hL != NULL )
  {
    ULogCore_HeaderSet(*hL, sztGPIOIntHeader);
  }

  #ifdef GPIOINTLOG_REG_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_REG;
  #endif /* GPIOINTLOG_REG_ENABLE */

  #ifdef GPIOINTLOG_MPM_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_MPM;
  #endif /* GPIOINTLOG_MPM_ENABLE */

  #ifdef GPIOINTLOG_NPAREQ_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_NPAREQ;
  #endif /* GPIOINTLOG_NPAREQ_ENABLE */

  #ifdef GPIOINTLOG_SETTRIGGER_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_SETTRIGGER;
  #endif /* GPIOINTLOG_SETTRIGGER_ENABLE */

  #ifdef GPIOINTLOG_ENABLE_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_ENABLE;
  #endif /* GPIOINTLOG_ENABLE_ENABLE */

  #ifdef GPIOINTLOG_CLEAR_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_CLEAR;
  #endif /* GPIOINTLOG_CLEAR_ENABLE */

  #ifdef GPIOINTLOG_TRIGGER_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_TRIGGER;
  #endif /* GPIOINTLOG_TRIGGER_ENABLE */

  #ifdef GPIOINTLOG_SUMMARYISR_ENABLE
  nGPIOIntLogMask |= GPIOINTLOG_SUMMARYISR;
  #endif /* GPIOINTLOG_SUMMARYISR_ENABLE */
}

/*===========================================================================
FUNCTION GPIOIntLog_Out
 
DESCRIPTION 
  This function will be called to print a log to the buffer.  
===========================================================================*/
void GPIOIntLog_Out( uint32 nMask, ULogHandle hL, uint32 nArgs, const char* sztStr, ... )
{
  va_list pArgs;

  if ( nMask & nGPIOIntLogMask )
  {
    va_start(pArgs, sztStr);
    ULogFront_RealTimeVprintf(hL, nArgs, sztStr, pArgs);
    va_end(pArgs);
  }

  return;
}


