/*==============================================================================
  @file ChipInfoLoader.c

  ChipInfo functions for the XBL Loader ChipInfo Driver.

================================================================================
        Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
================================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoLoader.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

==============================================================================*/

/*==============================================================================
      Include Files
==============================================================================*/
#include "string.h"

#include "HALhwio.h"
#include "ChipInfoHWIO.h"
#include "ChipInfoLocal.h"
#include "ChipInfoLoader.h"
#include "ChipInfoImage.h"  // perhaps a better name would be ChipInfoUtils.h

/*=========================================================================
      Data Definitions
==========================================================================*/

/*
 * Internal structure to hold chip info.
 *
 * During XBL Loader, this is populated by reading registers. PlatformInfo then
 * populates an SMEM region by calling into the APIs.
 *
 * The query APIs read data from this structure.
 */
ChipInfoCtxtType ChipInfoCtxt;


/*==============================================================================
**  Functions
**============================================================================*/

/*==============================================================================
  FUNCTION      ChipInfo_Init

  DESCRIPTION   Initialize the chip info driver
                This function reads the HW registers that store relevant
                chip information. This info is stored internally, until
                PlatformInfo calls into ChipInfo APIs to populate its
                shared SMEM region.

  RETURNS       TRUE if initialization and register reads were successful
                FALSE if initialization failed
==============================================================================*/

