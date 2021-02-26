/*===========================================================================

                    GLINK API Source File


 Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/src/glink_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/17    gp      link state notification workarounds added
08/22/16    dks     Initial version. 
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "glink_internal.h"
#include "xport_qmp.h"

/*===========================================================================
                        GLOBAL DATA DECLARATIONS
===========================================================================*/
/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

static glink_transport_if *glink_registered_xport_if[GLINK_NUM_REMOTE_HOSTS];

#define MAX_GLINK_NOTIFY_CB 5 
static glink_link_notify_type glink_link_notify[MAX_GLINK_NOTIFY_CB];
static uint8 glink_link_notify_cntr = 0;

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_init

DESCRIPTION   Initializes the GLink core library.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_init(void)
{

}

glink_transport_if *glinki_find_xport_if_by_name (const char *remote_ss)
{
  uint32 i;
  for (i = 0; i < GLINK_NUM_REMOTE_HOSTS && glink_registered_xport_if[i] != NULL; i++)
  {
    if(strcmp(glink_registered_xport_if[i]->glink_ch_ctx.remote_ss, remote_ss) == 0)
    {
      return glink_registered_xport_if[i];
    }
  }
  return NULL;
}


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
  const glink_open_config_type *cfg_ptr,
  glink_handle_type            *handle
)
{
  glink_err_type           status;
  glink_channel_ctx        *open_ctx;
  glink_transport_if       *if_ptr;

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

  /* Find context to use */
  if_ptr = glinki_find_xport_if_by_name(cfg_ptr->remote_ss);
  if (if_ptr == NULL)
  {
    return GLINK_STATUS_NOT_INIT;  
  }

  open_ctx = &if_ptr->glink_ch_ctx;

  glink_os_cs_acquire(&open_ctx->ctx_cs);

  if (strcmp (open_ctx->ch_name, cfg_ptr->name) != 0)
  {
    glink_os_cs_release(&open_ctx->ctx_cs);
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  if (open_ctx->handle != NULL)
  {
    glink_os_cs_release(&open_ctx->ctx_cs);
    return GLINK_STATUS_INVALID_PARAM;
  }

  *handle = NULL;
  status = xport_qmp_ch_open(if_ptr);

  if (status == GLINK_STATUS_SUCCESS)
  {
    strlcpy (open_ctx->ch_name, cfg_ptr->name, sizeof(open_ctx->ch_name));
    open_ctx->notify_rx = cfg_ptr->notify_rx;
    open_ctx->notify_state = cfg_ptr->notify_state;
    open_ctx->notify_tx_done = cfg_ptr->notify_tx_done;
    open_ctx->notify_tx_abort = cfg_ptr->notify_tx_abort;
    open_ctx->handle = open_ctx;
    open_ctx->priv = cfg_ptr->priv;
    *handle = open_ctx->handle;
  }

  glink_os_cs_release(&open_ctx->ctx_cs);

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
  glink_channel_ctx *open_ctx = handle;

  if(open_ctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_CLOSE, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  glink_os_cs_acquire(&open_ctx->ctx_cs);
 
  status = xport_qmp_ch_close((glink_transport_if *)open_ctx);

  glink_os_cs_release(&open_ctx->ctx_cs);
  
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
  glink_err_type status;
  glink_packet_ctx pkt_ctx;
  glink_channel_ctx *open_ctx = (glink_channel_ctx *)handle;

  if(open_ctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_TX, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  glink_os_cs_acquire(&open_ctx->ctx_cs);

  if (open_ctx->handle == NULL)
  {
    glink_os_cs_release(&open_ctx->ctx_cs);
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;    
  }

  pkt_ctx.pkt = (void *)data;
  pkt_ctx.pkt_priv = pkt_priv;
  pkt_ctx.size = size;
  
  status = xport_qmp_tx((glink_transport_if *)open_ctx, &pkt_ctx);
  if (status == GLINK_STATUS_SUCCESS)
  {
    open_ctx->tx_in_progress = TRUE;
  }

  glink_os_cs_release(&open_ctx->ctx_cs);
  
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
 * @param[in]   reuse    Reuse intent. Ignored on micro-cores.
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
  glink_channel_ctx *open_ctx = (glink_channel_ctx *)handle;  
  glink_err_type result;

  (void) reuse;

  if(open_ctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_TX, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  glink_os_cs_acquire(&open_ctx->ctx_cs);

  result = xport_qmp_rx_done((glink_transport_if *)open_ctx, ptr);

  glink_os_cs_release(&open_ctx->ctx_cs);
 
  return result;
}


void glink_rx_cmd_remote_open (glink_transport_if *if_ptr)
{
  glink_channel_ctx *open_ctx = &if_ptr->glink_ch_ctx;
  open_ctx->notify_state(open_ctx, open_ctx->priv, 
                                   GLINK_CONNECTED);
}

void glink_rx_cmd_remote_close (glink_transport_if *if_ptr,
                                           glink_packet_ctx *pkt_ctx)
{
  glink_channel_ctx *open_ctx = &if_ptr->glink_ch_ctx;
  if (open_ctx->handle == NULL)
  {
    return; /* local closed the channel already */
  }

  if (open_ctx->tx_in_progress)
  {
    open_ctx->tx_in_progress = FALSE;
    open_ctx->notify_tx_abort(open_ctx, open_ctx->priv, pkt_ctx->pkt_priv);
  }

  open_ctx->notify_state(open_ctx->handle, open_ctx->priv,
                         GLINK_REMOTE_DISCONNECTED);
}

void glink_rx_cmd_ch_close_ack(glink_transport_if *if_ptr)
{
  glink_channel_ctx *open_ctx = &if_ptr->glink_ch_ctx;

  open_ctx->notify_state(open_ctx->handle, open_ctx->priv,
                         GLINK_LOCAL_DISCONNECTED);

  if_ptr->glink_ch_ctx.handle = NULL;
}


void glink_rx_cmd_tx_done (glink_transport_if *if_ptr,
                                    glink_packet_ctx *pkt_ctx)
{
  glink_channel_ctx *open_ctx = &if_ptr->glink_ch_ctx;
  open_ctx->tx_in_progress = FALSE;
  open_ctx->notify_tx_done (open_ctx, open_ctx->priv,
                                 pkt_ctx->pkt_priv, pkt_ctx->pkt, pkt_ctx->size);
}

void glink_rx_cmd_data (glink_transport_if *if_ptr, void *ptr, uint32 size)
{
  glink_channel_ctx *open_ctx = &if_ptr->glink_ch_ctx;
  open_ctx->notify_rx(open_ctx, open_ctx->priv, 
                                NULL, ptr, size, size);
}

void glink_register_transport (glink_transport_if *if_ptr)
{
  uint32 i;
  ASSERT (if_ptr != NULL);

  for (i = 0; i < GLINK_NUM_REMOTE_HOSTS; i++)
  {
    if (glink_registered_xport_if[i] == NULL)
    {
      break;
    }
  }

  if (i == GLINK_NUM_REMOTE_HOSTS)
  {
    ERR_FATAL("xports exhausted %d", i, 0, 0);
  }

  glink_registered_xport_if[i] = if_ptr;
}

void glink_register_notify_clients
(
 glink_link_notify_type* glink_notify
)
{
  if(glink_link_notify_cntr < MAX_GLINK_NOTIFY_CB) 
  {
    glink_os_mem_copy(&glink_link_notify[glink_link_notify_cntr].remote_ss[0], 
                      GLINK_MAX_HOST_NAME,
                      &glink_notify->remote_ss[0], 
                      GLINK_MAX_HOST_NAME);
    glink_link_notify[glink_link_notify_cntr].cb =glink_notify->cb; 
    glink_link_notify_cntr++;
  }  
}

void glink_notify_clients
(
 void* ctx_ptr
)
{

  uint8 i = 0;
  
  glink_transport_if* if_ptr = (glink_transport_if*)ctx_ptr;

  if(!if_ptr) 
  {
    return;
  }

  for(;i<glink_link_notify_cntr;i++) 
  {
    if(0 != strcmp(&glink_link_notify[i].remote_ss[0],&if_ptr->glink_ch_ctx.remote_ss[0]))
    {
      continue;
    }

    if(glink_link_notify[i].cb ) 
    {
      glink_link_notify[i].cb();
    }
  }
}

