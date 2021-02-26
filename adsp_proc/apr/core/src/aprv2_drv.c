/*
  Copyright (C) 2010-2012, 2013,2016,2017 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/src/aprv2_drv.c#1 $
  $Author: pwbldsvc $
*/


#include "qurt.h" /* qurt_sysclock_get_hw_ticks(); */

/* DIAG includes. */
#include "err.h"
#include "msg.h"
/* MM standard includes. */
#include "mmstd.h"
/* Definition includes. */
#include "apr_errcodes.h"
#include "aprv2_packet.h"
#include "aprv2p1_packet.h"
#include "aprv2_ids_services.h"
/* API includes. */
#include "apr_atomic.h"
#include "apr_lock.h"
#include "apr_memq.h"
#include "apr_log.h"
#include "aprv2_api.h"
#include "aprv2_rtable_api_i.h"
#include "aprv2_ipc.h"
#include "aprv2_api_i.h"
#include "aprv2_mpd_i.h"
#include "apr_dbg_msg.h"
/* Interface includes. */
#include "aprv2_msg_if.h"
#include "apr_misc.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APRV2_DRV_NUM_PACKETS_V ( 100 )
#define APRV2_DRV_BYTES_PER_PACKET_V ( 512 )
#define APRV2_DRV_METADATA_ITEMS_V ( 1 )
#define APRV2_DRV_BYTES_PER_MEMQ_ITEM_V \
  ( APR_MEMQ_UNIT_OVERHEAD_V + APRV2_DRV_BYTES_PER_PACKET_V + \
  ( APRV2_DRV_METADATA_ITEMS_V * APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) )

#define APRV2_4K_NUM_PACKETS_V ( 4 )
#define APRV2_DRV_4KB_PACKET_SIZE_V ( 4096 )
#define APRV2_4K_MEMQ_SIZE \
  ( APR_MEMQ_UNIT_OVERHEAD_V + APRV2_DRV_4KB_PACKET_SIZE_V  + \
  ( APRV2_DRV_METADATA_ITEMS_V * APR_MEMQ_BYTES_PER_METADATA_ITEM_V ) )

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static bool_t aprv2_drv_debug_enable_error_panic = FALSE;
  /* Enable panic on error. */

static apr_lock_t aprv2_drv_task_lock;
static apr_lock_t aprv2_drv_isr_lock;

static char aprv2_drv_packet_heap[ APRV2_DRV_NUM_PACKETS_V * APRV2_DRV_BYTES_PER_MEMQ_ITEM_V ];
static apr_memq_block_t aprv2_drv_free_packets_memq;

static char aprv2_4k_drv_packet_heap[ APRV2_4K_NUM_PACKETS_V * APRV2_4K_MEMQ_SIZE ];
static apr_memq_block_t aprv2_4k_drv_free_packets_memq;

static uint16_t aprv2_drv_trace = 0;
  /**<
   * The trace value is largely undefined. We will use the PKTV1 32-bit trace
   * structure this way until something better is defined:
   *
   *  31                24 | 23                        8 | 7                0
   *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   *  |    8-bit domain    |  16-bit sequence number     | 8-bit hop number  |
   *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   *
   * New packets should stamp the 8-bit domain and an incremented sequence
   * number generated from the same domain. The hop number should be set to 0.
   *
   * On each packet forward, the domain and the sequence number stays the same,
   * but the hop number should increment by 1. It is assumed that the system
   * design does not require more than 256 hops; in case more than 256-hops are
   * required then the hop number should perform an 8-bit wrap-around and the
   * analyzer should rely other sources of information to determine how to
   * reconstruct the log.
   */


/*****************************************************************************
 * Core Routine Implementations                                              *
 ****************************************************************************/

static void aprv2_drv_task_lock_fn ( void )
{
  ( void ) apr_lock_enter( aprv2_drv_task_lock );
}

static void aprv2_drv_task_unlock_fn ( void )
{
  ( void ) apr_lock_leave( aprv2_drv_task_lock );
}

static void aprv2_drv_isr_lock_fn ( void )
{
  ( void ) apr_lock_enter( aprv2_drv_isr_lock );
}

static void aprv2_drv_isr_unlock_fn ( void )
{
  ( void ) apr_lock_leave( aprv2_drv_isr_lock );
}

