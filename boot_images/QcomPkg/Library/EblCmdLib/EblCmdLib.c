/** @file EblCmdLib.c
  Add custom commands for Qualcomm chipsets

  Copyright (c) 2010-2016, Qualcomm Technologies, Inc. All rights reserved.
  Portions copyright (c) 2008 - 2010, Apple Inc. All rights reserved.

  This program and the accompanying materials                          
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
 06/15/16   bd      Remove offlinecrashdump library dependency from QcomLib
 06/08/16   sj      Changed the mass storage mode from App
 12/02/16   vk      Add Dump GCD
 17/09/15   vk      Enable LinuxLoader
 17/03/14   vk      Enable fastboot
 12/11/14   bh      Added dbgfv cmd to LoadDebugFv
 07/08/14   vk      LLVM warning cleanup
 11/01/13   vk      Fix 64 bit compiler warnings
 10/15/13   vk      Make internal functions static
 07/11/13   yg      Add cmd history for later usage
 03/14/13   niting  Added dump of UefiSecApp log buffer
 02/14/13   vk      change delvars to removevars
 02/06/13   yg      Add options for Perf command
 01/09/13   vk      Fix KlockWork warnings
 12/18/12   yg      Display Load and Absolute time
 07/12/12   jz      Display performance using module name
 07/10/12   niting  Added delete all UEFI variables.
 06/12/12   niting  Added get/set var functionality
 04/10/12   jz      Fixed klockwork warnings
 07/07/11   niting  Added command to enter mass storage mode.
 05/27/11   niting  Removed excess performance measurements.
 02/04/11   niting  Added more performance printing.
 12/02/10   yg      Initial revision.

=============================================================================*/

#include <PiDxe.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/EblCmdLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/EfiFileLib.h>
#include <Library/ArmDisassemblerLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/PerformanceLib.h>
#include <Library/TimerLib.h>
#include <Library/QcomLib.h>
#include <Library/DxeServicesTableLib.h>

#include <Guid/DebugImageInfoTable.h>

#include <Protocol/DebugSupport.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EFIDebug.h>
#include <Library/GuidedFvLib.h>
#include <Library/OfflineCrashDump.h>


extern EFI_GUID gToolsFvGuid;


#if 0 //TIANOCORE UPDATE
extern UINTN
ArmDataCacheSets (
  VOID
  );

extern UINTN
EFIAPI
ArmInstructionCacheSets (
  VOID
  );

