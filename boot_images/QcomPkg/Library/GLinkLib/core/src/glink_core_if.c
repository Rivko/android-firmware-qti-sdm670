/*===========================================================================

                 GLINK Core<->transport Interface Source File


 Copyright (c) 2014-2015 by QUALCOMM Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_core_if.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/16   sm      Cleanup ch ctx in close ack when link is down
11/30/15   ap      Handle cases when the channel is not found by rcid or lcid
10/07/15   an      Added support for Rx abort
08/17/15   bc      Add error check for channel state in rx open ack
08/14/15   pp      Handle the case where the remote ss sending close might 
                   crash before we handle the close request from it
07/15/15   bc      Remove ASSERT condition in rx sig
07/10/15   rs      + Moved all statements after local declarations 
                   + Use OS defined macro for unused parameters
07/07/15   bc      Invalidate channel id when it's closed
06/19/15   pp      Update to use Ulogs and support for stats and diag commands
06/10/15   bc      Fix ASSERT of checking version index
                   Updated channel ctx searching, using utility function
05/20/15   an      support for deferred channel cleanup in case of SSR
05/29/15   bc      Ignore open ack when close is called before getting ack
05/18/15   bc      Separate channel migration from common core
05/06/15   bc      Bug fix for channel migration
04/17/15   bc      Separated channel state variable in context
04/14/15   bc      Channel migration update
03/24/15   an      Tx packet scheduler
03/20/15   an      Removed dead code
03/24/15   db      Added lock while changing xport status to REGISTERED/
                   NEGOTIATING/LINK_UP
                   Added support in glink_rx_cmd_ch_close_ack() function
                   for channel state when SSR happens				   
02/12/15   an      Fix accessing channel context after freeing it
02/13/15   bc      Update channel state during channel migration
01/28/15   db      Added extra states and locking to nullify race condition
                   where both subsytems call glink_close at the same time
01/26/15   db      Modified glink_rx_cmd_ch_remote_close to avoid possible race
                   condition in REMOTE_DISCONNECTED notification.
01/23/15   bc      Fix for notifying LOCAL_DISCONNECTED event
01/02/15   bc      Channel migration fix
12/26/14   bc      Feature Negotiation update
12/18/14   bm      Add channel closing state transitions
12/09/14   db      Channel migration code added
11/12/14   an      Core refactoring for limited memory environment support
07/04/14   bm      Updated API to v0.6.5 GLink spec
03/22/14   bm      Initial version. 
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "glink_os_utils.h"
#include "glink.h"
#include "glink_internal.h"
#include "glink_core_intent.h"

/*===========================================================================
                        GLOBAL FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_process_negotiation_complete
===========================================================================*/
/**

  This is called when negotiation is complete. 
  It will set the version and call link up callback to notify


  @param[in]    xport_ctx    transport context
  
  @param[in]    if_ptr       Pointer to interface instance; must be unique
                             for each edge
  
  @param[in]    version      negotiated version

  @param[in]    features     negotiated with local side
              
  @return        None
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static void glink_process_negotiation_complete
(
  glink_transport_if_type   *if_ptr,
  uint32                    version,
  uint32                    features
)
{
  glink_core_xport_ctx_type *xport_ctx = if_ptr->glink_core_priv;
  /* Version/Feautre can be negotiated both in ver_req and ver_ack 
   * Only go through process once in case they are negotiated 
   * in ver_req before receiving ver_ack */
  
  if (glinki_xport_linkup(if_ptr))
  {
    return;
  }
  
  /* setup core based on transport capabilities*/
  xport_ctx->xport_capabilities = if_ptr->set_version(if_ptr,
                                                      version,
                                                      features);
  glink_core_setup(if_ptr);
  
  /* Scan the notification list to check is we have to let any registered
   * clients know that link came online */
  glinki_update_link_state_and_notify(if_ptr, GLINK_XPORT_LINK_UP);
}

