/** @file

  Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/24/15   ai      Created
=============================================================================*/
#ifndef __AK_DXE_SERVICES_PORT_H__
#define __AK_DXE_SERVICES_PORT_H__

#include <Uefi.h>
#include <Library/MpIoMsgInfo.h>
#include <Pi/PiDxeCis.h>

#define DXESERVICESPORT      MPIO_PORT('D','x','e','T','a','b','l','e')

#define AK_DXESERVICES_GET_MEMSPACE_DESCRIPTOR     0x1
#define AK_DXESERVICES_SET_MEMSPACE_ATTRIBUTES     0x2

typedef struct {
  IN  EFI_PHYSICAL_ADDRESS             BaseAddress;
  OUT EFI_GCD_MEMORY_SPACE_DESCRIPTOR  *Descriptor;
} FN_GET_MEMSPACE_DESCRIPTOR_PARAMS;

typedef struct {
  IN EFI_PHYSICAL_ADDRESS         BaseAddress;
  IN UINT64                       Length;
  IN UINT64                       Attributes;
} FN_SET_MEMSPACE_ATTRIBUTES_PARAMS;

#endif // __AK_DXE_SERVICES_PORT_H__

