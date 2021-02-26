/**
* @file ChipInfoDxe.c
* @brief   This file implements ChipInfo EFI protocol 
*          interface.

  Copyright (c) 2010-2014, 2017 Qualcomm Technologies, Inc. All rights reserved.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/14   sho     Use DAL configuration to register devices
08/14/14   aa      Adding dummy gDALModStringDeviceInfoList for string device.
05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
02/13/13   pbi     Fixed warnings.
06/07/11   pbi     Created.
==============================================================================*/

/*==============================================================================

                       INCLUDE FILES FOR THIS MODULE

==============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDIChipInfo.h"
//#include "ChipInfo.h"
#include <Protocol/EFIChipInfo.h>
#include <Library/PcdLib.h>

/*==============================================================================
	  
	                         DEFINES FOR THIS MODULE
	  
==============================================================================*/



/*==============================================================================

                     EXTERNAL DEFINES FOR THIS MODULE

==============================================================================*/
DALResult 
ChipInfo_DalChipInfo_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice);

/*==============================================================================

                     GLOBAL VARIABLES FOR THIS MODULE

==============================================================================*/

/*
 * ChipInfo interface handle.  This must be initialized before accessing any 
 * ChipInfo APIs. 
 */

static DalDeviceHandle *chipInfoHandle;


/*
 * For dynamic device loading the DALSYSModConfig structure is populated 
 * by this module. 
 */

extern DALREG_DriverInfo DALChipInfo_DriverInfo;
static DALREG_DriverInfo * DALDriverInfoArr[1] = { &DALChipInfo_DriverInfo };

static DALSYSConfig DALSYSModConfig =
   {
      {0, NULL},              // string device info
      {1, DALDriverInfoArr}   // numeric driver info
   };

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/


/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetChipVersion

  DESCRIPTION   See EFI_DalChipInfo_GetChipVersion in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetChipVersion(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT EFIChipInfoVersionType *pnVersion)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetChipVersion(chipInfoHandle, 
                                       (DalChipInfoVersionType *)pnVersion);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetChipVersion */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetRawChipVersion

  DESCRIPTION   See EFI_DalChipInfo_GetRawChipVersion in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetRawChipVersion(IN  EFI_CHIPINFO_PROTOCOL *This,
                                  OUT UINT32 *pnVersion)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetRawChipVersion(chipInfoHandle, 
                                       (uint32 *)pnVersion);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetRawChipVersion */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetChipId

  DESCRIPTION   See EFI_DalChipInfo_GetChipId in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetChipId(IN  EFI_CHIPINFO_PROTOCOL *This,
                          OUT EFIChipInfoIdType *peId)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetChipId(chipInfoHandle, (ChipInfoIdType *)peId);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetChipId */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetRawChipId

  DESCRIPTION   See EFI_DalChipInfo_GetRawChipId in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetRawChipId(IN  EFI_CHIPINFO_PROTOCOL *This,
                             OUT UINT32 *pnId)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetRawChipId(chipInfoHandle, (uint32 *)pnId);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetRawChipId */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetChipIdString

  DESCRIPTION   See EFI_DalChipInfo_GetChipIdString in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetChipIdString(IN  EFI_CHIPINFO_PROTOCOL *This,
                                OUT CHAR8 *szIdString, 
                                IN UINT32 nMaxLength)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetChipIdString(chipInfoHandle, szIdString, nMaxLength );
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetChipIdString */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetChipFamily

  DESCRIPTION   See EFI_DalChipInfo_GetChipFamily in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetChipFamily(IN  EFI_CHIPINFO_PROTOCOL *This,
                              OUT EFIChipInfoFamilyType *peFamily)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetChipFamily(chipInfoHandle, (ChipInfoFamilyType *)peFamily);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetChipFamily */



/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetModemSupport

  DESCRIPTION   See EFI_DalChipInfo_GetModemSupport in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetModemSupport(IN  EFI_CHIPINFO_PROTOCOL *This,
                                OUT EFIChipInfoModemType *pnModem)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetModemSupport(chipInfoHandle,(ChipInfoModemType *) pnModem);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetModemSupport */


/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetProcessorNameString

  DESCRIPTION   See EFI_DalChipInfo_GetProcessorNameString in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetProcessorNameString(IN  EFI_CHIPINFO_PROTOCOL *This,
                                OUT CHAR8 *szNameString, 
                                IN UINT32 nMaxLength)
{
  return(EFI_UNSUPPORTED);
} /* EFI_DalChipInfo_GetProcessorNameString */

/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetSerialNumber

  DESCRIPTION   See EFI_DalChipInfo_GetSerialNumber in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetSerialNumber(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT EFIChipInfoSerialNumType *peId )
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetSerialNumber(chipInfoHandle,(ChipInfoSerialNumType *) peId);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetSerialNumber */

/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetFoundryId

  DESCRIPTION   See EFI_DalChipInfo_GetFoundryId in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetFoundryId(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT EFIChipInfoFoundryIdType *peId )
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetFoundryId(chipInfoHandle,(ChipInfoFoundryIdType *) peId);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetFoundryId */

/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetRawDeviceFamily

  DESCRIPTION   See EFI_DalChipInfo_GetRawDeviceFamily in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetRawDeviceFamily(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT uint32 *pnId )
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetRawDeviceFamily(chipInfoHandle,(uint32 *) pnId );
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetRawDeviceFamily */

