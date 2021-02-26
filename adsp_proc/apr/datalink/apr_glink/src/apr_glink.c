/*=============================================================================
  Copyright (C) 2015,2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/src/apr_glink.c
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ------------------------------------------------------------
08/08/2015   rs    Initial version.
=============================================================================*/

/******************************************************************************
 * Includes                                                                   *
 *****************************************************************************/
#include "mmstd.h"
#include "apr_dbg_msg.h"
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "apr_list.h"
#include "apr_glink.h"
#include "glink.h"
#include "aprv2_api_inline.h"
#include "aprv2_ipc_config.h"

#define APR_GLINK_QOS_SUPPORT
#define APR_GLINK_INTENTS_SUPPORT

#ifdef APR_GLINK_QOS_SUPPORT
  #include "glink_qos.h"
#define APRV2_IPC_QOS_LATENCY_US          ( 550 )
#define APRV2_IPC_QOS_PKT_SIZE            ( /*160*/ 544 )
#endif
/******************************************************************************
 * Defines                                                                    *
 *****************************************************************************/
#define APR_GLINK_MAX_PORTS ( GLINK_PORTS )
  /**< Max number of APR GLINK ports. */

#define APR_GLINK_PORT_HANDLE_BASE  ( 1 )
  /**< Used to make sure port handle are positive ( starting from 1 ). */

#define APR_GLINK_128_BYTES_BUFFER ( 128 )
#define APR_GLINK_512_BYTES_BUFFER ( 512 )
#define APR_GLINK_1024_BYTES_BUFFER ( 1024 )
#define APR_GLINK_4096_BYTES_BUFFER ( 4096 )

#define APR_GLINK_NUM_OF_128_BYTES_INTENTS ( 64 )
#define APR_GLINK_NUM_OF_512_BYTES_INTENTS ( 32 )
#define APR_GLINK_NUM_OF_1024_BYTES_INTENTS ( 16 )
#define APR_GLINK_NUM_OF_4096_BYTES_INTENTS ( 04 )
/******************************************************************************
 * Type Definitions                                                           *
 *****************************************************************************/
typedef uint32_t apr_glink_state_t;
typedef struct apr_glink_port_s {
  apr_glink_port_handle_t port_index;
  apr_glink_state_t state;
  glink_handle_type glink_handle;
  char_t port_name[ APR_GLINK_MAX_CHANNEL_NAME_SIZE ];
} apr_glink_port_t;

/******************************************************************************
 * Variables                                                                  *
 *****************************************************************************/
apr_glink_port_t apr_glink_port_pool[ APR_GLINK_MAX_PORTS ];

/******************************************************************************
 * Core Routine Implementations                                               *
 *****************************************************************************/
#ifndef SIM_DEFINED
int32_t aprv2_ipc_rx_cb (apr_glink_port_handle_t port_index, void* rx_read_buf, uint32_t size)
{
  int32_t handle;
  uint32_t packet_size;
  aprv2_packet_t *packet;
  
  if (NULL == rx_read_buf || size < 1)
  {
    APR_DBG_0(APR_DBG_ERROR,
           "QDSP aprv2_ipc_rx_cb(): Rcvd NULL buffer or buffer size < 1");
    return APR_EBADPARAM;
  }

  packet = (aprv2_packet_t *)rx_read_buf;
  packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);
  if (size != packet_size)
  {
     APR_DBG_0(APR_DBG_ERROR,"QDSP aprv2_ipc_rx_cb(): Rcvd invalid buffer size");
     return APR_EFAILED;
  }

  handle = ((uint32_t)
             (((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr)) << 8)
                   | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, packet->dst_addr))));

  (void) __aprv2_cmd_forward(handle, packet);

  return APR_EOK;
}

int32_t aprv2_ipc_tx_done_cb (apr_glink_port_handle_t port_index, void* rx_write_buf, uint32_t size)
{
  int32_t handle;
  uint32_t packet_size;
  aprv2_packet_t *packet;

  if (NULL == rx_write_buf || size < 1)
  {
    APR_DBG_0(APR_DBG_ERROR,
          "QDSP aprv2_ipc_tx_done_cb(): Rcvd NULL buffer or buffer size < 1");
    return APR_EBADPARAM;
  }

  packet = (aprv2_packet_t *)rx_write_buf;
  packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);

  if (size != packet_size)
  {
    APR_DBG_0(APR_DBG_ERROR,
                 "QDSP aprv2_ipc_tx_done_cb(): Rcvd invalid buffer size");
    return APR_EFAILED;
  }

  handle = ((uint32_t)
             (((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr)) << 8)
                   | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, packet->dst_addr))));

  (void) __aprv2_cmd_free(handle, packet);

  return APR_EOK;
}
#endif /* SIM_DEFINED */

