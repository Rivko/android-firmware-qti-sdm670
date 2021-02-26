#ifndef __APR_LIST_H__
#define __APR_LIST_H__

/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/inc/apr_list.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

typedef struct apr_list_node_t apr_list_node_t;
typedef struct apr_list_t apr_list_t;

typedef void ( *apr_list_lock_enter_fn_t ) ( void );
typedef void ( *apr_list_lock_leave_fn_t ) ( void );

struct apr_list_node_t
{
  apr_list_t* list;
    /**< Indicates the last known list for which this node belonged. */
  apr_list_node_t* prev;
    /**< The previous node. */
  apr_list_node_t* next;
    /**< The next node. */
};

struct apr_list_t
{
  apr_list_node_t dummy;
    /**< A special dummy node to implement the NULL object pattern. */
  uint32_t size;
    /**< The number of nodes linked to this linked list. */

  apr_list_lock_enter_fn_t lock_fn;
    /**< The user supplied lock implementation function. */
  apr_list_lock_leave_fn_t unlock_fn;
    /**< The user supplied unlock implementation function. */

  bool_t en_secure;
    /**< Enable secure linked list flag. */
};

APR_EXTERNAL int32_t apr_list_init ( apr_list_t* list, apr_list_lock_enter_fn_t lock_fn, apr_list_lock_leave_fn_t unlock_fn );
APR_EXTERNAL int32_t apr_list_init_v2 ( apr_list_t* list, apr_list_lock_enter_fn_t lock_fn, apr_list_lock_leave_fn_t unlock_fn ); /* Turns on secure linked-list mode. */
APR_EXTERNAL int32_t apr_list_destroy ( apr_list_t* list );
APR_EXTERNAL int32_t apr_list_init_node ( apr_list_node_t* node ); /* Initializes the node's data structure. Must call when using apr_list_init_v2(). */
APR_EXTERNAL int32_t apr_list_add_head ( apr_list_t* list, apr_list_node_t* node );
APR_EXTERNAL int32_t apr_list_add_tail ( apr_list_t* list, apr_list_node_t* node );
APR_EXTERNAL int32_t apr_list_peak_head ( apr_list_t* list, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_peak_tail ( apr_list_t* list, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_remove_head ( apr_list_t* list, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_remove_tail ( apr_list_t* list, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_get_next( apr_list_t* list, apr_list_node_t* pivot_node, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_get_prev( apr_list_t* list, apr_list_node_t* pivot_node, apr_list_node_t** ret_node );
APR_EXTERNAL int32_t apr_list_delete( apr_list_t* list, apr_list_node_t* node );
APR_EXTERNAL int32_t apr_list_clear ( apr_list_t* list );

#endif /* __APR_LIST_H__ */

