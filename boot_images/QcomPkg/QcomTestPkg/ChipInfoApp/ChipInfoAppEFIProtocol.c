/** @file ChipInfoAppEFIProtocol.c 
   
  This file contains application test code for ChipInfo 
  driver. The code uses ChipInfo EFI protocol interface to 
  make appropriate function calls. 

  Copyright (c) 2010-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*==========================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 06/16/11   pbi     Created.

==========================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIChipInfo.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/TestInterface.h>

/*=========================================================================
      Functions
==========================================================================*/

/**
  ChipInfo test using EFI protocol only. 

  @param 
  None. 

  @return
  None.
**/

EFI_STATUS ChipInfoTestUsingEFIProtocol()
{
  EFI_STATUS                eResult = EFI_UNSUPPORTED;
  EFI_CHIPINFO_PROTOCOL     *mChipInfoProtocol;
  EFIChipInfoVersionType    nVersion;
  UINT32                    nRawVersion;
  EFIChipInfoIdType         eId;
  UINT32                    nId;
  CHAR8                     szIdString[EFICHIPINFO_MAX_ID_LENGTH];
  CHAR8                     szNameString[EFICHIPINFO_MAX_NAME_LENGTH];
  EFIChipInfoFamilyType     eFamily;
  EFIChipInfoModemType      nModem;
  EFIChipInfoSerialNumType  nSerialNum;
  EFIChipInfoFoundryIdType  eFoundryId;

  AsciiPrint ("\n=====================o======================\n");
  AsciiPrint ("ChipInfo Application Driver - EFI Protocol\n");
  AsciiPrint ("==============================================\n");

  eResult = gBS->LocateProtocol (
                  &gEfiChipInfoProtocolGuid,
                  NULL,
                  (VOID **) &mChipInfoProtocol
                  );
  if (EFI_SUCCESS != eResult)
  { 
    return eResult;
  }

  eResult = mChipInfoProtocol->GetChipVersion( mChipInfoProtocol, 
                                                     &nVersion);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  Chip Version :%x \n", nVersion);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetChipVersion failed \n");
  }

  eResult = mChipInfoProtocol->GetRawChipVersion( mChipInfoProtocol, 
                                                     &nRawVersion);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  Raw Chip Version :%x \n", nRawVersion);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetRawChipVersion failed \n");
  }

  eResult = mChipInfoProtocol->GetChipId( mChipInfoProtocol, 
                                                     &eId);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  ChipId :%d \n", eId);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetChipId failed \n");
  }

  eResult = mChipInfoProtocol->GetRawChipId( mChipInfoProtocol, 
                                                     &nId);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  RawChipId :0x%x \n", nId);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetRawChipId failed \n");
  }

  eResult = mChipInfoProtocol->GetChipIdString( mChipInfoProtocol, 
                                                (CHAR8*)&szIdString,  
                                                EFICHIPINFO_MAX_ID_LENGTH);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  ChipIdString :%a \n", szIdString);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetChipIdString failed \n");
  }

  eResult = mChipInfoProtocol->GetChipFamily( mChipInfoProtocol, 
                                                     &eFamily);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  ChipFamily:0x%x \n", eFamily);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetChipFamily failed \n");
  }

  eResult = mChipInfoProtocol->GetModemSupport( mChipInfoProtocol, 
                                                     &nModem);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  GetModemSupport:0x%x \n", nModem);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetModemSupport failed \n");
  }
 
  eResult = mChipInfoProtocol->GetMarketingNameString( mChipInfoProtocol, 
                                                (CHAR8*)&szNameString,  
                                                EFICHIPINFO_MAX_NAME_LENGTH);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  MarketingNameString :%a \n", szNameString);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetMarketingNameString failed \n");
  }


  eResult = mChipInfoProtocol->GetSerialNumber( mChipInfoProtocol, 
                                                     &nSerialNum);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  ChipSerialNumber:0x%x \n", nSerialNum);
  }
  else
  {
    AsciiPrint ("ChipInfo:  ChipGetSerialNumber failed \n");
  }

  eResult = mChipInfoProtocol->GetFoundryId( mChipInfoProtocol, 
                                                     &eFoundryId);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  ChipFoundryId:0x%x \n", eFoundryId);
  }
  else
  {
    AsciiPrint ("ChipInfo:  ChipGetFoundryId failed \n");
  }
  eResult = mChipInfoProtocol->GetRawDeviceFamily( mChipInfoProtocol, 
                                                     &nId);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  RawDeviceFamily:0x%x \n", nId);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetRawDeviceFamily failed \n");
  }

  eResult = mChipInfoProtocol->GetRawDeviceNumber( mChipInfoProtocol, 
                                                     &nId);
  if (eResult  == EFI_SUCCESS)
  {
    AsciiPrint ("ChipInfo:  RawDeviceNumber:0x%x \n", nId);
  }
  else
  {
    AsciiPrint ("ChipInfo:  GetRawDeviceNumber failed \n");
  }
  return eResult;
}
