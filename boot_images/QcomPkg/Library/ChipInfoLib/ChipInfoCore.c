/*==============================================================================

FILE:      ChipInfoCore.c

DESCRIPTION:
  This file implements the parts of the ChipInfo driver specific to XBL Core

PUBLIC CLASSES:
  ChipInfo_Init

================================================================================
        Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
================================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoCore.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
==============================================================================*/

/*==============================================================================
      Include Files
==============================================================================*/
#include "ChipInfo.h"
#include "ChipInfoLocal.h"
#include "DALStdErr.h"
#include "DALStdDef.h"
#include "ChipInfoImage.h"
#include "PlatformInfoDefs.h"

/*=========================================================================
      Data Definitions
==========================================================================*/

/*
 * Internal structure to hold chip info.
 *
 * During XBL Core, ChipInfoDxe init pulls data from the SMEM region
 * and stores it in here.
 *
 * The query APIs read data from this structure.
 */
ChipInfoCtxtType ChipInfoCtxt;

/*==============================================================================
      Functions
==============================================================================*/

/*=============================================================================

  FUNCTION      ChipInfo_Init

  DESCRIPTION   Initialize the XBL Core chip info driver  by reading the
                SMEM buffer populated by PlatformInfo during XBL Loader phase

==============================================================================*/
DALResult ChipInfo_Init
(
  void
)
{
  //uint32 nSize = 0;
  uint32 *aCPUClusters, *aDefectiveParts;
  uint32 nClusterArraySize;
  DalPlatformInfoSMemType *pSMem;

  if (ChipInfoCtxt.bInitComplete)
  {
    return DAL_SUCCESS;
  }

  // Get the address of the SoCInfo structure using the
  // image-specific API
  pSMem = ChipInfo_GetSocInfo();

  if (pSMem == NULL)
  {
    // This will only happen in pre-sil on a standalone build.
    // Return SUCCESS using the UNKNOWN values for each field,
    // and let the information be set manually. However, mark the driver as
    // initialized, to record that this has happened.
    ChipInfo_strcpy(ChipInfoCtxt.szChipIdString, "UNKNOWN", CHIPINFO_MAX_ID_LENGTH);
    ChipInfoCtxt.bInitComplete = TRUE;
    return DAL_SUCCESS;
  }

  // Read the chip information from SMem
  ChipInfoCtxt.nRawPartNum = pSMem->nRawChipId;
  ChipInfoCtxt.eChipInfoId = (ChipInfoIdType)pSMem->eChipId;
  ChipInfoCtxt.nRevNumber = pSMem->nRawChipVersion;
  ChipInfoCtxt.nVersion = pSMem->nChipVersion;
  ChipInfoCtxt.eFamilyId = (ChipInfoFamilyType)pSMem->nChipFamily;
  ChipInfoCtxt.nFamilyDeviceId = pSMem->nRawDeviceNumber;
  ChipInfoCtxt.nRawFamilyId = pSMem->nRawDeviceFamily;
  ChipInfoCtxt.eFoundryId = (ChipInfoFoundryIdType)pSMem->nFoundryId;
  ChipInfoCtxt.nSerialNum = pSMem->nChipSerial;
  ChipInfoCtxt.nQFPROMChipId = pSMem->nQFPROMChipId;
  ChipInfoCtxt.nModemSupported = pSMem->nModemSupported;
  ChipInfo_strcpy(ChipInfoCtxt.szChipIdString, pSMem->szChipIdStr, CHIPINFO_MAX_ID_LENGTH);

  if (pSMem->nFormat >= DALPLATFORMINFO_VERSION(0, 14))
  {
    ChipInfoCtxt.nNumClusters = pSMem->nNumClusters;

    // Avoid crashes from DALSYS_Malloc on some images if size == 0
    if (ChipInfoCtxt.nNumClusters > 0)
    {
      aCPUClusters = (uint32 *)((uintnt)pSMem + pSMem->nClusterArrayOffset);
      nClusterArraySize = ChipInfoCtxt.nNumClusters * sizeof(uint32);

      ChipInfoCtxt.aCPUClusters = ChipInfo_malloc(nClusterArraySize);
      if (ChipInfoCtxt.aCPUClusters != NULL)
      {
        ChipInfo_memcpy(ChipInfoCtxt.aCPUClusters, nClusterArraySize, aCPUClusters, nClusterArraySize);
      }
    }

    aDefectiveParts = (uint32 *)((uintnt)pSMem + pSMem->nDefectivePartsArrayOffset);
    ChipInfo_memcpy(ChipInfoCtxt.aDefectiveParts, CHIPINFO_NUM_PARTS * sizeof(uint32),
        aDefectiveParts, pSMem->nNumDefectiveParts * sizeof(uint32));
  }

  ChipInfo_UnmapSMem();
  ChipInfoCtxt.bInitComplete = TRUE;
  return DAL_SUCCESS;

} /* END ChipInfo_DriverInit */


