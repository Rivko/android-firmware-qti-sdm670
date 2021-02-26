/*===========================================================================

                    GLINK API Source File


 Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/16    na     Updated glink_register_link_state_cb to copy parameters passed by client
05/30/16    ab     Adding lb client and fixing client callback checks
04/25/16    sm     Added timeout for wait on remote intent
10/07/15    an     Added support for Rx abort
08/20/15    pp      Change tx_cs type to os_mutex_type
07/24/15    db     Allow channel to be closed even if xport is not linked up
07/10/15    rs     Use OS defined macro for unused parameters 
06/19/15    pp     Update to use Ulogs and support for stats and diag commands
06/05/15    bc     Separate global data structure to internal file
05/20/15    an     fix for local channel ID exceeding max supported value
05/18/15    bc     update transport_q_cs to have cs per edge
05/14/15    bc     Update glink_open to avoid deadlock case
                   when channel migration happens at the same time
05/10/15    bc     Remove unnecessary channel migration variables
04/29/15    db     Fixed glink_close/glink_link_state_cb APIs for conditions
                   where NULL handle is passed
04/28/15    bc     Access handle after checking
04/17/15    bc     Seperated channel state variable in context
04/14/15    bc     Channel migration update
04/09/15    an     Memory optimizations
03/24/15    an     Tx packet scheduler
03/20/15    an     Copy-paste cleanup
03/24/15    db     SSR re-factoring
02/23/15    bc     Added tracer packet support
02/12/15    an     Fix race condition for channel state check and update
02/03/15    bc     Fix glink_sigs_set for referencing handle before checking
02/03/15    bc     Assign handle properly for glink_register_link_state_cb func
01/28/15    rs     + Temporary work-around to support RPM transport teardown, 
                     avoiding TX operations when detecting SSR
                   + Integrated TZ
01/28/14    db     Added extra states and locking to nullify race condition 
                   where both subsytems call glink_close at the same time
01/26/14    db     Handled LOCAL_OPEN_REMOTE_CLOSE state in glinki_add_ch_to_xport
                   function
12/19/14    bm     Add rx/tx abort notifiaction callbacks
12/18/14    bm     Add channel closing state transitions, and some other fixes
12/09/14    db     channel migration changes in code
11/12/14    an     Core refactoring for limited memory environment support
10/30/14    bm     GLink core fixes and updates as per new spec(0.7.3.9)
03/22/14    bm     Initial version. 
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_rpm.h"
#include "glink_os_utils.h"
#include "glink_internal.h"
#include "glink_core_intent.h"
#include "glink_vector.h"

#ifdef FEATURE_TRACER_PACKET
#include "glink_tracer.h"
#endif

#define GLINK_NOT_INITIALIZED 0
#define GLINK_INITIALIZED     1

/*===========================================================================
                        GLOBAL DATA DECLARATIONS
===========================================================================*/
/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/** Default implementation of optional callbacks */
void glink_default_notify_rx_sigs
(
  glink_handle_type  handle,
  const void         *priv,
  uint32             prev,
  uint32             curr
)
{
  GLINK_OS_UNREFERENCED_PARAM( handle );
  GLINK_OS_UNREFERENCED_PARAM( priv );
  GLINK_OS_UNREFERENCED_PARAM( prev );
  GLINK_OS_UNREFERENCED_PARAM( curr );
  return;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/** 
 * Opens a logical GLink based on the specified config params
 *
 * @param[in]    cfg_ptr  Pointer to the configuration structure for the
 *                        GLink. See glink.h
 * @param[out]   handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Allocates channel resources and informs remote host about
 *               channel open.
 */
glink_err_type glink_open
(
  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle
)
{
  glink_transport_if_type *if_ptr;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_channel_ctx_type  *ch_ctx;
  unsigned int             remote_host;
  glink_xport_priority     suggested_priority;
  glink_channel_ctx_type  *allocated_ch_ctx;
  glink_err_type           status;
  
  /* Param validation */
  if(cfg_ptr == NULL            ||
     cfg_ptr->remote_ss == NULL ||
     cfg_ptr->name == NULL      ||
     cfg_ptr->notify_state == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );    
    return GLINK_STATUS_INVALID_PARAM;
  }
  
  /* Evaluate the equivalent edge name->enum for future use */
  remote_host = glinki_find_remote_host(cfg_ptr->remote_ss);
  
  if (remote_host == GLINK_NUM_HOSTS)
  {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, 
                           cfg_ptr->name, "", 
                           cfg_ptr->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );    

    return GLINK_STATUS_INVALID_PARAM;
  }

  if_ptr = glinki_find_requested_xport(cfg_ptr->transport, 
                                       cfg_ptr->remote_ss,
                                       cfg_ptr->options,
                                       &suggested_priority);
  
  if (!if_ptr)
  {
    /* Code gets here if we are not able to find reqeusted transport */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN,
                           cfg_ptr->name,
                           cfg_ptr->transport,
                           cfg_ptr->remote_ss,
                           GLINK_STATUS_NO_TRANSPORT );
    return GLINK_STATUS_NO_TRANSPORT;
  }

  /* Allocate and initialize channel info structure */  
  ch_ctx = glink_os_calloc(sizeof(glink_channel_ctx_type));
  if(ch_ctx == NULL) 
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN, 
                           cfg_ptr->name, "", "",
                           GLINK_STATUS_OUT_OF_RESOURCES );
    return GLINK_STATUS_OUT_OF_RESOURCES;    
  }

  xport_ctx = if_ptr->glink_core_priv;
 
  /* Fill in the channel info structure */
  glink_os_string_copy(ch_ctx->name, cfg_ptr->name, sizeof(ch_ctx->name));
  ch_ctx->priv = cfg_ptr->priv;
  ch_ctx->notify_rx = cfg_ptr->notify_rx;
  ch_ctx->notify_rx_tracer_pkt = cfg_ptr->notify_rx_tracer_pkt;
  ch_ctx->notify_rxv = cfg_ptr->notify_rxv;
  ch_ctx->notify_tx_done = cfg_ptr->notify_tx_done;
  ch_ctx->notify_state = cfg_ptr->notify_state;
  ch_ctx->notify_rx_intent_req = cfg_ptr->notify_rx_intent_req;
  ch_ctx->notify_rx_intent = cfg_ptr->notify_rx_intent;
  ch_ctx->notify_rx_sigs = cfg_ptr->notify_rx_sigs;
  ch_ctx->ch_open_options = cfg_ptr->options;
  ch_ctx->rm_intent_timeout = cfg_ptr->rm_intent_timeout;
  ch_ctx->notify_rx_abort = cfg_ptr->notify_rx_abort;
  ch_ctx->notify_tx_abort = cfg_ptr->notify_tx_abort;
  if(cfg_ptr->options & GLINK_OPT_CLIENT_BUFFER_ALLOCATION)
  {
    ch_ctx->notify_allocate   = cfg_ptr->notify_allocate;
    ch_ctx->notify_deallocate = cfg_ptr->notify_deallocate;
  }

  status = xport_ctx->verify_open_cfg(ch_ctx);
  if(status != GLINK_STATUS_SUCCESS)
  {
    *handle = NULL;
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN,
                           cfg_ptr->name,
                           cfg_ptr->transport,
                           cfg_ptr->remote_ss,
                           status );
    glink_os_free(ch_ctx);
    return status;
  }
  
  status = glinki_add_ch_to_xport(if_ptr,
                                  ch_ctx,
                                  &allocated_ch_ctx,
                                  TRUE,
                                  suggested_priority);
  
  if(status == GLINK_STATUS_SUCCESS) 
  {
    /* Set the handle and return */
    *handle = allocated_ch_ctx;
    GLINK_LOG_EVENT( *handle,
                     GLINK_EVENT_CH_OPEN,
                     cfg_ptr->name,
                     cfg_ptr->transport,
                     cfg_ptr->remote_ss,
                     status );
  }
  else
  {
    /* Not freeing ch_ctx on error since 
       glinki_add_ch_to_xport frees ch_ctx on error */
    *handle = NULL;
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN,
                           cfg_ptr->name,
                           cfg_ptr->transport,
                           cfg_ptr->remote_ss,
                           status );
  }

  return status;
}
  
