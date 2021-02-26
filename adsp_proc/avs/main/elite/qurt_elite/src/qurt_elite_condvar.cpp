/*========================================================================

*//** @file qurt_elite_condvar.cpp
This file contains a utility to form a channel of a combination of up to
32 signals/queues/timers. Client can wait on any combination thereof and
be woken when any desired element is active.

   Copyright (c) 2014-2015, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_condvar.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/10/15   mh      Updated dependencies info for conditional variable wait
                   API
10/30/14   rbhatnk Created file.

========================================================================== */
#include "qurt_elite_condvar.h"

/**
  Initializes a QuRT_Elite condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  None. @newpage
 */
void qurt_elite_condvar_init(qurt_elite_condvar_t *condition_var_ptr)
{
     qurt_cond_init (&condition_var_ptr->condvar);
}


/**
  Signals a condition variable object. This utility is used to awaken a single
  waiting thread.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
 */
void qurt_elite_condvar_signal(qurt_elite_condvar_t *condition_var_ptr)
{
    qurt_cond_signal(&condition_var_ptr->condvar);
}


/**
  Broadcasts a condition variable object. This utility is used to awaken
  multiple threads waiting for a condition variable.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
 */
void qurt_elite_condvar_broadcast(qurt_elite_condvar_t *condition_var_ptr)
{
    qurt_cond_broadcast(&condition_var_ptr->condvar);

}

/**
  Waits for a condition variable object. This utility suspends the current
  thread until the specified condition is true.

  @datatypes
  #qurt_elite_condvar_t \n
  #qurt_elite_nmutex_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.
  @param[in] nmutex             Normal Mutex associated with the condition
                                variable.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function.
 */
void qurt_elite_condvar_wait(qurt_elite_condvar_t *condition_var_ptr, qurt_elite_nmutex_t *nmutex)
{
    qurt_cond_wait(&condition_var_ptr->condvar,(qurt_mutex_t*)nmutex);
}

/**
  Destroys a condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
void qurt_elite_condvar_destroy(qurt_elite_condvar_t *condition_var_ptr)
{
   qurt_cond_destroy(&condition_var_ptr->condvar);
}
