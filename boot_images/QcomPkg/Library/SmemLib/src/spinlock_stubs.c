/*=============================================================================

                          SMEM SPINLOCK STUBS

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/spinlock_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"
#include "spinlock.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  <function>
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*===========================================================================
FUNCTION      smem_spin_lock_init
===========================================================================*/
/**
  Initializes spinlocks.
 
  Spinlocks are used to acquire exclusive access to resources shared by
  all processors in the system.
 
  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Initializes OS specific mutual exclusion mechanisms.
*/
/*=========================================================================*/
void smem_spin_lock_init(void)
{
  /* Spinlocks are not implemented in boot, under the assumption that no other
   * processors are running concurrently. */
}

/*===========================================================================
FUNCTION      smem_spin_lock
===========================================================================*/
/**
  Acquires a spinlock as indicated by input integer, protected by non-
  preemtable critical section (effectively locking interrupts).
 
  @param[in] lock    Spinlock identifier.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Prevents context switch (via critical section) and causes remote
  processors to spin if they try to use the same resource (via spinlock).
*/
/*=========================================================================*/
void smem_spin_lock(uint8 lock)
{
  /* Spinlocks are not implemented in boot, under the assumption that no other
   * processors are running concurrently. */
}

/*===========================================================================
FUNCTION      smem_spin_unlock
===========================================================================*/
/**
  Releases a spin lock as indicated by input integer, protected by non-
  preemtable critical section (effectively unlocking interrupts).
 
  @param[in] lock    Spinlock identifier.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Allows other threads (via critical section) and remote processors (via
  spinlock) to acquire the shared resource.
*/
/*=========================================================================*/
void smem_spin_unlock(uint8 lock)
{
  /* Spinlocks are not implemented in boot, under the assumption that no other
   * processors are running concurrently. */
}

/*===========================================================================
  FUNCTION  smem_clear_spin_locks
===========================================================================*/
/**
  This function unlocks all spinlocks held by the specified host.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] host      Host identifier.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void smem_spin_locks_clear(smem_host_type host)
{
  /* Spinlocks are not implemented in boot, under the assumption that no other
   * processors are running concurrently. */
}
