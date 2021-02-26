/*========================================================================

*//** @file qurt_elite_channel.cpp
This file contains a utility to form a channel of a combination of up to
32 signals/queues/timers. Client can wait on any combination thereof and
be woken when any desired element is active.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_mutex.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   mwc      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt.h"
#include "qurt_pimutex.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
/**
  Initializes a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to initialize.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_mutex_init(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_init((qurt_mutex_t*)pqurt_elite_mutex);
   //The desired attributes are the default attributes.
}

/**
  Locks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to lock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_mutex_lock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_lock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Attempts to lock a mutex. If the lock is already held, a failure is returned.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to try locking.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
int qurt_elite_mutex_try_lock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
    return qurt_pimutex_try_lock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Unlocks a mutex. Recursive mutexes are always used.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to unlock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_mutex_unlock(qurt_elite_mutex_t *pqurt_elite_mutex)
{
   qurt_pimutex_unlock((qurt_mutex_t*) pqurt_elite_mutex);
}

/**
  Detroys a mutex. This function must be called for each corresponding
  qurt_elite_mutex_init() function to clean up all resources.

  @datatypes
  #qurt_elite_mutex_t

  @param[in] pqurt_elite_mutex   Pointer to the mutex to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_mutex_destroy(qurt_elite_mutex_t *pqurt_elite_mutex)
{
    qurt_pimutex_destroy((qurt_mutex_t*) pqurt_elite_mutex);
}

/** @} */ /* end_addtogroup qurt_elite_mutex */
