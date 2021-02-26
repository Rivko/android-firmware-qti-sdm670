#ifndef DDICHIPINFO_H
#define DDICHIPINFO_H
/**
* @file DDIChipInfo.h
* @brief ChipInfo Driver Interface Header File.
* This file contains the definitions of the constants, data
* structures, and interfaces that comprise the DAL Layer of the
* ChipInfo driver.
*
* Only the interfaces declared shall be used by the client for
* accessing the ChipInfo DAL driver.
*
* If ChipInfo is needed before DAL comes up, use the functions
* from ChipInfo.h
*/

/*
===========================================================================

FILE:         DDIChipInfo.h

DESCRIPTION:
  This is the DAL interface specification for the chip driver/service.

===========================================================================
             Copyright (c) 2011,2014, 2016-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/DDIChipInfo.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "DALDeviceId.h"
#include "ChipInfoDefsLegacy.h"
#include "ChipInfoDefs.h"

/**
  @addtogroup macros
  @{
*/

/**
 * Macro that defines the interface version.
 */
#define DALCHIPINFO_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/**
  @}
*/ /* end_group macros */

/** @cond */

typedef struct DalChipInfo DalChipInfo;
struct DalChipInfo
{
   DalDevice DalChipInfoDevice;
   DALResult (*GetChipVersion)(DalDeviceHandle * _h, ChipInfoVersionType * pnVersion);
   DALResult (*GetRawChipVersion)(DalDeviceHandle * _h, uint32 * pnVersion);
   DALResult (*GetChipId)(DalDeviceHandle * _h, ChipInfoIdType * peId);
   DALResult (*GetRawChipId)(DalDeviceHandle * _h, uint32 * pnId);
   DALResult (*GetChipIdString)(DalDeviceHandle * _h, char * szIdString, uint32  nMaxLength);
   DALResult (*GetChipFamily)(DalDeviceHandle * _h, ChipInfoFamilyType * peFamily);
   DALResult (*GetModemSupport)(DalDeviceHandle * _h, ChipInfoModemType * pnModem);
   DALResult (*GetMarketingNameString)(DalDeviceHandle * _h, char * pszMarketingNameStr, uint32  nMaxLength);
   DALResult (*GetSerialNumber)(DalDeviceHandle * _h, ChipInfoSerialNumType * pnSerialNum);
   DALResult (*GetFoundryId)(DalDeviceHandle * _h, ChipInfoFoundryIdType * peFoundryId);
   DALResult (*GetRawDeviceFamily)(DalDeviceHandle * _h, uint32 * pnId);
   DALResult (*GetRawDeviceNumber)(DalDeviceHandle * _h, uint32 * pnId);
   DALResult (*GetQFPROMChipId)(DalDeviceHandle * _h, ChipInfoQFPROMChipIdType * pnChipId);
   DALResult (*GetDefectivePart)(DalDeviceHandle * _h, ChipInfoPartType ePart, uint32 *pnMask);
   DALResult (*GetDefectiveCPUs)(DalDeviceHandle * _h, uint32 nCPUCluster, uint32 *pnMask);

};

typedef struct DalChipInfoHandle DalChipInfoHandle;
struct DalChipInfoHandle
{
   uint32 dwDalHandleId;
   const DalChipInfo * pVtbl;
   void * pClientCtxt;
};

#define DAL_ChipInfoDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALCHIPINFO_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/**
  @addtogroup functions
  @{
*/


/* ============================================================================
**  Function : DalChipInfo_GetChipVersion
** ============================================================================
*/
/**
  Returns the version of the chip.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnVersion  Pointer to a
                             ChipInfoVersionType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipVersion(DalDeviceHandle * _h,  ChipInfoVersionType * pnVersion)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetChipVersion(_h, pnVersion);
}

/* ============================================================================
**  Function : DalChipInfo_GetRawChipVersion
** ============================================================================
*/
/**
  Returns the version of the chip as read from the hardware register.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnVersion  Pointer to a uint32 passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawChipVersion(DalDeviceHandle * _h,  uint32 * pnVersion)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetRawChipVersion( _h, pnVersion);
}

/* ============================================================================
**  Function : DalChipInfo_GetChipId
** ============================================================================
*/
/**
  Returns the ChipId.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId  Pointer to a
                             ChipInfoIdType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipId(DalDeviceHandle * _h,  ChipInfoIdType * peId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetChipId( _h, peId);
}

/* ============================================================================
**  Function : DalChipInfo_GetRawChipId
** ============================================================================
*/
/**
  Returns the ChipId as read from the hardware register.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnId       Pointer to a uint32 passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawChipId(DalDeviceHandle * _h,  uint32 * pnId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetRawChipId( _h, pnId);
}

/* ============================================================================
**  Function : DalChipInfo_GetChipIdString
** ============================================================================
*/
/**
  Returns the string representing the chip name.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  szIdString Pointer passed by the caller that
                             will be populated by the driver.
  @param[in]    nMaxLength   Length of the string  to be passed by
                             the caller. CHIPINFO_MAX_ID_LENGTH
                             is the maximum length supported.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

  @sa
  CHIPINFO_MAX_ID_LENGTH
*/
static __inline DALResult
DalChipInfo_GetChipIdString(DalDeviceHandle * _h,  char * szIdString,  uint32  nMaxLength)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetChipIdString(_h, szIdString, nMaxLength);
}

