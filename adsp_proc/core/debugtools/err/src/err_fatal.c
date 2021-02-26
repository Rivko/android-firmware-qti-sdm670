/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides ERR_FATAL and its dependecies. Can be used
  to island mode directly

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_fatal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15    ip     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "qurt_memory.h"
#include "qurt_event.h"
#include "erri.h"
#include "tms_utils.h"
#include <stdlib.h>
#include <stringl.h>


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/

static void err_reentrancy_violated(void) ERR_NORETURN_ATTRIBUTE;
static void err_minimal_logging (const err_const_type* const_blk, uint32 code1, 
		         uint32 code2, uint32 code3);



/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void dog_force_bite(void);
extern void dog_force_kick(void);
/* from err_jettison_core.s */
extern void err_halt_execution(void);
extern void err_fatal_lock_init_check(void);

extern void err_emergency_error_recovery(void) ERR_NORETURN_ATTRIBUTE;
extern void err_raise_to_kernel(void);
extern void err_update_reentrancy_flag(void);

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
/* Used to replace all ERR_FATAL message strings.
 * Reduces code size by eliminating other const strings used for errors.
 */
const char err_generic_msg[] = "Error Fatal, parameters: %d %d %d";
const char err_generic_msg_dynamic[] = "Error Fatal, check coredump.err.aux_msg";
#endif

  
err_fatal_params_type err_fatal_params;

/* Ptr used by assembly routines to grab registers */
/*  (update this as needed if struct changes)      */
arch_coredump_type* arch_coredump_ptr = 
                          (arch_coredump_type*)(&err_fatal_params.array);


/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_FATAL_LOCK
DESCRIPTION
  Gets mutex for err_fatal to prevent multiple and/or cascading errors
============================================================================*/
void err_fatal_lock(void)
{
  err_fatal_lock_init_check();
  
  if(err_fatal_params.err_fatal_mutex_init==TRUE)
  {
      ERR_MUTEX_LOCK(&err_fatal_params.err_fatal_mutex);

      /* Mutex does not prevent the same thread being routed back 
       * into err_fatal by a bad callback */
      if(err_fatal_params.err_reentrancy_flag)
      {
        /* Does not return */
        err_reentrancy_violated();
      }
      else
      {
        err_fatal_params.err_reentrancy_flag = TRUE;
      }
    }
  else
  {
    /* If not intialized then this is an early ERR_FATAL */
    /* Proceed anyway so it can be handled */
  }

  /* Kick Dog */
  dog_force_kick();

} /* err_fatal_lock */

/*===========================================================================

FUNCTION       err_Fatal_internal

DESCRIPTION
  Do not call any of these functions directly.  They should be accessed by
  macros defined in err.h exclusively.
  Code is expected to on run in uImage as well.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void err_Fatal_internal3 
(
  const err_const_type* const_blk, 
  uint32                code1, 
  uint32                code2, 
  uint32                code3
)
{
 /* Enter critical section */
 err_fatal_lock();
 
 /* Capture registers */
 jettison_core();
 
 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_PRE);
 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_JETTISON_CORE_POST);

 /* Capture minimal log, Updates err_fatal_params */
 err_minimal_logging( const_blk, code1, code2, code3 );

 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_RAISE_EXCEPTION_PRE);

 /*  Raise an exception to qurt */
 qurt_exception_raise_nonfatal( ERR_RAISE_EXCEPTION_ARG(ERR_TYPE_ERR_FATAL) );

 /* Must not reach here */
 dog_force_bite();

 while(1);

}

#ifdef __HEXMSGABI_2_SUPPORTED__
/* Definition */
DLL_API_GLOBAL void err_Fatal_internal3_pcrelR0 
(
  const err_const_type* const_blk,
  uint32                code1,
  uint32                code2,
  uint32                code3
)
{
 asm("{r0 = memw(r31++#4); jump err_Fatal_internal3}");
}
#endif

void err_Fatal_internal2 (const err_const_type* const_blk, uint32 code1, uint32 code2)
{
  err_Fatal_internal3(const_blk, code1, code2, 0 );
}

