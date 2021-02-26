#ifndef __PM_CLK_SLEEP_H__
#define __PM_CLK_SLEEP_H__

/*! \file pm_clk_sleep.h
*  \n
*  \brief This header file contains functions and variable declarations 
*         to support PMIC Sleep Clock peripheral.
*  
*  \n &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/13   hw      Rearchitecting clk module driver to peripheral driver
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_clk.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum 
{
   PM_DELAY_0P5_SEC, 
   PM_DELAY_1P0_SEC, 
   PM_DELAY_1P5_SEC, 
   PM_DELAY_2P0_SEC, 
   PM_DELAY_INVALID,
}pm_clk_sleep_smpl_delay_type;

/*===========================================================================

                 API PROTOTYPES

===========================================================================*/

/**
* 
* @brief Turn on or off ( enable or disable ) SMPL ( Sudden Momentum Power Loss )
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param clk_type Selects which clk to use
* @param strenght Select the strength desired.
* 
* @return pm_err_flag_type PM_ERR_FLAG_SUCCESS if successful
*/
pm_err_flag_type pm_clk_sleep_smpl(uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type on_off, pm_clk_sleep_smpl_delay_type delay);

/**
* 
* @brief Turn On/Off (Enable/Disable) the Calibrated RC (CalRC)
*        clock
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param clk_type Selects which clk to use
* @param enable Turn the CalRC clock On or Off (enable or 
*               disable)
* @note
*    Usage Example:
*    pm_clk_sleep_cal_rc_enable(0, PM_CLK_SLEEP, PM_ON);
* 
* @return pm_err_flag_type PM_ERR_FLAG_SUCCESS if successful
*/
pm_err_flag_type pm_clk_sleep_cal_rc_enable(uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type enable);

/**
* 
* @brief Enable/Disable the COINCELL_GOOD bit in CalRC4 reg to 
*        indicate whether a qualified coin cell is installed or
*        not (to be used by the Calibrated RC (CalRC) clock when
*        needed)
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param clk_type Selects which clk to use
* @param enable Enable/Disable the COINCELL_GOOD bit in CalRC4
*               reg
* @note
*    Usage Example:
*    pm_clk_sleep_coincell_valid(0, PM_CLK_SLEEP, PM_ON);
* 
* @return pm_err_flag_type PM_ERR_FLAG_SUCCESS if successful
*/
pm_err_flag_type pm_clk_sleep_coincell_valid(uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type enable);

#endif /* __PM_CLK_SLEEP_H__ */

