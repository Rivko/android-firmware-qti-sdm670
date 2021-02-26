/*
  Copyright (C) 2009-2011, 2012 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/src/apr_objmgr.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h" /* NULL */
#include "apr_errcodes.h" /* For the standard APR_Exxx errro codes. */
#include "apr_objmgr.h"

/****************************************************************************
 * EXPORTED ROUTINES                                                        *
 ****************************************************************************/

APR_EXTERNAL int32_t apr_objmgr_construct (
  apr_objmgr_t* objmgr,
  apr_objmgr_setup_params_t* params
)
{
  int32_t rc;
  uint32_t i;
  apr_objmgr_object_t* obj;

  if ( ( objmgr == NULL ) ||
       ( params == NULL ) ||
       ( params->index_bits == 0 ) ||
       ( params->total_bits < ( params->index_bits + 1 /* version */ ) ) ||
         /* Handle must have sufficient space to hold mandatory fields. */
       ( ( params->lock_fn == NULL ) || ( params->unlock_fn == NULL ) )
         /* Must provide lock routines even if they're not used. */
     )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init( &objmgr->free_q, params->lock_fn, params->unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  objmgr->table = params->table;
  objmgr->total_num = ( 1 << params->index_bits );
  objmgr->index_mask = ( objmgr->total_num - 1 );
  objmgr->overflow_mask = ( ( 1 << params->total_bits ) - 1 );
  objmgr->lock_fn = params->lock_fn;
  objmgr->unlock_fn = params->unlock_fn;

  for ( i = 0; i < objmgr->total_num; ++i )
  {
    obj = &objmgr->table[ i ];
    obj->handle = i;
      /* TODO: Initialize the version field to a random value. */
    obj->type = 0;
    obj->any.ptr = NULL;
    rc = apr_list_add_tail( &objmgr->free_q, &obj->link );
  }

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_objmgr_destruct (
  apr_objmgr_t* objmgr
)
{
  if ( objmgr == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_list_destroy( &objmgr->free_q );

  return APR_EOK;
}

/* TODO: The L4 platform running on an SMT (Q6) architecture could end up with
*        application threads and interrupt threads in parallel. Acquiring the
*        lock is therefore required to accessed shared resources. The lifetime
*        of objects is typically controlled by the task context. An ISR may
*        need to hold onto the ISR lock until it's done with the object to
*        prevent the task from deleting it. We should use different locks to
*        reduce ISR and task contentions in the applications.
*/

APR_EXTERNAL int32_t apr_objmgr_alloc_object (
  apr_objmgr_t* objmgr,
  apr_objmgr_object_t** ret_object
)
{
  int32_t rc;

  if ( ( objmgr == NULL ) || ( ret_object == NULL ) )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_remove_head( &objmgr->free_q, ( ( apr_list_node_t** ) ret_object ) );
  if ( rc )
  {
    return APR_EFAILED;
  }

  ( *ret_object )->handle |= objmgr->total_num;
    /* Claim the free object by setting the visible flag. */

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_objmgr_find_object (
  apr_objmgr_t* objmgr,
  uint32_t handle,
  apr_objmgr_object_t** ret_object
)
{
  int32_t rc = APR_EOK;
  apr_objmgr_object_t* obj;

  if ( ( objmgr == NULL ) || ( ret_object == NULL ) )
  {
    return APR_EBADPARAM;
  }

  if ( ( handle & objmgr->total_num ) == 0 )
  { /* The client supplied an inactive handle. */
    return APR_EHANDLE;
  }

  obj = &objmgr->table[ handle & objmgr->index_mask ];

  /* objmgr->lock_fn( ); */
    /* The caller should hold the lock to guaranty safety in concurrent
     * systems. The caller doesn't need to use any locks when system is a
     * single writer and multiple readers setup.
     */

  if ( obj->handle == handle )
  {
    *ret_object = obj;
  }
  else
  {
    rc = APR_EHANDLE;
  }

  /* objmgr->unlock_fn( ); */

  return rc;
}

APR_EXTERNAL int32_t apr_objmgr_free_object (
  apr_objmgr_t* objmgr,
  uint32_t handle
)
{
  int32_t rc = APR_EOK;
  apr_objmgr_object_t* obj;

  if ( objmgr == NULL )
  {
    return APR_EBADPARAM;
  }

  if ( handle == 0 )
  {
    return APR_EHANDLE;
  }

  obj = &objmgr->table[ handle & objmgr->index_mask ];

  objmgr->lock_fn( );

  if ( obj->handle == handle )
  {
    obj->handle += objmgr->total_num;
      /* Increment the version and clear the visible flag in one-shot. */
    obj->handle &= objmgr->overflow_mask;
      /* Perform manual wrap-around for handles less than 32 bits. */
    obj->type = 0;
    obj->any.ptr = NULL;
    ( void ) apr_list_add_tail( &objmgr->free_q, &obj->link );
  }
  else
  {
    rc = APR_EHANDLE;
  }

  objmgr->unlock_fn( );

  return rc;
}

