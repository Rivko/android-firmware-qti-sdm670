/*========================================================================

 *//** @file qurt_elite_channel.cpp
This file contains a utility to form a channel of a combination of up to
32 signals/queues/timers. Client can wait on any combination thereof and
be woken when any desired element is active.

   Copyright (c) 2014-2015, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_channel_v2.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/17/15   mh      Removing channel group support
10/30/14   rbhatnk Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_channel_v2.h"
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */


/****************************************************************************
 ** Channels
 *****************************************************************************/

ADSPResult qurt_elite_channel_create(qurt_elite_channel_t** ppChannel)
{
	qurt_elite_channel_t *pChannel = *ppChannel;
	pChannel = (qurt_elite_channel_t*)qurt_elite_memory_malloc(sizeof(qurt_elite_channel_t),
			QURT_ELITE_HEAP_DEFAULT);
	if (NULL == pChannel)
	{
		*ppChannel = NULL;
		return ADSP_ENOMEMORY;
	}

	pChannel->unBitsUsedMask = 0;
	qurt_signal2_init(&pChannel->anysig);

	*ppChannel = pChannel;

	return ADSP_EOK;
}

void qurt_elite_channel_delete(qurt_elite_channel_t* pChannel)
{
	qurt_signal2_destroy(&pChannel->anysig);
	pChannel->unBitsUsedMask = 0;
	qurt_elite_memory_free(pChannel);
}


