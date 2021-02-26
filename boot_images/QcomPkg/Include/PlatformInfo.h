#ifndef __PLATFORMINFO_H__
#define __PLATFORMINFO_H__

/**
  @file PlatformInfo.h
  @brief Public interface include file for accessing the PlatformInfo driver.
*/

/*=============================================================================
  Copyright (c) 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "PlatformInfoDefs.h"
#include "DALStdDef.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Function Definitions
==========================================================================*/


/*=========================================================================
FUNCTION  PlatformInfo_Platform
===========================================================================*/
/**
  Return the platform type.

  @dependencies
  None.
*/
DalPlatformInfoPlatformType
PlatformInfo_Platform (void);


/*=========================================================================
FUNCTION  PlatformInfo_PlatformSubtype
===========================================================================*/
/**
  Return the platform subtype.

  @dependencies
  None.
*/
uint32
PlatformInfo_PlatformSubtype (void);


/*=========================================================================
FUNCTION  PlatformInfo_PlatformVersion
===========================================================================*/
/**
  Return the platform subtype.

  @dependencies
  None.
*/
uint32
PlatformInfo_PlatformVersion (void);


/*=========================================================================
FUNCTION  PlatformInfo_IsFusion
===========================================================================*/
/**
  Return if the platform is a Fusion variant.

  @dependencies
  None.
*/
boolean
PlatformInfo_IsFusion (void);


/*=========================================================================
FUNCTION  PlatformInfo_GetKeyValue
===========================================================================*/
/**
  Get a key value.

  This function returns a key value stored in the CDT.

  @param[in]  eKey   Key to get the value for.
  @param[out] pValue Key value.

  @return
  DAL_SUCCESS -- Key found and value stored in pValue. \n
  DAL_ERROR -- Key not found.

  @dependencies
  None.
*/
DALResult
PlatformInfo_GetKeyValue (DalPlatformInfoKeyType eKey, uint32 *pValue);


/*=========================================================================
FUNCTION  PlatformInfo_GetPlatformInfo
===========================================================================*/
/**
  Returns information on the current platform.

  This function returns information about the current platform in the parameter
  pInfo.

  @param[out] pInfo Variable to return to the caller.

  @return
  DAL_SUCCESS -- Platform identified and placed in pType. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  None.
*/
DALResult
PlatformInfo_GetPlatformInfo (DalPlatformInfoPlatformInfoType * pInfo);


/*=========================================================================
FUNCTION  PlatformInfo_CDTConfigPreDDR
===========================================================================*/
/**
  Configure the PlatformInfo driver with data from the CDT.

  This functions configures the PlatformInfo driver with data from the CDT,
  located at the parameter pCDTConfig. To be called before DDR is available.

  @param[in] pCDTConfig Address to the location in memory that contains
                        platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pCDTConfig must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult
PlatformInfo_CDTConfigPreDDR (void * pCDTConfig);


/*=========================================================================
FUNCTION  PlatformInfo_CDTConfigPostDDR
===========================================================================*/
/**
  Configure the PlatformInfo driver with data from the CDT.

  This functions configures the PlatformInfo driver with data from the CDT,
  located at the parameter pCDTConfig. To be called after DDR is available. If
  PlatformInfo_CDTConfigPreDDR has already been called, then the pCDTConfig
  parameter is optional.

  @param[in] pCDTConfig Address to the location in memory that contains
                        platform information.

  @return
  DAL_SUCCESS -- PlatformInfo DAL was configured successfully. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  The parameter pCDTConfig must point to a location in memory that is formatted
  according to the Platform ID block of the EEPROM software Configuration
  Data Table (CDT).

*/
DALResult
PlatformInfo_CDTConfigPostDDR (void * pCDTConfig);


/*=========================================================================
FUNCTION  PlatformInfo_Init
===========================================================================*/
/**
  Initialize the PlatformInfo driver.
*/
DALResult PlatformInfo_Init (void);

#endif
