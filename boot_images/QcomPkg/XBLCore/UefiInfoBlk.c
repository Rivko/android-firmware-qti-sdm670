/** @file UefiInfoBlk.c
  UEFI Info blocking holding helpful informational and statistics data.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved. 

  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/03/14    nk     Changed dump table address to static address 
 10/23/13   niting  Updated location of UefiInfoBlock
 10/22/13    vk     GCC compliant aligned macro
 03/19/13    vk     Move InfoBlk header to common
 02/01/13    vk     Add config table
 12/01/12    yg     Created
 
=============================================================================*/

#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Include/UefiInfoBlk.h>
#include <Library/DBIDump.h>

UefiInfoBlkType *UefiInfoBlockPtr = NULL;

extern UINTN _UefiInfoBlockPtr;
extern CHAR8 _UefiRelInfoString;
extern EFI_GUID gEfiInfoBlkHobGuid;

/* Initiailizes the UEFI Info Block */
UefiInfoBlkType*
InitInfoBlock (UINTN Address)
{
  VOID* Destination;
  if ((Address != 0) && (UefiInfoBlockPtr == NULL))
  {
    /* Update info block pointer to new location */
    UefiInfoBlockPtr = (UefiInfoBlkType *)Address;

    ZeroMem (UefiInfoBlockPtr, sizeof (UefiInfoBlkType));

    /* Do minimal initialize, rest of will be taken care of
     * by particular code that's owning the information */
    UefiInfoBlockPtr->Signature = UEFI_INFO_BLOCK_SIGNATURE;
    UefiInfoBlockPtr->StructVersion = UEFI_INFO_BLOCK_VERSION;

    /* Copy release info immediately after UEFI Info Block */
    Destination = (VOID*)((UINTN)UefiInfoBlockPtr + sizeof(UefiInfoBlkType));
    AsciiStrCpy(Destination, &_UefiRelInfoString);
    UefiInfoBlockPtr->RelInfo = Destination;

    _UefiInfoBlockPtr = (UINTN)UefiInfoBlockPtr;

  }

  return UefiInfoBlockPtr;
}

UefiInfoBlkType*
GetInfoBlock (VOID)
{
  /* If this API is published as lib then this could query from
   * HOB's for the location if installed as HOB's */
  return UefiInfoBlockPtr;
}

/* Add hob for sharing InfoBlock with Dxe */
VOID*
AddInfoBlkHob (VOID)
{
  VOID* InfoBlkPtr;
  InfoBlkPtr = (VOID*)UefiInfoBlockPtr;
  return BuildGuidDataHob (&gEfiInfoBlkHobGuid, &InfoBlkPtr, sizeof(InfoBlkPtr));
}