/**
  Dump cache info

  Argv[0] - cache

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblCacheInfo (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  UINT32 CCLIDR;

  if(ArmDataCachePresent()) {
    AsciiPrint ("Data Cache\n");
    AsciiPrint ("  Size %d\n", ArmDataCacheSize());
    AsciiPrint ("  Associativity %d\n", ArmDataCacheAssociativity());
    AsciiPrint ("  Sets %d\n", ArmDataCacheSets());
    AsciiPrint ("  Line Size %d\n", ArmDataCacheLineLength());
  }

  if(ArmInstructionCachePresent()) {
    AsciiPrint ("Instruction Cache\n");
    AsciiPrint ("  Size %d\n", ArmInstructionCacheSize());
    AsciiPrint ("  Associativity %d\n", ArmInstructionCacheAssociativity());
    AsciiPrint ("  Sets %d\n", ArmInstructionCacheSets());
    AsciiPrint ("  Line Size %d\n", ArmInstructionCacheLineLength());
  }

  CCLIDR        = ArmCacheArchitecture();
  AsciiPrint ("Cache Architecture\n");
  AsciiPrint ("  LoU 0x%x\n", (CCLIDR>>27) & 0x7);
  AsciiPrint ("  LoC 0x%x\n", (CCLIDR>>24) & 0x7);
  AsciiPrint ("  CL8 0x%x\n", (CCLIDR>>21) & 0x7);
  AsciiPrint ("  CL7 0x%x\n", (CCLIDR>>18) & 0x7);
  AsciiPrint ("  CL6 0x%x\n", (CCLIDR>>15) & 0x7);
  AsciiPrint ("  CL5 0x%x\n", (CCLIDR>>12) & 0x7);
  AsciiPrint ("  CL4 0x%x\n", (CCLIDR>>9) & 0x7);
  AsciiPrint ("  CL3 0x%x\n", (CCLIDR>>6) & 0x7);
  AsciiPrint ("  CL2 0x%x\n", (CCLIDR>>3) & 0x7);
  AsciiPrint ("  CL1 0x%x\n", CCLIDR & 0x7);

  return EFI_SUCCESS;
}
#endif

/**
  Simple arm disassembler via a library

  Argv[0] - symboltable
  Argv[1] - Optional qoted format string 
  Argv[2] - Optional flag

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblSymbolTable (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS                        Status;
  EFI_DEBUG_IMAGE_INFO_TABLE_HEADER *DebugImageTableHeader = NULL;
  EFI_DEBUG_IMAGE_INFO              *DebugTable;
  UINTN                             Entry;
  CHAR8                             *Format;
  CHAR8                             *Pdb;
  UINT32                            PeCoffSizeOfHeaders;
  UINTN                             ImageBase;
  BOOLEAN                           Elf;
  
  // Need to add lots of error checking on the passed in string
  // Default string is for RealView debugger
  Format = (Argc > 1) ? Argv[1] : "data.load.elf %a 0x%x /nocode /noclear";
  Elf = (Argc > 2) ? FALSE : TRUE;
  
  Status = EfiGetSystemConfigurationTable (&gEfiDebugImageInfoTableGuid, (VOID **)&DebugImageTableHeader);
  if (EFI_ERROR (Status) || (DebugImageTableHeader == NULL)) {
    return Status;
  }
  
  DebugTable = DebugImageTableHeader->EfiDebugImageInfoTable;
  if (DebugTable == NULL) {
    return EFI_SUCCESS;
  }

  for (Entry = 0; Entry < DebugImageTableHeader->TableSize; Entry++, DebugTable++) {
    if (DebugTable->NormalImage != NULL) {
      if ((DebugTable->NormalImage->ImageInfoType == EFI_DEBUG_IMAGE_INFO_TYPE_NORMAL) && (DebugTable->NormalImage->LoadedImageProtocolInstance != NULL)) {
        ImageBase = (UINTN) DebugTable->NormalImage->LoadedImageProtocolInstance->ImageBase;
        PeCoffSizeOfHeaders = PeCoffGetSizeOfHeaders ((VOID *)(UINTN)ImageBase);
        Pdb = PeCoffLoaderGetPdbPointer (DebugTable->NormalImage->LoadedImageProtocolInstance->ImageBase);
        if (Pdb != NULL) {
          if (Elf) {
            // ELF and Mach-O images don't include the header so the linked address does not include header
            ImageBase += PeCoffSizeOfHeaders;
          } 
          AsciiPrint (Format, Pdb, ImageBase);
          AsciiPrint ("\n");
        } else {
        }
      }
    }  
  }

  return EFI_SUCCESS;
}


/**
  Simple arm disassembler via a library

  Argv[0] - disasm
  Argv[1] - Address to start disassembling from
  ARgv[2] - Number of instructions to disassembly (optional)

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblDisassembler (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  UINT8   *Ptr, *CurrentAddress;
  UINT32  Address;
  UINT32  Count;
  CHAR8   Buffer[80];
  UINT32  ItBlock;
  
  if (Argc < 2) {
    return EFI_INVALID_PARAMETER;
  }
  
  Address = AsciiStrHexToUintn (Argv[1]);
  Count   = (Argc > 2) ? (UINT32)AsciiStrHexToUintn (Argv[2]) : 20;

  Ptr = (UINT8 *)(UINTN)Address;  
  ItBlock = 0;
  do {
    CurrentAddress = Ptr;
    DisassembleInstruction (&Ptr, TRUE, TRUE, &ItBlock, Buffer, sizeof (Buffer));
    AsciiPrint ("0x%08x: %a\n", CurrentAddress, Buffer);
  } while (Count-- > 0);
 

  return EFI_SUCCESS;
}


CHAR8 *
ImageHandleToPdbFileName (
  IN  EFI_HANDLE    Handle
  )
{
  EFI_STATUS                  Status;
  EFI_LOADED_IMAGE_PROTOCOL   *LoadedImage;
  CHAR8                       *Pdb;
  CHAR8                       *StripLeading;

  Status = gBS->HandleProtocol (Handle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (EFI_ERROR (Status) || (LoadedImage == NULL)) {
    return "";
  }

  Pdb = PeCoffLoaderGetPdbPointer (LoadedImage->ImageBase);
  if (Pdb == NULL)
  {
    return "";
  }
  StripLeading = AsciiStrStr (Pdb, "\\ARM\\");
  if (StripLeading == NULL) {
    StripLeading = AsciiStrStr (Pdb, "/ARM/");
    if (StripLeading == NULL) {
      return Pdb;
    }
  }
  // Hopefully we hacked off the unneeded part
  return (StripLeading + 5);
}


CHAR8 *mTokenList[] = {
  "SEC",
  "PEI",
  "DXE",
  "BDS",
  NULL
};

#define SIGNIFICANT_DURATION_MICRO_SEC     100

/**
  Simple arm disassembler via a library

  Argv[0] - disasm
  Argv[1] - Address to start disassembling from
  ARgv[2] - Number of instructions to disassembly (optional)

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblPerformance (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  UINTN       Key;
  CONST VOID  *Handle;
  CONST CHAR8 *Token, *Module;
  UINT64      Start, Stop, TimeStamp, TempMicroSec;
  UINT64      Delta, TicksPerSecond, Milliseconds, Microseconds;
  UINTN       Index, ShowAll, QLen, Major = 0, MajorDur = 0;

  TicksPerSecond = GetPerformanceCounterProperties (NULL, NULL);

  if (Argc > 1)
  {
    ShowAll = (AsciiStriCmp ("all", Argv[1]) == 0);  // Get "all" the perf entries

    if (AsciiStriCmp ("maj", Argv[1]) == 0)    // Gen only Major contributors (more than Significant duration cfg)
    {
      Major = TRUE;
      MajorDur = SIGNIFICANT_DURATION_MICRO_SEC;
      if (Argc > 2)
        MajorDur = AsciiStrDecimalToUintn (Argv[2]);
    }

    QLen = AsciiStrLen (Argv[1]);
    Key = 0;
    AsciiPrint ("\nAll units are in Micro Secs:\n"
                "  Start         End       TimeSpent     Token     Module\n"
                " ----------   ---------  -----------   --------------------\n");
    do {
      Key = GetPerformanceMeasurement (Key, (CONST VOID **)&Handle, &Token, &Module, &Start, &Stop);
      if (Key != 0)
      {
        UINT64 DeltauS, StartuS, StopuS;

        Delta = Stop - Start;

        DeltauS = 0;
        if (Stop != 0)
          DeltauS = DivU64x64Remainder (MultU64x32 (Delta, 1000000), TicksPerSecond, NULL);

        StartuS = DivU64x64Remainder (MultU64x32 (Start, 1000000), TicksPerSecond, NULL);
        StopuS = DivU64x64Remainder (MultU64x32 (Stop, 1000000), TicksPerSecond, NULL);

        /* Print if:
         * Show all is requested OR
         * Major contributor with default of specified threshold
         * or the Token matching string requested
         *  */
        if (ShowAll || (Major && (DeltauS > MajorDur)) ||
            (AsciiStrnCmp (Argv[1], Token, QLen) == 0))
          AsciiPrint ("%10ld  %10ld  %10ld    %a   %a\n", StartuS, StopuS, DeltauS, Token, Module);
      }
    } while (Key != 0);

    return EFI_SUCCESS;
  }
  
  Key       = 0;
  AsciiPrint ("\nAll units are in Micro Secs:\n"
              " TimeSpent    Absolute  Module (L: Load, S: Start/Init)\n"
              "----------    --------  -------------------------------\n");
  do {
    Key = GetPerformanceMeasurement (Key, (CONST VOID **)&Handle, &Token, &Module, &Start, &Stop);
    if (Key != 0) {
      if ((AsciiStriCmp ("StartImage:", Token) == 0)||(AsciiStriCmp ("LoadImage:", Token) == 0)) {
        if (Stop == 0) {
          // The entry for EBL is still running so the stop time will be zero. Skip it
          AsciiPrint ("   running  %a\n", Module);
        } else {
          Delta = Stop - Start;
          Microseconds = DivU64x64Remainder (MultU64x32 (Delta, 1000000), TicksPerSecond, NULL);
          TempMicroSec = DivU64x64Remainder (MultU64x32 (Start, 1000000), TicksPerSecond, NULL);
          AsciiPrint ("%10ld  %10ld  %a (%c)\n", Microseconds, TempMicroSec, Module, Token[0]);
        }
      }
    }
  } while (Key != 0);

  AsciiPrint ("\n");

  TimeStamp = 0;
  Key       = 0;
  do {
    Key = GetPerformanceMeasurement (Key, (CONST VOID **)&Handle, &Token, &Module, &Start, &Stop);
    if (Key != 0) {
      for (Index = 0; mTokenList[Index] != NULL; Index++) {
        if (AsciiStriCmp (mTokenList[Index], Token) == 0) {
          Delta = Stop - Start;
          TimeStamp += Delta;
          Milliseconds = DivU64x64Remainder (MultU64x32 (Delta, 1000), TicksPerSecond, NULL);
          AsciiPrint ("%6a %6ld ms\n", Token, Milliseconds);
          break;
        }
      }   
    }
  } while (Key != 0);

  AsciiPrint ("Total Time = %ld ms\n\n", DivU64x64Remainder (MultU64x32 (TimeStamp, 1000), TicksPerSecond, NULL));

  return EFI_SUCCESS;
}

