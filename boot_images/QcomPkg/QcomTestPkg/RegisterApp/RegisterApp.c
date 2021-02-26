/** @file RegisterApp.c
   
  Register read/write test application/utility

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/06/14   ML      Initial revision

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
     Local Function Declarations
==========================================================================*/

/* Display help text */
static void PrintHelp(void);

/* Display help text */
static EFI_STATUS ParseCommandLine(EFI_HANDLE ImageHandle,
                                         boolean    *pbRead,
                                         uint32     *puStartingAddress,
                                         uint32     *puNumberofDWords,
                                         uint32     *puValue,
                                         uint32     *puWidth);


/*=========================================================================
     Global Functions
==========================================================================*/

/**
  The user etry point for the display application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
RegisterAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS   eStatus = EFI_SUCCESS;

  UINTN        Argc;
  CHAR8      **Argv;

  boolean      bRead;
  uint32       uStartingAddress  = 0;
  uint32       uRange = 0;
  uint32       uValue = 0;
  uint32       uWidth = 0;
  uint32       uIndex;
  uint32       uCount;

  AsciiPrint("RegisterApp v1.0\nCopyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.\n\n");  

  // Get the command line arguments
  if (EFI_SUCCESS != ParseCommandLine(ImageHandle, &bRead, &uStartingAddress, &uRange, &uValue, &uWidth))
  {
    return eStatus;
  }

  if (bRead)
  {
    uint32 uColumn;  
    uint32 uStartOffset = (uStartingAddress & 0xFFFFFFFC); // Align to a dword

    AsciiPrint("___________|" );    
    for (uColumn=0;uColumn<uWidth;uColumn++)
    {
      AsciiPrint("____0x%02x___|", uColumn*4);
    }
    AsciiPrint("\n");

    while (uRange > 0)
    {
      AsciiPrint("0x%08x :", uStartOffset);

      for (uColumn=0;((uColumn<uWidth) && (uRange > 0));uColumn++)
      {
        volatile uint32 *pRegAddr = (uint32*)uStartOffset;
        uint32   uRegVal;

        uRegVal = *pRegAddr;
        AsciiPrint(" 0x%08x ", uRegVal);

        uStartOffset+=sizeof(uint32);
        uRange--;
      }
      if (uRange>0)
      {
        AsciiPrint("\n");      
      }
    }

    AsciiPrint("\n\n");
  }
  else
  {
    uint32           uStartOffset = (uStartingAddress & 0xFFFFFFFC); // Align to a dword
    volatile uint32 *pRegAddr = (uint32*)uStartOffset;

    if (uRange<=1)
    {
      AsciiPrint("Writing to 0x%08x to physical address 0x%08x...", uValue, uStartOffset);

      *pRegAddr = uValue;
      
      AsciiPrint("...Done!\n\n");
      
    }
    else
    {
      uint32            uCount;
      uint32            uIterator = (uRange+9) / 10; // Used to print progress
      uint32            uProgress = 0;
      
      AsciiPrint("Writing to 0x%08x to physical address 0x%08x--0x%08x : ", uValue, uStartOffset, uStartOffset+((uRange-1)*4));

      for (uCount=0;uCount<uRange;uCount++)
      {
        *pRegAddr++ = uValue;
        
        uProgress++;
        if (uProgress >= uIterator)
        {
            AsciiPrint(".");
            uProgress = 0;
        }
      }

      AsciiPrint("...Done!\n\n");
    }
  }


  return eStatus;
}


/*=========================================================================
     Local Functions
==========================================================================*/

uint32 StringToInt(CHAR8 *String)
{
  uint32 uValue = 0;

  if (AsciiStrSize(String) > AsciiStrSize("0xFFFFFFFF"))
  {
    // Invalid hex number
    uValue = 0;
  }
  else if ((NULL != AsciiStrStr(String, "0x")) ||
           (NULL != AsciiStrStr(String, "0X")))
  {
    uValue = (uint32) AsciiStrHexToUintn(String);  
  }
  else
  {
    uValue = (uint32) AsciiStrDecimalToUintn(String);  
  }
    
  return uValue;
}