/** 
 * Closes the GLink logical channel specified by the handle.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Closes local end of the channel and informs remote host
 */
glink_err_type glink_close
(
  glink_handle_type handle
)
{
  glink_err_type status;
  glink_core_xport_ctx_type *xport_ctx = NULL;

  if(handle == NULL) 
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_CLOSE, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }
  
  /* get xport context after NULL check */
  xport_ctx = handle->if_ptr->glink_core_priv;  
  
  ASSERT( xport_ctx != NULL );
  
  /* grab lock to change/check channel state atomically */  
  glink_os_cs_acquire( &xport_ctx->channel_q_cs );

  /* Check to see if closed called again for same channel */
  if (handle->local_state != GLINK_LOCAL_CH_OPENED &&
      handle->local_state != GLINK_LOCAL_CH_OPENING)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_CLOSE, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           handle->local_state );

    glink_os_cs_release(&xport_ctx->channel_q_cs);
   
    return GLINK_STATUS_CH_ALREADY_CLOSED;
  }

  handle->local_state = GLINK_LOCAL_CH_CLOSING;
  
  if (handle->remote_state == GLINK_REMOTE_CH_SSR_RESET ||
      glinki_xport_linkup(handle->if_ptr) == FALSE)
  {
    /* SSR happened on remote-SS OR XPORT link is down. Fake close_ack from here */
    glink_os_cs_release( &xport_ctx->channel_q_cs);
    
    handle->if_ptr->glink_core_if_ptr->rx_cmd_ch_close_ack(handle->if_ptr,
                                                           handle->lcid);
  
    status = GLINK_STATUS_SUCCESS;
  }
  else
  {
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    
    status = handle->if_ptr->tx_cmd_ch_close(handle->if_ptr, handle->lcid);
    
    GLINK_LOG_EVENT( handle,
                     GLINK_EVENT_CH_CLOSE, 
                     handle->name, 
                     xport_ctx->xport,
                     xport_ctx->remote_ss, 
                     status );
  }
  
  return status;
}

