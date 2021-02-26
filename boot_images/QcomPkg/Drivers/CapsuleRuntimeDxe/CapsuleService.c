/** @file
  Capsule Runtime Driver produces two UEFI capsule runtime services.
  (UpdateCapsule, QueryCapsuleCapabilities)
  It installs the Capsule Architectural Protocol defined in PI1.0a to signify 
  the capsule runtime services are ready.
 
  Copyright (c) 2012-2016, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  Portions Copyright (c) 2006 - 2010, Intel Corporation. All 
rights reserved.<BR> This program and the accompanying materials 
are licensed and made available under the terms and conditions 
of the BSD License which accompanies this distribution.  The 
full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php 

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/18/16   bh      Update to use offset from UEFI FD
 08/04/15   jb      Open capsule in read write to allow delete
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 06/12/15   jb      Only clear BootNext if it was used to process capsules
 03/11/15   vk      Return valid status
 03/05/15   jb      Add FMP support
 11/30/14   bh      fix 64-bit compiler errors
 10/28/13   niting  Skip hard reset when doing an UpdateCapsule at runtime
 10/09/13   niting  Removed PCDs
 02/19/13   vk      Cleanup warning
 02/12/13   niting  Create flush protocol to ensure data gets written before
                    write protection is enabled
 07/26/12   niting  Reinitialize CapsuleGuidTable if age has exceeded threshold
 07/10/12   niting  Updated logging
 06/27/12   jz      Fix KW warning
 04/24/12   yg      Log messages cleanup
 03/13/12   niting  Added firmware update support.
 03/01/12   jdt     ScatterGatherHeader conversion to Runtime-accessible memory moved to SEC.
 02/06/12   jdt     Initial revision.

=============================================================================*/
#include <Uefi.h>

#include <Protocol/Capsule.h>
#include <Protocol/EFICapsule.h>
#include <Guid/CapsuleVendor.h>
#include <Guid/FmpCapsule.h>
#include <Guid/FileInfo.h>
#include <Guid/GlobalVariable.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/CapsuleLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/SortLib.h>
#include <Library/HobLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/SimpleFileSystem.h>

#include <Guid/EventGroup.h>

#include "CapsuleService.h"
#include "CommonHeader.h"
//
// Handle for the installation of Capsule Architecture Protocol.
//
EFI_HANDLE  mNewHandle = NULL;

//
// The times of calling UpdateCapsule ()
//
UINTN       mTimes      = 0;

// Keeps track of whether to support a threshold for age of CapsuleGuidTable
#define MAX_CAPSULE_AGE_DEFAULT 5
UINT32 MaxCapsuleAge = MAX_CAPSULE_AGE_DEFAULT;

STATIC EFI_EVENT mVirtualAddressChangeEvent = NULL;
extern EFI_GUID gBdsEventGroupGuid;
STATIC EFI_EVENT mDelayedCapsuleProcessEvent = NULL;

// RAM based Scatter Gather List
STATIC SCATTERGATHERLIST_ARRAY_HEADER *mScatterGatherHeader = NULL;
UINT32 mScatterGatherHeaderSize = 0;
STATIC EFI_PHYSICAL_ADDRESS *mScatterGatherArray = NULL;

#define MAX_FILE_NAME_SIZE 1024
#define FILE_COUNT_STEP 5

/**
  Updates and flushes the capsule partition to nonvolatile memory.
  
  @param[in]  This   Pointer to the EFI_CAPSULE_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.

**/
EFI_STATUS
EFIAPI
FlushCapsulePartition (
  IN     EFI_CAPSULE_PROTOCOL    *This
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  CAPSULE_GUID_TABLE *CapsuleGuidTable = NULL; 

  UINT8   MemLoc[CAPSULE_UPDATE_TABLE_BLOCK_SIZE];
  UINT32  ImgSize;
  BOOLEAN TableUpdated = FALSE;                

  do {
    // Exit if partition does not exist
    if (CapsuleBlkIo == NULL)
    {
      break;
    }

    if (EfiAtRuntime())
    {
      Status = EFI_UNSUPPORTED;
      break;
    }

    // Get Capsule Guid Table
    ImgSize = CapsuleBlkIo->Media->BlockSize;
    if (ImgSize != CAPSULE_UPDATE_TABLE_BLOCK_SIZE)
    {
      // Block sizes must match
      break;
    }
    Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                       CapsuleBlkIo->Media->MediaId, 
                                       0, 
                                       ImgSize, 
                                       MemLoc);
    if (EFI_ERROR (Status))
    {  
      break;
    }
  
    CapsuleGuidTable = (CAPSULE_GUID_TABLE*)MemLoc;
  
    // Calculate CRC on table, if doesn't match then do not update age
    Status = ValidateCapsuleGuidTable(CapsuleGuidTable);
    if (Status != EFI_SUCCESS)
    {
      break;
    }
  
    // Update Capsule age only if any Guids are present
    if (CapsuleGuidTable->GuidCount > 0)
    {
      CapsuleGuidTable->Age++;
      TableUpdated = TRUE;
    }
  
    // Clear CapsuleGuidTable if age is too large and MaxCapsuleAge is
    // not zero (i.e. a value of 0 disables the check)
    if ((MaxCapsuleAge > 0) && (CapsuleGuidTable->Age > MaxCapsuleAge))
    {
      DEBUG((EFI_D_WARN,"Capsules in UpdateCapsule partition have exceeded "
                        "maximum age and have not been processed.\nClearing "
                        "out entire partition and deleting all capsules.\n"));

      // Reinitialize partition
      CapsuleGuidTable->Signature = CAPSULE_UPDATE_TABLE_SIGNATURE;
      CapsuleGuidTable->Revision = CAPSULE_UPDATE_TABLE_REVISION;
      CapsuleGuidTable->GuidCount = 0; 
      CapsuleGuidTable->BlockSize = CAPSULE_UPDATE_TABLE_BLOCK_SIZE;
      CapsuleGuidTable->BlockCount = CapsuleBlkIo->Media->LastBlock;
      CapsuleGuidTable->FreeBlocks = CapsuleBlkIo->Media->LastBlock - 1;
      CapsuleGuidTable->Age = 0;
      CapsuleGuidTable->Reserved[0] = 0;
      CapsuleGuidTable->Reserved[1] = 0;
      CapsuleGuidTable->Reserved[2] = 0;

      // Clear out all Guid entries
      SetMem(&(CapsuleGuidTable->CapsuleGuidEntries[0]), 
             sizeof(CapsuleGuidTable->CapsuleGuidEntries),
             0x0);
      TableUpdated = TRUE;
    }

    // Only write back table if it has been updated
    if (TableUpdated == TRUE)
    {
      CapsuleGuidTable->Crc32 = 0;
      Status = CalculateCrc32(CapsuleGuidTable, 
                              sizeof(CAPSULE_GUID_TABLE), 
                              &(CapsuleGuidTable->Crc32));
      if (EFI_ERROR (Status))
        break;
    
      // Copy back Capsule Guid Table to persisted storage
      ImgSize = CapsuleBlkIo->Media->BlockSize;
      Status = CapsuleBlkIo->WriteBlocks (CapsuleBlkIo, 
                                          CapsuleBlkIo->Media->MediaId, 
                                          0, 
                                          ImgSize, 
                                          MemLoc);
      if (EFI_ERROR (Status))
        break;
    }

  } while (0);

  return Status;
}

