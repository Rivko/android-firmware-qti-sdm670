/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Island exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2014 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/uerr_exception_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/16   abh     Graceful exit in case of TLB miss exception.
07/31/14   din     Made some changes to support Error on User PD.
07/24/14   din     Changed Diag Macro to use uImage Diag Macro.
07/02/14   din     Accomodate Qurt argument change to complete ERR_FATAL call flow.
06/13/14   din     Moved uErr task to TCM using qurt apis.
04/01/14   din     Initial verion (pulled code from exception_handler.c)
===========================================================================*/

#include <stdio.h>
#include "erri.h"
#include "qurt_event.h"
#include "qurt.h"
#include "uSleep_mode_trans.h"

uint8 uerr_task_stack_arr[UERR_TASK_STACK_SIZE];
qurt_thread_t       uerrr_task_thread_id = {0};

extern void dog_force_bite( void);
extern void err_update_coredump_tid_and_is_exception(uint32 tid, boolean is_exception);

/*===========================================================================
FUNCTION uerr_exception_handler

DESCRIPTION
  1. It is suppossed to run in ADSP island low power mode.
  2. Brings system out of island mode when recoverable exception occurs.
  3. Brings system out of island mode when non fatal or user exception occurs 
     and raises non fatal exception to the kernel.

===========================================================================*/
void uerr_exception_handler
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  void *                           ignored    /*lint -esym(715,ignored) */
)
{
  unsigned int     tid, ip, sp, badva, cause;
   
  for (;;)
  {
    /* Register self as uException_Handler. */
    tid = qurt_island_exception_wait(&ip, &sp, &badva, &cause);

    if (-1==tid)
    {
      continue;
    }

    if (USLEEP_SUCCESS != uSleep_exit())
    {
      dog_force_bite();
    }

	/* Do not raise exception in case of TLB miss. */
    if ( (QURT_EXCEPT_INVALID_INT == cause) || 
         (QURT_EXCEPT_TLBMISS == cause) || 
         (QURT_EXCEPT_TLBMISS_X == cause) ||
         (QURT_EXCEPT_EXIT == cause) )
    {
      continue;
    }

    /* Update tid to coredump, harcoding is_exception to FALSE. 
    Should be correctly updated in expcetion handler */
    err_update_coredump_tid_and_is_exception(tid, FALSE);

    qurt_exception_raise_nonfatal( ERR_RAISE_EXCEPTION_ARG(ERR_TYPE_NON_RECOVERABLE_EXCEPTION) );
  } 
} /* end of uerr_exception_handler */



