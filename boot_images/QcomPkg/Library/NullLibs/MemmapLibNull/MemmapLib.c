/** @file MemmapLib.c
   
  Stubs 

  Copyright (c) 2013, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/16/13   jz      Created stubs

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Protocol/EFIMemoryMap.h>


EFI_STATUS
EFIAPI
MemDumpInit(
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  Returns the memory map table with the data specified in PCDs
  defined in target specific .dsc file.
  
  @param Size            Pointer to the size in bytes of the MemoryMap buffer
  @param MemoryMap       Ponter to the buffer for subsystem memory map, which is
                         an array of EFI_SUBSYSTEM_MEMORY_DESCRIPTOR
  @param DescriptorCount Pointer to the location containing the descriptor count
  
  @retval RETURN_SUCCESS       The map was initialized.
  @retval RETURN_DEVICE_ERROR  EFI_INVALID_PARAMETER The parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL The supplied MemoryMap buffer was too small.
                               The current buffer size needed to hold the
                               memory map is returned in Size.
**/
EFI_STATUS
EFIAPI
GetSubsysMemMap(
  IN OUT UINTN                           *Size,
  IN OUT EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *MemoryMap,
  OUT UINTN                              *DescriptorCount
  )
{
  return RETURN_SUCCESS;
}
