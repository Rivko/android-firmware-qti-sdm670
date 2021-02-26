/*===========================================================================

GLINK Core internal funcitons for full feature transport Source File


Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.  All Rights
Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_core_intent.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/16   sm      Added timeout for wait on remote intent
02/18/16   dks     Handle deadlock: tx waits on intent when remote initiates close
11/30/15   ap      Handle cases when the channel is not found by rcid or lcid
10/07/15   an      Added support for Rx abort
06/19/15   pp      Update to use Ulogs and support for stats and diag commands
06/10/15   bc      Shared intent functions separated from full xport file
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_internal.h"
#include "glink_core_intent.h"

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glinki_get_rm_intent
===========================================================================*/
/**

  Finds matching remote intent

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  size          Size of intent.

  @return     Pointer to the intent.

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_rx_intent_type* glinki_get_rm_intent
(
  glink_channel_ctx_type   *ch_ctx,
  size_t                   size
)
{
  glink_rx_intent_type *remote_intent;

  /* Find first matching intent */
  remote_intent = smem_list_first(&ch_ctx->pintents->remote_intent_q);
  while( ( remote_intent != NULL ) && ( remote_intent->size < size ) )
  {
    /* traverse through the list until we find the intent of required size */
    remote_intent = smem_list_next( remote_intent );
  }

  if( remote_intent )
  {
    smem_list_delete(&ch_ctx->pintents->remote_intent_q, remote_intent );
  }

  return remote_intent;
}

/*===========================================================================
  FUNCTION      glinki_queue_intent
===========================================================================*/
/**

  Adds remote intent to the channel intent queue

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  rm_intent     Intent.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
static void glinki_queue_intent
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_rx_intent_type   *rm_intent
)
{
  size_t   rm_intent_req_size = 0;

  glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);

  smem_list_append(&open_ch_ctx->pintents->remote_intent_q, rm_intent);
  
  rm_intent_req_size = open_ch_ctx->pintents->rm_intent_req_size;

  glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);

  /* Signal the event in case tx was waiting on it */
  if (rm_intent_req_size != 0 &&
      rm_intent_req_size <= rm_intent->size)
  {
    open_ch_ctx->pintents->rm_intent_req_size = 0;
    glink_os_event_signal(&open_ch_ctx->pintents->rm_intent_avail_evt);
  }
  else if (open_ch_ctx->notify_rx_intent != NULL)
  {
    open_ch_ctx->notify_rx_intent(open_ch_ctx, open_ch_ctx->priv, rm_intent->size);
  }
}

/*===========================================================================
  FUNCTION      glink_cleanup_intent
===========================================================================*/
/**

  Release resources allocated for the intent

  @param[in]  pintent   Pointer to the intent object
  @param[in]  if_ptr    Transport interface pointer in case of freeing local
                        intent

  @return     None.
*/
/*=========================================================================*/
void glink_cleanup_intent
(
  glink_rx_intent_type    *pintent,
  glink_transport_if_type *if_ptr
)
{
  if (if_ptr)
  {
    if_ptr->deallocate_rx_intent(if_ptr, pintent);
    glink_os_free(pintent->data);
  }

  glink_os_free(pintent);
}

/*===========================================================================
  FUNCTION      glinki_free_intents
===========================================================================*/
/**

  Delete/Free all the intents from intent q (smem_list)

  @param[in]  intent_q  smem_list intent_q
  @param[in]  if_ptr    transport interface pointer in case of freeing local
                        intent

  @return     None.

  @sideeffects  This function needs to be called in protected context
*/
/*=========================================================================*/
static void glinki_free_intents
(
  smem_list_type          *intent_q,
  glink_transport_if_type *if_ptr
)
{
  glink_rx_intent_type *intent;

  for (intent = smem_list_first(intent_q);
       intent != NULL;
       intent = smem_list_first(intent_q))
  {
    smem_list_delete(intent_q, intent);
    glink_cleanup_intent(intent, if_ptr);
  }
}

