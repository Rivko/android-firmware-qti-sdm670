/*========================================================================
qurt_elite.h

This is the top-level include file for the qurt_elite utilities.

This file includes all header files required for using qurt_elite functions.
The user of qurt_elite should only include this file to call the qurt_elite functions.
*/
/*========================================================================
Copyright (c) 2010, 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/21/15   sw      (Tech Pubs) Updated legal statements above.
07/10/15   mh      Updated dependencies info for conditional variable wait
                   API
02/04/10   mwc     Created file.

========================================================================== */

#ifndef QURT_ELITE_H
#define QURT_ELITE_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#if defined(__qdsp6__)
#include "qurt.h"
#include "qurt_signal2.h"
#include "qube.h"
#include "hexagon_protos.h"
#ifndef MDSPMODE
#include "q6_system_defns.h"
#else
#define MAX_THREADS         128
#endif
#endif


#include "qurt_elite_diag.h"
#include "qurt_elite_bufmgr.h"
#include "qurt_elite_channel.h"
#include "qurt_elite_globalstate.h"
#include "qurt_elite_memory.h"
#include "qurt_elite_memorymap.h"
#include "qurt_elite_mutex.h"
#include "qurt_elite_nmutex.h"
#include "qurt_elite_profile.h"
#include "qurt_elite_queue.h"
#include "qurt_elite_signal.h"
#include "qurt_elite_thread.h"
#include "qurt_elite_timer.h"
#include "qurt_elite_types.h"
#include "qurt_elite_interrupt.h"
#include "qurt_elite_condvar.h"
#include "qurt_elite_std.h"

#include "adsp_error_codes.h"

#include <string.h>
#include "stringl.h"
#include <stdio.h>


#endif // #ifndef QURT_ELITE_H

