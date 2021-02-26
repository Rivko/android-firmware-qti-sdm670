#ifndef CORE_TIME_H
#define CORE_TIME_H

/*==============================================================================
  @file CoreTime.h

  Time related Function/Structure Declarations

  Copyright (c) 2009-2014  Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifdef USES_BLAST 
#include "blast.h"
#include "atomic_ops.h"
#include "qube.h"
#include "qtimer.h"
#elif (defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION))
#include "windows.h"
#elif defined(TARGET_UEFI)
#include <Library/TimerLib.h>
#include "com_dtypes.h"
extern uint32 UEFITimeGet(void);
extern uint64 UEFITimeGet64(void);
#else
#include "timetick.h"
#endif 

/**
   @brief CoreTimetick_Get - Get the current system
          timetick.
   
   Read the 32 khz timetick value, and return it.

   @return Current 32 KHz timetick
*/
#ifdef TARGET_UEFI
#define CoreTimetick_Get             UEFITimeGet

/**
   @brief CoreTimetick_Get64 - Get the current system
          time as 19.2MHz 64-bit data-type
   @return Current QTimer System time (19.2 MHz tick-rate)
*/
#define CoreTimetick_Get64           UEFITimeGet64
#elif defined USES_BLAST 
#define CoreTimetick_Get             blast_system_sclk_attr_gethwticks
#elif (defined (WINSIM) || defined (WIN_DEBUGGER_EXTENSION))
#define CoreTimetick_Get             GetTickCount
#else
#define CoreTimetick_Get             timetick_get_safe
#endif


#endif /* CORE_TIME_H */
