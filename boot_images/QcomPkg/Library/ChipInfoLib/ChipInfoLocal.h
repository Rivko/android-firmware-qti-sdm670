#ifndef CHIPINFOLOCAL_H
#define CHIPINFOLOCAL_H

/*==========================================================================

FILE:   ChipInfoLocal.h

DESCRIPTION:  Internal function and data structure declarations for ChipInfo
              shared across all images.

PUBLIC CLASSES:  Not Applicable

============================================================================
       Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
                       All Rights Reserved.
              QUALCOMM Proprietary and Confidential.
============================================================================

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ChipInfoLib/ChipInfoLocal.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

==========================================================================*/

#include "ChipInfo.h"
#include "ChipInfoDefs.h"

#include "com_dtypes.h"  // Needed for mpss compilation

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/**
 * ChipInfoType
 *
 * Container for all chip info
 */
typedef struct
{
  boolean                     bInitComplete;        /*  Driver has been initialized */
  uint32                      nRawPartNum;          /*  JTAG ID/Device Number */
  ChipInfoIdType              eChipInfoId;          /*  Sequential ID, unique across all chips */
  ChipInfoVersionType         nVersion;             /*  Major and Minor versions   */
  uint32                      nRevNumber;           /*  HW Revision number. e.g. 0 = v1.0, 1 = v1.1, 2 = v2.0 */
  ChipInfoFamilyType          eFamilyId;            /*  Internal ChipInfo family (not architectural family)  */
  uint32                      nFamilyDeviceId;      /*  Device ID within this family, unique within the family */
  uint32                      nRawFamilyId;         /*  Architectural family number  */
  ChipInfoFoundryIdType       eFoundryId;           /*  The foundry in which the chip was manufactured  */
  ChipInfoSerialNumType       nSerialNum;           /*  Unique (within chip family) serial number  */
  ChipInfoQFPROMChipIdType    nQFPROMChipId;        /*  Chip ID read from QFPROM fuses  */
  ChipInfoModemType           nModemSupported;      /*  Modem hardware is/isnt' supported on this platform */
  char                        szChipIdString[CHIPINFO_MAX_ID_LENGTH]; /*  The chip's name as a string, e.g. "MSM8998" */
  uint32                      nNumClusters;
  uint32                      *aCPUClusters;        /*  Bitmasks of defective CPU cores for each of nClusters clusters */
  uint32                      aDefectiveParts[CHIPINFO_NUM_PARTS];  /*  Set of parts marked as defective */
} ChipInfoCtxtType;



#endif /* CHIPINFOLOCAL_H */
