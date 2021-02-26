#ifndef DIAG_CFG_H
#define DIAG_CFG_H
/*===========================================================================

                                  D I A G _ C F G . H

  GENERAL DESCRIPTION
  Contains configuration information for diag per platform

Copyright (c) 2009 - 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/platform/adsp/src/diag_cfg.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/05/17    ph    Configure buffering modes for user PD send buffers.
01/08/17    sa    Update UserPD commit threshold in NRT mode
10/03/16    gn    Changes to support common PW for ADSP, CDSP and SLPI
05/12/16    ph    SMD references removal and code clean up.
12/22/15    is    Fix flow control race condition
07/14/15    sa    Added support for time sync Feature
04/08/15    ps    Diag over sockets and dsm removal 
01/03/13    sg    Command Req/Response Channel separation 
11/14/14    vk    Added drain threshould #define 
04/29/13    ph    Added a limit on active listeners.
12/14/12    is    Use proper NRT values to resolve issues with buffering mode
11/11/12    sg    Reverted 8 byte alignment for q6
10/15/12    rh    Changed diagbuf NRT drain parameters
11/18/10    vs    Moved diagbuf_next_aligned_byte definition from diagbuf
10/27/10    SG    Moved DIAG_TX_SLEEP_THRESHOLD_DEFAULT,DIAG_TX_SLEEP_TIME_DEFAULT
                  to here from diagbuf_v.h
10/27/10    sg    Increased diagbuf size to 32k for adsp
09/28/10    sg    Moved diag_time_get from diagtarget.h to here
05/11/10    JV    New heap for events
07/17/09    JV    Created
===========================================================================*/
#include "diag_v.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" /* For stubbed ts_get(); for Q6 modem */
#else
#include "time_svc.h" /* for time_get(); for Q6 apps and all other targets */
#endif

#define DIAGBUF_NEXT_ALIGNED_BYTE(p) (((p) + 3) & ~3)

/* Not adding the DIAG_MAX_TX_PKT_SIZ and DIAG_MAX_RX_PKT_SIZ here because 
they need to be exposed to clients. Currently defined in diagpkt.h */

/* DIAGBUF_SIZE is the size, in bytes, of diag's output buffer.  If this buffer
 * fills up, logs and messages will be dropped.  It is recommended to make this
 * number a power of 2 to allow the compiler to optimize the arithmetic using
 * this number to use shift operations instead of brute force math.
 *
 * Note: This must be larger than DIAG_MAX_TX_PKT_SIZ. */

#ifndef DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER
#define DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER_DEFAULT
#elif ((DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER < DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER_MIN) || (DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER > DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER_MAX))
#undef DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER
#define DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_PER_DEFAULT
#endif

#ifndef DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER
#define DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER_DEFAULT
#elif ((DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER < DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER_MIN) || (DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER > DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER_MAX))
#undef DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER
#define DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER_DEFAULT
#endif

#ifndef DIAG_MULTIPD_DRAIN_TIMER
#define DIAG_MULTIPD_DRAIN_TIMER DIAG_MULTIPD_DRAIN_TIMER_DEFAULT
#elif ((DIAG_MULTIPD_DRAIN_TIMER < DIAG_MULTIPD_DRAIN_TIMER_MIN) || (DIAG_MULTIPD_DRAIN_TIMER > DIAG_MULTIPD_DRAIN_TIMER_MAX))
#undef DIAG_MULTIPD_DRAIN_TIMER
#define DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER DIAG_MULTIPD_BUF_DRAIN_THRESHOLD_PER_DEFAULT
#endif

#ifndef DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_NRT_PER
#define DIAG_MULTIPD_BUF_COMMIT_THRESHOLD_NRT_PER 60
#endif

#endif /* DIAG_CFG_H */


