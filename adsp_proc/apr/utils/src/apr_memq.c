/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/utils/src/apr_memq.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include <stdlib.h>
#include "comdef.h"
#include "mmstd.h"
#include "msg.h"
#include "aprv2_msg_if.h"
#include "apr_errcodes.h"
#include "apr_memq.h"
#include "apr_log.h"
#include "apr_dbg_msg.h"

/*
** Helper Functions  
*/
uint32_t apr_memq_size_of_metadata_and_overhead ( 
  apr_memq_block_t* block 
)
{
  return ( ( APR_MEMQ_BYTES_PER_METADATA_ITEM_V * block->metadata_size ) + \
           APR_MEMQ_UNIT_OVERHEAD_V );
}

uint32_t apr_memq_is_metadata_unique (
  apr_memq_block_t* block,
  uint32_t total_unique_mds,
  int32_t metadata
)
{
  uint32_t md_index;

  for ( md_index=0; md_index < total_unique_mds; md_index++ )
  {
    if ( block->unique_metadata_ids[ md_index ] == metadata )
    {
      block->unique_metadata_counts[ md_index ]++; 
      return 0;
    }
  }

  return 1;
}

/*
** Memory Queue Routines
*/

/* The caller provides the contiguous virtual memory for partitioning into
** fixed memory allocation units. The unit_size is the amount of usable
** application memory for each allocation unit plus overhead
** (APR_MEMQ_UNIT_OVERHEAD).
*/
APR_EXTERNAL int apr_memq_init (
  apr_memq_block_t* block,
  char_t* heap_base,
  uint32_t heap_size,
  uint32_t unit_size,
  uint32_t metadata_size,
  apr_memq_lock_enter_fn_t lock_fn,
  apr_memq_lock_leave_fn_t unlock_fn
)
{
  uint32_t checkpoint = 0;

  for ( ;; )
  {
    if ( block == NULL )
    {
      return APR_EBADPARAM;
    }

    if ( unit_size < APR_MEMQ_BYTES_PER_METADATA_ITEM_V * metadata_size )
    {
      return APR_EBADPARAM;
    }

    if ( ( ( lock_fn != NULL ) ^ ( unlock_fn != NULL ) ) != 0 )
    {
      return APR_EBADPARAM;
    }

    /* Partition the heap into fixed units in the free queue */
    ( void ) apr_list_init_v2( &block->free_q, lock_fn, unlock_fn );
    block->base_addr = heap_base;
    block->unit_size = unit_size;
    block->metadata_size = metadata_size;

    while ( heap_size >= unit_size )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) heap_base );
      ( void ) apr_list_add_tail( &block->free_q, ( ( apr_list_node_t* ) heap_base ) );
      heap_base += unit_size;
      heap_size -= unit_size;
    }

    block->total_units = block->free_q.size;

    /* Allocate memory to be used for out-of-memory detection */
    block->unique_metadata_ids =
      ( ( uint32_t* ) malloc( block->total_units * APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) ); /* TODO: Remove malloc() dependency. Metadata should be static size not dynamic. */
    if ( block->unique_metadata_ids == NULL )
    {
      APR_DBG_0(APR_DBG_ERROR, "Cannot allocate memory for oom analysis");
      break;
    }
    checkpoint = 1;

    block->unique_metadata_counts =
      ( ( uint32_t* ) malloc( block->total_units * APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) ); /* TODO: Remove malloc() dependency. Metadata should be static size not dynamic. */
    if ( block->unique_metadata_counts == NULL )
    {
      APR_DBG_0(APR_DBG_ERROR, "Cannot allocate memory for oom analysis");
      break;
    }

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 1:
    free( block->unique_metadata_ids );
    /*-fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

/* This function must be called from task context */
APR_EXTERNAL void apr_memq_deinit (
  apr_memq_block_t* block
)
{
  if ( block == NULL )
  {
    APR_DBG_0(APR_DBG_ERROR, "block is NULL");
    return;
  }

  if ( block->free_q.size != block->total_units )
  {
    APR_DBG_0(APR_DBG_ERROR, "memory leak detected");
  }

  apr_list_clear( &block->free_q );

  free( block->unique_metadata_ids );
  free( block->unique_metadata_counts );
}