STATIC EFI_CAPSULE_PROTOCOL gCapsuleProtocol = {
  EFI_CAPSULE_PROTOCOL_REVISION,
  FlushCapsulePartition
};

/**
 * 
 * Check if the ScatterGatherHeader
 *  was written by a previous call
 *  to UpdateCapsule by running
 *  a CRC32 check.
 * 
 */
EFI_STATUS
EFIAPI
ValidateSGHeader(
  IN SCATTERGATHERLIST_ARRAY_HEADER *ScatterGatherArrayHeader
  )
{
  EFI_STATUS Status;

  UINT32 SavedCrc32;
  
  if(ScatterGatherArrayHeader == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  SavedCrc32 = ScatterGatherArrayHeader->Crc32;
  ScatterGatherArrayHeader->Crc32 = 0x0;

  Status = CalculateCrc32(ScatterGatherArrayHeader,
                          sizeof(SCATTERGATHERLIST_ARRAY_HEADER),
                          &ScatterGatherArrayHeader->Crc32);

  if(EFI_ERROR(Status))
  {
    goto DoneValidating;
  }

  if(SavedCrc32 != ScatterGatherArrayHeader->Crc32)
  {
    Status = EFI_CRC_ERROR;
    goto DoneValidating;
  }

  //See if there are any ScatterGatherLists to process
  // and Make sure the ScatterGather array offset is valid.
  if(ScatterGatherArrayHeader->PointerCount < 1 ||
     ScatterGatherArrayHeader->ArrayOffset < sizeof(SCATTERGATHERLIST_ARRAY_HEADER)) 
  {
    Status = EFI_UNSUPPORTED;
  }

DoneValidating:

  //Restore SavedCrc32.
  ScatterGatherArrayHeader->Crc32 = SavedCrc32;
  return Status;
}


/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
VirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer(0, (VOID**)&mScatterGatherHeader);
}

/**
 * Check to see if previous 
 *  OS called UpdateCapsule
 *  to persist a capsule across
 *  reset.
 * 
 * 
 * @return VOID 
 */
VOID CheckScatterGatherListArray(
  IN SCATTERGATHERLIST_ARRAY_HEADER *ScatterGatherArrayHeader,
  IN BOOLEAN                        PersistCapsule                         
  )
{
  EFI_PHYSICAL_ADDRESS *ScatterGatherArray;

  EFI_STATUS Status;

  VOID *MemBase = NULL;
  UINTN MemSize = 0;

  DEBUG((EFI_D_INFO,"CapsuleRuntimeDxe: +CheckScatterGatherListArray\n"));

  if(ScatterGatherArrayHeader!= NULL)
  {

    if(!EFI_ERROR(ValidateSGHeader(ScatterGatherArrayHeader)))
    {     

      ScatterGatherArray = 
        (EFI_PHYSICAL_ADDRESS*)((UINT8*)ScatterGatherArrayHeader +
                                ScatterGatherArrayHeader->ArrayOffset);


      //First find out the size of the capsules
      // we have been passed.
      Status = CapsuleDataCoalesce(NULL,
                                   ScatterGatherArray,
                                   &MemBase,
                                   &MemSize);

      if(Status == EFI_BUFFER_TOO_SMALL) 
      {
        // Allocate Memory for coalescing the capsules.
        if (PersistCapsule)
        {
          Status = gBS->AllocatePool(EfiBootServicesData,
                                     MemSize,
                                     (VOID**)&MemBase);
        }
        else
        {
          Status = gBS->AllocatePool(EfiRuntimeServicesData,
                                     MemSize,
                                     (VOID**)&MemBase);
        }

        if(!EFI_ERROR(Status)) 
        {

          DEBUG((EFI_D_INFO,"ScatterGatherList is 0x%llx, preparing to coalesce capsules.\n",
                 (UINT64)*ScatterGatherArray));

          Status = CapsuleDataCoalesce(NULL,
                                       ScatterGatherArray,
                                       &MemBase,
                                       &MemSize);
  
          if(EFI_ERROR(Status))
          {
            DEBUG((EFI_D_ERROR,"Could not coalesce capsule, Status: %r\n",Status));
            goto InitializeHeader;
          }       
         
          // Process coalesced capsules
          if (PersistCapsule)
          {
            Status = PersistCoalescedCapsules((VOID*)MemBase);
          }
          else
          {
            Status = ProcessCoalescedCapsules((VOID*)MemBase);
          }

          if(EFI_ERROR(Status))
          {
            DEBUG((EFI_D_ERROR,"Could not process coalesced capsule(s), Status: %r",Status));
            goto InitializeHeader;
          }   

          DEBUG((EFI_D_INFO,"Capsules processsed successfully!\n"));

        }
        else
        {
          DEBUG((EFI_D_ERROR,"Could not allocate memory for capsule coalescing, Status: %r",Status));          
        }
  
      }
          
    }
    else
    {
      DEBUG((EFI_D_INFO,"ScatterGather header CRC failed. Will not process capsules in RAM.\n"));
    }    

InitializeHeader:
    //Done with the ScatterGatherListArray, re-initialize the header.
    DEBUG((EFI_D_INFO,"CapsuleRuntimeDxe: Initializing ScatterGather header.\n"));
    ScatterGatherArrayHeader->Revision = SCATTERGATHERLIST_ARRAY_HEADER_REVISION;
    ScatterGatherArrayHeader->PointerCount = 0x0;
    ScatterGatherArrayHeader->ArrayOffset = sizeof(SCATTERGATHERLIST_ARRAY_HEADER);
  
    CopyMem(&(ScatterGatherArrayHeader->ScatterGatherHeaderGuid),
            (VOID*)&gQcomScatterGatherListHeaderGuid, 
            sizeof(EFI_GUID));

    ScatterGatherArrayHeader->Crc32 = 0x0;
    Status = CalculateCrc32(ScatterGatherArrayHeader,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER),
                            &ScatterGatherArrayHeader->Crc32);

    mScatterGatherArray = NULL;
       
  }

  DEBUG((EFI_D_INFO,"CapsuleRuntimeDxe: -CheckScatterGatherListArray\n"));

  return; 
}