/** 
 * Transmit the provided buffer over GLink.
 *
 * @param[in]    handle    GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv  Per packet private data
 *
 * @param[in]   *data      Pointer to the data buffer to be transmitted
 *
 * @param[in]   size       Size of buffer
 *
 * @param[in]   req_intent Whether to block and request for remote rx intent in
 *                         case it is not available for this pkt tx
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
glink_err_type glink_tx
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options
)
{
  return glink_txv(handle, pkt_priv, (void*)data, size, 
                   &glink_dummy_tx_vprovider, NULL, options);

}

/** 
 * Transmit the provided vector buffer over GLink.
 *
 * @param[in]    handle    GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv  Per packet private data
 *
 * @param[in]   *iovec     Pointer to the vector buffer to be transmitted
 *
 * @param[in]   size       Size of buffer
 *
 * @param[in]   vprovider  Buffer provider for virtual space
 *
 * @param[in]   pprovider  Buffer provider for physical space
 *
 * @param[in]   req_intent Whether to block and request for remote rx intent in
 *                         case it is not available for this pkt tx
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
glink_err_type glink_txv
(
  glink_handle_type        handle,
  const void               *pkt_priv,
  void                     *iovec,
  size_t                   size,
  glink_buffer_provider_fn vprovider,
  glink_buffer_provider_fn pprovider,
  uint32                   options
)
{
  glink_err_type         status;
  glink_core_tx_pkt_type *pctx;
  boolean                req_intent = options & GLINK_TX_REQ_INTENT;
  
  glink_core_xport_ctx_type *xport_ctx;

  /* Input validation */
  if(handle == NULL || iovec == NULL || size == 0 ||
     (vprovider == NULL && pprovider == NULL))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  xport_ctx = handle->if_ptr->glink_core_priv;
  
  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }
  
  pctx = glink_os_calloc(sizeof(glink_core_tx_pkt_type));

  if (pctx == NULL)
  {
    glinki_channel_ref_release(handle);
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           GLINK_STATUS_OUT_OF_RESOURCES );
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }
  
  /* Protect the entire tx operation under a lock as a client may call
     tx in different thread context */
  glink_os_mutex_acquire(&handle->tx_cs);

  pctx->pkt_priv = pkt_priv;
  pctx->size = size;
  pctx->size_remaining = size;
  pctx->vprovider = vprovider;
  pctx->pprovider = pprovider;
  
  if (vprovider == &glink_dummy_tx_vprovider)
  {
    pctx->data = (void*)iovec;
    pctx->iovec = pctx;
  }
  else
  {
    pctx->data = (void*)iovec;
    pctx->iovec = iovec;
  }

#ifdef FEATURE_TRACER_PACKET
  pctx->tracer_pkt = options & GLINK_TX_TRACER_PKT ? TRUE : FALSE;
  if( pctx->tracer_pkt )
  {
    glink_tracer_packet_log_pctx_pkt( pctx, GLINK_CORE_TX );
  }
