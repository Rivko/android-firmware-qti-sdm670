/** @file DppToolMain.c
   
  Qualcomm DppTool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  Portions Copyright (c) 2009 - 2012, Intel Corporation. All 
  rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/13   mic     Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/BlockIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
//#include <Protocol/FirmwareVolume2.h>

#include "../../Drivers/DppDxe/Dpp.h"
#include "CommonUtil.h"
#include "FatUtil.h"
#include "DppUtil.h"

static EFI_LOADED_IMAGE_PROTOCOL        *gLoadedImage  = NULL;
static EFI_HANDLE                        gFat16Handle  = NULL;
static EFI_GUID gEfiFat16Guid = 
  {0x543C031A, 0x4CB6, 0x4897, {0xBF, 0xFE, 0x4B, 0x48, 0x57, 0x68, 0xA8, 0xAD}};

typedef enum Operation_tag
{
  OP_UNKNOWN = 0,
  OP_LIST_PROVISION_DATA,
  OP_DUMP_QCOM_PROVISION_DATA,
  OP_SAVE_PROVISION_DATA,
  OP_DUMP_GENERIC_PROVISION_DATA
} Operation;

/* Define function pointer type for processing a command type */
typedef EFI_STATUS ProcFunc(IN UINTN argc, IN CHAR16 *argv[]);

/* Define function pointer type for displaying help for a command type */
typedef void HelpFunc(void);

/* Listing of available commands. This enumeration must be one-to-one mapped
   with argumentList[]. */
typedef enum
{
   ARG_UNKNOWN = 0,
   ARG_DELETE,
   ARG_WRITE,
   ARG_READ,
   ARG_ERASE_ALL,
   ARG_LIST_ALL
} ArgTypeEnum;


/* Structure to associate commands with their string, number of arguments required, and
   functions for processing the command and displaying help for the command. */
typedef struct
{
   ArgTypeEnum  argType;
   CHAR16      *argString;     /* C String to match against command line argument */
   UINTN        requiredCount; /* Required total number of arguments to be valid */
   ProcFunc    *procFunc;      /* Function to call to process the argument */
   HelpFunc    *printHelpFunc; /* Functin to call to print help for the argument */
} ArgMap;

/*=========================================================================== 
                         LOCAL FUNCTION DECLARATIONS
===========================================================================*/

/* Functions to process a given command type. */
static EFI_STATUS Process_Unknown(IN UINTN argc, IN CHAR16 *argv[]);
static EFI_STATUS Process_Delete(IN UINTN argc, IN CHAR16 *argv[]);
static EFI_STATUS Process_Write(IN UINTN argc, IN CHAR16 *argv[]);
static EFI_STATUS Process_Read(IN UINTN argc, IN CHAR16 *argv[]);
static EFI_STATUS Process_EraseAll(IN UINTN argc, IN CHAR16 *argv[]);
static EFI_STATUS Process_ListAll(IN UINTN argc, IN CHAR16 *argv[]);


/* Functions to display help for a given command type. */
static void Help_Help(void);
static void Help_Delete(void);
static void Help_Write(void);
static void Help_Read(void);
static void Help_EraseAll(void);
static void Help_ListAll(void);


/*=========================================================================== 
                         CONST DEFINITIONS
===========================================================================*/

#define TOOL_VERSION L"0.1"

/* The argumentList array must match the ArgTypeEnum list one-to-one, since the 
   function pointers are called from main() by directly indexing this array with 
   the enum value. */
const ArgMap argumentList[] =
{
   {ARG_UNKNOWN, L"", 0, Process_Unknown, Help_Help}, 
   {ARG_DELETE, L"/Delete", 4, Process_Delete, Help_Delete},
   {ARG_WRITE, L"/Write", 5, Process_Write, Help_Write},
   {ARG_READ, L"/Read", 4, Process_Read, Help_Read},
   {ARG_ERASE_ALL, L"/EraseAll", 2, Process_EraseAll, Help_EraseAll},
   {ARG_LIST_ALL, L"/List", 2, Process_ListAll, Help_ListAll},

};
/*=========================================================================== 
                         FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Display the name, copyright, and version banner to the user in the console
 *
 * @return       None
 * 
 */
