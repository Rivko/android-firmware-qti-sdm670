/*============================================================================

 FILE:      ChipInfo.c

 DESCRIPTION:
 This file implements the Chip Info APIs common to both XBL Loader and Core

 PUBLIC CLASSES:
 ChipInfo_GetChipVersion
 ChipInfo_GetRawChipVersion
 ChipInfo_GetChipId
 ChipInfo_GetRawChipId
 ChipInfo_GetChipIdString
 ChipInfo_GetChipFamily
 ChipInfo_GetMarketingNameString
 ChipInfo_GetModemSupport
 ChipInfo_GetSerialNumber
 ChipInfo_GetFoundryId
 ChipInfo_GetRawDeviceFamily
 ChipInfo_GetRawDeviceNumber
 ChipInfo_GetQFPROMChipId
 ChipInfo_GetDefectiveParts

 ============================================================================
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 ============================================================================

 $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfo.c#1 $
 $DateTime: 2017/09/18 09:26:13 $
 $Author: pwbldsvc $

==========================================================================*/

/*=========================================================================
 Include Files
 ==========================================================================*/

#include "ChipInfoLocal.h"
#include "DALStdErr.h"
#include "DALStdDef.h"
#include "ChipInfoImage.h"

/*=========================================================================
      Data Definitions
==========================================================================*/
extern ChipInfoCtxtType ChipInfoCtxt;


/*=========================================================================
 Functions
 ==========================================================================*/

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetChipVersion
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoVersionType ChipInfo_GetChipVersion(void)
{
  return ChipInfoCtxt.nVersion;
} /* END ChipInfo_GetChipVersion */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetRawChipVersion
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

uint32 ChipInfo_GetRawChipVersion(void)
{
  return ChipInfoCtxt.nRevNumber;
} /* END ChipInfo_GetRawChipVersion */

/* =========================================================================
 **  FUNCTION      Chipinfo_GetChipId
 **
 **  DESCRIPTION   See ChipInfo.h
 ** =========================================================================*/

ChipInfoIdType ChipInfo_GetChipId(void)
{
  return ChipInfoCtxt.eChipInfoId;
} /*  END ChipInfo_GetChipId  */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetRawChipId
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

uint32 ChipInfo_GetRawChipId(void)
{
  return ChipInfoCtxt.nRawPartNum;
} /*  END ChipInfo_GetRawChipId    */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetChipIdString
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

DALResult ChipInfo_GetChipIdString(char *szChipIdStr, uint32 nMaxLen)
{
  uint32 len = 0;
  if (nMaxLen < CHIPINFO_MAX_ID_LENGTH)
  {
    len = nMaxLen;
  }
  else
  {
    len = CHIPINFO_MAX_ID_LENGTH;
  }

  ChipInfo_strcpy(szChipIdStr, ChipInfoCtxt.szChipIdString, len);
  return DAL_SUCCESS;
} /*  END ChipInfo_GetChipIdString    */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetChipFamily
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoFamilyType ChipInfo_GetChipFamily(void)
{
  return ChipInfoCtxt.eFamilyId;
} /* END Chipinfo_GetChipFamily */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetMarketingNameString
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

DALResult ChipInfo_GetMarketingNameString(char *pszMarketingNameStr, uint32 nMaxlen)
{
  ChipInfo_GetChipIdString(pszMarketingNameStr, nMaxlen);
  return DAL_SUCCESS;
} /*  END ChipInfo_GetMarketingNameString    */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetModemSupport
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoModemType ChipInfo_GetModemSupport(void)
{
  return ChipInfoCtxt.nModemSupported;
} /* END Chipinfo_GetModemSupport */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetSerialNumber
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoSerialNumType ChipInfo_GetSerialNumber(void)
{
  return ChipInfoCtxt.nSerialNum;
} /*  END ChipInfo_GetSerialNumber    */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetFoundryId
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoFoundryIdType ChipInfo_GetFoundryId(void)
{
  return ChipInfoCtxt.eFoundryId;
} /*  END ChipInfo_GetFoundryId  */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetRawChipDeviceFamily
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

uint32 ChipInfo_GetRawDeviceFamily(void)
{
  return ChipInfoCtxt.nRawFamilyId;
} /* END ChipInfo_GetRawDeviceFamily */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetRawChipDeviceNumber
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

uint32 ChipInfo_GetRawDeviceNumber(void)
{
  return ChipInfoCtxt.nFamilyDeviceId;
} /* END ChipInfo_GetRawDeviceFamily */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetQFPROMChipId
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

ChipInfoQFPROMChipIdType ChipInfo_GetQFPROMChipId(void)
{
  return ChipInfoCtxt.nQFPROMChipId;
} /* END ChipInfo_GetQFPROMChipId */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetDefectivePart
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */

DALResult ChipInfo_GetDefectivePart(ChipInfoPartType ePart, uint32 *pnMask)
{
  if (pnMask == NULL)
  {
    return DAL_ERROR;
  }

  if (ePart >= CHIPINFO_NUM_PARTS)
  {
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  *pnMask = ChipInfoCtxt.aDefectiveParts[ePart];
  return DAL_SUCCESS;
} /* END ChipInfo_GetDefectivePart */

/* ===========================================================================
 **  FUNCTION      ChipInfo_GetDefectiveCPUs
 **
 **  DESCRIPTION   See ChipInfo.h
 ** ======================================================================== */
DALResult ChipInfo_GetDefectiveCPUs(uint32 nCPUCluster, uint32 *pnMask)
{
  if (pnMask == NULL)
  {
    return DAL_ERROR;
  }

  if (nCPUCluster >= ChipInfoCtxt.nNumClusters)
  {
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  *pnMask = ChipInfoCtxt.aCPUClusters[nCPUCluster];
  return DAL_SUCCESS;
} /* END ChipInfo_GetDefectiveCPUs */