static int32_t aprv2_drv_default_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  aprv2_cmd_end_command_t end_args;

  APR_DBG_1(APR_DBG_ERROR, "apr_async_send(0x%08X) dispatch_fn=NULL", packet);

  end_args.handle = 0x0; /* Use Null handle for freeing packet for unregisterd service. */
  end_args.packet = packet;
  end_args.status = APR_ENOSERVICE;
  ( void ) aprv2_cmd_end_command( &end_args );

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_drv_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;

  for ( ;; )
  {
    rc =  aprv2_rtable_init( aprv2_drv_default_dispatch_fn, NULL );
    if ( rc )
    {
      break;
    }
    checkpoint = 1;

    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &aprv2_drv_task_lock );
    if ( rc )
    {
      break;
    }
    checkpoint = 2;

    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &aprv2_drv_isr_lock );
    if ( rc )
    {
      break;
    }
    checkpoint = 3;

    rc = apr_memq_init(
           &aprv2_drv_free_packets_memq, aprv2_drv_packet_heap,
           sizeof( aprv2_drv_packet_heap ), APRV2_DRV_BYTES_PER_MEMQ_ITEM_V,
           APRV2_DRV_METADATA_ITEMS_V, aprv2_drv_isr_lock_fn,
           aprv2_drv_isr_unlock_fn
         );
    if ( rc )
    {
      break;
    }
    checkpoint = 4;

    rc = apr_memq_init(
           &aprv2_4k_drv_free_packets_memq, aprv2_4k_drv_packet_heap,
           sizeof( aprv2_4k_drv_packet_heap ), APRV2_4K_MEMQ_SIZE,
           APRV2_DRV_METADATA_ITEMS_V, aprv2_drv_isr_lock_fn,
           aprv2_drv_isr_unlock_fn
         );
    if ( rc )
    {
      break;
    }
    checkpoint = 5;

    rc = aprv2_mpd_init();
    if( rc)
    {
      break;
    }
    checkpoint = 6; 

    rc = aprv2_ipc_init( );
    if ( rc )
    {
      break;
    }
    checkpoint = 7;

    return APR_EOK; 
  }

  switch ( checkpoint )
  {
  case 7:
    ( void ) aprv2_ipc_deinit( );
    /*-fallthru */
  case 6: 
    ( void ) aprv2_mpd_deinit();
    /*-fallthru */
  case 5:
    apr_memq_deinit( &aprv2_4k_drv_free_packets_memq );
    /*-fallthru */
  case 4:
    apr_memq_deinit( &aprv2_drv_free_packets_memq );
    /*-fallthru */
  case 3:
    ( void ) apr_lock_destroy( aprv2_drv_isr_lock );
    /*-fallthru */
  case 2:
    ( void ) apr_lock_destroy( aprv2_drv_task_lock );
    /*-fallthru */
  case 1:
    ( void ) aprv2_rtable_deinit( );
    /*-fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t aprv2_drv_deinit ( void )
{
  ( void ) aprv2_mpd_deinit();
  ( void ) aprv2_ipc_deinit( );
  apr_memq_deinit( &aprv2_4k_drv_free_packets_memq );
  apr_memq_deinit( &aprv2_drv_free_packets_memq );
  ( void ) apr_lock_destroy( aprv2_drv_isr_lock );
  ( void ) aprv2_rtable_deinit();
  return APR_EOK;
}

/*****************************************************************************
 * Fundamental Controls Implementation                                       *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_cmd_register (
  aprv2_cmd_register_t* params
)
{
  aprv2_cmd_register2_t args;

  if ( params == NULL )
  {
    return APR_EBADPARAM;
  }

  args.ret_handle = params->ret_handle;
  args.name = NULL;
  args.name_size = 0;
  args.addr = params->addr;
  args.dispatch_fn = params->dispatch_fn;
  args.dispatch_data = params->dispatch_data;
  args.ret_addr = NULL;

  return aprv2_cmd_register2( &args );
}

APR_INTERNAL int32_t aprv2_cmd_register2 (
  aprv2_cmd_register2_t* params
)
{
  int32_t rc;
  uint16_t my_addr;
  uint16_t my_domain_id;
  uint16_t my_service_id;
  aprv2_rtable_service_entry_t* session = NULL;
  aprv2_cmd_local_dns_lookup_t dns_lookup;
  uint16_t num_static_services;
  uint16_t num_total_services;

  if ( ( params == NULL ) || ( params->ret_handle == NULL ) ||
       ( params->dispatch_fn == NULL ) )
  {
    return APR_EBADPARAM;
  }

  if ( params->name_size > APRV2_RTABLE_MAX_DNS_SIZE )
  {
    /* Not enough space for the NULL character. */
    return APR_EBADPARAM;
  }

  /* Service `name' and `addr' resolution logic:
   *
   *   1. If the service name is provided and the addr is 0 then register by
   *      DNS lookup.
   *        a. Static services will get assigned static addresses.
   *        b. Dynamic services will get assigned free dynamic addresses.
   *
   *      Rationale:
   *        Registration by name allows the registrant to be address agnostic.
   *        This feature allows moving code around to different process
   *        domains with minimal branching or recompilations needed. The
   *        aprv2_ids.c stores the configurable static mappings.
   *
   *      Rationale:
   *        Registration by name allows dynamic services to be discovered by
   *        remote clients at runtime.
   *
   *   2. If the service name is provided and the addr is not 0 then register
   *      by the addr.
   *
   *      Rationale:
   *        There are no real use cases for this case at present.
   *
   *        Statically assigned services could register with an alias name.
   *
   *   3. If the service name is empty and the addr is 0 then generate
   *      a new service ID without a service name.
   *
   *      Rationale:
   *        Application clients require only unique addresses to communicate
   *        with servers. Publication of names for remote resource discovery
   *        is unnecessary and is not needed.
   *
   *   4. If service name is empty and addr is not 0 then register by the
   *      addr.
   *
   *      Rationale:
   *        Statically assigned service addresses do not need to be
   *        dynamically discovered. Registering a service name for statically
   *        assigned services is not mandatory.
   */
  if ( ( params->name != NULL ) && ( params->name_size > 0 ) )
  { /* Case 1 and 2. */
    if ( params->addr == 0 )
    { /* Case 1. */
      dns_lookup.name = params->name;
      dns_lookup.name_size = params->name_size;
      dns_lookup.ret_addr = &my_addr;
      rc = aprv2_cmd_local_dns_lookup( &dns_lookup );
      if ( rc )
      {
        if ( rc == APR_ENOTEXIST )
        {
          my_addr = 0;
        }
        else
        {
          return APR_EFAILED;
        }
      }
    }
    else
    { /* Case 2. */
      return APR_EUNSUPPORTED;
    }
  }
  else
  { /* Case 3 and 4. */
    my_addr = params->addr;
  }

  my_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, my_addr );
  my_service_id = APR_GET_FIELD( APRV2_PKT_SERVICE_ID, my_addr );

  if ( my_domain_id == 0 )
  {
    ( void ) aprv2_rtable_get_default_domain( &my_domain_id );
  }

  aprv2_drv_task_lock_fn( );

  for ( ;; )
  {
    /* Use or assign a new service ID. */
    if ( my_service_id != 0 )
    {
      /* Check that the preferred service ID must remain under the static service ID range. */
      ( void ) aprv2_rtable_get_num_static_services( my_domain_id, &num_static_services );
      if ( my_service_id >= num_static_services )
      {
        rc = APR_EBADPARAM;
        goto label_error;
      }

      ( void ) aprv2_rtable_get_service( my_domain_id, my_service_id, &session );
      if ( session->dispatch_fn != aprv2_drv_default_dispatch_fn )
      {
        rc = APR_EALREADY;
        goto label_error;
      }
    }
    else
    {
      /* Find a free dynamic address searching from the highest to lowest service ID order. */
      ( void ) aprv2_rtable_get_num_static_services( my_domain_id, &num_static_services );
      ( void ) aprv2_rtable_get_num_total_services( my_domain_id, &num_total_services );

      for ( my_service_id = ( num_total_services - 1 ); ; --my_service_id )
      {
        if ( my_service_id < num_static_services )
        {
          rc = APR_ENORESOURCE;
          goto label_error;
        }

        ( void ) aprv2_rtable_get_service( my_domain_id, my_service_id, &session );
        if ( session->dispatch_fn == aprv2_drv_default_dispatch_fn )
        {
          break;
        }
      }
    }

    /* Register the service. */
    session->dispatch_fn = params->dispatch_fn;
    session->dispatch_data = params->dispatch_data;

    if ( ( params->name != NULL ) && ( params->name_size > 0 ) )
    {
      mmstd_strlcpy( session->name, params->name,
                     MMSTD_MIN( sizeof( session->name ), params->name_size ) );
    }

    *params->ret_handle = ( ( uint32_t ) ( ( my_domain_id << 8 ) | my_service_id ) );

    if ( params->ret_addr != NULL )
    {
      *params->ret_addr = ( ( uint16_t ) *params->ret_handle );
    }

    rc = APR_EOK;
    break;
  }

  /* Register with MPD module */
  rc = aprv2_mpd_register(*params->ret_handle);

