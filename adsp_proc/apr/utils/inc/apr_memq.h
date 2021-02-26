#ifndef __APR_MEMQ_H__
#define __APR_MEMQ_H__

/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/inc/apr_memq.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"
#include "apr_list.h"

typedef void ( *apr_memq_lock_enter_fn_t ) ( void );
typedef void ( *apr_memq_lock_leave_fn_t ) ( void );

/*
** Memory Queue Definitions
*/

typedef struct apr_memq_block_t
{
  apr_list_t free_q;
  char_t* base_addr;
  uint32_t total_units;
  uint32_t unit_size;
  uint32_t metadata_size;
  uint32_t* unique_metadata_ids;
  uint32_t* unique_metadata_counts;
}
  apr_memq_block_t;

typedef struct apr_memq_entry_t
{
  apr_list_node_t link; /* Must be first */
}
  apr_memq_entry_t;

#define APR_MEMQ_BYTES_PER_METADATA_ITEM_V ( sizeof( int32_t ) )
#define APR_MEMQ_UNIT_OVERHEAD_V ( sizeof ( apr_memq_entry_t ) )

/*
** Memory Queue Prototypes
*/

APR_EXTERNAL int apr_memq_init (
  apr_memq_block_t* block,
  char_t* heap_base,
  uint32_t heap_size,
  uint32_t unit_size,
  uint32_t metadata_size,
  apr_memq_lock_enter_fn_t lock_fn,
  apr_memq_lock_leave_fn_t unlock_fn
);
APR_EXTERNAL void apr_memq_deinit (
  apr_memq_block_t* block
);
APR_EXTERNAL void* apr_memq_alloc (
  apr_memq_block_t* block
);
APR_EXTERNAL void apr_memq_free (
  apr_memq_block_t* block,
  void* mem_ptr
);
APR_EXTERNAL int32_t apr_memq_node_set_metadata (
  apr_memq_block_t* block,
  void* mem_ptr,
  uint32_t index,
  int32_t value
);
APR_EXTERNAL int32_t apr_memq_node_get_metadata (
  apr_memq_block_t* block,
  void* mem_ptr,
  uint32_t index,
  int32_t* metadata
);

#endif /* __APR_MEMQ_H__ */

