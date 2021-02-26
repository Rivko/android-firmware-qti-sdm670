/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/variant/qurt/src/apr_event.c#1 $
  $Author: pwbldsvc $
*/

#include <stdlib.h>
#include "err.h"
#include "msg.h"
#include "DALSys.h"
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "apr_dbg_msg.h"

//DISABLED #define ENABLE_DEBUG_MSGS

#ifdef ENABLE_DEBUG_MSGS
  #include <stdarg.h>
  #include "apr_log.h"

/* #define DBG_PRINTF( ... ) safe_printf( __VA_ARGS__ ) */

  /* TODO: Create variadic versions of apr_log_printf(). Windows Mobile
   *       compiler is too old (MSVC 2003) and doesn't support the
   *       variadic macros that allows the macro to expand parameters into
   *       actual arguments into apr_log_printf() without vlists.
   */
  void DBG_PRINTF ( const char* fmt, ... )
  {
  }
#else
  void DBG_PRINTF ( const char* fmt, ... )
  {
    /* Do nothing. */
  }
#endif /* ENABLE_DEBUG_MSGS */

#include "apr_event.h"

#define APR_EVENT_SET_CLEARED_V ( 0 )
#define APR_EVENT_SET_SIGNALLED_V ( 1 )

#define APR_EVENT_ABORT_CLEARED_V ( 0 )
#define APR_EVENT_ABORT_SIGNALLED_V ( 1 )
#define APR_EVENT_ABORT_PROCESSED_V ( 2 )

typedef struct apr_event_internal_t
{
  apr_lock_t lock;
  DALSYSEventHandle event_handle;
  int32_t set;
  int32_t abort;
}
  apr_event_internal_t;

APR_INTERNAL int32_t apr_event_create (
  apr_event_t* ret_event
)
{
  int32_t rc;
  DALResult dal_rc;
  int32_t checkpoint;
  apr_event_internal_t* the_event;

  if ( ret_event == NULL )
  {
    return APR_EBADPARAM;
  }

  for ( ;; )
  {
    checkpoint = 0;
    the_event = ( ( apr_event_internal_t* ) malloc( sizeof( apr_event_internal_t ) ) );
    if ( the_event == NULL ) break;

    checkpoint = 1;
    the_event->set = APR_EVENT_SET_CLEARED_V;
    the_event->abort = APR_EVENT_ABORT_CLEARED_V;

    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &the_event->lock );
    if ( rc ) break;

    checkpoint = 2;
    dal_rc = DALSYS_EventCreate( DALSYS_EVENT_ATTR_NORMAL, &the_event->event_handle, NULL );
    if ( dal_rc != DAL_SUCCESS ) break;

    checkpoint = 3;
    *ret_event = ( ( apr_event_t* ) the_event );

    return APR_EOK;
  }

  APR_DBG_1(APR_DBG_ERROR, "Error checkpoint (%d)", checkpoint);

  switch ( checkpoint )
  {
  case 3:
    ( void ) DALSYS_DestroyObject( the_event->event_handle );
    /*-fallthru */
  case 2:
    ( void ) apr_lock_destroy( the_event->lock );
    /*-fallthru */
  case 1:
    ( void ) free( the_event );
    /*-fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t apr_event_destroy (
  apr_event_t event
)
{
  apr_event_internal_t* the_event = ( ( apr_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) DALSYS_DestroyObject( the_event->event_handle );
  ( void ) apr_lock_destroy( the_event->lock );
  ( void ) free( the_event );

  return APR_EOK;
}

APR_INTERNAL int32_t apr_event_wait (
  apr_event_t event
)
{
  DALResult dal_rc;
  int32_t errors = 0;
  apr_event_internal_t* the_event = ( ( apr_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( the_event->lock );

  DBG_PRINTF( "event_wait() enter - set: %d, abort: %d", the_event->set, the_event->abort );

  /**
   * DALSYS_EventWait() occurs whenever:
   *   1. The abort is cleared, or
   *   2. The abort is signalled but not yet processed.
   * Otherwise, abort the wait by returning immediately.
   */
  if ( the_event->abort != APR_EVENT_ABORT_PROCESSED_V )
  {
    ( void ) apr_lock_leave( the_event->lock );

    dal_rc = DALSYS_EventWait( the_event->event_handle );
      /**<
       * Note: DAL only supports waiting on the same thread that created the
       *       event due a REX RTOS limitation. The apr_event_wait()
       *       therefore can only be called by the same thread that created
       *       the event as a general limitation.
       */
    if ( dal_rc != DAL_SUCCESS )
    {
      errors |= 1;
      ERR_FATAL( "Event wait failed (%d)", dal_rc, 0, 0 );
    }

    ( void ) apr_lock_enter( the_event->lock );
  }

  DBG_PRINTF( "event_wait() leave - set: %d, abort: %d", the_event->set, the_event->abort );

  if ( the_event->abort != APR_EVENT_ABORT_CLEARED_V )
  { /* The abort processing takes precedence over the set processing. */
    the_event->abort = APR_EVENT_ABORT_PROCESSED_V;
    errors |= 2;
  }
  else
  {
    the_event->set = APR_EVENT_SET_CLEARED_V;
  }

  ( void ) apr_lock_leave( the_event->lock );

  if ( errors == 2 )
  {
    return APR_EABORTED;
  }
  else
  {
    return ( errors ? APR_EFAILED : APR_EOK );
  }
}

