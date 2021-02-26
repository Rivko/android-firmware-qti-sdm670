/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/avtimer/src/avtimer_defines.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/31/10   kk      Created file.
========================================================================== */
/**
@file avtimer_defines.h

@brief This file contains avtimer defines used by avtimer server and client.
*/

#ifndef AVTIMER_DEFINES_H
#define AVTIMER_DEFINES_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
 * avtimer sw timer states
 */
#define AVTIMER_CREATED                 0xcececece
#define AVTIMER_ACTIVE                  0xadadadad
#define AVTIMER_EXPIRED                 0xedededed
#define AVTIMER_INVALID                 0xdfdfdfdf
#define AVTIMER_ATTR_INIT               0xaeaeaeae


/**
 * Default Values for avtimer sw timers
 */
#define AVTIMER_DEFAULT_TYPE            AVTIMER_ONESHOT_DURATION
#define AVTIMER_DEFAULT_DURATION        1000uL
#define AVTIMER_DEFAULT_EXPIRY          0uL


/**
 * avtimer max duration ticks = 8.9 years
 * in avtimer, 1 tick = 1 micro sec.
 */
#define AVTIMER_MAX_DURATION_TICKS       AVTIMER_MAX_DURATION


/**
 * avtimer tick error margin, 1 tick = 1 usec
 */
//ToDo need to fine tune this value
#define AVTIMER_TICK_ERROR_MARGIN          10    //the interrupt latency/context switch is around 10 micro-sec

/**
 * avtimer tick calculation margin, 1 tick = 1 usec
 */
//Todo need to fine tune
#define AVTIMER_TICK_CALCULATION_MARGIN    1


/**
 * avtimer client sigmask
 */
#define AVTIMER_CLIENT_ACK_SIGMASK              0x1

/**
 * avtimer server commands
 */
typedef enum {
    AVTIMER_SERVER_CREATE_CMD=0,
    AVTIMER_SERVER_DELETE_CMD,
    AVTIMER_SERVER_GET_ATTR_CMD,
    AVTIMER_SERVER_RESTART_CMD,
    AVTIMER_SERVER_STOP_CMD,
    AVTIMER_SERVER_NUM_COMMANDS
}AVTIMER_SERVER_COMMAND_T;

//ToDo may not need avimer_tick_word_t, since both are same in avtimer
typedef uint64_t              avtimer_tick_type;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AVTIMER_DEFINES_H

