#ifndef __PM_SCHG_DRIVER_H__
#define __PM_SCHG_DRIVER_H__

/*! \file pm_schg_driver.h
 *  \n
 *  \brief This file contains functions prototypes and variable/type/constant
*          declarations for supporting SMBB peripheral  
 *  \n  
 *  \n &copy;
 *  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/17/17   pxm     Added sysok_reason_status in schg_misc_register_ds struct
02/08/17   ll      Added otg_eng_otg_cfg in schg_otg_register_ds and  
                    eng_ssupply_cfg2 in schg_dc_register_ds
02/03/17   sm      Added type_c_cfg_3 in schg_usb_register_ds struct
12/06/16   sm      Added eng_ssupply_cfg3 in schg_dc_register_ds struct
12/19/16   mr     Added API to get SCHG Charger PmicIndex
11/14/16   sm      Added eng_buckboost_cfg1 in schg_misc_register_ds struct
09/07/16   sm      Added pm_schg_get_dig_major API to return digital major 
01/13/16   al      Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/


/************************************************************************/
/* register definitions                                                                     */
/************************************************************************/
typedef struct
{
  uint32                   base_address;                  //0x1200
  pm_register_address_type perph_type                   ; //0x004
  pm_register_address_type perph_subtype                ; //0x005
  pm_register_address_type sec_access                   ; //0x0D0
  pm_register_address_type spare                        ; //0x0DF
  pm_register_address_type int_rt_sts                   ; //0x010
  pm_register_address_type int_set_type                 ; //0x011
  pm_register_address_type int_polarity_high            ; //0x012
  pm_register_address_type int_polarity_low             ; //0x013
  pm_register_address_type int_latched_clr              ; //0x014
  pm_register_address_type int_en_set                   ; //0x015
  pm_register_address_type int_en_clr                   ; //0x016
  pm_register_address_type int_latched_sts              ; //0x018
  pm_register_address_type int_pending_sts              ; //0x019
  pm_register_address_type ship_mode                    ; //0x040
  pm_register_address_type batocp_threshold_cfg         ; //0x050
  pm_register_address_type batocp_intrpt_delay_tmr_cfg  ; //0x051
  pm_register_address_type batocp_reset_tmr_cfg         ; //0x052
  pm_register_address_type low_batt_detect_en_cfg       ; //0x060
  pm_register_address_type low_batt_threshold_cfg       ; //0x061
  pm_register_address_type bat_fet_cfg                  ; //0x062
  pm_register_address_type bat_miss_src_cfg             ; //0x070
  pm_register_address_type bat_miss_alg_options_cfg     ; //0x071
  pm_register_address_type bat_miss_pin_gf_cfg          ; //0x072
}schg_batif_register_ds;

typedef struct schg_otg_register_ds
{
  pm_register_address_type   base_address;             //0x1100
  pm_register_address_type perph_type                 ; //0x004
  pm_register_address_type perph_subtype              ; //0x005
  pm_register_address_type sec_access                 ; //0x0D0
  pm_register_address_type spare                      ; //0x0DF
  pm_register_address_type rid_cc_control_23_16       ; //0x006
  pm_register_address_type rid_cc_control_15_8        ; //0x007
  pm_register_address_type rid_cc_control_7_0         ; //0x008
  pm_register_address_type otg_status                 ; //0x009
  pm_register_address_type int_rt_sts                 ; //0x010
  pm_register_address_type int_set_type               ; //0x011
  pm_register_address_type int_polarity_high          ; //0x012
  pm_register_address_type int_polarity_low           ; //0x013
  pm_register_address_type int_latched_clr            ; //0x014
  pm_register_address_type int_en_set                 ; //0x015
  pm_register_address_type int_en_clr                 ; //0x016
  pm_register_address_type int_latched_sts            ; //0x018
  pm_register_address_type int_pending_sts            ; //0x019
  pm_register_address_type int_mid_sel                ; //0x01A
  pm_register_address_type int_priority               ; //0x01B
  pm_register_address_type cmd_otg                    ; //0x040
  pm_register_address_type bat_uvlo_threshold_cfg     ; //0x051
  pm_register_address_type otg_current_limit_cfg      ; //0x052
  pm_register_address_type otg_cfg                    ; //0x053
  pm_register_address_type otg_eng_otg_cfg            ; //0x0C0
}schg_otg_register_ds;

