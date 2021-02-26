/** @file smem_wrapper.c
   
  Map smem api in common code to SMEM DXE interface

  Copyright (c) 2013-2014, Qualcomm Technologies, Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/25/14   bm      64bit support
02/14/13   bm      Initial Version.
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include "Protocol/EFISmem.h"
#include "comdef.h"
#include "smem.h"

void *smem_alloc
(
  smem_mem_type         smem_type,
  uint32                buf_size
)
{
  EFI_SMEM_PROTOCOL * smem_protocol;
  EFI_STATUS status;
  EFI_GUID smemGuid = EFI_SMEM_PROTOCOL_GUID;
  void* addr;

  status = gBS->LocateProtocol(&smemGuid, NULL,
      (void**)&smem_protocol);
  if(status != EFI_SUCCESS)
  {
    return NULL;
  }

  smem_protocol->SmemAlloc(smem_type, buf_size, &addr);

  return addr;
}

void *smem_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
)
{
  EFI_SMEM_PROTOCOL * smem_protocol;
  EFI_STATUS status;
  EFI_GUID smemGuid = EFI_SMEM_PROTOCOL_GUID;  
  void* addr;

  status = gBS->LocateProtocol(&smemGuid, NULL,
      (void**)&smem_protocol);
  if(status != EFI_SUCCESS)
  {
    return NULL;
  }

  smem_protocol->SmemGetAddr(smem_type, (UINT32*)buf_size, &addr);
  return addr;
}

void smem_init(void)
{
  /** Assume SMEM has already been initialized in UEFI */
}

boolean smem_version_set
(
 smem_mem_type type,
 uint32 version,
 uint32 mask
)
{
  /** Nothing to do */
  return TRUE;
}

void smem_spin_lock(uint32 spinlock_id)
{
  /** No locks in UEFI */
}

void smem_spin_unlock(uint32 spinlock_id)
{
  /** No locks in UEFI */
}

void smd_event_start_loopback(void)
{
  /* Do nothing */
}