#endif
  
  status = xport_ctx->channel_submit_pkt(handle, pctx, req_intent);
  
  /*Update the channel stats*/
  GLINK_UPDATE_CHANNEL_STATS( handle->ch_stats, tx_request, size);
  
  GLINK_LOG_EVENT( handle,
                   GLINK_EVENT_CH_TX, 
                   handle->name, 
                   xport_ctx->xport,
                   xport_ctx->remote_ss, 
                   status);

  glink_os_mutex_release(&handle->tx_cs);
  glinki_channel_ref_release(handle);
  return status;
}

/** 
 * Queue one or more Rx intent for the logical GPIC Link channel.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv Per packet private data
 *
 * @param[in]   size      Size of buffer
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  GLink XAL allocates rx buffers for receiving packets
 */
glink_err_type glink_queue_rx_intent
(
  glink_handle_type handle,
  const void        *pkt_priv,
  size_t            size
)
{
  glink_err_type             status;
  glink_rx_intent_type      *lc_intent;
  glink_core_xport_ctx_type *xport_ctx; 
  size_t tmp;

  /* Input validation */
  if(handle == NULL || size == 0)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  xport_ctx = handle->if_ptr->glink_core_priv;
  
  /* short circuit for intentless mode */
  if(xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS)
  {
    return GLINK_STATUS_API_NOT_SUPPORTED;
  }

  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  /* Allocate an intent structure */
  lc_intent = glink_os_calloc(sizeof(glink_rx_intent_type));
  if(lc_intent == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           GLINK_STATUS_OUT_OF_RESOURCES );
    glinki_channel_ref_release(handle);
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  glink_os_cs_acquire(&handle->if_ptr->glink_core_priv->liid_cs);
  lc_intent->iid = handle->if_ptr->glink_core_priv->liid++;
  glink_os_cs_release(&handle->if_ptr->glink_core_priv->liid_cs);

  lc_intent->size = size;
  lc_intent->pkt_priv = pkt_priv;
  lc_intent->handle = handle;

  /* Call transport API to allocate rx intent buffer */
  status = handle->if_ptr->allocate_rx_intent(handle->if_ptr, size, lc_intent);
  if(status != GLINK_STATUS_SUCCESS)
  {
    glink_os_free(lc_intent);
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           status );
    glinki_channel_ref_release(handle);
    return status;  
  }

  if (handle->notify_rxv == NULL && 
     (lc_intent->vprovider(lc_intent->iovec, 0, &tmp) == NULL || tmp < size))
  {
    /* Allocate bounce buffer for non-vectored Rx */
    lc_intent->data = glink_os_malloc(size);

    if(lc_intent->data == NULL)
    {
      handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
      glink_os_free(lc_intent);
      GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                             handle->name, 
                             xport_ctx->xport, 
                             xport_ctx->remote_ss, 
                             GLINK_STATUS_OUT_OF_RESOURCES );
      glinki_channel_ref_release(handle);
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
  }

  /* push the intent on local queue. Do this before calling tx cmd
     as transport may try to read data into the newly queued rx_buffer */
  glinki_enqueue_item(&handle->pintents->local_intent_q,
                      lc_intent,
                      &handle->pintents->intent_q_cs);

  /* Call transport API to queue rx intent */
  status = handle->if_ptr->tx_cmd_local_rx_intent(handle->if_ptr, 
              handle->lcid, size, lc_intent->iid);
  if(status != GLINK_STATUS_SUCCESS)
  {
    /* Failure */
    glinki_dequeue_item(&handle->pintents->local_intent_q,
                        lc_intent,
                        &handle->pintents->intent_q_cs);

    glink_cleanup_intent(lc_intent, handle->if_ptr);
  }

  GLINK_LOG_EVENT( handle,
                   GLINK_EVENT_CH_Q_RX_INTENT, 
                   handle->name, 
                   xport_ctx->xport,
                   xport_ctx->remote_ss, 
                   status );
  glinki_channel_ref_release(handle);
  return status;  
}

/** 
 * Client uses this to signal to GLink layer that it is done with the received 
 * data buffer. This API should be called to free up the receive buffer, which,
 * in zero-copy mode is actually remote-side's transmit buffer.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[in]   *ptr      Pointer to the received buffer
 *
 * @param[in]   reuse    Reuse intent
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  GLink XAL frees the Rx buffer
 */