APR_INTERNAL int32_t apr_event_signal (
  apr_event_t event
)
{
  DALResult dal_rc;
  int32_t errors = 0;
  apr_event_internal_t* the_event = ( ( apr_event_internal_t* ) event );
  int32_t set_flag;

  if ( the_event == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( the_event->lock );

  set_flag = the_event->set;

  DBG_PRINTF( "event_signal() enter - set: %d", set_flag );

  /* Subsequent signals are cascaded when an existing signal is pending. */
  if ( set_flag == APR_EVENT_SET_CLEARED_V )
  {
    the_event->set = APR_EVENT_SET_SIGNALLED_V;

    dal_rc = DALSYS_EventCtrlEx( the_event->event_handle, DALSYS_EVENT_CTRL_TRIGGER, 0, NULL, 0 );
    if ( dal_rc != DAL_SUCCESS )
    {
      errors |= 1;
      ERR_FATAL( "Event set failed (%d)", dal_rc, 0, 0 );
    }
  }

  DBG_PRINTF( "event_signal() leave - set: %d", the_event->set );

  ( void ) apr_lock_leave( the_event->lock );

  return ( errors ? APR_EFAILED : APR_EOK );
}

APR_INTERNAL int32_t apr_event_signal_abortall (
  apr_event_t event
)
{
  DALResult dal_rc;
  int32_t errors = 0;
  apr_event_internal_t* the_event = ( ( apr_event_internal_t* ) event );
  int32_t abort_flag;

  if ( the_event == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( the_event->lock );

  abort_flag = the_event->abort;

  DBG_PRINTF( "event_signal_abortall() enter - abort: %d", abort_flag );

  /* Subsequent aborts are cascaded when an existing abort is pending. */
  if ( abort_flag == APR_EVENT_ABORT_CLEARED_V )
  {
    the_event->abort = APR_EVENT_ABORT_SIGNALLED_V;

    /* DAL doesn't support event broadcasts. */
    dal_rc = DALSYS_EventCtrlEx( the_event->event_handle, DALSYS_EVENT_CTRL_TRIGGER, 0, NULL, 0 );
    if ( dal_rc != DAL_SUCCESS )
    {
      errors |= 1;
      ERR_FATAL( "Event abort failed (%d)", dal_rc, 0, 0 );
    }
  }

  DBG_PRINTF( "event_signal_abortall() leave - abort: %d", the_event->abort );

  ( void ) apr_lock_leave( the_event->lock );

  return ( errors ? APR_EFAILED : APR_EOK );
}

APR_INTERNAL int32_t apr_event_cancel_abortall (
  apr_event_t event
)
{
  apr_event_internal_t* the_event = ( ( apr_event_internal_t* ) event );

  if ( the_event == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( the_event->lock );

  DBG_PRINTF( "event_cancel_abortall() enter - abort: %d", the_event->abort );

  the_event->abort = APR_EVENT_ABORT_CLEARED_V;

  DBG_PRINTF( "event_cancel_abortall() leave - abort: %d", the_event->abort );

  ( void ) apr_lock_leave( the_event->lock );

  return APR_EOK;
}

