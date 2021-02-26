/** @file BusyWaitLibNull.c
  
  Stub functions for BusyWaitLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/24/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "busywait.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*==========================================================================

  FUNCTION      BUSYWAIT_INIT

  DESCRIPTION   This function initializes the busywait functionality.

  PARAMETERS    none

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS if the initialization is successful
                and DAL_ERROR otherwise.

  SIDE EFFECTS  None.

==========================================================================*/
DALResult busywait_init (void)
{
  return DAL_SUCCESS;
}


/*==========================================================================

  FUNCTION      BUSYWAIT

  DESCRIPTION   This function pauses the execution of a thread for a
                specified time.

  PARAMETERS    pause_time_us - Time to pause in microseconds

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void busywait (uint32 pause_time_us)
{
}
