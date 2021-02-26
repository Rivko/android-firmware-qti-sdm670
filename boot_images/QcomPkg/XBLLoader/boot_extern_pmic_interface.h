#ifndef BOOT_EXTERN_PMIC_INTERFACE_H
#define BOOT_EXTERN_PMIC_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN PMIC DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external pmic drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/17   ks      Added LCDB api's for Ramdump Display
08/01/17   czq     Export WLED/LAB/IBB/LDO APIs for display.
05/09/17   sj      Added Api to qurey the battery status in XBL ramdump
03/07/17   ds      Added boot_pm_aop_pre_init
01/26/17   yps     Added boot_pm_pbs_get_pon_reason_hist
04/18/16   digants Updated with changes for warm reset status check
02/18/16   aab     Added boot_pm_system_init()
10/05/15   kpa     Changed boot_pm_ps_hold_cfg() API to boot_pm_app_pon_pshold_cfg() to match pmic API name
09/29/15   kpa     Added support to check for warm reset status
07/11/15   rp	   Changed boot_pm_enable_led api to have LED color as an additional parameter
05/31/15   kpa     Replace pm_oem_init() with pm_sbl_chg_init()  
03/27/15   kpa     Added boot_pm_device_programmer_init
09/23/14   ck      Removed feature flag as API is always enabled in XBL
05/24/13   yp      Add boot_pm_vib_on and boot_pm_vib_off.
04/02/13   dh      Add boot_pm_pon_ps_hold_cfg.
03/29/13   plc     Add LED functions
02/26/13   dh      Add boot_pm_init_smem
09/26/12   dh      Add boot_pm_oem_init and boot_pm_driver_init
04/18/12   dh      change boot_pm_get_power_on_status to boot_pm_dev_get_power_on_reason
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "pm.h"
#include "pm_rgb.h"
#include "pm_ldo.h"
#include "pm_lab.h"
#include "pm_ibb.h"
#include "pm_wled.h"
#include "pm_lcdb.h"
#include "pm_fg_batt_info.h"
#include "boot_logger.h"
#include "boot_logger_timer.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_pm_device_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic device initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_device_init(void);


/*===========================================================================

**  Function :  boot_pm_get_power_on_status

** ==========================================================================
*/
/*!
 * @brief  This function returns the phone power-on reason. Typically used in boot
    during early bootup and stored in memory for later access.
 *
 * INPUT PARAMETERS
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *@param pwr_on_reason
 *    - pointer to 64-bit unsigned integer that stores the all PON reasons
 *   including PON power on, Warm Reset Reason and POFF_REASON,
 *   SOFT_RESET_REASON.
 *    PON_REASON
 *    PON_WARM_RESET_REASON
 *    PON_POFF_REASON
 *    PON_SOFT_RESET_REASON
 *
 * @return pm_err_flag_type.
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                       version of the PMIC.
 *         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
 *                                       with the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 *@par Dependencies
 *      and pm_init() must have been called.
===========================================================================*/
pm_err_flag_type boot_pm_dev_get_power_on_reason(unsigned pmic_device_index,
                                                 uint64 * pwr_on_reason);

/*===========================================================================   

**  Function :  boot_pm_pbs_get_pon_reason_history 
  
** ========================================================================== 
*/ 
/*! 
 * @brief  This function returns the phone pon reason history from PBS RAM 
 * 
 * INPUT PARAMETERS 
 * @param pmic_device_index When the target has more than 
 *          one pmic, this is the index of the PMIC in which 
 *          the power on module is physically located. The device 
 *          index starts at zero. 
 *@param pwr_on_reason 
 *    - pointer to 64-bit unsigned integer that stores the all PON reasons 
 *   including PON power on, Warm Reset Reason and POFF_REASON, 
 *   SOFT_RESET_REASON. 
 *    PON_REASON 
 *    PON_WARM_RESET_REASON 
 *    PON_POFF_REASON 
 *    PON_SOFT_RESET_REASON 
 * 
 * @return pm_err_flag_type. 
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range. 
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this  
 *                                       version of the PMIC. 
 *         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate 
 *                                       with the PMIC. 
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS. 
 * 
 *@par Dependencies 
 *      and pm_init() must have been called. 
===========================================================================*/ 
pm_err_flag_type boot_pm_pbs_get_pon_reason_history(pm_pbs_pon_reason_history_data_type* pon_reason_history); 


