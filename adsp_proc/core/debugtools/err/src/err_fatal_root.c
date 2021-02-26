/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides ERR_FATAL and its dependecies. This file must be island 
  mode safe.

Copyright (c) 2015,16,18 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_fatal_root.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "erri.h"
#include "tms_utils.h"

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_emergency_error_recovery(void) ERR_NORETURN_ATTRIBUTE;
void err_raise_to_kernel(void) ERR_NORETURN_ATTRIBUTE;
void err_fatal_lock_init_check(void);

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
/* from err_jettison_core.s */
extern void err_halt_execution( void ) ERR_NORETURN_ATTRIBUTE;

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/

  
extern err_fatal_params_type err_fatal_params;

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_FATAL_LOCK_INIT_CHECK

DESCRIPTION
  Called by err_fatal_lock
============================================================================*/
void err_fatal_lock_init_check(void)
{
} /* err_fatal_lock_init_check */


/*=========================================================================

FUNCTION err_emergency_error_recovery

DESCRIPTION
  Action to be taken when more than one error has occurred, or if an
  error occurs before err_init has completed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  No return from this function

===========================================================================*/
void err_emergency_error_recovery( void )
{
  /* Define action to be taken when multiple crashes occur */
  if ( qurt_island_get_status() == FALSE ) 
    ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_EMERGENCY_RECOVERY_PRE);
  else
    ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_EMERGENCY_RECOVERY_PRE);

  /* flush cache, etc - does not return*/
  err_raise_to_kernel();

} /* err_emergency_error_recovery */

/*=========================================================================

FUNCTION err_raise_to_kernel

DESCRIPTION
  Function which will terminate user space/PD handling and raise to kernel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_raise_to_kernel(void)
{
  if ( qurt_island_get_status() == FALSE )
  { 
    ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_RAISE_TO_KERNEL_PRE);
    tms_get_timetick(&(coredump.err.err_handler_end_time));
    /* Explicitly flushing it */
    ERR_FLUSH_ADDR(coredump.err.err_handler_end_time);
  }
  else
    ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_EMERGENCY_RECOVERY_PRE);

  #ifdef ERR_FATAL_FLUSH_CACHE_NO_RETURN
    ERR_FATAL_FLUSH_CACHE_NO_RETURN();
  #else
    #warning ERR_FATAL_FLUSH_CACHE_NO_RETURN not defined
  #endif

  /* Stop active thread - will not reach here*/
  err_halt_execution();

} /* err_raise_to_kernel */ 


