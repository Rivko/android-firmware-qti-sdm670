/*===========================================================================

GLINK Core internal funcitons for full feature transport Source File


Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.  All Rights
Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_core_intent_xport.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/16   ab      Adding lb client and fixing client callback checks
07/10/15   rs      Use OS defined macro for unused parameters
06/19/15   pp      Update to use Ulogs and support for stats and diag commands
06/10/15   bc      Initial version of intent xport
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
  FUNCTION      glink_default_notify_rx_intent_req
===========================================================================*/
/**

  Default implementation of Rx request notification

  @param[in]  open_ch_ctx   Channel context.

  @return     TRUE if config is good.

  @sideeffects  None.
*/
/*=========================================================================*/
static boolean glink_default_notify_rx_intent_req
(
  glink_handle_type         handle,
  const void                *priv,
  size_t                    req_size
)
{
  GLINK_OS_UNREFERENCED_PARAM( handle );
  GLINK_OS_UNREFERENCED_PARAM( priv );
  GLINK_OS_UNREFERENCED_PARAM( req_size );
  return FALSE;
}

/*===========================================================================
  FUNCTION      glink_verify_open_cfg
===========================================================================*/
/**

  Verifies open config for a full featured transport

  @param[in]  open_ch_ctx   Channel context.

  @return     GLINK_STATUS_SUCCESS if config is good.
              GLINK_STATUS_INVALID_PARAM if config is missing expected callbacks

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_err_type glink_verify_open_cfg
(
  glink_channel_ctx_type   *ch_ctx
)
{
  if ((ch_ctx->notify_rx != NULL || ch_ctx->notify_rxv != NULL) && ch_ctx->notify_tx_done != NULL)
  {
    /* set default callbacks, where required */
    if (ch_ctx->notify_rx_intent_req == NULL) 
    {
      ch_ctx->notify_rx_intent_req = glink_default_notify_rx_intent_req;
    }
    
    if(ch_ctx->notify_rx_sigs == NULL)
    {
      ch_ctx->notify_rx_sigs = glink_default_notify_rx_sigs;
    }

    return GLINK_STATUS_SUCCESS;
  }

  return GLINK_STATUS_INVALID_PARAM;
}

/*===========================================================================
FUNCTION      glink_channel_receive_pkt
===========================================================================*/
/**

  Precesses Rx packet for the channel

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  intent_ptr    Packet descriptor

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
static void glink_channel_receive_pkt
(
  glink_channel_ctx_type *open_ch_ctx, 
  glink_rx_intent_type *intent_ptr
)
{
  ASSERT(open_ch_ctx->pintents->cur_read_intent == intent_ptr);
 
  if (intent_ptr->pkt_sz <= intent_ptr->used)
  {
    /* Add the intent to the list of intents currently with the client */
    glink_os_cs_acquire(&open_ch_ctx->pintents->intent_q_cs);
    open_ch_ctx->pintents->cur_read_intent = NULL;
    smem_list_append(&open_ch_ctx->pintents->local_intent_client_q, intent_ptr);
    glink_os_cs_release(&open_ch_ctx->pintents->intent_q_cs);

    /* Call caller regsitered Rx callback and free the intent from
    * the queue */
    if (open_ch_ctx->notify_rxv != NULL)
    {
      GLINK_UPDATE_CHANNEL_STATS( open_ch_ctx->ch_stats, rx_notify, 
                                  intent_ptr->pkt_sz );
      
      open_ch_ctx->notify_rxv(open_ch_ctx,
                              open_ch_ctx->priv,
                              intent_ptr->pkt_priv,
                              intent_ptr->iovec,
                              intent_ptr->pkt_sz,
                              intent_ptr->size,
                              intent_ptr->vprovider,
                              intent_ptr->pprovider);
    }
    else
    {
      size_t size;
      void* data = intent_ptr->vprovider(intent_ptr->iovec, 0, &size);

      ASSERT(data != NULL);

      if (size < intent_ptr->pkt_sz)
      {
        /* copy into bounce buffer */
        size_t offset = 0;

        ASSERT(intent_ptr->data != NULL);

        do
        {
          ASSERT(offset + size <= intent_ptr->pkt_sz);
          glink_os_copy_mem((char*)(intent_ptr->data) + offset, data, size);
          offset += size;
          data = intent_ptr->vprovider(intent_ptr->iovec, offset, &size);
        } while (data != NULL);

        data = intent_ptr->data;
      }
      GLINK_UPDATE_CHANNEL_STATS( open_ch_ctx->ch_stats, rx_notify, 
                                  intent_ptr->pkt_sz );

      open_ch_ctx->notify_rx(open_ch_ctx,
                             open_ch_ctx->priv,
                             intent_ptr->pkt_priv,
                             data,
                             intent_ptr->pkt_sz,
                             intent_ptr->size);
    }
  }
}

/*===========================================================================
  FUNCTION      glink_tx_resume
===========================================================================*/
/**
  If transport was full and could not continue a transmit
  operation, then it will call this function to notify the core
  that it is ready to resume transmission.

  #param[in]   *if_ptr    Pointer to transport interface instance

  @return  None.

  @sideeffects  None
*/
/*=========================================================================*/
static void glink_tx_resume
(
  glink_transport_if_type *if_ptr
)
{
  GLINK_OS_UNREFERENCED_PARAM( if_ptr );
}

