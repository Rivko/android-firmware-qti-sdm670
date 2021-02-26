#ifndef __PM_VS__H__
#define __PM_VS__H__
/*! \file pm_vs.h
 *  \n
 *  \brief This header file contains enums and API definitions for PMIC voltage
 *         regulator driver.
 *  \n
 *  \n &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/15   al      Updated rails API
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/*!
 *  \brief 
 *  VSS peripheral index. This enum type contains all ldo regulators that you may need. 
 */
enum
{
  PM_VS_LVS_1,
  PM_VS_LVS_2,
  PM_VS_LVS_3,
  PM_VS_LVS_4,
  PM_VS_LVS_5,
  PM_VS_LVS_6,
  PM_VS_LVS_7,
  PM_VS_LVS_8,
  PM_VS_LVS_9,
  PM_VS_LVS_10,
  PM_VS_LVS_11,
  PM_VS_LVS_12,
  PM_VS_MVS_1,
  PM_VS_OTG_1,
  PM_VS_HDMI_1,
  PM_VS_INVALID
};

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/**
 * @name 
 *     pm_vs_pull_down
 *
 * @description
 *     Allows you to enable disable active pulldown.
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_vs_pull_down
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type on_off);

/**
 * @name 
 *     pm_vs_pull_down_status
 *
 * @description
 *     Read pull down status
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_vs_pull_down_status
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off);

/**
 * @name 
 *     pm_vs_sw_mode
 *
 * @description
 *     Switch between HPM, LPM, and other modes of a regulator.
 * 
 * @param 
 *     sw_mode - Select the different mode of a regulator. Example, HPM, LPM
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_vs_sw_mode
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_sw_mode_type sw_mode);

/** 
 * @name pm_vs_sw_mode_status 
 *  
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  vs_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] sw_mode:  
 *                Variable to return to the caller with mode status.
 *                Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_vs_sw_mode_status
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_sw_mode_type* sw_mode);

/**
 * @name 
 *     pm_vs_pin_ctrled
 *
 * @description
 *     select the pin ( connected to external signal ) that you would like to use
 *     to effect the state ( on/off ) and mode ( HPM, LPM etc ) of a regulator
 * 
 * @param 
 *     select_pin - Select a pin
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_vs_pin_ctrled
(uint8 pmic_chip, uint8 vs_peripheral_index, uint8 select_pin);

/** 
 * @name pm_vs_pin_ctrled_status 
 *  
 * @brief This function returns the pin ctrled status or h/w 
 *        enable status (On/Off) of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  vs_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] on_off:  
 *                Variable to return to the caller with pin ctrl
 *                status. Refer to pm_resources_and_types.h for
 *                the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_vs_pin_ctrled_status
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off);


/**
 * @name 
 *     pm_vs_sw_enable
 *
 * @description
 *     enable or disable a regulator or voltage switcher.
 * 
 * @param 
 *     on_off - turn on and off the regulator.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_vs_sw_enable
(uint8 pmic_chip, uint8  vs_peripheral_index, pm_on_off_type  on_off);

/** 
 * @name pm_vs_sw_enable_status 
 *  
 * @brief This function returns the s/w enable status (On/Off) 
 *        of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  vs_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] on_off:  
 *                Variable to return to the caller with s/w
 *                status. Refer to pm_resources_and_types.h for
 *                the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_vs_sw_enable_status
(uint8 pmic_chip, uint8 vs_peripheral_index, pm_on_off_type* on_off);

/** 
 * @name pm_vs_vreg_ok_status 
 *  
 * @brief This function returns the vreg_ok status.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] status:  
 *                Variable to return to the caller with vreg_ok status 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_vs_vreg_ready_status
(uint8 pmic_chip, uint8 vs_peripheral_index, boolean* status);


#endif /* __PM_VS__H__ */

