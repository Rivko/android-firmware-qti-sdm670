/*! \file  pm_uefi_BQ27541.h 
 *  \n
 *  \brief  PMIC-BQ27541 MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC Battery Gauge BQ27541 module.
 *
 *
 *  \n &copy; Copyright (c) 2009-2011    by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 01/31/12   sm      Added API to determine battery voltage and temp
 08/19/11   sm      created file
===========================================================================*/

#ifndef __BQ27541Lib_H__
#define __BQ27541Lib_H__

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*
 * @brief This function initializes the battery gauge BQ27541 library.
 *        This function must be called before attempting to use any PMIC
 *        Battery Gauge APIs
 *
 * @param  I2C port address
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS BQ27541Init(
  void
);

/**
 * @brief  This function calculates and returns the battery SoC, Rated capacity
 *         and ChargeCurrent
 *
 * @param  I2C port address
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
EFI_STATUS BQ27541GetBatteryStatus(
  OUT UINT32 *StateOfCharge,
  OUT UINT32 *RatedCapacity,
  OUT INT32  *ChargeCurrent
);

/**
 * @brief  This function calculates and returns the battery temperature
 *
 * @param  None
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
UINT16  BQ27541GetTemp(void);

/**
 * @brief  This function calculates and returns the battery voltage
 *
 * @param  None
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
UINT16 BQ27541GetVBatt(
  void
);

#endif /* __PM_UEFI_BQ27541_H__ */

