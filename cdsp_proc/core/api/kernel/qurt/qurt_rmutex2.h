#ifndef QURT_RMUTEX2_H
#define QURT_RMUTEX2_H
/**
  @file qurt_rmutex2.h 
  @brief Prototypes of rmutex2 API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


#include <qurt_futex.h>
#include <qurt_mutex.h>

/** @addtogroup mutex_types
@{ */
/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** QuRT rmutex2 type.                                       
  
   Mutex type used with rmutex2 APIs.
 */
typedef struct {
   /** @cond */
   unsigned int holder __attribute__((aligned(8)));    /* UGP value of mutex holder */
   unsigned short waiters;                             /* Number of threads that are waiting */
   unsigned short refs;                                /* Number of references to this mutex */
   unsigned int queue;                                 /* Kernel-maintained "futex queue" value */
   unsigned int excess_locks;                          /* Number of "excess" times the current holder has locked the mutex */
   /** @endcond */
} qurt_rmutex2_t;
/** @} */ /* end_addtogroup mutex_types */

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/*@ingroup func_qurt_rmutex2_init
   Initializes a recursive mutex object. 
   The recursive mutex is initially unlocked.
  
   Objects of type rmutex2 solve a potential race condition between
   unlock() and destroy() operations.

   @datatypes
   #qurt_rmutex2_t
   
   @param[out]  lock  Pointer to the recursive mutex object.

   @return 
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex2_init(qurt_rmutex2_t *lock);

/*@ingroup func_qurt_rmutex2_destroy
  Destroys the specified recursive mutex. \n
  @note1cont Recursive mutexes must not be destroyed while they are still in use. If this
             happens the behavior of QuRT is undefined. 
  @note1cont In general, application code should destroy an rmutex2 object prior to
             deallocating it; calling qurt_rmutex2_destroy() before deallocating it ensures
             that all qurt_rmutex2_unlock() calls have completed.
  
  @datatypes
  #qurt_rmutex2_t
  
  @param[in]  lock  Pointer to the recursive mutex object to destroy.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_rmutex2_destroy(qurt_rmutex2_t *lock);

/*@ingroup func_qurt_rmutex2_lock
  Locks the specified recursive mutex. \n 

  If a thread performs a lock operation on a recursive mutex that is not being used, the
  thread gains access to the shared resource that is protected by the mutex, and continues
  executing.

  If a thread performs a lock operation on a recursive mutex that is already being used by
  another thread, the thread is suspended. When the mutex becomes available again
  (because the other thread has unlocked it), the thread is awakened and given access to the
  shared resource.
  
  @note1hang A thread is not suspended if it locks a recursive mutex that it has already
             locked, but the mutex does not become available until the thread performs a
             balanced number of unlocks on the mutex.
  
   @datatypes
   #qurt_rmutex2_t
  
   @param[in]  lock  Pointer to the recursive mutex object to lock. 

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex2_lock(qurt_rmutex2_t *lock);

/*@ingroup func_qurt_rmutex2_unlock
   Unlocks the specified recursive mutex. \n 
   More than one thread can be suspended on a recursive mutex. When the mutex is
   unlocked, only the highest-priority thread waiting on the mutex is awakened. If the
   awakened thread has higher priority than the current thread, a context switch occurs.
  
   @datatypes
   #qurt_rmutex2_t
  
   @param[in]  lock  Pointer to the recursive mutex object to unlock. 

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_rmutex2_unlock(qurt_rmutex2_t *lock);

/*@ingroup func_qurt_rmutex2_try_lock
   Attempts to lock the specified recursive mutex.\n

   Non-blocking version of qurt_rmutex2_lock().  If a call to qurt_rmutex2_lock() would
   succeed immediately, this function behaves similarly, and returns 0 for success.
   If a call to qurt_rmutex2_lock() would not succeed immediately, this function has
   no effect and returns non-zero for failure.

   @datatypes
   #qurt_rmutex2_t
   
   @param[in]  lock  Pointer to the recursive mutex object to lock.

   @return 
   0 -- Success. \n 
   Nonzero -- Failure. 
  
 */
int qurt_rmutex2_try_lock(qurt_rmutex2_t *lock);

#endif /* QURT_RMUTEX2_H */