static void PrintVersionBanner(void)
{
    AsciiPrint("\r\nDppTool.efi\r\nCopyright (c) 2013 QUALCOMM Incorporated. All Rights Reserved.\r\nVersion: %s\r\n\r\n", TOOL_VERSION);
}


static void PrintShortUsage(void)
{
   AsciiPrint("DppTool.efi /List  \r\n\
           | /EraseAll \r\n\
           | /Delete <DPP item type> <Dpp item name> \r\n\
           | /Write <DPP item type> <Dpp item name> <file name in PLAT partition> \r\n\
           | /Read <DPP item type> <Dpp item name> \r\n\
             \r\n");
}

static void PrintFullUsage(void)
{
   PrintVersionBanner();

   PrintShortUsage();
   
   AsciiPrint("DESCRIPTION:\r\n\
OPTIONS:\r\n\
    /List     - List all provisioned items\r\n\
    /EraseAll - Erase all provisioned items\r\n\
    /Delete   - Delete single provisioned item\r\n\
    /Write    - Write a file located in the root of PLAT partition to DPP\r\n\
    /Read     - Read a DPP item out to a file in provdump folder of PLAT partition\r\n\
\r\n");
}

static void Help_Help(void)
{
   PrintFullUsage();
}

static void Help_Delete(void)
{
   PrintVersionBanner();
   AsciiPrint("DppTool.efi /Delete <item_type> <item_name>\n");
}


static void Help_Write(void)
{
   PrintVersionBanner();
   AsciiPrint("DppTool.efi /Write <item_type> <item_name> <input_File>\n");
}

static void Help_Read(void)
{
   PrintVersionBanner();
   AsciiPrint("DppTool.efi /Read <item_type> <item_name>\n");
}

/**
 * Display detailed help information for the "/EraseAll" argument
 *
 * @return       None
 *
 * @sideeffects None
 */
static void Help_EraseAll(void)
{
   PrintVersionBanner();
   AsciiPrint("DppTool.efi /EraseAll\n");

}

/**
 * Display detailed help information for the "/ListAll" argument
 *
 * @return       None
 *
 * @sideeffects None
 */
static void Help_ListAll(void)
{
   PrintVersionBanner();
   AsciiPrint("DppTool.efi /ListAll\n");
}

/**
 * Scan the input arguments to the application and look for a matching argument string that is
 * present in argumentList global, and if a match is found, check that the number of
 * arguments passed is valid. Also returns whether the user requested help for the particular
 * argument type.
 *
 * @param[in]  argc  Input argument count
 *
 * @param[in]  arg2  Second argument which should contain the command request
 *
 * @param[in]  arg3  Third argument which may contain a help request "/?"
 *
 * @param[out] pArgType  Output enumeration matching the command requested, or ARG_UNKNOWN if not found
 *
 * @param[out] pbArgHelp  Boolean indicating if the user requested help for the particular command
 *
 * @return       None
 *
 * @sideeffects None
 */
static void ParseArgType(IN UINTN         argc, 
                         IN CHAR16       *arg2, 
                         IN CHAR16       *arg3, 
                         OUT ArgTypeEnum *pArgType, 
                         OUT BOOLEAN     *pbArgHelp)
{
   int i;
   BOOLEAN bWantsHelp = FALSE;

   /* Check for help options */
   if (argc == 3)
   {
      if ( StrCmp(arg3, L"/?") == 0)
      {
         bWantsHelp = TRUE;
      }
   }

   for (i = 0; i < (sizeof(argumentList)/sizeof(argumentList[0])); i++)
   {
      if ( StrCmp(arg2, argumentList[i].argString) == 0)
      {
         *pArgType = argumentList[i].argType;
         if ((bWantsHelp == TRUE) || (argc != argumentList[i].requiredCount))
         {
            *pbArgHelp = TRUE;
         }
         else
         { 
            *pbArgHelp = FALSE;
         }
         return;
      }
   }

   *pArgType = ARG_UNKNOWN;
   *pbArgHelp = FALSE;
}