label_error:
  aprv2_drv_task_unlock_fn();

  return rc;
}

APR_INTERNAL int32_t aprv2_cmd_deregister (
  aprv2_cmd_deregister_t* params
)
{
  int32_t rc;
  uint16_t my_domain_id;
  uint16_t my_service_id;
  uint16_t num_total_services;
  aprv2_rtable_service_entry_t* session;

  if ( params == NULL )
  {
    return APR_EBADPARAM;
  }

  my_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID,
                                ( ( uint16_t ) params->handle ) );
  my_service_id = APR_GET_FIELD( APRV2_PKT_SERVICE_ID,
                                 ( ( uint16_t ) params->handle ) );

  rc = aprv2_ipc_is_domain_local( my_domain_id );
  if ( rc )
  {
    return APR_EBADPARAM;
  }

  ( void ) aprv2_rtable_get_num_total_services( my_domain_id, &num_total_services );
  if ( my_service_id >= num_total_services )
  {
    return APR_EBADPARAM;
  }

  ( void ) aprv2_rtable_get_service( my_domain_id, my_service_id, &session );

  aprv2_drv_task_lock_fn( );

  session->dispatch_fn = aprv2_drv_default_dispatch_fn;
  session->name[0] = 0;
    /* Setting the first character to NULL erases the name; the junk after
     * the first character will still be useful for debugging the last dynamic
     * service using this session slot.
     */

  aprv2_drv_task_unlock_fn( );

  /* De-Register with MPD module */
  rc = aprv2_mpd_deregister(params->handle);

  return APR_EOK;
}

