#ifndef DDR_PROFILER_H
#define DDR_PROFILER_H

/*===========================================================================

                                ddr profiler 
                                Header File

GENERAL DESCRIPTION
  This header file contains macro declarations and definitions for 
  linking to Boot Profiler functionality.
  Time unit in boot profiler is microsecond(10^-6 second).

Copyright  2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   --------     ------------------------------------------------------
03/24/15   plc          Initial revision
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_profiler.h"
#include "ddr_log.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef DDR_PROFILER_FEATURE
  #define boot_profiler_ddr_message(xxlevel, xxstring, ...) boot_profiler_message(xxlevel, xxstring, ##__VA_ARGS__)
  #define boot_profiler_ddr_start_timer(xxlevel, xxstring, ...) boot_profiler_start_timer(xxlevel, xxstring, ##__VA_ARGS__)
  #define boot_profiler_ddr_stop_timer(xxlevel, xxstring, ...) boot_profiler_stop_timer(xxlevel, xxstring, ##__VA_ARGS__)

  #ifdef BOOT_PROFILER_FEATURE
    #define DDR_PROFILER_EN_NORM_LVL
  #endif

#else
  #define boot_profiler_ddr_message(xxlevel, xxstring, ...) ddr_logf(xxlevel, xxstring, ##__VA_ARGS__)
  #define boot_profiler_ddr_start_timer(xxlevel, xxstring, ...)
  #define boot_profiler_ddr_stop_timer(xxlevel, xxstring, ...)  
#endif

  
#endif  /* DDR_PROFILER_H */