/* Display help text */
static EFI_STATUS ParseCommandLine(EFI_HANDLE ImageHandle,
                                         boolean    *pbRead,
                                         uint32     *puStartingAddress,
                                         uint32     *puNumberofDWords,
                                         uint32     *puValue,
                                         uint32     *puWidth)
{
  EFI_STATUS   eStatus = EFI_SUCCESS;

  UINTN        Argc;
  CHAR8      **Argv;
  UINTN        ArgIndex = 0;
  uint32       uStartingAddress  = 0;
  uint32       uRange = 0;
  uint32       uValue = 0;
  uint32       uWidth = 4;

  eStatus = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  if((EFI_SUCCESS != eStatus) || (0 == Argc))
  {
    PrintHelp();  
    return EFI_INVALID_PARAMETER;
  }

  // Check if this is a read or write operation
  if ((0==AsciiStriCmp(Argv[ArgIndex], "-h"))  ||
      (0==AsciiStriCmp(Argv[ArgIndex], "--h")) ||
      (0==AsciiStriCmp(Argv[ArgIndex], "-?")))
  {
    PrintHelp();  
    return EFI_INVALID_PARAMETER;
  }
  else if (0==AsciiStriCmp(Argv[ArgIndex], "read"))
  {
    *pbRead = 1;
  }
  else if (0==AsciiStriCmp(Argv[ArgIndex], "write"))
  {
    *pbRead = 0;
  }
  else
  {
    AsciiPrint("Error: Invalid argument %s", Argv[ArgIndex]);
    return EFI_INVALID_PARAMETER;
  }

  // Move up the index
  ArgIndex++;

  if (*pbRead)
  {
    uStartingAddress = StringToInt(Argv[ArgIndex]);

    if (uStartingAddress == 0)
    {
      AsciiPrint("Error: Staring address invalid or missing, cannot use zero as a starting address!\n");
      return EFI_INVALID_PARAMETER;
    }

    // Parse the optional range
    if (Argc >= 3)
    {
      ArgIndex++;
      uRange =  StringToInt(Argv[ArgIndex]);
    }

    // Parse the optional width
    if (Argc >= 4)
    {
      ArgIndex++;
      uWidth = StringToInt(Argv[ArgIndex]);

      // Limit the allowed ranges
      if (uWidth<1) uWidth=1;
      else if (uWidth > 16) uWidth=16;
    }

  }
  else
  {
    // Write must be at least 2 parameters
    if (Argc < 2)
    {
      PrintHelp();  
      return EFI_INVALID_PARAMETER;
    }

    uStartingAddress =  StringToInt(Argv[ArgIndex]);

    if (uStartingAddress == 0)
    {
      AsciiPrint("Error: Staring address invalid or missing, cannot use zero as a starting address!\n");
      return EFI_INVALID_PARAMETER;
    }    

    // Parse the value
    ArgIndex++;
    uValue =  StringToInt(Argv[ArgIndex]);

    // Parse the optional range
    if (Argc > 3)
    {
      uint32 uMaxRange = 5*1024*1024;
        
      ArgIndex++;
      uRange =  StringToInt(Argv[ArgIndex]);

      if (uRange > uMaxRange)
      {
        AsciiPrint("Warning: Range too larget, will be truncated to 5MB\n");
      }

      if (uRange<1) uRange=1;
      else if (uRange > uMaxRange) uRange = uMaxRange;      
    }

  }


  *puStartingAddress = uStartingAddress;
  *puNumberofDWords  = uRange;
  *puValue           = uValue;
  *puWidth           = uWidth;

  // For debugging
  // AsciiPrint("Params: Read:%d   Start:0x%08x   Range: %d    Value: 0x%08x   Width: %d\n\n", *pbRead, uStartingAddress, uRange, uValue, uWidth);
  
  return eStatus;
}


/* Display help text */
static void PrintHelp(void)
{
  AsciiPrint("Usage:\n");
  AsciiPrint("   RegisterApp read  [address] [range] [width]   : Read a register or a range of registers.\n\n");
  AsciiPrint("      [address]  - 32bit stating physical address.  ex 0xF9C00000\n");
  AsciiPrint("      [range]    - optional, number of registers (DWORDS) to read. Default is 1.\n");
  AsciiPrint("      [width]    - optional, how many columns of DWORDS to print at a time.  Default is 4.\n\n");
  AsciiPrint("    Examples:\n");
  AsciiPrint("      RegisterApp read 0xFD900000 1       - Read 1 DWORD register at 0xFD900000\n");
  AsciiPrint("      RegisterApp read 0xFD900000 32 8    - Read 32 DWORD registers at 0xFD900000 and print them as 8 column\n\n\n");
  
  AsciiPrint("   RegsiterApp write [address] [value] [range]   : write a register with a 32bit value.\n");
  AsciiPrint("      [address]  - 32bit stating physical address.  ex 0xF9C00000\n");
  AsciiPrint("      [value]    - 32bit value to write to the register.\n");
  AsciiPrint("      [range]    - optional, repeat the same write to multiple register sequentially.  The default is 1\n\n");
  AsciiPrint("    Examples:\n");  
  AsciiPrint("      RegisterApp write 0xFD900000 0x104            - Write 0x104 to the register at address at 0xFD900000\n");
  AsciiPrint("      RegisterApp write 0xFD900000 0xFFFFFFFF 1024  - Fill the registers from 0xFD900000-0xFD9003FF with all ones\n\n\n");
  AsciiPrint("    Note: Reading or writing to unclocked registers could hang the CPU. Writing invalid data registers can also hang or crash the system\n\n");
}