/** Data receive notification callback */  
static void apr_glink_rx_cb (
  glink_handle_type handle,     /* handle for the glink channel */
  const void        *priv,      /* priv client data passed in glink_open */
  const void        *pkt_priv,  /* private client data assiciated with the
                                   rx intent that client queued earlier */
  const void        *ptr,       /* pointer to the received buffer */
  size_t            size,       /* size of the packet */
  size_t            intent_used /* size of the intent used for this packet */
)
{
  int32_t rc;
  apr_glink_port_t *port = (apr_glink_port_t *)priv;
  glink_err_type glink_rc;

  if ((NULL == ptr)||(NULL == port))
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_rx_cb(): Rcvd NULL buffer ptr");
    return;
  }

  rc = aprv2_ipc_rx_cb(port->port_index, (void *)ptr, size);
  if (rc)
  {
     APR_DBG_1(APR_DBG_ERROR,"apr_glink_rx_cb(): Client cb returned error %d", rc);
     /* Failed to deliver the packet. Release the packet */
     if (GLINK_STATUS_SUCCESS != (glink_rc = glink_rx_done(port->glink_handle, ptr, TRUE)))
     {
        APR_DBG_1(APR_DBG_ERROR, "glink_rx_done() failed with rc = %d", glink_rc);
     }
  }

  return;
}

static void apr_glink_tx_abort (glink_handle_type glink_handle, const void *priv, const void *pkt_priv)
{
    apr_glink_port_t *port = (apr_glink_port_t *)priv;
    aprv2_packet_t *packet;
    int32_t handle;
    uint32_t packet_size;
    
    if ((NULL == pkt_priv)||(NULL == port))
    {
       APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_abort(): Rcvd NULL buffer ptr or null port");
       return;
    }
    
    APR_DBG_1(APR_DBG_HIGH, "Packet send failed & rcved tx_abort for : 0x%x", port->port_index);
    
    packet = (aprv2_packet_t *)pkt_priv;
    packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);
    handle = ((uint32_t)
             (((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr)) << 8)
                   | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, packet->dst_addr))));
   ( void )  __aprv2_cmd_end_command( handle, packet,APR_ENOSERVICE );
   return;
 }



/** Data transmit notification callback */  
static void apr_glink_tx_done_cb (
  glink_handle_type handle,    /* handle for the glink channel */
  const void        *priv,     /* priv client data passed in glink_open */
  const void        *pkt_priv, /* private client data assiciated with the
                                  tx pkt that client queued earlier */
  const void        *ptr,      /* pointer to the transmitted buffer */
  size_t            size       /* size of the packet */
)
{
  int32_t rc;
  apr_glink_port_t *port = (apr_glink_port_t *)priv;

  if ((NULL == ptr)||(NULL == port))
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_done_cb(): Rcvd NULL buffer ptr");
    return;
  }

  rc = aprv2_ipc_tx_done_cb(port->port_index, (void *)ptr, size);
  if (rc)
  {
    APR_DBG_1(APR_DBG_ERROR, "apr_glink_tx_done_cb(): Client cb returned error %d", rc);
  }
  return;

 }

APR_INTERNAL glink_err_type apr_glink_queue_rx_intents (
  uint32_t intent_size, 
  uint32_t num_of_intents,
  glink_handle_type glink_handle
)
{
  uint32_t index;
  glink_err_type glink_rc;

  /* Queue rx intents */
  for (index = 0; index < num_of_intents; index++)
  {
    if (GLINK_STATUS_SUCCESS != (glink_rc =
         glink_queue_rx_intent(glink_handle, NULL, intent_size)))
  {
        APR_DBG_2(APR_DBG_ERROR,
        "glink_queue_rx_intent() failed for size = %d @ count = %d.",
                                                   intent_size, index+1);
       return glink_rc;
     }
  }

  return GLINK_STATUS_SUCCESS;
}