glink_err_type glink_rx_done
(
  glink_handle_type handle,
  const void        *ptr,
  boolean           reuse
)
{
  glink_rx_intent_type      *lc_intent;
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;

  /* Input validation */
  if(handle == NULL || ptr == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* short circuit for intentless mode */
  if (xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS)
  {
    return GLINK_STATUS_API_NOT_SUPPORTED;
  }

  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, 
			   handle->name, 
			   xport_ctx->xport,
			   xport_ctx->remote_ss,
			   GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }
  
  /* Free the intent */
  lc_intent = smem_list_first(&handle->pintents->local_intent_client_q);
  while(lc_intent != NULL)
  {
    size_t tmp;

    if(lc_intent->iovec == ptr || (handle->notify_rxv == NULL && 
       (lc_intent->data == ptr || 
       ptr == lc_intent->vprovider(lc_intent->iovec, 0, &tmp))))
    {

      uint32 iid;

      /* Found intent, delete it */
      glinki_dequeue_item(&handle->pintents->local_intent_client_q,
                          lc_intent,
                          &handle->pintents->intent_q_cs);
      
      iid = lc_intent->iid;
      
      GLINK_UPDATE_CHANNEL_STATS( handle->ch_stats, rx_done, lc_intent->pkt_sz);

      if (reuse)
      {
        lc_intent->used = 0;
        
        glinki_enqueue_item(&handle->pintents->local_intent_q,
                            lc_intent,
                            &handle->pintents->intent_q_cs);
        
      }
      else
      {
        /* Free the intent */
        handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
        if(lc_intent->data)
        {
          /* Free the bounce buffer if we had allocated one */
          glink_os_free(lc_intent->data);
        }
        glink_os_free(lc_intent);
      }

      /* Note that the actual buffer, lc_intent->data, was allocated by the 
      * transport and should be freed by the xport. We should not touch it */
      /* Let the xport know we are done with the buffer */
      handle->if_ptr->tx_cmd_local_rx_done(handle->if_ptr,
                                           handle->lcid,
                                           iid,
                                           reuse);

      GLINK_LOG_EVENT( handle,
                       GLINK_EVENT_CH_RX_DONE, 
                       handle->name, 
                       xport_ctx->xport,
                       xport_ctx->remote_ss, 
                       GLINK_STATUS_SUCCESS );

      glinki_channel_ref_release(handle);
      return GLINK_STATUS_SUCCESS;     
    }
    lc_intent = smem_list_next(lc_intent);
  }

  GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, 
                         handle->name, 
                         xport_ctx->xport,
                         xport_ctx->remote_ss, 
                         GLINK_STATUS_INVALID_PARAM );
  glinki_channel_ref_release(handle);
  return GLINK_STATUS_INVALID_PARAM; 
}  

/** 
 * Set the 32 bit control signal field. Depending on the transport, it may
 * take appropriate actions on the set bit-mask, or transmit the entire 
 * 32-bit value to the remote host.
 *
 * @param[in]   handle     GLink handle associated with the logical channel
 *
 * @param[in]   sig_value  32 bit signal word
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_set
(
  glink_handle_type handle,
  uint32            sig_value
)
{
  glink_core_xport_ctx_type *xport_ctx;
  glink_err_type status;

  /* Input validation */
  if(handle == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_SET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  xport_ctx = handle->if_ptr->glink_core_priv;
  
  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_SET, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  status = handle->if_ptr->tx_cmd_set_sigs(handle->if_ptr,
                                           handle->lcid,
                                           sig_value);
  if(GLINK_STATUS_SUCCESS == status)
  {
    /* Update local copy of local control signal state */
    handle->local_sigs = sig_value;
  }
  glinki_channel_ref_release(handle);

  return status;
}

/** 
 * Get the local 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_local_get
(
  glink_handle_type handle,
  uint32            *sig_value
)
{
  /* Input validation */
  if(handle == NULL || sig_value == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_L_GET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_L_GET, 
                           handle->name, 
                           handle->if_ptr->glink_core_priv->xport,
                           handle->if_ptr->glink_core_priv->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  *sig_value = handle->local_sigs;

  glinki_channel_ref_release(handle);

  return GLINK_STATUS_SUCCESS;
}

/** 
 * Get the remote 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_remote_get
(
  glink_handle_type handle,
  uint32            *sig_value
)
{
  /* Input validation */
  if(handle == NULL || sig_value == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_R_GET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  if (!glinki_channel_ref_acquire(handle, TRUE))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_R_GET, 
                           handle->name, 
                           handle->if_ptr->glink_core_priv->xport,
                           handle->if_ptr->glink_core_priv->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  *sig_value = handle->remote_sigs;

  glinki_channel_ref_release(handle);

  return GLINK_STATUS_SUCCESS;
}

