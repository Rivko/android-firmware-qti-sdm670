/** @file PrintShLib.c
   
  Contains Print Shared Library implementation.

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/03/16   vk      Add NULL checks
 03/03/16   bh      Initial Version

=============================================================================*/

#include <Uefi.h>
#include <Base.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/ArmLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/PrintShLib.h>

extern EFI_GUID gEfiPrintShLibHobGuid;

STATIC PrintShLibType *PrintShLibPtr = NULL;

EFI_STATUS PrintShLibInitialize (VOID);

UINTN AsciiVSPrint (OUT CHAR8         *StartOfBuffer,
                    IN  UINTN         BufferSize,
                    IN  CONST CHAR8   *FormatString,
                    IN  VA_LIST       Marker)
{
  UINTN InterruptState, Return;
  InterruptState = ArmGetInterruptState();
  ArmDisableInterrupts();
  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return 0;
  }
 
  ASSERT (PrintShLibPtr->AsciiVSPrint != NULL);
  if (PrintShLibPtr->AsciiVSPrint == NULL)
    return 0;

  Return = PrintShLibPtr->AsciiVSPrint(StartOfBuffer, BufferSize, FormatString, Marker);

  if (InterruptState)
    ArmEnableInterrupts();
  return Return;
}

UINTN AsciiSPrint (OUT CHAR8         *StartOfBuffer,
                   IN  UINTN         BufferSize,
                   IN  CONST CHAR8   *FormatString,
                   ...)
{
  VA_LIST Marker;
  UINTN NumberOfPrinted, InterruptState;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();

  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return 0;
  }
  VA_START(Marker, FormatString);

  ASSERT (PrintShLibPtr->AsciiVSPrint != NULL);
  if (PrintShLibPtr->AsciiVSPrint == NULL)
    return 0;

  NumberOfPrinted = PrintShLibPtr->AsciiVSPrint(StartOfBuffer, BufferSize, FormatString, Marker);
  VA_END(Marker);

  if (InterruptState)
    ArmEnableInterrupts();
  return NumberOfPrinted;
}

UINTN UnicodeSPrint (OUT CHAR16         *StartOfBuffer,
                   IN  UINTN         BufferSize,
                   IN  CONST CHAR16   *FormatString,
                   ...)
{
  VA_LIST Marker;
  UINTN NumberOfPrinted, InterruptState;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();

  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return 0;
  }
  VA_START(Marker, FormatString);

  ASSERT (PrintShLibPtr->UnicodeVSPrint != NULL);
  if (PrintShLibPtr->UnicodeVSPrint == NULL)
    return 0;

  NumberOfPrinted = PrintShLibPtr->UnicodeVSPrint(StartOfBuffer, BufferSize, FormatString, Marker);
  VA_END(Marker);
  if (InterruptState)
    ArmEnableInterrupts();
  return NumberOfPrinted;
}

UINTN UnicodeVSPrint (OUT CHAR16        *StartOfBuffer,
                      IN  UINTN         BufferSize,
                      IN  CONST CHAR16  *FormatString,
                      IN  VA_LIST       Marker)
{
  UINTN InterruptState, Return;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();
  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return 0;
  }

  ASSERT (PrintShLibPtr->UnicodeVSPrint != NULL);
  if (PrintShLibPtr->UnicodeVSPrint == NULL)
    return 0;
  
  Return = PrintShLibPtr->UnicodeVSPrint(StartOfBuffer, BufferSize, FormatString, Marker);

  if (InterruptState)
    ArmEnableInterrupts();
  return Return;
}

UINTN UnicodeVSPrintAsciiFormat (OUT CHAR16        *StartOfBuffer,
                      IN  UINTN         BufferSize,
                      IN  CONST CHAR8  *FormatString,
                      IN  VA_LIST       Marker)
{
  UINTN InterruptState, Return;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();
  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return 0;
  }

  ASSERT (PrintShLibPtr->UnicodeVSPrintAsciiFormat != NULL);
  if (PrintShLibPtr->UnicodeVSPrintAsciiFormat == NULL)
    return 0;

  Return = PrintShLibPtr->UnicodeVSPrintAsciiFormat(StartOfBuffer, BufferSize, FormatString, Marker);

  if (InterruptState)
    ArmEnableInterrupts();
  return Return;
}

UINTN
EFIAPI
UnicodeValueToString (
  IN OUT CHAR16  *Buffer,
  IN UINTN       Flags,
  IN INT64       Value,
  IN UINTN       Width
  )
{
  UINTN InterruptState, Return;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();
  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return EFI_NOT_READY;
  }

  ASSERT (PrintShLibPtr->UnicodeValueToString != NULL);
  if (PrintShLibPtr->UnicodeValueToString == NULL)
    return 0;  
  Return = PrintShLibPtr->UnicodeValueToString(Buffer, Flags, Value, Width);

  if (InterruptState)
    ArmEnableInterrupts();
  return Return;
}

UINTN
EFIAPI
SPrintLength (
  IN  CONST CHAR16   *FormatString,
  IN  VA_LIST       Marker
  )
{
  UINTN InterruptState, Return;
  InterruptState = ArmGetInterruptState(); 
  ArmDisableInterrupts();
  if (PrintShLibPtr == NULL)
  {
    PrintShLibInitialize();
    if (PrintShLibPtr == NULL)
      return EFI_NOT_READY;
  }

  ASSERT (PrintShLibPtr->SPrintLength != NULL);
  if (PrintShLibPtr->SPrintLength == NULL)
    return 0;  
  Return = PrintShLibPtr->SPrintLength(FormatString, Marker);

  if (InterruptState)
    ArmEnableInterrupts();
  return Return;
}

EFI_STATUS PrintShLibInitialize (VOID)
{
  EFI_HOB_GUID_TYPE *GuidHob;
  UINT32** DataPtr;
  
  GuidHob = GetFirstGuidHob(&gEfiPrintShLibHobGuid);

  if (GuidHob == NULL)
    return EFI_NOT_READY;
  
  DataPtr = GET_GUID_HOB_DATA(GuidHob);
  PrintShLibPtr = (PrintShLibType *) DataPtr;
  return EFI_SUCCESS;
}

