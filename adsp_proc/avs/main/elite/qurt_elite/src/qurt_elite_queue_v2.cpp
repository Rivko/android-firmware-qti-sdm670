/*========================================================================

*//** @file qurt_elite_queue.cpp
This file contains an efficient queue of 8-uint8_t nodes.

Copyright (c) 2010 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_queue_v2.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/30/14   rbhatnk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_queue_v2.h"

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
** Queues
*****************************************************************************/

void qurt_elite_queue_delete(qurt_elite_queue_t* pQ)
{
	qurt_elite_queue_destroy(pQ);
}

