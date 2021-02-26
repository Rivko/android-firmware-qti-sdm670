/*
   Copyright (C) 2010-2011, 2012, 2014,2016 QUALCOMM Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - QUALCOMM Technologies, Inc.

   $Header: //components/rel/apr.common/2.0/datalink/diag/src/aprv2_diag.c#1 $
   $Author: pwbldsvc $
*/

#include <string.h>
#include "mmstd.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "msg.h"
#include "apr_errcodes.h"
#include "aprv2_api_inline.h"
#include "apr_lock.h"
#include "apr_memq.h"
#include "apr_list.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

#define APRV2_DIAG_METADATA_ITEMS ( 0 )
#define APRV2_DIAG_FIXED_PACKET_BYTE_SIZE_V ( 512 )
#define APRV2_DIAG_PACKET_MEMQ_UNIT_SIZE_V ( APR_MEMQ_UNIT_OVERHEAD_V + sizeof( aprv2_diag_tx_qitem_t ) )
#define APRV2_DIAG_PACKET_POOL_ITEMS_V ( 20 ) /* ~10KB Pool */
#define APRV2_DIAG_PACKET_POOL_WORD_SIZE_V \
  ( ( ( APRV2_DIAG_PACKET_POOL_ITEMS_V * APRV2_DIAG_PACKET_MEMQ_UNIT_SIZE_V ) + 3 ) >> 2 )

#define APRV2_DIAG_SEQUENCE_START_V ( ( uint16_t ) 0xFFFF )

#ifndef DIAG_SUBSYS_APR
  #define DIAG_SUBSYS_APR ( 78 )
#endif /* DIAG_SUBSYS_APR */

#ifndef MSG_SSID_APR_MODEM
  #define MSG_SSID_APR_MODEM ( MSG_SSID_AVS )
  #define MSG_BUILD_MASK_MSG_SSID_APR_MODEM ( MSG_BUILD_MASK_MSG_SSID_AVS )
#endif /* MSG_SSID_APR_MODEM */

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

/* Rx APR DIAG request message data structure */
typedef struct aprv2_diag_req_pkt_t aprv2_diag_req_pkt_t;

#include "apr_pack_begin.h"
struct aprv2_diag_req_pkt_t
{
  diagpkt_subsys_header_type xx_header;
  uint16_t size;
  uint16_t sequence_num; /* A 16-bit sequence number */
}
#include "apr_pack_end.h"
;

/* Tx APR DIAG response message data structure */
typedef struct aprv2_diag_rsp_pkt_t aprv2_diag_rsp_pkt_t;

#include "apr_pack_begin.h"
struct aprv2_diag_rsp_pkt_t
{
  diagpkt_subsys_header_type xx_header;
  uint16_t size;
  uint16_t padding0; /* Align payload that follows to 32-bit */
}
#include "apr_pack_end.h"
;

/* Tx APR DIAG packet management queue structure */
typedef struct aprv2_diag_tx_qitem_t aprv2_diag_tx_qitem_t;
struct aprv2_diag_tx_qitem_t
{
  apr_list_node_t link; /* Linked list header */
  uint8_t buf[ APRV2_DIAG_FIXED_PACKET_BYTE_SIZE_V ];
};

/* APR DIAG internal housekeeping variables structure */
typedef struct aprv2_diag_vars_t aprv2_diag_vars_t;
struct aprv2_diag_vars_t
{
  /* Used for packet error verification */
  uint16_t prev_seq_num;

  /* DIAG Tx packet management */
  uint32_t free_tx_pool[ APRV2_DIAG_PACKET_POOL_WORD_SIZE_V ];
  apr_memq_block_t free_tx_memq;
  apr_list_t ready_tx_q;

  /* Mutex */
  apr_lock_t lock;
};

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static aprv2_diag_vars_t aprv2_diag;

/*****************************************************************************
 * Routine Implementations                                                   *
 ****************************************************************************/

static void aprv2_diag_lock_fn ( void )
{
  ( void ) apr_lock_enter( aprv2_diag.lock );
}

static void aprv2_diag_unlock_fn ( void )
{
  ( void ) apr_lock_leave( aprv2_diag.lock );
}

