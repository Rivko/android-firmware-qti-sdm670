/*===========================================================================

                    BOOT EXTERN PLATFORMINFO DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external platform info drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011, 2013, 2015-2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/17   yps     Share platform-id APIs from XBL-loader->XBL-ramdump over shared-function-table interface
07/28/16   kpa     Added boot_DalPlatformInfo_Platform
10/05/15   ck      Corrected copyright
02/27/13   dh      Add boot_DalPlatformInfo_CDTConfigPreDDR and boot_DalPlatformInfo_CDTConfigPostDDR
10/18/11   dh      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DDIPlatformInfo.h"
#include "PlatformInfo.h"
#include "boot_extern_platforminfo_interface.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_DalPlatformInfo_CDTConfigPreDDR

** ==========================================================================
*/
/**
  Configures platform information from memory.

  This function configures the PlatformInfo DAL with the platform information
  located at the address contained in the parameter pMem.

  @param[in] _h   Handle to the PlatformInfo DAL device.
  @param[in] pMem Address to the location in memory that contains 
                  platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pMem must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult boot_DalPlatformInfo_CDTConfigPreDDR(DalDeviceHandle * _h,  void * pMem)
{
  return DalPlatformInfo_CDTConfigPreDDR(_h, pMem);
}


/*===========================================================================

**  Function :  boot_DalPlatformInfo_CDTConfigPostDDR

** ==========================================================================
*/
/**
  Configures platform information from memory.

  This function configures the PlatformInfo DAL with the platform information
  located at the address contained in the parameter pMem.

  @param[in] _h   Handle to the PlatformInfo DAL device.
  @param[in] pMem Address to the location in memory that contains 
                  platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pMem must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult boot_DalPlatformInfo_CDTConfigPostDDR(DalDeviceHandle * _h,  void * pMem)
{
  return DalPlatformInfo_CDTConfigPostDDR(_h, pMem);
}

/*=========================================================================

**  FUNCTION  boot_DalPlatformInfo_Platform

===========================================================================*/
/**
  Return the current platform.

  This function is a wrapper around DalPlatformInfo_GetPlatform(). It provides
  platform retrieval without the code overhead of attaching to the DAL.

  @return
  DalPlatformInfoPlatformType() -- Current platform. \n
  DALPLATFORMINFO_TYPE_UNKNOWN  -- DAL device attach failed or cannot
                                   determine the platform.

  @dependencies
  None.

  @sa
  DalPlatformInfo_GetPlatform().
*/
DalPlatformInfoPlatformType boot_DalPlatformInfo_Platform(void)
{
  return PlatformInfo_Platform();
}

/*===========================================================================

**  Function :  boot_DalPlatformInfo_Platforminfo

** ==========================================================================
*/
/**
  Get platform information from memory.

  This function returns information about the current platform in the parameter
  pPlatforminfo.

  @param[in] Address to the location in memory that return 
                   information

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  None

*/

DALResult boot_DalPlatformInfo_PlatformInfo(void * pPlatforminfo)
{
  
  DalDeviceHandle        *phPlatformInfo = NULL;
  DALResult              eResult = DAL_SUCCESS ; 
  
  if (phPlatformInfo == NULL)
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfo);
    if (eResult == DAL_SUCCESS)
    {
      eResult = DalPlatformInfo_GetPlatformInfo(phPlatformInfo, (DalPlatformInfoPlatformInfoType*)pPlatforminfo);
    }
  }
  
  
   return eResult;
}

/*===========================================================================

**  Function :  boot_DalPlatformInfo_ChipId

** ==========================================================================
*/
/**
  Get chipid from memory.

  This function returns chipid about the current platform in the parameter
  pChipSetId.

  @param[in] Address to the location in memory that return 
                   information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  None

*/

DALResult boot_DalPlatformInfo_ChipId(void * pChipSetId)
{
  
  DalDeviceHandle        *phChipInfo = NULL;
  DALResult              eResult = DAL_SUCCESS ; 
  
  if (phChipInfo == NULL)
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);
    if (eResult == DAL_SUCCESS)
    {
       eResult = DalChipInfo_GetChipId(phChipInfo, (DalChipInfoIdType *)pChipSetId); 
    }
   }
   return eResult;
}

/*===========================================================================

**  Function :  boot_DalPlatformInfo_ChipFamily

** ==========================================================================
*/
/**
  Get chipid from memory.

  This function returns chipsetfamily about the current platform in the parameter
  pChipsetfamily.

  @param[in] Address to the location in memory that return 
                   information

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  None

*/

DALResult boot_DalPlatformInfo_ChipFamily(void * pChipsetfamily)
{
  
  DalDeviceHandle        *phChipInfo = NULL;
  DALResult              eResult = DAL_SUCCESS ; 
 
  if (phChipInfo == NULL)
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);
    if (eResult == DAL_SUCCESS)
    { 
	   eResult = DalChipInfo_GetChipFamily( phChipInfo ,(DalChipInfoFamilyType *)pChipsetfamily);
    }

   }
   return eResult;
}