/*===========================================================================
  FUNCTION      glink_get_current_version
===========================================================================*/
/**

  Get current version/feature. This is necessarily highest version.

  @param[in]     xport_ctx    transport context
  
  @return        pointer to version/feature if available.
                 NULL otherwise.
  
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static const glink_core_version_type *glink_get_current_version
(
  glink_core_xport_ctx_type *xport_ctx
)
{
  const glink_core_version_type *ver;

  ASSERT(xport_ctx->version_indx >= 0);
  ver = &xport_ctx->version_array[xport_ctx->version_indx];
  ASSERT(ver);
  
  return ver;
}

/*===========================================================================
  FUNCTION      glink_is_local_ch_closed
===========================================================================*/
/**

  Check if local channel is fully closed
  
  @param[in]     local_state local channel state
  
  @return        TRUE, if local channel is closed
  
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static boolean glink_is_local_ch_closed
(
  glink_local_state_type local_state
)
{
  return local_state == GLINK_LOCAL_CH_INIT ||
         local_state == GLINK_LOCAL_CH_CLOSED;
}

/*===========================================================================
  FUNCTION      glinki_scan_channels_and_notify_discon
===========================================================================*/
/** 
 * Close all the channels belong to this transport
 * This is helper function for glink_ssr
 *
 * @param[in]    if_ptr  pointer to transport interface
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
static void glinki_scan_channels_and_notify_discon
(
  glink_transport_if_type *if_ptr
)
{
  glink_channel_ctx_type     *open_ch_ctx, *next_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  uint32                      lcid;
  glink_local_state_type      local_state;
  glink_remote_state_type     remote_state;
  
  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  glinki_channel_ref_acquire(open_ch_ctx, FALSE);
  while(open_ch_ctx)
  {
    remote_state = open_ch_ctx->remote_state;
    local_state  = open_ch_ctx->local_state;
    next_ch_ctx  = smem_list_next(open_ch_ctx);
    lcid         = open_ch_ctx->lcid;


    /* Take a reference to the next channel context before
     * releasing the channel CS */
    glinki_channel_ref_acquire(next_ch_ctx, FALSE);

    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* If the remote is opened, fake the remote-close. Else just
     * notify the client if the local side is opened. */
    if(remote_state == GLINK_REMOTE_CH_OPENED)
    {
      if_ptr->glink_core_if_ptr->rx_cmd_ch_remote_close(if_ptr, open_ch_ctx->rcid);
    }
    else if(local_state == GLINK_LOCAL_CH_OPENED || 
             local_state == GLINK_LOCAL_CH_OPENING)
    {
      open_ch_ctx->notify_state(open_ch_ctx, open_ch_ctx->priv, GLINK_REMOTE_DISCONNECTED);
    }

    /* If the local side was waiting for a close-ack, fake it */
    if(local_state == GLINK_LOCAL_CH_CLOSING)
    {
      if_ptr->glink_core_if_ptr->rx_cmd_ch_close_ack(if_ptr, lcid);
    }

    /* Ensure that a close command is not sent to the remote subsystem
     * after it restarts (if the client delays calling glink_close() */
    open_ch_ctx->remote_state = GLINK_REMOTE_CH_SSR_RESET;

    /* Release reference taken for this call */
    glinki_channel_ref_release(open_ch_ctx);
    glink_os_cs_acquire(&xport_ctx->channel_q_cs);

    /* This should still exist since we have a reference for it */
    open_ch_ctx = next_ch_ctx;
  }
  glink_os_cs_release(&xport_ctx->channel_q_cs);
}
/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_link_up
===========================================================================*/
/**
  Indicates that transport is now ready to start negotiation 
  using the v0 configuration
    
  @param[in]     if_ptr    Pointer to interface instance; must be unique
                           for each edge

  @return        None
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
void glink_link_up
(
  glink_transport_if_type *if_ptr
)
{
  glink_core_xport_ctx_type     *xport_ctx;
  const glink_core_version_type *version_array;

  xport_ctx = if_ptr->glink_core_priv;

  version_array = glink_get_current_version(xport_ctx);

  /* Update the transport state */
  glinki_update_link_state_and_notify(if_ptr, GLINK_XPORT_NEGOTIATING);

  /* Start the negtiation */
  if_ptr->tx_cmd_version(if_ptr,
                         version_array->version,
                         version_array->features);

  GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_LINK_UP, "", 
                             xport_ctx->xport, 
                             xport_ctx->remote_ss, 
                             GLINK_STATUS_SUCCESS);
}



