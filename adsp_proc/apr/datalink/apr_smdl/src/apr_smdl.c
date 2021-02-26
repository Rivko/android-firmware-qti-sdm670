/*
  Copyright (C) 2010-2014,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/datalink/apr_smdl/src/apr_smdl.c#1 $
  $Author: pwbldsvc $
*/

#include "mmstd.h"
#include <stdlib.h>
#include "smd_lite.h"
#include "apr_smdl.h"
#include "apr_list.h"
#include "apr_lock.h"
#include "apr_errcodes.h"
#include "msg.h"
#include "apr_dbg_msg.h"

/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define APR_SMDL_MAX_PORTS ( 64 )
  /**< Max number of APR SMDL ports. */

#define APR_SMDL_PORT_HANDLE_BASE ( 1 )
  /**< Used to make sure port handle are positive( starting from 1 ). */

/****************************************************************************
 * Definitions                                                              *
 ****************************************************************************/

typedef struct apr_smdl_port_t apr_smdl_port_t;
struct apr_smdl_port_t
{
  char_t port_name[ APR_SMDL_CHANNEL_NAME_SIZE_MAX ];
  smdl_handle_type smdl_handle;
  apr_smdl_rx_cb_t rx_cb;
  void* rx_cb_data;
  int8_t* read_buf;
  uint32_t read_buf_size;
};

typedef struct apr_smdl_port_list_item_t apr_smdl_port_list_item_t;
struct apr_smdl_port_list_item_t
{
  apr_list_node_t link;
  apr_smdl_port_handle_t port_handle;
  apr_smdl_port_t* port;
};

/****************************************************************************
 * Variables                                                                *
 ****************************************************************************/

static apr_list_t apr_smdl_free_port_list;
static apr_smdl_port_list_item_t apr_smdl_port_pool[ APR_SMDL_MAX_PORTS ];
static apr_lock_t apr_smdl_free_port_list_lock;

/****************************************************************************
 * Core Routine Implementations                                             *
 ****************************************************************************/

static void apr_smdl_free_port_list_lock_fn ( void )
{
  ( void ) apr_lock_enter( apr_smdl_free_port_list_lock );
}

static void apr_smdl_free_port_list_unlock_fn ( void )
{
  ( void ) apr_lock_leave( apr_smdl_free_port_list_lock );
}

static void apr_smdl_rx_cb_handler (
  void* cb_data
)
{
  int32_t rc;
  apr_smdl_port_t* port;
  int32_t packet_len;

  for ( ;; )
  {
    port = ( ( apr_smdl_port_t* ) cb_data );
    if ( port == NULL )
    {
      APR_DBG_0(APR_DBG_ERROR, "apr_smdl_rx_cb_handler: invalid cb data received from smdl.");
      break;
    }

    /* In read callback, more than one SMD Lite packets may be available. They must be 
     * read by calling smdl_read in a loop to ensure no data will be left in the FIFO. 
     */
    for ( ;; )
    {
      packet_len = smdl_rx_peek( port->smdl_handle );
      if ( packet_len == 0 ) 
      {/* No more data to read. */
        break;
      }
      else if ( packet_len < 0 ) 
      {/* An error has occured in the SMDL driver. */
        APR_DBG_0(APR_DBG_ERROR, "apr_smdl_rx_cb_handler: smdl_rx_peek error occured.");
        break;
      }

      rc = smdl_read( port->smdl_handle, port->read_buf_size, port->read_buf,
                      SMDL_READ_FLAGS_NONE );
      if ( rc < 0 )
      {/* smdl_read error. */
        APR_DBG_0(APR_DBG_ERROR, "apr_smdl_rx_cb_handler: smdl_read error occured.");
        break;
      }

      rc = port->rx_cb( port->rx_cb_data, port->read_buf, packet_len );
      if ( rc )
      {
        APR_DBG_0(APR_DBG_ERROR, "apr_smdl_rx_cb_handler: client callback error occured.");
        break;
      }
    }

    break;
  }

  return;
}

