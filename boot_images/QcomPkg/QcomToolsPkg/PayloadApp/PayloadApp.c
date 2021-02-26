/** @file PayloadApp.c
*   
* Payload Deployment App for QCOM WOA Firmware Update.
*
* Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Copyright (c) 2011, ARM Limited. All rights reserved. 
* 
* This program and the accompanying materials 
* are licensed and made available under the terms and conditions of the BSD License 
* which accompanies this distribution. The full text of the license may be found at 
* http://opensource.org/licenses/bsd-license.php 
* 
* THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
* WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 
* 
**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/04/09     mic         updated the data size type to support 64-bit efi
 2012/07/02     Mic         moved from OSAppPkg to QComPkg
 2011/11/02     jthompso    Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/DebugLib.h>
#include <Uefi/UefiSpec.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/BaseMemoryLib.h>

extern EFI_GUID gQcomTokenSpaceGuid;

/* Define a section to force size and mydata to be at the end of the .data segment */
UINT32 size __attribute__((section("foo"))) =0xDEADBEEF;
UINT32 mydata[] __attribute__((section("foo"))) ={0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD};//,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD,0xFEEDBEAD};//,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77};


#define DEBUG_SPIN 0

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS retcode=EFI_SUCCESS;
  UINTN size_backup=size;
  EFI_STATUS Status=EFI_SUCCESS;
  VOID              *PayloadBuffer;
  EFI_BOOT_SERVICES *BootServices;
  EFI_RUNTIME_SERVICES *RuntimeServices;
  
  BootServices = SystemTable->BootServices;
  RuntimeServices = SystemTable->RuntimeServices;

  if(size==0xDEADBEEF) 
  {
    AsciiPrint("EFI file untouched. No FV file to extract.\n");
    return EFI_SUCCESS;
  }
  size_backup=size;
      
#if DEBUG_SPIN == 1
  UINTN i=0;
  while(i<0x3FFFFFFF)
  {
    i++;
  }  
#endif
    

  retcode = BootServices->AllocatePool(EfiBootServicesData,        
                                  size_backup,
                                  &PayloadBuffer);

  if(EFI_ERROR(retcode)) {
    AsciiPrint("ERROR: Error allocating space for payload.\n");
    return Status;
  }

  CopyMem(PayloadBuffer,mydata,size_backup);


  retcode = RuntimeServices->SetVariable(
                                  L"PayloadBuffer",
                                  &gQcomTokenSpaceGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                  sizeof(VOID*),
                                  &PayloadBuffer);

  if(EFI_ERROR(retcode)) 
  {
    AsciiPrint("ERROR: Error saving payloadbuffer variable.\n");
    return Status;
  }

  retcode = RuntimeServices->SetVariable(
                                  L"PayloadBufferSize",
                                  &gQcomTokenSpaceGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                  sizeof(UINTN),
                                  &size_backup);

  if(EFI_ERROR(retcode)) 
  {
    AsciiPrint("ERROR: Error saving payloadbuffersize variable.\n");
    return Status;
  }


  return EFI_SUCCESS;

}