static EFI_STATUS Process_Unknown(IN UINTN argc, IN CHAR16* argv[])
{
   EFI_STATUS Status = EFI_SUCCESS;

   AsciiPrint("  Unknown argument \n\n");

   return Status;
}

/**
 * Process the "/Provision" command argument.
 *
 * @param[in]  argc  Input argument count
 *
 * @param[in]  argv  Array of C string pointers for each input arguement
 *
 * @return       Return code following winerror.h (ERROR_SUCCESS, ...)
 *
 * @sideeffects None.
 */
static EFI_STATUS Process_Delete(IN UINTN argc, IN CHAR16* argv[])
{

  EFI_STATUS         Status = EFI_SUCCESS;
  CHAR16            *Type                         = argv[2];
  CHAR16            *DPPName                      = argv[3];

  if ((Type == NULL) || (DPPName == NULL) ||(argc != 4) )
  {   
    AsciiPrint("  EFI_INVALID_PARAMETER\n\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = DppItemDelete ( Type, DPPName);
  if (!EFI_ERROR(Status))
  {
    AsciiPrint("  operation success\n\n");
  }
  else
  {
    AsciiPrint("  operation failed\n\n");
  }
   return Status;
}

static EFI_STATUS Process_Write(IN UINTN argc, IN CHAR16* argv[])
{
  EFI_STATUS                       Status                       = EFI_SUCCESS;
  VOID                            *pBuffer                      = NULL;
  UINTN                            nBufferSize                  = 0;
  CHAR16                          *Type                         = argv[2];
  CHAR16                          *DPPName                      = argv[3];
  CHAR16                          *FileName                     = argv[4];

  if ((Type == NULL) || (DPPName == NULL) || (FileName == NULL) ||(argc != 5) )
  {   
    AsciiPrint("  EFI_INVALID_PARAMETER\n\n");
    return EFI_INVALID_PARAMETER;
  }
  
  /* read file */
  Status =  FatRead(
                    gFat16Handle, 
                  FileName, 
                  &pBuffer, 
                  &nBufferSize);
  HANDLE_ERROR_LABEL(Status, FatRead, Process_WriteExit); 

  /* write to DPP */ 
  Status = DppItemWrite ( Type, 
                        DPPName, 
                        pBuffer, 
                        nBufferSize);
  HANDLE_ERROR_LABEL(Status, DppItemWrite, Process_WriteExit);    
  


Process_WriteExit:

  if (!EFI_ERROR(Status))
  {
    AsciiPrint("  operation success\n\n");
  }
  else
  {
    AsciiPrint("  operation failed\n\n");
  }

  if(pBuffer != NULL)
  {
    gBS->FreePool(pBuffer);
      pBuffer = NULL;
  }
   return Status;

}

static EFI_STATUS Process_Read(IN UINTN argc, IN CHAR16* argv[])
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  VOID             *Buffer      = NULL;
  UINTN             BufferSize  = 0;
  CHAR16           *Type        = argv[2];
  CHAR16           *DPPName     = argv[3];
  CHAR16           *FilePath    = NULL;

  if ((Type == NULL) || (DPPName == NULL) || (argc != 4) )
  {   
    AsciiPrint("  EFI_INVALID_PARAMETER\n\n");
    return EFI_INVALID_PARAMETER;
  }

  /* read data from DPP */
  Status = DppItemRead (Type,DPPName,(UINT8**)&Buffer, &BufferSize);
  HANDLE_ERROR_LABEL(Status, DppItemRead, Process_ReadExit);  

  /* StrCat file name */
  FilePath = AllocateZeroPool(StrSize(Type) +
                              StrSize(DPPName) + 
                              StrSize(OUTPUT_DIR_NAME) +
                              3 * StrSize(L"\\") );
  if(FilePath == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Process_ReadExit;
  }
  StrCat(FilePath, L"\\");
  StrCat(FilePath, OUTPUT_DIR_NAME);
  StrCat(FilePath, L"\\");
  StrCat(FilePath, Type);
  StrCat(FilePath, L"_");
  StrCat(FilePath, DPPName);
  
  /* Write to FAT */
  Status = FatWrite( gFat16Handle,
                     FilePath,
                     Buffer,
                     BufferSize
                   );
  HANDLE_ERROR_LABEL(Status, FatWrite, Process_ReadExit); 

Process_ReadExit:

  if (!EFI_ERROR(Status))
  {
    AsciiPrint("  operation success\n\n");
  }
  else
  {
    AsciiPrint("  operation failed\n\n");
  }

  if (Buffer != NULL)
  {
    gBS->FreePool(Buffer);
    Buffer = NULL;
  }
  if (FilePath != NULL)
  {
    gBS->FreePool(FilePath);
    FilePath = NULL;
  }
  return Status;

}

/**
 * Display help information for "/?" command argument.
 *
 * @param[in]  argc  Input argument count
 *
 * @param[in]  argv  Array of C string pointers for each input arguement
 *
 * @return       Return code following winerror.h (ERROR_SUCCESS, ...)
 *
 * @sideeffects None.
 */
static EFI_STATUS Process_EraseAll(IN UINTN argc, IN CHAR16* argv[])
{

  EFI_STATUS Status = EFI_SUCCESS;

  Status = DppErase();

  if (!EFI_ERROR(Status))
  {
    AsciiPrint("  operation success\n\n");
  }
  else
  {
    AsciiPrint("  operation failed\n\n");
  }
  return Status;

}

/**
 * Process the "/ListAll" command argument.
 *
 * @param[in]  argc  Input argument count
 *
 * @param[in]  argv  Array of C string pointers for each input arguement
 *
 * @return       Return code following winerror.h (ERROR_SUCCESS, ...)
 *
 * @sideeffects None.
 */
static EFI_STATUS Process_ListAll(IN UINTN argc, IN CHAR16* argv[])
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = ListAllDppItems();
  if (!EFI_ERROR(Status))
  {
     AsciiPrint("  operation success\n\n");
  }
  else
  {
    AsciiPrint("  operation failed\n\n");
  }
   return Status;

}

