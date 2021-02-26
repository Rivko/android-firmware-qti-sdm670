/*========================================================================

*//** @file qurt_elite_condvar.cpp
This file contains a utility to form a channel of a combination of up to
32 signals/queues/timers. Client can wait on any combination thereof and
be woken when any desired element is active.

   Copyright (c) 2014, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_condvar_v2.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/30/14   rbhatnk      Created file.

========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_condvar_v2.h"

/**
  Initializes a QuRT_Elite condtion variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  None. @newpage
 */
ADSPResult qurt_elite_condvar_create(qurt_elite_condvar_t **condition_var_ptr_ptr)
{
	qurt_elite_condvar_t *ptr = *condition_var_ptr_ptr;
	ptr = (qurt_elite_condvar_t*) qurt_elite_memory_malloc(
			sizeof(qurt_elite_condvar_t), QURT_ELITE_HEAP_DEFAULT);

	if (NULL == ptr) {
		*condition_var_ptr_ptr = NULL;
		return ADSP_ENOMEMORY;
	}

    qurt_cond_init (&ptr->condvar);

    *condition_var_ptr_ptr = ptr;

    return ADSP_EOK;
}


/**
  Deletes a condition variable object.

  @datatypes
  #qurt_elite_condvar_t

  @param[in] condition_var_ptr  Pointer to the QuRT_Elite condition variable.

  @return
  None.

  @dependencies
  The object must have been created and intialized before calling this
  function. @newpage
*/
void qurt_elite_condvar_delete(qurt_elite_condvar_t *condition_var_ptr)
{
   qurt_elite_condvar_destroy(condition_var_ptr);
   qurt_elite_memory_free(condition_var_ptr);
}