/*===========================================================================
EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_channel_use_rm_intent
===========================================================================*/
/**

  Prepares matching remote intent for transmission. If request intent flag is
  set the function blocks until the response is received.

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  pctx_ctx      Packet context.
  @param[in]  req_intent    Request intent flag

  @return     Error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type glink_channel_use_rm_intent
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_core_tx_pkt_type *pctx,
  boolean                 req_intent
)
{
  glink_rx_intent_type   *rm_intent;

  /* Check to see if remote side has queued intent to receive data */
  glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);

  rm_intent = glinki_get_rm_intent(open_ch_ctx, pctx->size);

  if (rm_intent == NULL)
  {
    if (req_intent == FALSE)
    {
      /* No buffer to receive data at the remote end */
      glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);
      GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX,
                             open_ch_ctx->name,
                             open_ch_ctx->if_ptr->glink_core_priv->xport,
                             open_ch_ctx->if_ptr->glink_core_priv->remote_ss,
                             GLINK_STATUS_NO_REMOTE_INTENT_FOUND );
      return GLINK_STATUS_NO_REMOTE_INTENT_FOUND;
    }
    else
    {
      /* request intent from remote end and block until an intent is
       * available or till timeout value given by client */
      glink_os_event_clear(&open_ch_ctx->pintents->rm_intent_avail_evt);

      open_ch_ctx->pintents->rm_intent_req_size = pctx->size;

      /* Unlock intent_q cs before requesting for an intent and waiting
       * for said item */
      glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);

      /* Check state before tx and wait; if remote close was initiated by now,
         a fake intent ack might be cleared and glink_wait will stall forever.*/
      if (!glinki_channel_fully_opened(open_ch_ctx))
      {
        return GLINK_STATUS_CH_NOT_FULLY_OPENED;
      }

      if (GLINK_STATUS_SUCCESS !=
          open_ch_ctx->if_ptr->tx_cmd_rx_intent_req(open_ch_ctx->if_ptr,
                                                    open_ch_ctx->lcid,
                                                    pctx->size))
      {
        open_ch_ctx->pintents->rm_intent_req_size = 0;
        GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX,
                               open_ch_ctx->name,
                               open_ch_ctx->if_ptr->glink_core_priv->xport,
                               open_ch_ctx->if_ptr->glink_core_priv->remote_ss,
                               GLINK_STATUS_TX_CMD_FAILURE );
        return GLINK_STATUS_TX_CMD_FAILURE;
      }

      if(open_ch_ctx->rm_intent_timeout)
      {
        glink_err_type rv = GLINK_STATUS_SUCCESS;
        
        rv = glink_os_event_wait_timeout(
              &open_ch_ctx->pintents->rm_intent_avail_evt,
              open_ch_ctx->rm_intent_timeout);
        if(GLINK_STATUS_SUCCESS != rv)
        {
          /* timeout or error */
          GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, 
                                 open_ch_ctx->name, 
                                 open_ch_ctx->if_ptr->glink_core_priv->xport,
                                 open_ch_ctx->if_ptr->glink_core_priv->remote_ss, 
                                 rv );
          return rv;        
        }
      }
      else
      {
        glink_os_event_wait(&open_ch_ctx->pintents->rm_intent_avail_evt);
      }

      glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);
      rm_intent = glinki_get_rm_intent(open_ch_ctx, pctx->size);

      if (rm_intent == NULL)
      {
        glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);
        /* we have already tried sending a rx_intent request to remote
        side, it has probably NAC'ed. return error */
        GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX,
                               open_ch_ctx->name,
                               open_ch_ctx->if_ptr->glink_core_priv->xport,
                               open_ch_ctx->if_ptr->glink_core_priv->remote_ss,
                               GLINK_STATUS_NO_REMOTE_INTENT_FOUND );
        return GLINK_STATUS_NO_REMOTE_INTENT_FOUND;
      }
    }
  }

  pctx->iid = rm_intent->iid;
  pctx->intent_data = rm_intent->intent_data;

  /* Store the intent in pending_tx_q */
  rm_intent->data = (void*)pctx->data;
  rm_intent->pkt_priv = pctx->pkt_priv;
  rm_intent->pkt_sz = pctx->size;
  rm_intent->used = pctx->size;

  smem_list_append(&open_ch_ctx->pintents->remote_intent_pending_tx_q, rm_intent);
  glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);
  
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glink_channel_intents_init
===========================================================================*/
/**

  Initializes resources of the channel

  @param[in]  open_ch_ctx   Channel context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type glink_channel_intents_init(glink_channel_ctx_type *ch_ctx)
{
  ch_ctx->pintents = glink_os_calloc(sizeof(ch_ctx->pintents[0]));

  if (ch_ctx->pintents == NULL)
  {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  ch_ctx->pintents->ch_ctx = ch_ctx;

  smem_list_init(&ch_ctx->pintents->remote_intent_q);
  smem_list_init(&ch_ctx->pintents->remote_intent_pending_tx_q);
  smem_list_init(&ch_ctx->pintents->local_intent_q);
  smem_list_init(&ch_ctx->pintents->local_intent_client_q);
  glink_os_cs_init(&ch_ctx->pintents->intent_q_cs);

  /* Initialize events */
  glink_os_event_init(&ch_ctx->pintents->rm_intent_avail_evt);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glink_channel_cleanup