/*===========================================================================
FUNCTION      glink_link_down
===========================================================================*/
/**
  Indicates that transport is down 
    
  @param[in]     if_ptr    Pointer to interface instance; must be unique
                           for each edge

  @return        None
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
void glink_link_down
(
  glink_transport_if_type *if_ptr
)
{
  /* Invoke LINK_DOWN notification for any registered notifiers */
  glinki_update_link_state_and_notify(if_ptr, GLINK_XPORT_LINK_DOWN);
  /* Invoke REMOTE_DISCONNECT for all channels associated with if_ptr */
  glinki_scan_channels_and_notify_discon(if_ptr);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_version

DESCRIPTION   Receive transport version for remote-initiated version 
              negotiation

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version  Remote Version

            features Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
)
{
  const glink_core_version_type *ver;
  uint32 negotiated_features;
  glink_core_xport_ctx_type *xport_ctx;
  
  xport_ctx = if_ptr->glink_core_priv;
  
  ver = glink_get_current_version(xport_ctx);
  
  /* The version to use must be a subset of supported version and features
   * on this host and remote host */
  if (version == ver->version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);
    
    if_ptr->tx_cmd_version_ack(if_ptr, version, negotiated_features);
    
    /* If negotiated features match the provided features, version nogetiation
     * is complete */
    if(negotiated_features == features)
    {
      glink_process_negotiation_complete( if_ptr, version, features );
    }
  }
  else
  {
    /* Versions don't match, return ACK with the feature set that we support */
    if_ptr->tx_cmd_version_ack(if_ptr, ver->version, ver->features);
  }
}

/*===========================================================================
FUNCTION      glink_rx_cmd_version_ack

DESCRIPTION   Receive ACK to previous glink_transport_if::tx_cmd_version 
              command

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version  Remote Version

            features Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version_ack
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
)
{
  const glink_core_version_type* ver;
  uint32 negotiated_features;
  glink_core_xport_ctx_type *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  /* Check to see if version returned by remote end is supported by 
   * this host. Remote side would have acked only when the version/features
   * sent by this host did not match the remote */
  ver = glink_get_current_version(xport_ctx);
  
  if (ver->version == version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);

    if(negotiated_features == features)
    {
      glink_process_negotiation_complete( if_ptr, version, features );
    }
    else
    {
      /* Continue negotiating */
      if_ptr->tx_cmd_version(if_ptr, version, negotiated_features);
    }
  }
  else
  {
    while (ver->version > version && xport_ctx->version_indx > 0)
    {
      /* Next time use older version */
      xport_ctx->version_indx--;
      ver = &xport_ctx->version_array[xport_ctx->version_indx];
    }
    
    /* Versions don't match, return ACK with the feature set that we support */
    if_ptr->tx_cmd_version(if_ptr, ver->version, ver->features);
  }
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_remote_open
===========================================================================*/
/** 
 * Receive remote channel open request; 
 * Calls glink_transport_if:: tx_cmd_ch_remote_open_ack as a result
 *
 * @param[in]    if_ptr    Pointer to interface instance; must be unique
                           for each edge
 * @param[in]    rcid      Remote Channel ID
 * @param[in]    name      String name for logical channel
 * @param[in]    priority  xport priority requested by remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_remote_open
(
  glink_transport_if_type *if_ptr,
  uint32                  rcid,
  const char              *name,
  glink_xport_priority    priority
)
{
  glink_channel_ctx_type *remote_ch_ctx  = NULL;
  glink_channel_ctx_type *allocated_ch_ctx;
  glink_err_type          status;

  GLINK_OS_UNREFERENCED_PARAM( priority );
  
  /* Allocate and initialize channel info structure */
  remote_ch_ctx = glink_os_calloc(sizeof(glink_channel_ctx_type));
  if (remote_ch_ctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_RM_CH_OPEN,
                           name,
                           if_ptr->glink_core_priv->xport,
                           if_ptr->glink_core_priv->remote_ss,
                           GLINK_STATUS_OUT_OF_RESOURCES );
    ASSERT(0);
  }
  
  /* Fill in the channel info structure */
  glink_os_string_copy(remote_ch_ctx->name, name, sizeof(remote_ch_ctx->name));
  remote_ch_ctx->rcid = rcid;

  status = glinki_add_ch_to_xport(if_ptr,
                                  remote_ch_ctx,
                                  &allocated_ch_ctx,
                                  FALSE,
                                  if_ptr->glink_priority);
  
  GLINK_LOG_EVENT( allocated_ch_ctx,
                   GLINK_EVENT_RM_CH_OPEN,
                   name,
                   if_ptr->glink_core_priv->xport,
                   if_ptr->glink_core_priv->remote_ss,
                   status );

  ASSERT(GLINK_STATUS_SUCCESS == status);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_open_ack
