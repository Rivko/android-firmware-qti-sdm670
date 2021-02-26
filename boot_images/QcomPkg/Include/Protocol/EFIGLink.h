/** 
  @file  EFIGLink.h
  @brief GLink Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 02/10/15   an      Added teardown interface function
 02/05/15   rli     Compilation fixes and unique Protocol GUID.
 02/02/15   bm      Initial version for GLink lib.

=============================================================================*/
#ifndef __EFIGLINK_H__
#define __EFIGLINK_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_rpm.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_glink_constants
  Protocol version.
*/
#define EFI_GLINK_PROTOCOL_REVISION 0x0000000000010001

/*  Protocol GUID definition */
/** @ingroup efi_glink_protocol */
#define EFI_GLINK_PROTOCOL_GUID \
  { 0x3bfcf64d, 0x00cd, 0x41cb, { 0xbe, 0xa5, 0xa7, 0x6c, 0x3a, 0x24, 0x3d, 0xed } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/*
  External reference to the GLINK Protocol GUID.
 */
extern EFI_GUID gEfiGLINKProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* EFI_GLINK_REGISTER_LINK_STATE_CB */
/** @ingroup efi_glink_register_link_state_cb
 @par Summary
 Regsiters a client specified callback to be invoked when the specified
 transport (link) is up/down.

 @param[in]    link_id  Pointer to the configuration structure for the
                        xport(link) to be monitored. See glink.h
 @param[in]    priv     Callback data returned to client when callback
                        is invoked.
 @param[out]   ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Puts the callback in a queue which gets scanned when a 
               transport(link) comes up OR an SSR happnes.
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_REGISTER_LINK_STATE_CB)
(
  glink_link_id_type *link_id,
  void               *priv,
  glink_err_type     *ret  
);

/* EFI_GLINK_DEREGISTER_LINK_STATE_CB */
/** @ingroup efi_glink_deregister_link_state_cb
 @par Summary
 Degsiter the link UP/DOWN notification callback associated with the
 provided handle.

 @param[in]    handle  Callback handler returned by 
                       glink_register_link_state_cb

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Removes the callback in a queue which gets scanned when a 
               transport(link) comes up OR an SSR happnes.
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_DEREGISTER_LINK_STATE_CB)
(
  glink_link_handle_type handle,
  glink_err_type         *ret  
);

/* EFI_GLINK_OPEN */
/** @ingroup efi_glink_open
 @par Summary
 Opens a logical GLink based on the specified config params

 @param[in]    cfg_ptr  Pointer to the configuration structure for the
                        GLink. See glink.h
 @param[out]   handle   GLink handle associated with the logical channel

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Allocates channel resources and informs remote host about
               channel open.
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_OPEN)
(
  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle,
  glink_err_type         *ret  
);

/* EFI_GLINK_CLOSE */
/** @ingroup efi_glink_close
 @par Summary
 Closes the GLink logical channel specified by the handle.

 @param[in]    handle   GLink handle associated with the logical channel

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Closes local end of the channel and informs remote host
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_CLOSE)
(
  glink_handle_type handle,
  glink_err_type    *ret  
);

/* EFI_GLINK_TX */
/** @ingroup efi_glink_tx
 @par Summary
 Transmit the provided buffer over GLink.

 @param[in]    handle    GLink handle associated with the logical channel

 @param[in]   *pkt_priv  Per packet private data

 @param[in]   *data      Pointer to the data buffer to be transmitted

 @param[in]   size       Size of buffer

 @param[in]   options    Flags specifying how transmission for this buffer 
                         would be handled. See GLINK_TX_* flag definitions.

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Causes remote host to wake-up and process rx pkt
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_TX)
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options,
  glink_err_type    *ret  
);

/* EFI_GLINK_TXV */
/** @ingroup efi_glink_txv
 @par Summary
 Transmit the provided vector buffer over GLink.

 @param[in]    handle    GLink handle associated with the logical channel

 @param[in]   *pkt_priv  Per packet private data

 @param[in]   *iovec     Pointer to the vector buffer to be transmitted

 @param[in]   size       Size of buffer

 @param[in]   vprovider  Buffer provider for virtual space

 @param[in]   pprovider  Buffer provider for physical space

 @param[in]   options    Flags specifying how transmission for this buffer 
                         would be handled. See GLINK_TX_* flag definitions.

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  Causes remote host to wake-up and process rx pkt
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_TXV)
(
  glink_handle_type        handle,
  const void               *pkt_priv,
  void                     *iovec,
  size_t                   size,
  glink_buffer_provider_fn vprovider,
  glink_buffer_provider_fn pprovider,
  uint32                   options,
  glink_err_type           *ret  
);

/* EFI_GLINK_QUEUE_RX_INTENT */
/** @ingroup efi_glink_queue_rx_intent
 @par Summary
 Queue one or more Rx intent for the logical GPIC Link channel.

 @param[in]    handle   GLink handle associated with the logical channel

 @param[in]   *pkt_priv Per packet private data

 @param[in]   size      Size of buffer

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  GLin;k XAL allocates rx buffers for receiving packets
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_QUEUE_RX_INTENT)
(
  glink_handle_type handle,
  const void        *pkt_priv,
  size_t            size,
  glink_err_type    *ret  
);

/* EFI_GLINK_RX_DONE */
/** @ingroup efi_glink_rx_done
 @par Summary
 Client uses this to signal to GLink layer that it is done with the received 
 data buffer. This API should be called to free up the receive buffer, which,
 in zero-copy mode is actually remote-side's transmit buffer.

 @param[in]   handle   GLink handle associated with the logical channel

 @param[in]   *ptr     Pointer to the received buffer

 @param[in]   reuse    Reuse intent

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  GLink XAL frees the Rx buffer
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_RX_DONE)
(
  glink_handle_type handle,
  const void        *ptr,
  boolean           reuse,
  glink_err_type    *ret  
);

/* EFI_GLINK_SIGS_SET */
/** @ingroup efi_glink_sigs_set
 @par Summary
 Set the 32 bit control signal field. Depending on the transport, it may
 take appropriate actions on the set bit-mask, or transmit the entire 
 32-bit value to the remote host.

 @param[in]   handle     GLink handle associated with the logical channel

 @param[in]   sig_value  32 bit signal word

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  None
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_SIGS_SET)
(
  glink_handle_type handle,
  uint32            sig_value,
  glink_err_type    *ret  
);

/* EFI_GLINK_SIGS_LOCAL_GET */
/** @ingroup efi_glink_sigs_local_get
 @par Summary
 Get the local 32 bit control signal bit-field.

 @param[in]   handle      GLink handle associated with the logical channel

 @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  None
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_SIGS_LOCAL_GET)
(
  glink_handle_type handle,
  uint32            *sig_value,
  glink_err_type    *ret  
);

/* EFI_GLINK_SIGS_REMOTE_GET */
/** @ingroup efi_glink_sigs_remote_get
 @par Summary
 Get the remote 32 bit control signal bit-field.

 @param[in]   handle      GLink handle associated with the logical channel

 @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  None
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_SIGS_REMOTE_GET)
(
  glink_handle_type handle,
  uint32            *sig_value,
  glink_err_type    *ret  
);

/* EFI_GLINK_RPM_RX_POLL */
/** @ingroup efi_glink_rpm_rx_poll
 @par Summary
 This API allows the RPM client to poll the transport for any new data
 that might have come in. It would *usually* be used when incoming interrupts
 are disabled.

 @param[in]   handle      GLink handle associated with the logical channel

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  None
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_RPM_RX_POLL)
(
  glink_handle_type handle,
  glink_err_type    *ret  
);

/* EFI_GLINK_RPM_MASK_RX_INTERRUPT */
/** @ingroup efi_glink_rpm_mask_rx_interrupt
 @par Summary
 This API allows the RPM client to mask/unmask rx interrupts

 @param[in]   handle      GLink handle associated with the logical channel

 @param[in]   mask        Whether to mask or un-mask the incoming interrupt

 @param[in]   platform_struct Any platform specific into that transport may
                              require

 @param[out]   *ret      Return code from the actual glink API                       

 @return       Standard EFI error codes

 @sideeffects  None
*/
typedef EFI_STATUS (EFIAPI*  EFI_GLINK_RPM_MASK_RX_INTERRUPT)
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct,
  glink_err_type    *ret
);