typedef struct
{
  uint32                   base_address;                                         //0x1000
  pm_register_address_type perph_type                                          ; //0x004
  pm_register_address_type perph_subtype                                       ; //0x005
  pm_register_address_type sec_access                                          ; //0x0D0
  pm_register_address_type spare                                               ; //0x0DF
  pm_register_address_type battery_charger_status_1                            ; //0x006
  pm_register_address_type battery_charger_status_2                            ; //0x007
  pm_register_address_type battery_charger_status_3                            ; //0x009
  pm_register_address_type battery_charger_status_4                            ; //0x00A
  pm_register_address_type battery_charger_status_5                            ; //0x00B
  pm_register_address_type battery_charger_status_6                            ; //0x00C
  pm_register_address_type battery_charger_status_7                            ; //0x00D
  pm_register_address_type battery_charger_status_8                            ; //0x00E
  pm_register_address_type int_rt_sts                                          ; //0x010
  pm_register_address_type int_set_type                                        ; //0x011
  pm_register_address_type int_polarity_high                                   ; //0x012
  pm_register_address_type int_polarity_low                                    ; //0x013
  pm_register_address_type int_latched_clr                                     ; //0x014
  pm_register_address_type int_en_set                                          ; //0x015
  pm_register_address_type int_en_clr                                          ; //0x016
  pm_register_address_type int_latched_sts                                     ; //0x018
  pm_register_address_type int_pending_sts                                     ; //0x019
  pm_register_address_type chg_option                                          ; //0x008
  pm_register_address_type step_chg_soc_vbatt_v                                ; //0x040
  pm_register_address_type step_chg_soc_vbatt_v_update                         ; //0x041
  pm_register_address_type charging_enable_cmd                                 ; //0x042
  pm_register_address_type allow_fast_charging_cmd                             ; //0x043
  pm_register_address_type qnovo_pt_enable_cmd                                 ; //0x044
  pm_register_address_type chgr_cfg1                                           ; //0x050
  pm_register_address_type chgr_cfg2                                           ; //0x051
  pm_register_address_type charger_enable_cfg                                  ; //0x052
  pm_register_address_type cfg                                                 ; //0x053
  pm_register_address_type charger_spare                                       ; //0x054
  pm_register_address_type pre_charge_current_cfg                              ; //0x060
  pm_register_address_type fast_charge_current_cfg                             ; //0x061
  pm_register_address_type charge_current_termination_cfg                      ; //0x062
  pm_register_address_type tccc_charge_current_termination_cfg                 ; //0x063
  pm_register_address_type charge_current_softstart_setting_cfg                ; //0x064
  pm_register_address_type float_voltage_cfg                                   ; //0x070
  pm_register_address_type auto_float_voltage_compensation_cfg                 ; //0x071
  pm_register_address_type charge_inhibit_threshold_cfg                        ; //0x072
  pm_register_address_type recharge_threshold_cfg                              ; //0x073
  pm_register_address_type pre_to_fast_charge_threshold_cfg                    ; //0x074
  pm_register_address_type fv_hysteresis_cfg                                   ; //0x075
  pm_register_address_type fvc_charge_inhibit_threshold_cfg                    ; //0x080
  pm_register_address_type fvc_recharge_threshold_cfg                          ; //0x081
  pm_register_address_type fvc_pre_to_fast_charge_threshold_cfg                ; //0x082
  pm_register_address_type fvc_full_on_threshold_cfg                           ; //0x083
  pm_register_address_type fvc_cc_mode_glitch_filter_sel_cfg                   ; //0x084
  pm_register_address_type fvc_termination_glitch_filter_sel_cfg               ; //0x085
  pm_register_address_type jeita_en_cfg                                        ; //0x090
  pm_register_address_type jeita_fvcomp_cfg                                    ; //0x091
  pm_register_address_type jeita_cccomp_cfg                                    ; //0x092
  pm_register_address_type safety_timer_enable_cfg                             ; //0x0A0
  pm_register_address_type pre_charge_safety_timer_cfg                         ; //0x0A1
  pm_register_address_type fast_charge_safety_timer_cfg                        ; //0x0A2
  pm_register_address_type step_chg_mode_cfg                                   ; //0x0B0
  pm_register_address_type step_chg_soc_or_batt_v_update_request_timeout_cfg   ; //0x0B1
  pm_register_address_type step_chg_soc_or_batt_v_update_fail_timeout_cfg      ; //0x0B2
  pm_register_address_type step_chg_soc_or_batt_v_threshold1                   ; //0x0B3
  pm_register_address_type step_chg_soc_or_batt_v_threshold2                   ; //0x0B4
  pm_register_address_type step_chg_soc_or_batt_v_threshold3                   ; //0x0B5
  pm_register_address_type step_chg_soc_or_batt_v_threshold4                   ; //0x0B6
  pm_register_address_type step_chg_current_delta1                             ; //0x0B7
  pm_register_address_type step_chg_current_delta2                             ; //0x0B8
  pm_register_address_type step_chg_current_delta3                             ; //0x0B9
  pm_register_address_type step_chg_current_delta4                             ; //0x0BA
  pm_register_address_type step_chg_current_delta5                             ; //0x0BB
  pm_register_address_type step_chg_fv_delta1                                  ; //0x0BC
  pm_register_address_type step_chg_fv_delta2                                  ; //0x0BD
  pm_register_address_type step_chg_fv_delta3                                  ; //0x0BE
  pm_register_address_type step_chg_fv_delta4                                  ; //0x0BF
  pm_register_address_type fv_cal_cfg                                          ; //0x076
  pm_register_address_type fv_adjust                                           ; //0x077
  pm_register_address_type fg_vadc_disq_thresh                                 ; //0x078
  pm_register_address_type fg_iadc_disq_thresh                                 ; //0x079
  pm_register_address_type fg_update_cfg_1                                     ; //0x07A
  pm_register_address_type fg_update_cfg_1_sel                                 ; //0x07B
  pm_register_address_type fg_update_cfg_2                                     ; //0x07C
  pm_register_address_type fg_update_cfg_2_sel                                 ; //0x07D
  pm_register_address_type fg_chg_interface_cfg                                ; //0x07E
  pm_register_address_type fg_chg_interface_cfg_sel                            ; //0x07F
}schg_chgr_register_ds;

