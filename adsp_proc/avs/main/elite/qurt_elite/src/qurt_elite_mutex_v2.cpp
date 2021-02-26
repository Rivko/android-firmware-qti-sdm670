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

 $Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_mutex_v2.cpp#1 $

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 10/30/14   rbhatnk      Created file.

 ========================================================================== */

/* =======================================================================
 INCLUDE FILES FOR MODULE
 ========================================================================== */
#include "qurt_elite.h"
#include "qurt.h"
#include "qurt_elite_mutex_v2.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
/**
 Create and initializes a mutex. Recursive mutexes are always used.

 @datatypes
 #qurt_elite_mutex_t

 @param[in] pqurt_elite_mutex   Pointer to the mutex to initialize.

 @return
 Error code.

 @dependencies
 None. @newpage
 */
ADSPResult qurt_elite_mutex_create(qurt_elite_mutex_t **ppqurt_elite_mutex)
{
	qurt_elite_mutex_t *pMutex = *ppqurt_elite_mutex;
	pMutex = (qurt_elite_mutex_t*) qurt_elite_memory_malloc(
			sizeof(qurt_elite_mutex_t), QURT_ELITE_HEAP_DEFAULT);

	if (NULL == pMutex) {
		*ppqurt_elite_mutex = NULL;
		return ADSP_ENOMEMORY;
	}

	qurt_pimutex_init((qurt_mutex_t*)pMutex);
	//The desired attributes are the default attributes.

	*ppqurt_elite_mutex = pMutex;

	return ADSP_EOK;
}

/**
 Deletes a mutex. This function must be called for each corresponding
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
void qurt_elite_mutex_delete(qurt_elite_mutex_t *pqurt_elite_mutex)
{
	qurt_pimutex_destroy((qurt_mutex_t*) pqurt_elite_mutex);
	qurt_elite_memory_free(pqurt_elite_mutex);
}

/** @} *//* end_addtogroup qurt_elite_mutex */