/**
  Passes capsules to the firmware with both virtual and physical mapping. Depending on the intended
  consumption, the firmware may process the capsule immediately. If the payload should persist
  across a system reset, the reset value returned from EFI_QueryCapsuleCapabilities must
  be passed into ResetSystem() and will cause the capsule to be processed by the firmware as
  part of the reset process.

  @param  CapsuleHeaderArray    Virtual pointer to an array of virtual pointers to the capsules
                                being passed into update capsule.
  @param  CapsuleCount          Number of pointers to EFI_CAPSULE_HEADER in
                                CaspuleHeaderArray.
  @param  ScatterGatherList     Physical pointer to a set of
                                EFI_CAPSULE_BLOCK_DESCRIPTOR that describes the
                                location in physical memory of a set of capsules.

  @retval EFI_SUCCESS           Valid capsule was passed. If
                                CAPSULE_FLAGS_PERSIT_ACROSS_RESET is not set, the
                                capsule has been successfully processed by the firmware.
  @retval EFI_DEVICE_ERROR      The capsule update was started, but failed due to a device error.
  @retval EFI_INVALID_PARAMETER CapsuleSize is NULL, or an incompatible set of flags were
                                set in the capsule header.
  @retval EFI_INVALID_PARAMETER CapsuleCount is Zero.
  @retval EFI_INVALID_PARAMETER For across reset capsule image, ScatterGatherList is NULL.
  @retval EFI_UNSUPPORTED       CapsuleImage is not recognized by the firmware.

**/
EFI_STATUS
EFIAPI
UpdateCapsule (
  IN EFI_CAPSULE_HEADER      **CapsuleHeaderArray,
  IN UINTN                   CapsuleCount,
  IN EFI_PHYSICAL_ADDRESS    ScatterGatherList OPTIONAL
  )
{
  UINTN                     ArrayNumber;
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_CAPSULE_HEADER        *CapsuleHeader;
  BOOLEAN                   NeedReset;
  BOOLEAN                   InitiateReset;  
  BOOLEAN                   InitiateCapsuleReset;
  UINT32                    SavedCrc32;
  
  //
  // Capsule Count can't be less than one.
  //
  if (CapsuleCount < 1) {
    return EFI_INVALID_PARAMETER;
  }

  NeedReset         = FALSE;
  InitiateReset     = FALSE;
  InitiateCapsuleReset = FALSE;
  CapsuleHeader     = NULL;
  

  for (ArrayNumber = 0; ArrayNumber < CapsuleCount; ArrayNumber++) {
    //
    // A capsule which has the CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE)) == CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // A capsule which has the CAPSULE_FLAGS_INITIATE_RESET flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET)) == CAPSULE_FLAGS_INITIATE_RESET) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // Check FMP capsule flag 
    //
    if (CompareGuid(&CapsuleHeader->CapsuleGuid, &gEfiFmpCapsuleGuid)
     && (CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0 ) {
       return EFI_INVALID_PARAMETER;
    }
    //
    // Check Capsule image without populate flag by firmware support capsule function  
    //
    if (((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) && 
          (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS)) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // Walk through all capsules, record whether there is a capsule needs reset
  // or initiate reset. And then process capsules which has no reset flag directly.
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount ; ArrayNumber++) {
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    //
    // Here should be in the boot-time for non-reset capsule image
    // Platform specific update for the non-reset capsule image.
    //
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) == 0) {
      if (EfiAtRuntime ()) { 
        // At runtime, processing of capsules that don't have PERSIST_ACROSS_RESET
        // is not supported
        Status = EFI_UNSUPPORTED;
      } else {
        // Processing of capsules that don't have CAPSULE_FLAGS_PERSIST_ACROSS_RESET
        // is only supported at boot time - currently do nothing
        Status = ProcessCapsuleImageEx(CapsuleHeader, 0, NULL);
      }
      if (EFI_ERROR(Status)) {
        return Status;
      }
    } else {
      NeedReset = TRUE;
      if ((CapsuleHeader->Flags & CAPSULE_FLAGS_INITIATE_RESET) != 0) {
        InitiateReset = TRUE;
      }
    }
  }
  
  //
  // After launching all capsules who has no reset flag, if no more capsules claims
  // for a system reset just return.
  //
  if (!NeedReset) {
    return EFI_SUCCESS;
  }

  //
  // ScatterGatherList is only referenced if the capsules are defined to persist across
  // system reset. 
  //
  if (ScatterGatherList == (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check if the platform supports update capsule across a system reset
  //
  if (!FeaturePcdGet(PcdSupportUpdateCapsuleReset)) {
    return EFI_UNSUPPORTED;
  }

  if ((!EfiAtRuntime()) && UpdateCapsulePartitionExists())
  {
    // If at boot time and capsule partition exists, 
    // then coalesce the capsule and persist capsule in storage

    // Create a temporary SCATTERGATHERLIST_ARRAY_HEADER
    SCATTERGATHERLIST_ARRAY_HEADER *SGArrayHeader = NULL;
    EFI_PHYSICAL_ADDRESS *ScatterArray = NULL;

    // Allocate SCATTERGATHERLIST_ARRAY_HEADER for one array pointer and NULL pointer;
    UINT32 MemSize = sizeof(SCATTERGATHERLIST_ARRAY_HEADER) + 
                     2*sizeof(EFI_PHYSICAL_ADDRESS);
    Status = gBS->AllocatePool(EfiBootServicesData,
                               MemSize,
                               (VOID**)&SGArrayHeader);

    SGArrayHeader->Revision = SCATTERGATHERLIST_ARRAY_HEADER_REVISION;
    SGArrayHeader->ArrayOffset = sizeof(SCATTERGATHERLIST_ARRAY_HEADER);
    SGArrayHeader->PointerCount = 0;

    CopyMem(&(SGArrayHeader->ScatterGatherHeaderGuid),
            (VOID*)&gQcomScatterGatherListHeaderGuid, 
            sizeof(EFI_GUID));

    ScatterArray = (EFI_PHYSICAL_ADDRESS*)((UINT8*)SGArrayHeader +                           
                    SGArrayHeader->ArrayOffset);
    ScatterArray[SGArrayHeader->PointerCount] = ScatterGatherList;
    SGArrayHeader->PointerCount++;
    ScatterArray[SGArrayHeader->PointerCount] = (UINT64)NULL;
    SGArrayHeader->Crc32 = 0x0;
    Status = CalculateCrc32(SGArrayHeader,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER),
                            &SGArrayHeader->Crc32);

    // Coalesce and persist capsules
    CheckScatterGatherListArray(SGArrayHeader, TRUE);
  }
  else
  {
    // If at runtime or Update Capsule partition does not exist,
    // store address of ScatterGatherList in RAM version
    
    // If the ScatterGatherHeader is not available, return.
    if(mScatterGatherHeader == NULL) 
    {
      return EFI_UNSUPPORTED;
    }
  
    //Check CRC
    SavedCrc32 = mScatterGatherHeader->Crc32;
    mScatterGatherHeader->Crc32 = 0x0;
  
    Status = CalculateCrc32(mScatterGatherHeader,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER),
                            &mScatterGatherHeader->Crc32);
      
    if(EFI_ERROR(Status))     
    {
      //Restore SavedCrc
      mScatterGatherHeader->Crc32 = SavedCrc32;
      return Status;
    }
  
    if(SavedCrc32 != mScatterGatherHeader->Crc32) 
    {
      //Restore SavedCrc
      mScatterGatherHeader->Crc32 = SavedCrc32;
      return EFI_CRC_ERROR;
    }
  
    mScatterGatherHeader->Crc32 = SavedCrc32;
  
    //
    // ScatterGatherList is only referenced if the capsules are defined to persist across
    // system reset. Add this ScatterGatherList pointer to the array
    // of ScatterGatherList pointers in memory and let pre-boot driver to pick it up 
    // after coming through a system reset.
    //
    if(mScatterGatherArray == NULL)
    {
      mScatterGatherArray = 
        (EFI_PHYSICAL_ADDRESS*)((UINT8*)mScatterGatherHeader +                           
                                mScatterGatherHeader->ArrayOffset);
            
    }
    
    // Check to ensure overflow does not occur - account for NULL pointer at end
    if ((mScatterGatherHeader->PointerCount+1)*sizeof(EFI_PHYSICAL_ADDRESS) >= 
        mScatterGatherHeaderSize - sizeof(SCATTERGATHERLIST_ARRAY_HEADER))
    {
      return EFI_OUT_OF_RESOURCES;
    }
  
    mScatterGatherArray[mScatterGatherHeader->PointerCount] = ScatterGatherList;
    mScatterGatherHeader->PointerCount++;
  
    //Ensure that last entry in ScatterGatherArray is NULL.
    mScatterGatherArray[mScatterGatherHeader->PointerCount] = (UINT64)NULL;
  
    //Calculate new Header CRC
    mScatterGatherHeader->Crc32 = 0x0;
  
    Status = CalculateCrc32(mScatterGatherHeader,
                            sizeof(SCATTERGATHERLIST_ARRAY_HEADER),
                            &mScatterGatherHeader->Crc32);
    InitiateCapsuleReset = TRUE;
  }

  //
  // Variable has been set successfully, increase variable index.
  //
  mTimes++;
  if(InitiateReset) {
    //
    // Firmware that encounters a capsule which has the CAPSULE_FLAGS_INITIATE_RESET Flag set in its header
    // will initiate a reset of the platform which is compatible with the passed-in capsule request and will 
    // not return back to the caller.
    //
    if (InitiateCapsuleReset)
    {
      CHAR16 *CapsuleResetStr = CAPSULE_RESET_STRING;
      EfiResetSystem (EfiResetWarm, EFI_SUCCESS, StrSize(CAPSULE_RESET_STRING), CapsuleResetStr);
    }
    else
    {
      EfiResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
  }

  return Status;
}

/**
  Returns if the capsule can be supported via UpdateCapsule().

  @param  CapsuleHeaderArray    Virtual pointer to an array of virtual pointers to the capsules
                                being passed into update capsule.
  @param  CapsuleCount          Number of pointers to EFI_CAPSULE_HEADER in
                                CaspuleHeaderArray.
  @param  MaxiumCapsuleSize     On output the maximum size that UpdateCapsule() can
                                support as an argument to UpdateCapsule() via
                                CapsuleHeaderArray and ScatterGatherList.
  @param  ResetType             Returns the type of reset required for the capsule update.

  @retval EFI_SUCCESS           Valid answer returned.
  @retval EFI_UNSUPPORTED       The capsule image is not supported on this platform, and
                                MaximumCapsuleSize and ResetType are undefined.
  @retval EFI_INVALID_PARAMETER MaximumCapsuleSize is NULL, or ResetTyep is NULL,
                                Or CapsuleCount is Zero, or CapsuleImage is not valid.

**/
EFI_STATUS
EFIAPI
QueryCapsuleCapabilities (
  IN  EFI_CAPSULE_HEADER   **CapsuleHeaderArray,
  IN  UINTN                CapsuleCount,
  OUT UINT64               *MaxiumCapsuleSize,
  OUT EFI_RESET_TYPE       *ResetType
  )
{
  EFI_STATUS              Status = EFI_SUCCESS;
  UINTN                   ArrayNumber;
  EFI_CAPSULE_HEADER      *CapsuleHeader;
  BOOLEAN                 NeedReset;

  CAPSULE_GUID_TABLE      *CapsuleGuidTable = NULL; 
  UINT32                  *MemLoc;
  UINT32                  ImgSize;

  //
  // Capsule Count can't be less than one.
  //
  if (CapsuleCount < 1) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Check whether input parameter is valid
  //
  if ((MaxiumCapsuleSize == NULL) ||(ResetType == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  CapsuleHeader = NULL;
  NeedReset     = FALSE;

  for (ArrayNumber = 0; ArrayNumber < CapsuleCount; ArrayNumber++) {
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    //
    // A capsule which has the CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE)) == CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // A capsule which has the CAPSULE_FLAGS_INITIATE_RESET flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET)) == CAPSULE_FLAGS_INITIATE_RESET) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Check FMP capsule flag 
    //
    if (CompareGuid(&CapsuleHeader->CapsuleGuid, &gEfiFmpCapsuleGuid)
     && (CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) != 0 ) {
       return EFI_INVALID_PARAMETER;
    }

    //
    // Check Capsule image without populate flag is supported by firmware
    //
    if (((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) && 
          (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS)) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // Find out whether there is any capsule defined to persist across system reset. 
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount ; ArrayNumber++) {
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) != 0) {
      NeedReset = TRUE;
      break;
    }
  }
  
  if (NeedReset) {
    //
    //Check if the platform supports update capsule across a system reset
    //
    if (!FeaturePcdGet(PcdSupportUpdateCapsuleReset)) {
      return EFI_UNSUPPORTED;
    }

    *ResetType = EfiResetWarm;
    // TODO: change to be based on EfiAtRuntime and size of storage left
    if (EfiAtRuntime() || (CapsuleBlkIo == NULL))
    {
      *MaxiumCapsuleSize = FixedPcdGet32(PcdMaxSizePopulateCapsule);
    }
    else
    {
      // Get Capsule Guid Table
      ImgSize = CapsuleBlkIo->Media->BlockSize;
      Status = gBS->AllocatePool(EfiBootServicesData,
                                 ImgSize,
                                 (VOID**)&MemLoc);
      if (Status != EFI_SUCCESS)
      {  
        return Status;
      }

      Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                         CapsuleBlkIo->Media->MediaId, 
                                         0, 
                                         ImgSize, 
                                         MemLoc);
      if (EFI_ERROR (Status))
      {  
        return Status;
      }

      CapsuleGuidTable = (CAPSULE_GUID_TABLE*)MemLoc;

      // Calculate CRC on table, if doesn't match or is uninitialized, reinitialize
      Status = ValidateCapsuleGuidTable(CapsuleGuidTable);
      if (Status != EFI_SUCCESS)
      {
        *MaxiumCapsuleSize = (CapsuleBlkIo->Media->LastBlock - 1)*CAPSULE_UPDATE_TABLE_BLOCK_SIZE;
        Status = EFI_SUCCESS;
      }
      else
      {
        *MaxiumCapsuleSize = (CapsuleGuidTable->FreeBlocks)*CAPSULE_UPDATE_TABLE_BLOCK_SIZE;
      }
    }
  } 
  else {
    //
    // For non-reset capsule image.
    //
    *ResetType = EfiResetCold;
    // TODO: change to be based on EfiAtRuntime and size of storage left
    *MaxiumCapsuleSize = FixedPcdGet32(PcdMaxSizeNonPopulateCapsule);
  }

  return Status;
}

/**

  This code loads capsules in boot time from the NV storage and processes them 
  in the same way as capsules in RAM.

  @retval EFI_SUCCESS    UEFI Capsule Runtime Services are installed successfully. 

**/
EFI_STATUS
EFIAPI
LoadPersistedCapsules (
  VOID
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  CAPSULE_GUID_TABLE *CapsuleGuidTable = NULL; 
  EFI_GUID InvalidGuid = CAPSULE_INVALID_GUID;
  CAPSULE_GUID_ENTRY* CapsuleGuidEntry = NULL;
  UINT32                  Index;

  UINT32                  *MemLoc;
  UINT32                  ImgSize;
  EFI_LBA                 CapsuleLBAOffset;
  BOOLEAN                 ResetNeeded = FALSE;

  do
  {
    if (CapsuleBlkIo == NULL)
    {
      Status = EFI_NOT_FOUND;
      break;
    }

    // Get Capsule Guid Table
    ImgSize = CapsuleBlkIo->Media->BlockSize;
    Status = gBS->AllocatePool(EfiBootServicesData,
                               ImgSize,
                               (VOID**)&MemLoc);
    if (Status != EFI_SUCCESS)
      break;

    Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                       CapsuleBlkIo->Media->MediaId, 
                                       0, 
                                       ImgSize, 
                                       MemLoc);
    if (EFI_ERROR (Status))
    {  
      break;
    }

    CapsuleGuidTable = (CAPSULE_GUID_TABLE*)MemLoc;

    // Calculate CRC on table, if doesn't match or is uninitialized, reinitialize
    Status = ValidateCapsuleGuidTable(CapsuleGuidTable);
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    // Load each capsule and process
    for (Index = 0; Index < CapsuleGuidTable->GuidCount; Index++)
    {
      CapsuleGuidEntry = &(CapsuleGuidTable->CapsuleGuidEntries[Index]);

      // Confirm GUID entry is valid
      if (CompareGuid(&(CapsuleGuidEntry->Guid), &InvalidGuid))
      {
        // This should never be the case
        Status = EFI_VOLUME_CORRUPTED;
        break;
      }

      // Get the Capsule size. Account for one extra block in case not a 
      // multiple of block size
      ImgSize = CapsuleGuidEntry->ImageSize;
      if ((CapsuleGuidEntry->ImageSize)%CAPSULE_UPDATE_TABLE_BLOCK_SIZE != 0)
      {
        ImgSize += CAPSULE_UPDATE_TABLE_BLOCK_SIZE - 
                   ((CapsuleGuidEntry->ImageSize)%CAPSULE_UPDATE_TABLE_BLOCK_SIZE);
      }

      CapsuleLBAOffset = CapsuleGuidEntry->BlockOffset;

      // TODO: Allocate boot time memory for those that are don't have
      // CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE set.
      // For now, allocate runtime memory for all capsules 
      Status = gBS->AllocatePool(EfiRuntimeServicesData,
                                 ImgSize,
                                 (VOID**)&MemLoc);
      if (Status != EFI_SUCCESS)
        break;

      // Load entire capsule into memory
      Status = CapsuleBlkIo->ReadBlocks (CapsuleBlkIo, 
                                         CapsuleBlkIo->Media->MediaId, 
                                         CapsuleLBAOffset, 
                                         ImgSize, 
                                         MemLoc);
      if (Status != EFI_SUCCESS)
      {  
        break;
      }
        
      // Process capsule
      Status = ProcessCapsuleImageEx((EFI_CAPSULE_HEADER*)MemLoc, CapsuleLBAOffset, &ResetNeeded);
    }

    if (Status != EFI_SUCCESS)
    {
      break;
    }
  }
  while(0);

  if (ResetNeeded)
  {
    EfiResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }

  return Status;
}

