/*
  Copyright (C) 2010, 2011 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/src/aprv2_rtable.c#1 $
  $Author: pwbldsvc $
*/

#include <stdlib.h>
#include "mmstd.h"
#include "msg.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "aprv2_ids_domains.h"
#include "aprv2_ids_services.h"
#include "aprv2_pl_config.h"
#include "aprv2_rtable_api_i.h"
#include "aprv2_ipc.h"
/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define APRV2_RTABLE_MAKE_ADDR( domain_id, service_id ) \
  ( ( domain_id << 8 ) | service_id )

#define APRV2_RTABLE_DISPATCH_TABLE_SIZE( last_static_id, num_dynamic_ids ) \
  { ( last_static_id + 1 ), ( last_static_id + num_dynamic_ids + 1 ) }

typedef struct aprv2_rtable_routing_table_size_t aprv2_rtable_routing_table_size_t;
struct aprv2_rtable_routing_table_size_t
{
  uint16_t num_static_services;
  uint16_t num_total_services;
};

typedef struct aprv2_rtable_local_dns_entry_t aprv2_rtable_local_dns_entry_t;
struct aprv2_rtable_local_dns_entry_t
{
  uint16_t addr;
  char_t* dns;
};

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static aprv2_rtable_routing_table_size_t aprv2_rtable_routing_table_size[ APRV2_PL_NUM_TOTAL_DOMAINS_V ] =
{
  APRV2_PL_ROUTING_TABLE_SIZE_ARRAY_V
};

static aprv2_rtable_service_entry_t* aprv2_rtable_routing_table[ APRV2_PL_NUM_TOTAL_DOMAINS_V ];

static aprv2_rtable_local_dns_entry_t aprv2_rtable_local_dns_table[] =
{
  APRV2_PL_LOCAL_DNS_ARRAY_V
};

/*****************************************************************************
 * Implementations                                                           *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_rtable_get_default_domain (
  uint16_t* ret_domain_id
)
{
   return aprv2_ipc_get_default_domain_id ( ret_domain_id );
}

APR_INTERNAL int32_t aprv2_rtable_get_num_static_services (
  uint16_t domain_id,
  uint16_t* ret_count
)
{
  if ( ret_count == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_count = aprv2_rtable_routing_table_size[ domain_id ].num_static_services;

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_rtable_get_num_total_services (
  uint16_t domain_id,
  uint16_t* ret_count
)
{
  if ( ret_count == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_count = aprv2_rtable_routing_table_size[ domain_id ].num_total_services;

  return APR_EOK;
}

static void aprv2_rtable_free_routing_table ( void )
{
  uint16_t domain_index;
  aprv2_rtable_service_entry_t** dispatch_table;

  for ( domain_index = 0 ; domain_index < APRV2_PL_NUM_TOTAL_DOMAINS_V; ++domain_index )
  {
    dispatch_table = &aprv2_rtable_routing_table[ domain_index ];
    if ( *dispatch_table != NULL )
    {
      free( *dispatch_table ); /* FIXME: Use from a static memory pool instead of dynamic. */
      *dispatch_table = NULL;
    }
  }
}

APR_INTERNAL int32_t aprv2_rtable_local_dns_lookup (
  char_t* name,
  uint32_t size,
  uint16_t* ret_addr
)
{
  uint32_t num_entries;
  uint32_t index;
  aprv2_rtable_local_dns_entry_t* cur_entry;

  if ( ( name == NULL ) || ( ret_addr == NULL ) )
  {
    return APR_EBADPARAM;
  }

  /* TODO: This routine could use some optimization! The caller should call
   *       this routine during initialization because it is very expensive.
   */

  num_entries = ( sizeof( aprv2_rtable_local_dns_table ) / sizeof( aprv2_rtable_local_dns_entry_t ) );

  for ( index = 0; index < num_entries; ++index )
  {
    cur_entry = &aprv2_rtable_local_dns_table[ index ];

    if ( mmstd_strncmp( cur_entry->dns, -1, name, size ) == 0 )
    {
      *ret_addr = cur_entry->addr;

      return APR_EOK;
    }
  }

  return APR_EUNSUPPORTED;
}

APR_INTERNAL int32_t aprv2_rtable_get_service (
  uint16_t domain_id,
  uint16_t service_id,
  aprv2_rtable_service_entry_t** ret_entry
)
{
  if ( ret_entry == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_entry = &aprv2_rtable_routing_table[ domain_id ][ service_id ];

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_rtable_get_num_total_domains (
  uint16_t* ret_count
)
{
  if ( ret_count == NULL )
  {
    return APR_EBADPARAM;
  }

  *ret_count = ( APRV2_PL_MAX_DOMAIN_ID_V + 1 );

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_rtable_init (
  aprv2_dispatch_fn_t default_dispatch_fn,
  void* default_dispatch_data
)
{
  uint16_t domain_index;
  uint16_t num_total_services;
  uint32_t table_size;
  aprv2_rtable_service_entry_t** dispatch_table;
  uint16_t service_index;
  aprv2_rtable_service_entry_t* service;

  /* Allocate routing table memory for every supported domain. */
  for ( ;; )
  {
    for ( domain_index = 0; domain_index < APRV2_PL_NUM_TOTAL_DOMAINS_V; ++domain_index )
    {
      /* Allocate the dispatch table for the current domain. */
      dispatch_table = &aprv2_rtable_routing_table[ domain_index ];

      ( void ) aprv2_rtable_get_num_total_services( domain_index, &num_total_services );
      table_size = ( sizeof( aprv2_rtable_service_entry_t ) * num_total_services );

      *dispatch_table = ( ( aprv2_rtable_service_entry_t* ) malloc( table_size ) );  /* FIXME: Use from a static memory pool instead of dynamic. */
      if ( *dispatch_table == NULL ) goto label_error;

      /* Initialize the dispatch table for the current domain. */
      for ( service_index = 0; service_index < num_total_services; ++service_index )
      {
        service = &( *dispatch_table )[ service_index ];
        service->dispatch_fn = default_dispatch_fn;
        service->dispatch_data = default_dispatch_data;
        ( void ) mmstd_memset( service->name, 0, sizeof( service->name ) );
      }
    }

    return APR_EOK;
  }
label_error:

  /* Perform error clean-up. */
  aprv2_rtable_free_routing_table( );

  return APR_EFAILED;
}

APR_INTERNAL int32_t aprv2_rtable_deinit ( void )
{
  aprv2_rtable_free_routing_table( );

  return APR_EOK;
}

