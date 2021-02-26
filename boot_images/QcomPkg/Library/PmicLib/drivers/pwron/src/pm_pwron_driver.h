#ifndef PWRON_V6_H
#define PWRON_V6_H

/*! \file
*  \n
*  \brief  pm_pwron_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting PWRON pin services for the Qualcomm
*  PMIC chip set.
*  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/14   sm      Added API to get PBL_PON Status
02/20/14   al      Adding watchdog APIs  
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
03/19/12   hs      Removed the IRQ handle for WDOG status since it will be
                   saved in one of the registers.
12/15/11   tdh	   Updated for V6 Badger
12/13/11   wra     pwron module V6 Initial Creation
========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/

#include "pm_pwron_driver.h"
#include "pm_pwron_alg.h"
#include "pmic_device.h"

/*===================================================================
               DEFINITIONS
====================================================================*/
typedef pm_err_flag_type (*pm_ipwron_power_key_setup_fp)                        (pmiC_PmicResource *rsrc, pm_pwr_key_delay_type  delay, pm_switch_cmd_type     pull_up_en, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_set_pwr_key_delay_fp)                      (pmiC_PmicResource *rsrc, uint32  microseconds, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_get_power_on_status_fp)                      (pmiC_PmicResource *rsrc, uint32*  pwr_on_status, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_set_pwr_key_pullup_fp)                     (pmiC_PmicResource *rsrc, pm_switch_cmd_type  pull_up_en, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_clear_power_on_status_fp)                  (pmiC_PmicResource *rsrc, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_watchdog_reset_detect_switch_fp)           (pmiC_PmicResource *rsrc, pm_switch_cmd_type  OnOff, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_cbl_pwr_on_switch_fp)                      (pmiC_PmicResource *rsrc, pm_cbl_pwr_on_switch_type  cmd, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_cbl_pwr_on_pullup_switch_fp)               (pmiC_PmicResource *rsrc, pm_switch_cmd_type       OnOff, pm_cbl_pwr_on_pin_type   cable, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_start_up_abort_timer_switch_fp)            (pmiC_PmicResource *rsrc, pm_switch_cmd_type  OnOff, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_soft_reset_fp)                       (pmiC_PmicResource *rsrc, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_hard_reset_enable_fp)                (pmiC_PmicResource *rsrc, boolean  enable, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_hard_reset_delay_timer_set_fp)       (pmiC_PmicResource *rsrc, int  delay_timer_ms, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_hard_reset_debounce_timer_set_fp)    (pmiC_PmicResource *rsrc, int  debounce_timer_ms, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_get_wdog_status_fp)                        (pmiC_PmicResource *rsrc, boolean  *dog_status, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_stay_on_enable_fp)                   (pmiC_PmicResource *rsrc, boolean  enable, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_pwron_signal_reset_with_action_fp)         (pmiC_PmicResource *rsrc, pm_power_reset_signal  signal, pm_power_reset_action_type  action, uint8  perph_index);

typedef pm_err_flag_type (*pm_ipwron_get_pwron_trigger_fp)                      (pmiC_PmicResource *rsrc, uint8 *pon_trigger , uint8 perph_index);
typedef pm_err_flag_type (*pm_ipwron_get_pon_pbl_status_fp)                     (pmiC_PmicResource *rsrc, pm_pwron_pon_pbl_status_type pbl_status_type, boolean  *status, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_get_on_off_reset_reasons_fp)               (pmiC_PmicResource *rsrc, pm_pwron_reasons_type reason, uint8 *triggers, uint8  perph_index);
typedef pm_err_flag_type (*pm_ipwron_did_device_warm_reset_fp)                  (pmiC_PmicResource *rsrc, uint8  perph_index, boolean  *device_warm_reset);
typedef pm_err_flag_type (*pm_ipwron_config_pon_trigger_fp)                     (pmiC_PmicResource *rsrc, pm_pwron_trigger_type trigger, boolean enable, uint8  perph_index);

typedef pm_err_flag_type (*pm_ipwron_wdog_cfg_fp)                               (pmiC_PmicResource *rsrc, uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type);
typedef pm_err_flag_type (*pm_ipwron_wdog_enable_fp)                            (pmiC_PmicResource *rsrc, pm_on_off_type enable);
typedef pm_err_flag_type (*pm_ipwron_wdog_pet_fp)                               (pmiC_PmicResource *rsrc);

typedef struct pwron_common_ds
{
    const int*           pm_pon_reset_delay_table;
    unsigned char        pm_pon_reset_delay_table_size;
    const int*           pm_pon_debounce_timer_table;
    unsigned char        pm_pon_debounce_timer_table_size;
    const uint32*        pm_pon_delay_table;
    unsigned char        pm_pon_delay_table_size;
}pwron_common_ds;

