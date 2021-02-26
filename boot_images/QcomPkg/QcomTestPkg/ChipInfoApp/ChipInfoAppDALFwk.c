/** @file ChipInfoAppDALFwk.c
   
  This file contains application test code for ChipInfo 
  driver. The code uses DAL framework to make appropriate 
  function calls. 

  Copyright (c) 2010-2012,2014 Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 06/16/11   pbi     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include "DALDeviceId.h"
#include "DDIChipInfo.h"
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/TestInterface.h>

/*=========================================================================
      Functions
==========================================================================*/


/**
  ChipInfo test using DAL framework only. 

  @param 
  None. 

  @return
  None.
**/

DALResult ChipInfoTestUsingDALFramework()
{
  DALResult                 result = DAL_ERROR;
  DalDeviceHandle           *chipInfoHandle;
  DalChipInfoVersionType    nVersion;
  uint32                    nRawVersion;
  DalChipInfoIdType         eId;
  uint32                    nId;
  char                      szIdString[DALCHIPINFO_MAX_ID_LENGTH];
  char                      szNameString[DALCHIPINFO_MAX_NAME_LENGTH];
  DalChipInfoFamilyType     eFamily;
  DalChipInfoModemType      nModem;
  DalChipInfoSerialNumType  nSerialNum;
  DalChipInfoFoundryIdType  eFoundryId;

  AsciiPrint ("\n=====================o=======================\n");
  AsciiPrint ("ChipInfo Application Driver - DAL Framework\n");
  AsciiPrint ("===============================================\n");

  /* Initialize DALSYS and attach to the ChipInfo driver. */
  DALSYS_InitMod(NULL); 
  result = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, 
                            &chipInfoHandle);

  if(DAL_ERROR == result)
  {
    AsciiPrint ("ChipInfo attach failed: \"%d\" ...\n", result);
    DALSYS_DeInitMod();
    return result;
  }

  /* 
     Test the ChipInfo driver interface. Most of the Chipinfo's
     functionality lies in ChipInfo_DriverInit. The HW_REVISION_NUMBER register                                                           .
     is read and the driver context is filled with all the info. When the                                                                                                  .
     chipinfo APIs are called, the corresponding fields from the driver context                                                                                            .
     structure are returned. There is no other functionality being done in the                                                                                             .
     APIs, so they always return success. If there was an error accessing the                                                                                              .
     HW_REVISION_NUMBER register, the attach to chipinfo itself would have                                                                                                 .
     failed. So we do not check for the return status of the APIs here.
  */
  if (chipInfoHandle!= NULL)
  {
    DalChipInfo_GetChipVersion(chipInfoHandle, &nVersion);
    AsciiPrint ("ChipInfo:  Chip Version :%x \n", nVersion);
  
    DalChipInfo_GetRawChipVersion(chipInfoHandle, &nRawVersion);
    AsciiPrint ("ChipInfo:  Raw Chip Version :%x \n", nRawVersion);
  
    DalChipInfo_GetChipId(chipInfoHandle, &eId);
    AsciiPrint ("ChipInfo:  Chip Id:%d \n", eId);
  
    DalChipInfo_GetRawChipId(chipInfoHandle, &nId);
    AsciiPrint ("ChipInfo:  Raw ChipId:0x%x \n", nId);
  

    DalChipInfo_GetChipIdString(chipInfoHandle, (char*)&szIdString,  DALCHIPINFO_MAX_ID_LENGTH);
    AsciiPrint ("ChipInfo:  ChipIdString:%a \n", szIdString);
  
    DalChipInfo_GetChipFamily(chipInfoHandle, &eFamily);
    AsciiPrint ("ChipInfo:  ChipFamily:0x%x \n", eFamily);
  
    DalChipInfo_GetModemSupport(chipInfoHandle, &nModem);
    AsciiPrint ("ChipInfo:  GetModemSupport:0x%x \n", nModem);
  
    DalChipInfo_GetMarketingNameString(chipInfoHandle, (char*)&szNameString,  DALCHIPINFO_MAX_NAME_LENGTH);
    AsciiPrint ("ChipInfo:  MarketingNameString:%a \n", szNameString);

    DalChipInfo_GetSerialNumber(chipInfoHandle, &nSerialNum);
    AsciiPrint ("ChipInfo:  ChipSerialNumber:0x%x \n", nSerialNum);

    DalChipInfo_GetFoundryId(chipInfoHandle, &eFoundryId);
    AsciiPrint ("ChipInfo:  ChipFoundryId:0x%x \n", eFoundryId);

    DalChipInfo_GetRawDeviceFamily(chipInfoHandle, &nId);
    AsciiPrint ("ChipInfo:  RawDeviceFamily:0x%x \n", nId);

    DalChipInfo_GetRawDeviceNumber(chipInfoHandle, &nId);
    AsciiPrint ("ChipInfo:  RawDeviceNumber:0x%x \n", nId);


    result = DAL_DeviceDetach(chipInfoHandle);
    DALSYS_DeInitMod();
  }
  return result;
}
