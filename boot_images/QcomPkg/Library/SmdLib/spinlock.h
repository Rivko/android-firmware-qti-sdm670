
/*---------------------------------------------------------------------------
   SPINLOCK.H  - Spinlock API
---------------------------------------------------------------------------*/
/**
  @file spinlock.h
*/
/* Definitions for a couple well know spin lock handles/keys. The defini- 
   tions for spin lock API can be compiled away to nothing if FEATURE_SPINLOCK
   is not defined.
*/
/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMD_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMD_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smd" group description 
         in the SMD_mainpage file. 
*/

/*--------------------------------------------------------------------------
Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/18/12   bt      Clean up header inclusion in spinlock files.
01/05/12   hwu     Decouple spinlock from SMEM internal header.
06/28/11   rs      Added function to force clear spinlocks owned by the 
                   specified host, required to support SSR Phase 2 
04/04/11   bt      Reserved SMEM_SPINLOCK_STATIC_LOG to remove Static log.
01/31/11   tl      Fixed extern "C" definition for C++ inclusion
01/25/11   rs      Initial revision (based on spinlockarm.h).
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "smem_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/* "Well known" spinlock keys/handles.
 * Add your well known key here if you 
 * need to spinlock something.
 */
#define SMEM_SPINLOCK_BIO            0
#define SMEM_SPINLOCK_SMSM           1
#define SMEM_SPINLOCK_SMEM_LOG       2
#define SMEM_SPINLOCK_SMEM_ALLOC     3
#define SMEM_SPINLOCK_TOUCHSCREEN    4
#define SMEM_SPINLOCK_RESERVED_5     5
#define SMEM_SPINLOCK_I2C            6
#define SMEM_SPINLOCK_CLKREGIM       7

/* Any change here needs a SMEM version update since it changed the memory map */
#define SPINLOCK_NUM_LOCKS           8

/*===========================================================================
    Defines for backwards compatibility. These defines should be removed
    when all clients have been switched to the new names.
===========================================================================*/
#define SPINLOCK_BIO            SMEM_SPINLOCK_BIO
#define SPINLOCK_TOUCHSCREEN    SMEM_SPINLOCK_TOUCHSCREEN

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_spin_lock_init
===========================================================================*/
/**
  Initializes spinlocks.
 
  Spinlocks are used to acquire exclusive access to resources shared by
  all processors in the system.
 
  @param[in] spin_lock_array    Pointer to spinlocks array in SMEM.
  @param[in] int_lock_array     Pointer to intlocks array in SMEM (unused).
  @param[in] num_int_locks      Number of spinlocks.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Initializes OS specific mutual exclusion mechanisms.
*/
/*=========================================================================*/
void smem_spin_lock_init( 
       volatile int *spin_lock_array,
       int *int_lock_array,
       int num_int_locks );

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
void smem_spin_lock( int lock );

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
void smem_spin_unlock( int lock );

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
void smem_spin_locks_clear( smem_host_type host );

#ifdef __cplusplus
};
#endif

#endif /* SPINLOCK_H */

