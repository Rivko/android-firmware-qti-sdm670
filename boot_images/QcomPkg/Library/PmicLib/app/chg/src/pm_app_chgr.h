#ifndef PM_APP_CHGR__H
#define PM_APP_CHGR__H

/*! \file pm_app_chgr.h
 *  \n
 *  \brief  PMIC sbl charger algorithms
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC SBL charging
 *
 *
 *  \n &copy; Copyright (c) 2012-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/17   aab     Remove pm_app_chgr_set_hw_afp_thresholds(), moved functionality to common code
06/28/17   sv      Updated pm_app_auto_power_on_check API name.
02/17/17   pxm     Added function to check if auto power on.
01/11/17   pxm     add support for PBS triggered AFP
10/20/16   aab     Updated structures to support  Batt_therm Out of bound temperature check
09/17/16   aab     Added pm_sbl_schg_wipower_specific_data_type
08/25/16   pxm     Add pm_sbl_skin_temp_threshold_data_type 
08/12/16   aab     Added debugboard detection support
07/10/16   aab     Added pm_sbl_schg_info_type
05/27/16   pxm     Add wipower power configuration Structures
03/15/16   aab     Driver update (Support for PMI8998)
02/16/16   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_schg_batif.h"
#include "pm_schg_misc.h"
#include "pm_schg_usb.h"
#include "pm_resources_and_types.h"
#include "pm_chg.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/



/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/

typedef struct
{
   pm_schg_batif_low_battery_threshold  vlowbatt_threshold;
   pm_param_config_enable_type          enable_config;
} pm_sbl_schg_vlowbatt_threshold_data_type;


typedef struct
{
   boolean                        en_bat_missing_via_batid_therm_pin;
   pm_param_config_enable_type    enable_config;
} pm_sbl_schg_en_bat_miss_detect_type ;  


typedef struct
{
   pm_schg_misc_sbb_wd_cfg           wdog_timeout;   //SBB= Snarl, Bark, Bite timeout
   pm_param_config_enable_type       enable_config;
} pm_sbl_schg_wdog_timeout_data_type;



typedef struct
{
   uint32 fast_chg_i_ma;      //Valid values are 0 to 4500 mA
   pm_param_config_enable_type     enable_config;
} pm_sbl_schg_fast_chg_current_data_type;


typedef struct
{
   uint32 pre_chg_i_ma;       //Valid values are 0 to 1575 mA                  
   pm_param_config_enable_type  enable_config;
} pm_sbl_schg_pre_chg_current_data_type;



typedef struct
{
   uint32 float_volt_theshold_mv; //Valid range is 3600mV to 4500 mv
   pm_param_config_enable_type     enable_config;
} pm_sbl_schg_float_voltage_threshold_data_type;


typedef struct
{
   uint32  usbin_input_current_limit; //Valid value is 300 to 3000mAmp
   pm_param_config_enable_type     enable_config;
} pm_sbl_schg_usbin_input_current_limit_data_type;


typedef struct
{
   uint32  dcin_input_current_limit; //valid range is 300 to 2000 mAmp
   pm_param_config_enable_type    enable_config;
} pm_sbl_schg_dcin_input_current_limit_data_type;

//Debug Board detect: BATT_ID PD Resistor ADC Min/Max Read Value range
typedef struct
{
   uint32  min_vlue; 
   uint32  max_value; 
} pm_sbl_chg_debug_board_batt_id_range_type;


typedef struct
{
   uint32  min_value; //valid range is 0 ohm to 14000 ohm 
   uint32  max_value; //valid range is 0 ohm to 14000 ohm
   pm_param_config_enable_type    enable_config;
} pm_sbl_chg_fake_batt_id_range_type;


typedef struct
{
   int  temp_threshold; //valid range is -30 to 97 degree C
   pm_param_config_enable_type    enable_config;
} pm_sbl_temp_threshold_data_type;


typedef struct
{
   uint32  bias_wait; // 0 = 0 ms; 1 = 1 ms; 2 = 4 ms; 3 = 12 ms; 4 = 20 ms; 5 = 40 ms; 6 = 60 ms; 7 = 80 ms
   boolean  ground_sel;
} pm_sbl_fg_therm_config_type;



typedef struct
{
    pm_sbl_schg_vlowbatt_threshold_data_type         dbc_bootup_volt_threshold;
    pm_sbl_schg_vlowbatt_threshold_data_type         apsd_reset_vlowbatt_threshold;
    pm_sbl_temp_threshold_data_type                  fg_skin_hot_threshold; 
    pm_sbl_temp_threshold_data_type                  fg_skin_too_hot_threshold; 
    pm_sbl_temp_threshold_data_type                  fg_charger_hot_threshold; 
    pm_sbl_temp_threshold_data_type                  fg_charger_too_hot_threshold; 
    pm_sbl_schg_en_bat_miss_detect_type              bat_miss_detect_src;
    pm_sbl_schg_wdog_timeout_data_type               wdog_timeout;
    pm_sbl_schg_fast_chg_current_data_type           fast_chg_i;
    pm_sbl_schg_pre_chg_current_data_type            pre_chg_i;

    pm_sbl_schg_float_voltage_threshold_data_type    float_volt_theshold_mv;
    pm_sbl_schg_usbin_input_current_limit_data_type  usbin_input_current_limit;
    pm_sbl_schg_dcin_input_current_limit_data_type   dcin_input_current_limit;
    pm_sbl_chg_fake_batt_id_range_type               fake_batt_id_range;
    pm_sbl_fg_therm_config_type                      batt_therm_cfg;
    pm_sbl_fg_therm_config_type                      aux_therm_cfg;
    pm_sbl_chg_debug_board_batt_id_range_type        debug_board_batt_id_pd_res_range;
	
    uint32  bootup_battery_theshold_mv;
    uint32  wipwr_bootup_battery_theshold_mv;
    uint32  apsd_reset_threshold_mv;
    uint32  apsd_reset_theshold_no_uvlo_mv;
    uint32  apsd_reset_theshold_uvlo_mv;
    boolean enable_jeita_hard_limit_check;
    boolean dbc_usb_500_mode;
    boolean verbose_uart_logging;
} pm_sbl_schg_specific_data_type;



typedef struct
{
   uint32  wipower_chg_power_level; //valid range is 5000000 to 20000000 uWatt
   pm_param_config_enable_type    enable_config;
} pm_sbl_schg_wipower_power_cfg_type;


typedef struct
{
   uint32                         wait_time_ms;  // Default min: 3500ms; Max: forever; wait time applicable only if enable_config is FALSE
   pm_param_config_enable_type    enable_config; // Default is PM_ENABLE_CONFIG: Wipower charging wait will be forever 
} pm_sbl_schg_wipower_chg_wait_time_cfg_type;


typedef struct
{
   pm_sbl_schg_wipower_power_cfg_type          wipower_chg_power_level;
   pm_sbl_schg_wipower_chg_wait_time_cfg_type  wipower_chg_wait;
} pm_sbl_schg_wipower_specific_data_type;


typedef struct
{
   uint8                           value;
   pm_param_config_enable_type     enable_config;
} pm_sbl_chg_param_config_type;


typedef struct
{
   pm_sbl_chg_param_config_type  jetta_hard_cold_threshold;
   pm_sbl_chg_param_config_type  jetta_soft_cold_threshold;
   pm_sbl_chg_param_config_type  jetta_soft_hot_threshold;
   pm_sbl_chg_param_config_type  jetta_hard_hot_threshold;
} pm_sbl_schg_jetta_threshold_config_type;


typedef struct
{
   uint8   coeff_c1;
   uint8   coeff_c2;
   uint8   coeff_c3;
} pm_sbl_schg_batt_therm_coeff_type;

typedef struct
{
   uint8   aux_coeff_c1;
   uint8   aux_coeff_c2;
   uint8   aux_coeff_c3;
} pm_sbl_schg_batt_aux_therm_coeff_type;


typedef struct
{
   int16 critical_low_limit;
   int8 critical_high_limit;
} pm_sbl_schg_batt_temp_critical_limit_type;


typedef struct
{
   pm_param_config_enable_type                 enable_batt_therm_cfg;
   pm_sbl_schg_batt_temp_critical_limit_type   batt_temp_limit;
   pm_sbl_schg_batt_therm_coeff_type           batt_therm_cfg;
   pm_sbl_schg_batt_aux_therm_coeff_type       batt_aux_therm_cfg;
} pm_sbl_schg_batt_therm_type;



typedef struct {
  uint32 sbl_entry_mV;
  int32  sbl_entry_soc;
}pm_sbl_schg_info_type;


/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
/**
 * @name 
 *     pm_sbl_chg_config_vbat_low_threshold
 *
 * @description
 *    This function configures battery charge status 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_target_information_init()
 */