static void apr_smdl_cb (
  smdl_handle_type port,
  smdl_event_type event,
  void* cb_data
)
{
  switch ( event )
  {
  case SMDL_EVENT_OPEN:
    /* Current implementation ignores this event. */
    break;

  case SMDL_EVENT_READ:
    apr_smdl_rx_cb_handler( cb_data );
    break;

  case SMDL_EVENT_WRITE:
    /* Current implementation ignores this event. */
    break;
   
  case SMDL_EVENT_CLOSE:
    /* Current implementation ignores this event. */
    break;

  default:
    break;
  }

  return;
}

APR_INTERNAL int32_t apr_smdl_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;
  uint32_t port_count;

  for ( ;; )
  {
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &apr_smdl_free_port_list_lock );
    if ( rc ) break;
    checkpoint = 1;

    rc = apr_list_init( &apr_smdl_free_port_list, apr_smdl_free_port_list_lock_fn, 
                        apr_smdl_free_port_list_unlock_fn );
    if ( rc ) break;

    for ( port_count = 0; port_count < APR_SMDL_MAX_PORTS; port_count++ )
    {
      apr_smdl_port_pool[ port_count ].port_handle = port_count;
      ( void ) apr_list_add_tail( &( apr_smdl_free_port_list ),
                                  ( ( apr_list_node_t* ) &( apr_smdl_port_pool[ port_count ] ) ) );
    }

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 1:
    ( void ) apr_lock_destroy( apr_smdl_free_port_list_lock );
    /*-fallthru */
  default:
      break;
  }

  APR_DBG_1(APR_DBG_ERROR, "apr_smdl_init: error after checkpoint %d." , checkpoint);
  return rc;
}

APR_INTERNAL int32_t apr_smdl_deinit ( void )
{
  ( void ) apr_list_destroy( &apr_smdl_free_port_list );
  ( void ) apr_lock_destroy( apr_smdl_free_port_list_lock );

  return APR_EOK;
}

APR_INTERNAL int32_t apr_smdl_open (
  char_t* port_name,
  uint32_t port_name_size,
  uint32_t channel_id,
  apr_smdl_rx_cb_t rx_cb,
  void* rx_cb_data,
  uint32_t fsize,
  apr_smdl_port_handle_t* port_handle
)
{
  int32_t rc;
  uint32_t checkpoint = 0;
  smd_channel_type smdl_channel;
  int8_t* read_buf;
  apr_smdl_port_list_item_t* port_item;
  apr_smdl_port_t* port;

  if ( ( port_name == NULL ) || ( port_name_size > APR_SMDL_CHANNEL_NAME_SIZE_MAX ) )
  {
    return APR_EBADPARAM;
  }

  switch ( channel_id )
  {
  case APR_SMDL_APPS_MODEM_CHANNEL_ID:
    smdl_channel = SMD_APPS_MODEM;
    break;

  case APR_SMDL_APPS_QDSP_CHANNEL_ID:
    smdl_channel = SMD_APPS_QDSP;
    break;

  case APR_SMDL_MODEM_QDSP_CHANNEL_ID:
    smdl_channel = SMD_MODEM_QDSP;
    break;

  default:
    return APR_EBADPARAM;
  }

  for ( ;; )
  {
    rc = apr_list_remove_head( &(apr_smdl_free_port_list ),
                               ( ( apr_list_node_t** ) &port_item ) );
    if ( rc ) break;
    checkpoint = 1;

    port = malloc( sizeof( apr_smdl_port_t ) );
    if ( port == NULL )
    {
      rc = APR_ENORESOURCE;
      break;
    }
    checkpoint = 2;

    mmstd_memset( port, 0, sizeof( apr_smdl_port_t ) ) ;
    mmstd_strlcpy( port->port_name, port_name, MMSTD_MIN( sizeof( port->port_name ), port_name_size ) );
    port->rx_cb = rx_cb;
    port->rx_cb_data = rx_cb_data;

    if ( fsize == 0 )
    {
      fsize = APR_SMDL_STANDARD_FIFO;
    }

    /* Ensure that FIFO size is a multiple of 32 and within the supported range. */ 
    if ( ( fsize & 31 ) ||
         ( fsize > APR_SMDL_MAX_FIFO ) ||
         ( fsize < APR_SMDL_MIN_FIFO ) )
    {
      rc = APR_EBADPARAM;
      break;
    }

    read_buf = malloc( fsize );
    if ( read_buf == NULL )
    {
      rc = APR_ENORESOURCE;
      break;
    }
    checkpoint = 3;

    mmstd_memset( read_buf, 0, fsize );
    port->read_buf = read_buf;
    port->read_buf_size = fsize;

    port->smdl_handle = smdl_open( port_name, smdl_channel,
                                   SMDL_OPEN_FLAGS_PRIORITY_IST, fsize,
                                   apr_smdl_cb, ( ( void* ) port ) );
    if ( port->smdl_handle == NULL )
    {
      rc = APR_EFAILED;
      break;
    }

    port_item->port = port;

    *port_handle = port_item->port_handle + APR_SMDL_PORT_HANDLE_BASE;

    APR_DBG_1(APR_DBG_HIGH, "apr_smdl_open() with port handle %d DONE.", *port_handle);

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 3:
    free( read_buf );
    /*-fallthru */
  case 2:
    free( port );
    /*-fallthru */
  case 1:
    ( void ) apr_list_add_tail( &( apr_smdl_free_port_list ),
                                ( ( apr_list_node_t* ) &port_item->link ) );
    /*-fallthru */
  default:
    break;
  }

  *port_handle = 0;

  APR_DBG_1(APR_DBG_ERROR, " apr_smdl_open: failed with rc = %d.", rc);
  return rc;
}