===========================================================================*/
/**

  Releases resources of the channel

  @param[in]  open_ch_ctx   Channel context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_channel_cleanup(glink_channel_ctx_type *open_ch_ctx)
{
  if (open_ch_ctx->pintents == NULL)
  {
    return;
  }

  glink_core_qos_cancel(open_ch_ctx);

  glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);

  glinki_free_intents(&open_ch_ctx->pintents->remote_intent_q, NULL);

  glinki_free_intents(&open_ch_ctx->pintents->remote_intent_pending_tx_q, NULL);

  glinki_free_intents(&open_ch_ctx->pintents->local_intent_q,
                      open_ch_ctx->if_ptr);

  glinki_free_intents(&open_ch_ctx->pintents->local_intent_client_q, NULL);

  glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);

  glink_os_cs_deinit(&open_ch_ctx->pintents->intent_q_cs);

  glink_os_event_deinit(&open_ch_ctx->pintents->rm_intent_avail_evt);

  glink_os_free(open_ch_ctx->pintents);

  open_ch_ctx->pintents = NULL;
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_remote_rx_intent_put_ext
===========================================================================*/
/**

  Transport invokes this call on receiving remote RX intent

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  riid      Remote intent ID
  @param[in]  size      Size of intent
  @param[in]  intent_data Data associated with the remote intent

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_rx_cmd_remote_rx_intent_put_ext
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  riid,    /* Remote intent ID */
  size_t                  size,    /* Size of receive intent */
  void                    *intent_data /* Data associated with the intent */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_rx_intent_type *rm_intent;
  glink_core_xport_ctx_type  *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(4, "Dropping intent(size=%d,riid=%d) for unknown channel(rcid=%d)", size, riid, rcid);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }

  /* Allocate an intent structure */
  rm_intent = glink_os_calloc(sizeof(glink_rx_intent_type));
  ASSERT(rm_intent != NULL);
  rm_intent->iid = riid;
  rm_intent->size = size;
  rm_intent->intent_data = intent_data;

      GLINK_LOG_EVENT( open_ch_ctx,
                       GLINK_EVENT_CH_INTENT_PUT,
                       open_ch_ctx->name,
                       xport_ctx->xport,
                       xport_ctx->remote_ss,
                       size );
  glink_os_cs_release(&xport_ctx->channel_q_cs);

  /* Found channel, queue the intent */
  glinki_queue_intent(open_ch_ctx, rm_intent);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_remote_rx_intent_put
===========================================================================*/
/**

  Transport invokes this call on receiving remote RX intent

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  riid      Remote intent ID
  @param[in]  size      Size of intent

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_rx_cmd_remote_rx_intent_put
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  riid,    /* Remote intent ID */
  size_t                  size     /* Size of receive intent */
)
{
  glink_rx_cmd_remote_rx_intent_put_ext(if_ptr, rcid, riid, size, NULL);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_tx_done
===========================================================================*/
/**

  Transport invokes this call to inform GLink that remote side is
  done with previous transmitted packet.

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  riid      Remote intent ID
  @param[in]  reuse     Reuse intent

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_rx_cmd_tx_done
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  riid,    /* Remote intent ID */
  boolean                 reuse    /* Reuse intent */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_rx_intent_type *rm_intent;
  glink_core_xport_ctx_type  *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);

  open_ch_ctx = glinki_find_ch_ctx_by_rcid( xport_ctx, rcid );
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(4, "Dropping TX Done(riid=%d) for unknown channel(rcid=%d)", riid, rcid);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }

  /* Found channel, check the list of pending completion intents */
  rm_intent = smem_list_first(&open_ch_ctx->pintents->remote_intent_pending_tx_q);
  while (rm_intent != NULL)
  {
    if (rm_intent->iid == riid)
    {
      /* found remote intent, delete it */
      glinki_dequeue_item(&open_ch_ctx->pintents->remote_intent_pending_tx_q,
                          rm_intent,
                          &open_ch_ctx->pintents->intent_q_cs);

      GLINK_LOG_EVENT( open_ch_ctx,
                       GLINK_EVENT_CH_TX_DONE,
                       open_ch_ctx->name,
                       xport_ctx->xport,
                       xport_ctx->remote_ss,
                       riid);

      GLINK_UPDATE_CHANNEL_STATS( open_ch_ctx->ch_stats, tx_done,
                                  rm_intent->pkt_sz );

      /* Let the client know that data was transmitted */
      glink_os_cs_release(&xport_ctx->channel_q_cs);

      open_ch_ctx->notify_tx_done(open_ch_ctx,
                                  open_ch_ctx->priv,
                                  rm_intent->pkt_priv,
                                  rm_intent->data,
                                  rm_intent->pkt_sz);

      if (reuse)
      {
        glinki_queue_intent(open_ch_ctx, rm_intent);
      }
      else
      {
        /* Free the intent */
        glink_os_free(rm_intent);
      }

      return;
    } /* end if */

    rm_intent = smem_list_next(rm_intent);
  } /* end while loop */

  glink_os_cs_release(&xport_ctx->channel_q_cs);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_rx_intent_req_ack