pm_err_flag_type pm_sbl_chg_config_vbat_low_threshold(uint32 device_index, pm_sbl_schg_vlowbatt_threshold_data_type vlowbatt_threshold_data);



/**
 * @name 
 *     pm_sbl_chg_check_weak_battery_status
 *
 * @description
 *    This function checks the battery charge status 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_chg_check_weak_battery_status(uint32 device_index);

/**
 * @name 
 *     pm_sbl_config_fg_sram
 *
 * @description
 *    This function configures fg related parameters to SRAM
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_fg_sram(uint32 device_index);

/**
 * @name 
 *     pm_sbl_config_chg_parameters
 *
 * @description
 *    This function configures charger parameters whose value is updated on configuration file
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_chg_parameters(uint32 device_index);


/**
 * @name 
 *     pm_sbl_config_thermister_coefficent
 *
 * @description
 *    This function configures charger parameters whose value is updated on configuration file
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_thermister_coefficent(uint32 device_index);




/**
 * @name 
 *     pm_sbl_config_jetta_threshold
 *
 * @description
 *    This function configures charger parameters whose value is updated on configuration file
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_jetta_threshold(uint32 device_index);



/**
 * @name 
 *     pm_sbl_schg_info_to_smem_update
 *
 * @description
 *    This function stores charger info into SMEM
 * 
 * @param 
 *     pm_sbl_schg_info_type struct pointer
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_schg_info_to_smem_update();


/**
 * @name 
 *     pm_sbl_get_debug_board_presence
 *
 * @description
 *    This function checkes if Debug Board is used 
 * 
 * @param 
 *     pm_sbl_schg_specific_data_type struct pointer
 *     boolean ptr to hold the state
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_get_debug_board_presence(uint32 device_index, pm_sbl_schg_specific_data_type *chg_param_ptr, boolean *debug_board_present);


/**
 * @name 
 *     pm_sbl_config_charger_and_skin_temp_threshold
 *
 * @description
 *    This function configures charger and skin temperature
 *    threshold parameters whose value is updated on
 *    configuration file 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_charger_and_skin_temp_threshold(uint32 device_index);


/**
 * @name 
 *     pm_sbl_config_therm_ground_sel
 *
 * @description
 *    This function configures batt_therm and aux_therm ground selection whose value is updated on configuration file
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_config_therm_ground_sel(uint32 device_index);


/**
 * @name 
 *     pm_sbl_fake_batt_id_range_config
 *
 * @description
 *    This function configures fake battery ID detection range whose value is updated on configuration file
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_sbl_chg_init()
 */
pm_err_flag_type pm_sbl_fake_batt_id_range_config(uint32 device_index, pm_sbl_schg_specific_data_type *chg_param_ptr);


/**
 * @name 
 *     pm_sbl_enable_batt_detection
 *
 * @description
 *    This function is used to enable BMD and disable battery ID conversion 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_sbl_enable_batt_detection(uint32 device_index);


/**
 * @name 
 *     pm_app_auto_power_on_check
 *
 * @description
 *    This function is used to check if auto power on.
 * 
 * @param 
 *    device_index:  PMIC chip index.
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_app_auto_power_on_check(uint32 device_index);

#endif  //PM_APP_CHGR__H

