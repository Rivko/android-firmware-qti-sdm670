/*===========================================================================

GLINK Core internal funcitons for intentless transport Source File


Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.  All Rights
Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_core_intentless_xport.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/16   ab      Adding lb client and fixing client callback checks
07/10/15   rs      Use OS defined macro for unused parameters
05/18/15   bc      Remove unused function pointer
04/09/15   an      Memory optimizations
03/24/15   an      Tx packet scheduler
11/12/14   an      Core refactoring for limited memory environment support
===========================================================================*/

/*===========================================================================
INCLUDE FILES
===========================================================================*/

#include "glink.h"
#include "glink_internal.h"

glink_core_if_type glink_core_intentless_interface = 
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
  NULL,
  /** Transport invokes this call on receiving remote RX intent */
  NULL,
  /** Get receive packet context (if successful, should be followed by call to 
      rx_put_pkt_ctx) */
  NULL,
  /** Receive a packet fragment (must have previously received an rx_cmd_rx_data 
      packet). */
  glink_rx_put_pkt_ctx,
  /** Transport invokes this call to inform GLink that remote side is
   *   done with previous transmitted packet. */
  NULL,
  /** Remote side is requesting an RX intent */
  NULL,
  /** ACK to RX Intent Request */
  NULL,
  /** Received 32-bit signals value from remote side. It is passed on to 
   *  the client */
  glink_rx_cmd_remote_sigs,
  /** If transport was full and could not continue a transmit operation,
   *  then it will call this function to notify the core that it is ready to 
   *  resume transmission. */
  NULL
};


/*===========================================================================
LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_core_stub_intentless
===========================================================================*/
/**

  Stub for intentless transport functionality.

  @return     GLINK_STATUS_SUCCESS.

  @sideeffects  None.
*/
/*=========================================================================*/

static glink_err_type glink_core_stub_intentless(void)
{
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glink_verify_open_cfg_intentless
===========================================================================*/
/**

  Verifies open config for an intentless transport

  @param[in]  open_ch_ctx   Channel context.

  @return     GLINK_STATUS_SUCCESS if config is good.
              GLINK_STATUS_INVALID_PARAM if config is missing expected callbacks

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_err_type glink_verify_open_cfg_intentless
(
glink_channel_ctx_type   *ch_ctx
)
{
  if (ch_ctx->notify_rxv != NULL && ch_ctx->notify_tx_done != NULL)
  {
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
static void glink_channel_receive_pkt_intentless
(
  glink_channel_ctx_type *open_ch_ctx, 
  glink_rx_intent_type *intent_ptr
)
{
  open_ch_ctx->notify_rxv(open_ch_ctx, open_ch_ctx->priv,
    NULL, intent_ptr->iovec, intent_ptr->pkt_sz, 0,
    intent_ptr->vprovider, intent_ptr->pprovider);
}

/*===========================================================================
FUNCTION      glink_channel_submit_pkt
===========================================================================*/
/**

  Invokes intentless transport Tx function to transmit a packet

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  pctx_ctx      Packet context.
  @param[in]  req_intent    Request intent flag

  @return     Error code.

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_err_type glink_channel_submit_pkt_intentless
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_core_tx_pkt_type *pctx,
  boolean req_intent
)
{
  glink_transport_if_type *if_ptr = open_ch_ctx->if_ptr;
  glink_err_type status = if_ptr->tx(if_ptr, open_ch_ctx->lcid, pctx);

  GLINK_OS_UNREFERENCED_PARAM( req_intent );
  glink_os_free(pctx);

  return status;
}

/*===========================================================================
EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_core_get_intentless_interface
===========================================================================*/
/**

  Provides core interface for the intentless transport.

  @return     Pointer to the core intentless interface.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_core_if_type* glink_core_get_intentless_interface(void)
{
  return &glink_core_intentless_interface;
}

/*===========================================================================
  FUNCTION      glink_core_setup_intentless_xport
===========================================================================*/
/**

  Initializes internal core functions for the intentless transport.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup_intentless_xport(glink_transport_if_type *if_ptr)
{
  if_ptr->glink_core_if_ptr = glink_core_get_intentless_interface();
  if_ptr->glink_core_priv->verify_open_cfg = glink_verify_open_cfg_intentless;
  if_ptr->glink_core_priv->channel_init = (channel_init_fn)glink_core_stub_intentless;
  if_ptr->glink_core_priv->channel_cleanup = (channel_cleanup_fn)glink_core_stub_intentless;
  if_ptr->glink_core_priv->channel_submit_pkt = glink_channel_submit_pkt_intentless;
  if_ptr->glink_core_priv->channel_receive_pkt = glink_channel_receive_pkt_intentless;
}
