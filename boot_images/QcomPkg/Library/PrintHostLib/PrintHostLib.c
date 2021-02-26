/** @file PrintHostLib.c
   
  Contains Print Host Library implementation.

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/03/16   bh      Initial Version

=============================================================================*/

#include <Uefi.h>
#include <Base.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/PrintShLib.h>

extern EFI_GUID gEfiPrintShLibHobGuid;

PrintShLibType PrShLib = 
{
  .AsciiVSPrint = AsciiVSPrint,
  .AsciiSPrint = AsciiSPrint,
  .UnicodeVSPrint = UnicodeVSPrint,
  .UnicodeSPrint = UnicodeSPrint,
  .UnicodeVSPrintAsciiFormat = UnicodeVSPrintAsciiFormat,
  .UnicodeValueToString = UnicodeValueToString,
  .SPrintLength = SPrintLength,
};


EFI_STATUS
InitPrintLibHost(VOID)
{
  VOID * Hob;
  Hob = BuildGuidDataHob(&gEfiPrintShLibHobGuid,
                   (VOID *) &PrShLib,
                   sizeof(PrShLib));

  if (Hob)
    return EFI_SUCCESS;
  else 
    return EFI_NOT_READY;
}