APR_INTERNAL int32_t apr_smdl_close (
  apr_smdl_port_handle_t port_handle
)
{
  int32_t rc;
  apr_smdl_port_list_item_t* port_item;
  apr_smdl_port_t* port;

  port_handle -= APR_SMDL_PORT_HANDLE_BASE;
  if ( port_handle >= APR_SMDL_MAX_PORTS )
  {
    return APR_EBADPARAM;
  }

  port = apr_smdl_port_pool[ port_handle ].port;
  if ( port == NULL )
  {
    return APR_EBADPARAM;
  }

  for ( ;; )
  {
    rc = smdl_close( port->smdl_handle );
    if ( rc != APR_EOK )
    {
      rc = APR_EFAILED;
      break;
    }

    free( port->read_buf );
    free( port );

    port_item = &apr_smdl_port_pool[ port_handle ];
    port_item->port = NULL;

    ( void ) apr_list_add_tail( &( apr_smdl_free_port_list ),
                                ( ( apr_list_node_t* ) &port_item->link ) );

    return APR_EOK;
  }

  APR_DBG_1(APR_DBG_ERROR, "apr_smdl_close: failed with rc = %d.", rc);
  return rc;
}

APR_INTERNAL int32_t apr_smdl_write (
  apr_smdl_port_handle_t port_handle,
  void* buf,
  uint32_t size
)
{
  int32_t rc;
  apr_smdl_port_t* port;

  port_handle -= APR_SMDL_PORT_HANDLE_BASE;
  if ( port_handle >= APR_SMDL_MAX_PORTS )
  {
    return APR_EBADPARAM;
  }

  port = apr_smdl_port_pool[ port_handle ].port;
  if ( port == NULL )
  {
    return APR_EBADPARAM;
  }

  for ( ;; )
  {
    rc = smdl_write( port->smdl_handle, size, buf, SMDL_WRITE_FLAGS_NONE );
    if ( rc < 0 ) 
    {/* SMD LITE write is failed. */
      rc = APR_EFAILED;
      break;
    }
    else if ( rc == 0 )
    {/* Not enough space in the FIFO. */
      rc = APR_ENORESOURCE;
      break;
    }

    return APR_EOK;
  }

  APR_DBG_1(APR_DBG_ERROR, "apr_smdl_write: failed with rc = %d.", rc);
  return rc;
}