PACKED void* aprv2_diag_cmd (
  PACKED void* req_pkt,
  uint16 req_size
)
{
  int32_t rc;
  const int do_once = 0;
  aprv2_diag_tx_qitem_t* tx_qitem;
  aprv2_diag_req_pkt_t* rx = ( ( aprv2_diag_req_pkt_t* ) req_pkt );
  aprv2_diag_rsp_pkt_t* tx;
  aprv2_packet_t* packet;
  uint16_t size;

  /* Process in-bound traffic */
  do
  {
    if ( rx->sequence_num == aprv2_diag.prev_seq_num )
    {
      MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Skipping stale Rx packet sequence number" );
      break;
    }
    else
    {
      aprv2_diag.prev_seq_num = rx->sequence_num;
    }

    if ( rx->size > 0 )
    {
      packet = ( ( aprv2_packet_t* ) APR_PTR_END_OF( rx, sizeof( aprv2_diag_req_pkt_t ) ) );

      if ( APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header ) != rx->size )
      {
        MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Mismatched Rx packet size" );
        break;
      }

      rc = __aprv2_cmd_alloc_send(
             packet->dst_addr, /* SUPER_HACK: Allocate memory from destination service's packet pool */
             APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ),
             packet->src_addr, packet->src_port,
             packet->dst_addr, packet->dst_port,
             packet->token,
             packet->opcode,
             APRV2_PKT_GET_PAYLOAD( void, packet ),
             APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( packet->header )
           );
      if ( rc )
      {
        MSG_1( MSG_SSID_APR_MODEM, MSG_LEGACY_FATAL, "Failed to send Rx packet (0x%08X)", rc );
        break;
      }
    }
  }
  while ( do_once );

  /* Process out-bound traffic */
  do
  {
    rc = apr_list_remove_head( &aprv2_diag.ready_tx_q,
                               ( ( apr_list_node_t** ) &tx_qitem ) );
    if ( !rc )
    { /* There is something to send */
      size_t dst_size = 0;

      packet = ( ( aprv2_packet_t* ) tx_qitem->buf );
      size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
      dst_size = sizeof( aprv2_diag_rsp_pkt_t ) + size;
      tx = ( ( aprv2_diag_rsp_pkt_t* ) diagpkt_subsys_alloc( DIAG_SUBSYS_APR, 0,
             ( dst_size ) ) );
             
      if ( tx )
      { /* diagpkt allocation succeeded to send to PC */
        tx->size = size;
        mmstd_memcpy( ( ( ( uint8_t* ) tx ) + sizeof( aprv2_diag_rsp_pkt_t ) ), 
                 ( dst_size - sizeof( aprv2_diag_rsp_pkt_t ) ), packet, size);
        apr_memq_free( &aprv2_diag.free_tx_memq, ( ( void* ) tx_qitem ) );
        MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_LOW, "Dequeue and send Tx packet" );
      }
      else
      { /* Re-queue the tx item until there is sufficient diagpkt memory */
        rc = apr_list_add_head( &aprv2_diag.ready_tx_q, &tx_qitem->link );
        MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Not enough memory to send queued Tx packet" );
      }
    }
    else
    {
      /* Even if there is nothing to send we still must ACK back DIAG
       * so that the PC-DIAG doesn't retry three times.
       */
      tx = ( ( aprv2_diag_rsp_pkt_t* ) diagpkt_subsys_alloc( DIAG_SUBSYS_APR,
             0, sizeof( aprv2_diag_rsp_pkt_t ) ) );
      if ( tx )
      {
        tx->size = 0;
      }
    }
  }
  while ( do_once );

  return tx;
}

PACKED void* aprv2_diag_sequence_reset (
  PACKED void* req_pkt,
  uint16 req_size
)
{
  aprv2_diag_rsp_pkt_t* tx;

  MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_HIGH, "Recieved a ReSync message, resetting sequence number" );
  aprv2_diag.prev_seq_num = APRV2_DIAG_SEQUENCE_START_V;

  /* Respond to diag with empty pkt */
  tx = ( ( aprv2_diag_rsp_pkt_t* ) diagpkt_subsys_alloc( DIAG_SUBSYS_APR, 1,
         sizeof( aprv2_diag_rsp_pkt_t ) ) );
  if ( tx )
  {
    tx->size = 0;
  }

  return tx;
}