typedef struct pwron_register_ds
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    
    pm_register_address_type pon_pbl_status				    ;//0x007
    pm_register_address_type pon_reason1                    ;//0x008
    pm_register_address_type warm_reset_reason1             ;//0x00A
    pm_register_address_type warm_reset_reason2             ;//0x00B
    pm_register_address_type poff_reason1                   ;//0x00C
    pm_register_address_type poff_reason2                   ;//0x00D
    pm_register_address_type soft_reset_reason1             ;//0x00E
    pm_register_address_type soft_reset_reason2             ;//0x00F
    pm_register_address_type int_rt_sts                     ;//0x010
    pm_register_address_type int_set_type                   ;//0x011
    pm_register_address_type int_polarity_high              ;//0x012
    pm_register_address_type int_polarity_low               ;//0x013
    pm_register_address_type int_latched_clr                ;//0x014
    pm_register_address_type int_en_set                     ;//0x015
    pm_register_address_type int_en_clr					    ;//0x016   
    pm_register_address_type int_latched_sts                    ;// 0x018
    pm_register_address_type int_pending_sts                    ;// 0x019
    pm_register_address_type int_mid_sel                        ;// 0x01A
    pm_register_address_type int_priority                       ;// 0x01B
    pm_register_address_type kpdpwr_n_reset_s1_timer            ;// 0x040
    pm_register_address_type kpdpwr_n_reset_s2_timer            ;// 0x041
    pm_register_address_type kpdpwr_n_reset_s2_ctl              ;// 0x042
    pm_register_address_type resin_n_reset_s1_timer             ;// 0x044
    pm_register_address_type resin_n_reset_s2_timer             ;// 0x045
    pm_register_address_type resin_n_reset_s2_ctl               ;// 0x046
    pm_register_address_type resin_and_kpdpwr_reset_s1_timeR    ;// 0x048
    pm_register_address_type resin_and_kpdpwr_reset_s2_timeR    ;// 0x049
    pm_register_address_type resin_and_kpdpwr_reset_s2_ctl      ;// 0x04A
    pm_register_address_type gp2_reset_s1_timer                 ;// 0x04C
    pm_register_address_type gp2_reset_s2_timer                 ;// 0x04D
    pm_register_address_type gp2_reset_s2_ctl                   ;// 0x04E
    pm_register_address_type gp1_reset_s1_timer                 ;// 0x050
    pm_register_address_type gp1_reset_s2_timer                 ;// 0x051
    pm_register_address_type gp1_reset_s2_ctl                   ;// 0x052
    pm_register_address_type pmic_wd_reset_s1_timer             ;// 0x054
    pm_register_address_type pmic_wd_reset_s2_timer             ;// 0x055
    pm_register_address_type pmic_wd_reset_s2_ctl               ;// 0x056
    pm_register_address_type pmic_wd_reset_s2_ctl2              ;// 0x057                                                                
    pm_register_address_type pmic_wd_reset_pet                  ;// 0x058
    pm_register_address_type ps_hold_reset_ctl                  ;// 0x05A
    pm_register_address_type ps_hold_reset_ctl2                 ;// 0x05B                                                             
    pm_register_address_type sw_reset_s2_ctl                    ;// 0x062
    pm_register_address_type sw_reset_go                        ;// 0x063
    pm_register_address_type overtemp_reset_ctl                 ;// 0x066
    pm_register_address_type tft_reset_ctl                      ;// 0x06A
    pm_register_address_type pullup_ctl                         ;// 0x070
    pm_register_address_type debounce_ctl                       ;// 0x071
    pm_register_address_type reset_s3_src                       ;// 0x074
    pm_register_address_type reset_s3_timer                     ;// 0x075
    pm_register_address_type pon_trigger_en                     ;// 0x080
    pm_register_address_type option_bits                        ;// 0x082
    pm_register_address_type uvlo                               ;// 0x088
    pm_register_address_type avdd_vph                           ;// 0x08A
    pm_register_address_type perph_rb_spare                     ;// 0x08C
    pm_register_address_type dvdd_rb_spare                      ;// 0x08D
    pm_register_address_type xvdd_rb_spare                      ;// 0x08E
    pm_register_address_type soft_rb_spare                      ;// 0x08F
    pm_register_address_type pon1_interface                     ;// 0x090
    pm_register_address_type pbs_interface                      ;// 0x091
    pm_register_address_type test1                              ;// 0x0E2
    pm_register_address_type test2                              ;// 0x0E3
    pm_register_address_type test4                              ;// 0x0E5
    pm_register_address_type test5                              ;// 0x0E6
    pm_register_address_type vmax_sel                           ;// 0x0E7
    pm_register_address_type trim_num                           ;// 0x0F0
}pwron_register_ds;             