static int32_t aprv2_drv_trace_initialize (
  uint32_t handle,
  uint32_t* ret_trace /* Assume ret_trace is not NULL. */
)
{
  aprv2_cmd_query_address_t args;
  uint16_t my_addr;
  uint16_t my_domain_id;

  args.handle = handle;
  args.ret_addr = &my_addr;
  ( void ) aprv2_cmd_query_address( &args );
  my_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, my_addr );
  ( void ) apr_atomic_int16_add( ( ( int16_t* ) &aprv2_drv_trace ), 1, NULL );
  *ret_trace = ( ( my_domain_id << 24 ) | ( aprv2_drv_trace << 8 ) );

  return APR_EOK;
}

static int32_t aprv2_drv_trace_inc_hop (
  uint32_t* ret_trace /* Assume ret_trace is not NULL. */
)
{
  uint8_t next_hop = ( ( uint8_t ) ( ( *ret_trace ) + 1 ) );

  *ret_trace = ( ( ( ( *ret_trace ) >> 8 ) << 8 ) | next_hop );

  return APR_EOK;
}

static int32_t aprv2_drv_trace_update (
  aprv2_packet_t* packet
)
{
  switch ( APR_GET_FIELD( APRV2_PKT_VERSION, packet->header ) )
  {
  case APRV2P1_PKT_VERSION_V:
    {
      if (APRV2P1_PKT_GET_HEADER_BYTE_SIZE(packet->header) >= sizeof(aprv2p1_packet_t))
      {
        aprv2p1_packet_t* typed_packet = ( ( aprv2p1_packet_t* ) packet );
        ( void ) aprv2_drv_trace_inc_hop( &typed_packet->trace );
#ifdef APR_PROFILING
        //get time stamp 
        uint64_t timestamp = apr_get_clock_ticks();
        typed_packet->timediff = timestamp - typed_packet->timestamp ;
        typed_packet->timestamp = timestamp;
#else
        typed_packet->timestamp = QURT_SYSCLOCK_TIMETICK_TO_US(qurt_sysclock_get_hw_ticks());
#endif
      }
    }
    break;

  default:
    break;
  }

  return APR_EOK;
}

