/*=========================================================================

                         Busywait 

GENERAL DESCRIPTION
  This file contains the implementation of the busywait() function for
  hardware based blocking waits.

EXTERNALIZED FUNCTIONS
  busywait

INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/busywait/src/busywait.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $
  $Change: 13907704 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/23/11   pbitra     Initial version.

==========================================================================*/ 

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "busywait.h"


/*==========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==========================================================================*/

/*=========================================================================
      Constants and Macros 
==========================================================================*/

#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 19200

/*=========================================================================
      Typedefs
==========================================================================*/


/*=========================================================================
      Variables
==========================================================================*/

/*==========================================================================

                         FUNCTIONS FOR MODULE

==========================================================================*/

/*==========================================================================

  FUNCTION      BUSYWAIT_INIT

  DESCRIPTION   This function enables the Busywait Counter.

  PARAMETERS    None

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None

==========================================================================*/

void busywait_init(void)
{
}

/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None

  RETURN VALUE  None.

  SIDE EFFECTS  The Busywait counter will be enabled and left running to 
                support nested busywait calls, i.e. to support 
                multithreading and/or ISRs.

==========================================================================*/

void busywait (uint32 pause_time_us)
{
  uint64 start_count = 0;
  uint64 delay_count = 0;
  uint64 utimer = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  __asm__ volatile ("%[start_count] = UTIMER" : [start_count] "=r"(start_count));
  utimer = start_count;
  /*
   * Perform the delay and handle potential overflows of the timer.
   */

  delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ)/1000;
  while ((utimer - start_count) < delay_count)
  {
    __asm__ volatile ("pause(#50)");
    __asm__ volatile ("%[utimer] = UTIMER" : [utimer] "=r"(utimer));
  }
}

