/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/src/apr_list.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include "err.h"
#include "apr_errcodes.h"
#include "apr_list.h"

/* Verify that the header of the linking node points to itself. This helps
 * catch programming errors where the same node is inserted more than once on
 * a linked list.
 */
static void apr_list_raw_assert_secure_node_is_unlinked (
  apr_list_t* list,
  apr_list_node_t* node
)
{
  if ( ( list->en_secure ) && (( node->next != node ) || ( node->prev != node )) )
  {
    ERR_FATAL( "Detected the node was already linked or is corrupted: "
               "node=0x%08X, prev=0x%08X, next=0x%08X",
               ( ( uint32_t ) node ), ( ( uint32_t ) node->prev ),
               ( ( uint32_t ) node->next ) );
  }
}

/* Verify that the header of the de-linking node is linked to a linked list.
 * This helps catch memory corruption issues, and programming errors where
 * the same node was inserted twice (i.e. init_node(node1),
 * insert(list1, node1), insert(list2, node1), error).
 */
static void apr_list_raw_assert_node_is_linked (
  apr_list_node_t* node
)
{
  if ( ( node->prev->next != node ) || ( node->next->prev != node ) )
  {
    ERR_FATAL( "Detected a corruption to the link pointer(s): "
               "node=0x%08X, prev->next=0x%08X, next->prev=0x%08X",
               ( ( uint32_t ) node ), ( ( uint32_t ) node->prev->next ),
               ( ( uint32_t ) node->next->prev ) );
  }
}

static void apr_list_raw_insert_node_before (
  apr_list_t* list,
  apr_list_node_t* ref_node,
  apr_list_node_t* new_node
)
{
  apr_list_raw_assert_node_is_linked( ref_node );
  apr_list_raw_assert_secure_node_is_unlinked( list, new_node );

  new_node->list = list;
  ref_node->prev->next = new_node;
  new_node->prev = ref_node->prev;
  new_node->next = ref_node;
  ref_node->prev = new_node;
}

static void apr_list_raw_insert_node_after (
  apr_list_t* list,
  apr_list_node_t* ref_node,
  apr_list_node_t* new_node
)
{
  apr_list_raw_assert_node_is_linked( ref_node );
  apr_list_raw_assert_secure_node_is_unlinked( list, new_node );

  new_node->list = list;
  ref_node->next->prev = new_node;
  new_node->prev = ref_node;
  new_node->next = ref_node->next;
  ref_node->next = new_node;
}

static void apr_list_raw_delete_node (
  apr_list_node_t* node
)
{
  apr_list_raw_assert_node_is_linked( node );
    /* Verify that the header of the unlinking node was not already corrupted.
     * This helps catch buffer overflow attacks targeting the linked list.
     */

  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->prev = node;
  node->next = node;
}

static inline int32_t apr_list_raw_is_empty (
  apr_list_t* list
)
{
  return ( list->dummy.next == &list->dummy );
}

static inline int32_t apr_list_raw_is_not_empty (
  apr_list_t* list
)
{
  return ( list->dummy.next != &list->dummy );
}

static void apr_list_default_lock ( void )
{
}

static void apr_list_default_unlock ( void )
{
}