/* ============================================================================
**  Function : DalChipInfo_GetChipFamily
** ============================================================================
*/
/**
  Returns the family of the chip.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peFamily  Pointer to a ChipInfoFamilyType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipFamily(DalDeviceHandle * _h,  ChipInfoFamilyType * peFamily)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetChipFamily(_h, peFamily);
}

/* ============================================================================
**  Function : DalChipInfo_GetModemSupport
** ============================================================================
*/
/**
  Returns the Modem support information of the chip.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnModem    Pointer to a ChipInfoModemType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetModemSupport(DalDeviceHandle * _h,  ChipInfoModemType * pnModem)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetModemSupport(_h, pnModem);
}


/*=========================================================================
FUNCTION  DalChipInfo_ChipVersion
===========================================================================*/
/**
  Returns the version of the chip.

  This function is a wrapper around DalChipInfo_GetChipVersion. It provides
  the chip version without the code overhead of attaching to the DAL.

  @return
  ChipInfoVersionType version of the chip.

  CHIPINFO_VERSION_UNKNOWN  -- Version detection failed because of
                               unsuccessful attach to DAL device.

  @sa
  DalChipInfo_GetChipVersion
*/
static __inline ChipInfoVersionType
DalChipInfo_ChipVersion(void)
{
  static DalDeviceHandle        *phChipInfo = NULL;
  DALResult                     eResult;
  ChipInfoVersionType           nVersion;

  if (phChipInfo == NULL)
  {
    eResult =
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return CHIPINFO_VERSION_UNKNOWN;
    }
  }

  DalChipInfo_GetChipVersion(phChipInfo, &nVersion);

  return nVersion;

}



/*=========================================================================
FUNCTION  DalChipInfo_ChipId
===========================================================================*/
/**
  Returns the ChipId.

  This function is a wrapper around DalChipInfo_GetChipId. It provides
  the chip ID without the code overhead of attaching to the DAL.

  @return
  ChipInfoIdType   neId

  CHIPINFO_ID_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetChipId
*/
static __inline DalChipInfoIdType
DalChipInfo_ChipId(void)
{
  static DalDeviceHandle        *phChipInfo = NULL;
  DALResult                     eResult;
  ChipInfoIdType                neId;

  if (phChipInfo == NULL)
  {
    eResult =
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return CHIPINFO_ID_UNKNOWN;
    }
  }

  DalChipInfo_GetChipId(phChipInfo, &neId);

  return neId;

}


/*=========================================================================
FUNCTION  DalChipInfo_ChipFamily
===========================================================================*/
/**
  Returns the family of the chip.

  This function is a wrapper around DalChipInfo_GetChipFamily. It provides
  the chip family without the code overhead of attaching to the DAL.

  @return
  ChipInfoFamilyType   peFamily

  CHIPINFO_FAMILY_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetChipFamily
*/
static __inline ChipInfoFamilyType
DalChipInfo_ChipFamily(void)
{
  static DalDeviceHandle        *phChipInfo = NULL;
  DALResult                     eResult;
  ChipInfoFamilyType            neFamily;

  if (phChipInfo == NULL)
  {
    eResult =
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return CHIPINFO_FAMILY_UNKNOWN;
    }
  }

  DalChipInfo_GetChipFamily(phChipInfo, &neFamily);

  return neFamily;

}



/* ============================================================================
**  Function : DalChipInfo_GetMarketingNameString
** ============================================================================
*/
/**
  Returns the string representing the marketing name.
  See ChipInfo.h for more information.

  @param[in,out]  _h                    Dal Device Handle given by the caller.
  @param[in,out]  pszMarketingNameStr   Pointer passed by the caller that
                                          will be populated by the driver.
  @param[in]    nMaxLength              Length of the string  to be passed by
                                        the caller. DALCHIPINFO_MAX_NAME_LENGTH
                                        is the maximum length supported.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

  @sa
  DALCHIPINFO_MAX_ID_LENGTH
*/
static __inline DALResult
DalChipInfo_GetMarketingNameString(DalDeviceHandle * _h,  char * pszMarketingNameStr,  uint32  nMaxLength)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetMarketingNameString(_h, pszMarketingNameStr, nMaxLength);
}

