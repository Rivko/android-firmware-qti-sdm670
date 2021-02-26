/** @file MemoryMapTest.c
   
  Tests for crash dump support including verification of memory map and
  UEFI Subsystem Memory Protocol.

  Usage: This app needs to be run twice. The first time it saves the
  memory map to the flash, enables carveout mode and resets the system. 
  After device reboots, run the app again then the verification will be
  executed.

  Copyright (c) 2011-2013, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/03/13   niting  Moved offline crash dump support into OfflineCrashDumpLib
 01/09/13   vk      Fix KlockWork warnings
 09/17/12   yg      Use ReadAnyKey API
 07/18/12   jz      Added function to compare memory maps in normal/debug mode v.s. carveout mode
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 06/08/12   vishalo Add variables for crash dump support
 05/30/12   jz      Cleanup
 05/22/12   jz      Added test to read from subsystem regions
 05/02/12   jz      Cleanup warnings, updated for testing subsystem memory report
 10/05/11   jz      Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIMemoryMap.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>  // for ZeroMem
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/OfflineCrashDump.h>

extern EFI_GUID gQcomMemoryCaptureGuid;
extern EFI_GUID gEfiMemoryMapProtocolGuid;
extern EFI_GUID gEfiPlatPartitionTypeGuid;

STATIC UINTN    HLOSCookieAddr; // physical address of the HLOS cookie
STATIC UINTN    DloadIdAddr;    // physical address of the flag
STATIC UINTN    DloadMagicSize; // size of the flag
STATIC UINT64   SetPattern;     // pattern used to set the flag
STATIC UINT64   ResetPattern;   // pattern used to reset the flag

STATIC UINTN    CarveoutRangeStart;
STATIC UINTN    CarveoutRangeEnd;

CHAR8 *gMemMapType[] = {
  "reserved  ",
  "LoaderCode",
  "LoaderData",
  "BS_code   ",
  "BS_data   ",
  "RT_code   ",
  "RT_data   ",
  "available ",
  "Unusable  ",
  "ACPI_recl ",
  "ACPI_NVS  ",
  "MemMapIO  ",
  "MemPortIO ",
  "PAL_code  "
};

STATIC
EFI_STATUS
SubsysMemTest (
  EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *pMemMap,
  UINTN Count
  )
{
  UINTN  Index;
  UINT32 *Address;
  INT32  tSize, count = 0;
  UINT32 data=0; 

  for (Index=0; Index<Count; Index++)
  {
    Address = (UINT32 *)(UINTN)(pMemMap[Index].PhysicalBaseAddress);
    tSize = pMemMap[Index].PageCount * EFI_PAGE_SIZE;
    while (tSize > 0)
    {
      // Read the first 8 uint32's in every 1MB chunk
      if (count < 8)
      {
        data = (UINT32)*Address;
        AsciiPrint("Address:0x%x, Data:0x%x\n", Address, data);
        Address ++;
        tSize -= sizeof(*Address);
        count++;
      }
      else // skip the rest of the 1MB chunk
      {
        Address += (SIZE_1MB / sizeof(*Address) - count);
        tSize -= (SIZE_1MB - count * sizeof(*Address));
        count = 0;
      }
    }
  }

  return EFI_SUCCESS;
}

STATIC
VOID
DisableCarveoutMode (
  VOID
  )
{
  *(UINT64 *)DloadIdAddr = ResetPattern;
  *(UINT32 *)HLOSCookieAddr = 0;
}

STATIC
VOID
EnableCarveoutMode (
  VOID
  )
{
  *(UINT64 *)DloadIdAddr = SetPattern;
  *(UINT32 *)HLOSCookieAddr = 1;
}

/**
  Compare/verify memory maps in normal mode and carveout mode

  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
STATIC
EFI_STATUS
MemmapCompare (
  VOID
  )
{
  EFI_STATUS              Status;
  CHAR16                  *FileName = L"memmap.bin";
  UINTN                   MemMapSize, LastMemMapSize;
  EFI_MEMORY_DESCRIPTOR   *MemMap = NULL;      // memory map in carveout mode 
  EFI_MEMORY_DESCRIPTOR   *LastMemMap = NULL;  // memory map in normal mode
  EFI_MEMORY_DESCRIPTOR   *pMemMap, *pCarveoutMemMap, *pNormalMemMap;
  UINTN                   MapKey;
  UINTN                   DescriptorSize;
  UINT32                  DescriptorVersion;
  UINTN                   Index, Index2;
  UINTN                   NumErrors = 0;
  EFI_PHYSICAL_ADDRESS    MemStart, MemEnd;
  EFI_PHYSICAL_ADDRESS    CarveoutMemStart, CarveoutMemEnd;
  EFI_PHYSICAL_ADDRESS    NormalMemStart, NormalMemEnd;
  
  // First call is to figure out how big the buffer needs to be
  MemMapSize = 0;
  Status = gBS->GetMemoryMap (&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    MemMapSize += EFI_PAGE_SIZE;
    MemMap = AllocatePool(MemMapSize);
    if (MemMap == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done; // error out
    }
    // Now get the memory map
    Status = gBS->GetMemoryMap(&MemMapSize, MemMap, &MapKey, &DescriptorSize, &DescriptorVersion);
  }

  if ((!EFI_ERROR (Status)) && (MemMap != NULL)) 
  {
    if (!InCarveoutMode())
    {
      static CHAR16 *MassStorageStr = MASS_STORAGE_STRING;	    
      AsciiPrint("\nCarveout mode is not enabled\n");

      pMemMap = MemMap;
      // Find the entry that matches the carveout memory range in the memory map (normal mode)
      for (Index = 0; Index < MemMapSize/DescriptorSize; Index++)
      {
        MemStart = pMemMap->PhysicalStart;
        MemEnd = pMemMap->PhysicalStart + EFI_PAGES_TO_SIZE(pMemMap->NumberOfPages);
        if (((EFI_PHYSICAL_ADDRESS)MemStart == CarveoutRangeStart) &&
            ((EFI_PHYSICAL_ADDRESS)MemEnd == CarveoutRangeEnd))
        {
          // Make sure its memory type is Reserved
          if (pMemMap->Type != EfiReservedMemoryType)
          {
            NumErrors++;
            AsciiPrint ("\n*****%a %016lx - %016lx: # %08lx %016lx\n", 
                        gMemMapType[pMemMap->Type % EfiMaxMemoryType], 
                        MemStart, 
                        MemEnd, 
                        pMemMap->NumberOfPages, 
                        pMemMap->Attribute);
            goto Done; // error out
          }
          else
            break;
        }
        // Move to next entry in MemMap
        pMemMap = NEXT_MEMORY_DESCRIPTOR (pMemMap, DescriptorSize);
      }

      // Not found
      if (Index == MemMapSize/DescriptorSize)
      {
        AsciiPrint ("\nUnable to find Reserved region for carveout memory!!!\n");
        Status = EFI_INVALID_PARAMETER;
        goto Done; // error out
      }

      // Save memory map in normal mode
      Status = WriteFile(FileName,
                         NULL,
                         &gEfiPlatPartitionTypeGuid,
                         TRUE,
                         NULL,
                         &MemMapSize,
                         0,
                         (UINT8 *)MemMap,
                         MemMapSize);
      if (EFI_ERROR(Status))
      {
        AsciiPrint ("Failed to save memory map\n");
        goto Done; // error out
      }

      if (MemMap != NULL)
        FreePool(MemMap);

      Print(L"%s is saved\n", FileName);
      AsciiPrint("Enable carve-out mode and resetting...\n\n");
      
      EnableCarveoutMode();
      
      // Force reset
      gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, StrSize(MASS_STORAGE_STRING), MassStorageStr);
    }
    else // In carveout mode
    {
      AsciiPrint("\nCarveout mode is enabled\n");

      // Set back to normal mode
      DisableCarveoutMode();
  
      // Check for file size
      Status = GetFileSize(FileName, 
                           NULL,
                           &gEfiPlatPartitionTypeGuid,
                           TRUE,
                           NULL,
                           &LastMemMapSize);

      if (!EFI_ERROR(Status))
      {
        LastMemMap = AllocatePool(LastMemMapSize);
        if (LastMemMap == NULL)
        {
          Status = EFI_OUT_OF_RESOURCES;
          goto Done; // error out
        }
        
        // Retrieve last saved memory map (in normal mode)
        Status = ReadFile(FileName,
                          NULL,
                          &gEfiPlatPartitionTypeGuid,
                          TRUE,
                          NULL,
                          &LastMemMapSize,
                          0,
                          (UINT8 *)LastMemMap,
                          LastMemMapSize);
        if (EFI_ERROR(Status))
        {
          Print (L"Failed to read memory map from %s, Status: 0x%s\n",
                 FileName, Status);
          // Restart the device to try again
          // so that the memory map in normal mode could be captured
          if (LastMemMap)
            FreePool(LastMemMap);
          gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
        
        Print (L"Last saved %s is retrieved\n", FileName);

        // Now compare the memory maps (in carveout mode v.s. normal mode)
        // In carveout mode, all the memory regions that are available to HLOS
        // should have been reserved and not released to HLOS in normal mode
        //
        // Here's the logic:
        //
        //	 Go through all the entries in carveout memmap
        //     If the range falls outside of the carveout memory range (PCDs)
        //	     If memory type is BS or CONV, then error
        //	     Else
        //	       Go through each entry in normal memmap
        //         	 Find the entry that exactly overlaps
        //             if not having the same memory type and attrib, then error

        pCarveoutMemMap = MemMap;
        // Loop through all the entries in CarveoutMemMap
        for (Index = 0; Index < MemMapSize/DescriptorSize; Index++)
        {
          CarveoutMemStart = pCarveoutMemMap->PhysicalStart;
          CarveoutMemEnd = pCarveoutMemMap->PhysicalStart + EFI_PAGES_TO_SIZE(pCarveoutMemMap->NumberOfPages);
          
          AsciiPrint ("\n=====%a %016lx - %016lx: # %08lx %016lx", 
                      gMemMapType[pCarveoutMemMap->Type % EfiMaxMemoryType], 
                      CarveoutMemStart, 
                      CarveoutMemEnd, 
                      pCarveoutMemMap->NumberOfPages, 
                      pCarveoutMemMap->Attribute);

          // Find any entry with range that falls outside of the carveout memory range
          if (!((CarveoutMemStart >= CarveoutRangeStart) && (CarveoutMemEnd <= CarveoutRangeEnd)))
          {
            // Check if its type is of BS or CONV in CarveoutMemMap
            if ((pCarveoutMemMap->Type == EfiBootServicesCode) ||
                (pCarveoutMemMap->Type == EfiBootServicesData) ||
                (pCarveoutMemMap->Type == EfiConventionalMemory))
            {
              NumErrors++;
              AsciiPrint ("\n*****%a %016lx - %016lx: # %08lx %016lx", 
                          gMemMapType[pCarveoutMemMap->Type % EfiMaxMemoryType], 
                          CarveoutMemStart, 
                          CarveoutMemEnd, 
                          pCarveoutMemMap->NumberOfPages, 
                          pCarveoutMemMap->Attribute);
            }
            else
            {
              pNormalMemMap = LastMemMap;
            
              // Loop through the entries in NormalMemMap
              for (Index2 = 0; Index2 < LastMemMapSize/DescriptorSize; Index2++)
              {
                NormalMemStart = pNormalMemMap->PhysicalStart;
                NormalMemEnd = pNormalMemMap->PhysicalStart + EFI_PAGES_TO_SIZE(pNormalMemMap->NumberOfPages);

                // Find the entry that exactly overlaps with an entry in CarveoutMemMap
                if ((CarveoutMemStart == NormalMemStart) && (CarveoutMemEnd == NormalMemEnd))
                {
                  AsciiPrint ("\n~~~~~%a %016lx - %016lx: # %08lx %016lx", 
                              gMemMapType[pNormalMemMap->Type % EfiMaxMemoryType], 
                              NormalMemStart, 
                              NormalMemEnd, 
                              pNormalMemMap->NumberOfPages, 
                              pNormalMemMap->Attribute);

                  // Check if their memory type and attributes match
                  if ((pCarveoutMemMap->Type != pNormalMemMap->Type) ||
                      (pCarveoutMemMap->Attribute!= pNormalMemMap->Attribute))
                  {
                    AsciiPrint ("\n*****%a %016lx - %016lx: # %08lx %016lx", 
                                gMemMapType[pNormalMemMap->Type % EfiMaxMemoryType], 
                                NormalMemStart, 
                                NormalMemEnd, 
                                pNormalMemMap->NumberOfPages, 
                                pNormalMemMap->Attribute);
                    NumErrors++;
                  }
                }
            
                // Move to next entry in NormalMemMap
                pNormalMemMap = NEXT_MEMORY_DESCRIPTOR (pNormalMemMap, DescriptorSize);
              } // end for Index2
            } // end else
          } // end if

          // Move to next entry in CarveoutMemMap
          pCarveoutMemMap = NEXT_MEMORY_DESCRIPTOR (pCarveoutMemMap, DescriptorSize);
          
        } // end for Index
        AsciiPrint ("\n");
      }
    }
  }

Done:
  if (MemMap != NULL)
    FreePool(MemMap);

  if (LastMemMap != NULL)
    FreePool(LastMemMap);
    
  if (NumErrors != 0)
  {
    AsciiPrint ("\n\n!!! %d memory regions have wrong type in carveout mode!\n", NumErrors);
    AsciiPrint ("!!! Please check the above entries marked with ***** in the memory map!\n", NumErrors);
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
MemmapTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_SUBSYSTEM_MEMORY_PROTOCOL *pMemoryMapProtocol;
  UINTN       Size;
  EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *SubsysMemMap = NULL, *pMemMap;
  UINTN       DescriptorCount;
  UINTN       Index;
  UINTN       DataSize;
  UINTN       MemoryBase;

  TEST_START("MemmapTest");

  // Get cookie address (i.e., memory_capture_mode_address) from variable
  DataSize= sizeof(HLOSCookieAddr);
  Status = gRT->GetVariable(L"memory_capture_mode_address", &gQcomMemoryCaptureGuid,
    NULL, &DataSize, &HLOSCookieAddr);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: GetVariable memory_capture_mode_address failed.(Status = %r)\n", Status);
    AsciiPrint ("Check if UEFI debug mode is enabled\n");
    return Status;
  }

  // Get legacy_flag_address from variable
  DataSize= sizeof(DloadIdAddr);
  Status = gRT->GetVariable(L"legacy_flag_address", &gQcomMemoryCaptureGuid,
    NULL, &DataSize, &DloadIdAddr);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: GetVariable failed legacy_flag_address (Status = %r)\n", Status);
    return Status;
  }
  AsciiPrint ("\nGot legacy_flag_address: 0x%x\n", DloadIdAddr);

  // Get legacy_flag_size from variable
  DataSize= sizeof(DloadMagicSize);
  Status = gRT->GetVariable(L"legacy_flag_size", &gQcomMemoryCaptureGuid,
    NULL, &DataSize, &DloadMagicSize);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: GetVariable failed legacy_flag_size (Status = %r)\n", Status);
    return Status;
  }
  AsciiPrint ("Got legacy_flag_size: 0x%x\n", DloadMagicSize);

  // Get legacy_flag_set_pattern from variable
  Status = gRT->GetVariable(L"legacy_flag_set_pattern", &gQcomMemoryCaptureGuid,
    NULL, &DloadMagicSize, &SetPattern);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: GetVariable failed legacy_flag_set_pattern (Status = %r)\n", Status);
    return Status;
  }
  AsciiPrint ("Got legacy_flag_set_pattern: 0x%016lx\n", SetPattern);


  // Get legacy_flag_reset_pattern from variable
  Status = gRT->GetVariable(L"legacy_flag_reset_pattern", &gQcomMemoryCaptureGuid,
    NULL, &DloadMagicSize, &ResetPattern);
    
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: GetVariable failed legacy_flag_reset_pattern (Status = %r)\n", Status);
    return Status;
  }
  AsciiPrint ("Got legacy_flag_reset_pattern: 0x%016lx\n", ResetPattern);

  if ((Status = GetMemoryInfo(DDR_SYSTEM_MEMORY, &MemoryBase, NULL)) != EFI_SUCCESS)
  {
    return Status;
  }

  CarveoutRangeStart = MemoryBase + FixedPcdGet32(PcdUefiMemPoolBaseOffset);
  CarveoutRangeEnd = CarveoutRangeStart + FixedPcdGet32(PcdUefiMemPoolSize);
  AsciiPrint ("\nCarve out memory range: 0x%x - 0x%x\n",
              CarveoutRangeStart, CarveoutRangeEnd);

  AsciiPrint ("\nVerify memory maps in normal mode and carveout mode...\n");
  Status = MemmapCompare();
  
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("\nERROR: memory map verification failed (Status = %r)\n", Status);
    return Status;
  }
  
  AsciiPrint ("\n~~~ Memory map verification PASSED ~~~\n");

  // Get subsystem memory map from protocol
  Status = gBS->LocateProtocol (
                &gEfiMemoryMapProtocolGuid,
                NULL,
                (VOID **) &pMemoryMapProtocol);
  if (EFI_ERROR(Status))
  {
    AsciiPrint ("ERROR: Failed to locate subsystem memory protocol\n");
    return Status;
  }

  Size = 0;
  Status = pMemoryMapProtocol->GetSubsystemMemoryMap(pMemoryMapProtocol, 
                                                     &Size,
                                                     SubsysMemMap,
                                                     &DescriptorCount);
                                                     
  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    SubsysMemMap = AllocatePool(Size);
    if (SubsysMemMap == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    }
    
    Status = pMemoryMapProtocol->GetSubsystemMemoryMap(pMemoryMapProtocol, 
                                                       &Size,
                                                       SubsysMemMap,
                                                       &DescriptorCount);
  }
  
  if (EFI_ERROR(Status) || (SubsysMemMap == NULL))  //For KW
  {
    AsciiPrint ("ERROR: Memory Map: GetSubsystemMemoryMap failed \n");
    return Status;
  }
  
  AsciiPrint ("\nSubsystem Memory Map\n");
  AsciiPrint ("Size: %d\n", Size);
  AsciiPrint ("DescriptorCount: %d\n", DescriptorCount);

  pMemMap = SubsysMemMap;
  for (Index = 0; Index < DescriptorCount; Index++)
  {
    AsciiPrint ("\n0x%x 0x%x 0x%016lx 0x%016lx: 0x%016lx 0x%016lx\n",
                pMemMap->Revision,
                pMemMap->SubsystemIdentifier.Data1,
                pMemMap->PhysicalBaseAddress,
                pMemMap->VirtualBaseAddress,
                pMemMap->PageCount,
                pMemMap->Attribute);
    
    pMemMap = (EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *)((UINT8 *)pMemMap + sizeof(EFI_SUBSYSTEM_MEMORY_DESCRIPTOR));
  }

  AsciiPrint("\n*** Subsystem memory regions read test ***\n");
  pMemMap = SubsysMemMap;
  Status = SubsysMemTest(pMemMap, DescriptorCount);

  if (SubsysMemMap != NULL)
    FreePool(SubsysMemMap);

  AsciiPrint("\nCrash dump test completed. Press any key to exit and reset the device\n");
  ReadAnyKey (NULL, 0);
  gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    
  TestStatus("MemmapTest", Status);
  TEST_STOP("MemmapTest");
  return Status;
}
