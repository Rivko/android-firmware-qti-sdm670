/*! \file  SMBCLib.h 
 *  \n
 *  \brief  PMIC-SMBC MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC SMBC Charger module.
 *
 *
 *  \n &copy; Copyright (c) 2009-2012    by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 7/27/12    al		Renamed all Enums,added device index and resource index
 01/18/12   sm      Added SetMaxCurrent API
 08/19/11   sm      New file
===========================================================================*/

#ifndef __SMBCLib_H__
#define __SMBCLib_H__

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * @brief This function initializes SMBC Charger driver
 *
 * @param  
 * 
 * @retval pm_err_flag_type 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
**/
EFI_STATUS EFI_PmicSMBCInit(
  UINT32 Pmic_Device_Index 
);

/**
 * @brief This function Enables charging SMBC driver
 *
 * @param  None 
 *  
 * @retval pm_err_flag_type PM_ERR_FLAG__FEATURE_NOT_SUPPORTED =
 *         Feature not available on this version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
**/
EFI_STATUS EFI_PmicSMBCEnableCharger(
  UINT32 Pmic_Device_Index, 
  UINT32 ResourceIndex ,
  IN BOOLEAN ChargerEnable
);

/**
 * @brief This function sets the maximum charge current
 *
 * @param  MaxCurrent 
 *  
 * @retval pm_err_flag_type PM_ERR_FLAG__FEATURE_NOT_SUPPORTED =
 *         Feature not available on this version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
**/
EFI_STATUS EFI_PmicSMBCSetMaxCurrent(
  UINT32 Pmic_Device_Index, 
  UINT32 ResourceIndex ,
  IN UINT32 MaxCurrent
);

/**
 * @brief This function pings SMBC hardware
 *
 * @param  None 
 *  
 * @retval pm_err_flag_type PM_ERR_FLAG__FEATURE_NOT_SUPPORTED =
 *         Feature not available on this version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
**/
EFI_STATUS EFI_PmicSMBCPingChargerHW(
    UINT32 Pmic_Device_Index, 
  UINT32 ResourceIndex 
);

#endif /*__SMBCLib_H__*/

