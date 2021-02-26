/** @file MemCpyTest.c
   
  MemCopy performance

  NOTE:
  EBL Only
  MemCopyTest u - runs tests on uncached buffer
  MemCopyTest - runs tests on cached buffer

  Copyright (c) 2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/5/2014  vk      Initial revision
=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/QcomLib.h>
#include <Library/ArmLib.h>

#define MEMCPY_BUFFER_SZ ((2 * 1024 * 1024))

VOID *
memcpy(
  VOID                      *DestinationBuff,
  CONST VOID                *SourceBuff,
  unsigned int              Length
  )
;

void a64_memcpy_8916( VOID *DestinationBuff,
                      CONST VOID *SourceBuff,
                      unsigned int Length
                      );

EFI_STATUS
EFIAPI
MemCpyTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  VOID *Src = NULL, *Dest = NULL;
  volatile UINT32 Start1 = 0, Stop1 = 0;
  volatile UINT32 Start2 = 0, Stop2 = 0;
  volatile UINT32 Start3 = 0, Stop3 = 0;
  UINT32 i,j = 0;
  UINTN Argc;
  CHAR8 **Argv;
  CHAR8* ArgStr = NULL;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  if (Status != EFI_SUCCESS)
    return EFI_INVALID_PARAMETER;

  if (Argc)
    ArgStr = Argv[0];

  if (ArgStr == NULL)
  {
    AsciiPrint ("Source Cached Allocation  ...\n");
    AsciiPrint ("Dest Cached Allocation  ...\n");
    Src = AllocatePool (MEMCPY_BUFFER_SZ);
    Dest = AllocatePool (MEMCPY_BUFFER_SZ);
  }
  else if (AsciiStriCmp ((CHAR8*)ArgStr, "u") == 0)
  {
    AsciiPrint ("Source Uncached Allocation  ...\n");
    AsciiPrint ("Dest Uncached Allocation  ...\n");
    Src = UncachedAllocatePool (MEMCPY_BUFFER_SZ);
    Dest = UncachedAllocatePool (MEMCPY_BUFFER_SZ);
  }


  if (Src == NULL)
    goto ExitErr;

  if (Dest == NULL)
    goto ExitErr;

  ArmDisableInterrupts();
  //ArmEnableAlignmentCheck();

  #define OFFSET_COUNT 16

  for (j = 0; j < OFFSET_COUNT; j++)
  for (i = 0; i < OFFSET_COUNT; i++)
  {
    /*  ArmPkg C MemCopy  */
    SetMem (Src, MEMCPY_BUFFER_SZ, 0x55);
    SetMem (Dest, MEMCPY_BUFFER_SZ, 0xDD);

    Start1 = GetTimerCountms();
    CopyMem (Dest+i, Src+j, MEMCPY_BUFFER_SZ/2-i);
    Stop1 = GetTimerCountms();

    if (CompareMem (Dest+i, Src+j, MEMCPY_BUFFER_SZ/2 - i) != 0)
       goto ExitErr;

    /*  ArmPkg C MemCopy  */
    SetMem (Src, MEMCPY_BUFFER_SZ, 0x55);
    SetMem (Dest, MEMCPY_BUFFER_SZ, 0xDD);

    Start2 = GetTimerCountms();
    memcpy (Dest+i, Src+i, MEMCPY_BUFFER_SZ/2 - i);
    Stop2 = GetTimerCountms();

    if (CompareMem (Dest+i, Src+j, MEMCPY_BUFFER_SZ/2 - i) != 0)
       goto ExitErr;

    /*  Qcom Optimized MemCopy  */
    SetMem (Src, MEMCPY_BUFFER_SZ, 0x55);
    SetMem (Dest, MEMCPY_BUFFER_SZ, 0xDD);

    Start3 = GetTimerCountms();
    a64_memcpy_8916 (Dest+i, Src+i, MEMCPY_BUFFER_SZ/2 - i);
    Stop3 = GetTimerCountms();
        
    AsciiPrint ("Sz %d Dest 0x%lx Src 0x%lx Time (mS)| C (edk) %4d | A (edk) %4d | A (QMem) %4d |\n", MEMCPY_BUFFER_SZ/2-i, (UINT64)Dest+i, (UINT64)Src+i, (Stop1 - Start1), (Stop2 - Start2), (Stop3 - Start3));
    if (CompareMem (Dest+i, Src+i, MEMCPY_BUFFER_SZ/2 - i) != 0)
       goto ExitErr;
  }

  ArmEnableInterrupts();

  return EFI_SUCCESS;

ExitErr:
  ArmEnableInterrupts();
  AsciiPrint ("Verification failed ...\n");
  if (Src != NULL)
    FreePool (Src);

  if (Dest != NULL)
    FreePool (Dest);

  return EFI_LOAD_ERROR;
}