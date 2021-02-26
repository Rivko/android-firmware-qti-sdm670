/*========================================================================
qurt_elite_signal.cpp

This file contains signals utilities.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_signal_v2.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/30/14   rbhatnk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "qurt_elite_signal_v2.h"

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
** Signals
*****************************************************************************/
void qurt_elite_signal_delete(qurt_elite_signal_t* pSignal)
{
	qurt_elite_signal_destroy(pSignal);
}

