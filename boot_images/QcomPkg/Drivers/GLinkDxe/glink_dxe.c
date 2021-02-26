/**
 * @file glink_dxe.c
 *
 * DXE driver for Glink device in UEFI
 */

/*==============================================================================
     Copyright (c) 2015,2017 Qualcomm Technologies, Inc. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   an      Added teardown interface function
02/02/15   bm      Created UEFI support code for GLink
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIGLink.h>
#include "com_dtypes.h"
#include <Library/glink.h>
#include <Library/glink_rpm.h>


extern void smd_initialize_static(void);

extern void glink_init(void);
extern void xport_smd_init(void);
extern void xport_rpm_init(void);
extern void xport_rpm_config_init_toc_addr(void);
extern void glink_ssr_tear_down_rpm( void );
extern void xport_smem_init(void);


/*===========================================================================
                        INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/** 
 * Regsiters a client specified callback to be invoked when the specified
 * transport (link) is up/down.
 *
 * @param[in]    link_id  Pointer to the configuration structure for the
 *                        xport(link) to be monitored. See glink.h
 * @param[in]    priv     Callback data returned to client when callback
 *                        is invoked.
 * @param[out]   ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Puts the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
static EFI_STATUS EFI_glink_register_link_state_cb
(
  glink_link_id_type *link_id,
  void               *priv,
  glink_err_type     *ret  
)
{
  *ret = glink_register_link_state_cb(link_id,
                                      priv);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Degsiter the link UP/DOWN notification callback associated with the
 * provided handle.
 *
 * @param[in]    handle  Callback handler returned by 
 *                       glink_register_link_state_cb
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Removes the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
static EFI_STATUS EFI_glink_deregister_link_state_cb
(
  glink_link_handle_type handle,
  glink_err_type         *ret  
)
{
  *ret = glink_deregister_link_state_cb(handle);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Opens a logical GLink based on the specified config params
 *
 * @param[in]    cfg_ptr  Pointer to the configuration structure for the
 *                        GLink. See glink.h
 * @param[out]   handle   GLink handle associated with the logical channel
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Allocates channel resources and informs remote host about
 *               channel open.
 */
