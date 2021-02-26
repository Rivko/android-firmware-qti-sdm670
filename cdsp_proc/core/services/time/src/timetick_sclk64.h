#ifndef TIMETICK_SCLK64_H
#define TIMETICK_SCLK64_H
/*=============================================================================

    T I M E   S E R V I C E   S C L K   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Time Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


EXTERNALIZED FUNCTIONS
  timetick_sclk64_get
    Read the SLEEP_XTAL_TIMETICK counter. Adds the rollover offset to provide 
  64 bit dynamic range.

  time_sclk_get
     Get the slow-clock based time_sclk

  timetick_sclk64_resync_task
     Task for using a rex timer for timetick_sclk64 resync

  timetick_sclk64_resync
     The 32kHz-based time-of-day is periodically resynced to system time, if
  available.  If it has not been re-synced for 1.5 days, the 32kHz counter
  will 'rollover', and time_sclk would jump backwards 1.5 days.
  To prevent this from occuring, this timer callback is used to resync
  time_sclk before the rollover will occur.

REGIONAL FUNCTIONS
  timetick_sclk64_init
    Initializes the timetick_sclk64 subsystem

INITIALIZATION AND SEQUENCING REQUIREMENTS
 timetick_sclk64 must be called


Copyright (c) 2009 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/services/time/src/timetick_sclk64.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/13/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"
#include "timer.h"

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION TIME_SCLK_GET                                                  

DESCRIPTION
  Get the slow-clock based time_sclk

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_sclk_get
(
  /* Output: Slow-clock based time_sclk in ts_timestamp format */
  time_type                       ts_val,
  /* Input: XO value to calculate time from, -1 to use present time */
  uint64                          xo_val
);



/*=============================================================================

FUNCTION TIMETICK_SCLK64_GET

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter. Adds the rollover offset to provide 
  64 bit dynamic range.

DEPENDENCIES
  TIMETICK_INIT_SCLK64 must be called first to initialize the timer.

RETURN VALUE
  64-bit tick count

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_sclk64_get( void );


/*=============================================================================

FUNCTION TIMETICK_SCLK64_INIT                                          REGIONAL

DESCRIPTION
  Initializes the timetick_sclk64 subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  Installs a timer to update base values before TimeTick rollover

=============================================================================*/

void timetick_sclk64_init( void );


#endif /* TIMETICK_SCLK64_H */

