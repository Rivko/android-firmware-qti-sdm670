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

  $Header: //components/rel/core.qdsp6/2.1/debugtools/task/src/coremain.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "err.h"

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

void bootstrap_rcinit_task(void); // forward reference

/**
API, coremain_main
@return None.
*/
void coremain_main(void)
{
   bootstrap_rcinit_task(); // RCINIT Framework, Returns (following successful init completion)

   err_exception_handler(); // REX API Only, Call exception handler, does not return
}

#if !defined(TASK_EXCLUDE_MAIN)
/**
API, main
@return None.
*/
int main(void)
{
   coremain_main(); // when main is not delivered here, this is the entry to start cbsp

   /* Not expected to return */
   return (0);
}
#endif

