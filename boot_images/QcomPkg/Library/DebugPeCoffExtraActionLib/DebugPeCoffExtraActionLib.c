/**@file

Copyright (c) 2015 - 2017, Qualcomm Technologies Inc. All rights reserved.<BR>
Portions Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
Portions copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>
Portions copyright (c) 2011 - 2012, ARM Ltd. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/PeCoffLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffExtraActionLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/DepLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/CacheMaintenanceLib.h>


/* SYMPATH_ANCHOR_STR must be present in the Symbol path 
   Everything before SYMPATH_ANCHOR_STR is stripped from Symbol path */ 
#define SYMPATH_ANCHOR_STR    "Build"

/* UEFICORE_ANCHOR_STR is used to differentiate QC UEFI code */
#define UEFICORE_ANCHOR_STR   "boot_images"

/* Maximum size for T32 practice script */  
#define ALLOC_PAGES           8

/* Maximum size of string being added to T32 practice script */
#define TEMP_STR_SZ           1024

#define DRIVER_DELIMITER_CHAR ','
#define BKPT_DRVR_MAX_NUM     3
#define BKPT_DRVR_MAX_LEN     32


STATIC UefiInfoBlkType *InfoBlkPtr      = NULL;
STATIC CHAR8           *TempCmdString   = NULL;
CHAR8  DrvrList[BKPT_DRVR_MAX_NUM][BKPT_DRVR_MAX_LEN];
UINTN  DrvrNum                          = 0;
UINT8  DrvrListBuilt                    = 0;


/* Search SymbolPath for AnchorStr and strip everything before it 
   Return NULL if AnchorStr is not found, or if there is an error */
CHAR8 *
SanitizeForSymbolPath (
    IN  CHAR8   *SymbolPath, 
    IN  CHAR8   *AnchorStr,
    OUT UINTN* StripPartsPtr
	)
{
  CHAR8   *Ptr;
  UINTN    StripParts;
  CHAR8    TempPathStr[64];
  
  if (SymbolPath == NULL || AnchorStr == NULL)
    return NULL;
  
  /* Handle Windows/Linux path seperators */
  AsciiSPrint(TempPathStr, sizeof(TempPathStr), "/%a/", AnchorStr);
  Ptr = AsciiStrStr (SymbolPath, TempPathStr);
  if (Ptr == NULL) {
    AsciiSPrint(TempPathStr, sizeof(TempPathStr), "\\%a\\", AnchorStr);
    Ptr = AsciiStrStr (SymbolPath, TempPathStr);
  }

  if (StripPartsPtr)
  {
    StripParts = 0;
    while (SymbolPath < Ptr)
    {
       if ((*SymbolPath == '/') || (*SymbolPath == '\\'))
         ++StripParts;
       ++SymbolPath;
    }

    *StripPartsPtr = StripParts;
  }
    
  if (Ptr)
    return Ptr + 1;  /* Strip path sep */
  else
    return NULL;
}


STATIC
CHAR8* AlignStringSize (CHAR8* Str, UINTN* LenPtr)
{
  UINTN  TempSize, ApndSz;
  UINT64 AppendData;
  CHAR8 *AppendStrPtr;

  TempSize = AsciiStrLen (Str);
  if (LenPtr)
    *LenPtr = TempSize;

  TempSize = TempSize % sizeof(UINTN);
  TempSize = sizeof(UINTN) - TempSize;
  ApndSz = TempSize;

  if (LenPtr)
    *LenPtr += TempSize;

  AppendData = 0;
  AppendStrPtr = (CHAR8*)&AppendData;
  
  if (TempSize & 0x1)
  {
    AppendStrPtr[0] = ' ';
    --TempSize;
    ++AppendStrPtr;
  }
  
  while (TempSize > 0)
  {
    AppendStrPtr[0] = 13; // CR
    AppendStrPtr[1] = 10; // LF
    AppendStrPtr += 2;
    TempSize -= 2;
  }

  AppendStrPtr = (CHAR8*)&AppendData;
  AsciiStrnCat(Str, AppendStrPtr, ApndSz);

  return Str;
}