DALResult ChipInfo_Init
(
 void
)
{
  DALResult retval = DAL_SUCCESS;
  uint32 MajorVersion, MinorVersion, RawArchFamilyId, RawDeviceId, RawFoundryId;
  uint32 LUTIdx, DefaultChipIdLUTIdx, ArchFamilyArrayIdx;
  ChipInfoIdType eDefaultChipId;
  ChipInfoArchFamilyType eArchFamilyId;
  ChipInfoFamilyType eFamilyId;
  ChipInfoArchFamilyDataType ArchFamilyData = { (ChipInfoArchFamilyType)0 };
  ChipInfoBSPType *pBSP = ChipInfo_GetBSPData();
  const ChipInfoDataLUTType *pLUT = pBSP->aLUT;
  const ChipInfoArchFamilyDataType *paArchFamilyArray = pBSP->aArchFamilyData;
  const uint32 nLUTSize = pBSP->nNumLUTEntries;
  const uint32 nArchFamilyDataEntries = pBSP->nNumArchFamilyDataEntries;

  if (ChipInfoCtxt.bInitComplete)
  {
    /* Nothing more to be done  */
    return DAL_SUCCESS;
  }

  /*
   * Read each register and parse out the relevant info.
   * Some of this info needs to be validated before being stored
   */

  ChipInfoCtxt.nRawPartNum          = HWIO_INF(CHIPINFO_TLMM_HW_REVISION_NUMBER, PRODUCT_DEVICE_ID);
  ChipInfoCtxt.nRevNumber           = HWIO_INF(CHIPINFO_TLMM_HW_REVISION_NUMBER, VERSION_ID);

  MajorVersion                      = HWIO_INF(CHIPINFO_TCSR_SOC_HW_VERSION, MAJOR_VERSION);
  MinorVersion                      = HWIO_INF(CHIPINFO_TCSR_SOC_HW_VERSION, MINOR_VERSION);
  ChipInfoCtxt.nVersion             = CHIPINFO_VERSION(MajorVersion, MinorVersion);
  RawArchFamilyId                   = HWIO_INF(CHIPINFO_TCSR_SOC_HW_VERSION, FAMILY_NUMBER);
  RawDeviceId                       = HWIO_INF(CHIPINFO_TCSR_SOC_HW_VERSION, DEVICE_NUMBER);

  RawFoundryId                      = HWIO_INF(CHIPINFO_FOUNDRY_ID_REG, FOUNDRY_ID);

  ChipInfoCtxt.nSerialNum           = HWIO_INF(CHIPINFO_SERIAL_NUM_REG, SERIAL_NUM);

  ChipInfoCtxt.nQFPROMChipId        = HWIO_INF(CHIPINFO_QFPROM_CHIP_ID_REG, CHIP_ID);


  /* Validate the raw values, where necessary */

  /*
   * Process the foundry Id.
   * Account for the additional CHIPINFO_FOUNDRY_ID_UNKNOWN, which
   * isn't actually a foundry ID reported by HW
   */
  if (RawFoundryId < CHIPINFO_NUM_FOUNDRYIDS - 1)
  {
    ChipInfoCtxt.eFoundryId = (ChipInfoFoundryIdType)(RawFoundryId + 1);
  }
  else
  {
    ChipInfoCtxt.eFoundryId = CHIPINFO_FOUNDRYID_UNKNOWN;
  }

  /* Parse the internal family ID using the architectural family info from HW */
  ChipInfoCtxt.nRawFamilyId = RawArchFamilyId;
  ChipInfoCtxt.nFamilyDeviceId = RawDeviceId;
  if (RawArchFamilyId >= CHIPINFO_NUM_ARCH_FAMILIES - 1 )
  {
    ChipInfoCtxt.eFamilyId = CHIPINFO_FAMILY_UNKNOWN;
    eArchFamilyId = CHIPINFO_ARCH_FAMILY_UNKNOWN;
  }
  else
  {
    /*
     * eArchFamilyId is going to be used to search the BSP
     * for the internal family ID corresponding to
     * RawArchFamilyId and RawDeviceNum. RawArchFamilyId doesn't account
     * for CHIPINFO_ARCH_FAMILY_UNKNOWN, so account for it
     * while converting it to its enum value
     */
    eArchFamilyId = (ChipInfoArchFamilyType)(RawArchFamilyId + 1);
  }

  /*
   * Search the arch family array for a family ID corresponding to
   * the HW arch family value and device number
   */
  eFamilyId = CHIPINFO_FAMILY_UNKNOWN;
  eDefaultChipId = CHIPINFO_ID_UNKNOWN;
  for (ArchFamilyArrayIdx = 0; ArchFamilyArrayIdx < nArchFamilyDataEntries; ArchFamilyArrayIdx++)
  {
    ArchFamilyData = paArchFamilyArray[ArchFamilyArrayIdx];
    if(ArchFamilyData.eArchFamily == eArchFamilyId && ArchFamilyData.nArchFamilyDeviceNum == RawDeviceId)
    {
      eFamilyId = ArchFamilyData.eDeviceFamily;
      eDefaultChipId = ArchFamilyData.eDefaultChipId;
      break;
    }
  }

  ChipInfoCtxt.eFamilyId = eFamilyId;

  /* Get the chip's name, internal ID, and supported modems from the LUT  */
  LUTIdx = 0;
  DefaultChipIdLUTIdx = 0;
  for (LUTIdx = 0; LUTIdx < nLUTSize; LUTIdx++)
  {
    if (pLUT[LUTIdx].nPartNum == ChipInfoCtxt.nRawPartNum)
    {
      break;
    }
    else if (pLUT[LUTIdx].eChipId == eDefaultChipId)
    {
      /*
       * Record the index of the defualt info so it can be retrieved
       * without looping through the LUT again
       */
      DefaultChipIdLUTIdx = LUTIdx;
    }
  }

  if (LUTIdx == nLUTSize)
  {
    /*
     * Searched the LUT without finding a matching part number, so try using
     * the index of the default chip ID. If one wasn't provided, or if the default
     * wasn't found in the LUT either, this defaults to 0 (UNKNOWN)
     */
    LUTIdx = DefaultChipIdLUTIdx;
  }

  ChipInfoCtxt.eChipInfoId = pLUT[LUTIdx].eChipId;

  ChipInfo_strcpy(ChipInfoCtxt.szChipIdString, pLUT[LUTIdx].szChipIdStr,
      sizeof(ChipInfoCtxt.szChipIdString));

  /*
   * Get the supported modems from the BSP
   * TODO: Decide on what to do with modem support
   */
  ChipInfoCtxt.nModemSupported = pLUT[LUTIdx].nModem;

  /* Fill in Partial Goods information  */
  ChipInfo_InitChipset(&ChipInfoCtxt);

  ChipInfoCtxt.bInitComplete  = TRUE;

  return retval;
}

