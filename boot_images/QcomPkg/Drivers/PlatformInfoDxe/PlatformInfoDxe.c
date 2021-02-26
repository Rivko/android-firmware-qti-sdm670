/** @file PlatformInfoDxe.c
   
  This file implements PlatformInfo EFI protocol interface.

  Copyright (c) 2011,2014 Qualcomm Technologies, Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/16/14   sho     Use DALSYS config structure to register devices
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 03/07/11   mdt     Created.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Protocol/EFIPlatformInfo.h>
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"

/* 
 * Defined in DalPlatformInfo.c
 */
extern DALREG_DriverInfo DALPlatformInfo_DriverInfo;

/* 
 * DALSYSModConfig needs to be created for WM and UEFI - dynamic 
 * loading of DAL drivers. This will not work for AMSS image (static loading 
 * via DAL based XML files at compile time) so the following can't go in 
 * DalPlatformInfo.c which is common across images 
 * 
 */ 
static DALREG_DriverInfo* DALDriverInfoArr[1] = { &DALPlatformInfo_DriverInfo};
static DALSYSConfig DALSYSModConfig = 
   {
      {0, NULL},              // string device info
      {1, DALDriverInfoArr}   // numeric driver info
   };

DALREG_DriverInfoList gDALModDriverInfoList= {1, DALDriverInfoArr};
DALProps gDALModStringDeviceInfoList = {NULL, 0 ,0 , NULL};


/*=========================================================================
      Function Prototypes
==========================================================================*/


EFI_STATUS EFI_GetPlatformInfo(IN  EFI_PLATFORMINFO_PROTOCOL *This,
  OUT EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *PlatformInfo);

EFI_STATUS EFI_GetKeyValue(
  IN  EFI_PLATFORMINFO_PROTOCOL   *This,
  IN  EFI_PLATFORMINFO_KEY_TYPE    Key,
  OUT UINT32                      *Value);


/*=========================================================================
      Data Declarations
==========================================================================*/


// PlatformInfo Public Protocol 
static EFI_PLATFORMINFO_PROTOCOL PlatformInfoProtocol =
{
   EFI_PLATFORMINFO_PROTOCOL_VERSION,
   EFI_GetPlatformInfo,
   EFI_GetKeyValue
};

static DalDeviceHandle *hPlatformInfo = NULL;


/*=========================================================================
      Functions
==========================================================================*/


EFI_STATUS
EFI_GetPlatformInfo(
  IN  EFI_PLATFORMINFO_PROTOCOL           *This,
  OUT EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *PlatformInfo)
{

  if (PlatformInfo == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  DalPlatformInfo_GetPlatformInfo(
    hPlatformInfo,
    (DalPlatformInfoPlatformInfoType*)PlatformInfo);

  return EFI_SUCCESS;
}

EFI_STATUS
EFI_GetKeyValue(
  IN  EFI_PLATFORMINFO_PROTOCOL   *This,
  IN  EFI_PLATFORMINFO_KEY_TYPE    Key,
  OUT UINT32                      *Value)
{
  DALResult eResult;

  if (Value == NULL || Key >= EFI_PLATFORMINFO_NUM_KEYS)
  {
    return EFI_INVALID_PARAMETER;
  }

  eResult = DalPlatformInfo_GetKeyValue(hPlatformInfo,
    (DalPlatformInfoKeyType)Key, (uint32 *)Value);
  if (eResult != DAL_SUCCESS)
  {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  PlatformInfo DXE driver entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
PlatformInfoDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status;
  DALResult   eResult;

  // Init DALSys
  DALSYS_InitMod(&DALSYSModConfig);

  //Attach to the PlatformInfo DAL
  eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &hPlatformInfo);
  if(eResult == DAL_SUCCESS)
  {
    // Publish the PlatformInfo Protocol
    status = 
      gBS->InstallMultipleProtocolInterfaces( 
        &handle, 
        &gEfiPlatformInfoProtocolGuid, 
        (void **)&PlatformInfoProtocol, 
        NULL );
  }
  else
  {
    DALSYS_DeInitMod();
    status = EFI_PROTOCOL_ERROR;
  }

  ASSERT_EFI_ERROR(status);

  return status;
}