/**
  Put the phone in mass storage mode

  Argv[0] - "massstorage"

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the command name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblMassStorageCmd (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{

    LaunchAppFromGuidedFv(&gToolsFvGuid, "UsbfnMsdApp", NULL);
    return EFI_UNSUPPORTED;
  
}

EFI_STATUS
EdlCmd (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{

  /* Should not return if supported */
  EnterEDLMode();

  return EFI_UNSUPPORTED;
}

#define IS_HYPHEN(a)               (a) == L'-'
#define IS_NULL(a)                 (a) == L'\0'
#define VAR_PRINT_LINE_LENGTH      16

/**
  Converts a list of string to a specified buffer.

  @param Buf             The output buffer that contains the string.
  @param BufferLength    The length of the buffer
  @param Str             The input string that contains the hex number

  @retval EFI_SUCCESS    The string was successfully converted to the buffer.

**/
STATIC EFI_STATUS
StrToBuf (
  OUT UINT8    *Buf,
  IN  UINTN    BufferLength,
  IN  CHAR16   *Str
  )
{
  UINTN       Index;
  UINTN       StrLength;
  UINT8       Digit;
  UINT8       Byte;

  Digit = 0;

  //
  // Two hex char make up one byte
  //
  StrLength = BufferLength * sizeof (CHAR16);

  for(Index = 0; Index < StrLength; Index++, Str++) {

    if ((*Str >= L'a') && (*Str <= L'f')) {
      Digit = (UINT8) (*Str - L'a' + 0x0A);
    } else if ((*Str >= L'A') && (*Str <= L'F')) {
      Digit = (UINT8) (*Str - L'A' + 0x0A);
    } else if ((*Str >= L'0') && (*Str <= L'9')) {
      Digit = (UINT8) (*Str - L'0');
    } else {
      return EFI_INVALID_PARAMETER;
    }

    //
    // For odd characters, write the upper nibble for each buffer byte,
    // and for even characters, the lower nibble.
    //
    if ((Index & 1) == 0) {
      Byte = (UINT8) (Digit << 4);
    } else {
      Byte = Buf[Index / 2];
      Byte &= 0xF0;
      Byte = (UINT8) (Byte | Digit);
    }

    Buf[Index / 2] = Byte;
  }

  return EFI_SUCCESS;
}