/*===========================================================================

**  Function :  boot_pm_driver_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic sw driver initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_driver_init(void);


/*===========================================================================

**  Function :  boot_pm_sbl_chg_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic sw driver initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_sbl_chg_init(void);


/*===========================================================================

**  Function :  boot_pm_system_init

** ==========================================================================
*/
/*! 
 * @brief This function executes pmic post initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_system_init(void);


/*===========================================================================

**  Function :  boot_pm_init_smem

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic smem initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_init_smem(void);


/*===========================================================================

**  Function :  boot_pm_enable_led

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic rgb led functions to turn on an LED
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_enable_led(uint32, boolean);


/*===========================================================================

**  Function :  boot_pm_app_pon_pshold_cfg

** ==========================================================================
*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_APP_PON_CFG_WARM_RESET,
 *          PM_APP_PON_CFG_HARD_RESET,
 *          PM_APP_PON_CFG_NORMAL_SHUTDOWN,
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type boot_pm_app_pon_pshold_cfg(pm_app_pon_reset_cfg_type);


/*===========================================================================

**  Function :  boot_pm_vib_on

** ==========================================================================
*/
/*! 
* @brief This function initializes vibration driver voltage and turn on it
* @return Error flag.
*
*/
pm_err_flag_type boot_pm_vib_on(void);


/*===========================================================================

**  Function :  boot_pm_vib_off

** ==========================================================================
*/
/*! 
 * @brief This function turn off vibration
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_vib_off(void);


/*===========================================================================

**  Function :  boot_pm_device_programmer_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic initialization for deviceprogrammer
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_device_programmer_init (void);

/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status

** ==========================================================================
*/
/*! 
 * @brief This function provides warm reset status information
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status( 
  unsigned pmic_device_index, 
  boolean *status
);


/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status_clear

** ==========================================================================
*/
/*! 
 * @brief This function clears the warm reset status
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status_clear(
  unsigned pmic_device_index
);

/*===========================================================================

**  Function :  boot_pm_pon_wdog_cfg

** ==========================================================================
*/
/*!
 *@brief  This function will configure the wdog with the given timer value 
 * and with the given reset configuration type.
 *
 *INPUT PARAMETERS
 *@param pmic_device_index
 *@param s1_timer
 *@param s2_timer
 *@param reset_cfg_type
 *
 *
 *@return pm_err_flag_type.
 *
 *api boot_pm_device_init() must have been called before calling this API
 *
 */
pm_err_flag_type boot_pm_pon_wdog_cfg(uint8 pmic_device_index,uint32 s1_timer,uint32 s2_timer,pm_pon_reset_cfg_type reset_cfg_type);
 
 /*===========================================================================

**  Function :  boot_pm_pon_wdog_enable

** ==========================================================================
*/
/*!
 *@brief  This function will enable the wdog with the given pmic_device_index.
 *
 *INPUT PARAMETERS
 *@param pmic_device_index
 *@param enable flag
 *
 *@return pm_err_flag_type.
 *
 *
 *@par Dependencies
 * api boot_pm_pon_wdog_cfg() must have been called before calling this API
 *
 */

pm_err_flag_type boot_pm_pon_wdog_enable(uint8 pmic_device_index,pm_on_off_type enable);
	

/*===========================================================================

**  Function :  boot_pm_pon_wdog_disable

** ==========================================================================
*/
/*!
 *@brief  This function will disable the wdog with the given pmic_device_index.
 *
 *INPUT PARAMETERS
 *@param pmic_device_index
 *@param disable flag
 *
 *@return pm_err_flag_type.
 *
 *
 *@par Dependencies
 *api boot_pm_pon_wdog_cfg() must have been called before calling this API
 *
 */

