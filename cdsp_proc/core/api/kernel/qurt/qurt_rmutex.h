#ifndef QURT_RMUTEX_H
#define QURT_RMUTEX_H
/**
  @file qurt_rmutex.h 
  @brief Prototypes of rmutex API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


#include <qurt_futex.h>
#include <qurt_mutex.h>

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_rmutex_init
   Initializes a recursive mutex object.
   The recursive mutex is initially unlocked.

   @note1hang Each rmutex-based object has one or more kernel resources associated with it;
              to prevent resource leaks, be sure to call qurt_rmutex_destroy()
              when this object is not used anymore

   @datatypes
   #qurt_mutex_t
   
   @param[out]  lock  Pointer to the recursive mutex object.

   @return 
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex_init(qurt_mutex_t *lock);

/**@ingroup func_qurt_rmutex_destroy  
  Destroys the specified recursive mutex. \n
  @note1hang Recursive mutexes must be destroyed when they are no longer in use. Failure
             to do this causes resource leaks in the QuRT kernel.\n
  @note1cont Recursive mutexes must not be destroyed while they are still in use. If this
             happens the behavior of QuRT is undefined. 
  
  @datatypes
  #qurt_mutex_t
  
  @param[in]  lock  Pointer to the recursive mutex object to destroy.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_rmutex_destroy(qurt_mutex_t *lock);

/**@ingroup func_qurt_rmutex_lock
  Locks the specified recursive mutex. \n 

  If a thread performs a lock operation on a mutex that is not being used, the thread 
  gains access to the shared resource that is protected by the mutex, and continues executing.

  If a thread performs a lock operation on a mutex that is already being used by another 
  thread, the thread is suspended. When the mutex becomes available again (because the 
  other thread has unlocked it), the thread is awakened and given access to the shared resource.
  
   @note1hang A thread is not suspended if it locks a recursive mutex that it has already 
   locked by itself. However, the mutex does not become available to other threads until the
   thread performs a balanced number of unlocks on the mutex.

   @datatypes
   #qurt_mutex_t
  
   @param[in]  lock  Pointer to the recursive mutex object to lock. 

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex_lock(qurt_mutex_t *lock);

/**@ingroup func_qurt_rmutex_unlock
   Unlocks the specified recursive mutex. \n 
   More than one thread can be suspended on a mutex. When the mutex is 
   unlocked, the thread waiting on the mutex is awakened. If the awakened
   thread has higher priority than the current thread, a context switch occurs.

   @note1hang When a thread unlocks a recursive mutex, the mutex is not available until 
   the balanced number of locks and unlocks has been performed on the mutex.

   @datatypes
   #qurt_mutex_t
  
   @param[in]  lock  Pointer to the recursive mutex object to unlock. 

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex_unlock(qurt_mutex_t *lock);

/**@ingroup func_qurt_rmutex_try_lock
   Attempts to lock the specified recursive mutex.\n

   If a thread performs a try_lock operation on a recursive mutex that is not being used, the
   thread gains access to the shared resource that is protected by the mutex, and continues
   executing.\n
   If a thread performs a try_lock operation on a recursive mutex that is already being used
   by another thread, qurt_rmutex_try_lock immediately returns with a nonzero result
   value.

   @note1hang If a thread performs a try_lock operation on a mutex that it has already locked,
              qurt_mutex_try_lock immediately returns with a nonzero result value.
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the recursive mutex object to lock.

   @return 
   0 -- Success. \n 
   Nonzero -- Failure. 
  
 */
int qurt_rmutex_try_lock(qurt_mutex_t *lock);

/**
  Attempts to lock a mutex object recursively. If the mutex is available, 
  it grabs the mutex. If the mutex is held by the current thread, 
  it increases the internal counter and returns 0. If not, it returns a
  nonzero value.
 
  @datatypes
  #qurt_mutex_t
  
  @param  lock  Pointer to the qurt_mutex_t object. 

  @return
  0 -- Success. \n
  Nonzero -- Failure. 

  @dependencies
  None.
 */
int qurt_rmutex_try_lock_block_once(qurt_mutex_t *lock);

#endif /* QURT_RMUTEX_H */
