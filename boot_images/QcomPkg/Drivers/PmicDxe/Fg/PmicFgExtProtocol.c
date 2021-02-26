/*! @file PmicFgExtProtocol.c
 *
 *  PMIC-FG MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support
 *  the PMIC FG Enahnced module.
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
03/28/16   va     Adding logging support
03/21/16   va     New File.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/**
  EFI interfaces
 */
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIPmicFg.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/BaseMemoryLib.h>

#include "DDIPlatformInfo.h"

/**
SPMI depedency
*/
#include "SpmiBus.h"

/**
  PMIC Lib interfaces
 */
#include "pm_uefi.h"
#include "PmicFileLog.h"
#include "PmicFgCommon.h"

#include "pm_fg_batt_soc.h"
//#include "pm_fg_adc_usr.h"
#include "pm_fg_batt_info.h"
#include "pm_fg_batt_soc.h"
#include "pm_fg_bcl.h"
#include "pm_version.h"
#include "pm_schg_batif.h"
#include "pm_schg_chgr.h"
#include "pm_schg_misc.h"
#include "pm_fg_mem_if.h"
#include <../../Library/PmicLib/utils/inc/pm_utils.h>
#include "pm_err_flags.h"
#include "pm_core_utils.h"

/**
  PMIC Fg interface
 */
#include <PmicFgBattProfile.h>
#include <PmicFgSram.h>

/**
QCom Lib dependency
*/
//#include <Library/QcomLib.h>
//#include <Library/MemoryAllocationLib.h>
#include "string.h"
#include <Library/BaseMemoryLib.h>

extern EFI_PM_FG_CFGDATA_TYPE gFgConfigData;

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/**
  PMIC FG UEFI Protocol implementation
 */
EFI_QCOM_PMIC_FG_EXT_PROTOCOL PmicFgExtProtocolImplementation =
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

