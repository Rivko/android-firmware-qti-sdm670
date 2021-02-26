#ifndef _USLEEPI_H_
#define _USLEEPI_H_
/*==============================================================================
  FILE:         uSleepi.h

  OVERVIEW:     Internal interface functions and types for uSleep

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/uSleepi.h#2 $
$DateTime: 2018/01/02 22:33:02 $
==============================================================================*/
#include "comdef.h"
#include "uSleep.h"
#include "qurt.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleep_mode_trans.h"
#endif

/*==============================================================================
                            MACRO DEFINITIONS
 =============================================================================*/
/* Macro defining the number of QTimer Ticks per usec. */
#define SLEEP_TICK_PER_US       (19.2)

/* Convert tick unit (19.2 MHz) values into sclk unit values (32768 Hz) values */
#define TICKS_TO_SCLK64(ticks)  ((uint64)((uint64)(ticks) / (uint64)586))

/* Convert sclk unit (32768 Hz) values into tick unit values (19.2 MHz) values */
#define SCLK_TO_TICKS64(sclks)  ((uint64)((uint64)(sclks) * (uint64)586))

/* Convert tick unit (19.2 MHz) values into us time */
#define TICKS_TO_US(ticks)      ((uint32)((ticks)/SLEEP_TICK_PER_US))

/* Macro to convert usec to 19.2Mhz QTimer Tick. */
#define US_TO_TICKS(usec)       ((uint32)((usec)*SLEEP_TICK_PER_US))

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* Value to compate with to check if threshold timer is "close" and bail out of* 
 * entering sleep or picking a mode */
#define USLEEP_THRESHOLD_LIMIT_CHECK      587

#ifdef USLEEP_SUPPORT_TRANSITIONS

/* Internal states for uSleep mode used to control tranistions and other behavior */
typedef enum uSleep_internal_state
{
  /* Steady mode state */
  uSLEEP_INTERNAL_STATE_IN_DDR            = 0x0001, /* Normal operational mode */
  uSLEEP_INTERNAL_STATE_ACTIVE            = 0x0002, /* Island operational mode */

  uSLEEP_INTERNAL_STATE_STDY_RSVD4        = 0x0004, /* Reserved steady states */
  uSLEEP_INTERNAL_STATE_STDY_RSVD8        = 0x0008,

  /* Entry transitions */
  uSLEEP_INTERNAL_STATE_FULL_ENTRY        = 0x0010, /* Initial entry request stage */
  uSLEEP_INTERNAL_STATE_STAGE1_ENTRY      = 0x0020, /* Initial processing, DDR fully accessable 
                                                     * still */ 
  uSLEEP_INTERNAL_STATE_STAGE2_ENTRY      = 0x0040, /* Kernel completed, performing any HW entry */
  uSLEEP_INTERNAL_STATE_STAGE3_ENTRY      = 0x0080, /* Kernal/HW entry done, completing SW entry */

  /* Exit transitions */
  uSLEEP_INTERNAL_STATE_REQ_EXIT          = 0x0100, /* Initial exit requested */
  uSLEEP_INTERNAL_STATE_STAGE1_EXIT       = 0x0200, /* Initial exit stage, where exit preperation
                                                     * is done */
  uSLEEP_INTERNAL_STATE_STAGE2_EXIT       = 0x0400, /* Exit prep is done, continuing with 
                                                     * first kernel stage exit */ 
  uSLEEP_INTERNAL_STATE_RSVD8_EXIT        = 0x0800, /* Reserved exit state */

  /* Fatal error exit */
  uSLEEP_INTERNAL_STATE_FATAL_EXIT        = 0x1000, /* Fatal error exit stage */
  
}uSleep_internal_state;

/**
 * uSleep_signals
 * 
 * @brief Signals to use to indicate callback function execution state between 
 *        different PD's
 */
typedef struct uSleep_signals_s
{
  qurt_anysignal_t start; /* Signal to start executing callbacks -user wait */ 
  qurt_signal_t    done;  /* Signal to indicate callbacks are done -guest wait*/
}uSleep_signals;

/**
 * uSleep_callback_list
 * 
 * @brief Main callback strucutre for guest PD
 */
typedef struct uSleep_callback_list_s
{
  uSleep_notification_cb_type   callback;
  uint32                        seedEnterLat;
  uint32                        seedExitLat;

  struct uSleep_callback_list_s *next;
}uSleep_callback_list;

/**
 * uSleep_user_signal_cb_type
 * 
 * @brief User callback function prototype
 */
typedef uSleep_status (*uSleep_user_signal_cb_type)(uSleep_state_notification  state, 
                                                    uSleep_signals             *signals);

/**
 * uSleep_user_callback_list
 * 
 * @brief Main callback strucutre for user PD which includes signals that guest
 *        PD will wait on when calling the user callbacks
 */
typedef struct uSleep_user_callback_list_s
{
  uSleep_user_signal_cb_type          callback;
  uSleep_signals                      *signals;
  struct uSleep_user_callback_list_s  *next;
}uSleep_user_callback_list;

/**
 * uSleep_callback_data
 * 
 * @brief Main callback structure that contains the two callback function lists
 *        as well as the total entry/exit latencies for the registered callbacks
 */
typedef struct uSleep_callback_data_s
{
  uSleep_callback_list      *callback_list;
  uSleep_user_callback_list *user_callback_list;
}uSleep_callback_data;

/* Signal values to trigger the execution of user callbacks */
#define USLEEP_SIGNAL_START_ENTER     0x01
#define USLEEP_SIGNAL_START_EXIT      0x02 /* exit callbacks not currently called */

/* Signal value for guest PD to wait on for user callbacks to finished exeuction */ 
#define USLEEP_SIGNAL_DONE_FINISHED   0x01

#endif /* USLEEP_SUPPORT_TRANSITIONS */

#endif /* _USLEEPI_H_ */

