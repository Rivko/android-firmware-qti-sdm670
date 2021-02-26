/** @file CmdLine.c
   
  Library to process and extract command line arguments.
  
  Copyright (c) 2013, Qualcomm Technologies, Inc. All rights reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  03/25/2013   yg      Treat no args as success condition (argc = 0)
  01/22/2013   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <Library/QcomLib.h>

#define MAX_ARGV_COUNT        16

#define DOUBLE_QUOTE_CHAR     0x22

/* Take the string, seperate command and arguments, typically only 2 of these */
STATIC
EFI_STATUS
ProcessCmdLineStr (CHAR8* CmdStr, UINTN CmdSize, UINTN *ArgcPtr, CHAR8 **ArgvPtr)
{
  CHAR8* EndPtr;
  BOOLEAN InDblQuotes = FALSE;

  if ((CmdStr == NULL) || (ArgcPtr == NULL) || (ArgvPtr == NULL) || (CmdSize == 0))
    return EFI_INVALID_PARAMETER;

  EndPtr = CmdStr + CmdSize;
  *ArgcPtr = 0;
  *ArgvPtr = NULL;

  while (1)
  {
    /* If we are past return */
    if ((CmdStr == NULL) || (CmdStr >= EndPtr) || (*CmdStr == 0))
      return EFI_SUCCESS;

    /* Remove all spaces */
    if ((*CmdStr == ' ') || (*CmdStr == '\t'))
    {
      *CmdStr = 0;
      ++CmdStr;
    }
    else
    {
      if (*CmdStr == DOUBLE_QUOTE_CHAR)
      {
        InDblQuotes = TRUE;
        ++CmdStr;
      }

      *ArgvPtr = CmdStr;
      ++ArgvPtr;
      ++(*ArgcPtr);

      /* Search for space char, go past valid chars */
      while (1)
      {
        if ((CmdStr == NULL) || (CmdStr >= EndPtr) || (*CmdStr == 0))
          return EFI_SUCCESS;

        if ((*CmdStr == ' ') || (*CmdStr == '\t'))
        {
          if (!InDblQuotes)
            break;
        }

        if ((*CmdStr == DOUBLE_QUOTE_CHAR) && InDblQuotes)
        {
          InDblQuotes = FALSE;
          *CmdStr = 0;
          ++CmdStr;
          break;
        }

        ++CmdStr;
      }
    }
  }
}

/**
  Extract the Command line arguments from the Image handle, process them and return
  in Argc, Argv format.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image to extract Cmd line
  @param[in] ArgcPtr        Pointer to Argc count variable.
  @param[in] ArgvArrayPtr   Pointer to Argv variable. If Argc>0 then Argv need to be freed using
                             FreePool call
  
  @retval EFI_SUCCESS           Successfully processed
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for the buffers
**/

EFI_STATUS
EFIAPI
GetCmdLineArgs (IN  EFI_HANDLE      ImageHandle,
                OUT UINTN          *ArgcPtr,
                OUT CHAR8        ***ArgvArrayPtr)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage  = NULL;
  CHAR8                       *AllocdMem = NULL;
  CHAR8                       *CmdStr, **ArgStrPtr;
  UINTN                       ArgsSize, AllocSize;
  BOOLEAN                     WideCharArg = FALSE;

  if ((ArgcPtr == NULL) || (ArgvArrayPtr == NULL) || (ImageHandle == NULL))
    return EFI_INVALID_PARAMETER;

  Status = gBS->HandleProtocol (ImageHandle, 
                                &gEfiLoadedImageProtocolGuid, (VOID *)&LoadedImage);
  if (EFI_ERROR(Status))
    return Status;
  
  if ((LoadedImage->LoadOptions == NULL) || (LoadedImage->LoadOptionsSize == 0))
  {
    *ArgcPtr = 0;
    *ArgvArrayPtr = NULL;
    return EFI_SUCCESS;
  }

  /* CmdStr will have to point to CHAR8* string,
   * if this contains WideChar then its handled below */
  CmdStr = (CHAR8*)LoadedImage->LoadOptions;

  AllocSize = sizeof(CHAR8*) * MAX_ARGV_COUNT;
  ArgsSize = AllocSize;

  /* See if we have to change from Unicode to Ascii
   * TODO: May need to revisit this
   *
   * Byte:  ASCII   Widechar
   *  0       t        t
   *  1       e        0  ->
   *  2       s        e  -> check this is not nul
   *  3       t        0  -> test these to be nul to determine if Ascii or widechar
   *
   * Single char case (Fails for one char Ascii, it depends on the junk after the string.
   *                   Possibly detect the single char using LoadOptionSize)
   *
   *  0       t        t
   *  1       0        0
   *  2       x        0
   *  3       x        0
   *
   *  */
  if ((CmdStr[1] == 0) && (CmdStr[2] != 0) && (CmdStr[3] == 0))
  {
    AllocSize += (StrLen ((CHAR16 *)CmdStr) + 2);
    WideCharArg = TRUE;
  }
  else
    AllocSize += (LoadedImage->LoadOptionsSize + 2);


  AllocdMem = AllocatePool (AllocSize);
  if (AllocdMem == NULL)
    return EFI_OUT_OF_RESOURCES;

  ArgStrPtr = (CHAR8**)AllocdMem;

  /* Caller should Free the memory we allocated */
  *ArgvArrayPtr = ArgStrPtr;

  if (WideCharArg)
    CmdStr = UnicodeStrToAsciiStr ((CHAR16 *)CmdStr, (AllocdMem + ArgsSize));
  else
    CmdStr = AsciiStrnCpy ((AllocdMem + ArgsSize), CmdStr, AllocSize - ArgsSize);

  /* Now CmdStr should have Ascii string, split them into Argc, Argv format */
  Status = ProcessCmdLineStr (CmdStr, LoadedImage->LoadOptionsSize, ArgcPtr, ArgStrPtr);

  if ((Status != EFI_SUCCESS) || (*ArgcPtr == 0))
    FreePool (AllocdMem);

#if 0
  {
    UINTN  i;
    AsciiPrint ("Cmd Line Params: %d\r\n", *ArgcPtr);
    for (i=0; i<*ArgcPtr; ++i)
      AsciiPrint ("%d : %a\r\n", i, ArgStrPtr[i]);
  }
#endif
  return Status;
}


