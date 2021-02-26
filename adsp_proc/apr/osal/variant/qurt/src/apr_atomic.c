/*
  Copyright (C) 2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/variant/qurt/src/apr_atomic.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "apr_atomic.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_lock_t apr_atomic_lock;

/*****************************************************************************
 * Routines                                                                  *
 ****************************************************************************/

APR_INTERNAL int32_t apr_atomic_init ( void )
{
  int32_t rc;

  for ( ;; )
  {
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &apr_atomic_lock );
    if ( rc ) break;

    return APR_EOK;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t apr_atomic_deinit ( void )
{
  ( void ) apr_lock_destroy( apr_atomic_lock );

  return APR_EOK;
}

/* ret_result returns the variable's value after the add. */
APR_INTERNAL int32_t apr_atomic_int16_add (
  volatile int16_t* const variable,
  const int16_t value,
  int16_t* const ret_result
)
{
  int16_t sum;

  if ( variable == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( apr_atomic_lock );
  sum = ( *variable + value );
  *variable = sum;
  ( void ) apr_lock_leave( apr_atomic_lock );

  if ( ret_result != NULL )
  {
    *ret_result = sum;
  }

  return APR_EOK;
}

/* ret_result returns the variable's value before the exchange. */
APR_INTERNAL int32_t apr_atomic_int16_cmpxchg (
  volatile int16_t* const variable,
  const int16_t compare,
  const int16_t value,
  int16_t* const ret_result
)
{
  int16_t saved;

  if ( variable == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( apr_atomic_lock );
  saved = *variable;
  if ( saved == compare )
  {
    *variable = value;
  }
  ( void ) apr_lock_leave( apr_atomic_lock );

  if ( ret_result != NULL )
  {
    *ret_result = saved;
  }

  return APR_EOK;
}

/* ret_result returns the variable's value after the add. */
APR_INTERNAL int32_t apr_atomic_int32_add (
  volatile int32_t* const variable,
  const int32_t value,
  int32_t* const ret_result
)
{
  int32_t sum;

  if ( variable == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( apr_atomic_lock );
  sum = ( *variable + value );
  *variable = sum;
  ( void ) apr_lock_leave( apr_atomic_lock );

  if ( ret_result != NULL )
  {
    *ret_result = sum;
  }

  return APR_EOK;
}

/* ret_result returns the variable's value before the exchange. */
APR_INTERNAL int32_t apr_atomic_int32_cmpxchg (
  volatile int32_t* const variable,
  const int32_t compare,
  const int32_t value,
  int32_t* const ret_result
)
{
  int32_t saved;

  if ( variable == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_lock_enter( apr_atomic_lock );
  saved = *variable;
  if ( saved == compare )
  {
    *variable = value;
  }
  ( void ) apr_lock_leave( apr_atomic_lock );

  if ( ret_result != NULL )
  {
    *ret_result = saved;
  }

  return APR_EOK;
}

