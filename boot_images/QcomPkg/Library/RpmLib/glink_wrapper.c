/** @file glink_wrapper.c
   
  Map glink api to GLINK Protocol 

  Copyright (c) 2015 Qualcomm Technologies Incorporated.

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/26/15   chandan Move to using Glink protocol
02/03/15   rli     Initial Version.
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIGLink.h>

EFI_GLINK_PROTOCOL * glink_protocol = NULL;

glink_err_type glink_rpm_rx_poll(glink_handle_type handle)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkRpmRxPoll(handle, &ret);
    
    return ret;
}

glink_err_type glink_rpm_mask_rx_interrupt
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct
)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkRpmMaskRxInterrupt(handle, mask, platform_struct, &ret);
    
    return ret;
}

glink_err_type glink_sigs_set
(
  glink_handle_type handle,
  uint32            sig_value
)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkSigsSet(handle, sig_value, &ret);
    
    return ret;
}

glink_err_type glink_tx
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options
)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkTx(handle, pkt_priv, data, size, options, &ret);
    
    return ret;
}

glink_err_type glink_open
(
  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle
)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkOpen(cfg_ptr, handle, &ret);
    
    return ret;
}

glink_err_type glink_register_link_state_cb
(
  glink_link_id_type *link_id,
  void* priv
)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkRegisterLinkStateCB(link_id, priv, &ret);
    
    return ret;
}

glink_err_type glink_close(glink_handle_type handle)
{
    glink_err_type  ret = GLINK_STATUS_FAILURE;

    if (glink_protocol == NULL) 
    {
        EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiGLINKProtocolGuid, NULL, (void**)&glink_protocol);
        if(status != EFI_SUCCESS)
        {
           return ret;
        }
    }
    
    glink_protocol->GlinkClose(handle, &ret);
    
    return ret;
}