/*
  NOTE: When the call to dispatch_fn() returns a failure then it means the
        receiver is unable to deliver the packet. It is the caller's
        responsibilty to free the undelivered packet and perform any error
        handling. When the caller is the APR (on behalf of the remote
        procedure call) the APR should consume the packet and reply an
        error response packet to the source. All senders will need to
        understand the APRV2_IBASIC_RSP_ACK response even when the
        interface definition specifies to wait for custom responses.
*/
APR_INTERNAL int32_t aprv2_cmd_async_send (
  aprv2_cmd_async_send_t* params
)
{
  int32_t rc;
  uint32_t handle;
  int32_t packet_len;
  aprv2_packet_t* packet;
  uint16_t dst_domain_id;
  uint16_t dst_service_id;
  uint16_t num_total_services;
  aprv2_rtable_service_entry_t* session;
  apr_memq_block_t* block;

  if ( params == NULL )
  {
    return APR_EBADPARAM;
  }

  handle = params->handle;
  packet = params->packet;

  if ( packet == NULL )
  {
    APR_DBG_1(APR_DBG_ERROR, "send_error(%08X,NULL)", handle);
    return APR_EBADPARAM;
  }

  ( void ) aprv2_drv_trace_update( packet );
  ( void ) apr_log_pktv2_packet( handle, packet );

  packet_len = APRV2_PKT_GET_PACKET_BYTE_SIZE( params->packet->header);
  if ( packet_len <= APRV2_DRV_BYTES_PER_PACKET_V )
  {
    block = &aprv2_drv_free_packets_memq;
  }
  else
  if ( packet_len <= APRV2_DRV_4KB_PACKET_SIZE_V )
  {
    block = &aprv2_4k_drv_free_packets_memq;
  }
  else
  {
    APR_DBG_1(APR_DBG_ERROR, "send_error(%08X,NULL)", handle);
    return APR_EFAILED;
  }
  
  if (((char *)packet > (char *)aprv2_drv_packet_heap &&
        (char *)packet < (((char *)aprv2_drv_packet_heap) + (sizeof(aprv2_drv_packet_heap)) - (sizeof( aprv2_packet_t)))) ||
      ((char *)packet > (char *)aprv2_4k_drv_packet_heap &&
         (char *)packet < (((char *)aprv2_4k_drv_packet_heap) + (sizeof(aprv2_4k_drv_packet_heap)) - (sizeof( aprv2_packet_t)))))
  {
  /* Sets the packet owner to the destination service. */
  apr_memq_node_set_metadata( block, packet, 0, packet->dst_addr );
  }

  APR_DBG_2(APR_DBG_LOW, "send(%08X,%08X)", handle, packet);

rc = aprv2_ipc_send( packet );
if ( rc == APR_ELPC )
  {
    dst_domain_id = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );
    dst_service_id = APR_GET_FIELD( APRV2_PKT_SERVICE_ID, packet->dst_addr );

    ( void ) aprv2_rtable_get_num_total_services( dst_domain_id, &num_total_services );
    if ( dst_service_id >= num_total_services )
    {
      if (((char *)packet > (char *)aprv2_drv_packet_heap &&
        (char *)packet < (((char *)aprv2_drv_packet_heap) + (sizeof(aprv2_drv_packet_heap)) - (sizeof( aprv2_packet_t)))) ||
      ((char *)packet > (char *)aprv2_4k_drv_packet_heap &&
         (char *)packet < (((char *)aprv2_4k_drv_packet_heap) + (sizeof(aprv2_4k_drv_packet_heap)) - (sizeof( aprv2_packet_t)))))
      {
      /* If sending fails, sets the packet owner to the sending service. */
      apr_memq_node_set_metadata( block, packet , 0, handle );
      }

      return APR_EFAILED;
    }

    ( void ) aprv2_rtable_get_service( dst_domain_id, dst_service_id, &session );
    rc = session->dispatch_fn( packet, session->dispatch_data );
  }

  if ( rc != APR_EOK )
  {
    if (((char *)packet > (char *)aprv2_drv_packet_heap &&
        (char *)packet < (((char *)aprv2_drv_packet_heap) + (sizeof(aprv2_drv_packet_heap)) - (sizeof( aprv2_packet_t)))) ||
      ((char *)packet > (char *)aprv2_4k_drv_packet_heap &&
         (char *)packet < (((char *)aprv2_4k_drv_packet_heap) + (sizeof(aprv2_4k_drv_packet_heap)) - (sizeof( aprv2_packet_t)))))
    {
    /* If sending fails, sets the packet owner to the sending service. */
    apr_memq_node_set_metadata( block, packet , 0, handle );
  }
  }

  return rc;
}

/* TODO: Need to allocate packets from the service handle. */
static int32_t aprv2_drv_alloc_raw (
  uint32_t handle,
  uint32_t alloc_type,
  uint32_t alloc_size,
  aprv2_packet_t** ret_packet
)
{
  aprv2_packet_t* new_packet = NULL;
  apr_memq_block_t* block;

  if ( alloc_size > APRV2_DRV_4KB_PACKET_SIZE_V )
  {
    APR_DBG_3(APR_DBG_ERROR, "alloc_error(%08X,%08X,...,%d)", handle, alloc_type, alloc_size);
    return APR_ENORESOURCE;
  }

  /* TODO: Need to have bin sizes for different packet sizes. WB PCM is 640 bytes. */
  /* TODO: Need to track which service memory comes from to return on free. */
  if ( alloc_size <= APRV2_DRV_BYTES_PER_PACKET_V )
  {
    block = &aprv2_drv_free_packets_memq;
    new_packet = ( ( aprv2_packet_t* ) apr_memq_alloc( block ) );
  }
  else
  if ( alloc_size <= APRV2_DRV_4KB_PACKET_SIZE_V )
  {
    block = &aprv2_4k_drv_free_packets_memq;
    new_packet = ( ( aprv2_packet_t* ) apr_memq_alloc( block ) );
  }

  if ( new_packet == NULL )
  {
    APR_DBG_3(APR_DBG_ERROR, "alloc_error(%08X,%08X,...,%d,NULL)", handle, alloc_type, alloc_size);
    return APR_ENORESOURCE;
  }

  /* Track which service memory comes from to return on free. */
  apr_memq_node_set_metadata( block, new_packet, 0, handle );
  APR_DBG_2(APR_DBG_LOW, "alloc(%08X,%08X)", handle, new_packet);

  *ret_packet = new_packet;

  return APR_EOK;
}

