#ifndef __PM_APP_PON__H__
#define __PM_APP_PON__H__

/*! \file pm_app_pon.h
 *  \n
 *  \brief  PMIC PON APP API support functions
 *  \details  This file contains functions and variable declarations to support PMIC PON APP Level APIs
 *
 *
 *  \n &copy; Copyright (c) 2016-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/16/16   aab     Added pm_pon_s3_reset_timer_cfg_type and updated pm_sbl_pon_specific_data_type
05/04/16   aab     Added pm_app_pon_get_mapped_reset_config()
04/29/16   aab     Added pm_app_pon_reset_init() 
03/28/16   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_pon.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/
enum
{
  PON_WARM_RST                      = PM_PON_RESET_CFG_WARM_RESET,
  PON_SHDN                          = PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  PON_DVDD_SHDN                     = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_XVDD_SHDN                     = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_HARD_RST                      = PM_PON_RESET_CFG_HARD_RESET,
  PON_DVDD_HARD_RST                 = PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_XVDD_HARD_RST                 = PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_IMMEDIATE_XVDD_SHDN           = PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_DVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_XVDD_SHDN       = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PON_WARM_RST_THEN_SHDN            = PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  PON_WARM_RST_THEN_HARD_RST        = PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PON_WARM_RST_THEN_DVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PON_WARM_RST_THEN_XVDD_HARD_RST   = PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PON_NO_CFG_NEEDED                 = PM_PON_RESET_CFG_INVALID  
};


/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
typedef struct
{
   uint32  timer_value;
   pm_app_pon_reset_source_type reset_source_type;
   pm_param_config_enable_type  enable_config;
} pm_pon_s3_reset_timer_cfg_type;


typedef struct
{
   pm_pon_s3_reset_timer_cfg_type  s3_reset_timer_cfg;
}pm_sbl_pon_specific_data_type;

typedef enum
{
  PM_PON_PWRKEY_DBNC_CHK_AT_NONE    = 0x0, 
  /*Don't check invalid pwr key press during boot*/
  PM_PON_PWRKEY_DBNC_CHK_AT_LOADER  = 0x1, 
  /*Check invalid pwr key press in Loader during boot*/
  PM_PON_PWRKEY_DBNC_CHK_AT_CORE    = 0x2, 
  /*Check invalid pwr key press in Core during boot*/
  PM_PON_PWRKEY_DBNC_CHK_AT_INVALID = 0x3, 
  /*Invalid*/
}pm_pon_pwrkey_dbnc_chk_at_type;

typedef struct 
{
  pm_pon_pwrkey_dbnc_chk_at_type chk_at;  
  /*check invalid boot pwr key press at loader or core or none*/
  uint32 dbnc_time_msec;                  
  /*pwr keypress debounce time in milli_sec*/
}pm_pwrkey_dbnc_chk_type;
  

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
/**
 * @brief PON Reset initial configuration
 *  
 * @details PON Reset iitial configuration 
 *  
 * @param none
 *  
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG_SUCCESS = SUCCESS else ERROR.
 */
pm_err_flag_type 
pm_app_pon_reset_init(void);


/**
 * @brief Configures PMIC GP1/GP2 Reset. .
 *  
 * @details Configure GP1/GP2 PON source and reset type for all the
 *        applicable PMICs internally
 *  
 * @param app_pon_reset_source  :   pm_app_pon_reset_source_type 
 *  
 *@param app_pon_reset_cfg : Supported configurations: 
 *                               PM_APP_PON_CFG_DVDD_HARD_RESET 
 *                               PM_APP_PON_CFG_WARM_RESET,
 *   
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG_SUCCESS = SUCCESS else 
 *         ERROR.
 */
pm_err_flag_type
pm_pon_gp_reset_cfg( uint32  pmic_device_index, pm_pon_reset_source_type reset_source, pm_pon_reset_cfg_type reset_cfg_type) ;


/*===========================================================================

FUNCTION pm_app_long_pwr_key_hold_check                                

DESCRIPTION
    This function detects if power key press to bootup is valid or not

===========================================================================*/
/**
* @brief This function detects if power key press to bootup is valid or not
*
* @param[in]  dbnc_chk_at: Refer pm_pon_pwrkey_dbnc_chk_at_type for more info
*
* @return pm_err_flag_type
*/
pm_err_flag_type pm_app_pwrkey_long_press_check(pm_pon_pwrkey_dbnc_chk_at_type  dbnc_chk_at);
#endif  //__PM_APP_PON__H__
