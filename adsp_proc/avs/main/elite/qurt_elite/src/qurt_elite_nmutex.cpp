/*========================================================================

*//** @file qurt_elite_nmutex.cpp
This file contains normal mutex utilities like initialization mutex, lock,
unlock, try lock and destroy mutex.

   Copyright (c) 2015, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_nmutex.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/08/15   mh      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_nmutex.h"
#include "qurt.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
/**
  Initializes a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex to initialize.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_nmutex_init(qurt_elite_nmutex_t *pqurt_elite_nmutex)
{
   qurt_mutex_init((qurt_mutex_t*)pqurt_elite_nmutex);
   //The desired attributes are the default attributes.
}

/**
  Locks a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex to lock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_nmutex_lock(qurt_elite_nmutex_t *pqurt_elite_nmutex)
{
   qurt_mutex_lock((qurt_mutex_t*) pqurt_elite_nmutex);
}

/**
  Attempts to lock a normal mutex. If the lock is already held, a failure is returned.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex to try locking.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
int qurt_elite_nmutex_try_lock(qurt_elite_nmutex_t *pqurt_elite_nmutex)
{
    return qurt_mutex_try_lock((qurt_mutex_t*) pqurt_elite_nmutex);
}

/**
  Unlocks a normal mutex.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex to unlock.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_nmutex_unlock(qurt_elite_nmutex_t *pqurt_elite_nmutex)
{
   qurt_mutex_unlock((qurt_mutex_t*) pqurt_elite_nmutex);
}

/**
  Destroys a normal mutex. This function must be called for each corresponding
  qurt_elite_nmutex_init() function to clean up all resources.

  @datatypes
  #qurt_elite_nmutex_t

  @param[in] pqurt_elite_nmutex   Pointer to the normal mutex to destroy.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
void qurt_elite_nmutex_destroy(qurt_elite_nmutex_t *pqurt_elite_nmutex)
{
    qurt_mutex_destroy((qurt_mutex_t*) pqurt_elite_nmutex);
}

/** @} */ /* end_addtogroup qurt_elite_nmutex */