APR_EXTERNAL void* apr_memq_alloc (
  apr_memq_block_t* block
)
{
  apr_list_node_t* node;
  char_t* mem_ptr;
  uint32_t md_interator;
  uint32_t md_index;
  int32_t metadata=0;
  uint32_t total_unique_mds = 0;

  if ( apr_list_remove_head( &block->free_q, &node ) == APR_EOK )
  {
    return ( ( ( char_t* ) node ) + apr_memq_size_of_metadata_and_overhead ( block ) );
  }

  APR_DBG_0(APR_DBG_ERROR, "Out of memory failure");

  mmstd_memset( block->unique_metadata_ids, 0xFF, 
                block->total_units * APR_MEMQ_BYTES_PER_METADATA_ITEM_V );
  mmstd_memset( block->unique_metadata_counts, 0, 
                block->total_units * APR_MEMQ_BYTES_PER_METADATA_ITEM_V );

  mem_ptr = block->base_addr + apr_memq_size_of_metadata_and_overhead ( block );

  for ( md_interator=0; md_interator < block->total_units; md_interator++ )
  {
    apr_memq_node_get_metadata ( block, mem_ptr, 0, &metadata );
    mem_ptr = mem_ptr + block->unit_size;
    if ( apr_memq_is_metadata_unique( block, total_unique_mds, metadata ) )
    {
      block->unique_metadata_ids[ total_unique_mds ] = metadata;
      block->unique_metadata_counts[ total_unique_mds ] = 1;
      total_unique_mds++;
    }
  }

  for ( md_index=0; md_index < total_unique_mds; md_index++ )
  {
    APR_DBG_2(APR_DBG_HIGH, "Metadata: %08X  Count: %d", 
              block->unique_metadata_ids[ md_index ], 
              block->unique_metadata_counts[ md_index ]);
  }
  apr_log_out_of_memory_error ( block->unique_metadata_ids, 
                                block->unique_metadata_counts, 
                                total_unique_mds );

  return NULL;
}

APR_EXTERNAL void apr_memq_free (
  apr_memq_block_t* block,
  void* data_ptr
)
{
  apr_list_node_t* node;

  if ( block == NULL )
  {
    APR_DBG_0(APR_DBG_ERROR, "block is NULL");
    return;
  }

  if ( data_ptr == NULL )
  {
    APR_DBG_0(APR_DBG_HIGH, "data_ptr is NULL");
    return;
  }

  node = ( ( apr_list_node_t* )
           ( ( ( char_t* ) data_ptr ) - apr_memq_size_of_metadata_and_overhead ( block ) ) );
  ( void ) apr_list_add_tail( &block->free_q, node );
}

APR_EXTERNAL int32_t apr_memq_node_set_metadata (
  apr_memq_block_t* block,
  void* mem_ptr,
  uint32_t index,
  int32_t value
)
{
  int32_t* metadata;

  if ( index >= block->metadata_size )
  {
    return APR_EBADPARAM;
  }

  metadata = ( ( int32_t* )
               ( ( ( char_t* ) mem_ptr ) - 
                 ( ( block->metadata_size - index ) * 
                   APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) ) );

  *metadata = value;

  return APR_EOK;
}

APR_EXTERNAL int32_t apr_memq_node_get_metadata (
  apr_memq_block_t* block,
  void* mem_ptr,
  uint32_t index,
  int32_t* metadata
)
{
  if ( index >= block->metadata_size )
  {
    return APR_EBADPARAM;
  }

  *metadata = *( ( int32_t* )
                 ( ( ( char_t* ) mem_ptr ) - 
                   ( ( block->metadata_size - index ) * 
                     APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) ) );

  return APR_EOK;
}