CHAR8 * ConvertToUpper (CHAR8* Str)
{
  CHAR8 *TempStr = Str;
  while (Str && *Str)
  {
    if ((*Str >= 'a') && (*Str <= 'z'))
      *Str = *Str - 0x20;
    ++Str;
  }
  return TempStr;
}


VOID
BuildDriverList(
  IN CONST CHAR8 *Drivers
  )
{
  CONST CHAR8 *Start, *End;

  if (Drivers == NULL)
    return;
  else if (*Drivers == 0)
    return;

  Start = End = Drivers;
  //check for multiple drivers
  while (Start != NULL)
  {
    if ((*End == DRIVER_DELIMITER_CHAR) || (*End == '\0'))
    {
      CopyMem(DrvrList[DrvrNum], Start, ((UINTN)End - (UINTN)Start)) ;
      Start = ++End;
      DrvrNum++;
      if (*End == '\0')
        break;
    }
    else{
      End++;
    }
  }
  DrvrListBuilt = 1;
}

CHAR8 *
EFIAPI
MatchDriverName(
  IN CONST CHAR8  *Command
  )
{
  CHAR8       *Rest = NULL;
  UINTN        Index =0;

  //Go up till DrvrNum so we don't compare empty string
  for (Index=0; Index < DrvrNum; Index++)
  {
    Rest = AsciiStrStr(Command, DrvrList[Index]);
    if (Rest)
      break;
  }
  return Rest;

}


