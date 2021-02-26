#ifndef GLINK_INTERNAL_H
#define GLINK_INTERNAL_H

/*===========================================================================

                    GLink Internal Header File


-----------------------------------------------------------------------------
Copyright (c) 2016 QUALCOMM Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/inc/glink_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16    dks     Initial version. 

===========================================================================*/

#include "glink.h"
#include "glink_os_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GLINK_MAX_CH_NAME 20

/* Tx packet context */
typedef struct glink_packet_ctx_type
{
  void *pkt;
  size_t size;
  const void *pkt_priv;
}glink_packet_ctx;

/* Logical channel conext */
typedef struct glink_channel_ctx_type{
  /* Remote subsystem name */
  char                        remote_ss[GLINK_MAX_HOST_NAME];

  /* Logical channel name */
  char                        ch_name[GLINK_MAX_CH_NAME];

  /** Private data for client to maintain context. Supplied on glink_open and
      is passed back to client in the notification callbacks */
  const void                          *priv;

  /** Data receive notification callback. Optional if notify_rxv is provided */
  glink_rx_notification_cb            notify_rx;

  /** Data transmit notification callback */
  glink_tx_notification_cb            notify_tx_done;

  /** GLink channel state notification callback */
  glink_state_notification_cb         notify_state;

  /** tx abort notification. This callback would be invoked if client
  *   had queued a tx buffer with glink and it had not been transmitted i.e.
  *   tx_done callback has not been called for this buffer and remote side
  *   or local side closed the port. Optional */
  glink_notify_tx_abort_cb            notify_tx_abort;

  /* Local channel ID */
  uint32                              lcid;

  /* Remote Channel ID */
  uint32                              rcid;

  /* Critical section to protect context */
  os_cs_type                          ctx_cs;
  
  /* Transmission state */
  boolean                             tx_in_progress;

  glink_handle_type                   handle;
} glink_channel_ctx;


typedef struct glink_transport_if_type {
  glink_channel_ctx glink_ch_ctx;
}glink_transport_if;


/*===========================================================================
FUNCTION      glink_register_transport

DESCRIPTION   Transport calls this API to register its interface with GLINK 
              Core

ARGUMENTS   *if_ptr   Pointer to xport to register with core

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_register_transport (glink_transport_if *if_ptr);


/*===========================================================================
FUNCTION      glink_rx_cmd_remote_open

DESCRIPTION   Transport calls this API to register its interface with GLINK 
              Core

ARGUMENTS   *if_ptr   Pointer to interface instance

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_remote_open (glink_transport_if *xport_handle);

void glink_rx_cmd_remote_close (glink_transport_if *xport_handle,
                                           glink_packet_ctx *pkt_ctx);

void glink_rx_cmd_ch_close_ack(glink_transport_if *xport_handle);


void glink_rx_cmd_tx_done (glink_transport_if *xport_handle,
                                   glink_packet_ctx *pkt_ctx);

void glink_rx_cmd_data (glink_transport_if *xport_handle,
                               void *ptr, uint32 size);

#endif /* GLINK_INTERNAL_H */
