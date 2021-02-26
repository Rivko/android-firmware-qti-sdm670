/*===========================================================================

               I P C    R O U T E R   X A L  G L I N K  C O M M O N

        This file provides OS generic functionality of the GLink
                            XAL for IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_xal_glink_common.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     IPC_ROUTER_XAL_GLINK_MAX_INTENT_TYPES moved to ipc_router_settings.h
11/06/14   aep     Initial creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_xal_glink_common.h"
#include "ipc_router_xal.h"
#include "ipc_router_settings.h"

static int ipc_enqueue_rx_intent
(
  ipc_router_xal_glink_port_type *port,
  size_t size,
  boolean is_static
);

/*===========================================================================
                       CALLBACK FUNCTIONS
===========================================================================*/

/*===========================================================================
CALLBACK ipc_notify_link_state

DESCRIPTION
   Called by GLink to handle link state changes

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_link_state
(
  glink_link_info_type *link_info,
  void *priv
)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;

  switch(link_info->link_state)
  {
    case GLINK_LINK_STATE_UP:
      ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_UP);
      break;
    case GLINK_LINK_STATE_DOWN:
      ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_DOWN);
      break;
    default:
      break;
  }
}

/*===========================================================================
CALLBACK ipc_free_cb

DESCRIPTION
   Called when the packet allocated by this module is freed
   (See ipc_notify_rx where we attach a buffer with this callback)

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_free_cb
(
  void *ptr,
  void *cookie
)
{
  ipc_router_xal_glink_intent_type *intent = (ipc_router_xal_glink_intent_type *)cookie;
  ipc_router_xal_glink_port_type *port;

  if(!intent || !intent->port)
  {
    return;
  }
  port = intent->port;
  
  /* This intent is not part of the intent pool, just
   * finish the rx and free the intent */
  if(intent->is_static == FALSE ||
      port->closing == TRUE ||
     NULL == (intent->pkt = ipc_router_packet_new(0,0)))
  {
    glink_rx_done(port->handle, ptr, FALSE);
    ipc_router_os_free(intent);
    return;
  }
  /* This is part of the pool, requeue the same intent */
  glink_rx_done(port->handle, ptr, TRUE);
}

/*===========================================================================
CALLBACK ipc_notify_rx

DESCRIPTION
   Called by GLink when there is a new packet.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_rx
(
  glink_handle_type handle,
  const void *priv,
  const void *pkt_priv,
  const void *ptr,
  size_t     size,
  size_t     intent_size
)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;
  ipc_router_xal_glink_intent_type *intent = (ipc_router_xal_glink_intent_type *)pkt_priv;
  int rc;

  rc = ipc_router_packet_attach(&intent->pkt, (void *)ptr, size, ipc_free_cb, intent);
  if(IPC_ROUTER_STATUS_SUCCESS != rc)
  {
    glink_rx_done(port->handle, ptr, TRUE);
    return;
  }

  ipc_router_packet_queue_put(&port->rx_queue, intent->pkt);

  ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_RX);
}

/*===========================================================================
CALLBACK ipc_notify_rx_abort

DESCRIPTION
   Called by GLink when the link is terminated so we may clean up
   any private data associated with the queued RX intents.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_rx_abort
(
  glink_handle_type handle,
  const void        *priv,
  const void        *pkt_priv
)
{
  ipc_router_xal_glink_intent_type *intent = (ipc_router_xal_glink_intent_type *)pkt_priv;
  if(intent)
  {
    ipc_router_packet_free(&intent->pkt);
    ipc_router_os_free(intent);
  }
}

/*===========================================================================
CALLBACK ipc_notify_tx_done

DESCRIPTION
   Called by GLink when the packet is successfully transmitted

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_tx_done
(
  glink_handle_type   handle,
  const void          *priv,
  const void          *pkt_priv,
  const void          *ptr,
  size_t              size
)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;
  ipc_router_packet_type *pkt = (ipc_router_packet_type *)pkt_priv;

  ipc_router_packet_queue_put(&port->tx_done_queue, pkt);

  ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_TX_DONE);
}

/*===========================================================================
CALLBACK ipc_notify_tx_abort

DESCRIPTION
   Called by GLink when the packet transmission was aborted because of link
   termination

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_tx_abort
(
  glink_handle_type handle,
  const void        *priv,
  const void        *pkt_priv
)
{
  /* Just call TX done since we dont have to do anything
   * special */
  ipc_notify_tx_done(handle, priv, pkt_priv, pkt_priv, 0);
}