/**
  return the next parameter from a command line string;

  This function moves the next parameter from Walker into TempParameter and moves
  Walker up past that parameter for recursive calling.  When the final parameter
  is moved *Walker will be set to NULL;

  Temp Parameter must be large enough to hold the parameter before calling this
  function.

  @param[in, out] Walker        pointer to string of command line.  Adjusted to
                                reminaing command line on return
  @param[in, out] TempParameter pointer to string of command line item extracted.

**/
VOID
EFIAPI
GetNextParameter(
  CHAR16 **Walker,
  CHAR16 **TempParameter
  )
{
  CHAR16 *NextDelim = NULL;
  CHAR16 *TempLoc = NULL;

  if (Walker == NULL ||
      *Walker == NULL ||
      TempParameter == NULL ||
      *TempParameter == NULL)
  {
    return;
  }
  
  //
  // make sure we dont have any leading spaces
  //
  while ((*Walker)[0] == L' ') {
      (*Walker)++;
  }

  //
  // make sure we still have some params now...
  //
  if (StrLen(*Walker) == 0) {
    ASSERT((*Walker)[0] == CHAR_NULL);
    *Walker = NULL;
    return;
  }

  //
  // we have a quoted parameter
  // could be the last parameter, but SHOULD have a trailing quote
  //
  if ((*Walker)[0] == L'\"') {
    NextDelim = NULL;
    for (TempLoc = *Walker + 1 ; TempLoc != NULL && *TempLoc != CHAR_NULL ; TempLoc++) {
      if (*TempLoc == L'^' && *(TempLoc+1) == L'\"') {
        TempLoc++;
      } else if (*TempLoc == L'\"') {
        NextDelim = TempLoc;
        break;
      }
    }

    if (NextDelim - ((*Walker)+1) == 0) {
      //
      // found ""
      //
      StrCpy(*TempParameter, L"");
      *Walker = NextDelim + 1;
    } else if (NextDelim != NULL) {
      StrnCpy(*TempParameter, (*Walker)+1, NextDelim - ((*Walker)+1));
      *Walker = NextDelim + 1;
    } else {
      //
      // last one... someone forgot the training quote!
      //
      StrCpy(*TempParameter, *Walker);
      *Walker = NULL;
    }
    for (TempLoc = *TempParameter ; TempLoc != NULL && *TempLoc != CHAR_NULL ; TempLoc++) {
      if (*TempLoc == L'^' && *(TempLoc+1) == L'\"') {
        CopyMem(TempLoc, TempLoc+1, StrSize(TempLoc) - sizeof(TempLoc[0]));
      }
    }
  } else {
    //
    // we have a regular parameter (no quote) OR
    // we have the final parameter (no trailing space)
    //
    NextDelim = StrStr((*Walker), L" ");
    if (NextDelim != NULL) {
      StrnCpy(*TempParameter, *Walker, NextDelim - (*Walker));
      (*TempParameter)[NextDelim - (*Walker)] = CHAR_NULL;
      *Walker = NextDelim+1;
    } else {
      //
      // last one.
      //
      StrCpy(*TempParameter, *Walker);
      *Walker = NULL;
    }
    for (NextDelim = *TempParameter ; NextDelim != NULL && *NextDelim != CHAR_NULL ; NextDelim++) {
      if (*NextDelim == L'^' && *(NextDelim+1) == L'^') {
        CopyMem(NextDelim, NextDelim+1, StrSize(NextDelim) - sizeof(NextDelim[0]));
      }
    }
    while ((*TempParameter)[StrLen(*TempParameter)-1] == L' ') {
      (*TempParameter)[StrLen(*TempParameter)-1] = CHAR_NULL;
    }
    while ((*TempParameter)[0] == L' ') {
      CopyMem(*TempParameter,
             (*TempParameter)+1,
             StrSize(*TempParameter) - sizeof((*TempParameter)[0]));
    }
  }
  return;
}

