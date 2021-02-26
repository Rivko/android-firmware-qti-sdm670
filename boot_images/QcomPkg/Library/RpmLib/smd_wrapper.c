/** @file smd_wrapper.c
   
  Map smd api in common code to SMD DXE interface

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/24/13   rli     Added smdl_close wrapper
04/15/13   rli     Removed redundant stall in smd_is_port_open.
03/21/13   rli     Initial Version.
=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFISmd.h>

boolean smd_is_port_open(const char *name,  smd_channel_type  channel_type )
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    uint32 isRpmPortOpen = 0;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlIsPortOpen(name, channel_type, &isRpmPortOpen);
        }
    return (isRpmPortOpen > 0);
}

smdl_handle_type smdl_open(const char *name,  smd_channel_type edge, uint32 flags, uint32 fsize, smdl_callback_t notify, void *cb_data)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    smdl_handle_type handle = NULL;

        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlOpen(name, edge, flags, fsize, notify, cb_data, &handle);
        }
    return handle;    
}

int32 smdl_sig_set(smdl_handle_type port,  smdl_sig_type    sig,  boolean value)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = -1;
    uint32 *ret_ptr = (uint32 *)&ret;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlSigSet(port,sig,value, ret_ptr);
        }
   return ret;
}

int32 smdl_sig_get(smdl_handle_type port,  smdl_sig_type sig)
{

    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = -1;
    uint32 *ret_ptr = (uint32 *)&ret;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
           smd_protocol->SmdlSigGet(port,sig, ret_ptr);
        }
   return ret;
}

int32 smdl_tx_peek(smdl_handle_type port)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = 0;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
           smd_protocol->SmdlTxPeek(port, &ret);
        }
   return ret;
}

int32 smdl_rx_peek(smdl_handle_type port)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = 0;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlRxPeek(port,&ret);
        }
   return ret;
}
int32 smdl_read(smdl_handle_type port, uint32 len, void *buffer, uint32 flags)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = 0;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
           smd_protocol->SmdlRead(port, len, buffer, flags, &ret);
        }
   return ret;
}

int32 smdl_writev( smdl_handle_type  port,  const smdl_iovec_type *iovec,  uint32 flags)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = -1;
    uint32 *ret_ptr = (uint32 *)&ret;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlWritev(port, iovec, flags, ret_ptr);
        }
  return ret;
}
int32 smdl_close( smdl_handle_type port)
{
    EFI_SMD_PROTOCOL * smd_protocol;
    EFI_STATUS status;
    int32 ret = -1;
        status = gBS->LocateProtocol(&gEfiSMDProtocolGuid, NULL, (void**)&smd_protocol);
        if(status == EFI_SUCCESS)
        {
            smd_protocol->SmdlClose(port, &ret);
        }
  return ret;
}
