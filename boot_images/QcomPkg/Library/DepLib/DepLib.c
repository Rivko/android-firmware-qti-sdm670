/**@file DepLib.c

  Copyright (c) 2015 - 2016, Qualcomm Technologies Inc. All rights reserved.<BR>

  Portions Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>
  Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 
 --------   ---     -----------------------------------------------------------
 09/09/15   vk      Add MMU lib
 07/29/16   vk      Remove cache maintainance, done in library functions
 06/13/16   vk      Initial version
=============================================================================*/

#include <PiDxe.h>
#include <Library/PeCoffLib.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/ArmMmuLib.h>

#define DEP_DEBUG_LEVEL EFI_D_INFO
#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

STATIC
VOID *
GetImageAddress (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT          *ImageContext,
  IN     UINTN                                 Address, 
  IN     UINTN                                 TeStrippedOffset
  )
{
  //
  // Make sure that Address and ImageSize is correct for the loaded image.
  //
  if (Address >= ImageContext->ImageSize + TeStrippedOffset) {
    ImageContext->ImageError = IMAGE_ERROR_INVALID_IMAGE_ADDRESS;
    return NULL;
  }

  return (CHAR8 *)((UINTN) ImageContext->ImageAddress + Address - TeStrippedOffset);
}

STATIC
VOID
PrintModuleName (IN CHAR8* PdbPointer)
{
  CHAR8 *ModuleNamePtr = NULL;
  if (PdbPointer)
  {
    if (AsciiStrStr(PdbPointer, "/") != NULL)
      ModuleNamePtr = AsciiStrStr(PdbPointer, "/DEBUG/") + AsciiStrLen ("/DEBUG/");	   
    else if (AsciiStrStr(PdbPointer, "\\") != NULL)
      ModuleNamePtr = AsciiStrStr(PdbPointer, "\\DEBUG\\") + AsciiStrLen ("\\DEBUG\\");
  
    if (ModuleNamePtr)
    {
      DEBUG ((DEP_DEBUG_LEVEL, "--------------------------------------------------------------------------------\n"));
      DEBUG ((DEP_DEBUG_LEVEL, "Im: %a\n", ModuleNamePtr));
    }
  }

  return;  
}

STATIC VOID
SetExecSetReadOnly (IN EFI_PHYSICAL_ADDRESS StartAddr, IN UINT64 Length)
{
  EFI_STATUS Status;

  UINTN InterruptState = ArmGetInterruptState();
  ArmDisableInterrupts();

  Status = ArmClearMemoryRegionNoExec (StartAddr, Length);
  if (Status != EFI_SUCCESS)
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t XN:I: 0x%016lx--0x%016lx [Failed]\n", StartAddr, StartAddr+Length-1));
  else
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t XN:I: 0x%016lx--0x%016lx [Clear]\n", StartAddr, StartAddr+Length-1));

  Status = ArmSetMemoryRegionReadOnly (StartAddr, Length);
  if (Status != EFI_SUCCESS)
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t RO:I: 0x%016lx--0x%016lx [Failed]\n", StartAddr, StartAddr+Length-1));
  else
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t RO:I: 0x%016lx--0x%016lx [Set]\n", StartAddr, StartAddr+Length-1));
	
  if (InterruptState)
    ArmEnableInterrupts();
}

STATIC VOID
SetNoExecSetReadWrite (IN EFI_PHYSICAL_ADDRESS StartAddr, IN UINT64 Length)
{
  EFI_STATUS Status;
  
  UINTN InterruptState = ArmGetInterruptState();
  ArmDisableInterrupts();

  Status = ArmSetMemoryRegionNoExec (StartAddr, Length);
	
  
  if (Status != EFI_SUCCESS)
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t XN:I: 0x%016lx--0x%016lx [Failed]\n", StartAddr, StartAddr+Length-1));
  else
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t XN:I: 0x%016lx--0x%016lx [Set]\n", StartAddr, StartAddr+Length-1));

  Status = ArmClearMemoryRegionReadOnly (StartAddr, Length);
  if (Status != EFI_SUCCESS)
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t RO:I: 0x%016lx--0x%016lx [Failed]\n", StartAddr, StartAddr+Length-1));
  else
    DEBUG ((DEP_DEBUG_LEVEL, "\t\t\t\t\t RO:I: 0x%016lx--0x%016lx [Clear]\n", StartAddr, StartAddr+Length-1));

  if (InterruptState)
    ArmEnableInterrupts();
}