/**
  Setup T32 practice script to enable host debug 

  If ImageContext is NULL, then return

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
SetupDebugEnv (IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  CHAR8 *ImgPathPtr;
  CHAR8 *TempCmdPtr;
  UINTN  StripParts;
  CHAR8* CmdPtr;
  UINTN  CmdLen;
  UINT64 LoadAddr;

  if (ImageContext == NULL || ImageContext->PdbPointer == NULL)
    return;

  if (InfoBlkPtr == NULL)
  {
    InfoBlkPtr = (UefiInfoBlkType*)GetInfoBlkPtr();
    if (InfoBlkPtr == NULL)
      return;
  }

  if (TempCmdString == NULL)
  {
    /* Use Allocatepages EFI_PAGE_SIZE for TEMP_STR_SZ */
    TempCmdString = AllocatePages(1);
    if(TempCmdString == NULL)
      return;
  }

  if (InfoBlkPtr->SymbolLoadCmdStrPtr == 0)
  {
    InfoBlkPtr->LoaderBreakPoint = (UINT64)BreakPointFunction;
    InfoBlkPtr->SymbolLoadCmdStrPtr = (UINT64)AllocatePages(ALLOC_PAGES);
    if (InfoBlkPtr->SymbolLoadCmdStrPtr == 0)
      return;
    InfoBlkPtr->SymbolLoadCmdStrSize = ALLOC_PAGES * EFI_PAGE_SIZE;
    
    SetMem((VOID *) InfoBlkPtr->SymbolLoadCmdStrPtr, InfoBlkPtr->SymbolLoadCmdStrSize, 0);

    CmdPtr = AsciiStrCat ((CHAR8*)InfoBlkPtr->SymbolLoadCmdStrPtr, 
        "global &ModuleSrcRoot\nglobal &ModuleObjRoot\nlocal &SymFile\n");
    CmdLen = 0;
    AlignStringSize(CmdPtr, &CmdLen);
    InfoBlkPtr->TotalCmdSize += (UINT32)CmdLen;
  }

  CmdPtr = (CHAR8*)InfoBlkPtr->SymbolLoadCmdStrPtr;
  LoadAddr = (ImageContext->ImageAddress + ImageContext->SizeOfHeaders);
  
  ImgPathPtr = SanitizeForSymbolPath(ImageContext->PdbPointer, UEFICORE_ANCHOR_STR, NULL);
  if (ImgPathPtr != NULL) {
    ImgPathPtr = SanitizeForSymbolPath(ImageContext->PdbPointer, SYMPATH_ANCHOR_STR, &StripParts);
    if (ImgPathPtr == NULL)
      return;

    /*
    * For each core module, add the following to the cmm script:
    * &SymFile="<path_to_dll>"
    * if (os.file("&SymFile"))
    * (
    *   data.load.elf "&SymFile" <addr> /nocode /noclear /strippart <strip>. 
    * )
    */
    AsciiSPrint (
      TempCmdString, 
      TEMP_STR_SZ, 
      "\n&SymFile=\"%a\"\nif (os.file(\"&SymFile\"))\n(\n  data.load.elf \"&SymFile\" 0x%012lx /nocode /noclear /strippart %d.\n)\n",
      ImgPathPtr, 
      LoadAddr,
	  StripParts 
      );
  }
  else {
    ImgPathPtr = SanitizeForSymbolPath(ImageContext->PdbPointer, SYMPATH_ANCHOR_STR, NULL);
    if (ImgPathPtr == NULL)
      return;

 /*
  * For each app pkg module, add the following to the cmm script:
  * &SymFile="<path_to_dll>"
  * if (os.file("&SymFile"))
  * (
  *   data.load.elf "&SymFile" <addr> /nocode /noclear /strippart "&ModuleSrcRoot" /strippart "&ModuleObjRoot" /sourcepath "."
  * )
  */
    AsciiSPrint (
      TempCmdString, 
      TEMP_STR_SZ, 
      "\n&SymFile=\"%a\"\nif (os.file(\"&SymFile\"))\n(\n  data.load.elf \"&SymFile\" 0x%012lx /nocode /noclear /strippart \"&ModuleSrcRoot\" /strippart \"&ModuleObjRoot\" /sourcepath \".\"\n)\n",
      ImgPathPtr, 
      LoadAddr 
      );
  }

  CmdLen = 0;
  AlignStringSize(TempCmdString, &CmdLen);

  /* Silently skip loading symbols if there is no more space */
  if (InfoBlkPtr->TotalCmdSize + CmdLen > InfoBlkPtr->SymbolLoadCmdStrSize) {
    return;
  }
 
  /* Remember start of cmd string so it can be removed during unload */
  InfoBlkPtr->LastCmdSize = InfoBlkPtr->TotalCmdSize;
  TempCmdPtr = CmdPtr + InfoBlkPtr->TotalCmdSize;

  /* Write T32 practice script changes to memory */
  InfoBlkPtr->TotalCmdSize += (UINT32)CmdLen;
  AsciiStrCat(CmdPtr, TempCmdString);
  WriteBackInvalidateDataCacheRange (TempCmdPtr, CmdLen);

  if (!DrvrListBuilt)
    BuildDriverList(InfoBlkPtr->DebugStopDriverStr);

  /* Check if current module matches string passed by host and enter breakpoint */
  AsciiSPrint(TempCmdString, TEMP_STR_SZ, "%a", ImgPathPtr);
  if (MatchDriverName(ConvertToUpper(TempCmdString)))
  {
    ExtractModuleName (ImgPathPtr, InfoBlkPtr->BkptModuleName);
    BreakPointFunction();
  }
}

STATIC
VOID
DebugEnvUnload (IN PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  CHAR8* TempCmdPtr;
  CHAR8* ImgPathPtr;
  CHAR8* Ptr;
  UINTN  CmdLen;

  if (ImageContext == NULL || ImageContext->PdbPointer == NULL)
    return;

  if (InfoBlkPtr == NULL)
  {
    InfoBlkPtr = (UefiInfoBlkType*)GetInfoBlkPtr();
    if (InfoBlkPtr == NULL)
      return;
  }

  if (InfoBlkPtr->TotalCmdSize == 0 || 
      InfoBlkPtr->TotalCmdSize == InfoBlkPtr->LastCmdSize) {
    return;
  }

  ImgPathPtr = SanitizeForSymbolPath(ImageContext->PdbPointer, SYMPATH_ANCHOR_STR, NULL);
  if (ImgPathPtr == NULL)
    return;

  TempCmdPtr = (CHAR8*)InfoBlkPtr->SymbolLoadCmdStrPtr + InfoBlkPtr->LastCmdSize;
  Ptr = AsciiStrStr (TempCmdPtr, ImgPathPtr);
  if (Ptr == NULL) {
    return;
  }

  CmdLen = InfoBlkPtr->TotalCmdSize - InfoBlkPtr->LastCmdSize;
  SetMem((VOID *) TempCmdPtr, CmdLen, 0);

  WriteBackInvalidateDataCacheRange (TempCmdPtr, CmdLen);

  if (InfoBlkPtr->CmdStartOffset == InfoBlkPtr->TotalCmdSize) {
    InfoBlkPtr->CmdStartOffset = InfoBlkPtr->LastCmdSize;
  }
  InfoBlkPtr->TotalCmdSize = InfoBlkPtr->LastCmdSize;  
}