typedef struct
{
  uint32                   base_address;                     //0x1400
  pm_register_address_type perph_type                      ; //0x004
  pm_register_address_type perph_subtype                   ; //0x005
  pm_register_address_type sec_access                      ; //0x0D0
  pm_register_address_type dcin_input_status               ; //0x006
  pm_register_address_type wipwr_status                    ; //0x007
  pm_register_address_type wipwr_range_status              ; //0x008
  pm_register_address_type int_rt_sts                      ; //0x010
  pm_register_address_type int_set_type                    ; //0x011
  pm_register_address_type int_polarity_high               ; //0x012
  pm_register_address_type int_polarity_low                ; //0x013
  pm_register_address_type int_latched_clr                 ; //0x014
  pm_register_address_type int_en_set                      ; //0x015
  pm_register_address_type int_en_clr                      ; //0x016
  pm_register_address_type int_latched_sts                 ; //0x018
  pm_register_address_type int_pending_sts                 ; //0x019
  pm_register_address_type cmd_il                          ; //0x040
  pm_register_address_type spare                           ; //0x0DF
  pm_register_address_type dcin_adapter_allow_cfg          ; //0x060
  pm_register_address_type dcin_load_cfg                   ; //0x065
  pm_register_address_type dcin_current_limit_cfg          ; //0x070
  pm_register_address_type dcin_aicl_options_cfg           ; //0x080
  pm_register_address_type dcin_aicl_ref_sel_cfg           ; //0x081
  pm_register_address_type dcin_icl_start_cfg              ; //0x082
  pm_register_address_type div2_en_gf_time_cfg             ; //0x090
  pm_register_address_type wipwr_irq_tmr_cfg               ; //0x091
  pm_register_address_type zin_icl_pt                      ; //0x092
  pm_register_address_type zin_icl_lv                      ; //0x093
  pm_register_address_type zin_icl_hv                      ; //0x094
  pm_register_address_type wi_pwr_options                  ; //0x095
  pm_register_address_type zin_icl_pt_hv                   ; //0x096
  pm_register_address_type zin_icl_mid_lv                  ; //0x097
  pm_register_address_type zin_icl_mid_hv                  ; //0x098
  pm_register_address_type eng_ssupply_cfg2                ; //0x0C1
  pm_register_address_type eng_ssupply_cfg3                ; //0x0C2
}schg_dc_register_ds;


