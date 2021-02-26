/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Main procedure for the AMSS Mobile software.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dspcore.adsp/4.0/main/task/src/coremain.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


/*===========================================================================
FUNCTION MAIN

DESCRIPTION
  This procedure uses the rex_init procedure to bootstrap tasking
  This procedure context is then reused as the err_exception_handler

DEPENDENCIES
  None

RETURN VALUE
  None, this routine does not return

SIDE EFFECTS
  None
===========================================================================*/

#if !defined(TASK_EXCLUDE_MAIN)
/**
API, main
@return None.
*/

#include "qurt.h"

QURT_DECLARE_STATIC_HEAP(0x100000);    // Declare a 1MB heap

//Declaration to make the dummy call to memalign 
void *memalign(int, int);

int main(void)
{
   /* Wait forever.*/
   unsigned dummy=0;
   //dummy call to memalign to link core library with memory APIs
   void *ptr=memalign(128, 0x600000);
   for(;;)
      qurt_futex_wait(&dummy,dummy);
   return (0);
}
#endif