EFI_STATUS
SetupDep (IN PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  EFI_STATUS                            Status;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_SECTION_HEADER              *FirstSection;
  EFI_IMAGE_SECTION_HEADER              *Section;
  UINTN                                 NumberOfSections;
  UINTN                                 Index;
  VOID                                  *Base = NULL;
  VOID                                  *End = NULL;
  UINTN                                 Size;
  UINTN                                 TeStrippedOffset;

  EFI_PHYSICAL_ADDRESS                  StartAddr = 0;
  UINT64                                Length = 0;

  /* Read the entire PE/COFF or TE header into memory */
  if (!(ImageContext->IsTeImage)) {
    Status = ImageContext->ImageRead (
                            ImageContext->Handle,
                            0,
                            &ImageContext->SizeOfHeaders,
                            (VOID *) (UINTN) ImageContext->ImageAddress
                            );

    Hdr.Pe32 = (EFI_IMAGE_NT_HEADERS32 *)((UINTN)ImageContext->ImageAddress + ImageContext->PeCoffHeaderOffset);

    FirstSection = (EFI_IMAGE_SECTION_HEADER *) (
                      (UINTN)ImageContext->ImageAddress +
                      ImageContext->PeCoffHeaderOffset +
                      sizeof(UINT32) +
                      sizeof(EFI_IMAGE_FILE_HEADER) +
                      Hdr.Pe32->FileHeader.SizeOfOptionalHeader);
    NumberOfSections = (UINTN) (Hdr.Pe32->FileHeader.NumberOfSections);
    TeStrippedOffset = 0;
  } else {
    Status = ImageContext->ImageRead (
                            ImageContext->Handle,
                            0,
                            &ImageContext->SizeOfHeaders,
                            (void *)(UINTN)ImageContext->ImageAddress
                            );

    Hdr.Te = (EFI_TE_IMAGE_HEADER *)(UINTN)(ImageContext->ImageAddress);
    FirstSection = (EFI_IMAGE_SECTION_HEADER *) (
                      (UINTN)ImageContext->ImageAddress +
                      sizeof(EFI_TE_IMAGE_HEADER)
                      );
    NumberOfSections  = (UINTN) (Hdr.Te->NumberOfSections);
    TeStrippedOffset  = (UINT32) Hdr.Te->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
  }

  if (RETURN_ERROR (Status))
  {
    ImageContext->ImageError = IMAGE_ERROR_IMAGE_READ;
    return RETURN_LOAD_ERROR;
  }
  

  PrintModuleName(ImageContext->PdbPointer);
  
  /* Load each section of the image */
  Section = FirstSection;
  for (Index = 0; Index < NumberOfSections; Index++)
  {
    /* Read the section */
    Size = (UINTN) Section->Misc.VirtualSize;
    if ((Size == 0) || (Size > Section->SizeOfRawData))
      Size = (UINTN) Section->SizeOfRawData;
    
    /* Compute sections address */
    Base = GetImageAddress (ImageContext, Section->VirtualAddress, TeStrippedOffset);
    End  = GetImageAddress (ImageContext, Section->VirtualAddress + Section->Misc.VirtualSize - 1, TeStrippedOffset);

    /* PE/COFF Spec allows for section name of length 8 to not have NULL terminator */
    if (AsciiStrnCmp ((CHAR8*)(&Section->Name), "INITDATA", 8)==0) 
      DEBUG ((DEP_DEBUG_LEVEL, "SetupDep: Section %a \t\t Base: 0x%016lx \t Size: 0x%08lx \n", "INITDATA", Base, Section->Misc.VirtualSize));
    else
      DEBUG ((DEP_DEBUG_LEVEL, "SetupDep: Section %a \t\t Base: 0x%016lx \t Size: 0x%08lx \n", Section->Name, Base, Section->Misc.VirtualSize));

    if ( ((Section->Name[1] == 't') && (Section->Name[2] == 'e')) )
    {
      StartAddr = ((EFI_PHYSICAL_ADDRESS)Base);

      /* Bootmgr does not need header size adjustment */
      if ( (StartAddr & (SIZE_4KB - 1)) != 0)
        StartAddr = ((EFI_PHYSICAL_ADDRESS)Base);

      Length = Section->Misc.VirtualSize;
      Length = ROUND_TO_PAGE (Length, (SIZE_4KB-1));

      if ( ( (Length & (SIZE_4KB - 1)) == 0) )
        SetExecSetReadOnly(StartAddr, Length);
      else
        DEBUG ((DEP_DEBUG_LEVEL, "SetupDep: SKIP\n"));
    }
      
    Section++;
   }

  return EFI_SUCCESS;
}


EFI_STATUS
ClearDep (IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  EFI_PHYSICAL_ADDRESS                  StartAddr = 0;
  UINT64                                Length = 0;

  if (ImageContext == NULL)
    return EFI_INVALID_PARAMETER;


  PrintModuleName(ImageContext->PdbPointer);

  StartAddr = ImageContext->ImageAddress;
  Length = ImageContext->ImageSize;
  Length = ROUND_TO_PAGE (Length, (SIZE_4KB-1));

  if ( ( (Length & (SIZE_4KB - 1)) == 0) )
    SetNoExecSetReadWrite (StartAddr, Length);
  else
    DEBUG ((DEP_DEBUG_LEVEL, "ClearDep: SKIP\n"));

  return EFI_SUCCESS;
}