typedef struct
{
    uint32                   base_address;                   // 0x1600
  pm_register_address_type perph_type                       ; //0x004
  pm_register_address_type perph_subtype                    ; //0x005
  pm_register_address_type sec_access                       ; //0x0D0
  pm_register_address_type spare                            ; //0x0DF
  pm_register_address_type temp_range_status                ; //0x006
  pm_register_address_type icl_status                       ; //0x007
  pm_register_address_type adapter_5v_icl_status            ; //0x008
  pm_register_address_type adapter_9v_icl_status            ; //0x009
  pm_register_address_type aicl_status                      ; //0x00A
  pm_register_address_type power_path_status                ; //0x00B
  pm_register_address_type wdog_status                      ; //0x00C
  pm_register_address_type sysok_reason_status              ; //0x00D
  pm_register_address_type int_rt_sts                       ; //0x010
  pm_register_address_type int_set_type                     ; //0x011
  pm_register_address_type int_polarity_high                ; //0x012
  pm_register_address_type int_polarity_low                 ; //0x013
  pm_register_address_type int_latched_clr                  ; //0x014
  pm_register_address_type int_en_set                       ; //0x015
  pm_register_address_type int_en_clr                       ; //0x016
  pm_register_address_type int_latched_sts                  ; //0x018
  pm_register_address_type int_pending_sts                  ; //0x019
  pm_register_address_type wdog_rst                         ; //0x040
  pm_register_address_type afp_mode                         ; //0x041
  pm_register_address_type gsm_pa_on_adj_en                 ; //0x042
  pm_register_address_type bark_bite_wdog_pet               ; //0x043
  pm_register_address_type phyon_cmd                        ; //0x044
  pm_register_address_type shdn_cmd                         ; //0x045
  pm_register_address_type finish_copy_command              ; //0x04F
  pm_register_address_type wd_cfg                           ; //0x051
  pm_register_address_type misc_cfg                         ; //0x052
  pm_register_address_type snarl_bark_bite_wd_cfg           ; //0x053
  pm_register_address_type phyon_cfg                        ; //0x054
  pm_register_address_type chgr_trim_options_7_0            ; //0x055
  pm_register_address_type ch_ov_option_cfg                 ; //0x056
  pm_register_address_type aicl_cfg                         ; //0x060
  pm_register_address_type aicl_rerun_time_cfg              ; //0x061
  pm_register_address_type aicl_rerun_temp_time_cfg         ; //0x062
  pm_register_address_type thermreg_src_cfg                 ; //0x070
  pm_register_address_type treg_die_cmp_inc_cycle_time_cfg  ; //0x071
  pm_register_address_type treg_die_cmp_dec_cycle_time_cfg  ; //0x072
  pm_register_address_type treg_die_adc_inc_cycle_time_cfg  ; //0x073
  pm_register_address_type treg_die_adc_dec_cycle_time_cfg  ; //0x074
  pm_register_address_type treg_skin_adc_inc_cycle_time_cfg ; //0x075
  pm_register_address_type treg_skin_adc_dec_cycle_time_cfg ; //0x076
  pm_register_address_type buck_options_cfg                 ; //0x080
  pm_register_address_type icl_softstart_rate_cfg           ; //0x081
  pm_register_address_type icl_softstop_rate_cfg            ; //0x082
  pm_register_address_type vsys_min_sel_cfg                 ; //0x083
  pm_register_address_type tracking_voltage_sel_cfg         ; //0x084
  pm_register_address_type stat_cfg                         ; //0x090
  pm_register_address_type lbc_en_cfg                       ; //0x091
  pm_register_address_type lbc_period_cfg                   ; //0x092
  pm_register_address_type lbc_duty_cycle_cfg               ; //0x093
  pm_register_address_type sysok_cfg                        ; //0x094
  pm_register_address_type eng_buckboost_cfg1               ; //0x0C0
}schg_misc_register_ds;