===========================================================================*/
/**

  ACK to RX Intent Request.

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  granted   True if RX Intent will be queued, FALSE otherwise

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_rx_cmd_rx_intent_req_ack
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  boolean                 granted  /* Whether or not request will be granted */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(2, "Dropping intent req for unknown channel(rcid=%d)", rcid);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }

  /* We got the ACK but we can't signal the glink_tx() call yet,
   * that event is signalled in glink_rx_cmd_remote_rx_intent_put
   * Only handle not granted case here */
  if (!granted && open_ch_ctx->pintents)
  {
    open_ch_ctx->pintents->rm_intent_req_size = 0;
    glink_os_event_signal(&open_ch_ctx->pintents->rm_intent_avail_evt);
  }

  glink_os_cs_release(&xport_ctx->channel_q_cs);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_remote_rx_intent_req
===========================================================================*/
/**

  Transport invokes this call to inform GLink that remote side is
  done with previous transmitted packet.

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  size      Size of the requested intent

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_rx_cmd_remote_rx_intent_req
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  size_t                  size     /* Size of the requested intent */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  boolean ack;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  glink_os_cs_release(&xport_ctx->channel_q_cs);

  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(3, "Dropping intent req (size=%d) for unknown channel(rcid=%d)", size, rcid);
    return;
  }

  /* Found channel, pass the intent request on to the client */
  ack = open_ch_ctx->notify_rx_intent_req(open_ch_ctx, open_ch_ctx->priv, size);

  /* send the client's response back to the other side */
  if_ptr->tx_cmd_remote_rx_intent_req_ack(if_ptr, open_ch_ctx->lcid, ack);
}

/*===========================================================================
  FUNCTION      glink_rx_get_pkt_ctx
===========================================================================*/
/**

  Transport invokes this call to get receive packet context
  (if successful, should be followed by call to rx_put_pkt_ctx)

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  liid      Local intent ID

  @return     Pointer to the intent structure.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_rx_intent_type* glink_rx_get_pkt_ctx
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  liid     /* Local intent ID */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_rx_intent_type *lc_intent;
  glink_core_xport_ctx_type  *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire( &xport_ctx->channel_q_cs );
  open_ch_ctx = glinki_find_ch_ctx_by_rcid( xport_ctx, rcid );
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(3, "Dropping packet req(liid=%d) for unknown channel(rcid=%d)", liid, rcid);
    glink_os_cs_release( &xport_ctx->channel_q_cs );
    return NULL;
  }

  /* Found channel, get the intent from the queue */
  if (open_ch_ctx->pintents->cur_read_intent == NULL)
  {
    glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);
    lc_intent = smem_list_first(&open_ch_ctx->pintents->local_intent_q);

    while (lc_intent && lc_intent->iid != liid)
    {
      lc_intent = smem_list_next(lc_intent);
    }

    ASSERT(lc_intent != NULL);

    open_ch_ctx->pintents->cur_read_intent = lc_intent;

    /* Remove the intent from the queue */
    smem_list_delete(&open_ch_ctx->pintents->local_intent_q, lc_intent);
    glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);
  }
  else
  {
    ASSERT(open_ch_ctx->pintents->cur_read_intent->iid == liid);
  }

  GLINK_LOG_EVENT( open_ch_ctx,
                   GLINK_EVENT_CH_GET_PKT_CTX,
                   open_ch_ctx->name,
                   xport_ctx->xport,
                   xport_ctx->remote_ss,
                   liid );

  glink_os_cs_release( &xport_ctx->channel_q_cs );

  return open_ch_ctx->pintents->cur_read_intent;
}