#ifdef __HEXMSGABI_2_SUPPORTED__
/* Definition */
DLL_API_GLOBAL void err_Fatal_internal2_pcrelR0  
(
  const err_const_type* const_blk,
  uint32                code1,
  uint32                code2
)
{
 asm("{r0 = memw(r31++#4); jump err_Fatal_internal2}");
}
#endif

void err_Fatal_internal1 (const err_const_type* const_blk, uint32 code1)
{
  err_Fatal_internal3(const_blk, code1, 0, 0 );
}

#ifdef __HEXMSGABI_2_SUPPORTED__
/* Definition */
DLL_API_GLOBAL void err_Fatal_internal1_pcrelR0 
(
  const err_const_type* const_blk,
  uint32                code1
)
{
 asm("{r0 = memw(r31++#4); jump err_Fatal_internal1}");
}
#endif

void err_Fatal_internal0 (const err_const_type* const_blk)
{
  err_Fatal_internal3(const_blk, 0, 0, 0 );
}

#ifdef __HEXMSGABI_2_SUPPORTED__
/* Definition */
DLL_API_GLOBAL void err_Fatal_internal0_pcrelR0 
(
  const err_const_type* const_blk
)
{
 asm("{r0 = memw(r31++#4); jump err_Fatal_internal0}");
}
#endif

/*===========================================================================

MACRO ERR_FATAL_AUX_MSG

DESCRIPTION
  Accepts a backward compatible, zero-parameter ERR_FATAL string, as well
  as an additional string to fill a dynamic message buffer
  which is stored in the coredump.err.aux_msg.

NOTE
  The SFR created by this message is parsed by some HLOS's/customers.
  As such format maybe important.  Please consult with owners before
  using.

DEPENDENCIES
  None

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  Terminates processing, dumps core

===========================================================================*/

void err_Fatal_aux_msg 
(
  const err_const_type* const_blk, 
  const char * dynamic_msg,
  uint16 size_dynamic_msg
)
{

 /* Enter critical section */
 err_fatal_lock();
 
 /* Capture registers */
 jettison_core();
 
 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_PRE);
 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_JETTISON_CORE_POST);
 
 /* Capture minimal log, Updates err_fatal_params */
 err_minimal_logging( const_blk, 0, 0, 0 );

 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_DYNAMIC_CALLBACK_PRE);
 
 /* Copy to relevant structures */
 memscpy(err_fatal_params.aux_msg, sizeof(err_fatal_params.aux_msg), 
	dynamic_msg, size_dynamic_msg );

 /* Guarantee null termination */
 err_fatal_params.aux_msg[ERR_LOG_MAX_MSG_LEN-1]=0;

 ERR_CRUMB_TRAIL_BMSK_ISLAND(ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_RAISE_EXCEPTION_PRE);

 /*  Raise an exception to qurt */
 qurt_exception_raise_nonfatal( ERR_RAISE_EXCEPTION_ARG(ERR_TYPE_ERR_FATAL) );

 /* Must not reach here */
 dog_force_bite();

 while(1);

} /* err_Fatal_aux_msg */

/*=========================================================================

FUNCTION err_reentrancy_violated

DESCRIPTION
  This will only be called when ERR_FATAL is called while processing an
  ERR_FATAL.  It usually means that somone has registered a non-compliant
  callback function using 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_reentrancy_violated(void)
{
  if ( qurt_island_get_status() == FALSE )
  {
    err_update_reentrancy_flag();
  }

  err_emergency_error_recovery();
} /* err_reentrancy_violated */

/*===========================================================================

FUNCTION       err_minimal_logging

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_minimal_logging 
(
  const err_const_type* const_blk, 
  uint32                code1, 
  uint32                code2, 
  uint32                code3
)
{
   err_fatal_params.param1 = code1;
   err_fatal_params.param2 = code2;
   err_fatal_params.param3 = code3;
   err_fatal_params.msg_const_ptr = const_blk;

} /* err_minimal_logging*/

