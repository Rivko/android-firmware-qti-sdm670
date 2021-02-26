#ifndef GLINK_LB_SERVER_OS_H
#define GLINK_LB_SERVER_OS_H

/**
 * @file glink_lb_server_os.h
 *
 * Declares the OS specific functions and macros for the G-LINK 
 * Loopback Server. 
 */

/** \defgroup glink_lb_server GLINK LB SERVER
 * \ingroup MPROC
 *
 * Provides all OS specific functions for the G-LINK Loopback 
 * Server. 
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/glink/lb_server/inc/glink_lb_server_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/15   bc      Remove unnecessary header
07/20/14   rs      Initial revision
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "glink_os_utils.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/

#define FEATURE_UNIT_TEST
#define FEATURE_PROFILING

#define GLINK_LB_SERVER_MAX_EVENTS        ( 1024 )

#define GLINK_LB_SERVER_BUFFER_FLAGS_NONE               ( 0x00000000 )
#define GLINK_LB_SERVER_BUFFER_FLAGS_DYNAMIC            ( 0x00000001 )
#define GLINK_LB_SERVER_BUFFER_FLAGS_EXTERNAL_ALLOC     ( 0x00000002 )


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** DAL based Timer type defitions */
typedef void ( *glink_lb_server_timer_cb_fn_type )( void *cb_data );

typedef struct glink_lb_server_timer_struct
{
  timer_type os_timer;
  glink_lb_server_timer_cb_fn_type cb;
  void *cb_data;
}glink_lb_server_timer_type;


/** Buffer type */
typedef struct glink_lb_server_buffer_struct
{
  void                          *ptr;
  size_t                         buffer_size;
  size_t                         size;
  uint32                         flags;
  struct glink_lb_server_buffer_struct *next;
}glink_lb_server_buffer_type;


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/
//extern ULogHandle     glink_lb_server_ulog_handle;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif   /* GLINK_LB_SERVER_OS_H */
