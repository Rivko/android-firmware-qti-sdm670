#ifndef GLINK_CORE_INTENT_H
#define GLINK_CORE_INTENT_H

/*===========================================================================

                    GLink Core Intent Header File


-----------------------------------------------------------------------------
Copyright (c) 2015 QUALCOMM Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/inc/glink_core_intent.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   an      Added support for Rx abort
06/09/15   bc      Intent API
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "glink_core_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
/*===========================================================================
                           GLOBAL DATA DECLARATIONS
===========================================================================*/
/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
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
glink_core_if_type* glink_core_get_intent_interface(void);

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
);

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
glink_err_type glink_channel_intents_init
(
  glink_channel_ctx_type *ch_ctx
);

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
void glink_channel_cleanup
(
  glink_channel_ctx_type *open_ch_ctx
);

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
);

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
);

/*===========================================================================
  FUNCTION      glink_rx_cmd_tx_done
===========================================================================*/
/**

  Transport invokes this call to inform GLink that remote side is
  done with previous transmitted packet.

  @param[in]  if_ptr    Pointer to interface instance; must be unique
  @param[in]  rcid      Remote Channel ID
  @param[in]  riid      Remote intent ID

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
);

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
);

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
);

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
glink_rx_intent_type *glink_rx_get_pkt_ctx
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  liid     /* Local intent ID */
);

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
);

#endif /* GLINK_CORE_INTENT_H */
