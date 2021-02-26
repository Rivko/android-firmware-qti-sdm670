/*! @file PmicFgBasicProtocol.c
 *
 *  PMIC-FG MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support
 *  the PMIC FG module.
 *
 *  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
12/28/17   pbitra API to enable BMD explicitly
12/19/16   sm     Added API to get FG PmicIndex
11/07/16   cs     Added API to disable ESR Pulse
05/26/16   va     Update for Profile Load
05/20/16   va     Adding Battery profile load
03/21/16   va     New File
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/**
  EFI interfaces
 */
#include <Protocol/EFIPmicFg.h>

/**
  PMIC common interface file
 */
#include "PmicFgCommon.h"

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
  PMIC FG Basic UEFI Protocol implementation
 */
EFI_QCOM_PMIC_FG_BASIC_PROTOCOL PmicFgBasicProtocolImplementation =
{
  PMIC_FG_REVISION,
  EFI_PmicFgBasicInit,
  EFI_PmicFgExtendedInit,
  EFI_GetBatteryStatus,
  EFI_GetBattProfileStatus,
  EFI_PmicFgGetBatteryId,
  EFI_PmicFgConfigureHwJeita,
  EFI_PmicFgGetHWJeitaStatus,
  EFI_PmicFgGetBattMissingCfg,
  EFI_PmicFgSetBattMissingCfg,
  EFI_PmicFgBasicExit,
  EFI_PmicFgExtendedExit,
  EFI_PmicFgDumpPeripheral,
  EFI_PmicFgDumpFgSram,
  EFI_PmicFgEnableESRPulse,
  EFI_PmicFgBasicGetPmicInfo,
  EFI_PmicFgEnableBMD,
};