/** GLink channel state change notification callback */  
static void apr_glink_state_cb (
  glink_handle_type         handle, /* handle for the glink channel */
  const void                *priv,  /* priv client data passed in glink_open */
  glink_channel_event_type  event   /* Notification event */
)
{
  apr_glink_port_t *port = (apr_glink_port_t *)priv;
  apr_glink_port_handle_t port_index = 0;
  glink_err_type glink_rc;
  glink_handle_type glink_handle;
  int32_t rc = APR_EOK;

  //APR_DBG_1(APR_DBG_HIGH, "apr_glink_state_cb() rcvd with event = 0x%x", event);
  if (NULL == port)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_state_cb(): Rcvd NULL priv pointer");
    return;
  }

  port_index = port->port_index;
  //glink_handle = port->glink_handle;
  glink_handle = handle;
  switch(event)
  {
    case GLINK_CONNECTED:
      port->state = APR_GLINK_CONNECTED;
      APR_DBG_1(APR_DBG_ERROR,"APR Glink Connected to Remote Client, ghandle: %d", port_index);

#ifdef APR_GLINK_INTENTS_SUPPORT  
         glink_rc = apr_glink_queue_rx_intents ( APR_GLINK_128_BYTES_BUFFER, 
                                                        APR_GLINK_NUM_OF_128_BYTES_INTENTS, glink_handle);
         glink_rc |= apr_glink_queue_rx_intents ( APR_GLINK_512_BYTES_BUFFER, 
                                                        APR_GLINK_NUM_OF_512_BYTES_INTENTS, glink_handle);
         glink_rc |= apr_glink_queue_rx_intents ( APR_GLINK_1024_BYTES_BUFFER, 
                                                        APR_GLINK_NUM_OF_1024_BYTES_INTENTS, glink_handle);
         glink_rc |= apr_glink_queue_rx_intents ( APR_GLINK_4096_BYTES_BUFFER, 
                                                        APR_GLINK_NUM_OF_4096_BYTES_INTENTS, glink_handle);
         if ( glink_rc != GLINK_STATUS_SUCCESS )
	  {
            APR_DBG_1(APR_DBG_ERROR,"APR Glink failed to queue Rx intents, closing the port, handle: %d", port_index);
		(void) glink_close(glink_handle);
		port->state = APR_GLINK_DISCONNECTING;
		return;
	  }
#endif // APR_GLINK_INTENTS_SUPPORT
  	  
#ifdef APR_GLINK_QOS_SUPPORT
      /* Set QoS Latency requirements */
      if (GLINK_STATUS_SUCCESS != (glink_rc = glink_qos_latency(port->glink_handle, APRV2_IPC_QOS_LATENCY_US, APRV2_IPC_QOS_PKT_SIZE)))
        {
          APR_DBG_2(APR_DBG_ERROR,"APR Glink failed to set QOS, handle: %d, rc = %d", port_index, glink_rc);
          //port->state = APR_GLINK_DISCONNECTING;
          //(void) glink_close(glink_handle);
          //return;
        }
#endif // APR_GLINK_QOS_SUPPORT
      break;

    case GLINK_REMOTE_DISCONNECTED:
      port->state = APR_GLINK_REMOTE_DISCONNECTED;
      //When the remote has disconnected, the QDSP side is expected to call glink_close (here via apr_glink_close).
      APR_DBG_1(APR_DBG_HIGH, "APR Remote Glink Client Disconnected, Calling apr_glink_close, handle: %d", port_index);
      (void)apr_glink_close(port_index);

      break;

    case GLINK_LOCAL_DISCONNECTED:
      port->state = APR_GLINK_DISCONNECTED;
      APR_DBG_1(APR_DBG_HIGH, "APR Local Glink Client Disconnected, handle: %d", port_index);
      //Re-open the glink channel.
      rc = apr_glink_open(glink_port_data[port_index].port_name,
                          sizeof(glink_port_data[port_index].port_name),
                          glink_port_data[port_index].channel_id, glink_port_data[port_index].index,
                          glink_port_data[port_index].remote_ss);
	  if(rc != APR_EOK)
      {
	     APR_DBG_2(APR_DBG_ERROR, "Glink open failed for the index:%d with status : %d", port_index, rc);
	  }
	  break;

    default:
      return;
  }
  
  return;
}

