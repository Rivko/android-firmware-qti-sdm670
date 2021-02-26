/*! @file PmicFgCommon.h
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
01/23/17   sm      CFG parameter Structure changes
12/19/16   sm     Added API to get FG PmicIndex
11/07/16   cs     Added API to disable ESR Pulse
10/06/16   cs     configure batt therm/aux therm bias wait 
07/26/16   va     Restarting FG Changes on warm boot 
04/26/16   va     Passing debug cfg data to common Init 
03/28/16   va     Consolidating config data into one struct 
03/21/16   va     New File.
===========================================================================*/

#ifndef __PMICFGCOMMON_H__
#define __PMICFGCOMMON_H__

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/**
  EFI interfaces
 */

#include <Protocol/EFIPmicFg.h>

/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS EFIAPI EFI_PmicFgBasicInit(IN UINT32 PmicDeviceIndex, IN EFI_PM_FG_CFGDATA_TYPE *pFgCfgData);

EFI_STATUS EFIAPI EFI_PmicFgExtendedInit( IN UINT32 PmicDeviceIndex, IN EFI_PM_FG_BATT_PROFILE_DATA *pBattProfileData);

EFI_STATUS EFIAPI EFI_GetBatteryStatus(IN UINT32 PmicDeviceIndex, OUT EFI_PM_FG_BATT_STATUS_INFO *BattStsInfo);

EFI_STATUS EFIAPI EFI_GetBattProfileStatus(IN  UINT32 PmicDeviceIndex, OUT EFI_PM_FG_BATT_PROFILE_STATUS *pBattProfileStatus);

EFI_STATUS EFIAPI EFI_PmicFgGetBatteryId(OUT EFI_PM_BATTERY_ID* BatteryId);

EFI_STATUS EFIAPI EFI_PmicFgBasicExit(IN UINT32 PmicDeviceIndex);

EFI_STATUS EFIAPI EFI_PmicFgExtendedExit(IN UINT32 PmicDeviceIndex);

EFI_STATUS EFIAPI EFI_PmicFgDumpPeripheral( VOID );

EFI_STATUS EFIAPI EFI_PmicFgDumpFgSram( IN UINT32 PmicDeviceIndex);

EFI_STATUS EFIAPI EFI_PmicFgSetBattMissingCfg(IN UINT32 PmicDeviceIndex, IN PM_FG_BATT_INFO_BATT_MISSING_CFG BattMissingCfg);

EFI_STATUS EFIAPI EFI_PmicFgGetBattMissingCfg(IN UINT32 PmicDeviceIndex, OUT PM_FG_BATT_INFO_BATT_MISSING_CFG *pBattMissingCfg);

EFI_STATUS EFIAPI EFI_PmicFgConfigureHwJeita(IN UINT32 PmicDeviceIndex);

EFI_STATUS EFIAPI EFI_PmicFgGetHWJeitaStatus(IN UINT32 PmicDeviceIndex, OUT EFI_PM_FG_BATT_TEMP_STATUS *pHwJeitaStatus);

EFI_STATUS EFIAPI EFI_PmicFgEnableESRPulse(IN UINT32 PmicDeviceIndex, IN BOOLEAN enable);

EFI_STATUS EFIAPI EFI_PmicFgBasicGetPmicInfo(OUT EFI_PM_FG_PMIC_INFO *FgPmicIndex);

EFI_STATUS EFIAPI EFI_PmicFgEnableBMD(IN UINT32 PmicDeviceIndex);


/*===========================================================================
                  LOCAL FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS PmicFgCommon_Init(UINT32 PmicDeviceIndex, IN EFI_PM_FG_CFGDATA_TYPE FgCfgData);

EFI_STATUS PmicFgCommon_GetBatteryStatus(UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_STATUS_INFO *BattStsInfo);
EFI_STATUS PmicFgCommon_GetBatteryId(EFI_PM_BATTERY_ID* BatteryId);
EFI_STATUS PmicFgCommon_DumpPeripheral(VOID);

EFI_STATUS PmicFgCommon_ConfigHwJeita(UINT32 PmicDeviceIndex);
EFI_STATUS PmicFgCommon_GetHWJeitaStatus(UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_TEMP_STATUS *pHwJeitaStatus);
VOID       PmicFgPostExit(VOID);
EFI_STATUS PmicFgCommon_ProfileLoadNeeded(UINT32 PmicDeviceIndex, EFI_PM_FG_BATT_PROFILE_STATE *eProfileState);
EFI_STATUS PmicFgCommon_SetThermBiasWait(UINT32 PmicDeviceIndex, EFI_PM_FG_THERM_CONFIG* pBattThermCfg, EFI_PM_FG_THERM_CONFIG* pAuxThermCfg);



#endif  /* __PMICFGCOMMON_H__ */