/*===========================================================================
CALLBACK ipc_notify_state

DESCRIPTION
   Called by GLink to notify channel state changes

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_state
(
  glink_handle_type  handle,
  const void         *priv,
  glink_channel_event_type event
)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;

  /* There could be a race at bootup where
   * handle is not set */
  if(!port->handle)
  {
    port->handle = handle;
  }

  switch(event)
  {
    case GLINK_CONNECTED:
      ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_CONNECTED);
      break;
    case GLINK_LOCAL_DISCONNECTED:
      ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_LDISCONNECTED);
      break;
    case GLINK_REMOTE_DISCONNECTED:
      ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_RDISCONNECTED);
      break;
  }
}

/*===========================================================================
CALLBACK ipc_notify_rx_intent

DESCRIPTION
   Called by GLink to notify when remote client queues intent

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_notify_rx_intent(
  glink_handle_type  handle,
  const void         *priv,
  size_t             size
)
{
	ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;
	/* Since there is an rx intent queued on remote side, signal 
	   worker thread to check if there is any pending data to be transmitted */
	ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_TX);
}

/*===========================================================================
CALLBACK ipc_notify_rx_intent_req

DESCRIPTION
   Called by GLink when the remote router is requesting for an intent.
   We will return TRUE if the intent is part of our pool and will be queued
   eventually. Otherwise, we will enqueue (Poor-man's queue since we might 
   not be able to allocate in this callback) the request and queue the intent
   when the event is handled.

RETURN VALUE
   TRUE on success, FALSE otherwise. 

SIDE EFFECTS
===========================================================================*/
static boolean ipc_notify_rx_intent_req
(
  glink_handle_type  handle,
  const void         *priv,
  size_t             req_size
)
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)priv;

    int rc;
  
  /* If this function got called means the static
     intents are busy with other rx packets and thus
     create a dynamic intent for the tx packet on 
     the remote side */

    rc = ipc_enqueue_rx_intent(port, req_size, FALSE);
    if(rc != IPC_ROUTER_STATUS_SUCCESS)
    {
      return FALSE;
    }
  return TRUE;
}

/*===========================================================================
CALLBACK ipc_packet_provider

DESCRIPTION
   Called by GLink to access the ipc_router_packet_type IO Vector.
   (See more in glink.h about packet providers)

RETURN VALUE
   Pointer to the buffer requested

SIDE EFFECTS
===========================================================================*/
static void *ipc_packet_provider
(
  void *iovec,
  size_t offset,
  size_t *size
)
{
  struct priv_pkt_s {
    struct priv_pkt_s *next;
    uint32             len;
    void              *ptr;
  } *pkt = (struct priv_pkt_s *)iovec;
  void * ret = NULL;
  size_t off = 0;

  *size = 0;

  while(pkt)
  {
    size_t skip_len = offset - off;
    /* If I can skip at least skip_len in this packet,
     * then I can start from here */
    if(pkt->len > skip_len)
    {
      *size = pkt->len - skip_len;
      ret = (uint8 *)pkt->ptr + skip_len;
      break;
    }
    off += pkt->len;
    pkt = pkt->next;
  }
  return ret;
}