typedef struct pwron_wdog_cfg_val_ds
{
   uint32 pon_reset_cfg_warm_reset_val;                      
   uint32 pon_reset_cfg_immediate_xvdd_shutdown_val;
   uint32 pon_reset_cfg_normal_shutdown_val;        
   uint32 pon_reset_cfg_d_vdd_shutdown_val;                  
   uint32 pon_reset_cfg_x_vdd_shutdown_val;                  
   uint32 pon_reset_cfg_hard_reset_val;                      
   uint32 pon_reset_cfg_dvdd_hard_reset_val;                  
   uint32 pon_reset_cfg_xvdd_hard_reset_val;                  
   uint32 pon_reset_cfg_warm_reset_and_dvdd_shutdown_val;     
   uint32 pon_reset_cfg_warm_reset_and_xvdd_shutdown_val;     
   uint32 pon_reset_cfg_warm_reset_and_shutdown_val;          
   uint32 pon_reset_cfg_warm_reset_then_hard_reset_val;       
   uint32 pon_reset_cfg_warm_reset_then_dvdd_hard_reset_val;  
   uint32 pon_reset_cfg_warm_reset_then_xvdd_hard_reset_val; 
   uint32 pon_wdog_max_s1_s2_timer_val;                      
}pwron_wdog_cfg_val_ds;

typedef struct pm_pwron_data_type
{
    pmiC_PmicResource super; // Inheritance from IGPIOstructure. This must be the first item in structure

    /********************************************
    DATA MEMBER DECLARATION
    *********************************************/
    pwron_register_ds* pwron_register;
    pwron_common_ds* pwron_common;
    boolean m_wdog_reset_status;
    uint8 num_of_peripherals;
    
    // Function Pointers for Virtual Methods
    pm_ipwron_power_key_setup_fp                        ipwron_power_key_setup;
    pm_ipwron_set_pwr_key_delay_fp                      ipwron_set_pwr_key_delay;
    pm_ipwron_get_power_on_status_fp                    ipwron_get_power_on_status;
    pm_ipwron_set_pwr_key_pullup_fp                     ipwron_set_pwr_key_pullup;
    pm_ipwron_clear_power_on_status_fp                  ipwron_clear_power_on_status;
    pm_ipwron_watchdog_reset_detect_switch_fp           ipwron_watchdog_reset_detect_switch;
    pm_ipwron_cbl_pwr_on_switch_fp                      ipwron_cbl_pwr_on_switch;
    pm_ipwron_cbl_pwr_on_pullup_switch_fp               ipwron_cbl_pwr_on_pullup_switch;
    pm_ipwron_start_up_abort_timer_switch_fp            ipwron_start_up_abort_timer_switch;
    pm_ipwron_pwron_soft_reset_fp                       ipwron_pwron_soft_reset;
    pm_ipwron_pwron_hard_reset_enable_fp                ipwron_pwron_hard_reset_enable;
    pm_ipwron_pwron_hard_reset_delay_timer_set_fp       ipwron_pwron_hard_reset_delay_timer_set;
    pm_ipwron_pwron_hard_reset_debounce_timer_set_fp    ipwron_pwron_hard_reset_debounce_timer_set;
    pm_ipwron_get_wdog_status_fp                        ipwron_get_wdog_status;
    pm_ipwron_get_pon_pbl_status_fp                     ipwron_get_pon_pbl_status;
    pm_ipwron_pwron_stay_on_enable_fp                   ipwron_pwron_stay_on_enable;
    pm_ipwron_pwron_signal_reset_with_action_fp         ipwron_pwron_signal_reset_with_action;    
    pm_ipwron_get_pwron_trigger_fp                      ipwron_get_pwron_trigger;             
    pm_ipwron_get_on_off_reset_reasons_fp               ipwron_get_on_off_reset_reasons; 
    pm_ipwron_did_device_warm_reset_fp                  ipwron_did_device_warm_reset;     
    pm_ipwron_config_pon_trigger_fp                     ipwron_config_pon_trigger;  
    pm_ipwron_wdog_cfg_fp                               ipwron_wdog_cfg;
    pm_ipwron_wdog_enable_fp                            ipwron_wdog_enable;
    pm_ipwron_wdog_pet_fp                               ipwron_wdog_pet ;
	
}pm_pwron_data_type;
/*************************************
NOTE: Initializer IMPLEMENTATION
**************************************/
pmiC_PmicResource* pm_pwron_driver_init(pmiC_PmicDevice*  device);

#endif //PM_PWRON_DRIVER__H
