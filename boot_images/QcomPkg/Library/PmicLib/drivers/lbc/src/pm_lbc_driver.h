#ifndef PM_LBC_DRIVER__H
#define PM_LBC_DRIVER__H

/*! \file
*  \n
*  \brief  pm_lbc_driver.h
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting LBC pin services for the Qualcomm
*  PMIC chip set.
*  \n &copy;  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when		 who      what, where, why
--------	 ---      ----------------------------------------------------------
05/02/14     sv       pm_lbc_irq_status updated to read correct peripheral irq status.(CR-656724)
02/13/14     ps       File created
========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_uefi_lbc.h"
#include "pm_lbc_alg.h"

/*===================================================================
               DEFINITIONS
====================================================================*/
typedef pm_err_flag_type(*pm_ilbc_set_mode_fp)(pmiC_PmicResource *rsrc,  pm_lbc_mode_type mode);
typedef pm_err_flag_type(*pm_ilbc_get_battery_present_fp)(pmiC_PmicResource *rsrc,  boolean *present);
typedef pm_err_flag_type(*pm_ilbc_enable_wdog_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_wdog_fp)(pmiC_PmicResource *rsrc,  uint32 delay_seconds, uint32 timeout_seconds);
typedef pm_err_flag_type(*pm_ilbc_pet_chg_wdog_fp)(pmiC_PmicResource *rsrc);
typedef pm_err_flag_type(*pm_ilbc_enable_ttrkl_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_ttrkl_fp)(pmiC_PmicResource *rsrc,  uint8 value_minutes);
typedef pm_err_flag_type(*pm_ilbc_set_vtrkl_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_set_itrickle)(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
typedef pm_err_flag_type(*pm_ilbc_set_itrickle_switch_mode_fp)(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
typedef pm_err_flag_type(*pm_ilbc_set_vdd_safe_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_set_vdd_max_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_set_vin_min_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_set_temp_threshold_fp)(pmiC_PmicResource *rsrc,  uint32 stop_deg_c, uint32 resume_deg_c);
typedef pm_err_flag_type(*pm_ilbc_set_ibat_max_fp)(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
typedef pm_err_flag_type(*pm_ilbc_set_ibat_safe_fp)(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
typedef pm_err_flag_type(*pm_ilbc_set_vbat_det_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_set_vbat_weak_thr_fp)(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_get_vbat_weak_thr_fp)(pmiC_PmicResource *rsrc,  uint32 *value_mv);
typedef pm_err_flag_type(*pm_ilbc_get_battery_weak_fp)(pmiC_PmicResource *rsrc,  boolean *weak);
typedef pm_err_flag_type(*pm_ilbc_get_vbat_status_fp)(pmiC_PmicResource *rsrc,  pm_lbc_vbat_status_type vbat_status, boolean *status);
typedef pm_err_flag_type(*pm_ilbc_get_attached_charger_fp)(pmiC_PmicResource *rsrc,  pm_lbc_attached_chgr_type *type);
typedef pm_err_flag_type(*pm_ilbc_set_tchg_fp)(pmiC_PmicResource *rsrc,  uint32 value_minutes);
typedef pm_err_flag_type(*pm_ilbc_enable_tchg_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_iterm_chgr_fp)(pmiC_PmicResource *rsrc,  pm_lbc_ieoc_method_type method, boolean enable, uint32 in_milliampere);
typedef pm_err_flag_type (*pm_ilbc_get_usb_ovp_ctl_fp)(pmiC_PmicResource *rsrc,  pm_lbc_ovp_thr_type *ovp_thr, pm_lbc_uvd_thr_type *uvd_thr, pm_lbc_valid_db_type *valid_db);
typedef pm_err_flag_type(*pm_ilbc_set_usb_ovp_ctl_fp)(pmiC_PmicResource *rsrc,  pm_lbc_ovp_thr_type ovp_thr, pm_lbc_uvd_thr_type uvd_thr, pm_lbc_valid_db_type valid_db);
typedef pm_err_flag_type(*pm_ilbc_enable_low_power_mode_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_enable_boot_timer_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_adaptive_boot_type_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_boot_timer_fp)(pmiC_PmicResource *rsrc,  pm_lbc_boot_timer_type boot_timer);
typedef pm_err_flag_type(*pm_ilbc_get_vbat_boot_thres_fp)(pmiC_PmicResource *rsrc,  uint32 *in_millivolt);
typedef pm_err_flag_type(*pm_ilbc_get_batt_therm_presence_fp)(pmiC_PmicResource *rsrc,  boolean *present);
typedef pm_err_flag_type(*pm_ilbc_get_batt_id_presence_fp)(pmiC_PmicResource *rsrc,  boolean *present);
typedef pm_err_flag_type(*pm_ilbc_get_batt_temp_status_fp)(pmiC_PmicResource *rsrc,  pm_lbc_bat_temp_status_type *temp_status);
typedef pm_err_flag_type(*pm_ilbc_is_vref_bat_thm_on_fp)(pmiC_PmicResource *rsrc,  boolean *status);
typedef pm_err_flag_type(*pm_ilbc_is_vref_bat_thm_source_avdd_fp)(pmiC_PmicResource *rsrc,  boolean *status);
typedef pm_err_flag_type(*pm_ilbc_is_vref_bat_thm_source_vreg_xoadc_fp)(pmiC_PmicResource *rsrc,  boolean *status);
typedef pm_err_flag_type(*pm_ilbc_enable_bpd_bat_thm_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_enable_bpd_bat_id_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_set_bat_temp_comp_fp)(pmiC_PmicResource *rsrc,  boolean enable_btc, pm_lbc_batt_temp_cold_thr_type cold_thd, pm_lbc_batt_temp_hot_thr_type hot_thd);
typedef pm_err_flag_type(*pm_ilbc_set_vref_bat_thm_ctrl_fp)(pmiC_PmicResource *rsrc,  pm_lbc_vref_bat_thm_enable_type enable_type);
typedef pm_err_flag_type(*pm_ilbc_get_vref_bat_thm_ctrl_fp)(pmiC_PmicResource *rsrc,  pm_lbc_vref_bat_thm_enable_type *enable_type);
typedef pm_err_flag_type(*pm_ilbc_enable_vbat_dischg_fp)(pmiC_PmicResource *rsrc,  boolean enable);
typedef pm_err_flag_type(*pm_ilbc_is_battery_removed_fp)(pmiC_PmicResource *rsrc,  boolean *battery_removed);
typedef pm_err_flag_type(*pm_ilbc_is_batt_trckl_charged_fp)(pmiC_PmicResource *rsrc,  boolean *trickle_charged);
typedef pm_err_flag_type(*pm_ilbc_irq_enable_fp)(pmiC_PmicResource *rsrc, pm_lbc_irq_type irq, boolean enable);
typedef pm_err_flag_type(*pm_ilbc_irq_clear_fp) (pmiC_PmicResource *rsrc, pm_lbc_irq_type irq);
typedef pm_err_flag_type(*pm_ilbc_irq_set_trigger_fp)(pmiC_PmicResource *rsrc,pm_lbc_irq_type irq, pm_irq_trigger_type trigger);
typedef pm_err_flag_type(*pm_ilbc_irq_status_fp)(pmiC_PmicResource *rsrc, unsigned pheripheral_index, pm_lbc_irq_type irq, pm_irq_status_type type, boolean *status);

/************************************************************************/
/* register definitions                                                                     */
/************************************************************************/
typedef struct lbc_bat_if_register_ds
{
    pm_register_address_type    base_address;        //0x1200
    pm_register_address_type    peripheral_offset;   //0x100

    pm_register_address_type bat_pres_status; // 0x008
    pm_register_address_type bat_temp_status; // 0x009
    pm_register_address_type vref_bat_thm_status; // 0x00a
    pm_register_address_type int_rt_sts; // 0x010
    pm_register_address_type int_set_type; // 0x011
    pm_register_address_type int_polarity_high; // 0x012
    pm_register_address_type int_polarity_low; // 0x013
    pm_register_address_type int_latched_clr; // 0x014
    pm_register_address_type int_en_set; // 0x015
    pm_register_address_type int_en_clr; // 0x016
    pm_register_address_type int_latched_sts; // 0x018
    pm_register_address_type int_pending_sts; // 0x019
    pm_register_address_type int_mid_sel; // 0x01a
    pm_register_address_type int_priority; // 0x01b
    pm_register_address_type bpd_ctrl; // 0x048
    pm_register_address_type btc_ctrl; // 0x049
    pm_register_address_type vref_bat_thm_ctrl; // 0x04a
    pm_register_address_type vbat_dischg; // 0x04e
    pm_register_address_type bat_removed_offmode; // 0x04f
    pm_register_address_type sec_access; // 0x0d0
    pm_register_address_type perph_reset_ctl2; // 0x0d9
    pm_register_address_type perph_reset_ctl3; // 0x0da
    pm_register_address_type perph_reset_ctl4; // 0x0db
    pm_register_address_type perph_reset_ctl5; // 0x0dc
    pm_register_address_type perph_reset_ctl6; // 0x0dd
    pm_register_address_type bat_if_spare; // 0x0df
    pm_register_address_type int_test1; // 0x0e0
    pm_register_address_type int_test_val; // 0x0e1
    pm_register_address_type bat_if_test1; // 0x0e2
    pm_register_address_type bat_if_test3; // 0x0e4
    pm_register_address_type comp_ovr0; // 0x0e5
    pm_register_address_type comp_ovr1; // 0x0e6
    pm_register_address_type bat_if_comp_ovr_global; // 0x0e7
}lbc_bat_if_register_ds;


typedef struct lbc_chgr_register_ds
{
    pm_register_address_type    base_address;        //0x1000
    pm_register_address_type    peripheral_offset;   //0x100

    pm_register_address_type perph_subtype;        //0x005
    pm_register_address_type chg_option;           //0x008
    pm_register_address_type atc_status;           //0x00a
    pm_register_address_type vbat_status;          //0x00b
    pm_register_address_type int_rt_sts;           //0x010
    pm_register_address_type int_set_type;         //0x011
    pm_register_address_type int_polarity_high;    //0x012
    pm_register_address_type int_polarity_low;     //0x013
    pm_register_address_type int_latched_clr;      //0x014
    pm_register_address_type int_en_set;           //0x015
    pm_register_address_type int_en_clr;           //0x016
    pm_register_address_type int_latched_sts;      //0x018
    pm_register_address_type int_pending_sts;      //0x019
    pm_register_address_type int_mid_sel;          //0x01a
    pm_register_address_type int_priority;         //0x01b
    pm_register_address_type vdd_max;              //0x040
    pm_register_address_type vdd_safe;             //0x041
    pm_register_address_type ibat_max;             //0x044
    pm_register_address_type ibat_safe;            //0x045
    pm_register_address_type vin_min;              //0x047
    pm_register_address_type chg_ctrl;             //0x049
    pm_register_address_type chg_failed;           //0x04a
    pm_register_address_type atc_failed;           //0x04c
    pm_register_address_type vbat_trkl;            //0x050
    pm_register_address_type vbat_weak;            //0x052
    pm_register_address_type ibat_atc_b;           //0x055
    pm_register_address_type ibat_term_chgr;       //0x05b
    pm_register_address_type ttrkl_max_en;         //0x05e
    pm_register_address_type ttrkl_max;            //0x05f
    pm_register_address_type tchg_max_en;          //0x060
    pm_register_address_type tchg_max;            //0x061
    pm_register_address_type chg_wdog_time;        //0x062
    pm_register_address_type chg_wdog_dly;         //0x063
    pm_register_address_type chg_wdog_pet;         //0x064
    pm_register_address_type chg_wdog_en;          //0x065
    pm_register_address_type chg_temp_thresh;      //0x066
    pm_register_address_type sec_access;           //0x0d0
    pm_register_address_type perph_reset_ctl2;     //0x0d9
    pm_register_address_type perph_reset_ctl3;     //0x0da
    pm_register_address_type perph_reset_ctl4;     //0x0db
    pm_register_address_type perph_reset_ctl5;     //0x0dc
    pm_register_address_type perph_reset_ctl6;     //0x0dd
    pm_register_address_type chgr_spare;           //0x0df
    pm_register_address_type int_test1;            //0x0e0
    pm_register_address_type int_test_val;         //0x0e1
    pm_register_address_type trkl_chg_test;        //0x0e2
    pm_register_address_type trickle_clamp;        //0x0e3
    pm_register_address_type fsm_state_capture;    //0x0e6
    pm_register_address_type fsm_state_sts;        //0x0e7
    pm_register_address_type fsm_state_test;       //0x0e8
    pm_register_address_type fsm_state_write;      //0x0e9
    pm_register_address_type dtest_ctrl1;          //0x0ea
    pm_register_address_type timer_speedup;        //0x0ec
    pm_register_address_type comp_ovr0;            //0x0ed
    pm_register_address_type comp_ovr1;            //0x0ee
    pm_register_address_type chgr_comp_ovr_global; //0x0ef
}lbc_chgr_register_ds;

typedef struct lbc_misc_register_ds
{
    pm_register_address_type    base_address;        //0x1600
    pm_register_address_type    peripheral_offset;   //0x100
                                                     
    pm_register_address_type revision2;              // 0x001
    pm_register_address_type low_power_mode;         // 0x040
    pm_register_address_type boot;                   // 0x041
    pm_register_address_type boot_done;              // 0x042
    pm_register_address_type vbat_boot_thres;        // 0x043
    pm_register_address_type raw_xvdd_rb_scratch;    // 0x0cd
    pm_register_address_type raw_dvdd_rb_scratch;    // 0x0ce
    pm_register_address_type sec_access;             // 0x0d0
    pm_register_address_type lbc_misc_spare;        // 0x0d7
    pm_register_address_type perph_reset_ctl2;       // 0x0d9
    pm_register_address_type perph_reset_ctl3;       // 0x0da
    pm_register_address_type perph_reset_ctl4;       // 0x0db
    pm_register_address_type perph_reset_ctl5;       // 0x0dc
    pm_register_address_type perph_reset_ctl6;       // 0x0dd
    pm_register_address_type perph_reset_ctl7;       // 0x0de
    pm_register_address_type perph_reset_ctl8;       // 0x0df
    pm_register_address_type clock_test_sleep;       // 0x0e2
}lbc_misc_register_ds;


typedef struct lbc_usb_chgpth_register_ds
{
    pm_register_address_type    base_address;        //0x1300
    pm_register_address_type    peripheral_offset;   //0x100

    pm_register_address_type pwr_pth_sts;           // 0x008
    pm_register_address_type usb_chg_pth_sts;       // 0x009
    pm_register_address_type chg_gone_int;          // 0x00a
    pm_register_address_type int_rt_sts;            // 0x010
    pm_register_address_type int_set_type;          // 0x011
    pm_register_address_type int_polarity_high;     // 0x012
    pm_register_address_type int_polarity_low;      // 0x013
    pm_register_address_type int_latched_clr;       // 0x014
    pm_register_address_type int_en_set;            // 0x015
    pm_register_address_type int_en_clr;            // 0x016
    pm_register_address_type int_latched_sts;       // 0x018
    pm_register_address_type int_pending_sts;       // 0x019
    pm_register_address_type int_mid_sel;           // 0x01a
    pm_register_address_type int_priority;          // 0x01b
    pm_register_address_type usb_ovp_ctl;           // 0x042
    pm_register_address_type usb_susp;              // 0x047
    pm_register_address_type enum_timer_stop;       // 0x04e
    pm_register_address_type enum_timer;            // 0x04f
    pm_register_address_type sec_access;            // 0x0d0
    pm_register_address_type perph_reset_ctl2;      // 0x0d9
    pm_register_address_type perph_reset_ctl3;      // 0x0da
    pm_register_address_type perph_reset_ctl4;      // 0x0db
    pm_register_address_type perph_reset_ctl5;      // 0x0dc
    pm_register_address_type perph_reset_ctl6;      // 0x0dd
    pm_register_address_type usb_spare;             // 0x0df
    pm_register_address_type int_test1;             // 0x0e0
    pm_register_address_type int_test_val;          // 0x0e1
    pm_register_address_type usb_ovp_tst0;          // 0x0e2
    pm_register_address_type usb_ovp_tst1;          // 0x0e3
    pm_register_address_type usb_ovp_tst2;          // 0x0e4
    pm_register_address_type usb_ovp_tst3;          // 0x0e5
    pm_register_address_type usb_ovp_tst4;          // 0x0e6
    pm_register_address_type usb_ovp_tst5;          // 0x0e7
    pm_register_address_type comp_ovr0;             // 0x0e9
    pm_register_address_type comp_ovr1;             // 0x0ea
    pm_register_address_type usb_comp_ovr_global;   // 0x0eb
    pm_register_address_type enum_timer_en;         // 0x0ec
}lbc_usb_chgpth_register_ds;

typedef struct lbc_register_ds  //dummy, to be replaced
{
    lbc_chgr_register_ds          *chgr_register;     
    lbc_bat_if_register_ds        *bat_if_register;
    lbc_usb_chgpth_register_ds    *usb_chgpth_register;
    lbc_misc_register_ds          *misc_register;     
}lbc_register_ds;


typedef struct pm_lbc_data_type
{
    pmiC_PmicResource super; // Inheritance from ILBCstructure. This must be the first item in structure

    /********************************************
    DATA MEMBER DECLARATION
    *********************************************/
    lbc_register_ds * lbc_register;
    uint8 num_of_peripherals;                
    
    // Function Pointers for Virtual Methods
    pm_ilbc_set_mode_fp                                ilbc_set_mode;
    pm_ilbc_get_battery_present_fp                     ilbc_get_battery_present;
    pm_ilbc_enable_wdog_fp                             ilbc_enable_wdog;
    pm_ilbc_set_wdog_fp                                ilbc_set_wdog;
    pm_ilbc_pet_chg_wdog_fp                            ilbc_pet_chg_wdog;
    pm_ilbc_enable_ttrkl_fp                            ilbc_enable_ttrkl;
    pm_ilbc_set_ttrkl_fp                               ilbc_set_ttrkl;
    pm_ilbc_set_vtrkl_fp                               ilbc_set_vtrkl;
    pm_ilbc_set_itrickle                               ilbc_set_itrickle;
    pm_ilbc_set_itrickle_switch_mode_fp                ilbc_set_itrickle_switch_mode;
    pm_ilbc_set_vdd_safe_fp                            ilbc_set_vdd_safe;
    pm_ilbc_set_vdd_max_fp                             ilbc_set_vdd_max;
    pm_ilbc_set_vin_min_fp                             ilbc_set_vin_min;
    pm_ilbc_set_temp_threshold_fp                      ilbc_set_temp_threshold;
    pm_ilbc_set_ibat_max_fp                            ilbc_set_ibat_max;
    pm_ilbc_set_ibat_safe_fp                           ilbc_set_ibat_safe;
    pm_ilbc_set_vbat_det_fp                            ilbc_set_vbat_det;
    pm_ilbc_set_vbat_weak_thr_fp                       ilbc_set_vbat_weak_thr;
    pm_ilbc_get_vbat_weak_thr_fp                       ilbc_get_vbat_weak_thr;
    pm_ilbc_get_battery_weak_fp                        ilbc_get_battery_weak;
    pm_ilbc_get_vbat_status_fp                         ilbc_get_vbat_status;
    pm_ilbc_get_attached_charger_fp                    ilbc_get_attached_charger;
    pm_ilbc_set_tchg_fp                                ilbc_set_tchg;
    pm_ilbc_enable_tchg_fp                             ilbc_enable_tchg;
    pm_ilbc_set_iterm_chgr_fp                          ilbc_set_iterm_chgr;
    pm_ilbc_get_usb_ovp_ctl_fp                         ilbc_get_usb_ovp_ctl;
    pm_ilbc_set_usb_ovp_ctl_fp                         ilbc_set_usb_ovp_ctl;
    pm_ilbc_enable_low_power_mode_fp                   ilbc_enable_low_power_mode;
    pm_ilbc_enable_boot_timer_fp                       ilbc_enable_boot_timer;
    pm_ilbc_set_adaptive_boot_type_fp                  ilbc_set_adaptive_boot_type;
    pm_ilbc_set_boot_timer_fp                          ilbc_set_boot_timer;
    pm_ilbc_get_vbat_boot_thres_fp                     ilbc_get_vbat_boot_thres;
    pm_ilbc_get_batt_therm_presence_fp                 ilbc_get_batt_therm_presence;
    pm_ilbc_get_batt_id_presence_fp                    ilbc_get_batt_id_presence;
    pm_ilbc_get_batt_temp_status_fp                    ilbc_get_batt_temp_status;
    pm_ilbc_is_vref_bat_thm_on_fp                      ilbc_is_vref_bat_thm_on;
    pm_ilbc_is_vref_bat_thm_source_avdd_fp             ilbc_is_vref_bat_thm_source_avdd;
    pm_ilbc_is_vref_bat_thm_source_vreg_xoadc_fp       ilbc_is_vref_bat_thm_source_vreg_xoadc;
    pm_ilbc_enable_bpd_bat_thm_fp                      ilbc_enable_bpd_bat_thm;
    pm_ilbc_enable_bpd_bat_id_fp                       ilbc_enable_bpd_bat_id;
    pm_ilbc_set_bat_temp_comp_fp                       ilbc_set_bat_temp_comp;
    pm_ilbc_set_vref_bat_thm_ctrl_fp                   ilbc_set_vref_bat_thm_ctrl;
    pm_ilbc_get_vref_bat_thm_ctrl_fp                   ilbc_get_vref_bat_thm_ctrl;
    pm_ilbc_enable_vbat_dischg_fp                      ilbc_enable_vbat_dischg;
    pm_ilbc_is_battery_removed_fp                      ilbc_is_battery_removed;
    pm_ilbc_is_batt_trckl_charged_fp                   ilbc_is_batt_trckl_charged;
    pm_ilbc_irq_enable_fp                              ilbc_irq_enable;
    pm_ilbc_irq_clear_fp                               ilbc_irq_clear;
    pm_ilbc_irq_set_trigger_fp                         ilbc_irq_set_trigger;
    pm_ilbc_irq_status_fp                              ilbc_irq_status;

}pm_lbc_data_type;
/*************************************
NOTE: Initializer IMPLEMENTATION
**************************************/
pmiC_PmicResource* pm_lbc_driver_init(pmiC_PmicDevice *device);

#endif //PM_LBC_DRIVER__H