static int32_t aprv2_drv_alloc_typical (
  uint32_t handle,
  uint32_t alloc_type,
  uint32_t alloc_size,
  aprv2_packet_t** ret_packet
)
{
  int32_t rc;
  uint32_t packet_size;
  aprv2p1_packet_t* new_packet;

  packet_size = ( APRV2P1_PKT_HEADER_BYTE_SIZE_V + alloc_size );
    /* The packet_size is the total bytes required without rounding. */

  rc = aprv2_drv_alloc_raw( handle, alloc_type, packet_size,
                            ( ( aprv2_packet_t** ) &new_packet ) );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  new_packet->header =
    APR_SET_FIELD( APRV2_PKT_VERSION, APRV2P1_PKT_VERSION_V ) |
    APR_SET_FIELD( APRV2_PKT_HEADER_SIZE, APRV2P1_PKT_HEADER_WORD_SIZE_V ) |
    APR_SET_FIELD( APRV2_PKT_PACKET_SIZE, packet_size );
  new_packet->src_addr = APRV2_PKT_INIT_ADDR_V;
  new_packet->src_port = APR_NULL_V;
  new_packet->dst_addr = APRV2_PKT_INIT_ADDR_V;
  new_packet->dst_port = APR_NULL_V;
  new_packet->token = APR_NULL_V;
  new_packet->opcode = APR_UNDEFINED_ID_V;
  ( void ) aprv2_drv_trace_initialize( handle, &new_packet->trace );

  *ret_packet = ( ( aprv2_packet_t* ) new_packet );

  return APR_EOK;
}

/* TODO: Use the service handle to limit the number of allocations allowed.
 *       The local RPC dispatcher should use the destination service ID to
 *       allocate a packet from the appropriate alloc_type store.
 */