===========================================================================*/
/** 
 * This function is invoked by the transport
 * in response to glink_transport_if:: tx_cmd_ch_open
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    lcid              Local Channel ID
 * @param[in]    migrated_ch_prio  Negotiated xport priority from remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid,
  glink_xport_priority     migrated_ch_prio
)
{
  glink_channel_ctx_type    *open_ch_ctx;
  glink_core_xport_ctx_type *xport_ctx;
  glink_remote_state_type    remote_state;

  GLINK_OS_UNREFERENCED_PARAM( migrated_ch_prio );
  
  xport_ctx = if_ptr->glink_core_priv;

  /* Move to closed state. Implies we clean up the channel from the 
   * open list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_lcid(xport_ctx, lcid);
  
  if (!open_ch_ctx || open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING)
  {
    /* Client called glink_close before gettng open ack. 
     * Ignore open ack and go for closing sequence */
    OS_LOG_ERROR(2, "Dropping open ack for closed/closing channel(lcid=%d,chctx=%x)", 
                    lcid, open_ch_ctx);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }
  
  ASSERT(open_ch_ctx->local_state == GLINK_LOCAL_CH_OPENING);
  
  open_ch_ctx->local_state = GLINK_LOCAL_CH_OPENED;
  
  remote_state = open_ch_ctx->remote_state;
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  
  if (remote_state == GLINK_REMOTE_CH_OPENED)
  {
    /* remote channel is open on same xport as current xport.
     * change channel state to GLINK_CH_STATE_OPEN and notify client */
    open_ch_ctx->notify_state(open_ch_ctx,
                              open_ch_ctx->priv, 
                              GLINK_CONNECTED);
  }
  
  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_OPEN_ACK, 
                   open_ch_ctx->name, 
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   lcid );
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_close_ack
===========================================================================*/
/** 
 * This function is invoked by the transport in response to 
 * glink_transport_if_type:: tx_cmd_ch_close
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    lcid              Local Channel ID
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_close_ack
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_rx_intent_type *lc_intent;
  smem_list_type temp_q;
  glink_notify_rx_abort_cb rx_abort_cb = NULL;
  glink_state_notification_cb state_cb;
  const void *priv;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_lcid(xport_ctx, lcid);
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(2, "Dropping close ack for unknown channel(lcid=%d)", lcid);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }

  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_CLOSE_ACK, 
                   open_ch_ctx->name,
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   lcid);

  ASSERT(open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING);
  
  open_ch_ctx->local_state = GLINK_LOCAL_CH_CLOSED;
  open_ch_ctx->lcid        = GLINK_INVALID_CID;

  if (open_ch_ctx->notify_rx_abort != NULL && 
      open_ch_ctx->pintents != NULL)
  {
    glink_channel_intents_type *pintents = open_ch_ctx->pintents;

    rx_abort_cb = open_ch_ctx->notify_rx_abort;

    smem_list_init(&temp_q);

    glink_os_cs_acquire(&pintents->intent_q_cs);
    smem_list_move_all(&pintents->local_intent_q, &temp_q);
    glink_os_cs_release(&pintents->intent_q_cs);
  }

  priv = open_ch_ctx->priv;
  state_cb = open_ch_ctx->notify_state;

  glink_os_cs_release( &xport_ctx->channel_q_cs );

  if (rx_abort_cb != NULL)
  {
    for (lc_intent = smem_list_first(&temp_q);
         lc_intent != NULL;
         lc_intent = smem_list_first(&temp_q))
    {
      smem_list_delete(&temp_q, lc_intent);

      rx_abort_cb(open_ch_ctx, priv, lc_intent->pkt_priv);

      glink_cleanup_intent(lc_intent, if_ptr);
    }
  }

  state_cb(open_ch_ctx, priv, GLINK_LOCAL_DISCONNECTED);

  /* Release reference taken for the local client
   * (As part of glink_open) */
  glinki_channel_ref_release(open_ch_ctx);
}

