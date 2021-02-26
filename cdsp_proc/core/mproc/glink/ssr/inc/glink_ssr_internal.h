#ifndef GLINK_SSR_INTERNAL_H
#define GLINK_SSR_INTERNAL_H

/**
 * @file glink_ssr_internal.h
 *
 * Declares the coomon functions and macros for the 
 * GLINK SSR component. 
 */

/** \defgroup glink_ssr GLINK SSR
 * \ingroup MPROC
 *
 * Provides functions, types and macros for the GLINK SSR 
 * component for subsystems.
 *
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014 - 2016 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/ssr/inc/glink_ssr_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/15   bc      Remove unnecessary header
10/22/14   rs      Initial revision
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "glink.h"

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  glink_ssr_ctrl_rx_cb
  ===========================================================================*/
/**
  This function implements the callback to process RX notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  pkt_priv     Pointer to the RX packet's private data.
  @param[in]  buffer       Pointer to the RX buffer.
  @param[in]  size         RX buffer size.
  @param[in]  intent_used  Size of the intent used for this packet.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_rx_cb
(
  glink_handle_type  handle,
  const void        *priv,
  const void        *pkt_priv,
  const void        *buffer,
  size_t             size,
  size_t             intent_used
);

/*===========================================================================
  FUNCTION  glink_ssr_rxv_cb
  ===========================================================================*/
/**
  This function implements the callback to process RX notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  pkt_priv     Pointer to the RX packet's private data.
  @param[in]  buffer       Pointer to the RX buffer.
  @param[in]  size         RX buffer size.
  @param[in]  intent_used  Size of the intent used for this packet.
  @param[in]  vprovider    Buffer provider for virtual space.
  @param[in]  pprovider    Buffer provider for physical space.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_rxv_cb
(
  glink_handle_type  handle,
  const void        *priv,
  const void        *pkt_priv,
  const void        *buffer,
  size_t             size,
  size_t             intent_used,
  glink_buffer_provider_fn vprovider,
  glink_buffer_provider_fn pprovider
);

/*===========================================================================
  FUNCTION  glink_ssr_tx_done_cb
  ===========================================================================*/
/**
  This function implements the callback to process TX Done notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  pkt_priv     Pointer to the TX packet's private data.
  @param[in]  buffer       Pointer to the TX buffer.
  @param[in]  size         RX buffer size.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_tx_done_cb
(
  glink_handle_type  handle,
  const void        *priv,
  const void        *pkt_priv,
  const void        *buffer,
  size_t             size
);

/*===========================================================================
  FUNCTION  glink_ssr_state_cb
  ===========================================================================*/
/**
  This function implements the callback to process state change notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  event        Channel state change event.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_state_cb
(
  glink_handle_type         handle,
  const void               *priv,
  glink_channel_event_type event
);

/*===========================================================================
  FUNCTION  glink_ssr_rx_intent_req_cb
  ===========================================================================*/
/**
  This function implements the callback to process a request to queue an RX
  intent of the specified size.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  size         Requested size.
 
  @return
  NA.

*/
/*=========================================================================*/
boolean glink_ssr_rx_intent_req_cb
(
  glink_handle_type         handle,
  const void               *priv,
  size_t                    size
);

/*===========================================================================
  FUNCTION  glink_ssr_rx_intent_cb
  ===========================================================================*/
/**
  This function implements the callback to process the notification of new
  intent queued.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  size         Requested size.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_rx_intent_cb
(
  glink_handle_type         handle,
  const void               *priv,
  size_t                    size
);

/*===========================================================================
  FUNCTION  glink_ssr_rx_sigs_cb
  ===========================================================================*/
/**
  This function implements the callback to process the notification of new
  intent queued.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  prev         Previous signals.
  @param[in]  curr         Current signals.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_rx_sigs_cb
(
  glink_handle_type         handle,
  const void               *priv,
  uint32                    prev,
  uint32                    curr
);

/*===========================================================================
  FUNCTION  glink_ssr_rx_abort_cb
  ===========================================================================*/
/**
  This function implements the callback to process RX abort notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  pkt_priv     Pointer to the RX packet's private data.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_rx_abort_cb
(
  glink_handle_type  handle,
  const void        *priv,
  const void        *pkt_priv
);

/*===========================================================================
  FUNCTION  glink_ssr_tx_abort_cb
  ===========================================================================*/
/**
  This function implements the callback to process TX abort notifications.
 
  @param[in]  handle       Pointer to the G-LINK channel handle.
  @param[in]  priv         Pointer to the channel's private data.
  @param[in]  pkt_priv     Pointer to the TX packet's private data.
 
  @return
  NA.

*/
/*=========================================================================*/
void glink_ssr_tx_abort_cb
(
  glink_handle_type  handle,
  const void        *priv,
  const void        *pkt_priv
);

/*===========================================================================
  FUNCTION  glink_ssr_link_state_cb
===========================================================================*/
/**
  This function services SSR native transport link notifications.

  @param[in]  link_info   Pointer to the affected link.
  @param[in]  priv        Associated callback data (currently NULL).
 
  @return
  NA

*/
/*=========================================================================*/
void glink_ssr_link_state_cb( glink_link_info_type *link_info, void *priv );

extern void glink_ssr( const char* remote_ss );

#endif   /* GLINK_SSR_INTERNAL_H */