/* TODO: Need to differentiate in-bound and out-bound packet allocations. */
APR_INTERNAL int32_t aprv2_cmd_alloc (
  aprv2_cmd_alloc_t* params
)
{
  int32_t rc;

  if ( params == NULL )
  {
    return APR_EBADPARAM;
  }

  if ( params->ret_packet == NULL )
  {
    APR_DBG_2(APR_DBG_ERROR, "alloc_error(%08X,%08X,...,NULL)", params->handle, params->alloc_type);
    return APR_EBADPARAM;
  }

  switch ( params->alloc_type )
  {
  case APRV2_ALLOC_TYPE_COMMAND_RAW:
  case APRV2_ALLOC_TYPE_RESPONSE_RAW:
    rc = aprv2_drv_alloc_raw( params->handle, params->alloc_type,
                              params->alloc_size, params->ret_packet );
    break;

  case APRV2_ALLOC_TYPE_COMMAND:
  case APRV2_ALLOC_TYPE_RESPONSE:
    rc = aprv2_drv_alloc_typical( params->handle, params->alloc_type,
                                  params->alloc_size, params->ret_packet );
    break;

  default:
    APR_DBG_2(APR_DBG_ERROR, "alloc_error(%08X,%08X)", params->handle, params->alloc_type);
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

/* TODO: Free allocations back to the owning service. */
APR_INTERNAL int32_t aprv2_cmd_free (
  aprv2_cmd_free_t* params
)
{
  uint32_t packet_len;
  int32_t stored_handle;
  apr_memq_block_t* block;

  if ( params == NULL )
  {
    return APR_EBADPARAM;
  }

  APR_DBG_2(APR_DBG_LOW, "free(%08X,%08X)", params->handle, params->packet);

  if ( params->packet == NULL )
  {
    return APR_EBADPARAM;
  }

  if (((char *)params->packet < (char *)aprv2_drv_packet_heap ||
        (char *)params->packet > (((char *)aprv2_drv_packet_heap) + (sizeof(aprv2_drv_packet_heap)) - (sizeof(aprv2_packet_t)))) &&
      ((char *)params->packet < (char *)aprv2_4k_drv_packet_heap ||
         (char *)params->packet > (((char *)aprv2_4k_drv_packet_heap) + (sizeof(aprv2_4k_drv_packet_heap)) - (sizeof(aprv2_packet_t)))))
  {
    return aprv2_ipc_free(params->packet);
  }

  packet_len = APRV2_PKT_GET_PACKET_BYTE_SIZE( params->packet->header);
  if ( packet_len <= APRV2_DRV_BYTES_PER_PACKET_V )
  {
    block = &aprv2_drv_free_packets_memq;
  }
  else
  if ( packet_len <= APRV2_DRV_4KB_PACKET_SIZE_V )
  {
    block = &aprv2_4k_drv_free_packets_memq;
  }
  else
  {
    return APR_EBADPARAM;
  }
  /* Check if the caller's handle matches the packet owner's handle. */
  apr_memq_node_get_metadata( block, params->packet, 0, &stored_handle );

  if ( ( stored_handle != params->handle ) && (params->handle != 0x0) )
  {
    APR_DBG_3(APR_DBG_ERROR,
            "On freeing, packet owner's handle 0x%08X, does not match the caller's handle 0x%08X, packet is 0x%08X",
                                   stored_handle, params->handle, params->packet );

    apr_log_error( params->handle, params->packet, __LINE__,
                    __FILENAME__, "Packet owner: 0x%08X, caller: 0x%08X",
                   stored_handle, params->handle, 0 );
  }

  /* Sets the packet owner to 0 before free the packet. */
  apr_memq_node_set_metadata( block, params->packet, 0, 0 );

  apr_memq_free( block, params->packet );

  return APR_EOK;
}

/*****************************************************************************
 * Utility Control Implementations                                           *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_cmd_query_address (
  aprv2_cmd_query_address_t* params
)
{
  if ( ( params == NULL ) || ( params->ret_addr == NULL ) )
  {
    return APR_EBADPARAM;
  }

  *params->ret_addr = ( ( uint16_t ) params->handle );
    /* Note: Currently assumes the lower 16 bits is the registered service's
     *       address.
     */

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_cmd_local_dns_lookup (
  aprv2_cmd_local_dns_lookup_t* params
)
{
  int32_t rc;
  uint16_t domain_index;
  uint16_t service_index;
  uint16_t num_total_domains;
  uint16_t num_total_services;
  aprv2_rtable_service_entry_t* session;

  if ( ( params == NULL ) ||
       ( params->ret_addr == NULL ) ||
       ( params->name_size > APRV2_RTABLE_MAX_DNS_SIZE ) )
  {
    return APR_EBADPARAM;
  }

  rc = aprv2_rtable_local_dns_lookup( params->name, params->name_size, params->ret_addr );
  if ( rc )
  {
    ( void ) aprv2_rtable_get_num_total_domains( &num_total_domains );

    for ( domain_index = 0 ; domain_index < num_total_domains; ++domain_index )
    {
      /* Find any registered service with the same name. */
      ( void ) aprv2_rtable_get_num_total_services( domain_index, &num_total_services );

      for ( service_index = 0; service_index < num_total_services; ++service_index )
      {
        ( void ) aprv2_rtable_get_service( domain_index, service_index, &session );
        if ( session->dispatch_fn == aprv2_drv_default_dispatch_fn )
        { /* This service entry is inactive. */
          continue;
        }

        if ( mmstd_strncmp( session->name, sizeof( session->name ),
                            params->name, params->name_size ) == 0 )
        {
          *params->ret_addr =
            ( APR_SET_FIELD( APRV2_PKT_DOMAIN_ID, domain_index ) |
              APR_SET_FIELD( APRV2_PKT_SERVICE_ID, service_index ) );
          goto label_success;
        }
      }
    }

    return APR_ENOTEXIST;
  }
label_success:

  return APR_EOK;
}

/*****************************************************************************
 * Utility Control Implementations                                           *
 ****************************************************************************/

APR_INTERNAL int32_t aprv2_cmd_forward (
  aprv2_cmd_forward_t* params
)
{
  int32_t rc;
  aprv2_cmd_async_send_t send_args;
  aprv2_cmd_free_t free_args;

  if ( ( params == NULL ) || ( params->packet == NULL ) )
  {
    return APR_EBADPARAM;
  }

  send_args.handle = params->handle;
  send_args.packet = params->packet;
  rc = aprv2_cmd_async_send( &send_args );
  if ( rc )
  {
    free_args.handle = params->handle;
    free_args.packet = params->packet;
    ( void ) aprv2_cmd_free( &free_args );
    if ( aprv2_drv_debug_enable_error_panic == TRUE )
    {
      ERR_FATAL( "Unable to forward command", 0, 0, 0 );
    }
  }

  return rc;
}

APR_INTERNAL int32_t aprv2_cmd_alloc_ext (
  aprv2_cmd_alloc_ext_t* params
)
{
  int32_t rc;
  aprv2_cmd_alloc_t args;
  aprv2_packet_t* new_packet;

  if ( ( params == NULL ) || ( params->ret_packet == NULL ) )
  {
    return APR_EBADPARAM;
  }

  args.handle = params->handle;
  args.alloc_type = ( ( params->msg_type == APRV2_PKT_MSGTYPE_CMDRSP_V ) ?
                      APRV2_ALLOC_TYPE_RESPONSE :
                      APRV2_ALLOC_TYPE_COMMAND );
  args.alloc_size = params->payload_size;
  args.ret_packet = &new_packet;

  rc = aprv2_cmd_alloc( &args );
  if ( rc )
  {
    return APR_ENORESOURCE;
  }

  new_packet->header &= ~( APRV2_PKT_MSGTYPE_MASK );
  new_packet->header |= APR_SET_FIELD( APRV2_PKT_MSGTYPE, params->msg_type );
  new_packet->src_addr = params->src_addr;
  new_packet->src_port = params->src_port;
  new_packet->dst_addr = params->dst_addr;
  new_packet->dst_port = params->dst_port;
  new_packet->token = params->token;
  new_packet->opcode = params->opcode;

  *params->ret_packet = new_packet;

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_cmd_alloc_send (
  aprv2_cmd_alloc_send_t* params
)
{
  int32_t rc;
  aprv2_cmd_alloc_ext_t alloc_args;
  aprv2_packet_t* packet;
  aprv2_cmd_forward_t forward_args;

  if ( ( params == NULL ) ||
       ( ( params->payload_size > 0 ) && ( params->payload == NULL ) ) )
  {
    return APR_EBADPARAM;
  }

  alloc_args.handle = params->handle;
  alloc_args.msg_type = params->msg_type;
  alloc_args.src_addr = params->src_addr;
  alloc_args.src_port = params->src_port;
  alloc_args.dst_addr = params->dst_addr;
  alloc_args.dst_port = params->dst_port;
  alloc_args.token = params->token;
  alloc_args.opcode = params->opcode;
  alloc_args.payload_size = params->payload_size;
  alloc_args.ret_packet = &packet;

  rc = aprv2_cmd_alloc_ext( &alloc_args );
  if ( rc )
  {
    return rc;
  }

  if ( params->payload_size > 0 )
  {
    /* TODO: Make sure the payload copy is cache/uncache safe. */
    ( void ) mmstd_memcpy( APRV2_PKT_GET_PAYLOAD( void, packet ),
                           APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header ),
                           params->payload, params->payload_size );
  }

  forward_args.handle = params->handle;
  forward_args.packet = packet;
  rc = aprv2_cmd_forward( &forward_args );

  return rc;
}

APR_INTERNAL int32_t aprv2_cmd_accept_command (
  aprv2_cmd_accept_command_t* params
)
{
  aprv2_packet_t* packet;
  aprv2_cmd_alloc_send_t send_args;
  aprv2_ibasic_evt_accepted_t rsp;

  if ( ( params == NULL ) || ( params->packet == NULL ) )
  {
    return APR_EBADPARAM;
  }

  packet = params->packet;

  if ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
         APRV2_PKT_MSGTYPE_SEQCMD_V ) ||
       ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
         APRV2_PKT_MSGTYPE_NSEQCMD_V ) )
  {
    /* Reverse the source and destination addresses to send a command
     * accepted event.
     */
    send_args.handle = params->handle;
    send_args.msg_type = APRV2_PKT_MSGTYPE_EVENT_V;
    send_args.src_addr = packet->dst_addr;
    send_args.src_port = packet->dst_port;
    send_args.dst_addr = packet->src_addr;
    send_args.dst_port = packet->src_port;
    send_args.token = packet->token;
    send_args.opcode = APRV2_IBASIC_EVT_ACCEPTED;
    rsp.opcode = params->packet->opcode;
    send_args.payload = &rsp;
    send_args.payload_size = sizeof( rsp );

    ( void ) aprv2_cmd_alloc_send( &send_args );
  }

  return APR_EOK;
}