typedef struct
{
    uint32                   base_address;                       //0x1300
    pm_register_address_type perph_type                      ;   //0x004
  pm_register_address_type perph_subtype                       ; //0x005
  pm_register_address_type sec_access                          ; //0x0D0
  pm_register_address_type spare                               ; //0x0DF
  pm_register_address_type usbin_input_status                  ; //0x006
  pm_register_address_type apsd_status                         ; //0x007
  pm_register_address_type apsd_result_status                  ; //0x008
  pm_register_address_type qc_change_status                    ; //0x009
  pm_register_address_type qc_pulse_count_status               ; //0x00A
  pm_register_address_type type_c_status_1                     ; //0x00B
  pm_register_address_type type_c_status_2                     ; //0x00C
  pm_register_address_type type_c_status_3                     ; //0x00D
  pm_register_address_type type_c_status_4                     ; //0x00E
  pm_register_address_type int_rt_sts                          ; //0x010
  pm_register_address_type int_set_type                        ; //0x011
  pm_register_address_type int_polarity_high                   ; //0x012
  pm_register_address_type int_polarity_low                    ; //0x013
  pm_register_address_type int_latched_clr                     ; //0x014
  pm_register_address_type int_en_set                          ; //0x015
  pm_register_address_type int_en_clr                          ; //0x016
  pm_register_address_type int_latched_sts                     ; //0x018
  pm_register_address_type int_pending_sts                     ; //0x019
  pm_register_address_type cmd_il                              ; //0x040
  pm_register_address_type cmd_apsd                            ; //0x041
  pm_register_address_type cmd_hvdcp_2                         ; //0x043
  pm_register_address_type usb_misc2                           ; //0x057
  pm_register_address_type type_c_cfg                          ; //0x058
  pm_register_address_type type_c_cfg_2                        ; //0x059
  pm_register_address_type type_c_cfg_3                        ; //0x05A
  pm_register_address_type usbin_adapter_allow_cfg             ; //0x060
  pm_register_address_type usbin_options_1_cfg                 ; //0x062
  pm_register_address_type usbin_options_2_cfg                 ; //0x063
  pm_register_address_type taper_timer_sel_cfg                 ; //0x064
  pm_register_address_type usbin_load_cfg                      ; //0x065
  pm_register_address_type usbin_icl_options                   ; //0x066
  pm_register_address_type type_c_intrpt_enb                   ; //0x067
  pm_register_address_type type_c_intrpt_enb_software_ctrl     ; //0x068
  pm_register_address_type usbin_source_change_intrpt_enb      ; //0x069
  pm_register_address_type usbin_current_limit_cfg             ; //0x070
  pm_register_address_type usbin_aicl_options_cfg              ; //0x080
  pm_register_address_type usbin_5v_aicl_threshold_cfg         ; //0x081
  pm_register_address_type usbin_9v_aicl_threshold_cfg         ; //0x082
  pm_register_address_type usbin_12v_aicl_threshold_cfg        ; //0x083
  pm_register_address_type usbin_cont_aicl_threshold_cfg       ; //0x084
}schg_usb_register_ds;

