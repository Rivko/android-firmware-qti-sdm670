/*! \file  BQ30Z55Lib.h 
 *  \n
 *  \brief  PMIC-BQ30Z55 MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC Battery Gauge BQ30Z55 module.
 *
 *
 *  \n &copy; Copyright (c) 2013    , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 03/05/13   sm      created file
===========================================================================*/

#ifndef __BQ30Z55Lib_H__
#define __BQ30Z55Lib_H__

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*
 * @brief This function initializes the battery gauge BQ30Z55 library.
 *        This function must be called before attempting to use any PMIC
 *        Battery Gauge APIs
 *
 * @param  I2C port address
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS BQ30Z55Init(
  void
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
EFI_STATUS
BQ30Z55GetTemp
(
  INT32 *pBattTemp
);

/**
 * @brief  This function gets battery capacity
 *
 * @param[out] pBatteryCapacity
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
EFI_STATUS
BQ30Z55GetSoC
(
  UINT32 *pBatteryCapacity
);

/**
 * @brief  This function gets the rated capacity
 *
 * @param[out] pRatedCapacity
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
EFI_STATUS
BQ30Z55GetRatedCapacity
(
  OUT UINT32 *pRatedCapacity
);

/**
 * @brief  This function gets the charge current
 *
 * @param[out]  pChargeCurrent
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
EFI_STATUS
BQ30Z55GetChargeCurrent
(
OUT INT32  *pChargeCurrent
);

EFI_STATUS
BQ30Z55GetBatteryVoltage
(
OUT INT32  *pBattVoltage
);

#endif /* __BQ30Z55Lib__ */