STATIC VOID 
PrintEfiLoadInfo(PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  UINTN Index;
  UINTN StartIndex;
  CHAR8 EfiFileName[256];

  DEBUG((EFI_D_WARN,
        " - 0x%9p [%5d] ",
        (VOID *) (UINTN) ImageContext->ImageAddress,
        GetTimerCountms() ));
  //
  // Print Module Name by Pdb file path.
  // Windows and Unix style file path are all trimmed correctly.
  //
  if (ImageContext->PdbPointer != NULL) {
    StartIndex = 0;
    for (Index = 0; ImageContext->PdbPointer[Index] != 0; Index++) {
      if ((ImageContext->PdbPointer[Index] == '\\') || (ImageContext->PdbPointer[Index] == '/')) {
        StartIndex = Index + 1;
      }
    }
    //
    // Copy the PDB file name to our temporary string, and replace .pdb with .efi
    // The PDB file name is limited in the range of 0~255.
    // If the length is bigger than 255, trim the redudant characters to avoid overflow in array boundary.
    //
    for (Index = 0; Index < sizeof (EfiFileName) - 4; Index++) {
      EfiFileName[Index] = ImageContext->PdbPointer[Index + StartIndex];
      if (EfiFileName[Index] == 0) {
        EfiFileName[Index] = '.';
      }
      if (EfiFileName[Index] == '.') {
        EfiFileName[Index + 1] = 'e';
        EfiFileName[Index + 2] = 'f';
        EfiFileName[Index + 3] = 'i';
        EfiFileName[Index + 4] = 0;
        break;
      }
    }

    if (Index == sizeof (EfiFileName) - 4) {
      EfiFileName[Index] = 0;
    }
    DEBUG ((EFI_D_WARN, "%a", EfiFileName)); // &ImageContext->PdbPointer[StartIndex]));
  }
  DEBUG ((EFI_D_WARN, "\n"));
}

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
VOID
EFIAPI
PeCoffLoaderRelocateImageExtraAction (IN PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext)
{
  EFI_STATUS Status;
  if (ImageContext == NULL)
    return;

  PrintEfiLoadInfo(ImageContext);

  SetupDebugEnv (ImageContext);

#ifdef ENABLE_DEP_64
  Status = SetupDep (ImageContext);
  {
    if (Status != EFI_SUCCESS)
      DEBUG ((EFI_D_ERROR, "ERROR: Failed to enable DEP\n"));
  }
#endif
}

/**
  Performs additional actions just before a PE/COFF image is unloaded.  Any resources
  that were allocated by PeCoffLoaderRelocateImageExtraAction() must be freed.
  
  If ImageContext is NULL, then ASSERT().
  
  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that is being unloaded.

**/
VOID
EFIAPI
PeCoffLoaderUnloadImageExtraAction (
  IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext
  )
{
  EFI_STATUS Status;

  if (ImageContext == NULL) 
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Encountered NULL ImageContext\n"));
    return;
  }

#ifdef ENABLE_DEP_64
  Status = ClearDep (ImageContext);
  {
    if (Status != EFI_SUCCESS)
      DEBUG ((EFI_D_ERROR, "ERROR: Failed to enable DEP\n"));
  }
#endif  

  DebugEnvUnload (ImageContext);

}