/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetRawDeviceNumber

  DESCRIPTION   See EFI_DalChipInfo_GetRawDeviceNumber in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetRawDeviceNumber(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT uint32 *pnId )
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetRawDeviceNumber(chipInfoHandle,(uint32 *) pnId );
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetRawDeviceNumber */


/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetQFPROMChipId

  DESCRIPTION   See EFI_DalChipInfo_GetQFPROMChipId in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetQFPROMChipId(IN  EFI_CHIPINFO_PROTOCOL *This,
                               OUT EFIChipInfoQFPROMChipIdType *pnId )
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetQFPROMChipId(chipInfoHandle,(ChipInfoQFPROMChipIdType *) pnId);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetQFPROMChipId */


/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetMarketingNameString

  DESCRIPTION   See EFI_DalChipInfo_GetMarketingNameString in Protocol/EFIChipInfo.h

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetMarketingNameString(IN  EFI_CHIPINFO_PROTOCOL *This,
                                OUT CHAR8 *szNameString, 
                                IN UINT32 nMaxLength)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetMarketingNameString(chipInfoHandle, szNameString, nMaxLength);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetMarketingNameString */


/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetDefectivePart

  DESCRIPTION   See EFI_DalChipInfo_GetDefectivePart

==============================================================================*/
EFI_STATUS
EFI_DalChipInfo_GetDefectivePart(IN  EFI_CHIPINFO_PROTOCOL *This, 
                              IN  EFIChipInfoPartType ePart,
                              OUT UINT32 *pnMask)
{
  DALResult result = DAL_ERROR;

  if(chipInfoHandle)
  {
    result = DalChipInfo_GetDefectivePart(chipInfoHandle, (ChipInfoPartType)ePart, pnMask);
  }
  if(DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  else if (DAL_ERROR_OUT_OF_RANGE_PARAMETER == result)
  {
    return(EFI_NOT_FOUND);
  }
  return(EFI_UNSUPPORTED);

} /* EFI_DalChipInfo_GetDefectiveParts */

/*==============================================================================

  FUNCTION      EFI_DalChipInfo_GetDefectiveCPUs

  DESCRIPTION   See EFI_DalChipInfo_GetDefectiveCPUs

==============================================================================*/
EFI_STATUS 
EFI_DalChipInfo_GetDefectiveCPUs(IN EFI_CHIPINFO_PROTOCOL *This,
                              IN  UINT32 nCPUCluster, 
                              OUT UINT32 *pnMask)
{
  DALResult result = DAL_ERROR;

  if (chipInfoHandle)
  {
    result = DalChipInfo_GetDefectiveCPUs(chipInfoHandle, nCPUCluster, pnMask);
  }
  if (DAL_SUCCESS == result)
  {
    return(EFI_SUCCESS);
  }
  else if (DAL_ERROR_OUT_OF_RANGE_PARAMETER == result)
  {
    return(EFI_NOT_FOUND);
  }
  return(EFI_UNSUPPORTED);
}
  
  
/*
 * Function table for DAL wrapper APIs.
 */

static EFI_CHIPINFO_PROTOCOL ChipInfoProtocol = 
{
  EFI_CHIPINFO_PROTOCOL_REVISION,
  EFI_DalChipInfo_GetChipVersion,
  EFI_DalChipInfo_GetRawChipVersion,
  EFI_DalChipInfo_GetChipId,
  EFI_DalChipInfo_GetRawChipId,
  EFI_DalChipInfo_GetChipIdString,
  EFI_DalChipInfo_GetChipFamily,
  EFI_DalChipInfo_GetModemSupport,
  EFI_DalChipInfo_GetProcessorNameString,
  EFI_DalChipInfo_GetSerialNumber,
  EFI_DalChipInfo_GetFoundryId,
  EFI_DalChipInfo_GetRawDeviceFamily,
  EFI_DalChipInfo_GetRawDeviceNumber,
  EFI_DalChipInfo_GetQFPROMChipId,
  EFI_DalChipInfo_GetMarketingNameString,
  EFI_DalChipInfo_GetDefectivePart,
  EFI_DalChipInfo_GetDefectiveCPUs,
};


/*==============================================================================

  FUNCTION      ChipInfoEntryPoint

  DESCRIPTION   EFI DAL-Wrapper Entry point for ChipInfo access.

==============================================================================*/ 

EFI_STATUS
EFIAPI
ChipInfoEntryPoint (IN EFI_HANDLE ImageHandle, 
                    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status = EFI_UNSUPPORTED;
  DALResult result = DAL_ERROR;

  DALSYS_InitMod(&DALSYSModConfig);

  result = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &chipInfoHandle);

  if (DAL_SUCCESS == result)
  {
    result = chipInfoHandle->pVtbl->DalDevice.Init(chipInfoHandle);

    if(DAL_SUCCESS == result)
    {
    status = gBS->InstallMultipleProtocolInterfaces(&handle, &gEfiChipInfoProtocolGuid, 
                                                    (void **)&ChipInfoProtocol, 
                                                    NULL, NULL, NULL );
    }
    else
    {
      chipInfoHandle->pVtbl->DalDevice.DeInit(chipInfoHandle);
      DALSYS_DeInitMod();
    }
  }
  return status;

} /* ChipInfoEntryPoint */