/*===========================================================================
                       LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION ipc_enqueue_rx_intent

DESCRIPTION
   Enqueues an intent of the specified size

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS on success, error code otherwise.

SIDE EFFECTS
===========================================================================*/
static int ipc_enqueue_rx_intent
(
  ipc_router_xal_glink_port_type *port,
  size_t size,
  boolean is_static
)
{
  ipc_router_xal_glink_intent_type *intent;

  intent = ipc_router_os_calloc(1, sizeof(*intent));
  if(!intent)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  LINK_INIT(intent->link);
  intent->is_static = is_static;
  intent->pkt = ipc_router_packet_new(0,0);
  if(!intent->pkt)
  {
    ipc_router_os_free(intent);
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  intent->size = size;
  intent->port = port;

  if(GLINK_STATUS_SUCCESS != glink_queue_rx_intent(port->handle, intent, size))
  {
    ipc_router_packet_free(&intent->pkt);
    ipc_router_os_free(intent);
    return IPC_ROUTER_STATUS_FAILURE;
  }
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION ipc_enqueue_rx_intents

DESCRIPTION
   Enqueues RX intents with a pattern provided during the creation of the
   port.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_enqueue_rx_intents
(
  ipc_router_xal_glink_port_type *port 
)
{
  int i,j;

  for(i = 0; i < IPC_ROUTER_XAL_GLINK_MAX_INTENT_TYPES; i++)
  {
    size_t size  = port->params.intents[i].size;
    size_t count = port->params.intents[i].count;
    if(!size || !count)
    {
      break;
    }
    for(j = 0; j < count; j++)
    {
      int rc = ipc_enqueue_rx_intent(port, size, TRUE);
      if(rc != IPC_ROUTER_STATUS_SUCCESS)
      {
        break;
      }
    }
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_local_connect

DESCRIPTION
   Opens a channel with GLink of parameters specified during open.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_local_connect
(
  ipc_router_xal_glink_port_type *port 
)
{
  glink_err_type err;
  glink_open_config_type config;

  /* This port will be in remote disconnected
   * till we get the connected cb */
  port->ch_state = GLINK_REMOTE_DISCONNECTED;

  /* Set all members to 0 */
  memset(&config, 0, sizeof(config));

  /* User specified options */
  config.transport = port->params.transport;
  config.remote_ss = port->params.remote_ss;
  config.name      = port->params.name;
  config.options   = port->params.options;

  /* XAL stuff */
  config.priv = port;
  config.notify_rx = ipc_notify_rx;
  config.notify_rx_abort = ipc_notify_rx_abort;
  config.notify_rxv = NULL;
  config.notify_tx_done = ipc_notify_tx_done;
  config.notify_tx_abort = ipc_notify_tx_abort;
  config.notify_state = ipc_notify_state;
  config.notify_rx_intent_req = ipc_notify_rx_intent_req;
  config.notify_rx_intent = ipc_notify_rx_intent;
  config.notify_rx_sigs = NULL;

  port->handle = NULL;

  err = glink_open(&config, &port->handle);
  if(err != GLINK_STATUS_SUCCESS)
  {
    /* Reset state back to local-disconnected */
    port->ch_state = GLINK_LOCAL_DISCONNECTED;
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_connect

DESCRIPTION
   Handles the event when the connection to a channel is established

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_handle_connect
(
  ipc_router_xal_glink_port_type *port 
)
{
  ipc_router_os_sem_lock(&port->lock);
  port->ch_state = GLINK_CONNECTED;
  ipc_router_os_sem_unlock(&port->lock);

  ipc_enqueue_rx_intents(port);

  ipc_router_xal_up(port->cb_handle);
}


/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_local_disconnect

DESCRIPTION
   Handles the event when the channel is successfully cleaned up by GLink

RETURN VALUE
   None

SIDE EFFECTS
  If the port is being closed, we will set the CLOSED event.
===========================================================================*/
static void ipc_router_xal_glink_handle_local_disconnect
(
  ipc_router_xal_glink_port_type *port 
)
{
  port->ch_state = GLINK_LOCAL_DISCONNECTED;

  /* If we are currently waiting to close, then just set the 
   * closed event and return, else we need to reconnect */
  if(port->closing == TRUE)
  {
    /* Port is successfully closed, now signal to complete */
    ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_CLOSED);
  }
  else
  {
    /* Start connect state machine only if the link is up,
     * else we need to wait till the UP notification is provided */
    if(port->link_state == GLINK_LINK_STATE_UP)
    {
      /* Link is up, so re-connect */
      ipc_router_xal_glink_local_connect(port);
    }
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_remote_disconnect

DESCRIPTION
   Handles the event when the remote router closes the channel or the remote
   subsystem terminates. If the latter is the case, we should have received
   a DOWN event. 

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_handle_remote_disconnect
(
  ipc_router_xal_glink_port_type *port 
)
{
  ipc_router_packet_type *pkt;
  glink_handle_type handle = port->handle;


  ipc_router_xal_down(port->cb_handle);
  port->handle = NULL;

  ipc_router_os_sem_lock(&port->lock);
  port->ch_state = GLINK_REMOTE_DISCONNECTED;
  while(NULL != (pkt = ipc_router_packet_queue_get(&port->rx_queue)))
  {
    ipc_router_packet_free(&pkt);
  }
  ipc_router_os_sem_unlock(&port->lock);

  port->ch_state = GLINK_LOCAL_DISCONNECTED;

  glink_close(handle);
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_tx

DESCRIPTION
   Handles the event when a packet needs to be transmitted

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_handle_tx
(
  ipc_router_xal_glink_port_type *port 
)
{
  ipc_router_packet_type *packet;
  glink_err_type          err;

  do{
      port->tx_leftover = packet = port->tx_leftover ? port->tx_leftover :
                                    ipc_router_packet_queue_get(&port->tx_queue);
      if(!packet)
      {
        break;
      }

      err = glink_txv(port->handle, packet, packet, ipc_router_packet_length(packet), 
	                    ipc_packet_provider, NULL, GLINK_TX_REQ_INTENT);
      if(err != GLINK_STATUS_SUCCESS)
      {
        /* Could not TX packet or an error was detected.
         * So re-try again later */
        break;
      }

      /* Packet written successfully */
      port->tx_leftover = NULL;
  }while(1);
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_tx_done

DESCRIPTION
   Handles the event when a packet is successfully transmitted.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_handle_tx_done
(
  ipc_router_xal_glink_port_type *port 
)
{
  ipc_router_packet_type *pkt;
  while(NULL != (pkt = ipc_router_packet_queue_get(&port->tx_done_queue)))
  {
    ipc_router_packet_free(&pkt);
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_rx

DESCRIPTION
   Handles the event when a packet is received (Present in the RX Queue).
   
RETURN VALUE
   NOne

SIDE EFFECTS
   Under heavy RX load, the worker thread may constantly be handling 
   incoming packets thus the thread wont be able to handle the TX_DONE
   event which frees the packet. Hence in order to avoid memory exhaustion
   we call ipc_router_xal_glink_handle_tx_done event 5 packets.
===========================================================================*/
static void ipc_router_xal_glink_handle_rx
(
  ipc_router_xal_glink_port_type *port 
)
{
  ipc_router_packet_type *pkt;
  uint32_t count = 0;
  while(NULL != (pkt = ipc_router_packet_queue_get(&port->rx_queue)))
  {
    ipc_router_xal_recv(pkt, port->cb_handle);
    if(++count % 5 == 0)
    {
      ipc_router_xal_glink_handle_tx_done(port);
    }
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_rx_req

DESCRIPTION
   Handles the event when the remote router is requesting for an intent
   of a packet size not supported by the pool.

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
static void ipc_router_xal_glink_handle_rx_req
(
  ipc_router_xal_glink_port_type *port 
)
{
  int rc;
  size_t size = 0;
  while(1)
  {
    int i;
    size = 0;
    ipc_router_os_sem_lock(&port->lock);
    for(i = 0; i < IPC_ROUTER_XAL_GLINK_MAX_REQ_INTENTS; i++)
    {
      size = port->requested_intents[i];
      port->requested_intents[i] = 0;
      if(size != 0)
      {
        break;
      }
    }
    ipc_router_os_sem_unlock(&port->lock);
    if(size == 0)
    {
      break;
    }
    rc = ipc_enqueue_rx_intent(port, size, FALSE);
    if(rc != IPC_ROUTER_STATUS_SUCCESS)
    {
      /* TODO Log error */
    }
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_handle_events

DESCRIPTION
   Handle all events specified in the mask 'got_events'

RETURN VALUE
   None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_glink_handle_events
(
  ipc_router_xal_glink_port_type *port, 
  unsigned int got_events
)
{
  /* Handle down before anything else. */
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_DOWN))
  {
    port->link_state = GLINK_LINK_STATE_DOWN;
    /* Do nothing, wait for disconnected to actually
     * cleanup */
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_UP))
  {
    if(port->link_state == GLINK_LINK_STATE_UP)
    {
      /* Do nothing if this is a spurious up
       * notification */
      return;
    }
    /* Link deemed up */
    port->link_state = GLINK_LINK_STATE_UP;
    ipc_router_xal_glink_local_connect(port);
  }
  /*  Handle disconnect before connect to avoid race conditions 
   *  which might lose the connect */
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_RDISCONNECTED))
  {
    ipc_router_xal_glink_handle_remote_disconnect(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_LDISCONNECTED))
  {
    ipc_router_xal_glink_handle_local_disconnect(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_CONNECTED))
  {
    ipc_router_xal_glink_handle_connect(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_TX))
  {
    ipc_router_xal_glink_handle_tx(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_TX_DONE))
  {
    ipc_router_xal_glink_handle_tx_done(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_RX))
  {
    ipc_router_xal_glink_handle_rx(port);
  }
  if(IS_EVENT_SET(got_events, IPC_ROUTER_XAL_GLINK_EVENT_RX_REQ))
  {
    ipc_router_xal_glink_handle_rx_req(port);
  }
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_open

DESCRIPTION
   opens a port on this XAL. This initiates the connection to the destination
   subsystem.

RETURN VALUE
   Xport handle or NULL on error

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_glink_open
(
 void *xport_params,
 void *cb_handle,
 void **out_handle
)
{
  ipc_router_xal_glink_param_type *param = (ipc_router_xal_glink_param_type *)xport_params;
  ipc_router_xal_glink_port_type *port;
  glink_err_type err;
  glink_link_id_type link_id;
  int rc;

  if(!param || !cb_handle)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  port = ipc_router_os_calloc(1, sizeof(*port));
  if(!port)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  *out_handle = (void *)port;

  port->cb_handle = cb_handle;
  port->closing = FALSE;

  /* Start with assuming that the link is down */
  port->link_state = GLINK_LINK_STATE_DOWN;
  /* Start with assuming that the channel is in local closed
   * (handle not valid yet) */
  port->ch_state   = GLINK_LOCAL_DISCONNECTED;
  port->handle = NULL;

  port->params = *param;
  port->tx_leftover = NULL;
  ipc_router_packet_queue_init(&port->tx_queue);
  ipc_router_packet_queue_init(&port->rx_queue);
  ipc_router_packet_queue_init(&port->tx_done_queue);
  ipc_router_os_sem_init(&port->lock);

  /* Set up worker thread attr, stack etc. */
  rc = ipc_router_xal_glink_os_init(port);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    goto bail_free;
  }

  GLINK_LINK_ID_STRUCT_INIT(link_id);
  link_id.xport = param->transport;
  link_id.remote_ss = param->remote_ss;
  link_id.link_notifier = ipc_notify_link_state;
  link_id.handle = NULL;

  err = glink_register_link_state_cb(&link_id, port);
  if(err != GLINK_STATUS_SUCCESS)
  {
    ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_CLOSED);
    port->closing = TRUE;
    ipc_router_xal_glink_os_deinit(port);
    goto bail_free;
  }
  port->link_state_handle = link_id.handle;
  return IPC_ROUTER_STATUS_SUCCESS;
bail_free:
  ipc_router_packet_queue_deinit(&port->rx_queue);
  ipc_router_packet_queue_deinit(&port->tx_done_queue);
  ipc_router_os_sem_deinit(&port->lock);
  ipc_router_os_free(port);
  *out_handle = NULL;
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_close

DESCRIPTION
   Closes a previously opened port on this XAL.

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS
   IPC_ROUTER_STATUS_INVALID_PARAM
   IPC_ROUTER_STATUS_INTERRUPTED
   IPC_ROUTER_STATUS_TIMEOUT

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_glink_close
(
 void *handle
)
{
  glink_err_type err;
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)handle;
  if(!handle)
  {
    return IPC_ROUTER_STATUS_FAILURE; 
  }

  /* Deregister link state notification to minimize the number
   * of active state changes while we are closing */
  glink_deregister_link_state_cb(port->link_state_handle);

  /* Set closing flag to TRUE so we dont reinit when
   * we get the LOCAL_DISCONNECTED callback */
  port->closing = TRUE;

  err = glink_close(port->handle);
  if(err != GLINK_STATUS_SUCCESS)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  /* Will wait till everything is cleaned up and it
   * also waits till GLink signals the LOCAL_DISCONNECT */
  ipc_router_xal_glink_os_deinit(port);

  ipc_router_xal_closed(port->cb_handle);

  ipc_router_packet_queue_deinit(&port->rx_queue);
  ipc_router_packet_queue_deinit(&port->tx_done_queue);
  ipc_router_os_sem_deinit(&port->lock);
  ipc_router_os_free(port);
  
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION ipc_router_xal_glink_write

DESCRIPTION
   Write data to the specified port.

RETURN VALUE
   IPC_ROUTER_STATUS_SUCCESS
   IPC_ROUTER_STATUS_INVALID_PARAM
   IPC_ROUTER_STATUS_INTERRUPTED

SIDE EFFECTS
===========================================================================*/
static int ipc_router_xal_glink_write
(
 void *handle,
 ipc_router_packet_type *msg
 )
{
  ipc_router_xal_glink_port_type *port = (ipc_router_xal_glink_port_type *)handle;

  if(!port || !msg)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  
  ipc_router_packet_queue_put(&port->tx_queue, msg);
  
  ipc_router_xal_glink_set_event(port, IPC_ROUTER_XAL_GLINK_EVENT_TX);

  return IPC_ROUTER_STATUS_SUCCESS;
}

ipc_router_xal_ops_type ipc_router_xal_glink =
{
  ipc_router_xal_glink_open,
  ipc_router_xal_glink_close,
  ipc_router_xal_glink_write,
};