static EFI_STATUS EFI_glink_open
(
  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle,
  glink_err_type         *ret  
)
{
  *ret = glink_open(cfg_ptr,
                    handle);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Closes the GLink logical channel specified by the handle.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Closes local end of the channel and informs remote host
 */
static EFI_STATUS EFI_glink_close
(
  glink_handle_type handle,
  glink_err_type    *ret  
)
{
  *ret = glink_close(handle);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
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
 * @param[in]   options    Flags specifying how transmission for this buffer 
 *                         would be handled. See GLINK_TX_* flag definitions.
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
static EFI_STATUS EFI_glink_tx
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options,
  glink_err_type    *ret  
)
{
  *ret = glink_tx(handle,
                  pkt_priv,
                  data,
                  size,
                  options);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
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
 * @param[in]   options    Flags specifying how transmission for this buffer 
 *                         would be handled. See GLINK_TX_* flag definitions.
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
static EFI_STATUS EFI_glink_txv
(
  glink_handle_type        handle,
  const void               *pkt_priv,
  void                     *iovec,
  size_t                   size,
  glink_buffer_provider_fn vprovider,
  glink_buffer_provider_fn pprovider,
  uint32                   options,
  glink_err_type           *ret  
)
{
  *ret = glink_txv(handle,
                   pkt_priv,
                   iovec,
                   size,
                   vprovider,
                   pprovider,
                   options);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
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
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  GLink XAL allocates rx buffers for receiving packets
 */
static EFI_STATUS EFI_glink_queue_rx_intent
(
  glink_handle_type handle,
  const void        *pkt_priv,
  size_t            size,
  glink_err_type    *ret  
)
{
  *ret = glink_queue_rx_intent(handle,
                               pkt_priv,
                               size);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Client uses this to signal to GLink layer that it is done with the received 
 * data buffer. This API should be called to free up the receive buffer, which,
 * in zero-copy mode is actually remote-side's transmit buffer.
 *
 * @param[in]   handle   GLink handle associated with the logical channel
 *
 * @param[in]   *ptr     Pointer to the received buffer
 *
 * @param[in]   reuse    Reuse intent
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  GLink XAL frees the Rx buffer
 */
static EFI_STATUS EFI_glink_rx_done
(
  glink_handle_type handle,
  const void        *ptr,
  boolean           reuse,
  glink_err_type    *ret  
)
{
  *ret = glink_rx_done(handle,
                       ptr,
                       reuse);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
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
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_glink_sigs_set
(
  glink_handle_type handle,
  uint32            sig_value,
  glink_err_type    *ret  
)
{
  *ret = glink_sigs_set(handle,
                        sig_value);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Get the local 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_glink_sigs_local_get
(
  glink_handle_type handle,
  uint32            *sig_value,
  glink_err_type    *ret  
)
{
  *ret = glink_sigs_local_get(handle,
                              sig_value);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * Get the remote 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_glink_sigs_remote_get
(
  glink_handle_type handle,
  uint32            *sig_value,
  glink_err_type    *ret  
)
{
  *ret = glink_sigs_remote_get(handle,
                               sig_value);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 * This API allows the RPM client to poll the transport for any new data
 * that might have come in. It would *usually* be used when incoming interrupts
 * are disabled.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_glink_rpm_rx_poll
(
  glink_handle_type handle,
  glink_err_type    *ret  
)
{
  *ret = glink_rpm_rx_poll(handle);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
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
 * @param[out]   *ret      Return code from the actual glink API                       
 *
 * @return       Standard EFI error codes
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_glink_rpm_mask_rx_interrupt
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct,
  glink_err_type    *ret
)
{
  *ret = glink_rpm_mask_rx_interrupt(handle,
                                     mask,
                                     platform_struct);
  
  if(*ret == GLINK_STATUS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_COMPROMISED_DATA;
  }
}

/** 
 *  Performs necessary connection teardown upon exit.
 *
 *  This function will tear down all commenctions.
 *
 *  @return
 *  EFI_SUCCESS -- Function completed successfully. \n
 *  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
 *  for additional possible returns.
 */
static EFI_STATUS EFI_glink_rpm_teardown
(
)
{
  glink_ssr_tear_down_rpm();
  return EFI_SUCCESS;
}

/*===========================================================================
                           INTERNAL DATA STRUCTURES
===========================================================================*/

static EFI_GLINK_PROTOCOL GLINKProtocol =
{
  EFI_GLINK_PROTOCOL_REVISION,
  EFI_glink_register_link_state_cb,
  EFI_glink_deregister_link_state_cb,
  EFI_glink_open,
  EFI_glink_close,
  EFI_glink_tx,
  EFI_glink_txv,
  EFI_glink_queue_rx_intent,
  EFI_glink_rx_done,
  EFI_glink_sigs_set,
  EFI_glink_sigs_local_get,
  EFI_glink_sigs_remote_get,
  EFI_glink_rpm_rx_poll,
  EFI_glink_rpm_mask_rx_interrupt,
  EFI_glink_rpm_teardown
};

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/**
 * Entry point for the GLink DXE driver.
 */
EFI_STATUS EFIAPI GLinkEntryPoint
(
  EFI_HANDLE            ImageHandle,
  EFI_SYSTEM_TABLE     *SystemTable
)
{
  EFI_HANDLE handle = NULL;
  EFI_STATUS status;

  /* TODO: initialize SMD drivers here */
  //smd_initialize_static();
  
  /* Init GLink and other xport libs */
  glink_init();
  //xport_rpm_config_init_toc_addr();
  //xport_rpm_init();
  //xport_smd_init();
  xport_smem_init();

  status = gBS->InstallMultipleProtocolInterfaces(&handle,
      &gEfiGLINKProtocolGuid, (void **)&GLINKProtocol, NULL, NULL, NULL );

  return status;
}