/* EFI_SMDL_EXIT */
/** @ingroup efi_glink_rpm_teardown
  @par Summary
   Performs necessary deinitialization upon exit.
   @par
   This function will tear down all commenctions.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
*/
typedef EFI_STATUS (EFIAPI* EFI_GLINK_RPM_TEARDOWN)(
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_glink_protocol
  @par Summary
  GLink Protocol interface.

  @par Parameters
  @inputprotoparams{glink_proto_params.tex}
*/
typedef struct _EFI_GLINK_PROTOCOL {
  UINT64                              Revision;
  EFI_GLINK_REGISTER_LINK_STATE_CB    GlinkRegisterLinkStateCB;
  EFI_GLINK_DEREGISTER_LINK_STATE_CB  GlinkDeregisterLinkStateCB;
  EFI_GLINK_OPEN                      GlinkOpen;
  EFI_GLINK_CLOSE                     GlinkClose;
  EFI_GLINK_TX                        GlinkTx;
  EFI_GLINK_TXV                       GlinkTxV;
  EFI_GLINK_QUEUE_RX_INTENT           GlinkQueueRxIntent;
  EFI_GLINK_RX_DONE                   GlinkRxDone;
  EFI_GLINK_SIGS_SET                  GlinkSigsSet;
  EFI_GLINK_SIGS_LOCAL_GET            GlinkSigsLocalGet;
  EFI_GLINK_SIGS_REMOTE_GET           GlinkSigsRemoteGet;
  EFI_GLINK_RPM_RX_POLL               GlinkRpmRxPoll;
  EFI_GLINK_RPM_MASK_RX_INTERRUPT     GlinkRpmMaskRxInterrupt;
  EFI_GLINK_RPM_TEARDOWN              GlinkRpmTeardown;
}EFI_GLINK_PROTOCOL;

#endif /* __EFIGLINK_H__ */