/** 
 * Regsiters a client specified callback to be invoked when the specified
 * transport (link) is up/down.
 *
 * @param[in]    link_id  Pointer to the configuration structure for the
 *                        xport(link) to be monitored. See glink.h
 * @param[in]    priv     Callback data returned to client when callback
 *                        is invoked.
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Puts the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
glink_err_type glink_register_link_state_cb
(
  glink_link_id_type *link_id,
  void*              priv
)
{
  glink_link_notif_data_type* link_notif_data;
  unsigned int remote_host;

  /* Input validation */
  if (link_id == NULL ||
      link_id->version != GLINK_LINK_ID_VER ||
      link_id->link_notifier == NULL)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Check for remote_ss validity */
  if (link_id->remote_ss != NULL)
  {
    remote_host = glinki_find_remote_host(link_id->remote_ss);
    if (remote_host == GLINK_NUM_HOSTS)
    {
      return GLINK_STATUS_INVALID_PARAM;
    }
  }
  
  /* Save the callback on the notification list */
  if((link_notif_data = glink_os_calloc(sizeof(glink_link_notif_data_type)))
       == NULL)
  {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }
  
  /* Copy the client provided parameters locally */
  if(link_id->xport == NULL)
  {
	link_notif_data->xport = NULL;  
  }
  else
  {
	if(strlen(link_id->xport)+1 > GLINK_XPORT_NAME_LEN)
	{
	  glink_os_free(link_notif_data);
	  return GLINK_STATUS_INVALID_PARAM;
	}
	else
	{
	  glink_os_string_copy(link_notif_data->xport_store, link_id->xport, GLINK_XPORT_NAME_LEN);
      link_notif_data->xport = link_notif_data->xport_store;	  
	}	
  }
  
  if(link_id->remote_ss == NULL)
  {
	link_notif_data->remote_ss = NULL;  
  }
  else
  {
	if(strlen(link_id->remote_ss)+1 > GLINK_REMOTE_SS_NAME_LEN)
	{
	  glink_os_free(link_notif_data);
	  return GLINK_STATUS_INVALID_PARAM;
	}
	else
	{
	  glink_os_string_copy(link_notif_data->remote_ss_store, link_id->remote_ss, GLINK_REMOTE_SS_NAME_LEN);
      link_notif_data->remote_ss = link_notif_data->remote_ss_store;	  
	}	
  }
    
  link_notif_data->link_notifier = link_id->link_notifier;
  link_notif_data->priv = priv; /* private client data */

  link_id->handle = (glink_link_handle_type)link_notif_data;

  /* Append the request to the list for link UP/DOWN notifications */
  glinki_register_link_notif_data(link_notif_data);

  return GLINK_STATUS_SUCCESS;
}

/** 
 * Degsiter the link UP/DOWN notification callback associated with the
 * provided handle.
 *
 * @param[in]    handle  Callback handler returned by 
 *                       glink_register_link_state_cb
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Removes the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
glink_err_type glink_deregister_link_state_cb
(
  glink_link_handle_type handle
)
{
  /* check if glink handle is NULL and return appropriate
     return code  */
  if(handle == NULL) 
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  glinki_deregister_link_notif_data((glink_link_notif_data_type *)handle);
  
  glink_os_free(handle);

  return GLINK_STATUS_SUCCESS;
}

/*================= RESTRICTED API ==========================*/

/** 
 * This API allows the RPM client to poll the transport for any new data
 * that might have come in. It would *usually* be used when incoming interrupts
 * are disabled.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_rx_poll
(
  glink_handle_type handle
)
{
  if (!handle)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(handle->if_ptr->poll)
  {
    return handle->if_ptr->poll(handle->if_ptr);
  }

  return GLINK_STATUS_API_NOT_SUPPORTED;
}

/** 
 * This API allows the RPM client to mask/unmask rx interrupts
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[in]   mask        Whether to mask or un-mask the incoming interrupt
 *
 * @param[in]   platform_struct Any platform specific into that transport may
                                require
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_mask_rx_interrupt
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct
)
{
  if (!handle)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }
  
  if(handle->if_ptr->mask_rx_irq)
  {
    return handle->if_ptr->mask_rx_irq(handle->if_ptr, mask);
  }
  return GLINK_STATUS_API_NOT_SUPPORTED;
}
