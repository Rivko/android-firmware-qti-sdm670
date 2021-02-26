#ifndef DDIPLATFORMINFO_H
#define DDIPLATFORMINFO_H

/**
@file DDIPlatformInfo.h
@brief 
Public interface include file for accessing the PlatformInfo DAL.
*/

/*=============================================================================
                             Edit History

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/16/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
09/07/10   mdt     Initial revision
===============================================================================
Copyright (c) 2010 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "PlatformInfoDefs.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "DALSys.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/**
  @addtogroup platform_info Platform Information
  @{
*/

/**
  Defines the interface version.
 */
#define DALPLATFORMINFO_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


/*=========================================================================
      DAL glue code
==========================================================================*/


/** @cond
*/
typedef struct DalPlatformInfo DalPlatformInfo;
struct DalPlatformInfo
{
   struct DalDevice DalDevice;
   DALResult (*GetPlatform)(DalDeviceHandle * _h,  DalPlatformInfoPlatformType * pType);
   DALResult (*GetPlatformInfo)(DalDeviceHandle * _h,  DalPlatformInfoPlatformInfoType * pInfo);
   DALResult (*GetKeyValue)(DalDeviceHandle * _h,  DalPlatformInfoKeyType eKey, uint32 *pValue);
   DALResult (*CDTConfigPreDDR)(DalDeviceHandle * _h, void *platformAddress);
   DALResult (*CDTConfigPostDDR)(DalDeviceHandle * _h, void *platformAddress);
};

typedef struct DalPlatformInfoHandle DalPlatformInfoHandle;
struct DalPlatformInfoHandle
{
   uint32 dwDalHandleId;
   const DalPlatformInfo * pVtbl;
   void * pClientCtxt;
};

#define DAL_PlatformInfoDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALPLATFORMINFO_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Function Definitions
==========================================================================*/


/*=========================================================================
FUNCTION  DalPlatformInfo_GetPlatform
===========================================================================*/
/**
  Returns the current platform in the parameter pType. 

  @param[in] _h     Handle to the PlatformInfo DAL device.
  @param[out] pType Variable to return to the caller.

  @return
  DAL_SUCCESS -- Platform identified and placed in pType. \n
  DAL_ERROR -- Invalid parameter.

  @dependencies
  None.

  @sa
  DalPlatformInfo_Platform().
*/
static __inline DALResult
DalPlatformInfo_GetPlatform(DalDeviceHandle * _h,  DalPlatformInfoPlatformType * pType)
{
   return ((DalPlatformInfoHandle *)_h)->pVtbl->GetPlatform( _h, pType);
}


/*=========================================================================
FUNCTION  DalPlatformInfo_GetPlatformInfo
===========================================================================*/
/**
  Returns information on the current platform.

  This function returns information about the current platform in the parameter
  pInfo.

  @param[in] _h     Handle to the PlatformInfo DAL device.
  @param[out] pInfo Variable to return to the caller.

  @return
  DAL_SUCCESS -- Platform identified and placed in pType. \n
  DAL_ERROR   -- Invalid parameter.

  @dependencies
  None.
*/
static __inline DALResult
DalPlatformInfo_GetPlatformInfo (DalDeviceHandle * _h,  DalPlatformInfoPlatformInfoType * pInfo)
{
   return ((DalPlatformInfoHandle *)_h)->pVtbl->GetPlatformInfo( _h, pInfo);
}


/*=========================================================================
FUNCTION  DalPlatformInfo_GetKeyValue
===========================================================================*/
/**
  Get a key value.

  This function returns a key value stored in the CDT.

  @param[in]  _h     Handle to the PlatformInfo DAL device.
  @param[in]  eKey   Key to get the value for.
  @param[out] pValue Key value.

  @return
  DAL_SUCCESS -- Key found and value stored in pValue. \n
  DAL_ERROR -- Key not found.

  @dependencies
  None.
*/
static __inline DALResult
DalPlatformInfo_GetKeyValue (DalDeviceHandle * _h, DalPlatformInfoKeyType eKey, uint32 *pValue)
{
   return ((DalPlatformInfoHandle *)_h)->pVtbl->GetKeyValue( _h, eKey, pValue);
}


/*=========================================================================
FUNCTION  DalPlatformInfo_CDTConfigPreDDR
===========================================================================*/
/**
  Configure the PlatformInfo driver with data from the CDT.

  This functions configures the PlatformInfo driver with data from the CDT,
  located at the parameter pCDTConfig. To be called before DDR is available.

  @param[in] _h         Handle to the PlatformInfo DAL device.
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
static __inline DALResult
DalPlatformInfo_CDTConfigPreDDR (DalDeviceHandle * _h,  void * pCDTConfig)
{
   return ((DalPlatformInfoHandle *)_h)->pVtbl->CDTConfigPreDDR( _h, pCDTConfig);
}

/*=========================================================================
FUNCTION  DalPlatformInfo_CDTConfigPostDDR
===========================================================================*/
/**
  Configure the PlatformInfo driver with data from the CDT.

  This functions configures the PlatformInfo driver with data from the CDT,
  located at the parameter pCDTConfig. To be called after DDR is available. If
  DalPlatformInfo_CDTConfigPreDDR has already been called, then the pCDTConfig
  parameter is optional.

  @param[in] _h         Handle to the PlatformInfo DAL device.
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
static __inline DALResult
DalPlatformInfo_CDTConfigPostDDR (DalDeviceHandle * _h,  void * pCDTConfig)
{
   return ((DalPlatformInfoHandle *)_h)->pVtbl->CDTConfigPostDDR( _h, pCDTConfig);
}


/*=========================================================================
FUNCTION  DalPlatformInfo_Platform
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
static __inline DalPlatformInfoPlatformType
DalPlatformInfo_Platform(void)
{
  static DalDeviceHandle        *phPlatformInfo = NULL;
  DALResult                     eResult; 
  DalPlatformInfoPlatformType   platform;

  if (phPlatformInfo == NULL)
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfo);
    if (eResult != DAL_SUCCESS)
    {
      return DALPLATFORMINFO_TYPE_UNKNOWN;
    }
  }

  DalPlatformInfo_GetPlatform(phPlatformInfo, &platform);
  return platform;

} /* END DalPlatformInfo_Platform */


/*=========================================================================
FUNCTION  DalPlatformInfo_IsFusion
===========================================================================*/
/**
  Return if the platform is a Fusion variant.

  @return
  TRUE -- Platform is Fusion.
  FALSE -- Platform isn't Fusion.

  @dependencies
  None.

  @sa
  DalPlatformInfo_GetPlatformInfo().
*/
static __inline boolean
DalPlatformInfo_IsFusion (void)
{
  static DalDeviceHandle          *phPlatformInfo = NULL;
  DALResult                        eResult;
  DalPlatformInfoPlatformInfoType  PlatformInfo;

  if (phPlatformInfo == NULL)
  {
    eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfo);
    if (eResult != DAL_SUCCESS)
    {
      return 0;
    }
  }

  DalPlatformInfo_GetPlatformInfo(phPlatformInfo, &PlatformInfo);
  return PlatformInfo.fusion;

}

/** @} */ /* end_addtogroup platform_info */

#endif