/*===========================================================================
  FUNCTION      glink_channel_submit_pkt
===========================================================================*/
/**

  Adds a packet to Tx queue according to the channel QoS settings
  and signals transport-bound Tx thread for processing

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  pctx_ctx      Packet context.
  @param[in]  req_intent    Request intent flag

  @return     Error code.

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_err_type glink_channel_submit_pkt
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_core_tx_pkt_type *pctx,
  boolean                 req_intent
)
{
  glink_transport_if_type *if_ptr = open_ch_ctx->if_ptr;
  glink_err_type status = glink_channel_use_rm_intent(open_ch_ctx,
                                                      pctx,
                                                      req_intent);
  
  while(pctx->size_remaining != 0 && status == GLINK_STATUS_SUCCESS)
  {
    status = if_ptr->tx(if_ptr, open_ch_ctx->lcid, pctx);
  }
  
  glink_os_free(pctx);
  
  return status;
}

/*===========================================================================
                        GLOBAL DATA DECLARATION
===========================================================================*/
glink_core_if_type glink_intent_core_interface = 
{
  /** Indicates that transport is now ready to start negotiation using the 
   *  v0 configuration. */
  glink_link_up,
  /** Indicates that the transport is down  */
  glink_link_down,
  /** Receive transport version for remote-initiated version negotiation */
  glink_rx_cmd_version,
  /** Receive ACK to previous glink_transport_if_type::tx_cmd_version command */
  glink_rx_cmd_version_ack,
  /** Receive remote channel open request; expected response is 
   *  glink_transport_if_type:: tx_cmd_ch_remote_open_ack */
  glink_rx_cmd_ch_remote_open,
  /** This function is invoked by the transport in response to 
   *  glink_transport_if_type:: tx_cmd_ch_open */
  glink_rx_cmd_ch_open_ack,
  /** This function is invoked by the transport in response to 
   *  glink_transport_if_type:: tx_cmd_ch_close */
  glink_rx_cmd_ch_close_ack,
  /** Remote channel close request; will result in sending 
   *  glink_transport_if_type:: tx_cmd_ch_remote_close_ack */
  glink_rx_cmd_ch_remote_close,
  /** Transport invokes this call on receiving remote RX intent */
  glink_rx_cmd_remote_rx_intent_put,
  /** Transport invokes this call on receiving remote RX intent */
  glink_rx_cmd_remote_rx_intent_put_ext,
  /** Get receive packet context (if successful, should be followed by call to 
      rx_put_pkt_ctx) */
  glink_rx_get_pkt_ctx,
  /** Receive a packet fragment (must have previously received an rx_cmd_rx_data 
      packet). */
  glink_rx_put_pkt_ctx,
  /** Transport invokes this call to inform GLink that remote side is
   *   done with previous transmitted packet. */
  glink_rx_cmd_tx_done,
  /** Remote side is requesting an RX intent */
  glink_rx_cmd_remote_rx_intent_req,
  /** ACK to RX Intent Request */
  glink_rx_cmd_rx_intent_req_ack,
  /** Received 32-bit signals value from remote side. It is passed on to 
   *  the client */
  glink_rx_cmd_remote_sigs,
  /** If transport was full and could not continue a transmit operation,
   *  then it will call this function to notify the core that it is ready to 
   *  resume transmission. */
  glink_tx_resume
};

/*===========================================================================
  EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_core_get_intent_interface
===========================================================================*/
/**

  Provides core interface for the intentless transport.

  @return     Pointer to the core intentless interface.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_core_if_type* glink_core_get_intent_interface(void)
{
  return &glink_intent_core_interface;
}

/*===========================================================================
FUNCTION      glink_core_setup_intent_xport
===========================================================================*/
/**

  Initializes internal core functions for the intent featured transport.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup_intent_xport(glink_transport_if_type *if_ptr)
{
  glink_core_xport_ctx_type *xport_ctx   = if_ptr->glink_core_priv;

  if (if_ptr->allocate_rx_intent == NULL ||
      if_ptr->deallocate_rx_intent == NULL ||
      if_ptr->tx_cmd_local_rx_intent == NULL ||
      if_ptr->tx_cmd_local_rx_done == NULL) 
  {
    ASSERT(0); /* Need intent handling APIs */
  }

  if_ptr->glink_core_if_ptr = glink_core_get_intent_interface();
  
  xport_ctx->verify_open_cfg     = glink_verify_open_cfg;
  xport_ctx->channel_init        = glink_channel_intents_init;
  xport_ctx->channel_cleanup     = glink_channel_cleanup;
  xport_ctx->channel_submit_pkt  = glink_channel_submit_pkt;
  xport_ctx->channel_receive_pkt = glink_channel_receive_pkt;
}

/*===========================================================================
  FUNCTION      glink_core_qos_cancel
===========================================================================*/
/**

  Releases QoS resources.

  @param[in]  open_ch_ctx  Pointer to the channel context.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_qos_cancel(glink_channel_ctx_type *open_ch_ctx)
{
  return;
}