APR_INTERNAL int32_t apr_glink_init(void)
{
  uint32_t index;
  glink_link_id_type link_id;
  glink_err_type err;

  // Init apr_glink_port_pool state
  for (index = 0; index < APR_GLINK_MAX_PORTS; index++)
  {
     apr_glink_port_pool[index].state = APR_GLINK_DISCONNECTED;
     apr_glink_port_pool[index].port_index = index;
  }

  // Register glink_link_state_notif_cb for each remote subsystem link.
  // Note that we do not register this callback for each separate *channel*,
  // only for each link (each link can have multiple channels).
  for (index = 0; index < sizeof(glink_remote_ss_list)/sizeof(glink_remote_ss_list[0]); index++)
  {
     /* Initialize the structure */
     GLINK_LINK_ID_STRUCT_INIT(link_id);
     link_id.xport = "SMEM";
     link_id.remote_ss = glink_remote_ss_list[index];
     link_id.link_notifier = (glink_link_state_notif_cb)aprv2_ipc_glink_link_state_cb;
     err = glink_register_link_state_cb(&link_id, NULL);
     if (err == GLINK_STATUS_SUCCESS)
     {
       APR_DBG_1(APR_DBG_ERROR, "apr_glink_init: Successfully register linkstate cb, port_index: %d.", index);
     }
     else
     {
       APR_DBG_1(APR_DBG_ERROR, "apr_glink_init: Failed to register linkstate cb, port_index: %d.", index);
     }
  }

  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_deinit ( void )
{
  APR_DBG_0(APR_DBG_HIGH, "apr_glink_deinit() Inside DEINIT");
  return APR_EOK;
}

boolean apr_glink_notify_rx_intent_req_cb (
  glink_handle_type         handle,   /* handle for the glink channel */
  const void                *priv,    /* priv client data passed in glink_open */
  size_t                    req_size  /* Requested size */
)
{
  return TRUE;  
}

APR_INTERNAL int32_t apr_glink_open (char_t *port_name, uint32_t port_name_size, uint32_t remote_ssid, apr_glink_port_handle_t port_index, char *remote_ss)
{
  apr_glink_port_t *port = &apr_glink_port_pool[port_index];
  glink_open_config_type glink_cfg; 
  glink_err_type glink_rc;

  if (NULL == port_name || port_name_size < 2 /*1 char + '\0'*/ ||
                              port_name_size > APR_GLINK_MAX_CHANNEL_NAME_SIZE || NULL == remote_ss)
  {
    return APR_EBADPARAM;
  }

  if (APR_GLINK_CONNECTING == port->state) 
  {
     return APR_EOK;
  }

  // prepare G-link cfg
  mmstd_memset(&glink_cfg, 0, sizeof(glink_cfg));  
  glink_cfg.name = port_name;
  glink_cfg.options = GLINK_OPT_INITIAL_XPORT;
  glink_cfg.notify_rx = apr_glink_rx_cb;
  glink_cfg.notify_tx_done = apr_glink_tx_done_cb;
  glink_cfg.notify_tx_abort = apr_glink_tx_abort;
  glink_cfg.notify_state = apr_glink_state_cb;
  glink_cfg.notify_rx_intent_req = apr_glink_notify_rx_intent_req_cb;
  glink_cfg.remote_ss = remote_ss;
  glink_cfg.transport = "SMEM";

  mmstd_strlcpy(port->port_name, port_name, MMSTD_MIN(sizeof(port->port_name), port_name_size));
  glink_cfg.priv = port;
  port->state = APR_GLINK_CONNECTING;
  
  if (GLINK_STATUS_SUCCESS != (glink_rc = glink_open(&glink_cfg, &port->glink_handle)))
  {
     APR_DBG_1(APR_DBG_ERROR, "glink_open() failed with rc = %d.", glink_rc);
	 port->state = APR_GLINK_DISCONNECTED;
     return APR_EFAILED;
  }

  APR_DBG_1(APR_DBG_HIGH,"apr_glink_open() with port handle %d DONE.", port_index);
  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_close ( apr_glink_port_handle_t port_index )
{
  apr_glink_port_t *port = &apr_glink_port_pool[port_index];
  
  if (APR_GLINK_DISCONNECTED == port->state || APR_GLINK_DISCONNECTING == port->state)
  {
    return APR_EALREADY;
  }
  port->state = APR_GLINK_DISCONNECTING;

  if (port->glink_handle) 
  {
#ifdef APR_GLINK_QOS_SUPPORT
     (void) glink_qos_cancel(port->glink_handle);
#endif // APR_GLINK_QOS_SUPPORT
     (void) glink_close(port->glink_handle);
  }

  APR_DBG_0(APR_DBG_HIGH, "apr_glink_close() called APR_GLINK_DISCONNECTING");

  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_tx (apr_glink_port_handle_t port_index, void *buf, uint32_t size)
{
  apr_glink_port_t *port = &apr_glink_port_pool[port_index];
  glink_err_type glink_rc;

  if (APR_GLINK_CONNECTED != port->state)
  {
    APR_DBG_1(APR_DBG_ERROR, "apr_glink_tx(): port not connected, state=0x%x", port->state);
    return APR_ENOTREADY;
  }

  if (GLINK_STATUS_SUCCESS != (glink_rc = glink_tx(port->glink_handle, buf, buf, size, GLINK_TX_REQ_INTENT)))
  {
    APR_DBG_1(APR_DBG_ERROR, "glink_tx() failed with rc = %d.", glink_rc);
    return APR_EFAILED;
  }

  return APR_EOK;
}


APR_INTERNAL int32_t apr_glink_rx_done (apr_glink_port_handle_t port_index, void *buf)
{
  apr_glink_port_t *port = &apr_glink_port_pool[port_index];
  glink_err_type glink_rc;

  //APR_DBG_1(APR_DBG_HIGH, "apr_glink_rx_done() called with APR packet 0x%x", buf);
  if (APR_GLINK_CONNECTED != port->state)
  {
    return APR_ENOTREADY;
  }

  if (GLINK_STATUS_SUCCESS != (glink_rc = glink_rx_done(port->glink_handle, buf, TRUE)))
  {
    APR_DBG_1(APR_DBG_ERROR, "glink_rx_done() failed with rc = %d", glink_rc);
    return APR_EFAILED;
  }

  return APR_EOK;
}