/* NOTE: The command packet is freed irrespective of failing to send the
 *       command response.
 */
APR_INTERNAL int32_t aprv2_cmd_end_command (
  aprv2_cmd_end_command_t* params
)
{
  int32_t rc;
  aprv2_packet_t* packet;
  aprv2_cmd_alloc_send_t send_args;
  aprv2_cmd_free_t free_args;
  aprv2_ibasic_rsp_result_t rsp;

  if ( ( params == NULL ) || ( params->packet == NULL ) )
  {
    return APR_EBADPARAM;
  }

  packet = params->packet;

  if ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
         APRV2_PKT_MSGTYPE_SEQCMD_V ) ||
       ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
         APRV2_PKT_MSGTYPE_NSEQCMD_V ) )
  {
    /* Reverse the source and destination addresses to send a command response. */
    send_args.handle = params->handle;
    send_args.msg_type = APRV2_PKT_MSGTYPE_CMDRSP_V;
    send_args.src_addr = packet->dst_addr;
    send_args.src_port = packet->dst_port;
    send_args.dst_addr = packet->src_addr;
    send_args.dst_port = packet->src_port;
    send_args.token = packet->token;
    send_args.opcode = APRV2_IBASIC_RSP_RESULT;
    rsp.opcode = params->packet->opcode;
    rsp.status = params->status;
    send_args.payload = &rsp;
    send_args.payload_size = sizeof( rsp );

    rc = aprv2_cmd_alloc_send( &send_args );
    if ( rc )
    {
      if ( aprv2_drv_debug_enable_error_panic == TRUE )
      {
        ERR_FATAL( "Unable to end command", 0, 0, 0 );
      }
    }
  }
  else
  {
    /* Silently drop command responses and events. */
    rc = APR_EOK;
  }

  free_args.handle = params->handle;
  free_args.packet = packet;
  ( void ) aprv2_cmd_free( &free_args );

  return rc;
}