/* ============================================================================
**  Function : DalChipInfo_GetSerialNumber
** ============================================================================
*/
/**
  Returns the Chip Serial Number.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a ChipInfoSerialNumType variable
                             passed by the caller that will be populated by
                             the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetSerialNumber(DalDeviceHandle * _h,  ChipInfoSerialNumType * pnSerialNum)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetSerialNumber(_h, pnSerialNum);
}

/* ============================================================================
**  Function : DalChipInfo_GetFoundryId
** ============================================================================
*/
/**
  Returns the FoundryId.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a ChipInfoFoundryIdType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetFoundryId(DalDeviceHandle * _h,  ChipInfoFoundryIdType * peId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetFoundryId(_h, peId);
}


/*=========================================================================
FUNCTION  DalChipInfo_FoundryId
===========================================================================*/
/**
  Returns the FoundryId.

  This function is a wrapper around DalChipInfo_GetFoundryId.
  It provides the Foundry Id without the code overhead of attaching to
  the DAL.

  @return
  ChipInfoFoundryIdType   peId

  CHIPINFO_FOUNDRYID_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetFoundryId
*/

static __inline ChipInfoFoundryIdType
DalChipInfo_FoundryId(void)
{
  static DalDeviceHandle        *phChipInfo = NULL;
  DALResult                     eResult;
  ChipInfoFoundryIdType         peId;

  if (phChipInfo == NULL)
  {
    eResult =
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return CHIPINFO_FOUNDRYID_UNKNOWN;
    }
  }

  DalChipInfo_GetFoundryId(phChipInfo, &peId);

  return peId;

}


/* ============================================================================
**  Function : DalChipInfo_GetRawDeviceFamily
** ============================================================================
*/
/**
  Returns the RawDeviceFamily as read from the TCSR_SOC_VERSION register.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a uint32 type variable
                             passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawDeviceFamily(DalDeviceHandle * _h,  uint32 * pnId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetRawDeviceFamily(_h, pnId);
}


/* ============================================================================
**  Function : DalChipInfo_GetRawDeviceNumber
** ============================================================================
*/
/**
  Returns the RawDeviceNumber as read from the TCSR_SOC_VERSION register.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a uint32 type variable
                             passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawDeviceNumber(DalDeviceHandle * _h,  uint32 * pnId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetRawDeviceNumber(_h, pnId);
}

/* ============================================================================
**  Function : DalChipInfo_GetQFPROMChipId
** ============================================================================
*/
/**
  Returns the QFPROM ChipId.
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a ChipInfoQFPROMChipIdType variable
                             passed by the caller that will be populated by
                             the driver.

  @return
  DAL_SUCCESS always.

  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetQFPROMChipId(DalDeviceHandle * _h,  ChipInfoQFPROMChipIdType * pnQFPROMChipId)
{
   return ((DalChipInfoHandle *)_h)->pVtbl->GetQFPROMChipId(_h, pnQFPROMChipId);
}

/* ============================================================================
**  Function : DalChipInfo_GetDefectivePart
* ============================================================================
*/
/**
  Check if the selected part is marked "Defective" in PTE fuses
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in]     ePart       A ChipInfoPartType to check for defectiveness
  @param[out]    pnMask      Pointer to a uint32 whose contents will be set to a non-zero value
                              if the ePart is defective.

  @return
  DAL_SUCCESS if successful
  DAL_ERROR_OUT_OF_RANGE_PARAMETER if ePart is not a supported ChipInfoPartType
  DAL_ERROR if some other error occured
*/
static __inline DALResult
DalChipInfo_GetDefectivePart(DalDeviceHandle * _h, ChipInfoPartType ePart, uint32 *pnMask)
{
  return ((DalChipInfoHandle *)_h)->pVtbl->GetDefectivePart(_h, ePart, pnMask);
}

/* ============================================================================
**  Function : DalChipInfo_GetDefectiveCPUs
* ============================================================================
*/
/**
  Retrieve the defective cores in the selected CPU cluster
  See ChipInfo.h for more information.

  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in]      nCPUCluster The CPU cluster to check for defectiveness
  @param[out]     pnMask      Pointer to a uint32 whose bits will be set to 1
                                if the corresponding core is defective.

  @return
  DAL_SUCCESS if successful
  DAL_ERROR_OUT_OF_RANGE_PARAMETER if nCPUCluster > number of clusters on the chip
  DAL_ERROR if not successful
*/
static __inline DALResult
DalChipInfo_GetDefectiveCPUs(DalDeviceHandle * _h, uint32 nCPUCluster, uint32 *pnMask)
{
  return ((DalChipInfoHandle *)_h)->pVtbl->GetDefectiveCPUs(_h, nCPUCluster, pnMask);
}
/**
  @}
*/ /* end_group Functions */

#endif /* !DDICHIPINFO_H */

