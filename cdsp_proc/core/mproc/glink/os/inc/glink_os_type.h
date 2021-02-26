#ifndef OS_TYPE_H
#define OS_TYPE_H

/*===========================================================================

                      G-Link OS Specific types/functions


 Copyright (c) 2014-2015 by QUALCOMM Technologies, Incorporated.  
 All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/os/inc/glink_os_type.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/15   pp      Added a different ulog handle for error logging
07/10/15   rs      Use OS defined macro for unused parameters 
07/01/15   bc      Switch from comdef to com_dtypes
05/07/15   an      Added type for timeticks
04/18/14   bm      Initial version
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "string.h"
#include "qurt.h"
#include "err.h"
#include "assert.h"
#include "timer.h"  /* Clock services definitions           */
#include "DALFramework.h"
#include "DDIIPCInt.h"
#include "ULogFront.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

#define OS_LOG_INIT( ) glink_os_log_init()

/** Tracing macro */
#ifdef FEATURE_PROFILING
#define OS_EVENT_TRACE( args... ) \
  if( os_log_handle ) \
  { \
    ULogFront_RealTimePrintf( os_log_handle, ##args ); \
  }
#else
#define OS_EVENT_TRACE( args... )
#endif

/** Generic debug message */
#define OS_LOG_MSG( args... ) \
  if( os_log_handle ) \
  { \
    ULogFront_RealTimePrintf( os_log_handle, ##args ); \
  }

/** Generic error message print */
#define OS_LOG_ERROR( args... ) \
  if( os_log_handle ) \
  { \
    ULogFront_RealTimePrintf( os_error_log_handle, ##args ); \
  }
#define OS_LOG_ERROR_FATAL( args... ) \
  do \
  { \
    if( os_log_handle ) \
    { \
      ULogFront_RealTimePrintf( os_error_log_handle, ##args ); \
    } \
    ASSERT( FALSE ); \
  } \
  while( 0 )


#define GLINK_OS_SEC_TO_USEC (1000000ULL)
#define GLINK_OS_SEC_TO_TICK (19200000ULL)

#define GLINK_OS_TICK_TO_USEC(t) ((t)*10ULL/192ULL)
#define GLINK_OS_USEC_TO_TICK(u) ((u)*192ULL/10ULL)

#define GLINK_OS_UNREFERENCED_PARAM( param ) ( void )param

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
typedef qurt_mutex_t os_cs_type;
typedef qurt_mutex_t os_mutex_type;

typedef void ( *os_timer_cb_fn )( void *cb_data );

typedef timer_type os_timer;

typedef struct 
{
  DALSYSEventHandle dal_event[2];
  DALSYSEventObj    dal_obj_memory[2];
}os_event_type;

typedef void ( *os_isr_cb_fn )( void *cb_data );

typedef struct os_ipc_intr_struct
{
  DalIPCIntProcessorType  processor;
  DalIPCIntInterruptType  irq_out;
} os_ipc_intr_type;

typedef unsigned long long os_timetick_type;

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL VARIABLE DEFINITIONS
===========================================================================*/

extern ULogHandle os_log_handle;
extern ULogHandle os_error_log_handle;

#endif /* OS_TYPE_H */