/*===========================================================================
  FUNCTION      glink_rx_cmd_ch_remote_close
===========================================================================*/
/** 
 * Remote channel close request; will result in sending 
 * glink_transport_if_type:: tx_cmd_ch_remote_close_ack
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    rcid              Remote Channel ID
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glink_rx_cmd_ch_remote_close
(
  glink_transport_if_type *if_ptr,
  uint32                   rcid
)
{
  glink_channel_ctx_type    *open_ch_ctx;
  glink_core_xport_ctx_type *xport_ctx;
  
  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);

  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(2, "Dropping remote close for unknown channel(rcid=%d)", rcid);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }
  
  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_REMOTE_CLOSE, 
                   open_ch_ctx->name, 
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   rcid );

  /* It is possible that the remote subsystem sending close might crash 
     before we handle the close request from it */
  if (open_ch_ctx->remote_state != GLINK_REMOTE_CH_OPENED)
  {
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }

  if ( if_ptr->glink_core_if_ptr->rx_cmd_rx_intent_req_ack )
  {
    /* If there is a pending TX, unblock it */
    if_ptr->glink_core_if_ptr->rx_cmd_rx_intent_req_ack(if_ptr, 
                                             open_ch_ctx->rcid, FALSE);
  }
  
  open_ch_ctx->remote_state = GLINK_REMOTE_CH_CLOSED;
  open_ch_ctx->rcid         = GLINK_INVALID_CID;
  
  glink_os_cs_release(&xport_ctx->channel_q_cs);

  /* Send the remote close ACK back to the other side If link is up */
  if ( glinki_xport_linkup(if_ptr))
  {
    if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, rcid);
  }

  if ( !glink_is_local_ch_closed( open_ch_ctx->local_state ) )
  {
    /* Inform the client */
    open_ch_ctx->notify_state( open_ch_ctx,
                               open_ch_ctx->priv,
                               GLINK_REMOTE_DISCONNECTED );
  }

  /* Release reference taken for the remote client */
  glinki_channel_ref_release(open_ch_ctx);
}

/*===========================================================================
  FUNCTION      glink_rx_put_pkt_ctx
===========================================================================*/
/** 
 * Transport invokes this call to receive a packet fragment (must 
 * have previously received an rx_cmd_rx_data packet)
 *
 * @param[in]    if_ptr       Pointer to interface instance; must be unique
 *                            for each edge
 * @param[in]    rcid         Remote Channel ID
 * @param[in]    intent_ptr   Pointer to the intent fragment
 * @param[in]    complete     True if pkt is complete
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glink_rx_put_pkt_ctx
(
  glink_transport_if_type *if_ptr,
  uint32                   rcid,
  glink_rx_intent_type    *intent_ptr,
  boolean                  complete
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  GLINK_OS_UNREFERENCED_PARAM( complete );
  
  ASSERT(intent_ptr);

  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(2, "Dropping put_pkt_ctx for unknown channel(rcid=%d)", rcid);
    return;
  }
  
  GLINK_LOG_EVENT( open_ch_ctx,
                   GLINK_EVENT_CH_PUT_PKT_CTX, 
                   open_ch_ctx->name,
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   intent_ptr->iid);

  xport_ctx->channel_receive_pkt(open_ch_ctx, intent_ptr);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_remote_sigs

DESCRIPTION   Transport invokes this call to inform GLink of remote side
              changing its control signals

ARGUMENTS   *if_ptr      Pointer to interface instance; must be unique
                         for each edge

            rcid         Remote Channel ID

            remote_sigs  Remote signal state.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_remote_sigs
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  uint32                  rcid,       /* Remote channel ID */
  uint32                  remote_sigs /* Remote control signals */
)
{
  glink_core_xport_ctx_type *xport_ctx;
  glink_channel_ctx_type    *open_ch_ctx;
  uint32 prev_sigs;

  xport_ctx = if_ptr->glink_core_priv;
  
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  glink_os_cs_release( &xport_ctx->channel_q_cs );
  if(!open_ch_ctx)
  {
    OS_LOG_ERROR(3, "Dropping remote sigs update for unknown channel(rcid=%d), sigs=0x%x", rcid, remote_sigs);
    return;
  }
  
  /* Found channel, let client know of new remote signal state */
  prev_sigs = open_ch_ctx->remote_sigs;
  open_ch_ctx->remote_sigs = remote_sigs;
  open_ch_ctx->notify_rx_sigs(open_ch_ctx,
                              open_ch_ctx->priv,
                              prev_sigs,
                              remote_sigs);
}