/**
  Converts a string to GUID value.
  Guid Format is xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx

  @param Str              The registry format GUID string that contains the GUID value.
  @param Guid             A pointer to the converted GUID value.

  @retval EFI_SUCCESS     The GUID string was successfully converted to the GUID value.
  @retval EFI_UNSUPPORTED The input string is not in registry format.
  @return others          Some error occurred when converting part of GUID value.

**/
STATIC EFI_STATUS
StrToGuid (
  IN  CHAR16   *Str,
  OUT EFI_GUID *Guid
  )
{
  //
  // Get the first UINT32 data
  //
  Guid->Data1 = (UINT32) StrHexToUint64  (Str);
  while (!(IS_HYPHEN (*Str)) && (!(IS_NULL (*Str)))) {
    Str ++;
  }
  
  if (IS_HYPHEN (*Str)) {
    Str++;
  } else {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Get the second UINT16 data
  //
  Guid->Data2 = (UINT16) StrHexToUint64  (Str);
  while ((!(IS_HYPHEN (*Str))) && (!(IS_NULL (*Str)))) {
    Str ++;
  }

  if (IS_HYPHEN (*Str)) {
    Str++;
  } else {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Get the third UINT16 data
  //
  Guid->Data3 = (UINT16) StrHexToUint64  (Str);
  while ((!(IS_HYPHEN (*Str))) && (!(IS_NULL (*Str)))) {
    Str ++;
  }

  if (IS_HYPHEN (*Str)) {
    Str++;
  } else {
    return EFI_UNSUPPORTED;
  }

  //
  // Get the following 8 bytes data
  //  
  StrToBuf (&Guid->Data4[0], 2, Str);
  //
  // Skip 2 byte hex chars
  //
  Str += 2 * 2;

  if (IS_HYPHEN (*Str)) {
    Str++;
  } else {
    return EFI_UNSUPPORTED;
  }
  StrToBuf (&Guid->Data4[2], 6, Str);

  return EFI_SUCCESS;
}

/**
  Displays the options for setvar and getvar

  Argv[0] - varoptions

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblVarOptions (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  AsciiPrint ("Guid should be in the following format:\r\n");
  AsciiPrint ("  xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx\r\n\r\n");

  AsciiPrint ("Attribute Options:\r\n");
  AsciiPrint ("  Abbreviations:\r\n");
  AsciiPrint ("  NV     = 0x00000001 = EFI_VARIABLE_NON_VOLATILE\r\n");
  AsciiPrint ("  BS     = 0x00000002 = EFI_VARIABLE_BOOTSERVICE_ACCESS\r\n");
  AsciiPrint ("  RT     = 0x00000004 = EFI_VARIABLE_RUNTIME_ACCESS\r\n");
  AsciiPrint ("  AUTH   = 0x00000020 = EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS\r\n");
  AsciiPrint ("  APPEND = 0x00000040 = EFI_VARIABLE_APPEND_WRITE\r\n");

  AsciiPrint("  0:   Delete a variable\r\n");
  AsciiPrint("  2:   BS\r\n");
  AsciiPrint("  3:   NV | BS\r\n");
  AsciiPrint("  6:   RT | BS\r\n");
  AsciiPrint("  7:   NV | RT | BS\r\n");
  AsciiPrint("  35:  AUTH | NV | BS\r\n");
  AsciiPrint("  37:  AUTH | NV | RT | BS\r\n");
  AsciiPrint("  66:  APPEND | BS\r\n");
  AsciiPrint("  67:  APPEND | NV | BS\r\n");
  AsciiPrint("  70:  APPEND | RT | BS\r\n");
  AsciiPrint("  71:  APPEND | NV | RT | BS\r\n");
  AsciiPrint("  99:  APPEND | AUTH | NV | BS\r\n");
  AsciiPrint("  101: APPEND | AUTH | NV | RT | BS\r\n");
  AsciiPrint ("\r\n");

  AsciiPrint ("setvarbytes Data Dormat:\r\n");
  AsciiPrint ("  setvarbytes MyVariable xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 7 8 0AA34B7C11459912\r\n");
  AsciiPrint ("  Result sets MyVariable with 8-byte value of 0x129945117C4BA30A - LSB is the first two character in the byte stream above\r\n");

  return EFI_SUCCESS;
}

/**
  Sets a UEFI Variable

  Argv[0] - setvar
  Argv[1] - name
  Argv[2] - guid
  Argv[3] - attributes
  Argv[4] - datasize
  Argv[5] - databytestream
  
  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblSetVariable (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHAR16 *VariableName = NULL;
  UINT32 VariableNameSize = 0;
  EFI_GUID VendorGuid;
  UINT32 VendorGuidUniStrSize = 0;
  CHAR16 *VendorGuidUniStr = NULL;
  UINT32 Attributes;
  UINT32 DataSize = 0;
  UINT8* Data = NULL;
  CHAR8 DataStr[3] = {0,0,0}; // used to store 2 characters and NULL
  UINT8 DataVal = 0;
  UINT32 DataIndex = 0;
  UINT32 Index = 0;
  UINT32 LineIndex = 0;
  UINT8 LineStr[VAR_PRINT_LINE_LENGTH + 1]; // hold 16 characters + NULL char

  do {
    if (Argc != 6)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
  
    /* Get name */
    VariableNameSize = (AsciiStrLen (Argv[1]) + 1) * sizeof (CHAR16);
    VariableName = AllocatePool(VariableNameSize);
    if (VariableName == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    AsciiStrToUnicodeStr(Argv[1], VariableName);

    /* Get Guid */
    VendorGuidUniStrSize = (AsciiStrLen (Argv[2]) + 1) * sizeof (CHAR16);
    VendorGuidUniStr = AllocatePool(VendorGuidUniStrSize);
    if (VendorGuidUniStr == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    AsciiStrToUnicodeStr(Argv[2], VendorGuidUniStr);

    StrToGuid(VendorGuidUniStr, &VendorGuid);

    /* Get Attributes */
    Attributes = AsciiStrDecimalToUintn(Argv[3]);

    /* Get DataSize */
    DataSize = AsciiStrDecimalToUintn(Argv[4]);

    if (DataSize > 0)
    {
      Data = AllocatePool(DataSize);
      if (Data == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      /* Prefill data buffer with # ascii character */
      SetMem(Data, DataSize, 0x23);

      /* Get Data */
      if (AsciiStrLen(Argv[5]) != 2*DataSize)
      {
        AsciiPrint("DataSize does not match size of data byte string \n");
        Status = EFI_INVALID_PARAMETER;
        break;      
      }
      for (Index = 0; Index < 2*DataSize; Index+=2)
      {
        DataStr[0] = Argv[5][Index];
        DataStr[1] = Argv[5][Index+1];
        
        DataVal = (UINT8)AsciiStrToHex(DataStr, sizeof(DataStr));
        Data[DataIndex++] = DataVal;
        DataVal = 0;
      }
    }
    else
    {
      /* Get NULL Data - data for deletion must be 0 */
      if ((AsciiStrLen(Argv[5]) != 1) && (Argv[5][0] != '0'))
      {
        AsciiPrint("ERROR: Must pass data byte stream as '0' when deleting a "
                   "variable or when datasize is 0\n");
        Status = EFI_INVALID_PARAMETER;
        break;      
      }
    }

    AsciiPrint("Set Variable:\n"
               "  Name: %s\n"
               "  Vendor Guid: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
               VariableName,
               VendorGuid.Data1, VendorGuid.Data2, VendorGuid.Data3,
               VendorGuid.Data4[0], VendorGuid.Data4[1], VendorGuid.Data4[2],
               VendorGuid.Data4[3], VendorGuid.Data4[4], VendorGuid.Data4[5],
               VendorGuid.Data4[6], VendorGuid.Data4[7]);
    AsciiPrint("  Attributes:%d\n"
               "  Data Size:%d\n",
               Attributes, 
               DataSize);

    Status = gRT->SetVariable(VariableName,
                              &VendorGuid,
                              Attributes,
                              DataSize,
                              Data);
    if (Status == EFI_SUCCESS) {
      AsciiPrint("  Data Set Successfully.\n");
      AsciiPrint("  Data Values in Hex and converted to ASCII:\n");
      Index = 0;
      LineStr[VAR_PRINT_LINE_LENGTH] = 0; // Setup NULL char
      SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
      while (Index < DataSize)
      {
        AsciiPrint("%02x ", Data[Index]);
        if ((Data[Index] < 0x20) || (Data[Index] > 0x7E))
        {
          LineStr[LineIndex] = '.'; // print a . for any unreadable chars
        }
        else
        {
          LineStr[LineIndex] = Data[Index];
        }

        LineIndex++;
        if (LineIndex == VAR_PRINT_LINE_LENGTH)
        {
          // Print ascii string
          AsciiPrint(" | %a\n", LineStr);
          SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
          LineIndex = 0;
        }

        Index++;
      }
      if (LineIndex > 0)
      {
        for (Index = LineIndex; Index < VAR_PRINT_LINE_LENGTH; Index++)
        {
          AsciiPrint("   "); // fill remaining chars with spaces
        }
        AsciiPrint(" | %a\n", LineStr);
      }
    }
    else
    {
      AsciiPrint("ERROR: Status = 0x%08x", Status);
    }

    AsciiPrint("\n");

  } while (0);

  if (Data != NULL)
  {
    FreePool(Data);
  }

  if (VariableName != NULL)
  {
    FreePool(VariableName);
  }

  if (VendorGuidUniStr != NULL)
  {
    FreePool(VendorGuidUniStr);
  }
  
  return Status;
}

/**
  Sets a UEFI Variable

  Argv[0] - setvarfile
  Argv[1] - name
  Argv[2] - guid
  Argv[3] - attributes
  Argv[4] - datasize
  Argv[5] - datafile
  
  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblSetVariableFromFile (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  return Status;
}

/**
  Gets a UEFI Variable

  Argv[0] - getvar 
  Argv[1] - name 
  Argv[2] - guid 
  Argv[3] - datasize 
   
  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line. 
                 Argv[0] is the comamnd name

  @return EFI_SUCCESS

**/
EFI_STATUS
EblGetVariable (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  CHAR16 *VariableName = NULL;
  UINT32 VariableNameSize = 0;
  EFI_GUID VendorGuid;
  UINT32 VendorGuidUniStrSize = 0;
  CHAR16 *VendorGuidUniStr = NULL;
  UINT32 Attributes;
  UINTN DataSize = 0;
  BOOLEAN DataSizeSpecified = TRUE;
  UINT8* Data = NULL;
  UINT32 Index = 0;
  UINT32 LineIndex = 0;
  UINT8 LineStr[VAR_PRINT_LINE_LENGTH + 1]; // hold 16 characters + NULL char

  do {
    if (Argc == 3)
    {
      // size not specified. default to 0 and allocate necessary buffer
      DataSize = 0;
      DataSizeSpecified = FALSE;
    }
    else if (Argc != 4)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    else
    {
      /* Get DataSize */
      DataSize = AsciiStrDecimalToUintn(Argv[3]);
    }
  
    /* Get name */
    VariableNameSize = (AsciiStrLen (Argv[1]) + 1) * sizeof (CHAR16);
    VariableName = AllocatePool(VariableNameSize);
    if (VariableName == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    AsciiStrToUnicodeStr(Argv[1], VariableName);

    /* Get Guid */
    VendorGuidUniStrSize = (AsciiStrLen (Argv[2]) + 1) * sizeof (CHAR16);
    VendorGuidUniStr = AllocatePool(VendorGuidUniStrSize);
    if (VendorGuidUniStr == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }
    AsciiStrToUnicodeStr(Argv[2], VendorGuidUniStr);

    StrToGuid(VendorGuidUniStr, &VendorGuid);
    if (DataSizeSpecified == FALSE)
    {
      Status = gRT->GetVariable(VariableName,
                                &VendorGuid,
                                &Attributes,
                                &DataSize,
                                Data);
      if (Status != EFI_BUFFER_TOO_SMALL)
      {
        break;
      }
    }

    if (DataSize > 0)
    {
      Data = AllocatePool(DataSize);
      if (Data == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      /* Prefill data buffer with # ascii character */
      SetMem(Data, DataSize, 0x23);
    }

    AsciiPrint("Get Variable:\n"
               "  Name: %s\n"
               "  Vendor Guid: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
               VariableName,
               VendorGuid.Data1, VendorGuid.Data2, VendorGuid.Data3,
               VendorGuid.Data4[0], VendorGuid.Data4[1], VendorGuid.Data4[2],
               VendorGuid.Data4[3], VendorGuid.Data4[4], VendorGuid.Data4[5],
               VendorGuid.Data4[6], VendorGuid.Data4[7]);
    AsciiPrint("  Data Buffer Size:%d\n", DataSize);

    Status = gRT->GetVariable(VariableName,
                              &VendorGuid,
                              &Attributes,
                              &DataSize,
                              Data);

    if (Status == EFI_SUCCESS)
    {
      AsciiPrint("  Attributes:%d\n"
                 "  Data Size:%d\n",
                 Attributes, 
                 DataSize);
      AsciiPrint("  Data Values in Hex and converted to ASCII:\n");
      Index = 0;
      LineStr[VAR_PRINT_LINE_LENGTH] = 0; // Setup NULL char
      SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
      while ((Index < DataSize) && (Data != NULL))
      {
        AsciiPrint("%02x ", Data[Index]);
        if ((Data[Index] < 0x20) || (Data[Index] > 0x7E))
        {
          LineStr[LineIndex] = '.'; // print a . for any unreadable chars
        }
        else
        {
          LineStr[LineIndex] = Data[Index];
        }

        LineIndex++;
        if (LineIndex == VAR_PRINT_LINE_LENGTH)
        {
          // Print ascii string
          AsciiPrint(" | %a\n", LineStr);
          SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
          LineIndex = 0;
        }

        Index++;
      }
      if (LineIndex > 0)
      {
        for (Index = LineIndex; Index < VAR_PRINT_LINE_LENGTH; Index++)
        {
          AsciiPrint("   "); // fill remaining chars with spaces
        }
        AsciiPrint(" | %a\n", LineStr);
      }
    }
    else if (Status == EFI_BUFFER_TOO_SMALL)
    {
      AsciiPrint("ERROR: Status = 0x%08x", Status);
      AsciiPrint("  Data Size Required:%d\n",
                 DataSize);
    }
    else
    {
      AsciiPrint("ERROR: Status = 0x%08x", Status);
    }

    AsciiPrint("\n");

  } while (0);

  if (Data != NULL)
  {
    FreePool(Data);
  }
  
  if (VariableName != NULL)
  {
    FreePool(VariableName);
  }

  if (VendorGuidUniStr != NULL)
  {
    FreePool(VendorGuidUniStr);
  }

  return Status;
}

/**
  Deletes all UEFI Variables

  @return EFI_SUCCESS

**/
EFI_STATUS
EblDeleteVariables (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  CHAR16                    VariableName[512];
  UINTN                     Size = 0;
  EFI_GUID                  VendorGuid;
  UINT8                     *DataBuffer = NULL;
  UINTN                     DataSize = 0; /* for deletion*/
  UINT32                    Attributes = 0; /* for deletion */
  UINT32                    VarCount = 0;
  
  AsciiPrint("Deleting Storage Tables:\n");
  for (VarCount = 0;;VarCount++)
  {
    Size      = 512; /* block size in bytes */
    DataSize  = 0;
    if (Status == EFI_SUCCESS)
    {
      /* If a variable was deleted, then reset the name
         NOTE: this may result in many failed attempts to
         delete secure variables if PK is already set since
         secure variables cannot be deleted unless a signed
         NULL data is passed       
      */
      VariableName[0] = 0x0000;
    }

    Status = gRT->GetNextVariableName(&Size, VariableName, &VendorGuid);
    if (Status == EFI_NOT_FOUND) {
      AsciiPrint("\nDeleting complete\n");
      Status = EFI_SUCCESS;
      break;
    }

    AsciiPrint("Index: %d Name: %s\n"
               "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
               "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
               "  Name Size: %d\n",
               VarCount,
               VariableName,
               VendorGuid.Data1, VendorGuid.Data2, VendorGuid.Data3,
               VendorGuid.Data4[0], VendorGuid.Data4[1], VendorGuid.Data4[2],
               VendorGuid.Data4[3], VendorGuid.Data4[4], VendorGuid.Data4[5],
               VendorGuid.Data4[6], VendorGuid.Data4[7],
               Size);

    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to get next name with status: 0x%08x\n", Status);
      break;
    }

    Status = gRT->SetVariable(VariableName, 
                              &VendorGuid, 
                              Attributes, 
                              DataSize, 
                              DataBuffer);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("  ERROR: Failed to delete variable; Status = 0x%08x\n", Status);
    }
    else
    {
      AsciiPrint("  Variable successfully deleted\n");
    }
  }

  return Status;
}

#define LOG_BUFFER_SIZE 4096

/**
  Dumps UefiSecApp Log Buffer.

  @return EFI_SUCCESS

**/
EFI_STATUS
EblGetDebugLogs (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32 BufferSize = LOG_BUFFER_SIZE + 1; /* 1 extra for NULL char*/
  CHAR8 *Buffer = AllocatePool(BufferSize);
  CHAR16 *OutputBuffer = AllocatePool(2*BufferSize);
  UINT32 Index = 0;
  EFI_QCOMDEBUG_PROTOCOL *QcomDebugProtocol = NULL;

  if ((Buffer == NULL)||(OutputBuffer == NULL))
  {
    DEBUG(( EFI_D_ERROR, "ERROR: Could not allocate space for dump buffer\r\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem(Buffer, BufferSize, 0x00);

  Status = gBS->LocateProtocol(&gEfiQcomDebugProtocolGuid, 
                               NULL,
                               (VOID**) &QcomDebugProtocol);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "WARNING: Could not find protocol to dump buffer\r\n"));
    goto Exit;
  }
  else if (QcomDebugProtocol == NULL)
  {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  Status = QcomDebugProtocol->GetDebugLog(QcomDebugProtocol,
                                          (UINT8*)Buffer,
                                          &BufferSize,
                                          EFI_DEBUG_LOG_UEFISECAPP);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "ERROR: Failed to get debug log. Status = 0x%08x\r\n", Status));
    goto Exit;
  }

  /* Convert all NULL chars to spaces except for last */
  for (Index = 0; Index < BufferSize; Index++)
  {
    if(Buffer[Index] == 0)
    {
      Buffer[Index] = ' ';
    }
  }
  Buffer[BufferSize] = 0;
  AsciiStrToUnicodeStr(Buffer, OutputBuffer);

  gST->ConOut->OutputString(gST->ConOut, L"UefiSecApp Log Buffer:\r\n");
  gST->ConOut->OutputString(gST->ConOut, OutputBuffer);
  gST->ConOut->OutputString(gST->ConOut, L"\r\nExiting\r\n");

Exit:
  if (Buffer)
    FreePool(Buffer);
  if (OutputBuffer)
    FreePool(OutputBuffer);

  return Status;
}

EFI_STATUS
EblLoadDebugFv (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS Status;
  Status = LoadDebugFv();
  return Status;
}


#define EFI_MEMORY_PORT_IO  0x4000000000000000ULL

STATIC VOID
PrintCurrentAttributes (EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap)
{
#if 0  
  if (MemorySpaceMap->Attributes & EFI_MEMORY_PORT_IO)
      AsciiPrint (" IO");
  else
      AsciiPrint (" --");
#endif

  if (MemorySpaceMap->Attributes & EFI_MEMORY_RUNTIME)
      AsciiPrint (" RT");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_NV)
      AsciiPrint (" NV");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_XP)
      AsciiPrint (" XP");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_RP)
      AsciiPrint (" RP");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_WP)
      AsciiPrint (" WP");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_UCE)
      AsciiPrint (" UCE");
  else
      AsciiPrint (" ---");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_WB)
      AsciiPrint (" WB");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_WT)
      AsciiPrint (" WT");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_WC)
      AsciiPrint (" WC");
  else
      AsciiPrint (" --");

  if (MemorySpaceMap->Attributes & EFI_MEMORY_UC)
      AsciiPrint (" UC");
  else
      AsciiPrint (" --");
}