/* Assume aprv2_diag_send() is called in ISR dispatch context */
APR_INTERNAL int32_t aprv2_diag_send (
  aprv2_packet_t* packet
)
{
  uint32_t size;
  aprv2_diag_tx_qitem_t* tx_qitem;

  if ( packet == NULL )
  {
    MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Tx packet is NULL" );
    return APR_EBADPARAM;
  }

  size = APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );
  if ( size > APRV2_DIAG_FIXED_PACKET_BYTE_SIZE_V )
  {
    MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Tx packet is too large" );
    return APR_ENORESOURCE;
  }

  tx_qitem = ( ( aprv2_diag_tx_qitem_t* )
               apr_memq_alloc( &aprv2_diag.free_tx_memq ) );
  if ( tx_qitem != NULL )
  {
    mmstd_memcpy(tx_qitem->buf, sizeof(tx_qitem->buf), packet, size);
    ( void ) apr_list_add_tail( &aprv2_diag.ready_tx_q, &tx_qitem->link );
    ( void ) __aprv2_cmd_free( 0xBADC0DE, packet );
    MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_LOW, "Enqueue Tx packet" );
    return APR_EOK;
  }
  else
  {
    MSG( MSG_SSID_APR_MODEM, MSG_LEGACY_ERROR, "Failed to queue Tx packet" );
    return APR_EFAILED;
  }
}

static const diagpkt_user_table_entry_type aprv2_diag_dispatch_table[] =
  {
    { 0, 0, aprv2_diag_cmd },
    { 1, 1, aprv2_diag_sequence_reset }
  };

APR_INTERNAL int32_t aprv2_diag_init ( void )
{
  int32_t rc;
  uint32_t check_point = 0;

  for ( ;; )
  {
    aprv2_diag.prev_seq_num = APRV2_DIAG_SEQUENCE_START_V;

    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &aprv2_diag.lock );
    if ( rc ) break;
    check_point = 1;

    rc = apr_memq_init( &aprv2_diag.free_tx_memq,
                        ( ( char_t* ) aprv2_diag.free_tx_pool ),
                        sizeof( aprv2_diag.free_tx_pool ),
                        APRV2_DIAG_PACKET_MEMQ_UNIT_SIZE_V,
						APRV2_DIAG_METADATA_ITEMS,
                        aprv2_diag_lock_fn, aprv2_diag_unlock_fn );
    if ( rc ) break;
    check_point = 2;

    rc = apr_list_init( &aprv2_diag.ready_tx_q,
                        aprv2_diag_lock_fn, aprv2_diag_unlock_fn );
    if ( rc ) break;
    check_point = 3;

    DIAGPKT_DISPATCH_TABLE_REGISTER( DIAG_SUBSYS_APR, aprv2_diag_dispatch_table );

    return APR_EOK;
  }

  switch ( check_point )
  {
  case 3:
    ( void ) apr_list_destroy( &aprv2_diag.ready_tx_q );
    /* -fallthru */
  case 2:
    apr_memq_deinit( &aprv2_diag.free_tx_memq );
    /* -fallthru */
  case 1:
    rc = apr_lock_destroy( aprv2_diag.lock );
    /* -fallthru */
  default:
    break;
  }

  return APR_EFAILED;
}

APR_INTERNAL int32_t aprv2_diag_deinit ( void )
{
  /* WARNING: DIAG doesn't support safe de-registerations so there isn't a
   *          de-register routine to call.
   */

  /* Assume the system is idle when de-initializing; this function is really
   * not yet concurrent safe.
   */
  ( void ) apr_list_destroy( &aprv2_diag.ready_tx_q );
  apr_memq_deinit( &aprv2_diag.free_tx_memq );
  ( void ) apr_lock_destroy( aprv2_diag.lock );

  return APR_EOK;
}