typedef struct
{
    uint32                   base_address;           //0x1b00

    pm_register_address_type perph_subtype;         //0x1b05
    pm_register_address_type bsi_sts;               //0x1b08
    pm_register_address_type mipi_bif_fsm_sts;      //0x1b09
    pm_register_address_type rx_fifo_sts;           //0x1b0a
    pm_register_address_type bsi_bat_sts;           //0x1b0d
    pm_register_address_type int_rt_sts;            //0x1b10
    pm_register_address_type int_set_type;          //0x1b11
    pm_register_address_type int_polarity_high;     //0x1b12
    pm_register_address_type int_polarity_low;      //0x1b13
    pm_register_address_type int_latched_clr;       //0x1b14
    pm_register_address_type int_en_set;            //0x1b15
    pm_register_address_type int_en_clr;            //0x1b16
    pm_register_address_type int_latched_sts;       //0x1b18
    pm_register_address_type int_pending_sts;       //0x1b19
    pm_register_address_type int_mid_sel;           //0x1b1a
    pm_register_address_type int_priority;          //0x1b1b
    pm_register_address_type bsi_en;                //0x1b46
    pm_register_address_type mipi_bif_err_clear;    //0x1b4f
    pm_register_address_type mipi_bif_fsm_reset;    //0x1b50
    pm_register_address_type mipi_bif_force_bcl_low; //0x1b51
    pm_register_address_type mipi_bif_tau_cfg;      //0x1b52
    pm_register_address_type mipi_bif_mode;         //0x1b53
    pm_register_address_type mipi_bif_en;           //0x1b54
    pm_register_address_type mipi_bif_cfg;          //0x1b55
    pm_register_address_type mipi_bif_rx_cfg;       //0x1b56
    pm_register_address_type mipi_bif_tx_dly;       //0x1b59
    pm_register_address_type mipi_bif_data_tx_0;    //0x1b5a
    pm_register_address_type mipi_bif_data_tx_1;    //0x1b5b
    pm_register_address_type mipi_bif_data_tx_2;    //0x1b5c
    pm_register_address_type mipi_bif_tx_ctl;       //0x1b5d
    pm_register_address_type mipi_bif_data_rx_0;    //0x1b60
    pm_register_address_type mipi_bif_data_rx_1;    //0x1b61
    pm_register_address_type mipi_bif_data_rx_2;    //0x1b62
    pm_register_address_type rx_fifo_word1_0;       //0x1b63
    pm_register_address_type rx_fifo_word1_1;       //0x1b64
    pm_register_address_type rx_fifo_word2_0;       //0x1b65
    pm_register_address_type rx_fifo_word2_1;       //0x1b66
    pm_register_address_type rx_fifo_word3_0;       //0x1b67
    pm_register_address_type rx_fifo_word3_1;       //0x1b68
    pm_register_address_type rx_fifo_word4_0;       //0x1b69
    pm_register_address_type rx_fifo_word4_1;       //0x1b6a
    pm_register_address_type mipi_bif_bcl_raw;      //0x1b6d
    pm_register_address_type mipi_bif_error;        //0x1b70
    pm_register_address_type bat_gone_cfg;          //0x1ba7
    pm_register_address_type bat_rmv_deb_timer;     //0x1ba8
    pm_register_address_type bat_pres_deb_timer;    //0x1ba9
}schg_bsi_register_ds;

typedef struct
{
    schg_bsi_register_ds          *bsi_register;
    schg_chgr_register_ds         *chgr_register;
    schg_otg_register_ds          *otg_register;
    schg_batif_register_ds        *batif_register;
    schg_usb_register_ds          *usb_register;
    schg_dc_register_ds           *dc_register;
    schg_misc_register_ds         *misc_register;
}schg_register_ds;


typedef struct
{
    pm_comm_info_type          *comm_ptr;
    schg_register_ds           *schg_register;
    uint8                      num_of_peripherals;
}pm_schg_data_type;


/*===========================================================================

                     FUNCTION DECLARATION

===========================================================================*/
void pm_schg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_schg_data_type* pm_schg_get_data(uint8 pmic_index);

uint8 pm_schg_get_dig_major(void);

void pm_schg_get_pmic_info(uint8 *pmic_index, uint8 *slave_id);

#endif // __PM_SCHG_DRIVER_H__