STATIC VOID
PrintGcdMemoryType (EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap)
{
  if (MemorySpaceMap->GcdMemoryType == EfiGcdMemoryTypeNonExistent)
      AsciiPrint (" GcdMemNonExistent\t");

  else if (MemorySpaceMap->GcdMemoryType == EfiGcdMemoryTypeReserved)
      AsciiPrint (" GcdMemReserved\t");

  else if (MemorySpaceMap->GcdMemoryType == EfiGcdMemoryTypeSystemMemory)
      AsciiPrint (" GcdSystemMemory\t");

  else if (MemorySpaceMap->GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo)
      AsciiPrint (" GcdMemMmapIo\t");

  else if (MemorySpaceMap->GcdMemoryType == EfiGcdMemoryTypePersistentMemory)
      AsciiPrint (" GcdMemPersistent\t");
  else
      AsciiPrint (" ????????????\t");
}

EFI_STATUS
EblDumpGcd (
  IN UINTN  Argc,
  IN CHAR8  **Argv
  )
{
  EFI_STATUS                        Status;
  UINTN                             NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR   *MemorySpaceMap;
  EFI_GCD_IO_SPACE_DESCRIPTOR       *IoSpaceMap;
  UINTN                             i;
  UINTN                             PrintCount = 0;
  UINTN                             SkipType = 100; /*Skip None*/

  if (Argc > 1)
  {
    if (AsciiStrCmp (Argv[1], "-skiptype") == 0)
    {
      SkipType = AsciiStrDecimalToUintn (Argv[2]);

      if (SkipType > EfiGcdMemoryTypeMaximum)
      {
        AsciiPrint ("Invalid skiptype value %d !\n", SkipType);
        return EFI_INVALID_PARAMETER;
      }
    }
  }

  Status = gDS->GetMemorySpaceMap(&NumberOfDescriptors,&MemorySpaceMap);
  if (EFI_ERROR (Status)) {
      return Status;
  }
  AsciiPrint ("-----------------------------------------------------------------------------------------------------------------------------------------------------\n");
  AsciiPrint ("    Count  Address Range                       # pages     Cur. Attributes GMT [Current Attributes Decoded  ] Image    Device   [GMT - GcdMemoryType]\n");
  AsciiPrint ("-----------------------------------------------------------------------------------------------------------------------------------------------------\n");
  
  PrintCount = 0;
  for (i=0; i < NumberOfDescriptors; i++)
  {
    if (SkipType == MemorySpaceMap[i].GcdMemoryType)
      continue;

    PrintCount++;

    AsciiPrint ("MEM [%3d]  %016lx - %016lx # %08lx", PrintCount, 
                                                      (UINT64)MemorySpaceMap[i].BaseAddress,
                                                      MemorySpaceMap[i].BaseAddress+MemorySpaceMap[i].Length-1, 
                                                      (RShiftU64(MemorySpaceMap[i].BaseAddress+MemorySpaceMap[i].Length - MemorySpaceMap[i].BaseAddress, 12) ) );
    AsciiPrint (" %016lx %d ",MemorySpaceMap[i].Attributes, MemorySpaceMap[i].GcdMemoryType);
    
    PrintCurrentAttributes (&MemorySpaceMap[i]);
    AsciiPrint (" %08x %08x", MemorySpaceMap[i].ImageHandle,MemorySpaceMap[i].DeviceHandle);
    PrintGcdMemoryType (&MemorySpaceMap[i]);

    AsciiPrint ("\n");
  }

  AsciiPrint ("\nMemorySpace Map Entry Total Count = %d, Print Count = %d \n", i, PrintCount);

  FreePool (MemorySpaceMap);

  Status = gDS->GetIoSpaceMap(&NumberOfDescriptors,&IoSpaceMap);
  if (EFI_ERROR (Status)) {
      return Status;
  }

  PrintCount = 0;
  for (i=0; i < NumberOfDescriptors; i++)
  {
    PrintCount++;

    AsciiPrint ("IO  [%3d]  %016lx - %016lx", PrintCount, IoSpaceMap[i].BaseAddress,IoSpaceMap[i].BaseAddress+IoSpaceMap[i].Length);
    AsciiPrint ("                                                    %08x %08x",IoSpaceMap[i].ImageHandle,IoSpaceMap[i].DeviceHandle);

    if (IoSpaceMap[i].GcdIoType & EfiGcdMemoryTypeNonExistent)
        AsciiPrint (" TYPE_NONEXISTENT");
    if (IoSpaceMap[i].GcdIoType & EfiGcdMemoryTypeReserved)
        AsciiPrint (" TYPE_RESERVED");
    if (IoSpaceMap[i].GcdIoType & EfiGcdIoTypeIo)
        AsciiPrint (" TYPE_IO");

    AsciiPrint ("\n");
  }
  AsciiPrint ("\nIoSpace Map Entry Total Count = %d, Print Count = %d \n", i, PrintCount);

  FreePool (IoSpaceMap);
  AsciiPrint ("----------------------------------------------------------------------------------------------------------------------------------------------\n");

  return EFI_SUCCESS;
}