static int32_t apr_list_init_internal (
  apr_list_t* list,
  apr_list_lock_enter_fn_t lock_fn,
  apr_list_lock_leave_fn_t unlock_fn,
  bool_t en_secure
)
{
  if ( list == NULL )
  {
    return APR_EBADPARAM;
  }

  if ( ( ( lock_fn != NULL ) ^ ( unlock_fn != NULL ) ) != 0 )
  { /* Lock functions must be provided in pairs. */
    return APR_EBADPARAM;
  }

  list->dummy.prev = &list->dummy;
  list->dummy.next = &list->dummy;
  list->size = 0;

  if ( lock_fn != NULL )
  {
    list->lock_fn = lock_fn;
    list->unlock_fn = unlock_fn;
  }
  else
  {
    list->lock_fn = apr_list_default_lock;
    list->unlock_fn = apr_list_default_unlock;
  }

  list->en_secure = en_secure;

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_init (
  apr_list_t* list,
  apr_list_lock_enter_fn_t lock_fn,
  apr_list_lock_leave_fn_t unlock_fn
)
{
  return apr_list_init_internal( list, lock_fn, unlock_fn, FALSE );
}

APR_EXTERNAL int32_t apr_list_init_v2 (
  apr_list_t* list,
  apr_list_lock_enter_fn_t lock_fn,
  apr_list_lock_leave_fn_t unlock_fn
)
{
  return apr_list_init_internal( list, lock_fn, unlock_fn, TRUE );
}

APR_EXTERNAL int32_t apr_list_destroy (
  apr_list_t* list
)
{
  if ( list == NULL )
  {
    return APR_EBADPARAM;
  }

  list->dummy.prev = &list->dummy;
  list->dummy.next = &list->dummy;
  list->size = 0;
  list->lock_fn = apr_list_default_lock;
  list->unlock_fn = apr_list_default_unlock;

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_init_node (
  apr_list_node_t* node
)
{
  if ( node == NULL )
  {
    return APR_EBADPARAM;
  }

  node->list = NULL;
  node->prev = node;
  node->next = node;

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_add_head (
  apr_list_t* list,
  apr_list_node_t* node
)
{
  if ( ( list == NULL ) || ( node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  apr_list_raw_insert_node_after( list, &list->dummy, node );
  list->size += 1;

  list->unlock_fn( );

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_add_tail (
  apr_list_t* list,
  apr_list_node_t* node
)
{
  if ( ( list == NULL ) || ( node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  apr_list_raw_insert_node_before( list, &list->dummy, node );
  list->size += 1;

  list->unlock_fn( );

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_peak_head (
  apr_list_t* list,
  apr_list_node_t** ret_node
)
{
  if ( ( list == NULL ) || ( ret_node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  if ( apr_list_raw_is_empty( list ) )
  {
    list->unlock_fn( );

    return APR_EFAILED;
  }

  *ret_node = list->dummy.next;

  list->unlock_fn( );

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_peak_tail (
  apr_list_t* list,
  apr_list_node_t** ret_node
)
{
  if ( ( list == NULL ) || ( ret_node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  if ( apr_list_raw_is_empty( list ) )
  {
    list->unlock_fn( );

    return APR_EFAILED;
  }

  *ret_node = list->dummy.prev;

  list->unlock_fn( );

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_remove_head (
  apr_list_t* list,
  apr_list_node_t** ret_node
)
{
  apr_list_node_t* node;

  if ( list == NULL )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  if ( apr_list_raw_is_empty( list ) )
  {
    list->unlock_fn( );

    return APR_EFAILED;
  }

  node = list->dummy.next;
  apr_list_raw_delete_node( node );
  list->size -= 1;

  list->unlock_fn( );

  if ( ret_node != NULL )
  {
    *ret_node = node;
  }

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_remove_tail (
  apr_list_t* list,
  apr_list_node_t** ret_node
)
{
  apr_list_node_t* node;

  if ( list == NULL )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  if ( apr_list_raw_is_empty( list ) )
  {
    list->unlock_fn( );

    return APR_EFAILED;
  }

  node = list->dummy.prev;
  apr_list_raw_delete_node( node );
  list->size -= 1;

  list->unlock_fn( );

  if ( ret_node != NULL )
  {
    *ret_node = node;
  }

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_get_next (
  apr_list_t* list,
  apr_list_node_t* pivot_node,
  apr_list_node_t** ret_node
)
{
  apr_list_node_t* node;

  if ( ( list == NULL ) || ( pivot_node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  /* list->lock_fn( ); */ /* Assume the caller ensures thread safety. */

  node = pivot_node->next;

  /* list->unlock_fn( ); */

  if ( ret_node != NULL )
  {
    *ret_node = node;
  }

  if ( node == &list->dummy )
  { /* No more iterations. */
    return APR_EFAILED;
  }
  else
  {
    return APR_EOK;
  }
}

APR_EXTERNAL int32_t apr_list_get_prev (
  apr_list_t* list,
  apr_list_node_t* pivot_node,
  apr_list_node_t** ret_node
)
{
  apr_list_node_t* node;

  if ( ( list == NULL ) || ( pivot_node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  /* list->lock_fn( ); */ /* Assume the caller ensures thread safety. */

  node = pivot_node->prev;

  /* list->unlock_fn( ); */

  if ( ret_node != NULL )
  {
    *ret_node = node;
  }

  if ( node == &list->dummy )
  { /* No more iterations. */
    return APR_EFAILED;
  }
  else
  {
    return APR_EOK;
  }
}

APR_EXTERNAL int32_t apr_list_delete (
  apr_list_t* list,
  apr_list_node_t* node
)
{
  if ( ( list == NULL ) || ( node == NULL ) )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  apr_list_raw_delete_node( node );
  list->size -= 1;

  list->unlock_fn( );

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_list_clear (
  apr_list_t* list
)
{
  apr_list_node_t* node;

  if ( list == NULL )
  {
    return APR_EBADPARAM;
  }

  list->lock_fn( );

  while ( apr_list_raw_is_not_empty( list ) )
  {
    node = list->dummy.next;
    apr_list_raw_delete_node( node );
  }

  list->size = 0;

  list->unlock_fn( );

  return APR_EOK;
}

