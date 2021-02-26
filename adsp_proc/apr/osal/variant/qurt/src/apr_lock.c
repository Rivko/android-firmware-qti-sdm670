/*
  Copyright (C) 2009-2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/variant/qurt/src/apr_lock.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include "DALSys.h" /* For DALSYS_Malloc and DALSYS_free. */
#include "qurt_rmutex2.h" /* For qurt_rmutex2. */
#include "apr_errcodes.h"
#include "apr_lock.h"

typedef struct apr_lock_internal_t
{
  qurt_rmutex2_t lock;
}
  apr_lock_internal_t;

APR_INTERNAL int32_t apr_lock_create (
  uint32_t lock_type,
  apr_lock_t* ret_lock
)
{
  apr_lock_internal_t* the_lock;
  DALResult dal_rc;

  if ( ret_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( lock_type )
  {
  case APR_LOCK_TYPE_MUTEX:
  case APR_LOCK_TYPE_INTERRUPT:
    break;

  default:
    return APR_EBADPARAM;
  }

  dal_rc = DALSYS_Malloc( sizeof( apr_lock_internal_t ), ( ( void** ) &the_lock ) );
  if ( dal_rc != DAL_SUCCESS )
  {
    return APR_ENORESOURCE;
  }

  qurt_rmutex2_init( &the_lock->lock );

  *ret_lock = the_lock;

  return APR_EOK;
}


APR_INTERNAL int32_t apr_lock_destroy (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;
  DALResult dal_rc;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_rmutex2_destroy( &the_lock->lock );

  dal_rc = DALSYS_Free( the_lock );
  if ( dal_rc != DAL_SUCCESS )
  {
    return APR_EPANIC;
  }

  return APR_EOK;
}


APR_INTERNAL int32_t apr_lock_enter (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_rmutex2_lock( &the_lock->lock );

  return APR_EOK;
}


APR_INTERNAL int32_t apr_lock_leave (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_rmutex2_unlock( &the_lock->lock );

  return APR_EOK;
}