GLOBAL_REMOVE_IF_UNREFERENCED const EBL_COMMAND_TABLE mLibCmdTemplate[] =
{
  {
    "disasm address [count];",
    " disassemble count instructions",
    NULL,
    EblDisassembler
  },
  {
    "performance;",
    " Display boot performance info",
    NULL,
    EblPerformance
  },
  {
    "symboltable [\"format string\"] [PECOFF];",
    " show symbol table commands for debugger",
    NULL,
    EblSymbolTable
  },
#if 0 //TIANOCORE_UPDATE
  {
    "cache;",
    " Display cpu cache info",
    NULL,
    EblCacheInfo
  },
#endif  
  {
    "massstorage;",
    " Enter mass storage mode if supported",
    NULL,
    EblMassStorageCmd
  },
  {
    "edl;",
    " Enter edl storage mode if supported",
    NULL,
    EdlCmd
  },
  {
    "varoptions;",
    " Displays the list of numbered options for setvar and getvar commands",
    NULL,
    EblVarOptions
  },
  {
    "setvarbytes name guid attribute datasize databytestream;",
    " Sets a UEFI variable based on attributes - data must be represented as a byte stream\r\n    Use varoptions command to see list of options for guid and attributes",
    NULL,
    EblSetVariable
  },
  {
    "setvarfile name guid attribute datasize datafile;",
    " Sets a UEFI variable based on attributes - data will be read from the datafile\r\n    Use varoptions command to see list of options for guid and attributes",
    NULL,
    EblSetVariableFromFile
  },
  {
    "getvar name guid datasize;",
    " Gets and displays a UEFI variable\r\n    Use varoptions command to see list of options for guid and attributes",
    NULL,
    EblGetVariable
  },
  {
    "removevars;",
    " Deletes all UEFI variables - does not attempt to delete secure variables if PK is set\r\n",
    NULL,
    EblDeleteVariables
  },
  {
    "dumplogs;",
    " Dumps UefiSecApp Log Buffer to console\r\n",
    NULL,
    EblGetDebugLogs
  },
  {
    "dumpgcd",
    " Dump Global Coherency Domain",
    NULL,
    EblDumpGcd
  },  
  {
    "dbgfv",
    " Loads the debug FV regions",
    NULL,
    EblLoadDebugFv
  }
};

VOID SetCmdHistory (CHAR8* CmdLine);

VOID
EblInitializeExternalCmd (
  VOID
  )
{
  EblAddCommands (mLibCmdTemplate, sizeof (mLibCmdTemplate)/sizeof (EBL_COMMAND_TABLE));

  /* Add commands to history so that we can execute them with just arrow keys */
  SetCmdHistory ("exit");                         // Continue to boot to HLOS

  SetCmdHistory ("start fv2:\\Shell");            // Launch UEFI 2.0 Shell

  // Last added is the quickest to get to
  SetCmdHistory ("start fv1:\\LinuxFdtLoader");   // Launch LinuxLoader
  SetCmdHistory ("start fv1:\\fastboot");         // Launch fastboot

  // Last added is the quickest to get to
  SetCmdHistory ("start fv1:\\menu");             // Launch Menu

  return;
}