/**
  Function to compare strings per section 7.5.5 of the UEFI 2.4B specification

  @param[in] Buffer1                  The pointer to first buffer.
  @param[in] Buffer2                  The pointer to second buffer.

  @retval 0                           Buffer1 equal to Buffer2.
  @return <0                          Buffer1 is less than Buffer2.
  @return >0                          Buffer1 is greater than Buffer2.
**/
STATIC
INTN
EFIAPI
CapsuleStringCompare (
  IN CONST VOID                 *Buffer1,
  IN CONST VOID                 *Buffer2
  )
{
  CONST CHAR16 *String1 = Buffer1;
  CONST CHAR16 *String2 = Buffer2;

  do
  {
    BOOLEAN LastDot1 = ((*String1 == '.') && (NULL == StrStr(String1+1,L".")));
    BOOLEAN LastDot2 = ((*String2 == '.') && (NULL == StrStr(String2+1,L".")));
    CHAR16 Compare1;
    CHAR16 Compare2;

    if ((*String1 == '\0') && (*String2 == '\0'))
    {
      return 0;
    }

    if((LastDot1 && !LastDot2) || (*String1 == '\0'))
    {
      Compare1 = ' ';
    }
    else
    {
      if ((*String1 >= 'a') && (*String1 <= 'z'))
      {
        Compare1 = *String1 + 'A' - 'a';
      }
      else
      {
        Compare1 = *String1;
      }
    }
    if((LastDot2 && !LastDot1) || (*String2 == '\0'))
    {
      Compare2 = ' ';
    }
    else
    {
      if ((*String2 >= 'a') && (*String2 <= 'z'))
      {
        Compare2 = *String1 + 'A' - 'a';
      }
      else
      {
        Compare2 = *String1;
      }
    }

    if (Compare1 != Compare2) {
      return Compare1 - Compare2;
    }

    if ((*String1 != '\0') ||
        !LastDot1 ||
        (LastDot1 && LastDot2))
    {
      String1++;
    }

    if ((*String2 != '\0') ||
        !LastDot2 ||
        (LastDot1 && LastDot2))
    {
      String2++;
    }
  } while(1);
}


