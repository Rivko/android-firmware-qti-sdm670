#ifndef _SLEEPI_H_
#define _SLEEPI_H_
/*==============================================================================
  FILE:         sleepi.h
  
  OVERVIEW:     This file provides the global macro's and types needed internal
                to the sleep module.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/sleepi.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "CoreConfig.h"
#include "CoreTime.h"
#include "sleep_log.h"

/*==============================================================================
                            MACRO DEFINITIONS
 =============================================================================*/
/**
 * @brief SLEEP_TICK_PER_US
 *
 * Macro defining the number of QTimer Ticks per usec.
 */
#define SLEEP_TICK_PER_US (19.2)

/**
 * @brief TICKS_TO_SCLK64
 *
 * Convert tick unit (19.2 MHz) values into sclk unit values (32768 Hz) values  
 */
#define TICKS_TO_SCLK64(sclks) ((uint64)((uint64)(sclks) / (uint64)586))

/**
 * @brief SCLK_TO_TICKS64
 *
 * Convert sclk unit (32768 Hz) values into tick unit values (19.2 MHz) values  
 */
#define SCLK_TO_TICKS64(sclks) ((uint64)((uint64)(sclks) * (uint64)586))

/**
 * @brief TICKS_TO_US
 *
 * Convert tick unit (19.2 MHz) values into us time
 */
#define TICKS_TO_US(ticks)    ((uint32)((ticks)/SLEEP_TICK_PER_US))

/**
 * @brief US_TO_TICKS
 *
 * Macro to convert usec to 19.2Mhz QTimer Tick.
 */
#define US_TO_TICKS(usec) ((uint32)((usec)*SLEEP_TICK_PER_US))

/**
 * @brief ABS
 *
 * Returns absolute value of the argument
 */
#ifndef ABS
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )
#endif /* ABS */

/**
 * @brief MAX
 *
 * Returns the greater value of the two arguments 'a' and 'b' 
 */
#ifndef MAX
#define MAX( a, b ) ((a)>(b)) ? (a) : (b)
#endif

/**
 * @brief SLEEP_QUERY_HARD_WAKEUP_TIME
 *
 * Macro to use when querying the wakeup node for hard wakeup time settings.
 */
#define SLEEP_QUERY_HARD_WAKEUP_TIME (NPA_QUERY_RESERVED_END+2)

/**
 * SLEEP_MIN_MATCH_VAL_DELTA 
 *  
 * @brief This is the minimum amount of time that the timer HW 
 *        requires us to set the timetick into the future, in
 *        order for the timer interrupt to expire.  This means
 *        that we must be able to sleep for at least this much
 *        time if we want to do a backoff.  Otherwise, the
 *        backoff match value will be set to expire after the
 *        actual timer expiration was supposed to occur.
 */
#define SLEEP_MIN_MATCH_VAL_DELTA  10

/**
 * @brief SLEEP_LATENCY_NAME_LENGTH
 *
 * Defines the maximum allowable length for each latency npa resources name.
 */
#define SLEEP_LATENCY_NAME_LENGTH 32

#if !defined(SLEEP_INLINE)
#if defined(WINSIM) || defined(__ARMCC_VERSION)
#define SLEEP_INLINE __inline
#elif defined(__clang__)
#define SLEEP_INLINE
#else
#define SLEEP_INLINE inline
#endif
#endif /* SLEEP_INLINE */

/*==============================================================================
                            TYPE DEFINITIONS
 =============================================================================*/
/**
 * sleep_status enum
 *
 * @brief This enum lists all the states a mode can be in. 
 *
 * Details of each status is as follows.
 *   SLEEP_ENABLED:    The mode is enabled and hence can be entered
 *   SLEEP_DISABLED:   The mode is disabled and cannot be entered. 
 *   SLEEP_IMPOSSIBLE: The mode can never be in enabled state and so it won't
 *                     be entered ever.
 *
 * The mode can switch between SLEEP_ENABLED and SLEEP_DIABLED but once it goes
 * into SLEEP_IMPOSSIBLE state, it cannot change from there.
 */
typedef enum sleep_status
{
  SLEEP_ENABLED,
  SLEEP_DISABLED,
  SLEEP_IMPOSSIBLE
} sleep_status;

/*==============================================================================
                         GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * @brief sleepNPA_initialize
 * 
 * Initialize the sleep NPA subsystem.  This function registers the latency
 * and wakeup nodes that are used by the sleep task with the NPA framework.
 */
void sleepNPA_initialize( void );

/**
 * @brief sleepLPR_initialize
 * 
 * Initialize the sleep LPR subsystem.  This function registers the sleep
 * LPR node with the NPA framework.
 */
void sleepLPR_initialize( void );

/**
 * @brief sleepOS_createConfigHandle
 * 
 * Creates a CoreConfigHandle, to be used when reading the sleep 
 * configuration data.  The mechanism for reading the config 
 * data varies per OS. 
 *  
 * @return NULL if an error occurred.  Otherwise, a valid 
 *         CoreConfigHandle is returned.
 */
CoreConfigHandle sleepOS_createConfigHandle(void);

/**
 * @brief sleepOS_destroyConfigHandle
 * 
 * Destroys a CoreConfigHandle.  Closes any file descriptors 
 * that might be open, and frees the memory that was allocated 
 * for the object. 
 * 
 * @param config: Handle to the CoreConfig object to destroy.
 */
void sleepOS_destroyConfigHandle( CoreConfigHandle config );

/**
 * @brief sleepOS_getLastTimerDeadline
 * 
 * Returns the last deadline (in absolute ticks) that was used for the
 * hard wakeup for timers.  This is used by the NPA scheduler code to determine
 * whether the NPA scheduler deadline is the limiting factor in sleep, or
 * whether it's the timer deadline.
 * 
 * @return The last timer deadline (in absolute ticks).
 *
*/
uint64 sleepOS_getLastTimerDeadline( void );

/**
 * @brief sleepOS_offsetSleepDuration
 * 
 * Adds the offset passed in to the currently-used sleep duration.  This is
 * called by the NPA scheduler LPR, to adjust the sleep duration that's passed
 * to the LPRMs, after adjusting the NPA scheduler deadline.
 * 
 * @param offset: Amount to offset the sleep duration by, in ticks.
 *
*/
void sleepOS_offsetSleepDuration( uint64 offset );

#ifdef __cplusplus
}
#endif

#endif /* _SLEEPI_H_ */