/**
  Function to populate Argc and Argv.

  This function parses the CommandLine and divides it into standard C style Argc/Argv
  parameters for inclusion in EFI_SHELL_PARAMETERS_PROTOCOL.  this supports space
  delimited and quote surrounded parameter definition.

  @param[in] CommandLine         String of command line to parse
  @param[in, out] Argv           pointer to array of strings; one for each parameter
  @param[in, out] Argc           pointer to number of strings in Argv array

  @return EFI_SUCCESS           the operation was sucessful
  @return EFI_OUT_OF_RESOURCES  a memory allocation failed.
**/
EFI_STATUS
EFIAPI
ParseCommandLineToArgs(
  IN CONST CHAR16 *CommandLine,
  IN OUT CHAR16 ***Argv,
  IN OUT UINTN *Argc
  )
{
  UINTN       Count = 0;
  CHAR16      *TempParameter = NULL;
  CHAR16      *Walker = NULL;
  CHAR16      *NewParam = NULL;
  UINTN       Size = 0;

  if (Argc == NULL || Argv == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (CommandLine == NULL || StrLen(CommandLine)==0) {
    (*Argc) = 0;
    (*Argv) = NULL;
    return (EFI_SUCCESS);
  }

  Size = StrSize(CommandLine);
  TempParameter = AllocateZeroPool(Size);
  if (TempParameter == NULL) {
    return (EFI_OUT_OF_RESOURCES);
  }

  for ( Count = 0
      , Walker = (CHAR16*)CommandLine
      ; Walker != NULL && *Walker != CHAR_NULL
      ; GetNextParameter(&Walker, &TempParameter)
      , Count++
     );

/*  Count = 0;
  Walker = (CHAR16*)CommandLine;
  while(Walker != NULL) {
    GetNextParameter(&Walker, &TempParameter);
    Count++;
  }
*/
  //
  // lets allocate the pointer array
  //
  (*Argv) = AllocateZeroPool((Count)*sizeof(CHAR16*));
  if (*Argv == NULL) {
    return (EFI_OUT_OF_RESOURCES);
  }

  *Argc = 0;
  Walker = (CHAR16*)CommandLine;
  while(Walker != NULL && *Walker != CHAR_NULL) {
    SetMem16(TempParameter, Size, CHAR_NULL);
    GetNextParameter(&Walker, &TempParameter);
    NewParam = AllocateZeroPool(StrSize(TempParameter));
    if (NewParam == NULL)
    {
      return EFI_OUT_OF_RESOURCES;
    }    
    StrCpy(NewParam, TempParameter);
    ((CHAR16**)(*Argv))[(*Argc)] = NewParam;
    (*Argc)++;
  }
  ASSERT(Count >= (*Argc));
  return (EFI_SUCCESS);
}
EFI_STATUS
GetPlatPartitionHandle()
{

  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    PartHandle = NULL;

  Status = GetPartitionHandle(&PartHandle,
                              &gEfiFat16Guid,
                              WP_FAT16_NAME,
                              &gEfiSimpleFileSystemProtocolGuid);
  if(!EFI_ERROR(Status) && (PartHandle != NULL))
  {
     gFat16Handle = PartHandle;

  }
  else if (Status == EFI_NOT_FOUND)
  {
    Status = GetPartitionHandle(&PartHandle,
                                &gEfiFat16Guid,
                                WA_FAT16_NAME,
                                &gEfiSimpleFileSystemProtocolGuid);
    if(!EFI_ERROR(Status) && (PartHandle != NULL))
    {
      gFat16Handle = PartHandle;
    }
    else
    {
      Status = EFI_UNSUPPORTED;
      AsciiPrint("Warning: PLAT partition doesn't exist \n");
      gFat16Handle = NULL;
      goto GetPlatPartitionHandleExit;
    }
  }
  else
  {
     Status = EFI_UNSUPPORTED;
     AsciiPrint("Warning:  couldn't get PLAT partition handle \n");
     goto GetPlatPartitionHandleExit;
  }

  GetPlatPartitionHandleExit:

  return Status;
}
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
DppToolMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status      = EFI_SUCCESS;
  CHAR16       *FullCmdLine = NULL;
  CHAR16      **Argv        = NULL;
  UINTN         Argc        = 0;
  ArgTypeEnum   argType     = ARG_UNKNOWN;
  BOOLEAN       bArgHelp    = FALSE;

  // Save a copy of ImageHandle
  gImageHandle = ImageHandle;

  // Get image info to parse command line
  Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID *)&gLoadedImage);
  if (EFI_ERROR(Status)) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to handle LoadedImage protocol! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ProvDumpMainExit;
  }

  if ( gLoadedImage->LoadOptions == NULL || gLoadedImage->LoadOptionsSize == 0 ) 
  {
    Status = EFI_INVALID_PARAMETER;
    goto ProvDumpMainExit;
  }

  FullCmdLine = gLoadedImage->LoadOptions;

  Status = ParseCommandLineToArgs(FullCmdLine, &Argv, &Argc);
  if (EFI_ERROR(Status) || Argv == NULL) 
  {
    AsciiPrint("ERROR: ProvDump - Failed to parse command line to args! "
               "Error Code: 0x%08X\r\n",
               Status);
    goto ProvDumpMainExit;
  }

  if(Argc < 2)
   {
      PrintFullUsage();
      Status = EFI_SUCCESS;
      goto ProvDumpMainExit;
  }

   ParseArgType(Argc, Argv[1], Argv[2], &argType, &bArgHelp);

   if (bArgHelp == TRUE)
   {
      /* Call Help_XXX() function for the given argument type. */
      
      argumentList[(unsigned int)argType].printHelpFunc();
      Status = EFI_SUCCESS;
   }
   else
   {
      /* Call Process_XXX() function for the given argument type. */
      Status = DppGvariableInit();
      HANDLE_ERROR_LABEL(Status, DppGvariableInit, ProvDumpMainExit);

      Status = GetPlatPartitionHandle();
      HANDLE_ERROR_LABEL(Status, GetPlatPartitionHandle, ProvDumpMainExit);

      Status = argumentList[(unsigned int)argType].procFunc(Argc, Argv);
   }
  
ProvDumpMainExit:

  return Status;
}