/**
  Locates the handle to a device on children of the device path that supports the specified protocol.

  @param  Protocol              Specifies the protocol to search for.
  @param  DevicePath            Pointer to a pointer to the device path.
  @param  HandleCount           A pointer to the number of handles returned.
  @param  Handles               A pointer to an array of handles.

  @retval EFI_SUCCESS           The resulting handle was returned.
  @retval EFI_NOT_FOUND         No handles match the search.
  @retval EFI_INVALID_PARAMETER Protocol is NULL.
  @retval EFI_INVALID_PARAMETER DevicePath is NULL.
  @retval EFI_INVALID_PARAMETER A handle matched the search and Device is NULL.

**/
EFI_STATUS
EFIAPI
LocateChildDevicePath (
  IN EFI_GUID                       *Protocol,
  IN EFI_DEVICE_PATH_PROTOCOL      **DevicePath,
  OUT UINTN                         *MatchHandleCount,
  OUT EFI_HANDLE                   **MatchHandles
  )
{
  INTN                        SourceSize;
  INTN                        Size;
  UINTN                       HandleCount;
  UINTN                       Index;
  EFI_STATUS                  Status;
  EFI_HANDLE                  *Handles;
  EFI_HANDLE                  Handle;
  EFI_DEVICE_PATH_PROTOCOL    *SourcePath;
  EFI_DEVICE_PATH_PROTOCOL    *TmpDevicePath;

  if (Protocol == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DevicePath == NULL) || (*DevicePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((MatchHandles == NULL) || (MatchHandleCount == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Handles = NULL;
  SourcePath = *DevicePath;
  TmpDevicePath = SourcePath;
  while (!IsDevicePathEnd (TmpDevicePath)) {
    if (IsDevicePathEndInstance (TmpDevicePath)) {
      //
      // If DevicePath is a multi-instance device path,
      // the function will operate on the first instance 
      //
      break;
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
  }

  SourceSize = (UINTN) TmpDevicePath - (UINTN) SourcePath;

  //
  // Get a list of all handles that support the requested protocol
  //
  Status = gBS->LocateHandleBuffer (ByProtocol, Protocol, NULL, &HandleCount, &Handles);
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return EFI_NOT_FOUND;
  }

  *MatchHandles = AllocatePool (HandleCount * sizeof(EFI_HANDLE));
  if (NULL == *MatchHandles) {
    return EFI_NOT_FOUND;
  }

  *MatchHandleCount = 0;
  for(Index = 0; Index < HandleCount; Index += 1) {
    Handle = Handles[Index];
    Status = gBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, (VOID **)&TmpDevicePath);
    if (EFI_ERROR (Status)) {
      //
      // If this handle doesn't support device path, then skip it
      //
      continue;
    }

    //
    // Check if DevicePath is first part of SourcePath
    //
    Size = GetDevicePathSize (TmpDevicePath) - sizeof(EFI_DEVICE_PATH_PROTOCOL);
    ASSERT (Size >= 0);
    if ((Size >= SourceSize) && CompareMem (SourcePath, TmpDevicePath, (UINTN) SourceSize) == 0) {
      *MatchHandles[*MatchHandleCount] = Handle;
      (*MatchHandleCount)++;
    }
  }

  FreePool (Handles);

  //
  // If there wasn't any match, then no parts of the device path was found.
  // Which is strange since there is likely a "root level" device path in the system.
  //
  if (*MatchHandleCount == 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}
/**
  Function to process capsules from mass storage per section 7.5.5 of the UEFI 2.4B specification

**/
STATIC
VOID
EFIAPI
LoadMassStorageCapsules (
    VOID
  )
{
  EFI_HANDLE *Handles = NULL;
  UINTN NoHandles = 0;
  UINTN Index;
  EFI_STATUS Status;
  EFI_FILE_INFO *FileInfo = NULL;
  UINTN  Size;
  BOOLEAN ResetNeeded = FALSE;

  if (BOOT_IN_RECOVERY_MODE == GetBootModeHob())
  {
    UINT64 *OsIndications = BdsLibGetVariableAndSize (L"OsIndications", &gEfiGlobalVariableGuid, &Size);

    if((OsIndications != NULL) && ((*OsIndications & EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED) != 0))
    {
      *OsIndications &= ~(UINT64)EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED;
      Status = gRT->SetVariable(L"OsIndications",
                                &gEfiGlobalVariableGuid,
                                EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                                sizeof(UINT64),
                                OsIndications);
    }


    //Get all ESP partitions
    Status = gBS->LocateHandleBuffer ( ByProtocol,
                                  &gEfiPartTypeSystemPartGuid,
                                  NULL,
                                  &NoHandles,
                                  &Handles );
    if (EFI_ERROR(Status))
    {
      return;
    }
  }
  else
  {
    UINT16 *BootVar;
    CHAR16 BootVariableName[12];
    UINT64 *OsIndications = BdsLibGetVariableAndSize (L"OsIndications", &gEfiGlobalVariableGuid, &Size);

    if((OsIndications == NULL) || ((*OsIndications & EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED) == 0))
    {
      return;
    }
    *OsIndications &= ~(UINT64)EFI_OS_INDICATIONS_FILE_CAPSULE_DELIVERY_SUPPORTED;
    Status = gRT->SetVariable(L"OsIndications",
                              &gEfiGlobalVariableGuid,
                              EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                              sizeof(UINT64),
                              OsIndications);
    FreePool (OsIndications);

    BootVar = BdsLibGetVariableAndSize (L"BootNext", &gEfiGlobalVariableGuid, &Size);
    if (NULL != BootVar)
    {
      CHAR8 *BootVariable;
      EFI_DEVICE_PATH_PROTOCOL  *DevPath = NULL;
      UnicodeSPrint (BootVariableName, sizeof (BootVariableName), L"Boot%04x", BootVar[0]);
      BootVariable = BdsLibGetVariableAndSize (
                      BootVariableName,
                      &gEfiGlobalVariableGuid,
                      &Size
                      );

      if (BootVariable != NULL)
      {
        if ((*(UINT16*)(BootVariable+4)) != 0)
        {
          DevPath = (EFI_DEVICE_PATH_PROTOCOL *)(BootVariable+6+StrSize((CHAR16*)(BootVariable+6)));
          Status = LocateChildDevicePath ( &gEfiPartTypeSystemPartGuid,
                                           &DevPath,
                                           &NoHandles,
                                           &Handles );
          if (EFI_ERROR(Status))
          {
            NoHandles = 0;
            Handles = NULL;
          }
        }
        FreePool(BootVariable);
      }
      FreePool(BootVar);
    }

    if (NULL == Handles)
    {
      BootVar = BdsLibGetVariableAndSize (L"BootOrder", &gEfiGlobalVariableGuid, &Size);
      if (NULL != BootVar)
      {
        for (Index = 0; Index < Size/sizeof(UINT16); Index++)
        {
          CHAR8 *BootVariable;
          EFI_DEVICE_PATH_PROTOCOL  *DevPath = NULL;
          UnicodeSPrint (BootVariableName, sizeof (BootVariableName), L"Boot%04x", BootVar[Index]);
          BootVariable = BdsLibGetVariableAndSize (
                          BootVariableName,
                          &gEfiGlobalVariableGuid,
                          &Size
                          );

          if (BootVariable != NULL)
          {
            if ((*(UINT16*)(BootVariable+4)) != 0)
            {
              DevPath = (EFI_DEVICE_PATH_PROTOCOL *)(BootVariable+6+StrSize((CHAR16*)(BootVariable+6)));
              Status = LocateChildDevicePath ( &gEfiPartTypeSystemPartGuid,
                                               &DevPath,
                                               &NoHandles,
                                               &Handles );
              if (EFI_ERROR(Status))
              {
                NoHandles = 0;
                Handles = NULL;
              }
              else
              {
                break;
              }
            }
            FreePool(BootVariable);
          }
        }
        FreePool(BootVar);
      }
    }
  }

  if (NoHandles != 0)
  {
    FileInfo = AllocatePool (sizeof(EFI_FILE_INFO) + MAX_FILE_NAME_SIZE);
    if (FileInfo == NULL)
    {
      if(NULL != Handles)
      {
        FreePool(Handles);
      }
      return;
    }
  }
  if (NULL == Handles) {
    return;
  }
  for (Index = 0; Index < NoHandles; Index++)
  {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
    EFI_FILE_PROTOCOL *RootVolume;
    EFI_FILE_PROTOCOL *Directory;
    CHAR16 *FileNames = NULL;
    UINTN Files = 0;
    UINTN AllocatedFiles = 0;

    Status = gBS->HandleProtocol (Handles[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&SimpleFileSystem);
    if (EFI_ERROR(Status))
    {
      continue;
    }

    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &RootVolume);
    if (EFI_ERROR(Status))
    {
      continue;
    }

    Status = RootVolume->Open (RootVolume, &Directory, L"EFI\\UpdateCapsule", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status))
    {
      RootVolume->Close(RootVolume);
      continue;
    }
    RootVolume->Close(RootVolume);

    Size = sizeof(EFI_FILE_INFO) + MAX_FILE_NAME_SIZE;
    while (EFI_SUCCESS == Directory->Read (Directory, &Size, FileInfo))
    {
      if(Size == 0)
      {
        break;
      }

      if ((FileInfo->Attribute & EFI_FILE_DIRECTORY) != 0)
      {
        Size = sizeof(EFI_FILE_INFO) + MAX_FILE_NAME_SIZE;
        continue;
      }

      if (Files == AllocatedFiles)
      {
        CHAR16 *FileNamesNew = AllocatePool((AllocatedFiles + FILE_COUNT_STEP) * MAX_FILE_NAME_SIZE);
        if (NULL == FileNamesNew)
        {
          break;
        }
        if (NULL != FileNames)
        {
          CopyMem(FileNamesNew, FileNames, AllocatedFiles * MAX_FILE_NAME_SIZE);
          FreePool(FileNames);
        }
        AllocatedFiles += FILE_COUNT_STEP;
        FileNames = FileNamesNew;
      }
      CopyMem(FileNames + (Files * MAX_FILE_NAME_SIZE/sizeof(CHAR16)), FileInfo->FileName, StrSize(FileInfo->FileName));
      Files++;
      Size = sizeof(EFI_FILE_INFO) + MAX_FILE_NAME_SIZE;
    }

    if (Files != 0)
    {
      UINTN File;
      PerformQuickSort( FileNames, Files, MAX_FILE_NAME_SIZE, CapsuleStringCompare );
      for (File = 0; File < Files; File++)
      {
        EFI_FILE_PROTOCOL *FileHandle;
        VOID *FileData;
        Status = Directory->Open(Directory, &FileHandle, &FileNames[File*(MAX_FILE_NAME_SIZE/sizeof(CHAR16))], EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE, 0);
        if (EFI_ERROR(Status))
        {
          // Try read only
          Status = Directory->Open(Directory, &FileHandle, &FileNames[File*(MAX_FILE_NAME_SIZE/sizeof(CHAR16))], EFI_FILE_MODE_READ, 0);
          if (EFI_ERROR(Status))
          {
            continue;
          }
        }

        Size = sizeof(EFI_FILE_INFO) + MAX_FILE_NAME_SIZE;
        Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &Size, FileInfo);
        if (EFI_ERROR(Status))
        {
          FileHandle->Delete(FileHandle);
          continue;
        }

        FileData = AllocatePool(FileInfo->FileSize);
        if (NULL == FileData)
        {
          FileHandle->Delete(FileHandle);
          continue;
        }

        Size = FileInfo->FileSize;
        Status = FileHandle->Read(FileHandle, &Size, FileData);
        ResetNeeded = TRUE;
        if (!EFI_ERROR(Status))
        {
          Status = ProcessCapsuleImageEx(FileData, 0, NULL);
        }
        FileHandle->Delete(FileHandle);
        FreePool(FileData);
      }
    }
    Directory->Close(Directory);
  }

  if (NULL != FileInfo)
  {
    FreePool(FileInfo);
  }

  if (NULL != Handles)
  {
    FreePool(Handles);
  }

  if (ResetNeeded)
  {
    Status = gRT->SetVariable(L"BootNext",
                              &gEfiGlobalVariableGuid,
                              EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                              0,
                              NULL);
    EfiResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }
}

/**
  Function to process capsule after driver load

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
STATIC
VOID
EFIAPI
DelayedCapsuleProcess (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{

  // Get Block I/O for Update Capsule Partition if it exists
  if (UpdateCapsulePartitionExists() == TRUE)
  {
    // Load all persisted capsules from NV storage and process them
    LoadPersistedCapsules();
  }

  LoadMassStorageCapsules();

}


#define CAPSULE_HEADER_OFFSET 0x3E2000
#define CAPSULE_HEADER_SIZE   0x1000

/**

  This code installs UEFI capsule runtime service.

  @param  ImageHandle    The firmware allocated handle for the EFI image.  
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    UEFI Capsule Runtime Services are installed successfully. 

**/
EFI_STATUS
EFIAPI
CapsuleServiceInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT32      Data;
  UINTN       DataSize = sizeof(Data);
  MemRegionInfo UefiFdMemRegionInfo;
  STATIC CHAR8* MemLabel = "UEFI FD";
  UINT32 DelayCapsuleProcessing = 0;

  // Get the Capsule Header region.
  Status = GetMemRegionInfoByName(MemLabel, &UefiFdMemRegionInfo);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Could not find the Capsule Header region.\r\n"));
    return Status;
  }

  mScatterGatherHeader = (SCATTERGATHERLIST_ARRAY_HEADER*)((UINTN)UefiFdMemRegionInfo.MemBase + CAPSULE_HEADER_OFFSET);
  mScatterGatherHeaderSize = CAPSULE_HEADER_SIZE;
  
  //
  // Install capsule runtime services into UEFI runtime service tables.
  //
  gRT->UpdateCapsule                    = UpdateCapsule;
  gRT->QueryCapsuleCapabilities         = QueryCapsuleCapabilities;

  //
  // Install the Capsule Architectural Protocol on a new handle
  // to signify the capsule runtime services are ready.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mNewHandle,
                  &gEfiCapsuleArchProtocolGuid,
                  NULL,
                  &gEfiCapsuleProtocolGuid,
                  &gCapsuleProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  // Support MaxCapsuleAge of 0 (disabled), default, or greater than default
  Status = gRT->GetVariable(L"MaxCapsuleAge",
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &DataSize,
                            &Data);
  if (Status == EFI_NOT_FOUND)
  {
    MaxCapsuleAge = MAX_CAPSULE_AGE_DEFAULT;
  }
  else if ((Status == EFI_SUCCESS) && (Data > MAX_CAPSULE_AGE_DEFAULT))
  {
    MaxCapsuleAge = Data;
  }
  else if ((Status == EFI_SUCCESS) && (Data == 0))
  {
    MaxCapsuleAge = 0;
  }
  else if (Status != EFI_SUCCESS)
  {
    ASSERT(Status);
  }

  Status = gBS->CreateEventEx (
              EVT_NOTIFY_SIGNAL,
              TPL_CALLBACK,
              VirtualAddressChangeCallBack,
              NULL,
              &gEfiEventVirtualAddressChangeGuid,
              &mVirtualAddressChangeEvent
              );
  ASSERT_EFI_ERROR (Status);

  CheckScatterGatherListArray(mScatterGatherHeader, FALSE);


  Status = GetConfigValue("DelayCapsuleProcessing", (UINT32*)&DelayCapsuleProcessing);
  if (EFI_ERROR(Status)) {
    DelayCapsuleProcessing = 0;
    Status = EFI_SUCCESS;
  }

  if (DelayCapsuleProcessing == 0) {
    // Get Block I/O for Update Capsule Partition if it exists
    if (UpdateCapsulePartitionExists() == TRUE)
    {
      // Load all persisted capsules from NV storage and process them
      LoadPersistedCapsules();
    }
  } else {
    Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                DelayedCapsuleProcess,
                NULL,
                &gBdsEventGroupGuid,
                &mDelayedCapsuleProcessEvent
                );
    ASSERT_EFI_ERROR (Status);
  }
    


  return Status;
}
