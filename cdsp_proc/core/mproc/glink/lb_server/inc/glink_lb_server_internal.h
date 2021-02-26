#ifndef GLINK_LB_SERVER_INTERNAL_H
#define GLINK_LB_SERVER_INTERNAL_H

/**
 * @file glink_lb_server_internal.h
 *
 * Declares the coomon functions and macros for the 
 * GLINK Loopback Server. 
 */

/** \defgroup glink_lb_server GLINK LB SERVER
 * \ingroup MPROC
 *
 * Provides functions, types and macros for the GLINK Loopback 
 * Server. 
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014 - 2017 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/lb_server/inc/glink_lb_server_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/16   rv/ab   Added separate init/signal/process functions for 
                   legacy loopback server
07/01/15   bc      Remove unnecessary header
02/08/15   bc      Added timer deinit api
07/14/14   rs      Initial revision
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "glink.h"
#include "glink_lb_server_os.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/

#define GLINK_LB_SERVER_MAX_EVENTS        ( 1024 )



/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

int32 glink_lb_server_timer_deinit( glink_lb_server_timer_type *timer );
int32 glink_lb_server_timer_init( glink_lb_server_timer_type *timer, glink_lb_server_timer_cb_fn_type cb, void *cb_data );

/*===========================================================================
  FUNCTION  glink_lb_server_timer_start
===========================================================================*/
/**
  Starts a timer with specified duration.

  @param[in] pTimer    pointer to os_timer data structure.
  @param[in] duration Timer expiry duration in ms.

  @return    0 if succesful.
             Negative error code if an error occurred.
*/
/*=========================================================================*/
int32 glink_lb_server_timer_start( glink_lb_server_timer_type *ptimer, uint32 duration );

/*===========================================================================
  FUNCTION  glink_lb_server_timer_stop
===========================================================================*/
/**
  Stops a timer.

  @param[in] pTimer    pointer to os_timer data structure.
  @param[in] duration Timer expiry duration in ms.

  @return
  NA.
*/
/*=========================================================================*/
void glink_lb_server_timer_stop( glink_lb_server_timer_type *ptimer );

/*===========================================================================
  FUNCTION  glink_lb_server_init
===========================================================================*/
/**
  Glink loopback server initialization.

  @return
  0 if initialization was successful, a negative error code otherwise.
*/
/*=========================================================================*/
int glink_lb_server_init( void );

/*===========================================================================
  FUNCTION  glink_lb_server_process_events
  ===========================================================================*/
/**
  This function processes all events in the queue.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_lb_server_process_events( void );

/*===========================================================================
  FUNCTION  glink_legacy_lb_server_process_events
  ===========================================================================*/
/**
  This function processes all legacy loopback server events in the queue.

  @return
  NA.

*/
/*=========================================================================*/
#ifdef FEATURE_GLINK_LEGACY_LB_SERVER
void glink_legacy_lb_server_process_events( void );
#endif /* FEATURE_GLINK_LEGACY_LB_SERVER */

/*===========================================================================
  FUNCTION  glink_lb_server_signal_task
  ===========================================================================*/
/**
  Triggers signal to workloop to process the loopback server events.

  @return
  0 if initialization was successful, a negative error code otherwise.

*/
/*=========================================================================*/
int32 glink_lb_server_signal_task( void );

/*===========================================================================
  FUNCTION  glink_legacy_lb_server_signal_task
  ===========================================================================*/
/**
  Triggers signal to workloop to process the legacy loopback server events.

  @return
  0 if initialization was successful, a negative error code otherwise.

*/
/*=========================================================================*/
#ifdef FEATURE_GLINK_LEGACY_LB_SERVER
int32 glink_legacy_lb_server_signal_task( void );
#endif /* FEATURE_GLINK_LEGACY_LB_SERVER */

#endif   /* GLINK_LB_SERVER_INTERNAL_H */