pm_err_flag_type boot_pm_pon_wdog_disable(uint8 pmic_device_index, pm_on_off_type disable);

/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status

** ==========================================================================
*/
/*! 
 * @brief This function provides warm reset status information
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status( 
  unsigned pmic_device_index, 
  boolean *status
);


/*===========================================================================

**  Function :  boot_pm_pon_warm_reset_status_clear

** ==========================================================================
*/
/*! 
 * @brief This function clears the warm reset status
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_pon_warm_reset_status_clear(
  unsigned pmic_device_index
);

/*===========================================================================

**  Function :  boot_pm_aop_pre_init

** ==========================================================================
*/
/*! 
 * @brief This function executes the pmic pre AOP initialization
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_aop_pre_init(void);

boolean boot_check_display_vbatt(void);

/*===========================================================================

**  Function :  boot_pm_ldo_sw_enable

** ==========================================================================
*/
/*! 
 * @brief Enables or disables a regulator or voltage switcher.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ldo_sw_enable(uint8 pmic_chip, uint8  ldo_peripheral_index, pm_on_off_type  on_off);

/*===========================================================================

**  Function :  boot_pm_ldo_volt_level

** ==========================================================================
*/
/*! 
 * @brief Sets the voltage level for an LDO.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ldo_volt_level(uint8 pmic_chip, uint8 ldo_peripheral_index, pm_volt_level_type volt_level);

/*===========================================================================

**  Function :  boot_pm_ibb_lcd_amoled_sel

** ==========================================================================
*/
/*! 
 * @brief This function configures module for AMOLED or LCD
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);

/*===========================================================================

**  Function :  boot_pm_ibb_set_soft_strt_chgr_resistor

** ==========================================================================
*/
/*! 
 * @brief TThis function sets the charging resistance for soft start
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms);

/*===========================================================================

**  Function :  boot_pm_ibb_ibb_module_rdy

** ==========================================================================
*/
/*! 
 * @brief This function prepares module to be enabled or not
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy);

/*===========================================================================

**  Function :  boot_pm_ibb_config_ibb_ctrl

** ==========================================================================
*/
/*! 
 * @brief This function enables/disables module and makes S-wire ready for listening
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy);

/*===========================================================================

**  Function :  boot_pm_ibb_get_ibb_status

** ==========================================================================
*/
/*! 
 * @brief This reads the IBB module setting status
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type* ibb_status);

/*===========================================================================

**  Function :  boot_pm_lab_config_current_sense

** ==========================================================================
*/
/*! 
 * @brief This configures LAB current sense 
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_config_current_sense(uint32 device_index, pm_lab_isense_type isense_tap, pm_lab_isense_type isense_gain);

/*===========================================================================

**  Function :  boot_pm_lab_config_pulse_skip_ctrl

** ==========================================================================
*/
/*! 
 * @brief This configures LAB pulse skipping parameters
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_config_pulse_skip_ctrl(uint32 device_index, uint32 ps_threshold, boolean select_ps_table, boolean select_digital_ps, boolean enable_ps);

/*===========================================================================

**  Function :  boot_pm_lab_lcd_amoled_sel

** ==========================================================================
*/
/*! 
 * @brief This function configures module for AMOLED or LCD
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode);

/*===========================================================================

**  Function :  boot_pm_lab_lab_module_rdy

** ==========================================================================
*/
/*! 
 * @briefThis function configures module to be enabled or not enabled
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy);

/*===========================================================================

**  Function :  boot_pm_lab_ibb_rdy_en

** ==========================================================================
*/
/*! 
 * @brief This function sets LAB to be enabled by IBB or not
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_rdy_en);

/*===========================================================================

**  Function :  boot_pm_lab_get_lab_status

** ==========================================================================
*/
/*! 
 * @brief This reads the LAB module setting status
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status);

/*===========================================================================

**  Function :  boot_pm_wled_sel_ovp_thr

** ==========================================================================
*/
/*! 
 * @brief This function selects OVP threshold
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_sel_ovp_thr(uint32 pmic_device_index, uint32 resource_index, pm_wled_ovp_threshold_type ovp_threshold_type);

/*===========================================================================

**  Function :  boot_pm_wled_sel_bst_ilimit

** ==========================================================================
*/
/*! 
 * @brief his function selects boost current limit
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_sel_bst_ilimit(uint32 pmic_device_index, uint32 resource_index, pm_wled_bst_ilimit_type bst_ilimit_type);

/*===========================================================================

**  Function :  boot_pm_wled_sel_slew_rate

** ==========================================================================
*/
/*! 
 * @brief This function is used to set slew rate
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_sel_slew_rate(uint32 pmic_device_index, pm_wled_slew_rate_type slew_rate_type);

/*===========================================================================

**  Function :  boot_pm_wled_set_fsw_ctrl

** ==========================================================================
*/
/*! 
 * @brief This function is used to set FSW.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_set_fsw_ctrl(uint32 pmic_device_index, uint32 resource_index, uint32 fsw_khz);

/*===========================================================================

**  Function :  boot_pm_wled_set_led_duty_cycle

** ==========================================================================
*/
/*! 
 * @brief This function is uesd to set duty cycle.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_set_led_duty_cycle(uint32 pmic_device_index, pm_wled_led_type led_type, uint16 data);

/*===========================================================================

**  Function :  boot_pm_wled_enable_current_sink

** ==========================================================================
*/
/*! 
 * @brief This function is uesd to enable WLED current source.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_enable_current_sink(uint32 pmic_device_index, pm_wled_led_type led_type);

/*===========================================================================

**  Function :  boot_pm_wled_enable_modulator

** ==========================================================================
*/
/*! 
 * @brief This function is uesd to enable WLED modulator
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_enable_modulator(uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable);

/*===========================================================================

**  Function :  boot_pm_wled_enable_module

** ==========================================================================
*/
/*! 
 * @brief This function is used to enable WLED module.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_enable_module(uint32 pmic_device_index, uint32 resource_index, boolean enable);

/*===========================================================================

**  Function :  boot_pm_wled_en_sync

** ==========================================================================
*/
/*! 
 * @brief This function is used to enable WLED sync.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_en_sync(uint32 pmic_device_index, pm_wled_led_type led_type, boolean enable);

/*===========================================================================

**  Function :  boot_pm_wled_en_amoled

** ==========================================================================
*/
/*! 
 * @brief This function enables/disables amoled.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_en_amoled(uint32 pmic_device, pm_wled_display_type display_type);

/*===========================================================================

**  Function :  boot_pm_wled_set_amoled_vout

** ==========================================================================
*/
/*! 
 * @@brief This function sets amoled vout.
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_wled_set_amoled_vout(uint32 pmic_device, pm_wled_amoled_ctrl_type  vout_ctrl, uint32 avdd_milli_volt);

/*===========================================================================

**  Function :  boot_pm_lcdb_module_enable

** ==========================================================================
*/
/*! 
 * @@brief  function enable/disables the lcdb module
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lcdb_module_enable(uint8 pmic_index,boolean module_en);


/*===========================================================================

**  Function :  boot_pm_lcdb_set_module_rdy

** ==========================================================================
*/
/*! 
 * @@brief  function configures module to be enabled or not enabled
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_lcdb_set_module_rdy(uint8 pmic_index, boolean module_rdy);



/*===========================================================================

**  Function :  boot_pm_fg_batt_info_get_vbatt

** ==========================================================================
*/
/*! 
 * @@brief  This function is uesd to get battery voltage
 * @return Error flag.
 *
 */ 
pm_err_flag_type boot_pm_fg_batt_info_get_vbatt(uint32 pmic_device, uint32 *pvbat);


#endif /* BOOT_EXTERN_PMIC_INTERFACE_H */
